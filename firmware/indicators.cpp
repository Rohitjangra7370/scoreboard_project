#include "indicators.h"
#include <Arduino.h>
#include "game_logic.h"   // MATCH_TARGET

static const uint8_t A_LEDS[3] = {26, 27, 14};  // green, player A
static const uint8_t B_LEDS[3] = {4, 16, 17};   // red,   player B

static int  winnerSide = -1;   // -1 none, 0 = A, 1 = B
static bool blinkOn = false;
static uint32_t lastBlink = 0;

void indicators_init() {
  for (int i = 0; i < 3; i++) {
    pinMode(A_LEDS[i], OUTPUT); digitalWrite(A_LEDS[i], LOW);
    pinMode(B_LEDS[i], OUTPUT); digitalWrite(B_LEDS[i], LOW);
  }
}

void indicators_update(int gamesA, int gamesB) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(A_LEDS[i], i < gamesA ? HIGH : LOW);
    digitalWrite(B_LEDS[i], i < gamesB ? HIGH : LOW);
  }
  if      (gamesA >= MATCH_TARGET) winnerSide = 0;
  else if (gamesB >= MATCH_TARGET) winnerSide = 1;
  else                             winnerSide = -1;
}

void indicators_tick(uint32_t now) {
  if (winnerSide < 0) return;
  if (now - lastBlink < 400) return;
  lastBlink = now;
  blinkOn = !blinkOn;
  uint8_t pin = (winnerSide == 0) ? A_LEDS[2] : B_LEDS[2];  // spare 3rd LED
  digitalWrite(pin, blinkOn ? HIGH : LOW);
}
