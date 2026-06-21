# Smart A4 Scoreboard

A portable, battery-powered two-player scoreboard built on an ESP32-WROOM-32.
Each player has two MAX7219 16×16 square matrix modules (~70 mm each) arranged
in a vertical stack — the top square shows initials and the bottom square shows
the live score; six indicator LEDs track games won. Game selection and player
names are set from a phone over Wi-Fi (captive portal — no app required).

## Hardware

- **ESP32-WROOM-32** — Wi-Fi SoftAP + game logic
- **4× MAX7219 16×16 square matrix module** (~70 mm, daisy-chained on one SPI chain) — 2 per player stacked: top = initials, bottom = score
- **6× 5 mm LED** (3 green / 3 red) + 220 Ω resistors — round-tally indicators
- **4× momentary push button** — UP (+1) and DOWN (-1 point, own player only) per player
- **Power chain:** 3.7 V LiPo → TP4056 USB-C charger → MT3608 5 V boost → SPDT master switch

## Firmware features

- Best-of-3 match format; round LEDs light per win; 3rd LED blinks on match over
- 8 game modes: Table Tennis, Badminton, Tennis, Free Count, Pickleball,
  Volleyball, Basketball 21, Cornhole
- Per-player point correction (DOWN button removes one point from that player only)
- Non-volatile score storage (NVS) — survives power-off
- Wi-Fi SoftAP captive portal for game selection and player names

## Repo layout

```
firmware/    scoreboard.ino + .h/.cpp modules
simulation/  diagram.json, wokwi.toml, README.md
wiring/      wiring_table.md
enclosure/   scoreboard_case.scad
```

## Docs

- **BOM:** `BOM.md`
- **Wiring:** `wiring/wiring_table.md`
- **Operating guide:** `OPERATING_GUIDE.md`
- **Delivery summary:** `DELIVERY.md`
