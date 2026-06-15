#pragma once
#include <cstdio>

inline int g_failures = 0;

#define CHECK(cond) do { if (!(cond)) { \
  printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); g_failures++; } } while (0)

#define CHECK_EQ(a, b) do { long _va = (long)(a), _vb = (long)(b); \
  if (_va != _vb) { printf("FAIL %s:%d: %s == %s (got %ld vs %ld)\n", \
  __FILE__, __LINE__, #a, #b, _va, _vb); g_failures++; } } while (0)

#define CHECK_STR(a, b) do { if (std::string(a) != std::string(b)) { \
  printf("FAIL %s:%d: %s == \"%s\" (got \"%s\")\n", __FILE__, __LINE__, #a, b, a); \
  g_failures++; } } while (0)
