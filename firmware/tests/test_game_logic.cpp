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

int main() {
  test_free_count();
  if (g_failures == 0) printf("ALL TESTS PASSED\n");
  return g_failures ? 1 : 0;
}
