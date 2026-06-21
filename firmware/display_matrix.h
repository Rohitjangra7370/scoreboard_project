#pragma once
// 4x 16x16 MAX7219 square matrices, driven via Adafruit GFX + Max72xxPanel.
// Layout (stacked per player, A left / B right):
//     [initA]   [initB]      <- top    16x16 quadrants
//     [scoreA]  [scoreB]     <- bottom 16x16 quadrants
// Each cell shows up to 2 characters (1 char = large, 2 chars = small).
void display_init();
void display_show(const char* initA, const char* scoreA,
                  const char* initB, const char* scoreB);
void display_tick();  // kept for loop() compatibility (writes are immediate; no-op)
