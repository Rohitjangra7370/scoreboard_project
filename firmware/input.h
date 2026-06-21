#pragma once
enum BtnEvent { EV_NONE, EV_A_UP, EV_A_DOWN, EV_B_UP, EV_B_DOWN };
void input_init();
BtnEvent input_poll();
