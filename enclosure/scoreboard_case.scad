// ============================================================================
// Smart A4 Scoreboard - Parametric Enclosure  (Rev F — assembly-ready)
// ----------------------------------------------------------------------------
// Hardware: 4× MAX7219 16×16 square matrix modules (~70 mm), stacked 2 per
//           player: initials over score.
//           6× 5 mm status LEDs (3 per side), 4× 12 mm push-buttons.
//
// Rev F adds the features needed to actually assemble a clean product:
//   • M3 heat-set-insert corner bosses (4.0 mm bore, 8 mm OD ≥ 2× insert OD)
//   • elephant's-foot chamfer on the printed-down outer edges (0.5 mm)
//   • counterbored faceplate screw holes (M3 heads sit flush)
//   • chamfered LED / button / screw hole entries (self-aligning, clean)
//   • faceplate registration lip that nests into the tray (0.4 mm clearance)
//   • locating frames behind each matrix window (centre + retain the modules)
//   • PCB standoffs on the tray floor for the ESP32 + power boards
//
// Sources for the numbers: InsertGuide (M3 heat-set hole/boss), Hydra Research
// & Protolabs/Hubs (enclosure chamfers, lips, 0.5 mm component clearance),
// Sovol (apply clearance to the hole, not the peg).
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
CAVITY_H = 26;         // internal depth (matrices hang from faceplate; boards on floor)
CORNER_R = 6;          // outer corner radius

// ---- Assembly / printability (Rev F) ---------------------------------------
USE_HEATSET  = true;   // true: bosses bored 4.0 mm for M3 heat-set inserts
                       // false: bored 2.6 mm for M3 self-tapping screws
INSERT_BORE  = 4.0;    // M3 heat-set insert hole  (InsertGuide: 4.0–4.2)
SELFTAP_BORE = 2.6;    // M3 self-tap into plastic
POST_D       = 8.0;    // corner boss OD (≥ 2× insert OD ≈ 4.6 mm)
CHAMFER_BOT  = 0.5;    // elephant's-foot chamfer on printed-down outer edge
CHAMFER_HOLE = 0.6;    // 45° countersink on hole entries
SCREW_CLR    = 3.2;    // M3 shank clearance through the faceplate
HEAD_D       = 6.2;    // M3 head counterbore dia (flush heads)
HEAD_H       = 1.8;    // counterbore depth
LIP_T        = 1.6;    // registration-lip wall thickness
LIP_H        = 4.0;    // registration-lip depth (long engagement, self-aligns)
LIP_CLEAR    = 0.4;    // lip-to-inner-wall clearance (snug but removable)
COMP_CLEAR   = 0.5;    // clearance around internal components

// ---- MAX7219 16×16 square matrix module ------------------------------------
MAT_MODULE   = 70;     // physical module side (~70 mm)
MAT_WIN      = 68;     // square window side (exposes active area, captures module)
MAT_FRAME_T  = 2.4;    // locating-frame wall thickness behind each window
MAT_FRAME_H  = 4.0;    // locating-frame height (lateral capture of the module)

MAT_A_X = 78;                 // left column centre  (window 44..112)
MAT_B_X = A4_W - 78;          // right column centre (window 185..253) = 219
INIT_Y  = 166;                // top window  132..200
SCORE_Y = 88;                 // bottom window 54..122

// ---- 5 mm status LED holes — central 2×3 tally array -----------------------
// 6 LEDs grouped in the gap BETWEEN the two matrix columns (like the original
// sketch): left column = 3 green (Player A), right column = 3 red (Player B).
LED5_D     = 5.2;          // bore for 5 mm LED body
LED_CX     = A4_W / 2;     // centre line between the matrix columns (148.5)
LED_COL_DX = 9;            // green (A) column left of centre, red (B) right
LED_CY     = 127;          // vertical centre of the tally block
LED_ROW_DY = 24;           // spacing between the 3 rows

// ---- Push-button holes -----------------------------------------------------
BTN_D        = 12.2;   // bore for 12 mm panel momentary button
BTN_SPACING  = 20;
BTN_Y        = 22;

// ---- USB-C charge port (TP4056) on right side wall -------------------------
USB_W = 12;
USB_H = 7;

// ---- Corner screw bosses ---------------------------------------------------
INSET = 12;            // boss centre inset from corners (clears POST_D/2 = 4)

// ---- PCB standoffs (integrator confirms exact board hole patterns) ---------
// Posts on the tray floor for the ESP32 dev board + TP4056 + MT3608. The XY
// positions below sit in the free floor area (the central column + lower strip,
// not behind the matrix modules). Adjust to your exact boards before printing.
STANDOFF_OD   = 6.0;
STANDOFF_H    = 5.0;
STANDOFF_BORE = 2.6;   // M2.5/M3 self-tap
// [x, y] post centres — central strip (ESP32) + lower corners (power boards)
PCB_POSTS = [ [138, 150], [159, 150], [138, 120], [159, 120],   // ESP32 (~21×30)
              [130, 40],  [165, 40] ];                          // TP4056 / MT3608

$fn = 48;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
module rrect(w, h, r) {
    hull() for (x = [r, w - r], y = [r, h - r]) translate([x, y]) circle(r);
}

// Body with a 45° elephant's-foot chamfer on the bottom (z=0) outer edge.
module chamfered_body(w, h, r, height, cham) {
    hull() {
        linear_extrude(0.01) offset(-cham) rrect(w, h, r);
        translate([0, 0, cham]) linear_extrude(0.01) rrect(w, h, r);
    }
    translate([0, 0, cham]) linear_extrude(height - cham) rrect(w, h, r);
}

// A through hole with a chamfered (countersunk) entry at its top face.
// z_top = the surface the chamfer opens onto; bores downward (−Z).
module hole_chamfered(d, thickness, z_top) {
    translate([0, 0, z_top - thickness - 1])
        cylinder(d = d, h = thickness + 2);
    // top chamfer cone
    translate([0, 0, z_top - CHAMFER_HOLE])
        cylinder(d1 = d, d2 = d + 2 * CHAMFER_HOLE, h = CHAMFER_HOLE + 0.01);
}

// Counterbored screw hole through the faceplate. The OUTER (visible) face is
// z=0; the lip + matrix frames are on the INNER face (z=FACE) that goes into
// the tray. So the head counterbore opens on the OUTER face → heads sit flush
// on the visible side, threads reach down into the tray bosses.
module faceplate_screw() {
    translate([0, 0, -1]) cylinder(d = SCREW_CLR, h = FACE + 2);          // shank through
    translate([0, 0, -0.01]) cylinder(d = HEAD_D, h = HEAD_H + 0.01);     // head on outer face (z=0)
}

// Square matrix window centred at (cx, cy).
module mat_window(cx, cy) {
    translate([cx - MAT_WIN / 2, cy - MAT_WIN / 2, -1])
        cube([MAT_WIN, MAT_WIN, FACE + 2]);
}

// Raised locating frame behind a window (on the faceplate underside) that
// centres + laterally retains the ~70 mm module. Inner = module + clearance.
module mat_frame(cx, cy) {
    inner = MAT_MODULE + COMP_CLEAR;          // 70.5
    outer = inner + 2 * MAT_FRAME_T;
    translate([0, 0, FACE - 0.01])
        linear_extrude(MAT_FRAME_H)
            difference() {
                translate([cx, cy]) square(outer, center = true);
                translate([cx, cy]) square(inner, center = true);
            }
}

// 6-LED tally as a 2×3 array centred between the matrix columns:
// left column = 3 green (A), right column = 3 red (B).
module led_tally() {
    for (j = [-1, 0, 1]) {
        translate([LED_CX - LED_COL_DX, LED_CY + j * LED_ROW_DY, FACE])   // A (green)
            hole_chamfered(LED5_D, FACE, FACE);
        translate([LED_CX + LED_COL_DX, LED_CY + j * LED_ROW_DY, FACE])   // B (red)
            hole_chamfered(LED5_D, FACE, FACE);
    }
}
module btn_pair(cx) {
    for (i = [-1, 1])
        translate([cx + i * BTN_SPACING / 2, BTN_Y, FACE])
            hole_chamfered(BTN_D, FACE, FACE);
}

// Tray PCB standoff with a self-tap bore.
module standoff(x, y) {
    translate([x, y, FLOOR]) difference() {
        cylinder(d = STANDOFF_OD, h = STANDOFF_H);
        translate([0, 0, -0.5]) cylinder(d = STANDOFF_BORE, h = STANDOFF_H + 1);
    }
}

// ---------------------------------------------------------------------------
// Tray (bottom)
// ---------------------------------------------------------------------------
module tray() {
    boss_bore = USE_HEATSET ? INSERT_BORE : SELFTAP_BORE;
    difference() {
        union() {
            // floor with elephant's-foot chamfer on the printed-down edge
            chamfered_body(A4_W, A4_H, CORNER_R, FLOOR, CHAMFER_BOT);
            // perimeter wall
            translate([0, 0, FLOOR - 0.01]) linear_extrude(CAVITY_H - FLOOR)
                difference() {
                    rrect(A4_W, A4_H, CORNER_R);
                    offset(-WALL) rrect(A4_W, A4_H, CORNER_R);
                }
            // corner screw bosses (heat-set / self-tap)
            for (x = [INSET, A4_W - INSET], y = [INSET, A4_H - INSET])
                translate([x, y, FLOOR - 0.01]) cylinder(d = POST_D, h = CAVITY_H - FLOOR);
            // PCB standoffs
            for (p = PCB_POSTS) standoff(p[0], p[1]);
        }
        // boss bores (from the top, stop short of the floor)
        for (x = [INSET, A4_W - INSET], y = [INSET, A4_H - INSET])
            translate([x, y, FLOOR + 1.5]) cylinder(d = boss_bore, h = CAVITY_H);
        // chamfer the boss bore mouths for easy insert start
        for (x = [INSET, A4_W - INSET], y = [INSET, A4_H - INSET])
            translate([x, y, CAVITY_H - CHAMFER_HOLE])
                cylinder(d1 = boss_bore, d2 = boss_bore + 2 * CHAMFER_HOLE, h = CHAMFER_HOLE + 0.01);
        // USB-C charge port on right wall
        translate([A4_W - WALL - 1, A4_H / 2 - USB_W / 2, FLOOR + 4])
            cube([WALL + 2, USB_W, USB_H]);
    }
}

// ---------------------------------------------------------------------------
// Faceplate (top)
// ---------------------------------------------------------------------------
module faceplate() {
    difference() {
        union() {
            // panel: outer face printed down → chamfer that edge
            chamfered_body(A4_W, A4_H, CORNER_R, FACE, CHAMFER_BOT);
            // registration lip on the underside (nests inside the tray walls)
            translate([0, 0, FACE - 0.01]) linear_extrude(LIP_H)
                difference() {
                    offset(-(WALL + LIP_CLEAR)) rrect(A4_W, A4_H, CORNER_R);
                    offset(-(WALL + LIP_CLEAR + LIP_T)) rrect(A4_W, A4_H, CORNER_R);
                }
            // matrix locating frames (centre + retain each module)
            mat_frame(MAT_A_X, INIT_Y);  mat_frame(MAT_A_X, SCORE_Y);
            mat_frame(MAT_B_X, INIT_Y);  mat_frame(MAT_B_X, SCORE_Y);
        }
        // 4 square matrix windows
        mat_window(MAT_A_X, INIT_Y);   mat_window(MAT_A_X, SCORE_Y);
        mat_window(MAT_B_X, INIT_Y);   mat_window(MAT_B_X, SCORE_Y);
        // 6-LED central tally + 4 push buttons (chamfered entries)
        led_tally();
        btn_pair(MAT_A_X); btn_pair(MAT_B_X);
        // USB-C slot (matches tray)
        translate([A4_W - WALL - 1, A4_H / 2 - USB_W / 2, -1])
            cube([WALL + 2, USB_W, USB_H + 2]);
        // counterbored corner screw holes (heads flush on the top face)
        for (x = [INSET, A4_W - INSET], y = [INSET, A4_H - INSET])
            translate([x, y, 0]) faceplate_screw();
    }
}

// ---------------------------------------------------------------------------
// Product render — visualization only (NOT a printable part)
// Shows the assembled front: white faceplate, the 4 matrices "lit" red with
// initials + score, the 6-LED tally, and the buttons.
// ---------------------------------------------------------------------------
module disp_panel(cx, cy, txt) {
    color([0.04, 0.04, 0.05])                          // dark matrix face (behind window)
        translate([cx, cy, FACE + 1]) cube([MAT_WIN - 1, MAT_WIN - 1, 2], center = true);
    color([1, 0.18, 0.12])                             // red "lit" characters, in front of it
        translate([cx, cy, FACE - 1.6]) mirror([1, 0, 0]) linear_extrude(1.5)
            text(txt, size = 40, halign = "center", valign = "center",
                 font = "Liberation Sans:style=Bold");
}
module tally_dots() {
    for (j = [-1, 0, 1]) {
        color(j < 1 ? [0.25, 1, 0.35] : [0.12, 0.3, 0.15])     // 2 green lit (A won 2)
            translate([LED_CX - LED_COL_DX, LED_CY + j * LED_ROW_DY, -1]) cylinder(d = LED5_D, h = 2);
        color(j < 0 ? [1, 0.25, 0.25] : [0.3, 0.12, 0.12])     // 1 red lit (B won 1)
            translate([LED_CX + LED_COL_DX, LED_CY + j * LED_ROW_DY, -1]) cylinder(d = LED5_D, h = 2);
    }
}
module btn_caps() {
    for (cx = [MAT_A_X, MAT_B_X]) for (i = [-1, 1])
        color([0.13, 0.13, 0.14])
            translate([cx + i * BTN_SPACING / 2, BTN_Y, -2]) cylinder(d = BTN_D, h = 3);
}
module product_render() {
    color([0.92, 0.92, 0.95]) faceplate();
    disp_panel(MAT_A_X, INIT_Y,  "AB");
    disp_panel(MAT_A_X, SCORE_Y, "07");
    disp_panel(MAT_B_X, INIT_Y,  "RB");
    disp_panel(MAT_B_X, SCORE_Y, "12");
    tally_dots();
    btn_caps();
}

// ---------------------------------------------------------------------------
// Output
// ---------------------------------------------------------------------------
if (PART == "tray")           tray();
else if (PART == "faceplate") faceplate();
else if (PART == "render")    product_render();
else {  // exploded preview: faceplate flipped into assembled orientation above
        // the tray — outer face up, registration lip + matrix frames pointing
        // DOWN into the open tray (the way it actually drops on).
    tray();
    translate([0, A4_H, CAVITY_H + 18])
        rotate([180, 0, 0])
            color("LightGrey") faceplate();
}
