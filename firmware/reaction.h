#pragma once
#include <stdint.h>

enum ReactionPhase { R_IDLE, R_COUNTDOWN, R_ARMED, R_GO, R_RESULT };

#define COUNTDOWN_LEDS     5
#define COUNTDOWN_STEP_MS  1000
#define HOLD_MIN_MS        1000
#define HOLD_MAX_MS        3000
#define RESULT_MS          2500

struct ReactionState {
  ReactionPhase phase;
  uint32_t phaseEnteredAt;
  uint32_t goAt;
  uint8_t  ledsLit;          // 0..COUNTDOWN_LEDS
  uint32_t holdMs;
  int reactionA, reactionB;  // ms; -1 = none, -2 = jump start
  bool haveWinner;
  bool winnerIsA;
  int winsA, winsB;          // session round-win tally
};

void reaction_start(ReactionState* s, uint32_t now);   // zero tally + begin a round
void reaction_update(ReactionState* s, uint32_t now);   // time-based transitions
void reaction_button(ReactionState* s, bool playerA, uint32_t now);
void reaction_set_forced_hold(uint32_t ms);             // test hook; 0 = random
