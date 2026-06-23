#pragma once
// MAX7219 square-matrix display, driven via Adafruit GFX + Max72xxPanel.
// Size is set by SCALE in display_matrix.cpp:
//   SCALE 1 -> 4x 16x16 modules  (32x32 px, ~6 cm digits, ~12 m)
//   SCALE 2 -> 16x 16x16 modules (64x64 px, ~12 cm digits, ~18 m)  <- current build
// Layout (one field per quadrant, A left / B right):
//     [initA]   [initB]      <- top    fields
//     [scoreA]  [scoreB]     <- bottom fields
// Each cell shows up to 2 characters (1 char = large, 2 chars = small).
void display_init();
void display_show(const char* initA, const char* scoreA,
                  const char* initB, const char* scoreB);
void display_tick();  // kept for loop() compatibility (writes are immediate; no-op)
