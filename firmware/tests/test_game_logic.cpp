#include "test_util.h"
#include "game_logic.h"
#include <string>

static void test_free_count() {
  ScoreState s; History h;
  state_reset(&s); history_clear(&h);
  apply_point(&s, &h, FREE_COUNT, true);   // +1 A
  apply_point(&s, &h, FREE_COUNT, true);   // +1 A
  apply_point(&s, &h, FREE_COUNT, false);  // +1 B
  CHECK_EQ(s.scoreA, 2);
  CHECK_EQ(s.scoreB, 1);
  CHECK_EQ(s.gamesA, 0);                    // no win rule
  char buf[8];
  format_score(&s, FREE_COUNT, true, buf, sizeof(buf));
  CHECK_STR(buf, "2");
}

static void test_table_tennis() {
  ScoreState s; History h; state_reset(&s); history_clear(&h);
  for (int i = 0; i < 10; i++) apply_point(&s, &h, TABLE_TENNIS, true);  // A=10
  for (int i = 0; i < 10; i++) apply_point(&s, &h, TABLE_TENNIS, false); // B=10
  apply_point(&s, &h, TABLE_TENNIS, true);   // 11-10, not win-by-2
  CHECK_EQ(s.scoreA, 11); CHECK_EQ(s.gamesA, 0);
  apply_point(&s, &h, TABLE_TENNIS, true);   // 12-10 -> A wins game
  CHECK_EQ(s.gamesA, 1);
  CHECK_EQ(s.scoreA, 0); CHECK_EQ(s.scoreB, 0);  // round resets
}

static void test_badminton() {
  // normal win by 2 at 21
  ScoreState s; History h; state_reset(&s); history_clear(&h);
  for (int i = 0; i < 20; i++) apply_point(&s, &h, BADMINTON, true);   // A=20
  for (int i = 0; i < 20; i++) apply_point(&s, &h, BADMINTON, false);  // B=20
  apply_point(&s, &h, BADMINTON, true);   // 21-20 no win
  CHECK_EQ(s.gamesA, 0);
  apply_point(&s, &h, BADMINTON, true);   // 22-20 -> win by 2
  CHECK_EQ(s.gamesA, 1);

  // hard cap at 30 wins regardless of margin.
  // Must climb in lockstep (never 2 apart) or win-by-2 ends it at 21.
  state_reset(&s); history_clear(&h);
  for (int i = 0; i < 29; i++) {        // -> 29-29, no win-by-2 along the way
    apply_point(&s, &h, BADMINTON, true);
    apply_point(&s, &h, BADMINTON, false);
  }
  CHECK_EQ(s.gamesA, 0);
  apply_point(&s, &h, BADMINTON, true);   // 30-29 -> cap win
  CHECK_EQ(s.gamesA, 1);
  CHECK_EQ(s.scoreA, 0);
}

int main() {
  test_free_count();
  test_table_tennis();
  test_badminton();
  if (g_failures == 0) printf("ALL TESTS PASSED\n");
  return g_failures ? 1 : 0;
}
