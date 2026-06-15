/*
 * Smart A4 Scoreboard
 * ----------------------------------------------------------------------------
 * Two-player game scoreboard on an ESP32-WROOM-32.
 *
 *  - 4x 2.4" ILI9341 TFT displays on a shared VSPI bus (separate CS each):
 *        [A initials][A score]  ||LED column||  [B score][B initials]
 *  - 1x WS2812B 8-LED stick      -> games-won tracker (centre column)
 *  - 2x arcade buttons (red/green, hardware-debounced interrupts)
 *  - Scores + games-won + selected game persist in NVS flash (survive power-off)
 *
 * Controls (gesture-based, only two buttons):
 *   Red short    = +1 Player A          Green short = +1 Player B
 *   Red long     = UNDO one step        Green long  = RESET match
 *   Hold both 2s = open / close the game MENU
 *   In MENU: Red = next game (preview), Green = select (resets scores)
 *
 * Scoring rules live in the hardware-independent core (game_logic.h/.cpp),
 * unit-tested on the host.
 *
 * Libraries: Adafruit GFX, Adafruit ILI9341, Adafruit NeoPixel.
 * Author: Rohit Jangra
 * ----------------------------------------------------------------------------
 */

#include <SPI.h>
#include <stdio.h>
#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>
#include "game_logic.h"
#include "reaction.h"

// ----------------------------------------------------------------------------
// Pin map  (see wiring_table.md)
// ----------------------------------------------------------------------------
#define TFT_SCLK     18      // shared VSPI clock
#define TFT_MOSI     23      // shared VSPI data
#define TFT_DC       21      // shared data/command
#define TFT_RST      22      // shared reset

#define CS_A_INIT     5      // display 1 - Player A initials
#define CS_A_SCORE   13      // display 2 - Player A score
#define CS_B_SCORE   14      // display 3 - Player B score
#define CS_B_INIT     4      // display 4 - Player B initials

#define BTN_RED      32      // Player A   (INPUT_PULLUP -> GND)
#define BTN_GREEN    33      // Player B   (INPUT_PULLUP -> GND)

#define NEOPIXEL_PIN 25      // WS2812B DIN
#define NUM_LEDS      8      // 8-LED stick

// ----------------------------------------------------------------------------
// Timing
// ----------------------------------------------------------------------------
#define DEBOUNCE_MS    40    // button debounce window
#define LONG_PRESS_MS  600   // >= this on release = long press
#define MENU_HOLD_MS   2000  // hold BOTH this long to toggle the menu

const char* INITIALS_A = "P1";   // red player
const char* INITIALS_B = "P2";   // green player

// Colours (565)
#define COL_RED      ILI9341_RED
#define COL_GREEN    ILI9341_GREEN
#define COL_BLACK    ILI9341_BLACK
#define COL_WHITE    ILI9341_WHITE

// ----------------------------------------------------------------------------
// Globals
// ----------------------------------------------------------------------------
// 4 display instances, all sharing the same SPI bus + DC, unique CS.
// RST is shared in hardware but pulsed ONCE manually in setup(); we pass -1 so
// each begin() does a per-CS *software* reset instead of yanking the shared RST
// line (which would wipe the displays already initialized before it).
Adafruit_ILI9341 tftAInit  = Adafruit_ILI9341(CS_A_INIT,  TFT_DC, -1);
Adafruit_ILI9341 tftAScore = Adafruit_ILI9341(CS_A_SCORE, TFT_DC, -1);
Adafruit_ILI9341 tftBScore = Adafruit_ILI9341(CS_B_SCORE, TFT_DC, -1);
Adafruit_ILI9341 tftBInit  = Adafruit_ILI9341(CS_B_INIT,  TFT_DC, -1);

Adafruit_NeoPixel strip(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Preferences prefs;

// Game state (logic lives in the core)
ScoreState gState;
History    gHist;
Mode       gMode = PLAYING;
Game       gGame = TABLE_TENNIS;
int        gCursor = 0;
ReactionState gReact;
ReactionPhase prevReactPhase = R_IDLE;
int prevReactLeds = -1;

// redraw caches (avoid flicker)
char prevAText[8] = "", prevBText[8] = "";
bool prevMenu = false;
int  prevCursor = -1;

// button edge capture
volatile uint32_t redDownAt = 0, greenDownAt = 0;
volatile bool redEdgeDown = false, greenEdgeDown = false;

// ----------------------------------------------------------------------------
// Interrupt service routines (capture press time; logic resolves on release)
// ----------------------------------------------------------------------------
void IRAM_ATTR isrRed() {
  uint32_t now = millis();
  if (now - redDownAt > DEBOUNCE_MS) { redDownAt = now; redEdgeDown = true; }
}
void IRAM_ATTR isrGreen() {
  uint32_t now = millis();
  if (now - greenDownAt > DEBOUNCE_MS) { greenDownAt = now; greenEdgeDown = true; }
}

// ----------------------------------------------------------------------------
// Persistence
// ----------------------------------------------------------------------------
void saveState() {
  prefs.putInt("scoreA", gState.scoreA);
  prefs.putInt("scoreB", gState.scoreB);
  prefs.putInt("gamesA", gState.gamesA);
  prefs.putInt("gamesB", gState.gamesB);
  prefs.putUChar("tA", gState.tA);
  prefs.putUChar("tB", gState.tB);
  prefs.putUChar("game", (gMode == REACTION) ? (uint8_t)REACTION_ITEM : (uint8_t)gGame);
}

void loadState() {
  gState.scoreA = prefs.getInt("scoreA", 0);
  gState.scoreB = prefs.getInt("scoreB", 0);
  gState.gamesA = prefs.getInt("gamesA", 0);
  gState.gamesB = prefs.getInt("gamesB", 0);
  gState.tA = prefs.getUChar("tA", 0);
  gState.tB = prefs.getUChar("tB", 0);
  uint8_t item = prefs.getUChar("game", (uint8_t)TABLE_TENNIS);
  if (item >= GAME_COUNT) { gMode = REACTION; gGame = TABLE_TENNIS; gCursor = REACTION_ITEM; }
  else                    { gMode = PLAYING;  gGame = (Game)item;   gCursor = item; }
}

// ----------------------------------------------------------------------------
// Display helpers
// ----------------------------------------------------------------------------
// Draw a player's initials on its display.
void drawInitials(Adafruit_ILI9341 &tft, const char* text, uint16_t colour) {
  tft.fillScreen(COL_BLACK);
  tft.setTextColor(colour);
  tft.setTextSize(6);
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((320 - w) / 2, (240 - h) / 2);
  tft.print(text);
}

// Draw centred text at a given size on one panel (landscape 320x240).
void drawBig(Adafruit_ILI9341 &tft, const char* text, uint16_t colour, uint8_t size) {
  tft.fillScreen(COL_BLACK);
  tft.setTextColor(colour);
  tft.setTextSize(size);
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((320 - w) / 2, (240 - h) / 2);
  tft.print(text);
}

// LEDs 0..3 = Player A games (red), 7..4 = Player B games (green).
void renderLeds() {
  strip.clear();
  for (int i = 0; i < gState.gamesA && i < 4; i++)
    strip.setPixelColor(i, strip.Color(60, 0, 0));
  for (int i = 0; i < gState.gamesB && i < 4; i++)
    strip.setPixelColor(NUM_LEDS - 1 - i, strip.Color(0, 60, 0));
  strip.show();
}

void reactionText(int ms, char* out, size_t n) {
  if (ms == -2)      snprintf(out, n, "JUMP");
  else if (ms == -1) snprintf(out, n, "--");
  else               snprintf(out, n, "%d", ms);
}

void showStartLeds(uint8_t n) {
  strip.clear();
  for (uint8_t i = 0; i < n && i < NUM_LEDS; i++)
    strip.setPixelColor(i, strip.Color(80, 0, 0));
  strip.show();
}

void showReactionTally() {
  strip.clear();
  for (int i = 0; i < gReact.winsA && i < 4; i++)
    strip.setPixelColor(i, strip.Color(60, 0, 0));
  for (int i = 0; i < gReact.winsB && i < 4; i++)
    strip.setPixelColor(NUM_LEDS - 1 - i, strip.Color(0, 60, 0));
  strip.show();
}

void renderReaction() {
  // LEDs
  if (gReact.phase == R_RESULT) {
    if (prevReactPhase != R_RESULT) showReactionTally();
  } else if (gReact.ledsLit != prevReactLeds || prevReactPhase != gReact.phase) {
    showStartLeds(gReact.ledsLit);
    prevReactLeds = gReact.ledsLit;
  }
  // screens, on phase change
  if (gReact.phase != prevReactPhase) {
    if (gReact.phase == R_COUNTDOWN || gReact.phase == R_ARMED) {
      drawBig(tftAScore, "READY", COL_WHITE, 5);
      drawBig(tftBScore, "READY", COL_WHITE, 5);
    } else if (gReact.phase == R_GO) {
      drawBig(tftAScore, "GO!", COL_GREEN, 8);
      drawBig(tftBScore, "GO!", COL_GREEN, 8);
    } else if (gReact.phase == R_RESULT) {
      char a[8], b[8];
      reactionText(gReact.reactionA, a, sizeof(a));
      reactionText(gReact.reactionB, b, sizeof(b));
      drawBig(tftAScore, a, COL_RED,   6);
      drawBig(tftBScore, b, COL_GREEN, 6);
    }
    prevReactPhase = gReact.phase;
  }
}

void render() {
  if (gMode == REACTION) { renderReaction(); return; }

  if (gMode == MENU) {
    int nextc = menu_advance(gCursor);
    if (!prevMenu || prevCursor != gCursor) {
      drawBig(tftAInit,  "MENU", COL_WHITE, 4);
      drawBig(tftAScore, menu_name(gCursor), COL_GREEN, 2);
      drawBig(tftBScore, menu_name(nextc),   COL_WHITE, 2);
      drawBig(tftBInit,  "R:next G:ok", COL_WHITE, 2);
      prevMenu = true; prevCursor = gCursor;
    }
    return;
  }

  // PLAYING
  if (prevMenu) {                       // just left the menu -> redraw initials
    drawInitials(tftAInit, INITIALS_A, COL_RED);
    drawInitials(tftBInit, INITIALS_B, COL_GREEN);
    prevAText[0] = prevBText[0] = '\0';
    prevMenu = false; prevCursor = -1;
  }
  char a[8], b[8];
  format_score(&gState, gGame, true,  a, sizeof(a));
  format_score(&gState, gGame, false, b, sizeof(b));
  if (strcmp(a, prevAText)) { drawBig(tftAScore, a, COL_RED,   10); strcpy(prevAText, a); }
  if (strcmp(b, prevBText)) { drawBig(tftBScore, b, COL_GREEN, 10); strcpy(prevBText, b); }
  renderLeds();
}

// ----------------------------------------------------------------------------
// Input handling
// ----------------------------------------------------------------------------
// Resolve a button on release: long vs short, routed by mode.
void handleRelease(int pin, volatile bool* edge, uint32_t downAt, bool isRed) {
  if (!*edge) return;
  if (digitalRead(pin) == LOW) return;     // still held; wait for release
  *edge = false;
  bool isLong = (millis() - downAt) >= LONG_PRESS_MS;

  if (gMode == PLAYING) {
    if (isRed && isLong)        { undo(&gState, &gHist); saveState(); }
    else if (!isRed && isLong)  { reset_match(&gState, &gHist); saveState(); }
    else                        { apply_point(&gState, &gHist, gGame, isRed); saveState(); }
  } else { // MENU
    if (isRed) { gCursor = menu_advance(gCursor); }
    else {                                         // green = select
      if (gCursor < GAME_COUNT) {
        gGame = (Game)gCursor;
        state_reset(&gState); history_clear(&gHist);
        gMode = PLAYING;
      } else {                                     // Reaction entry
        gMode = REACTION;
        drawInitials(tftAInit, INITIALS_A, COL_RED);
        drawInitials(tftBInit, INITIALS_B, COL_GREEN);
        prevReactPhase = R_IDLE; prevReactLeds = -1;
        reaction_start(&gReact, millis());
      }
      saveState();
    }
    render();
  }
}

// ----------------------------------------------------------------------------
// Setup / loop
// ----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  prefs.begin("scoreboard", false);
  history_clear(&gHist);
  loadState();

  // shared SPI bus
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, -1);

  // one shared hardware reset for ALL four panels, before any begin()
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, HIGH); delay(10);
  digitalWrite(TFT_RST, LOW);  delay(20);
  digitalWrite(TFT_RST, HIGH); delay(150);

  tftAInit.begin();  tftAInit.setRotation(1);
  tftAScore.begin(); tftAScore.setRotation(1);
  tftBScore.begin(); tftBScore.setRotation(1);
  tftBInit.begin();  tftBInit.setRotation(1);

  strip.begin();
  strip.setBrightness(255);

  drawInitials(tftAInit, INITIALS_A, COL_RED);
  drawInitials(tftBInit, INITIALS_B, COL_GREEN);
  prevMenu = false; prevCursor = -1;
  if (gMode == REACTION) { prevReactPhase = R_IDLE; prevReactLeds = -1; reaction_start(&gReact, millis()); }
  render();

  pinMode(BTN_RED,   INPUT_PULLUP);
  pinMode(BTN_GREEN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_RED),   isrRed,   FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_GREEN), isrGreen, FALLING);

  Serial.println("Scoreboard ready.");
}

void loop() {
  uint32_t now = millis();
  bool redDown   = digitalRead(BTN_RED)   == LOW;
  bool greenDown = digitalRead(BTN_GREEN) == LOW;

  // both-held -> toggle menu (from PLAYING or REACTION -> MENU; from MENU -> PLAYING)
  static uint32_t bothSince = 0;
  if (redDown && greenDown) {
    if (bothSince == 0) bothSince = now;
    else if (now - bothSince > MENU_HOLD_MS) {
      gMode = (gMode == MENU) ? PLAYING : MENU;
      bothSince = 0;
      while (digitalRead(BTN_RED) == LOW || digitalRead(BTN_GREEN) == LOW) delay(10);
      redEdgeDown = greenEdgeDown = false;
      render();
    }
    return;
  }
  bothSince = 0;

  if (gMode == REACTION) {
    // feed presses at press-time (timing matters), not on release
    if (redEdgeDown)   { reaction_button(&gReact, true,  redDownAt);   redEdgeDown = false; }
    if (greenEdgeDown) { reaction_button(&gReact, false, greenDownAt); greenEdgeDown = false; }
    reaction_update(&gReact, now);
    render();
    return;
  }

  handleRelease(BTN_RED,   &redEdgeDown,   redDownAt,   true);
  handleRelease(BTN_GREEN, &greenEdgeDown, greenDownAt, false);
}
