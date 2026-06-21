#include "display_matrix.h"
#include <string.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

// All four 16x16 modules (16x MAX7219 8x8 tiles) hang off ONE SPI chain:
//   ESP32 hardware VSPI -> SCK=GPIO18, MOSI(DIN)=GPIO23, CS=GPIO5.
#define MATRIX_CS 5

// 4 physical 16x16 modules = a 4x4 grid of 8x8 tiles -> 32x32 logical pixels.
#define H_TILES 4
#define V_TILES 4
#define NUM_TILES (H_TILES * V_TILES)

static Max72xxPanel matrix = Max72xxPanel(MATRIX_CS, H_TILES, V_TILES);

// ---------------------------------------------------------------------------
// HARDWARE CALIBRATION (cheap MAX7219 panels vary by maker)
// ---------------------------------------------------------------------------
// The 16 8x8 tiles are chained in this physical order:
//   tiles  0..3  -> A-initials module   (top-left  16x16 quadrant)
//   tiles  4..7  -> A-score module      (bottom-left)
//   tiles  8..11 -> B-initials module   (top-right)
//   tiles 12..15 -> B-score module      (bottom-right)
// Each module's 4 internal tiles are assumed chained TL,TR,BL,BR.
// Map each chain tile to its (col,row) position in the 4x4 logical grid:
static const uint8_t TILE_X[NUM_TILES] = { 0,1,0,1,  0,1,0,1,  2,3,2,3,  2,3,2,3 };
static const uint8_t TILE_Y[NUM_TILES] = { 0,0,1,1,  2,2,3,3,  0,0,1,1,  2,2,3,3 };
// If a quadrant shows up rotated/mirrored on real hardware, change this
// (0..3). FC16-style modules commonly need 1; generic 16x16 often need 0.
#define TILE_ROTATION 1

// 16x16 quadrant top-left origins (logical pixels).
struct Cell { int ox, oy; };
static const Cell CELL_A_INIT  = {  0,  0 };
static const Cell CELL_A_SCORE = {  0, 16 };
static const Cell CELL_B_INIT  = { 16,  0 };
static const Cell CELL_B_SCORE = { 16, 16 };

void display_init() {
  for (uint8_t i = 0; i < NUM_TILES; i++) {
    matrix.setPosition(i, TILE_X[i], TILE_Y[i]);
    matrix.setRotation(i, TILE_ROTATION);
  }
  matrix.setIntensity(6);          // 0..15 brightness
  matrix.fillScreen(LOW);
  matrix.write();
}

// Draw up to 2 chars centered in a 16x16 cell. 1 char -> size 2 (12x16),
// 2 chars -> size 1 (6x8 each) so both fit across 16 px.
static void drawCell(const Cell& c, const char* s) {
  char buf[3];
  strncpy(buf, s, 2); buf[2] = 0;
  int len = strlen(buf);
  if (len == 0) return;
  uint8_t size = (len <= 1) ? 2 : 1;
  int charW = 6 * size, charH = 8 * size;
  int x = c.ox + (16 - charW * len) / 2;
  int y = c.oy + (16 - charH) / 2;
  matrix.setTextSize(size);
  matrix.setTextColor(HIGH, LOW);
  matrix.setCursor(x, y);
  matrix.print(buf);
}

void display_show(const char* initA, const char* scoreA,
                  const char* initB, const char* scoreB) {
  matrix.fillScreen(LOW);
  drawCell(CELL_A_INIT,  initA);
  drawCell(CELL_A_SCORE, scoreA);
  drawCell(CELL_B_INIT,  initB);
  drawCell(CELL_B_SCORE, scoreB);
  matrix.write();
}

void display_tick() {
  // Max72xxPanel pushes the frame in display_show(); nothing to maintain here.
}
