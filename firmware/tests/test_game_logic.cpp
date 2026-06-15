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

static void test_tennis() {
  ScoreState s; History h; state_reset(&s); history_clear(&h);
  char buf[8];
  apply_point(&s, &h, TENNIS, true);  // A:15
  format_score(&s, TENNIS, true, buf, sizeof(buf)); CHECK_STR(buf, "15");
  apply_point(&s, &h, TENNIS, true);  // A:30
  apply_point(&s, &h, TENNIS, true);  // A:40
  format_score(&s, TENNIS, true, buf, sizeof(buf)); CHECK_STR(buf, "40");
  apply_point(&s, &h, TENNIS, true);  // A wins game (40 vs 0)
  CHECK_EQ(s.gamesA, 1);
  CHECK_EQ(s.tA, 0); CHECK_EQ(s.tB, 0);

  // deuce / advantage path
  state_reset(&s); history_clear(&h);
  for (int i = 0; i < 3; i++) apply_point(&s, &h, TENNIS, true);   // A:40
  for (int i = 0; i < 3; i++) apply_point(&s, &h, TENNIS, false);  // B:40 -> deuce
  apply_point(&s, &h, TENNIS, true);   // A advantage
  format_score(&s, TENNIS, true, buf, sizeof(buf)); CHECK_STR(buf, "AD");
  apply_point(&s, &h, TENNIS, false);  // back to deuce
  format_score(&s, TENNIS, true, buf, sizeof(buf)); CHECK_STR(buf, "40");
  apply_point(&s, &h, TENNIS, false);  // B advantage
  apply_point(&s, &h, TENNIS, false);  // B wins game
  CHECK_EQ(s.gamesB, 1);
  CHECK_EQ(s.tA, 0); CHECK_EQ(s.tB, 0);
}

static void test_undo() {
  ScoreState s; History h; state_reset(&s); history_clear(&h);

  // undo a plain point
  apply_point(&s, &h, TABLE_TENNIS, true);   // A=1
  apply_point(&s, &h, TABLE_TENNIS, false);  // B=1
  CHECK(undo(&s, &h));                        // remove B's point
  CHECK_EQ(s.scoreA, 1); CHECK_EQ(s.scoreB, 0);

  // undo a round-winning point restores pre-win score + removes the game/LED.
  // Need 10-10 first, else TT wins at 11-0 (win-by-2 vs an opponent on 0).
  state_reset(&s); history_clear(&h);
  for (int i = 0; i < 10; i++) apply_point(&s, &h, TABLE_TENNIS, true);  // A=10
  for (int i = 0; i < 10; i++) apply_point(&s, &h, TABLE_TENNIS, false); // B=10
  apply_point(&s, &h, TABLE_TENNIS, true);   // 11-10, no win (diff 1)
  CHECK_EQ(s.gamesA, 0); CHECK_EQ(s.scoreA, 11);
  apply_point(&s, &h, TABLE_TENNIS, true);   // 12-10 -> win, gamesA=1, reset
  CHECK_EQ(s.gamesA, 1); CHECK_EQ(s.scoreA, 0);
  CHECK(undo(&s, &h));                         // un-win
  CHECK_EQ(s.gamesA, 0); CHECK_EQ(s.scoreA, 11);

  // empty stack -> false
  state_reset(&s); history_clear(&h);
  CHECK(!undo(&s, &h));

  // depth caps at UNDO_DEPTH (oldest dropped, never crashes)
  state_reset(&s); history_clear(&h);
  for (int i = 0; i < UNDO_DEPTH + 5; i++) apply_point(&s, &h, FREE_COUNT, true);
  CHECK_EQ(h.len, UNDO_DEPTH);
  int undone = 0; while (undo(&s, &h)) undone++;
  CHECK_EQ(undone, UNDO_DEPTH);
}

static void test_reset() {
  ScoreState s; History h; state_reset(&s); history_clear(&h);
  for (int i = 0; i < 11; i++) apply_point(&s, &h, FREE_COUNT, true);  // A=11
  reset_match(&s, &h);
  CHECK_EQ(s.scoreA, 0); CHECK_EQ(s.gamesA, 0);
  CHECK(undo(&s, &h));                 // reset itself is undoable
  CHECK_EQ(s.scoreA, 11);
}

static void test_menu_next() {
  int cursor = 0;
  CHECK_EQ(menu_next(&cursor), TENNIS);      // 0 -> 1
  CHECK_EQ(menu_next(&cursor), BADMINTON);   // 1 -> 2
  CHECK_EQ(menu_next(&cursor), FREE_COUNT);  // 2 -> 3
  // wrap: from last game back to TABLE_TENNIS (index 0)
  cursor = GAME_COUNT - 1;
  CHECK_EQ(menu_next(&cursor), TABLE_TENNIS); // GAME_COUNT-1 -> 0
}

// helper: alternate to (target-1, target-1) then have A score `extra` times
static void rally_to(ScoreState* s, History* h, Game g, int even, int extraA) {
  for (int i = 0; i < even; i++) { apply_point(s, h, g, true); apply_point(s, h, g, false); }
  for (int i = 0; i < extraA; i++) apply_point(s, h, g, true);
}

static void test_new_sports() {
  ScoreState s; History h;

  // Pickleball: to 11, win by 2
  state_reset(&s); history_clear(&h);
  rally_to(&s, &h, PICKLEBALL, 9, 0);          // 9-9
  apply_point(&s, &h, PICKLEBALL, true);       // 10-9
  apply_point(&s, &h, PICKLEBALL, true);       // 11-9 -> win
  CHECK_EQ(s.gamesA, 1); CHECK_EQ(s.scoreA, 0);

  // Volleyball: to 25, win by 2
  state_reset(&s); history_clear(&h);
  rally_to(&s, &h, VOLLEYBALL, 24, 0);         // 24-24
  apply_point(&s, &h, VOLLEYBALL, true);       // 25-24, no win
  CHECK_EQ(s.gamesA, 0);
  apply_point(&s, &h, VOLLEYBALL, true);       // 26-24 -> win
  CHECK_EQ(s.gamesA, 1);

  // Basketball 21: to 21, win by 2
  state_reset(&s); history_clear(&h);
  rally_to(&s, &h, BASKETBALL_21, 20, 0);      // 20-20
  apply_point(&s, &h, BASKETBALL_21, true);    // 21-20, no win
  CHECK_EQ(s.gamesA, 0);

  // Cornhole: to 21, win by 2
  state_reset(&s); history_clear(&h);
  rally_to(&s, &h, CORNHOLE, 19, 0);           // 19-19
  apply_point(&s, &h, CORNHOLE, true);         // 20-19
  apply_point(&s, &h, CORNHOLE, true);         // 21-19 -> win
  CHECK_EQ(s.gamesA, 1);
}

int main() {
  test_free_count();
  test_table_tennis();
  test_badminton();
  test_tennis();
  test_undo();
  test_reset();
  test_menu_next();
  test_new_sports();
  if (g_failures == 0) printf("ALL TESTS PASSED\n");
  return g_failures ? 1 : 0;
}
