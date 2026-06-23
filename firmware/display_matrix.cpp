#include "display_matrix.h"
#include <string.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "Max72xxPanel.h"   // vendored (BSD, Mark Ruys) — not in Arduino registry

// All MAX7219 modules hang off ONE SPI chain:
//   ESP32 hardware VSPI -> SCK=GPIO18, MOSI(DIN)=GPIO23, CS=GPIO5.
#define MATRIX_CS 5

// ---------------------------------------------------------------------------
// DISPLAY SCALE
// ---------------------------------------------------------------------------
// SCALE = number of 16x16 modules along each side of ONE field.
//   SCALE 1 -> field = 1x 16x16 module (16x16 px), 4 fields = 4 modules,  32x32 logical.
//   SCALE 2 -> field = 2x2 16x16 modules (32x32 px), 4 fields = 16 modules, 64x64 logical.
// Bigger SCALE = bigger digits = readable from further, but more modules,
// more current, more cost, more weight. SCALE 2 ~= 12 cm digits, clear to ~18 m.
#define SCALE 2

// 16x16 module = 2x2 of the MAX7219 8x8 tiles.
#define TILES_PER_MODULE_SIDE 2
#define FIELD_TILES   (TILES_PER_MODULE_SIDE * SCALE)   // 8x8 tiles per field side
#define H_TILES       (2 * FIELD_TILES)                 // 2 fields wide
#define V_TILES       (2 * FIELD_TILES)                 // 2 fields tall
#define NUM_TILES     (H_TILES * V_TILES)
#define CELLPX        (16 * SCALE)                       // one field, in logical pixels

static Max72xxPanel matrix = Max72xxPanel(MATRIX_CS, H_TILES, V_TILES);

// ---------------------------------------------------------------------------
// HARDWARE CALIBRATION (cheap MAX7219 panels vary by maker)
// ---------------------------------------------------------------------------
// The 8x8 tiles are daisy-chained from the ESP32 in this PHYSICAL order:
//   1. Fields, in the order: A-initials, A-score, B-initials, B-score.
//   2. Within a field, its SCALE x SCALE modules, row-major (TL, TR, BL, BR...).
//   3. Within a module, its 4 tiles, in order TL, TR, BL, BR.
// Wire the chain to match this order (DOUT -> DIN, module to module) and the
// map below is computed for you — no hand-editing needed when SCALE changes.
//
// If a quadrant shows up rotated/mirrored on real hardware, change TILE_ROTATION
// (0..3). FC16-style modules commonly need 1; generic 16x16 often need 0.
#define TILE_ROTATION 1

// Field top-left origins, in TILE units (each field is FIELD_TILES tiles per side).
//   A-init  top-left      A-score  bottom-left
//   B-init  top-right     B-score  bottom-right
static const uint8_t FIELD_TX[4] = { 0,          0,          FIELD_TILES, FIELD_TILES };
static const uint8_t FIELD_TY[4] = { 0,          FIELD_TILES, 0,          FIELD_TILES };

// Computed per-chain-tile (x,y) position in the H_TILES x V_TILES logical grid.
static uint8_t TILE_X[NUM_TILES];
static uint8_t TILE_Y[NUM_TILES];

static void buildTileMap() {
  int idx = 0;
  for (int f = 0; f < 4; f++) {                     // field
    for (int m = 0; m < SCALE * SCALE; m++) {        // module within field (row-major)
      int mrow = m / SCALE, mcol = m % SCALE;
      int mbx = FIELD_TX[f] + mcol * TILES_PER_MODULE_SIDE;  // module base, tile units
      int mby = FIELD_TY[f] + mrow * TILES_PER_MODULE_SIDE;
      // tiles within module: TL, TR, BL, BR
      static const int TDX[4] = { 0, 1, 0, 1 };
      static const int TDY[4] = { 0, 0, 1, 1 };
      for (int t = 0; t < 4; t++) {
        TILE_X[idx] = mbx + TDX[t];
        TILE_Y[idx] = mby + TDY[t];
        idx++;
      }
    }
  }
}

// 16x16-per-module quadrant origins scale up to CELLPX-sized field cells.
struct Cell { int ox, oy; };
static const Cell CELL_A_INIT  = {     0,      0 };
static const Cell CELL_A_SCORE = {     0, CELLPX };
static const Cell CELL_B_INIT  = { CELLPX,     0 };
static const Cell CELL_B_SCORE = { CELLPX, CELLPX };

void display_init() {
  buildTileMap();
  for (uint16_t i = 0; i < NUM_TILES; i++) {
    matrix.setPosition(i, TILE_X[i], TILE_Y[i]);
    matrix.setRotation(i, TILE_ROTATION);
  }
  matrix.setIntensity(6);          // 0..15 brightness (keep modest: 16 modules draw real current)
  matrix.fillScreen(LOW);
  matrix.write();
}

// Draw up to 2 chars centered in a CELLPX x CELLPX field.
// 1 char -> size 2*SCALE (tall, fills the cell), 2 chars -> size SCALE (both fit across).
static void drawCell(const Cell& c, const char* s) {
  char buf[3];
  strncpy(buf, s, 2); buf[2] = 0;
  int len = strlen(buf);
  if (len == 0) return;
  uint8_t size = (len <= 1) ? (2 * SCALE) : SCALE;
  int charW = 6 * size, charH = 8 * size;
  int x = c.ox + (CELLPX - charW * len) / 2;
  int y = c.oy + (CELLPX - charH) / 2;
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
