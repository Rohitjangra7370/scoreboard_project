# Smart A4 Scoreboard — Delivery Package

Hi Swapna — here's everything for the Smart A4 Scoreboard, built and tested in
simulation. No physical hardware required to verify; you can run it in your
browser in a couple of minutes.

## What's included
```
firmware/    scoreboard.ino        ESP32 firmware (Arduino)
             game_logic.h/.cpp     match / scoring logic
             display_matrix.h/.cpp MAX7219 panel driver
             indicators.h/.cpp     round-LED driver
             input.h/.cpp          button debounce / events
             web_ui.h/.cpp         SoftAP captive-portal setup
simulation/  diagram.json          Wokwi virtual circuit
             wokwi.toml            sim config
             README.md             how to run the simulation
wiring/      wiring_table.md       full pin-by-pin wiring + power chain
enclosure/   scoreboard_case.scad  parametric 3D-printable case (~300mm, 2 parts)
```

## Features delivered
- **16× MAX7219 16×16 square matrix modules** (~70 mm each) on a single daisy-chained SPI bus — **4 per field (2×2 = 32×32 px), 2 fields per player** stacked: top field = initials (up to 2 letters), bottom field = score (00–99 or tennis AD). Player A = left column, Player B = right column. Gives **~12 cm digits, readable to ~18 m** (SCALE=2). Display size is one line in firmware (`SCALE` in `display_matrix.cpp`): SCALE 1 = 4 modules/~6 cm/~12 m, SCALE 2 = 16 modules/~12 cm/~18 m.
- 6x round-indicator LEDs (3 green for Player A, 3 red for Player B) — light up as rounds are won.
- 4x momentary buttons — UP (+1) and DOWN (-1 point, own player only) for each player.
- **Best-of-3 match format:** first to win 2 rounds wins the match; the 3rd LED blinks on match over.
- **Wi-Fi SoftAP captive portal:** connect phone to `Scoreboard-XXXX`, pick game and enter player names — no app required.
- **8 game modes:** Table Tennis, Tennis, Badminton, Free Count, Pickleball, Volleyball, Basketball 21, Cornhole.
- **Non-volatile memory (NVS):** scores, round wins, player names, and selected game survive power-off / dead battery.
- **Per-player correction:** each player's DOWN button removes one point from **that player only** (never affects the opponent's score).
- Power spec'd: USB power bank (5 V / ≥3 A) → SPDT master switch → 5 V bus. 16 modules draw ~1–3 A — too much for a single LiPo + boost, so a power bank is used (~3 h runtime on 10000 mAh). See `BOM.md`.
- Custom enclosure (tray + faceplate) with 4 square ~136 mm windows in a 2×2 grid (top row: A/B initials; bottom row: A/B score), a central 2×3 LED tally between the columns (green = A, red = B), 4 button holes below the columns, registration lip, matrix-locating frames, PCB standoffs, power-bank pocket, and a USB cut-out. **Footprint ~380 × 346 mm (active display ~280 × 280 mm) — bigger than a single print bed; tile the faceplate or use a frame.**

## How to run the simulation
See `simulation/README.md` — paste the sketch + diagram into wokwi.com, press
Play. Click the on-screen buttons to score.

## To flash on real hardware
1. Arduino IDE → install ESP32 boards, then install **one** library:
   - **Adafruit GFX Library** via Arduino Library Manager.
   - **Max72xxPanel** by Mark Ruys (BSD) is **bundled** in the project
     (`firmware/Max72xxPanel.h` / `.cpp`) — no separate install.
2. Keep all `.ino`, `.h`, and `.cpp` files (including `Max72xxPanel.*`) in the
   same sketch folder.
3. Wire per `wiring/wiring_table.md` (16 modules daisy-chained on GPIO 18/23/5; 5 V from power bank).
4. Select "ESP32 Dev Module", upload `scoreboard.ino`.
5. Open `enclosure/scoreboard_case.scad` (Rev F) in OpenSCAD, set `PART="tray"` then `PART="faceplate"`, export each as STL, print.

## Optional future add-ons (not in this order)
- Remote scoreboard view via phone browser (the ESP32 Wi-Fi is already active).
- On-screen "match winner" banner / best-of-N tournament mode.
