// ============================================================================
// Smart A4 Scoreboard - Enclosure  LAYOUT C  (faithful to client sketch)
// ----------------------------------------------------------------------------
//   [on/off]                                      Wi-Fi + BT (built-in)
//   [ AB ]              G   R              [ RB ]   <- small initials displays
//   [    ]              o   o              [    ]
//   [ 4  ]              o   o              [ 2  ]   <- big 2.4" score displays
//   [    ]              o   o              [    ]
//        (o)                                 (o)    <- 2 buttons (A red / B green)
//
// Per player: initials display on top + score display below (both 2.4", same size).
// Central green/red games-won LEDs (3+3 = 6). On/off switch, 2 buttons,
// Wi-Fi + Bluetooth built in.
// Render parts separately:  PART = "tray";  PART = "faceplate";
// Author: Rohit Jangra. Units = millimetres.
// ============================================================================

PART = "faceplate";   // "tray" | "faceplate" | "preview"

A4_W = 297;  A4_H = 210;
WALL = 2.4;  FLOOR = 2.0;  FACE = 2.5;  CAVITY_H = 24;  CORNER_R = 6;

// player columns
COL_L = 78;  COL_R = A4_W - 78;
// initials (top) + score (below) - 2.4" ILI9341, rotated 90 (landscape).
// Real module: active area 36.7 x 49 mm, PCB ~43 x 72 mm.
// Window = active + ~1mm reveal, < PCB so the board body hides behind faceplate.
NM_W = 50;  NM_H = 38;  NM_Y = 152;   // initials, on top
SC_W = 50;  SC_H = 38;  SC_Y = 90;    // score, below
// central LED tally: green column + red column, 3 rows each (6 total)
LED_D = 9;  LED_XG = 139;  LED_XR = 158;  LED_YS = [148, 117, 86];
// on/off switch slot (top-left)
SW_W = 18;  SW_H = 11;  SW_X = 28;  SW_Y = 192;
// two buttons (Player A red / Player B green)
BTN_D = 16;  BTN_Y = 38;  BTN_XS = [COL_L, COL_R];
// USB-C charge port (side wall)
USB_W = 12;  USB_H = 7;
POST_D = 7;  SCREW_D = 3.2;  INSET = 10;
$fn = 56;

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
        translate([A4_W-WALL-1, A4_H/2-USB_W/2, FLOOR+5]) cube([WALL+2, USB_W, USB_H]);
    }
}

module faceplate() {
    difference() {
        linear_extrude(FACE) rrect(A4_W, A4_H, CORNER_R);
        // per player: small initials display on top + big score display below
        for (x = [COL_L, COL_R]) {
            translate([x-NM_W/2, NM_Y-NM_H/2, -1]) cube([NM_W, NM_H, FACE+2]);
            translate([x-SC_W/2, SC_Y-SC_H/2, -1]) cube([SC_W, SC_H, FACE+2]);
        }
        // central LED tally: green col + red col, 3 rows
        for (y = LED_YS) {
            translate([LED_XG, y, -1]) cylinder(d=LED_D, h=FACE+2);
            translate([LED_XR, y, -1]) cylinder(d=LED_D, h=FACE+2);
        }
        // on/off switch slot
        translate([SW_X-SW_W/2, SW_Y-SW_H/2, -1]) cube([SW_W, SW_H, FACE+2]);
        // two buttons
        for (x = BTN_XS) translate([x, BTN_Y, -1]) cylinder(d=BTN_D, h=FACE+2);
        // screw holes
        for (x=[INSET, A4_W-INSET], y=[INSET, A4_H-INSET])
            translate([x,y,-1]) cylinder(d=SCREW_D, h=FACE+2);
    }
}

if (PART == "tray")           tray();
else if (PART == "faceplate") faceplate();
else { tray(); translate([0,0,CAVITY_H+8]) color("LightGrey") faceplate(); }
