# Smart A4 Scoreboard — Wiring / Pin Connection Table

**MCU:** ESP32-WROOM-32 Dev Board
**SPI bus:** VSPI (hardware) — all 4 TFTs share SCLK + MOSI; each has its own CS. DC and RST are shared.

---

## 1. TFT Displays (4× 2.4" ILI9341, SPI)

| Signal | ESP32 GPIO | Notes |
|--------|-----------|-------|
| SCLK (shared) | GPIO 18 | VSPI clock → all 4 displays |
| MOSI (shared) | GPIO 23 | VSPI data → all 4 displays |
| MISO | — | not used (write-only) |
| DC (shared) | GPIO 21 | Data/Command, all 4 displays |
| RST (shared) | GPIO 22 | Reset, all 4 displays |
| CS — Player A Initials | GPIO 5 | display 1 |
| CS — Player A Score | GPIO 13 | display 2 |
| CS — Player B Score | GPIO 14 | display 3 |
| CS — Player B Initials | GPIO 4 | display 4 |
| LED (backlight) | 3V3 | tie all backlights to 3V3 (or one GPIO for dimming) |
| VCC | 3V3 | |
| GND | GND | common ground |

Layout (left → right):
`[A Initials][A Score]  ||LED column||  [B Score][B Initials]`

---

## 2. WS2812B NeoPixel 8-LED Stick (games-won column)

| Signal | ESP32 GPIO | Notes |
|--------|-----------|-------|
| DIN | GPIO 25 | data in (add 330–470Ω series resistor on real hardware) |
| 5V | 5V (from MT3608) | |
| GND | GND | |

> Put a 1000µF cap across 5V/GND near the stick on real hardware.
>
> **3.3 V data note:** GPIO25 outputs 3.3 V into the 5 V WS2812B `DIN`. The first
> pixel usually accepts this, but it's out of spec. If the LEDs glitch, add a
> 74AHCT125 / level shifter on the data line, or power the stick from ~4.3 V
> (one diode drop off 5V) so 3.3 V clears its logic-high threshold.

---

## 3. Buttons + Switch

| Signal | ESP32 GPIO | Notes |
|--------|-----------|-------|
| Red arcade button (Player A +1) | GPIO 32 | INPUT_PULLUP, other leg → GND |
| Green arcade button (Player B +1) | GPIO 33 | INPUT_PULLUP, other leg → GND |
| SPDT slide switch | — | master power, on the TP4056 OUT+ rail (not a GPIO) — see §4 |

**Controls** (firmware — full details in `OPERATING_GUIDE.md`):
Red/Green tap = +1. Red long-press = undo. Green long-press = reset match.
Hold **both** ~2 s = open/close the game menu.

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
        ESP32 5V/VIN                    WS2812B 5V          (TFT 3V3 via ESP32 reg)
```

- **Switch goes on TP4056 OUT+ (load side), NOT between the battery and the TP4056.**
  This keeps the LiPo permanently on the charger's protected B+/B- terminals, so the
  device can still charge while switched off and the cell stays protected.
- TP4056 = safe USB-C LiPo charging + protection.
- MT3608 = boosts sagging 3.7V → steady 5V so screens/LEDs never dim.
- ESP32 onboard regulator supplies 3V3 to the TFTs.

---

## Avoided pins (don't reuse)
- Strapping: GPIO 0, 2, 12, 15 — left free to avoid boot issues.
- Input-only: GPIO 34–39 — not used for outputs.
- Flash: GPIO 6–11 — never use.
