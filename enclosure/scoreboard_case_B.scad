// ============================================================================
// Smart A4 Scoreboard - Enclosure  LAYOUT B  (stacked / 2x2)
// ----------------------------------------------------------------------------
// Alternative to Layout A (single row of 4). Here the 4 screens are stacked
// up-and-down in two player columns, with the LED strip running HORIZONTALLY
// in the gap between the upper and lower screens. USB-C charge port on the side.
//
//   [A initials]            [B initials]      <- upper row
//   ===========  LED strip  ===========       <- horizontal strip, centre
//   [A score]               [B score]         <- lower row
//
// Render the two parts separately for printing:
//   PART = "tray";       // F6 -> export STL
//   PART = "faceplate";  // F6 -> export STL
//
// Author: Rohit Jangra. Units = millimetres.
// ============================================================================

PART = "faceplate";   // "tray" | "faceplate" | "preview"

// ---- A4 footprint ----------------------------------------------------------
A4_W = 297;  A4_H = 210;
WALL = 2.4;  FLOOR = 2.0;  FACE = 2.5;  CAVITY_H = 22;  CORNER_R = 6;

// ---- 2.4" ILI9341 module ---------------------------------------------------
DISP_WIN_W = 39;  DISP_WIN_H = 53;

// ---- column / row centres --------------------------------------------------
COL_L = 85;   COL_R = A4_W - 85;        // two player columns (symmetric)
ROW_T = 152;  ROW_B = 58;               // upper / lower screen centres

// ---- horizontal LED strip (centre band) ------------------------------------
LED_W = 100;  LED_H = 12;               // long axis horizontal

// ---- inputs ----------------------------------------------------------------
BTN_D = 30;                              // 30mm arcade buttons, on the wings
BTN_X_L = 38;  BTN_X_R = A4_W - 38;  BTN_Y = A4_H/2;
USB_W = 12;  USB_H = 7;

// ---- mounting --------------------------------------------------------------
POST_D = 7;  SCREW_D = 3.2;  INSET = 10;
$fn = 48;

module rrect(w, h, r) { hull() for (x=[r,w-r], y=[r,h-r]) translate([x,y]) circle(r); }

module tray() {
    difference() {
        union() {
            linear_extrude(FLOOR) rrect(A4_W, A4_H, CORNER_R);
            linear_extrude(CAVITY_H) difference() {
                rrect(A4_W, A4_H, CORNER_R);
                offset(-WALL) rrect(A4_W, A4_H, CORNER_R);
            }
            for (x=[INSET, A4_W-INSET], y=[INSET, A4_H-INSET])
                translate([x,y,0]) cylinder(d=POST_D, h=CAVITY_H);
        }
        for (x=[INSET, A4_W-INSET], y=[INSET, A4_H-INSET])
            translate([x,y,FLOOR]) cylinder(d=SCREW_D, h=CAVITY_H);
        translate([A4_W-WALL-1, A4_H/2-USB_W/2, FLOOR+4]) cube([WALL+2, USB_W, USB_H]);
    }
}

module faceplate() {
    cols = [COL_L, COL_R];
    rows = [ROW_T, ROW_B];
    difference() {
        linear_extrude(FACE) rrect(A4_W, A4_H, CORNER_R);
        // 4 display windows (2 columns x 2 rows)
        for (x=cols, y=rows)
            translate([x-DISP_WIN_W/2, y-DISP_WIN_H/2, -1])
                cube([DISP_WIN_W, DISP_WIN_H, FACE+2]);
        // horizontal LED strip, centre band
        translate([A4_W/2-LED_W/2, A4_H/2-LED_H/2, -1])
            cube([LED_W, LED_H, FACE+2]);
        // two arcade buttons on the wings
        translate([BTN_X_L, BTN_Y, -1]) cylinder(d=BTN_D, h=FACE+2);
        translate([BTN_X_R, BTN_Y, -1]) cylinder(d=BTN_D, h=FACE+2);
        // screw holes
        for (x=[INSET, A4_W-INSET], y=[INSET, A4_H-INSET])
            translate([x,y,-1]) cylinder(d=SCREW_D, h=FACE+2);
    }
}

if (PART == "tray")           tray();
else if (PART == "faceplate") faceplate();
else { tray(); translate([0,0,CAVITY_H+8]) color("LightGrey") faceplate(); }
