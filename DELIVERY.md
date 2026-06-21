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
enclosure/   scoreboard_case.scad  parametric 3D-printable case (A4, 2 parts)
```

## Features delivered
- 4× MAX7219 16×16 square matrix modules (~70 mm each) on a single daisy-chained SPI bus — 2 per player, stacked: top square = initials (up to 2 letters), bottom square = score (00–99 or tennis AD). Player A = left column, Player B = right column.
- 6x round-indicator LEDs (3 green for Player A, 3 red for Player B) — light up as rounds are won.
- 4x momentary buttons — UP (+1) and DOWN (-1 point, own player only) for each player.
- **Best-of-3 match format:** first to win 2 rounds wins the match; the 3rd LED blinks on match over.
- **Wi-Fi SoftAP captive portal:** connect phone to `Scoreboard-XXXX`, pick game and enter player names — no app required.
- **8 game modes:** Table Tennis, Tennis, Badminton, Free Count, Pickleball, Volleyball, Basketball 21, Cornhole.
- **Non-volatile memory (NVS):** scores, round wins, player names, and selected game survive power-off / dead battery.
- **Per-player correction:** each player's DOWN button removes one point from **that player only** (never affects the opponent's score).
- Power chain spec'd: LiPo → TP4056 USB-C charge → MT3608 5V boost → SPDT master switch.
- Custom A4 enclosure (tray + faceplate, Rev F) with 4 square ~68 mm windows in a 2×2 grid (top row: A-initials left, B-initials right; bottom row: A-score left, B-score right), plus 6 LED holes, 4 button holes below each player's column, and USB-C charge port cut-out.

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
3. Wire per `wiring/wiring_table.md` (4 modules daisy-chained on GPIO 18/23/5).
4. Select "ESP32 Dev Module", upload `scoreboard.ino`.
5. Open `enclosure/scoreboard_case.scad` (Rev F) in OpenSCAD, set `PART="tray"` then `PART="faceplate"`, export each as STL, print.

## Optional future add-ons (not in this order)
- Remote scoreboard view via phone browser (the ESP32 Wi-Fi is already active).
- On-screen "match winner" banner / best-of-N tournament mode.
