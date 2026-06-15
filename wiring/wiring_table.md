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

---

## 3. Buttons + Switch

| Signal | ESP32 GPIO | Notes |
|--------|-----------|-------|
| Red arcade button (Player A +1) | GPIO 32 | INPUT_PULLUP, other leg → GND |
| Green arcade button (Player B +1) | GPIO 33 | INPUT_PULLUP, other leg → GND |
| SPDT slide switch | — | master power, in series with battery + (not a GPIO) |

Hold **both buttons ~2s** = reset match (scores + games + saved memory).

---

## 4. Power Chain (no GPIO — hardware only)

```
3.7V LiPo ──► SPDT slide switch ──► TP4056 (B+/B-)  [USB-C charge in]
                                       │ OUT+ / OUT-
                                       ▼
                                   MT3608 boost ──► stable 5V rail
                                       │
            ┌──────────────────────────┼───────────────────┐
            ▼                          ▼                   ▼
        ESP32 5V/VIN            WS2812B 5V          (TFT via 3V3 from ESP32 reg)
```

- TP4056 = safe USB-C LiPo charging + protection.
- MT3608 = boosts sagging 3.7V → steady 5V so screens/LEDs never dim.
- ESP32 onboard regulator supplies 3V3 to the TFTs.

---

## Avoided pins (don't reuse)
- Strapping: GPIO 0, 2, 12, 15 — left free to avoid boot issues.
- Input-only: GPIO 34–39 — not used for outputs.
- Flash: GPIO 6–11 — never use.
