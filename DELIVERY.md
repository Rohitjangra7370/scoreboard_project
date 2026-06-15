# Smart A4 Scoreboard — Delivery Package

Hi Swapna — here's everything for the Smart A4 Scoreboard, built and tested in
simulation. No physical hardware required to verify; you can run it in your
browser in a couple of minutes.

## What's included
```
firmware/    scoreboard.ino        ESP32 firmware (Arduino)
simulation/  diagram.json          Wokwi virtual circuit (4 displays, LEDs, buttons)
             wokwi.toml            sim config
             README.md             how to run the simulation
wiring/      wiring_table.md       full pin-by-pin wiring + power chain
enclosure/   scoreboard_case.scad  parametric 3D-printable case (A4, 2 parts)
```

## Features delivered
- ✅ 4× 2.4" TFT displays on a shared SPI bus — Player A initials + score, Player B score + initials.
- ✅ WS2812B 8-LED stick — games-won tracker (red from one end, green from the other).
- ✅ 2× arcade buttons via hardware interrupts — instant, debounced scoring.
- ✅ **Win logic:** first to 11, win-by-2 (fully configurable).
- ✅ **Non-volatile memory (NVS):** scores + games survive power-off / dead battery.
- ✅ **Match reset:** hold both buttons ~2 seconds.
- ✅ Power chain spec'd: LiPo → TP4056 USB-C charge → MT3608 5V boost.
- ✅ Custom A4 enclosure (tray + faceplate) with cutouts for all 4 screens, LED column, both buttons, and USB-C port.

## How to run the simulation
See `simulation/README.md` — paste the sketch + diagram into wokwi.com, add the
3 Adafruit libraries, press Play. Click the red/green buttons to score.

## Easy tweaks (top of scoreboard.ino)
- `INITIALS_A` / `INITIALS_B` — player initials shown on screen.
- `WIN_SCORE`, `WIN_BY_TWO` — winning rules.
- `MAX_GAMES` — games shown on the LED stick.

## To flash on real hardware later
1. Arduino IDE → install ESP32 boards + the 3 Adafruit libraries.
2. Wire per `wiring/wiring_table.md`.
3. Select "ESP32 Dev Module", upload `scoreboard.ino`.
4. Open `scoreboard_case.scad` in OpenSCAD, set `PART="tray"` then `PART="faceplate"`, export each as STL, print.

## Optional future add-ons (not in this order)
- Wi-Fi/BLE phone app to set player names + watch live scores (the ESP32 already supports it).
- On-screen "match winner" banner / best-of-N tournament mode.
