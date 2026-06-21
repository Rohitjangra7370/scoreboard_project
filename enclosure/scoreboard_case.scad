// ============================================================================
// Smart A4 Scoreboard - Parametric Enclosure  (Rev E — 4× square matrices)
// ----------------------------------------------------------------------------
// Hardware: 4× MAX7219 16×16 square matrix modules (~70 mm), stacked 2 per
//           player: initials over score.
//           6× 5 mm status LEDs (3 per side, best-of-3 tally),
//           4× 12 mm panel momentary push-buttons (2 per player).
//
// A4-footprint, two-part (tray + faceplate) 3D-printable case.
// Layout (landscape A4, 297 × 210 mm):
//   [A-init ] [B-init ]      <- top    square windows
//   [A-score] [B-score]      <- bottom square windows
//   LEDs + buttons sit in the strip below each player's column.
//
// Render the two parts separately for printing:
//   openscad -o faceplate.stl -D 'PART="faceplate"' scoreboard_case.scad
//   openscad -o tray.stl      -D 'PART="tray"'      scoreboard_case.scad
//
// Author: Rohit Jangra. Units = millimetres.
// ============================================================================

PART = "faceplate";   // "tray" | "faceplate" | "preview"

// ---- A4 footprint ----------------------------------------------------------
A4_W = 297;            // width  (landscape)
A4_H = 210;            // height
WALL = 2.4;            // wall thickness
FLOOR = 2.0;           // tray floor thickness
FACE = 2.5;            // faceplate thickness
CAVITY_H = 22;         // internal depth for boards/battery
CORNER_R = 6;          // outer corner radius

// ---- MAX7219 16×16 square matrix module ------------------------------------
// Physical module ≈ 70 mm; active LED area ≈ 64 mm. The 68 mm window exposes
// the active area and captures the module body behind the plate.
MAT_WIN = 68;          // square window side

// Column centres (A left, B right), symmetric about the A4 mid-line.
MAT_A_X = 78;                 // left column centre  (window 44..112)
MAT_B_X = A4_W - 78;          // right column centre (window 185..253) = 219

// Row centres: initials on top, score below.
INIT_Y  = 166;                // top window  132..200
SCORE_Y = 88;                 // bottom window 54..122  (gap 122..132 = 10 mm)

// ---- 5 mm status LED holes (best-of-3 tally) -------------------------------
// Sit in the strip below each player's score window.
LED5_D       = 5.2;    // bore for 5 mm LED body
LED5_SPACING = 10;     // centre-to-centre spacing in the row of 3
LED5_Y       = 44;     // below the bottom (score) window

// ---- Panel push-button holes -----------------------------------------------
BTN_D        = 12.2;   // bore for 12 mm panel momentary button
BTN_SPACING  = 20;     // centre-to-centre between the +/- pair
BTN_Y        = 22;     // bottom strip, below the LED row

// ---- USB-C charge port (TP4056) on right side wall -------------------------
USB_W = 12;            // cutout width
USB_H = 7;             // cutout height

// ---- Mounting / screw bosses -----------------------------------------------
POST_D = 7;            // screw boss outer dia
SCREW_D = 3.2;         // M3 clearance
INSET = 10;            // boss inset from corners

$fn = 48;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
module rrect(w, h, r) {
    hull() for (x = [r, w - r], y = [r, h - r]) translate([x, y]) circle(r);
}

// A square matrix window centred at (cx, cy).
module mat_window(cx, cy) {
    translate([cx - MAT_WIN / 2, cy - MAT_WIN / 2, -1])
        cube([MAT_WIN, MAT_WIN, FACE + 2]);
}

// 3 LED holes centred on column X cx, at row LED5_Y.
module led_row(cx) {
    for (i = [-1, 0, 1])
        translate([cx + i * LED5_SPACING, LED5_Y, -1])
            cylinder(d = LED5_D, h = FACE + 2);
}

// 2 button holes centred on column X cx, at row BTN_Y.
module btn_pair(cx) {
    for (i = [-1, 1])
        translate([cx + i * BTN_SPACING / 2, BTN_Y, -1])
            cylinder(d = BTN_D, h = FACE + 2);
}

// ---------------------------------------------------------------------------
// Tray (bottom): floor + perimeter walls + screw bosses + USB-C slot
// UNCHANGED from Rev A — only the faceplate changes.
// ---------------------------------------------------------------------------
module tray() {
    difference() {
        union() {
            // floor
            linear_extrude(FLOOR) rrect(A4_W, A4_H, CORNER_R);
            // perimeter wall
            linear_extrude(CAVITY_H)
                difference() {
                    rrect(A4_W, A4_H, CORNER_R);
                    offset(-WALL) rrect(A4_W, A4_H, CORNER_R);
                }
            // screw bosses (4 corners)
            for (x = [INSET, A4_W - INSET], y = [INSET, A4_H - INSET])
                translate([x, y, 0]) cylinder(d = POST_D, h = CAVITY_H);
        }
        // screw pilot holes
        for (x = [INSET, A4_W - INSET], y = [INSET, A4_H - INSET])
            translate([x, y, FLOOR]) cylinder(d = SCREW_D, h = CAVITY_H);
        // USB-C charge port on right wall
        translate([A4_W - WALL - 1, A4_H / 2 - USB_W / 2, FLOOR + 4])
            cube([WALL + 2, USB_W, USB_H]);
    }
}

// ---------------------------------------------------------------------------
// Faceplate (top): A4 panel — Rev E (4 square matrices)
//   • 4× square matrix windows (initials top, score bottom; A left, B right)
//   • 6× 5 mm LED holes (3 per side, below each column)
//   • 4× 12 mm button holes (2 per player, below the LEDs)
//   • USB-C pass-through slot (matches tray)
//   • 4× M3 screw holes (match tray bosses)
// ---------------------------------------------------------------------------
module faceplate() {
    difference() {
        // solid A4 panel
        linear_extrude(FACE) rrect(A4_W, A4_H, CORNER_R);

        // ---- 4 square matrix windows --------------------------------------
        mat_window(MAT_A_X, INIT_Y);    // A initials (top-left)
        mat_window(MAT_A_X, SCORE_Y);   // A score    (bottom-left)
        mat_window(MAT_B_X, INIT_Y);    // B initials (top-right)
        mat_window(MAT_B_X, SCORE_Y);   // B score    (bottom-right)

        // ---- status LEDs (3 per player, below each column) ----------------
        led_row(MAT_A_X);
        led_row(MAT_B_X);

        // ---- push buttons (2 per player, below the LEDs) ------------------
        btn_pair(MAT_A_X);
        btn_pair(MAT_B_X);

        // ---- USB-C slot (right edge, matches tray cutout) -----------------
        translate([A4_W - WALL - 1, A4_H / 2 - USB_W / 2, -1])
            cube([WALL + 2, USB_W, USB_H + 2]);

        // ---- M3 screw holes (match tray boss positions) -------------------
        for (x = [INSET, A4_W - INSET], y = [INSET, A4_H - INSET])
            translate([x, y, -1]) cylinder(d = SCREW_D, h = FACE + 2);
    }
}

// ---------------------------------------------------------------------------
// Output
// ---------------------------------------------------------------------------
if (PART == "tray")           tray();
else if (PART == "faceplate") faceplate();
else {                         // preview: faceplate floating above tray
    tray();
    translate([0, 0, CAVITY_H + 8]) color("LightGrey") faceplate();
}
