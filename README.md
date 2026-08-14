# Smart A4 Scoreboard

A portable, large-format two-player scoreboard built on an ESP32-WROOM-32.
Each player has two **32×32** display fields (each a 2×2 block of MAX7219 16×16
modules) arranged in a vertical stack — the top field shows initials and the
bottom field shows the live score; six indicator LEDs track games won. The big
fields give **~12 cm digits, readable to ~18 m**. Game selection and player
names are set from a phone over Wi-Fi (captive portal — no app required).

> **Display size is set by `SCALE` in `firmware/display_matrix.cpp`.**
> `SCALE 2` (current build) = 16 modules, 64×64 px, ~12 cm digits, ~18 m.
> `SCALE 1` = 4 modules, 32×32 px, ~6 cm digits, ~12 m (smaller/cheaper/battery-light).

## Hardware

- **ESP32-WROOM-32** — Wi-Fi SoftAP + game logic
- **16× MAX7219 16×16 square matrix module** (~70 mm, all daisy-chained on one SPI chain) — 4 per field, 2 fields per player stacked: top = initials, bottom = score
- **6× 5 mm LED** (3 green / 3 red) + 220 Ω resistors — round-tally indicators
- **4× momentary push button** — UP (+1) and DOWN (-1 point, own player only) per player
- **Power:** USB power bank (5 V / ≥3 A) → SPDT master switch → 5 V bus (16 modules draw ~1–3 A; a single LiPo + boost can't supply this — see `BOM.md`)

## Firmware features

- Best-of-5 match format (first to 3 rounds); round LEDs light per win; 3rd LED blinks on match over
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
