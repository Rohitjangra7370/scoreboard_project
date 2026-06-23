// ============================================================================
// Smart Scoreboard - Parametric Enclosure  (Rev G — SCALE-driven, large format)
// ----------------------------------------------------------------------------
// Hardware (SCALE=2): 16× MAX7219 16×16 square matrix modules (~70 mm each),
//           4 per field (a 2×2 block = 32×32 px), 2 fields per player stacked:
//           initials over score. → ~12 cm digits, readable to ~18 m.
//           6× 5 mm status LEDs (3 per side), 4× 12 mm push-buttons.
//           Powered from an internal USB power bank (5 V / ≥3 A).
//
// The whole geometry is driven by SCALE (matches `SCALE` in the firmware
// `display_matrix.cpp`): SCALE=1 → 4 modules, ~A4 board; SCALE=2 → 16 modules,
// ~380×346 mm board. Change SCALE and the footprint, windows, frames, and
// component positions all recompute.
//
// Rev F/G assembly features (kept): M3 heat-set corner bosses, elephant's-foot
// chamfer, counterbored faceplate screws, chamfered hole entries, registration
// lip, per-field locating frames, PCB standoffs.
//
// Render the two parts separately for printing:
//   openscad -o faceplate.stl -D 'PART="faceplate"' scoreboard_case.scad
//   openscad -o tray.stl      -D 'PART="tray"'      scoreboard_case.scad
//
// NOTE: at SCALE=2 the board (~380×346 mm) is larger than most print beds —
// tile the faceplate (split in OpenSCAD or your slicer) or build a frame.
//
// Author: Rohit Jangra. Units = millimetres.
// ============================================================================

PART = "faceplate";   // "tray" | "faceplate" | "render" | "preview"

// ---- Display scale (match firmware display_matrix.cpp) ----------------------
SCALE = 2;             // modules per field side: 1 → 16×16 field, 2 → 32×32 field

// ---- MAX7219 16×16 square matrix module ------------------------------------
MAT_MODULE   = 70;                 // physical single-module side (~70 mm)
FIELD        = MAT_MODULE * SCALE; // one display field (block of SCALE×SCALE modules)
MAT_WIN      = FIELD - 2;          // square window (exposes active area, captures block)
MAT_FRAME_T  = 2.4;                // locating-frame wall thickness behind each window
MAT_FRAME_H  = 4.0;                // locating-frame height (lateral capture)

// ---- Layout (margins / gaps) -----------------------------------------------
MARGIN   = 18;         // outer margin around the display block
GAP_X    = 64;         // centre gap between the two columns (holds the LED tally)
GAP_Y    = 8;          // gap between the initials field and the score field
BTN_BAND = 40;         // bottom band height for the push buttons

// ---- Derived footprint ------------------------------------------------------
BOARD_W = 2 * MARGIN + 2 * FIELD + GAP_X;
BOARD_H = BTN_BAND + FIELD + GAP_Y + FIELD + MARGIN;

WALL = 2.4;            // wall thickness
FLOOR = 2.0;           // tray floor thickness
FACE = 2.5;            // faceplate thickness
CAVITY_H = 26;         // internal depth (matrices hang from faceplate; boards on floor)
CORNER_R = 6;          // outer corner radius

// ---- Field / component centres (computed) ----------------------------------
MAT_A_X = MARGIN + FIELD / 2;            // left column centre
MAT_B_X = BOARD_W - MARGIN - FIELD / 2;  // right column centre
SCORE_Y = BTN_BAND + FIELD / 2;          // bottom field centre
INIT_Y  = BTN_BAND + FIELD + GAP_Y + FIELD / 2;  // top field centre

// ---- Assembly / printability -----------------------------------------------
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

// ---- 5 mm status LED holes — central 2×3 tally array -----------------------
// 6 LEDs in the gap BETWEEN the columns: left = 3 green (A), right = 3 red (B).
LED5_D     = 5.2;                     // bore for 5 mm LED body
LED_CX     = BOARD_W / 2;             // centre line between the matrix columns
LED_COL_DX = 12;                      // green (A) left of centre, red (B) right
LED_CY     = (SCORE_Y + INIT_Y) / 2;  // vertical centre of the tally block
LED_ROW_DY = 36;                      // spacing between the 3 rows

// ---- Push-button holes -----------------------------------------------------
BTN_D        = 12.2;   // bore for 12 mm panel momentary button
BTN_SPACING  = 26;
BTN_Y        = BTN_BAND / 2;

// ---- USB cut-out (power bank charge / passthrough) on right side wall -------
USB_W = 14;
USB_H = 8;

// ---- Corner screw bosses ---------------------------------------------------
INSET = 12;            // boss centre inset from corners (clears POST_D/2 = 4)

// ---- PCB standoffs (integrator confirms exact board hole patterns) ---------
// Posts on the tray floor for the ESP32 dev board; the USB power bank sits in
// the central gap / lower band. Positions are computed in the free floor area
// (centre column + lower strip, not behind the matrix blocks). Adjust to your
// exact boards before printing.
PCB_POSTS = [ [LED_CX - 11, LED_CY + 10], [LED_CX + 11, LED_CY + 10],   // ESP32 (~21×30)
              [LED_CX - 11, LED_CY - 20], [LED_CX + 11, LED_CY - 20],
              [LED_CX - 30, BTN_BAND/2],  [LED_CX + 30, BTN_BAND/2] ];  // power-bank tabs

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
module hole_chamfered(d, thickness, z_top) {
    translate([0, 0, z_top - thickness - 1])
        cylinder(d = d, h = thickness + 2);
    translate([0, 0, z_top - CHAMFER_HOLE])
        cylinder(d1 = d, d2 = d + 2 * CHAMFER_HOLE, h = CHAMFER_HOLE + 0.01);
}

// Counterbored screw hole through the faceplate (head flush on the OUTER face z=0).
module faceplate_screw() {
    translate([0, 0, -1]) cylinder(d = SCREW_CLR, h = FACE + 2);
    translate([0, 0, -0.01]) cylinder(d = HEAD_D, h = HEAD_H + 0.01);
}

// Square matrix window centred at (cx, cy).
module mat_window(cx, cy) {
    translate([cx - MAT_WIN / 2, cy - MAT_WIN / 2, -1])
        cube([MAT_WIN, MAT_WIN, FACE + 2]);
}

// Raised locating frame behind a window (faceplate underside) that centres +
// laterally retains the FIELD-sized module block. Inner = block + clearance.
module mat_frame(cx, cy) {
    inner = FIELD + COMP_CLEAR;
    outer = inner + 2 * MAT_FRAME_T;
    translate([0, 0, FACE - 0.01])
        linear_extrude(MAT_FRAME_H)
            difference() {
                translate([cx, cy]) square(outer, center = true);
                translate([cx, cy]) square(inner, center = true);
            }
}

// 6-LED tally as a 2×3 array centred between the matrix columns.
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
STANDOFF_OD   = 6.0;
STANDOFF_H    = 5.0;
STANDOFF_BORE = 2.6;   // M2.5/M3 self-tap
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
            chamfered_body(BOARD_W, BOARD_H, CORNER_R, FLOOR, CHAMFER_BOT);
            translate([0, 0, FLOOR - 0.01]) linear_extrude(CAVITY_H - FLOOR)
                difference() {
                    rrect(BOARD_W, BOARD_H, CORNER_R);
                    offset(-WALL) rrect(BOARD_W, BOARD_H, CORNER_R);
                }
            for (x = [INSET, BOARD_W - INSET], y = [INSET, BOARD_H - INSET])
                translate([x, y, FLOOR - 0.01]) cylinder(d = POST_D, h = CAVITY_H - FLOOR);
            for (p = PCB_POSTS) standoff(p[0], p[1]);
        }
        for (x = [INSET, BOARD_W - INSET], y = [INSET, BOARD_H - INSET])
            translate([x, y, FLOOR + 1.5]) cylinder(d = boss_bore, h = CAVITY_H);
        for (x = [INSET, BOARD_W - INSET], y = [INSET, BOARD_H - INSET])
            translate([x, y, CAVITY_H - CHAMFER_HOLE])
                cylinder(d1 = boss_bore, d2 = boss_bore + 2 * CHAMFER_HOLE, h = CHAMFER_HOLE + 0.01);
        // USB cut-out on right wall
        translate([BOARD_W - WALL - 1, BOARD_H / 2 - USB_W / 2, FLOOR + 4])
            cube([WALL + 2, USB_W, USB_H]);
    }
}

// ---------------------------------------------------------------------------
// Faceplate (top)
// ---------------------------------------------------------------------------
module faceplate() {
    difference() {
        union() {
            chamfered_body(BOARD_W, BOARD_H, CORNER_R, FACE, CHAMFER_BOT);
            translate([0, 0, FACE - 0.01]) linear_extrude(LIP_H)
                difference() {
                    offset(-(WALL + LIP_CLEAR)) rrect(BOARD_W, BOARD_H, CORNER_R);
                    offset(-(WALL + LIP_CLEAR + LIP_T)) rrect(BOARD_W, BOARD_H, CORNER_R);
                }
            mat_frame(MAT_A_X, INIT_Y);  mat_frame(MAT_A_X, SCORE_Y);
            mat_frame(MAT_B_X, INIT_Y);  mat_frame(MAT_B_X, SCORE_Y);
        }
        mat_window(MAT_A_X, INIT_Y);   mat_window(MAT_A_X, SCORE_Y);
        mat_window(MAT_B_X, INIT_Y);   mat_window(MAT_B_X, SCORE_Y);
        led_tally();
        btn_pair(MAT_A_X); btn_pair(MAT_B_X);
        translate([BOARD_W - WALL - 1, BOARD_H / 2 - USB_W / 2, -1])
            cube([WALL + 2, USB_W, USB_H + 2]);
        for (x = [INSET, BOARD_W - INSET], y = [INSET, BOARD_H - INSET])
            translate([x, y, 0]) faceplate_screw();
    }
}

// ---------------------------------------------------------------------------
// Product render — visualization only (NOT a printable part)
// ---------------------------------------------------------------------------
// Rendered for a TOP view (look down +Z): dark matrix face sits just above the
// faceplate, the red "lit" characters sit on top of it (no mirror → read right,
// Player A on the left as on the real board).
module disp_panel(cx, cy, txt) {
    color([0.04, 0.04, 0.05])
        translate([cx, cy, FACE + 0.4]) cube([MAT_WIN - 1, MAT_WIN - 1, 0.8], center = true);
    color([1, 0.18, 0.12])
        translate([cx, cy, FACE + 0.9]) linear_extrude(1.2)
            text(txt, size = MAT_WIN * 0.42, halign = "center", valign = "center",
                 font = "Liberation Sans:style=Bold");
}
module tally_dots() {
    for (j = [-1, 0, 1]) {
        color(j < 1 ? [0.25, 1, 0.35] : [0.12, 0.3, 0.15])
            translate([LED_CX - LED_COL_DX, LED_CY + j * LED_ROW_DY, FACE + 0.5]) cylinder(d = LED5_D, h = 1);
        color(j < 0 ? [1, 0.25, 0.25] : [0.3, 0.12, 0.12])
            translate([LED_CX + LED_COL_DX, LED_CY + j * LED_ROW_DY, FACE + 0.5]) cylinder(d = LED5_D, h = 1);
    }
}
module btn_caps() {
    for (cx = [MAT_A_X, MAT_B_X]) for (i = [-1, 1])
        color([0.13, 0.13, 0.14])
            translate([cx + i * BTN_SPACING / 2, BTN_Y, FACE + 0.5]) cylinder(d = BTN_D, h = 1.5);
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
else {  // exploded preview
    tray();
    translate([0, BOARD_H, CAVITY_H + 18])
        rotate([180, 0, 0])
            color("LightGrey") faceplate();
}
