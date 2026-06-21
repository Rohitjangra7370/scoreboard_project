#pragma once
// 2x MAX7219 8x32 dot-matrix as two MD_Parola zones (A = modules 0-3, B = 4-7).
void display_init();
void display_show(const char* a, const char* b);  // e.g. ("A 07", "B 40")
void display_tick();  // MUST be called every loop() — drives MD_Parola rendering
