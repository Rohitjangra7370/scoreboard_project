/*
 * Smart Scoreboard — LED-array build
 * ----------------------------------------------------------------------------
 *  - 16x MAX7219 16x16 square matrices (4 per field, SCALE=2), stacked per player:
 *        initials on top, score below (A left column, B right column)
 *        -> 64x64 logical px, ~12 cm digits, readable to ~18 m
 *  - 6x discrete LEDs: 3 green (A rounds) + 3 red (B rounds), Best-of-3
 *  - 4 buttons: A-up/A-down, B-up/B-down (up=+point, down=-1 point, own player)
 *  - ESP32 SoftAP captive portal: pick game + enter names
 *  - Game/names/scores persist in NVS.
 * Author: Rohit Jangra
 * ----------------------------------------------------------------------------
 */
#include <Arduino.h>
#include <Preferences.h>
#include "game_logic.h"
#include "display_matrix.h"
#include "indicators.h"
#include "input.h"
#include "web_ui.h"

ScoreState gState;
History    gHist;
Game       gGame = TABLE_TENNIS;
char       gNameA[17] = "Player A";
char       gNameB[17] = "Player B";
Preferences prefs;

// First two non-space letters, uppercased.
static void initialsOf(const char* name, char* out2) {
  int j = 0;
  for (int i = 0; name[i] && j < 2; i++)
    if (name[i] != ' ') out2[j++] = toupper(name[i]);
  out2[j] = 0;
  if (j == 0) { out2[0] = '?'; out2[1] = 0; }
}

static void renderBoard() {
  char a[8], b[8], ia[3], ib[3];
  format_score(&gState, gGame, true,  a, sizeof(a));
  format_score(&gState, gGame, false, b, sizeof(b));
  initialsOf(gNameA, ia);
  initialsOf(gNameB, ib);
  // 4 square cells: A initials, A score, B initials, B score.
  display_show(ia, a, ib, b);
  indicators_update(gState.gamesA, gState.gamesB);
}

static void saveState() {
  prefs.putInt("scoreA", gState.scoreA);
  prefs.putInt("scoreB", gState.scoreB);
  prefs.putInt("gamesA", gState.gamesA);
  prefs.putInt("gamesB", gState.gamesB);
  prefs.putUChar("tA", gState.tA);
  prefs.putUChar("tB", gState.tB);
  prefs.putUChar("game", (uint8_t)gGame);
  prefs.putString("nameA", gNameA);
  prefs.putString("nameB", gNameB);
}

static void loadState() {
  gState.scoreA = prefs.getInt("scoreA", 0);
  gState.scoreB = prefs.getInt("scoreB", 0);
  gState.gamesA = prefs.getInt("gamesA", 0);
  gState.gamesB = prefs.getInt("gamesB", 0);
  gState.tA = prefs.getUChar("tA", 0);
  gState.tB = prefs.getUChar("tB", 0);
  uint8_t g = prefs.getUChar("game", (uint8_t)TABLE_TENNIS);
  gGame = (g < GAME_COUNT) ? (Game)g : TABLE_TENNIS;
  prefs.getString("nameA", gNameA, sizeof(gNameA));
  prefs.getString("nameB", gNameB, sizeof(gNameB));
}

// ---- web callbacks ----
static void onConfig(int gameIdx, const char* nameA, const char* nameB) {
  if (gameIdx >= 0 && gameIdx < GAME_COUNT) gGame = (Game)gameIdx;
  if (nameA[0]) strncpy(gNameA, nameA, sizeof(gNameA) - 1), gNameA[sizeof(gNameA)-1] = 0;
  if (nameB[0]) strncpy(gNameB, nameB, sizeof(gNameB) - 1), gNameB[sizeof(gNameB)-1] = 0;
  state_reset(&gState); history_clear(&gHist);
  saveState(); renderBoard();
}
static void onReset() {
  reset_match(&gState, &gHist);
  saveState(); renderBoard();
}
static void stateJson(char* out, size_t n) {
  char a[8], b[8];
  format_score(&gState, gGame, true,  a, sizeof(a));
  format_score(&gState, gGame, false, b, sizeof(b));
  snprintf(out, n, "{\"a\":\"%s\",\"b\":\"%s\",\"ga\":%d,\"gb\":%d}",
           a, b, gState.gamesA, gState.gamesB);
}

void setup() {
  Serial.begin(115200);
  prefs.begin("scoreboard", false);
  history_clear(&gHist);
  loadState();

  display_init();
  indicators_init();
  input_init();
  web_init(onConfig, onReset, stateJson);

  renderBoard();
  Serial.println("Scoreboard (LED-array) ready.");
}

void loop() {
  web_tick();
  display_tick();                 // display refresh hook (no-op for Max72xxPanel)
  indicators_tick(millis());

  BtnEvent ev = input_poll();
  if (ev == EV_NONE) return;

  if (match_over(&gState) && (ev == EV_A_UP || ev == EV_B_UP)) return;  // match done: ignore scoring

  switch (ev) {
    case EV_A_UP:   apply_point(&gState, &gHist, gGame, true);  break;
    case EV_B_UP:   apply_point(&gState, &gHist, gGame, false); break;
    case EV_A_DOWN: decrement_point(&gState, gGame, true);  break;  // only A
    case EV_B_DOWN: decrement_point(&gState, gGame, false); break;  // only B
    default: return;
  }
  saveState();
  renderBoard();
}
