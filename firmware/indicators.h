#pragma once
#include <stdint.h>
void indicators_init();
void indicators_update(int gamesA, int gamesB);
void indicators_tick(uint32_t now);
