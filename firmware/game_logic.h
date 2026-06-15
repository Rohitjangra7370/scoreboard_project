#pragma once
#include <stddef.h>
#include <stdint.h>

enum Game { TABLE_TENNIS, TENNIS, BADMINTON, FREE_COUNT,
            PICKLEBALL, VOLLEYBALL, BASKETBALL_21, CORNHOLE, GAME_COUNT };
enum Mode { PLAYING, MENU, REACTION };

#define UNDO_DEPTH 20

struct ScoreState {
  int scoreA, scoreB;     // raw points (TT / badminton / free count)
  int gamesA, gamesB;     // rounds/games won -> LED stick
  uint8_t tA, tB;         // tennis point index: 0=0,1=15,2=30,3=40,4=AD
};

struct History {
  ScoreState items[UNDO_DEPTH];
  int len;
};

typedef void (*OnPointFn)(ScoreState* s, bool playerA);
typedef void (*FormatFn)(const ScoreState* s, bool playerA, char* out, size_t n);

struct GameRules {
  const char* name;
  OnPointFn   onPoint;    // nullptr => use the generic count_winByTwo handler
  FormatFn    format;
  int         winScore;   // for the generic handler
  int         hardCap;    // 0 = no cap
};

#define REACTION_ITEM (GAME_COUNT)     // menu index of the reaction game
#define NUM_MENU      (GAME_COUNT + 1) // scoring games + reaction

extern const GameRules GAMES[GAME_COUNT];

void state_reset(ScoreState* s);
void history_clear(History* h);
void apply_point(ScoreState* s, History* h, Game g, bool playerA);
bool undo(ScoreState* s, History* h);          // false if nothing to undo
void reset_match(ScoreState* s, History* h);   // pushes snapshot, zeroes scores+games
void format_score(const ScoreState* s, Game g, bool playerA, char* out, size_t n);
const char* game_name(Game g);
int         menu_advance(int cursor);  // (cursor + 1) % NUM_MENU
const char* menu_name(int idx);        // game name, or "Reaction" for REACTION_ITEM
