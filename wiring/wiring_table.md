# Smart A4 Scoreboard — Wiring / Pin Connection Table

**MCU:** ESP32-WROOM-32 Dev Board
**SPI chain:** four MAX7219 16×16 square matrix modules daisy-chained on one
SPI chain (CLK, DIN, CS shared). Each module contains four 8×8 MAX7219 tiles
(16 tiles total). Assignment: modules 1–2 = Player A (left column, top=initials,
bottom=score); modules 3–4 = Player B (right column, top=initials, bottom=score).

---

## 1. MAX7219 Dot-Matrix Modules (4× 16×16 square, ~70 mm)

| Signal | ESP32 GPIO | Notes |
|--------|-----------|-------|
| CLK (shared) | GPIO 18 | Hardware SPI clock → all 4 modules in parallel |
| DIN (MOSI) | GPIO 23 | SPI data → module 1 DIN |
| CS (shared) | GPIO 5 | Chip select → all 4 modules in parallel |
| Module 1 DOUT | — | Wire to module 2 DIN (daisy-chain) |
| Module 2 DOUT | — | Wire to module 3 DIN (daisy-chain) |
| Module 3 DOUT | — | Wire to module 4 DIN (daisy-chain) |
| Module 1–4 VCC | 5 V (from MT3608) | Each module VCC → 5 V rail |
| Module 1–4 GND | GND | Each module GND → GND |

**Daisy-chain wiring:**
- ESP32 GPIO 23 → module 1 DIN
- Module 1 DOUT → module 2 DIN
- Module 2 DOUT → module 3 DIN
- Module 3 DOUT → module 4 DIN
- CLK (GPIO 18) and CS (GPIO 5) connect to all 4 modules in parallel

**Layout (2×2 grid, front face):**
```
Top row:    [A-initials (mod 1)]  [B-initials (mod 3)]
Bottom row: [A-score    (mod 2)]  [B-score    (mod 4)]
            [A LEDs + buttons ]   [B LEDs + buttons  ]
```

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
| SPDT slide switch | — | master power, on the TP4056 OUT+ rail (not a GPIO) — see §4 |

**Controls** (firmware — full details in `OPERATING_GUIDE.md`):
UP button = +1 point. DOWN button = -1 point to that player only.

---

## 4. Power Chain (no GPIO — hardware only)

```
3.7V LiPo B+ ──► TP4056 B+        [USB-C charge in]
3.7V LiPo B- ──► TP4056 B-
                 │ OUT+ ──► SPDT slide switch ──┐   (switch on the LOAD side)
                 │ OUT- ───────────────────────┐│
                                               ▼▼
                                          MT3608 boost ──► stable 5V rail
                                               │
            ┌──────────────────────────────────┼─────────────────────────┐
            ▼                     ▼            ▼            ▼            ▼
        ESP32 5V/VIN        Mod 1 VCC    Mod 2 VCC    Mod 3 VCC    Mod 4 VCC
```

- **Switch goes on TP4056 OUT+ (load side), NOT between the battery and the TP4056.**
  This keeps the LiPo permanently on the charger's protected B+/B- terminals, so the
  device can still charge while switched off and the cell stays protected.
- TP4056 = safe USB-C LiPo charging + protection.
- MT3608 = boosts sagging 3.7V → steady 5V so the matrix panels never dim.
- ESP32 onboard regulator supplies 3.3V to its own internal logic; the matrix
  panels and LEDs run from the 5V rail.

---

## Avoided pins (don't reuse)
- Strapping: GPIO 0, 2, 12, 15 — left free to avoid boot issues.
- Input-only: GPIO 34–39 — not used for outputs.
- Flash: GPIO 6–11 — never use.
