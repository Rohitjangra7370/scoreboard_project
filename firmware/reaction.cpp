#include "reaction.h"
#include <stdlib.h>

static uint32_t g_forcedHold = 0;
void reaction_set_forced_hold(uint32_t ms) { g_forcedHold = ms; }

static uint32_t pick_hold() {
  if (g_forcedHold) return g_forcedHold;
  return HOLD_MIN_MS + (uint32_t)(rand() % (HOLD_MAX_MS - HOLD_MIN_MS + 1));
}

static void start_round(ReactionState* s, uint32_t now) {
  s->phase = R_COUNTDOWN;
  s->phaseEnteredAt = now;
  s->ledsLit = 0;
  s->goAt = 0;
  s->holdMs = 0;
  s->reactionA = s->reactionB = -1;
  s->haveWinner = false;
  s->winnerIsA = false;
}

void reaction_start(ReactionState* s, uint32_t now) {
  s->winsA = s->winsB = 0;
  start_round(s, now);
}

static void to_result(ReactionState* s, uint32_t now, bool winnerA) {
  s->haveWinner = true;
  s->winnerIsA = winnerA;
  if (winnerA) s->winsA++; else s->winsB++;
  s->phase = R_RESULT;
  s->phaseEnteredAt = now;
  s->ledsLit = 0;
}

void reaction_update(ReactionState* s, uint32_t now) {
  switch (s->phase) {
    case R_COUNTDOWN: {
      uint32_t step = (now - s->phaseEnteredAt) / COUNTDOWN_STEP_MS;
      if (step >= COUNTDOWN_LEDS) {
        s->ledsLit = COUNTDOWN_LEDS;
        s->phase = R_ARMED;
        s->phaseEnteredAt = now;
        s->holdMs = pick_hold();
        s->goAt = now + s->holdMs;
      } else {
        s->ledsLit = (uint8_t)(step + 1);
      }
      break;
    }
    case R_ARMED:
      if (now >= s->goAt) {
        s->phase = R_GO;
        s->phaseEnteredAt = now;
        s->ledsLit = 0;
      }
      break;
    case R_GO:
      break;  // waiting for a press
    case R_RESULT:
      if (now - s->phaseEnteredAt >= RESULT_MS) start_round(s, now);
      break;
    case R_IDLE:
      break;
  }
}

void reaction_button(ReactionState* s, bool playerA, uint32_t now) {
  if (s->haveWinner) return;
  if (s->phase == R_COUNTDOWN || s->phase == R_ARMED) {
    if (playerA) s->reactionA = -2; else s->reactionB = -2;   // jump start
    to_result(s, now, !playerA);
  } else if (s->phase == R_GO) {
    int rt = (int)(now - s->goAt);
    if (playerA) s->reactionA = rt; else s->reactionB = rt;
    to_result(s, now, playerA);
  }
}
