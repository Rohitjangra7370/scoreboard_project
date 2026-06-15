/*
 * Smart A4 Scoreboard
 * ----------------------------------------------------------------------------
 * Two-player game scoreboard on an ESP32-WROOM-32.
 *
 *  - 4x 2.4" ILI9341 TFT displays on a shared VSPI bus (separate CS each):
 *        [A initials][A score]  ||LED column||  [B score][B initials]
 *  - 1x WS2812B 8-LED stick      -> games-won tracker (centre column)
 *  - 2x arcade buttons (red/green, hardware-debounced interrupts) -> +1 score
 *  - Scores + games-won persist in NVS flash (survive power-off / dead battery)
 *
 * Libraries (install via Library Manager):
 *   - Adafruit GFX Library
 *   - Adafruit ILI9341
 *   - Adafruit NeoPixel
 *
 * Author: Rohit Jangra
 * ----------------------------------------------------------------------------
 */

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>

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

#define BTN_RED      32      // Player A +1   (INPUT_PULLUP -> GND)
#define BTN_GREEN    33      // Player B +1   (INPUT_PULLUP -> GND)

#define NEOPIXEL_PIN 25      // WS2812B DIN
#define NUM_LEDS      8      // 8-LED stick

// ----------------------------------------------------------------------------
// Game configuration
// ----------------------------------------------------------------------------
#define WIN_SCORE        11   // first to this score wins the round
#define WIN_BY_TWO       true // must win by 2 (table-tennis style)
#define MAX_GAMES        4    // games shown per player on the LED stick (4+4)
#define DEBOUNCE_MS      40   // button debounce window
#define RESET_HOLD_MS    2000 // hold BOTH buttons this long to reset match

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

// Game state
volatile uint32_t lastRedIsr   = 0;
volatile uint32_t lastGreenIsr = 0;
volatile bool     redPressed   = false;
volatile bool     greenPressed = false;

int scoreA = 0, scoreB = 0;   // current round
int gamesA = 0, gamesB = 0;   // games (rounds) won
int prevScoreA = -1, prevScoreB = -1;   // for redraw-on-change

uint32_t bothHeldSince = 0;   // for reset detection

// ----------------------------------------------------------------------------
// Interrupt service routines (debounced flag-setters; logic runs in loop)
// ----------------------------------------------------------------------------
void IRAM_ATTR isrRed() {
  uint32_t now = millis();
  if (now - lastRedIsr > DEBOUNCE_MS) { redPressed = true; lastRedIsr = now; }
}
void IRAM_ATTR isrGreen() {
  uint32_t now = millis();
  if (now - lastGreenIsr > DEBOUNCE_MS) { greenPressed = true; lastGreenIsr = now; }
}

// ----------------------------------------------------------------------------
// Persistence
// ----------------------------------------------------------------------------
void saveState() {
  prefs.putInt("scoreA", scoreA);
  prefs.putInt("scoreB", scoreB);
  prefs.putInt("gamesA", gamesA);
  prefs.putInt("gamesB", gamesB);
}

void loadState() {
  scoreA = prefs.getInt("scoreA", 0);
  scoreB = prefs.getInt("scoreB", 0);
  gamesA = prefs.getInt("gamesA", 0);
  gamesB = prefs.getInt("gamesB", 0);
}

// ----------------------------------------------------------------------------
// Display helpers
// ----------------------------------------------------------------------------
// Draw a player's initials on its display (called once / on reset).
void drawInitials(Adafruit_ILI9341 &tft, const char* text, uint16_t colour) {
  tft.fillScreen(COL_BLACK);
  tft.setTextColor(colour);
  tft.setTextSize(6);
  // rough centre for a 240x320 panel in landscape (320x240)
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((320 - w) / 2, (240 - h) / 2);
  tft.print(text);
}

// Draw a big centred score number.
void drawScore(Adafruit_ILI9341 &tft, int value, uint16_t colour) {
  tft.fillScreen(COL_BLACK);
  tft.setTextColor(colour);
  tft.setTextSize(12);
  char buf[6];
  snprintf(buf, sizeof(buf), "%d", value);
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((320 - w) / 2, (240 - h) / 2);
  tft.print(buf);
}

// Light the NeoPixel stick: LEDs 0..3 = Player A games (red),
// LEDs 4..7 = Player B games (green).
void updateLeds() {
  strip.clear();
  for (int i = 0; i < gamesA && i < MAX_GAMES; i++)
    strip.setPixelColor(i, strip.Color(60, 0, 0));
  for (int i = 0; i < gamesB && i < MAX_GAMES; i++)
    strip.setPixelColor(NUM_LEDS - 1 - i, strip.Color(0, 60, 0));
  strip.show();
}

void refreshScores() {
  if (scoreA != prevScoreA) { drawScore(tftAScore, scoreA, COL_RED);   prevScoreA = scoreA; }
  if (scoreB != prevScoreB) { drawScore(tftBScore, scoreB, COL_GREEN); prevScoreB = scoreB; }
}

// ----------------------------------------------------------------------------
// Game logic
// ----------------------------------------------------------------------------
bool isRoundWon(int me, int them) {
  if (me < WIN_SCORE) return false;
  if (WIN_BY_TWO) return (me - them) >= 2;
  return true;
}

void winFlash(uint16_t colour) {
  for (int k = 0; k < 3; k++) {
    strip.fill(strip.Color((colour == COL_RED) ? 80 : 0,
                            (colour == COL_GREEN) ? 80 : 0, 0));
    strip.show(); delay(120);
    strip.clear(); strip.show(); delay(120);
  }
}

void addPoint(bool playerA) {
  if (playerA) scoreA++; else scoreB++;

  // round won?
  if (playerA && isRoundWon(scoreA, scoreB)) {
    gamesA++; winFlash(COL_RED);  scoreA = scoreB = 0;
  } else if (!playerA && isRoundWon(scoreB, scoreA)) {
    gamesB++; winFlash(COL_GREEN); scoreB = scoreA = 0;
  }

  saveState();
  refreshScores();
  updateLeds();
}

void resetMatch() {
  scoreA = scoreB = gamesA = gamesB = 0;
  saveState();
  prevScoreA = prevScoreB = -1;
  refreshScores();
  updateLeds();
}

// ----------------------------------------------------------------------------
// Setup / loop
// ----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  prefs.begin("scoreboard", false);
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

  drawInitials(tftAInit, INITIALS_A, COL_RED);
  drawInitials(tftBInit, INITIALS_B, COL_GREEN);
  refreshScores();

  strip.begin();
  strip.setBrightness(255);
  updateLeds();

  pinMode(BTN_RED,   INPUT_PULLUP);
  pinMode(BTN_GREEN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_RED),   isrRed,   FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_GREEN), isrGreen, FALLING);

  Serial.println("Scoreboard ready.");
}

void loop() {
  // handle scoring
  if (redPressed)   { redPressed = false;   addPoint(true);  }
  if (greenPressed) { greenPressed = false; addPoint(false); }

  // hold BOTH buttons -> reset match
  bool bothDown = (digitalRead(BTN_RED) == LOW) && (digitalRead(BTN_GREEN) == LOW);
  if (bothDown) {
    if (bothHeldSince == 0) bothHeldSince = millis();
    else if (millis() - bothHeldSince > RESET_HOLD_MS) {
      resetMatch();
      // wait for release so it fires once
      while (digitalRead(BTN_RED) == LOW || digitalRead(BTN_GREEN) == LOW) delay(10);
      redPressed = greenPressed = false;
      bothHeldSince = 0;
    }
  } else {
    bothHeldSince = 0;
  }
}
