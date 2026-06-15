#include "game_logic.h"
#include <stdio.h>

void state_reset(ScoreState* s) {
  s->scoreA = s->scoreB = 0;
  s->gamesA = s->gamesB = 0;
  s->tA = s->tB = 0;
}

void history_clear(History* h) { h->len = 0; }

static void history_push(History* h, const ScoreState* s) {
  if (h->len == UNDO_DEPTH) {
    for (int i = 1; i < UNDO_DEPTH; i++) h->items[i - 1] = h->items[i];
    h->len--;
  }
  h->items[h->len++] = *s;
}

// ---- Free Count -----------------------------------------------------------
static void free_onPoint(ScoreState* s, bool playerA) {
  if (playerA) s->scoreA++; else s->scoreB++;
}
static void int_format(const ScoreState* s, bool playerA, char* out, size_t n) {
  snprintf(out, n, "%d", playerA ? s->scoreA : s->scoreB);
}

// ---- Table Tennis ---------------------------------------------------------
static void award_round(ScoreState* s, bool playerA) {
  if (playerA) s->gamesA++; else s->gamesB++;
  s->scoreA = s->scoreB = 0;
}
static void tt_onPoint(ScoreState* s, bool playerA) {
  if (playerA) s->scoreA++; else s->scoreB++;
  int me   = playerA ? s->scoreA : s->scoreB;
  int them = playerA ? s->scoreB : s->scoreA;
  if (me >= 11 && (me - them) >= 2) award_round(s, playerA);
}

const GameRules GAMES[GAME_COUNT] = {
  { "Table Tennis", tt_onPoint, int_format },
  { "Tennis",       free_onPoint, int_format },  // placeholder, fixed in Task 4
  { "Badminton",    free_onPoint, int_format },  // placeholder, fixed in Task 3
  { "Free Count",   free_onPoint, int_format },
};

void apply_point(ScoreState* s, History* h, Game g, bool playerA) {
  history_push(h, s);
  GAMES[g].onPoint(s, playerA);
}

bool undo(ScoreState* s, History* h) {
  if (h->len == 0) return false;
  *s = h->items[--h->len];
  return true;
}

void reset_match(ScoreState* s, History* h) {
  history_push(h, s);
  s->scoreA = s->scoreB = 0;
  s->gamesA = s->gamesB = 0;
  s->tA = s->tB = 0;
}

void format_score(const ScoreState* s, Game g, bool playerA, char* out, size_t n) {
  GAMES[g].format(s, playerA, out, n);
}

const char* game_name(Game g) { return GAMES[g].name; }

Game menu_next(int* cursor) {
  *cursor = (*cursor + 1) % GAME_COUNT;
  return (Game)(*cursor);
}
