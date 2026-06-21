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
- 2x MAX7219 8x32 dot-matrix panels on a daisy-chained SPI bus — Player A initials + score (left), Player B initials + score (right).
- 6x round-indicator LEDs (3 green for Player A, 3 red for Player B) — light up as rounds are won.
- 4x momentary buttons — UP (+1 point) and DOWN (undo) for each player.
- **Best-of-3 match format:** first to win 2 rounds wins the match; the 3rd LED blinks on match over.
- **Wi-Fi SoftAP captive portal:** connect phone to `Scoreboard-XXXX`, pick game and enter player names — no app required.
- **8 game modes:** Table Tennis, Tennis, Badminton, Free Count, Pickleball, Volleyball, Basketball 21, Cornhole.
- **Non-volatile memory (NVS):** scores, round wins, player names, and selected game survive power-off / dead battery.
- **Full undo:** DOWN button steps back through scoring history, including across round boundaries.
- Power chain spec'd: LiPo → TP4056 USB-C charge → MT3608 5V boost → SPDT master switch.
- Custom A4 enclosure (tray + faceplate) with cutouts for both matrix panels, 6 LEDs, 4 buttons, and USB-C port.

## How to run the simulation
See `simulation/README.md` — paste the sketch + diagram into wokwi.com, add the
required libraries (MD_MAX72xx, MD_Parola), press Play. Click the on-screen
buttons to score.

## To flash on real hardware
1. Arduino IDE → install ESP32 boards + MD_MAX72xx + MD_Parola libraries.
2. Keep all `.ino`, `.h`, and `.cpp` files in the same sketch folder.
3. Wire per `wiring/wiring_table.md`.
4. Select "ESP32 Dev Module", upload `scoreboard.ino`.
5. Open `scoreboard_case.scad` in OpenSCAD, set `PART="tray"` then `PART="faceplate"`, export each as STL, print.

## Optional future add-ons (not in this order)
- Remote scoreboard view via phone browser (the ESP32 Wi-Fi is already active).
- On-screen "match winner" banner / best-of-N tournament mode.
