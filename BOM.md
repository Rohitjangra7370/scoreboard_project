# Smart A4 Scoreboard — Bill of Materials (BOM)

Everything needed to build one unit. Section 1 is the core electronics stack
(as specified). Section 2 lists the passives and build extras that real hardware
needs but that aren't headline components. Section 3 is the enclosure.

Pin assignments for every part are in `wiring/wiring_table.md`.

---

## Quick cost summary (USA, single unit, approx.)

Prices are typical US street prices as of June 2026 (Amazon / Adafruit /
SparkFun), single-quantity. Many parts ship in multipacks, so the **per-unit**
cost drops if you build more than one. Prices marked *(kit)* come from a parts
assortment you likely already have or buy once.

| Part | Qty | Approx. unit | Line total | Where to buy (US) |
|------|-----|--------------|-----------|--------------|
| ESP32-WROOM-32 dev board | 1 | $8 | $8 | [Amazon – HiLetgo/ESP-WROOM-32](https://www.amazon.com/ESP-WROOM-32-Development-Microcontroller-Integrated-Compatible/dp/B08D5ZD528) |
| MAX7219 16×16 square matrix module (~70 mm, 4× 8×8 tiles per module) | 4 | $9–12 | ~$40 | [Amazon – MAX7219 16x16 matrix](https://www.amazon.com/s?k=MAX7219+16x16+dot+matrix+module) |
| 5 mm LED — green (Player A round indicators) | 3 | *(kit)* | ~$0.50 | [Amazon (LED assortment kit)](https://www.amazon.com/s?k=5mm+LED+assortment+kit) |
| 5 mm LED — red (Player B round indicators) | 3 | *(kit)* | ~$0.50 | [Amazon (LED assortment kit)](https://www.amazon.com/s?k=5mm+LED+assortment+kit) |
| 220 Ω resistor | 6 | *(kit)* | ~$0.50 | [Amazon (resistor kit)](https://www.amazon.com/s?k=resistor+assortment+kit) |
| Momentary push button (6×6 mm tactile or panel-mount) | 4 | *(kit)* | ~$1 | [Amazon (tactile button kit)](https://www.amazon.com/s?k=momentary+push+button+assortment) |
| SPDT slide switch | 1 | $1 | $1 | [Amazon (multipack)](https://www.amazon.com/s?k=SPDT+slide+switch) |
| 3.7 V LiPo 1200–2000 mAh | 1 | $9 | $9 | [Adafruit 1200 mAh LiPo](https://www.adafruit.com/product/258) |
| TP4056 USB-C (w/ protection) | 1 | $2 | $2 | [Amazon – HiLetgo TP4056 Type-C](https://www.amazon.com/HiLetgo-Lithium-Charging-Protection-Functions/dp/B07PKND8KG) |
| MT3608 boost converter | 1 | $1.50 | $1.50 | [Amazon – MT3608](https://www.amazon.com/MT3608-Module-Type-C-2V-24V-TP4056/dp/B0DZ1K4B7G) |
| Passives (capacitor, extra resistors) | — | — | ~$0.50 | [Amazon (kit)](https://www.amazon.com/s?k=electronics+component+assortment+kit) |
| Hook-up wire, headers, protoboard | — | — | ~$8 | [Amazon (kit, reusable)](https://www.amazon.com/s?k=22+awg+hookup+wire+protoboard+kit) |
| 3D-print filament (PLA/PETG) | ~200 g | — | ~$5 | [Amazon](https://www.amazon.com/s?k=PLA+filament+1.75mm) · MatterHackers |
| M3 screws (+ optional inserts) | 4 | — | ~$1 | [Amazon (kit)](https://www.amazon.com/s?k=M3+screw+assortment+kit) |
| **Core electronics only** | | | **≈ $64** | |
| **Full build (incl. wire/print/extras)** | | | **≈ $78–90** | |

*Direct product links are US sellers (Amazon US / Adafruit). "Kit" rows use a
search link since the exact pack you pick doesn't matter — any assortment works.*

**Sourcing notes:**
- **Amazon** — cheapest + fastest for most parts; brands like HiLetgo are reliable.
- **Adafruit / SparkFun** — slightly pricier but verified specs + good docs; best for the LiPo.
- **DigiKey / Mouser** — use if you need an exact part number or buy in volume.

The detailed engineering tables below list every part with its exact spec, pin
role, and build cautions.

---

## 1. Core electronics

| # | Component | Spec / part | Qty | Notes |
|---|-----------|-------------|-----|-------|
| 1 | Microcontroller | **ESP32-WROOM-32** dev board (30/38-pin) | 1 | The brain. Drives both dot-matrix panels over one SPI chain + reads buttons + drives indicator LEDs. Wi-Fi capable (used for the setup portal). |
| 2 | Dot-matrix display | **MAX7219 16×16 square matrix module** (~70 mm, each module contains four 8×8 MAX7219 tiles) | 4 | 2 per player, stacked vertically: TOP square = initials (up to 2 letters), BOTTOM square = score (00–99 or tennis AD). Player A = left column, Player B = right column. All 4 modules daisy-chained on one SPI chain (16 tiles total). Powered from the 5 V rail. |
| 3 | Round-indicator LED (green) | **5 mm LED, green** | 3 | Player A games-won tally (one LED per round). Series 220 Ω resistor each. |
| 4 | Round-indicator LED (red) | **5 mm LED, red** | 3 | Player B games-won tally (one LED per round). Series 220 Ω resistor each. |
| 5 | Current-limiting resistor | **220 Ω**, ¼ W | 6 | One in series per LED (GPIOs 26, 27, 14, 4, 16, 17 → resistor → LED → GND). |
| 6 | Buttons | **Momentary push button** (normally-open) | 4 | Two per player: UP (+1 point) and DOWN (undo). |
| 7 | Power switch | **SPDT slide switch** (heavy-duty) | 1 | Master on/off. Wire on the TP4056 **OUT+** rail (see wiring §4). |
| 8 | Battery | **3.7 V Li-ion / LiPo**, ~1200–2000 mAh | 1 | Portable power. Single cell. |
| 9 | Charge controller | **TP4056, USB-C** charging module (with protection) | 1 | Safe USB-C LiPo charging + over-discharge protection. |
| 10 | Boost converter | **MT3608** step-up (set to 5 V out) | 1 | Boosts 3.7 V → steady 5 V for the matrix panels and ESP32. Set the trimpot to 5.0 V before connecting. |

---

## 2. Passives & build extras (recommended)

These keep the build reliable; quantities are per unit.

| # | Item | Spec | Qty | Notes |
|---|------|------|-----|-------|
| 11 | Capacitor | **1000 µF**, ≥6.3 V electrolytic | 1 | Across the 5 V rail (inrush smoothing for the matrix panels). |
| 12 | Hook-up wire | 24–26 AWG, silicone | — | Button/LED/power wiring. |
| 13 | Protoboard or PCB | Perfboard, ~5×7 cm | 1 | To mount the ESP32 + TP4056 + MT3608 + junctions. |
| 14 | Pin headers / DuPont | 2.54 mm | — | For the dev board + module connections (optional, can solder direct). |
| 15 | USB-C cable | Standard | 1 | Charging (usually user-supplied). |

---

## 3. Enclosure (3D printed)

| # | Item | Spec | Qty | Notes |
|---|------|------|-----|-------|
| 16 | Case — tray + faceplate | 3D print from `enclosure/scoreboard_case.scad` Rev E (export `tray.stl` + `faceplate.stl`) | 1 set | A4 footprint. Faceplate has 4 square ~68 mm windows in a 2×2 grid (top row: A-initials left, B-initials right; bottom row: A-score left, B-score right), plus 6 LED holes and 4 button holes below each player's column. USB-C charge port cut-out on side. |
| 17 | Filament | PLA or PETG (PETG if heat/impact matters) | ~150–250 g | For both parts. |
| 18 | Screws | **M3 × ~8 mm** | 4 | Faceplate → tray, into the 4 screw bosses. |
| 19 | Heat-set inserts *(optional)* | M3 brass | 4 | Cleaner threads than screwing straight into plastic. |

---

## Compatibility & US sourcing

All parts are common and stocked in the USA (Amazon, Adafruit, SparkFun, Micro
Center, DigiKey, Mouser). Read these before ordering:

- **MAX7219 matrix modules:** use the **16×16 square matrix module** (~70 mm
  per side, each module contains four 8×8 MAX7219 tiles). Four modules total —
  2 per player, stacked (initials on top, score on bottom). All 4 modules are
  daisy-chained on one SPI chain. Each module is powered from the 5 V rail;
  the SPI data lines operate at 3.3 V and are accepted by the MAX7219 without
  a level shifter in practice.
- **Libraries:** firmware uses **Adafruit GFX Library** (install via Arduino
  Library Manager). **Max72xxPanel** by Mark Ruys (BSD) is bundled in the
  project (`firmware/Max72xxPanel.*`) — no separate install. MD_Parola /
  MD_MAX72xx are no longer used.
- **LEDs:** any standard 5 mm through-hole green/red LED works. The 220 Ω
  resistor limits current to ~10–15 mA from a 3.3 V GPIO — safe and bright
  enough for indoor use.
- **Buttons:** any normally-open momentary push button (6×6 mm tactile or
  panel-mount style) works. Firmware uses internal pull-ups; no external
  resistor needed.
- **Logic levels:** the ESP32 GPIOs output 3.3 V; the MAX7219 DIN input
  typically accepts this reliably. If displays show noise/corruption, add a
  small pull-down (10 kΩ) on CS when de-asserted.
- **Boost sizing:** the MT3608 set to 5.0 V handles the two matrix panels +
  ESP32 comfortably (peak load well under 1 A). Keep it set to exactly 5.0 V
  before wiring.
- **TP4056:** order the **USB-C** variant **with battery protection** (some bare
  modules omit it).

## Notes

- **Wiring & pins:** `wiring/wiring_table.md`. **Assembly/usage:** `OPERATING_GUIDE.md`.
- **MT3608 first:** set its output to 5.0 V with a multimeter **before** wiring it to anything — it ships at an arbitrary voltage.
- **Switch placement:** on the TP4056 OUT+ (load side), not between the cell and the
  charger, so the device can still charge while switched off.
- Section 1 is the exact stack from the project spec; Sections 2–3 are the build
  consumables needed to actually assemble and house it.
