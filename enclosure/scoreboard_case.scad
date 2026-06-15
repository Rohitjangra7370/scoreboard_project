// ============================================================================
// Smart A4 Scoreboard - Parametric Enclosure
// ----------------------------------------------------------------------------
// A4-footprint, two-part (tray + faceplate) 3D-printable case.
// Layout (landscape A4, 297 x 210 mm):
//   [A initials][A score]  || LED column ||  [B score][B initials]
//
// Render the two parts separately for printing:
//   PART = "tray";       // F6 -> export STL
//   PART = "faceplate";  // F6 -> export STL
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

// ---- 2.4" ILI9341 module --------------------------------------------------
DISP_PCB_W = 50;       // module board width
DISP_PCB_H = 72;       // module board height
DISP_WIN_W = 39;       // visible glass window width
DISP_WIN_H = 53;       // visible glass window height
DISP_GAP   = 6;        // gap between display windows

// ---- NeoPixel stick (centre column) ---------------------------------------
LED_W = 10;            // stick window width
LED_H = 90;            // stick window height

// ---- Inputs ----------------------------------------------------------------
BTN_D = 30;            // 30mm arcade button bore
BTN_Y = 40;            // button centre height from bottom edge
USB_W = 12;            // USB-C cutout (TP4056) on side wall
USB_H = 7;

// ---- Mounting -------------------------------------------------------------
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

// X centres of the 4 displays + the LED column, laid symmetrically.
function disp_centres() =
    let (unit = DISP_PCB_W + DISP_GAP,
         total = 4 * DISP_PCB_W + 3 * DISP_GAP + LED_W + DISP_GAP,
         x0 = (A4_W - total) / 2 + DISP_PCB_W / 2)
    [ x0,
      x0 + unit,
      x0 + 2 * unit + LED_W + DISP_GAP,
      x0 + 3 * unit + LED_W + DISP_GAP ];

function led_centre_x() =
    let (c = disp_centres()) (c[1] + c[2]) / 2;

// ---------------------------------------------------------------------------
// Tray (bottom): floor + perimeter walls + screw bosses
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
            // screw bosses
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
// Faceplate (top): A4 panel with display windows, LED slot, buttons
// ---------------------------------------------------------------------------
module faceplate() {
    cx = disp_centres();
    difference() {
        linear_extrude(FACE) rrect(A4_W, A4_H, CORNER_R);

        // 4 display windows (centred vertically)
        for (x = cx)
            translate([x - DISP_WIN_W / 2, A4_H / 2 - DISP_WIN_H / 2, -1])
                cube([DISP_WIN_W, DISP_WIN_H, FACE + 2]);

        // central LED column slot
        translate([led_centre_x() - LED_W / 2, A4_H / 2 - LED_H / 2, -1])
            cube([LED_W, LED_H, FACE + 2]);

        // two arcade buttons, under the inner score displays
        translate([cx[1], BTN_Y, -1]) cylinder(d = BTN_D, h = FACE + 2);
        translate([cx[2], BTN_Y, -1]) cylinder(d = BTN_D, h = FACE + 2);

        // screw holes
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
