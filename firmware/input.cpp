#include "input.h"
#include <Arduino.h>

#define PIN_A_UP   32
#define PIN_A_DOWN 33
#define PIN_B_UP   25
#define PIN_B_DOWN 13
#define DEBOUNCE_MS 40

struct Btn { uint8_t pin; BtnEvent ev; bool last; uint32_t t; };
static Btn btns[4] = {
  {PIN_A_UP,   EV_A_UP,   true, 0},
  {PIN_A_DOWN, EV_A_DOWN, true, 0},
  {PIN_B_UP,   EV_B_UP,   true, 0},
  {PIN_B_DOWN, EV_B_DOWN, true, 0},
};

void input_init() {
  for (int i = 0; i < 4; i++) {
    pinMode(btns[i].pin, INPUT_PULLUP);
    btns[i].last = HIGH;
    btns[i].t = 0;
  }
}

BtnEvent input_poll() {
  uint32_t now = millis();
  for (int i = 0; i < 4; i++) {
    bool cur = digitalRead(btns[i].pin);          // HIGH idle, LOW pressed
    if (cur != btns[i].last && (now - btns[i].t) > DEBOUNCE_MS) {
      btns[i].t = now;
      btns[i].last = cur;
      if (cur == LOW) return btns[i].ev;           // fire on press (falling edge)
    }
  }
  return EV_NONE;
}
