# Smart A4 Scoreboard — Wiring / Pin Connection Table

**MCU:** ESP32-WROOM-32 Dev Board
**SPI chain:** two MAX7219 8×32 panels daisy-chained — ESP32 drives both panels
from a single SPI chain (CLK, DIN, CS shared; DOUT of panel A feeds DIN of
panel B). Modules 0–3 = Player A, modules 4–7 = Player B.

---

## 1. MAX7219 Dot-Matrix Panels (2× 8×32, 4-in-1 FC16)

| Signal | ESP32 GPIO | Notes |
|--------|-----------|-------|
| CLK (shared) | GPIO 18 | SPI clock → both panels |
| DIN (DATA) | GPIO 23 | SPI data → Panel A DIN |
| CS (shared) | GPIO 5 | Chip select → both panels |
| Panel A DOUT | — | Wire to Panel B DIN (daisy-chain) |
| Panel A VCC | 5 V (from MT3608) | |
| Panel A GND | GND | |
| Panel B VCC | 5 V (from MT3608) | |
| Panel B GND | GND | |

**Daisy-chain wiring:**
- ESP32 GPIO 23 → Panel A DIN
- Panel A DOUT → Panel B DIN
- CLK (GPIO 18) and CS (GPIO 5) connect to both panels in parallel
- Module order: modules 0–3 = Player A, modules 4–7 = Player B

Layout (left → right):
`[A matrix]  [A LEDs]  ||  [B LEDs]  [B matrix]`

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
| Button A-down (Player A undo) | GPIO 33 | INPUT_PULLUP, other leg → GND |
| Button B-up (Player B +1 point) | GPIO 25 | INPUT_PULLUP, other leg → GND |
| Button B-down (Player B undo) | GPIO 13 | INPUT_PULLUP, other leg → GND |
| SPDT slide switch | — | master power, on the TP4056 OUT+ rail (not a GPIO) — see §4 |

**Controls** (firmware — full details in `OPERATING_GUIDE.md`):
UP button = +1 point. DOWN button = undo the last point.

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
            ┌──────────────────────────────────┼───────────────────┐
            ▼                                  ▼                    ▼
        ESP32 5V/VIN                   Panel A VCC             Panel B VCC
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
