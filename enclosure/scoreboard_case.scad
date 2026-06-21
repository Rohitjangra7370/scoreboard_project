// ============================================================================
// Smart A4 Scoreboard - Parametric Enclosure  (Rev D — LED-array pivot)
// ----------------------------------------------------------------------------
// Hardware: 2× MAX7219 FC-16 "4-in-1" 8×32 matrix modules (one per player),
//           6× 5 mm status LEDs (3 per side, best-of-3 tally),
//           4× 12 mm panel momentary push-buttons (2 per player).
//
// A4-footprint, two-part (tray + faceplate) 3D-printable case.
// Layout (landscape A4, 297 × 210 mm):
//   [A matrix ][A LEDs+Btns] || [B LEDs+Btns][B matrix ]
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

// ---- MAX7219 FC-16 4-in-1 matrix module ------------------------------------
// Active LED area: 4 chained 8×8 blocks, each 32×32 mm → 128 mm × 32 mm.
// Window adds a 1 mm bezel margin on each side.
MAT_WIN_W   = 130;     // matrix window width  (128 + 1 mm margin each side)
MAT_WIN_H   = 34;      // matrix window height ( 32 + 1 mm margin each side)
MAT_WIN_Y   = A4_H / 2 + 10;   // window centre Y (upper half — score display)

// Horizontal placement: each matrix window is centred in its player half,
// with MARGIN_X inset from the outer edge to allow wall + boss clearance.
// Boss outer radius = POST_D/2 = 3.5 mm at x=INSET=10 → left edge of A
// window must exceed 10+3.5 = 13.5 mm; margin=15 gives 5 mm clearance.
MAT_MARGIN_X = 15;     // inset from A4 left/right edge to window left edge

// Player A matrix centre X (left half)
MAT_A_X = MAT_MARGIN_X + MAT_WIN_W / 2;   // = 80 mm
// Player B matrix centre X (right half) — mirror
MAT_B_X = A4_W - MAT_MARGIN_X - MAT_WIN_W / 2;  // = 217 mm
// Gap between A right edge (145) and B left edge (152) = 7 mm

// ---- 5 mm status LED holes (best-of-3 tally) -------------------------------
LED5_D       = 5.2;    // bore for 5 mm LED body
LED5_SPACING = 10;     // centre-to-centre spacing in the row of 3
LED5_Y       = MAT_WIN_Y - MAT_WIN_H / 2 - 14;  // below the matrix window
// LED row X centres (3 LEDs per player, centred under their matrix window)
LED5_A_X = MAT_A_X;   // mid LED aligns with matrix centre
LED5_B_X = MAT_B_X;

// ---- Panel push-button holes -----------------------------------------------
BTN_D        = 12.2;   // bore for 12 mm panel momentary button
BTN_SPACING  = 20;     // centre-to-centre between the +/- pair
BTN_Y        = LED5_Y - 18;  // below the LED row

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
// Faceplate (top): A4 panel — Rev D (LED-array pivot)
//   • 2× matrix windows (one per player)
//   • 6× 5 mm LED holes (3 per side)
//   • 4× 12 mm button holes (2 per player)
//   • USB-C pass-through slot (matches tray)
//   • 4× M3 screw holes (match tray bosses)
// ---------------------------------------------------------------------------
module faceplate() {
    difference() {
        // solid A4 panel
        linear_extrude(FACE) rrect(A4_W, A4_H, CORNER_R);

        // ---- Player A matrix window (left) ---------------------------------
        translate([MAT_A_X - MAT_WIN_W / 2,
                   MAT_WIN_Y - MAT_WIN_H / 2, -1])
            cube([MAT_WIN_W, MAT_WIN_H, FACE + 2]);

        // ---- Player B matrix window (right) --------------------------------
        translate([MAT_B_X - MAT_WIN_W / 2,
                   MAT_WIN_Y - MAT_WIN_H / 2, -1])
            cube([MAT_WIN_W, MAT_WIN_H, FACE + 2]);

        // ---- Player A: 3 status LED holes (centred under A matrix) ---------
        for (i = [-1, 0, 1])
            translate([LED5_A_X + i * LED5_SPACING, LED5_Y, -1])
                cylinder(d = LED5_D, h = FACE + 2);

        // ---- Player B: 3 status LED holes (centred under B matrix) ---------
        for (i = [-1, 0, 1])
            translate([LED5_B_X + i * LED5_SPACING, LED5_Y, -1])
                cylinder(d = LED5_D, h = FACE + 2);

        // ---- Player A: 2 push-button holes (+/- score) ---------------------
        for (i = [-1, 1])
            translate([MAT_A_X + i * BTN_SPACING / 2, BTN_Y, -1])
                cylinder(d = BTN_D, h = FACE + 2);

        // ---- Player B: 2 push-button holes (+/- score) ---------------------
        for (i = [-1, 1])
            translate([MAT_B_X + i * BTN_SPACING / 2, BTN_Y, -1])
                cylinder(d = BTN_D, h = FACE + 2);

        // ---- USB-C slot (right edge, matches tray cutout) ------------------
        translate([A4_W - WALL - 1, A4_H / 2 - USB_W / 2, -1])
            cube([WALL + 2, USB_W, USB_H + 2]);

        // ---- M3 screw holes (match tray boss positions) --------------------
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
