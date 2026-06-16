# Smart A4 Scoreboard — Bill of Materials (BOM)

Everything needed to build one unit. Section 1 is the core electronics stack
(as specified). Section 2 lists the passives and build extras that real hardware
needs but that aren't headline components. Section 3 is the enclosure.

Pin assignments for every part are in `wiring/wiring_table.md`.

---

## 1. Core electronics

| # | Component | Spec / part | Qty | Notes |
|---|-----------|-------------|-----|-------|
| 1 | Microcontroller | **ESP32-WROOM-32** dev board (30/38-pin) | 1 | The brain. Drives all 4 displays over one SPI bus + reads buttons. Wi-Fi/BLE capable. |
| 2 | Display | **2.4" ILI9341 TFT LCD, SPI** (240×320) | 4 | Player A initials + score, Player B score + initials. Shared SCLK/MOSI/DC/RST, separate CS each. |
| 3 | RGB LED stick | **WS2812B "NeoPixel", 8 LEDs** | 1 | Centre column: games-won tally + F1 reaction-game start lights. |
| 4 | Buttons | **30 mm arcade button** (1× red, 1× green) | 2 | Score / control inputs. Momentary, normally-open. |
| 5 | Power switch | **SPDT slide switch** (heavy-duty) | 1 | Master on/off. Wire on the TP4056 **OUT+** rail (see wiring §4). |
| 6 | Battery | **3.7 V Li-ion / LiPo**, ~1200–2000 mAh | 1 | Portable power. Single cell. |
| 7 | Charge controller | **TP4056, USB-C** charging module (with protection) | 1 | Safe USB-C LiPo charging + over-discharge protection. |
| 8 | Boost converter | **MT3608** step-up (set to 5 V out) | 1 | Boosts 3.7 V → steady 5 V so screens/LEDs never dim. Set the trimpot to 5.0 V before connecting. |

---

## 2. Passives & build extras (recommended)

These keep the build reliable; quantities are per unit.

| # | Item | Spec | Qty | Notes |
|---|------|------|-----|-------|
| 9  | Resistor | **330–470 Ω**, ¼ W | 1 | In series on the WS2812B `DIN` data line (GPIO25). |
| 10 | Capacitor | **1000 µF**, ≥6.3 V electrolytic | 1 | Across the 5 V rail near the LED stick (inrush smoothing). |
| 11 | Level shifter *(optional)* | **74AHCT125** (or similar) | 1 | Only if the LEDs glitch — shifts 3.3 V data → 5 V. Often not needed for the first pixel. |
| 12 | Hook-up wire | 24–26 AWG, silicone | — | Display/button/power wiring. |
| 13 | Protoboard or PCB | Perfboard, ~5×7 cm | 1 | To mount the ESP32 + TP4056 + MT3608 + junctions. |
| 14 | Pin headers / DuPont | 2.54 mm | — | For the dev board + display connections (optional, can solder direct). |
| 15 | USB-C cable | Standard | 1 | Charging (usually user-supplied). |

---

## 3. Enclosure (3D printed)

| # | Item | Spec | Qty | Notes |
|---|------|------|-----|-------|
| 16 | Case — tray + faceplate | 3D print from `enclosure/scoreboard_case.scad` (export `tray.stl` + `faceplate.stl`) | 1 set | A4 footprint, cutouts for 4 screens, the LED column, 2 buttons, USB-C port. |
| 17 | Filament | PLA or PETG (PETG if heat/impact matters) | ~150–250 g | For both parts. |
| 18 | Screws | **M3 × ~8 mm** | 4 | Faceplate → tray, into the 4 screw bosses. |
| 19 | Heat-set inserts *(optional)* | M3 brass | 4 | Cleaner threads than screwing straight into plastic. |

---

## Compatibility & US sourcing

All parts are common and stocked in the USA (Amazon, Adafruit, SparkFun, Micro
Center, DigiKey, Mouser). Read these before ordering:

- **⚠️ Displays — buy the SPI version, NOT parallel.** The ILI9341 is sold in two
  interfaces. This project uses **SPI** (a small ~9–14-pin header: VCC, GND, CS,
  RST, DC, SDI/MOSI, SCK, LED, SDO/MISO). The common cheap "red 2.4″" board is
  usually the **8-bit parallel** type (40-pin) and will **not** work with this
  firmware. When ordering, confirm the listing says **"SPI"** and shows the
  9–14-pin header.
- **NeoPixel stick:** any **WS2812B** (or **SK6812** — same protocol/brightness)
  8-LED stick works with the `Adafruit NeoPixel` library. Adafruit's own stick now
  ships as SK6812; clones are typically WS2812B. Either is fine.
- **Logic levels:** everything runs at 3.3 V except the WS2812B data line (5 V part
  driven by 3.3 V GPIO) — see the data-line note in `wiring_table.md`.
- **⚠️ Display power:** four TFT backlights pulling from the ESP32 dev board's
  on-board 3.3 V regulator is marginal and can run hot. For a reliable build, feed
  the four displays' VCC from a **dedicated 3.3 V supply** (a small AMS1117-3.3 or
  buck module off the 5 V rail) rather than the ESP32's 3V3 pin.
- **Boost sizing:** keep the MT3608 set to 5.0 V; total peak load (4 displays +
  ESP32 + LEDs) is roughly ≤1 A with the LED brightness used in firmware, within a
  typical MT3608's range. If you raise LED brightness a lot, size the boost up.
- **TP4056:** order the **USB-C** variant **with battery protection** (some bare
  modules omit it).

## Notes

- **Wiring & pins:** `wiring/wiring_table.md`. **Assembly/usage:** `OPERATING_GUIDE.md`.
- **MT3608 first:** set its output to 5.0 V with a multimeter **before** wiring it to anything — it ships at an arbitrary voltage.
- **Switch placement:** on the TP4056 OUT+ (load side), not between the cell and the
  charger, so the device can still charge while switched off.
- Section 1 is the exact stack from the project spec; Sections 2–3 are the build
  consumables needed to actually assemble and house it.
