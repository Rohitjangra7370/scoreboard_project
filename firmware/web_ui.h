#pragma once
#include <stddef.h>
typedef void (*ConfigCb)(int gameIdx, const char* nameA, const char* nameB);
typedef void (*ResetCb)();
typedef void (*StateFn)(char* out, size_t n);
void web_init(ConfigCb onConfig, ResetCb onReset, StateFn stateJson);
void web_tick();
