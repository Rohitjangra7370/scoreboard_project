# Smart A4 Scoreboard — Wiring / Pin Connection Table

**MCU:** ESP32-WROOM-32 Dev Board
**SPI chain:** sixteen MAX7219 16×16 square matrix modules daisy-chained on one
SPI chain (CLK, DIN, CS shared). Each module contains four 8×8 MAX7219 tiles
(64 tiles total). SCALE=2 build: each of the 4 fields is a 2×2 block of modules
(32×32 px). Chain order = A-initials (mods 1–4), A-score (mods 5–8),
B-initials (mods 9–12), B-score (mods 13–16). Within each field, wire the 4
modules row-major: top-left, top-right, bottom-left, bottom-right.

---

## 1. MAX7219 Dot-Matrix Modules (16× 16×16 square, ~70 mm)

| Signal | ESP32 GPIO | Notes |
|--------|-----------|-------|
| CLK (shared) | GPIO 18 | Hardware SPI clock → all 16 modules in parallel |
| DIN (MOSI) | GPIO 23 | SPI data → module 1 DIN |
| CS (shared) | GPIO 5 | Chip select → all 16 modules in parallel |
| Module n DOUT | — | Wire to module n+1 DIN (daisy-chain, 1→16) |
| Module 1–16 VCC | 5 V (from power bank) | Each module VCC → 5 V rail (thick wire / multiple feeds) |
| Module 1–16 GND | GND | Each module GND → common GND |

**Daisy-chain wiring (one continuous chain of 16):**
- ESP32 GPIO 23 → module 1 DIN
- Module n DOUT → module n+1 DIN, for n = 1…15
- CLK (GPIO 18) and CS (GPIO 5) connect to all 16 modules in parallel
- **Chain order:** mods 1–4 = A-initials field, 5–8 = A-score, 9–12 = B-initials,
  13–16 = B-score. Within each field wire row-major: TL → TR → BL → BR.

> **Power wiring matters at 16 modules.** Don't feed all 16 from one thin daisy
> chain of VCC/GND — voltage will sag at the far end. Run the 5 V and GND as a
> bus (thicker wire) and tap each field's 4 modules from it. Put a 1000 µF cap on
> the 5 V rail at each column.

**Layout (each field = 2×2 modules, front face):**
```
Top row:    [A-initials: mods 1-4]   ●●   [B-initials: mods 9-12 ]
Bottom row: [A-score:    mods 5-8]   ●●   [B-score:    mods 13-16]
                                     ●●
            [ A↑ A↓ ]                      [ B↑ B↓ ]
```
Centre column ●● = the 6 round-tally LEDs (2×3): left = 3 green (A), right =
3 red (B). The 4 buttons sit below each player's matrix column.

---

## 2. Round-Indicator LEDs

Six 5 mm LEDs (one 220 Ω series resistor each): GPIO → 220 Ω → LED anode →
LED cathode → GND.

| Function | ESP32 GPIO |
|----------|-----------|
| LED A round 1 (green — Player A) | GPIO 26 |
| LED A round 2 (green — Player A) | GPIO 27 |
| LED A round 3 (green — Player A) | GPIO 14 |
| LED B round 1 (red — Player B) | GPIO 4 |
| LED B round 2 (red — Player B) | GPIO 16 |
| LED B round 3 (red — Player B) | GPIO 17 |

> 3 green LEDs track Player A games won; 3 red LEDs track Player B games won.
> The 3rd LED on the winning side blinks when the match is over (Best of 3).

---

## 3. Buttons

Four momentary push buttons, normally-open. Each wired: GPIO → button → GND.
Firmware uses INPUT_PULLUP; no external resistor needed.

| Function | ESP32 GPIO | Notes |
|----------|-----------|-------|
| Button A-up (Player A +1 point) | GPIO 32 | INPUT_PULLUP, other leg → GND |
| Button A-down (Player A -1 point) | GPIO 33 | INPUT_PULLUP, other leg → GND |
| Button B-up (Player B +1 point) | GPIO 25 | INPUT_PULLUP, other leg → GND |
| Button B-down (Player B -1 point) | GPIO 13 | INPUT_PULLUP, other leg → GND |
| SPDT slide switch | — | master power, in the 5 V feed from the power-bank tap (not a GPIO) — see §4 |

**Controls** (firmware — full details in `OPERATING_GUIDE.md`):
UP button = +1 point. DOWN button = -1 point to that player only.

---

## 4. Power Chain (no GPIO — hardware only)

```
USB power bank (5V / ≥3A out)
   │  USB-A → screw-terminal/pigtail tap
   ▼
  5V ──► SPDT slide switch ──┐   (switch in the 5V feed)
  GND ───────────────────────┼──────────────────────────────────────┐
                             ▼                                        ▼
                       5V rail (bus, thick wire) ───────────────► common GND
                             │
   ┌───────────┬─────────────┼─────────────┬─────────────┐
   ▼           ▼             ▼             ▼             ▼
ESP32 5V/VIN  A-init 4 mods  A-score 4    B-init 4     B-score 4
              (+1000µF)      (+1000µF)
```

- **Use a USB power bank rated 5 V / ≥3 A.** 16 modules + ESP32 draw ~1–3 A on
  typical content — a single LiPo + MT3608 cannot supply this. The bank gives 5 V
  directly, so no boost converter is needed.
- **Pick an "always-on"/passthrough bank** that won't auto-shut-off at low draw.
- **Switch** goes in the 5 V feed between the tap and the rail (master on/off).
- Run 5 V and GND as a **bus** (thicker wire); tap each field's 4 modules from it
  to avoid voltage sag. One 1000 µF cap per column on the 5 V rail.
- ESP32 onboard regulator supplies 3.3 V to its own logic; modules + LEDs run
  from the 5 V rail.
- *(SCALE=1 alt.)* For the smaller 4-module build, the old LiPo + TP4056 + MT3608
  chain is fine — see the project history.

---

## Avoided pins (don't reuse)
- Strapping: GPIO 0, 2, 12, 15 — left free to avoid boot issues.
- Input-only: GPIO 34–39 — not used for outputs.
- Flash: GPIO 6–11 — never use.
