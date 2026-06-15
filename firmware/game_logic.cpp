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

static void count_winByTwo(ScoreState* s, bool playerA, int winScore, int hardCap) {
  if (playerA) s->scoreA++; else s->scoreB++;
  int me   = playerA ? s->scoreA : s->scoreB;
  int them = playerA ? s->scoreB : s->scoreA;
  bool win = (me >= winScore && (me - them) >= 2) || (hardCap > 0 && me >= hardCap);
  if (win) award_round(s, playerA);
}

// ---- Tennis ---------------------------------------------------------------
static void award_game_tennis(ScoreState* s, bool playerA) {
  if (playerA) s->gamesA++; else s->gamesB++;
  s->tA = s->tB = 0;
}
static void tennis_onPoint(ScoreState* s, bool playerA) {
  uint8_t* me  = playerA ? &s->tA : &s->tB;
  uint8_t* opp = playerA ? &s->tB : &s->tA;
  if (*me < 3 && *opp < 4) {            // climbing 0->15->30->40
    (*me)++;
  } else if (*me == 3 && *opp < 3) {    // 40 vs <40 -> game
    award_game_tennis(s, playerA);
  } else if (*me == 3 && *opp == 3) {   // deuce -> my advantage
    *me = 4;
  } else if (*me == 3 && *opp == 4) {   // I score, opp loses advantage -> deuce
    *opp = 3;
  } else if (*me == 4) {                // I had advantage -> game
    award_game_tennis(s, playerA);
  }
}
static const char* TENNIS_POINTS[] = { "0", "15", "30", "40", "AD" };
static void tennis_format(const ScoreState* s, bool playerA, char* out, size_t n) {
  uint8_t idx = playerA ? s->tA : s->tB;
  snprintf(out, n, "%s", TENNIS_POINTS[idx]);
}

const GameRules GAMES[GAME_COUNT] = {
  { "Table Tennis",  nullptr,        int_format,    11, 0  },
  { "Tennis",        tennis_onPoint, tennis_format, 0,  0  },
  { "Badminton",     nullptr,        int_format,    21, 30 },
  { "Free Count",    free_onPoint,   int_format,    0,  0  },
  { "Pickleball",    nullptr,        int_format,    11, 0  },
  { "Volleyball",    nullptr,        int_format,    25, 0  },
  { "Basketball 21", nullptr,        int_format,    21, 0  },
  { "Cornhole",      nullptr,        int_format,    21, 0  },
};

void apply_point(ScoreState* s, History* h, Game g, bool playerA) {
  history_push(h, s);
  if (GAMES[g].onPoint) GAMES[g].onPoint(s, playerA);
  else count_winByTwo(s, playerA, GAMES[g].winScore, GAMES[g].hardCap);
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
