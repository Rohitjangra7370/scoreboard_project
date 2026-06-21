#include "display_matrix.h"
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8        // 2 panels x 4 modules
#define MATRIX_CLK  18
#define MATRIX_DATA 23
#define MATRIX_CS   5

static MD_Parola P = MD_Parola(HARDWARE_TYPE, MATRIX_DATA, MATRIX_CLK, MATRIX_CS, MAX_DEVICES);

void display_init() {
  P.begin(2);                       // two zones
  P.setZone(0, 0, 3);               // Player A: modules 0..3
  P.setZone(1, 4, 7);               // Player B: modules 4..7
  P.setIntensity(6);                // 0..15 brightness
  P.displayClear();
}

void display_show(const char* a, const char* b) {
  P.displayZoneText(0, a, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(1, b, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayReset(0);
  P.displayReset(1);
  P.displayAnimate();               // PA_PRINT renders immediately
}
