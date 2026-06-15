#include "test_util.h"
#include "reaction.h"

static void test_normal_round() {
  ReactionState s;
  reaction_set_forced_hold(1500);
  reaction_start(&s, 0);
  CHECK_EQ(s.phase, R_COUNTDOWN);
  reaction_update(&s, 0);    CHECK_EQ(s.ledsLit, 1);
  reaction_update(&s, 4000); CHECK_EQ(s.ledsLit, 5);
  reaction_update(&s, 5000); CHECK_EQ(s.phase, R_ARMED);   // goAt = 5000+1500
  reaction_update(&s, 6000); CHECK_EQ(s.phase, R_ARMED);
  reaction_update(&s, 6500); CHECK_EQ(s.phase, R_GO);
  reaction_button(&s, true, 6650);                         // A reacts in 150ms
  CHECK_EQ(s.phase, R_RESULT);
  CHECK(s.haveWinner); CHECK(s.winnerIsA);
  CHECK_EQ(s.reactionA, 150); CHECK_EQ(s.reactionB, -1);
  CHECK_EQ(s.winsA, 1);
  // extra press ignored
  reaction_button(&s, false, 6700); CHECK_EQ(s.reactionB, -1);
  // RESULT auto-advances after RESULT_MS, wins preserved
  reaction_update(&s, 6650 + RESULT_MS);
  CHECK_EQ(s.phase, R_COUNTDOWN); CHECK_EQ(s.winsA, 1);
}

static void test_jump_start_countdown() {
  ReactionState s;
  reaction_set_forced_hold(1500);
  reaction_start(&s, 0);
  reaction_update(&s, 2000);            // still counting down
  reaction_button(&s, true, 2000);      // A jumps
  CHECK_EQ(s.phase, R_RESULT);
  CHECK(s.winnerIsA == false);          // B wins
  CHECK_EQ(s.reactionA, -2); CHECK_EQ(s.winsB, 1);
}

static void test_jump_start_armed() {
  ReactionState s;
  reaction_set_forced_hold(2000);
  reaction_start(&s, 0);
  reaction_update(&s, 5000);            // -> ARMED, goAt = 7000
  CHECK_EQ(s.phase, R_ARMED);
  reaction_button(&s, false, 5500);     // B jumps before GO
  CHECK_EQ(s.phase, R_RESULT);
  CHECK(s.winnerIsA);                   // A wins
  CHECK_EQ(s.reactionB, -2); CHECK_EQ(s.winsA, 1);
}

int main() {
  test_normal_round();
  test_jump_start_countdown();
  test_jump_start_armed();
  if (g_failures == 0) printf("ALL TESTS PASSED\n");
  return g_failures ? 1 : 0;
}
