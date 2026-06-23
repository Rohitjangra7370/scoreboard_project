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
| MAX7219 **16×16** square matrix module (~70 mm, 4× 8×8 tiles per module) | 16 | $13–18 | ~$210–290 | [Amazon – MusRock MAX7219 16×16 (B0FMQT8PHC)](https://www.amazon.com/MusRock-MAX7219-16x16-LED-Module/dp/B0FMQT8PHC) |
| 5 mm LED — green (Player A round indicators) | 3 | *(kit)* | ~$0.50 | [Amazon (LED assortment kit)](https://www.amazon.com/s?k=5mm+LED+assortment+kit) |
| 5 mm LED — red (Player B round indicators) | 3 | *(kit)* | ~$0.50 | [Amazon (LED assortment kit)](https://www.amazon.com/s?k=5mm+LED+assortment+kit) |
| 220 Ω resistor | 6 | *(kit)* | ~$0.50 | [Amazon (resistor kit)](https://www.amazon.com/s?k=resistor+assortment+kit) |
| Momentary push button (6×6 mm tactile or panel-mount) | 4 | *(kit)* | ~$1 | [Amazon (tactile button kit)](https://www.amazon.com/s?k=momentary+push+button+assortment) |
| SPDT slide switch | 1 | $1 | $1 | [Amazon (multipack)](https://www.amazon.com/s?k=SPDT+slide+switch) |
| USB power bank, **5 V / ≥3 A out**, 10000–20000 mAh | 1 | $15–25 | ~$20 | [Amazon (5V 3A USB power bank)](https://www.amazon.com/s?k=usb+power+bank+5v+3a+20000mah) |
| USB-A → screw-terminal / pigtail (power tap) | 1 | $3 | $3 | [Amazon (USB power pigtail)](https://www.amazon.com/s?k=usb+to+screw+terminal+adapter) |
| Passives (capacitor, extra resistors) | — | — | ~$0.50 | [Amazon (kit)](https://www.amazon.com/s?k=electronics+component+assortment+kit) |
| Hook-up wire, headers, protoboard | — | — | ~$8 | [Amazon (kit, reusable)](https://www.amazon.com/s?k=22+awg+hookup+wire+protoboard+kit) |
| 3D-print filament (PLA/PETG) | ~200 g | — | ~$5 | [Amazon](https://www.amazon.com/s?k=PLA+filament+1.75mm) · MatterHackers |
| M3 screws (+ optional inserts) | 4 | — | ~$1 | [Amazon (kit)](https://www.amazon.com/s?k=M3+screw+assortment+kit) |
| **Core electronics only** | | | **≈ $240–320** | |
| **Full build (incl. wire/print/extras)** | | | **≈ $260–350** | |

> **The 16 displays dominate the cost** (~$210–290) — this is the price of
> ~12 cm digits readable to ~18 m (SCALE=2 build; see `firmware/display_matrix.cpp`).
> Prices on the 16×16 modules vary a lot by seller — verify the current Amazon
> price before ordering, and confirm the listing shows **four 8×8 MAX7219 chips
> per module** with a **DIN/CS/CLK input + DOUT output** for chaining (see the
> matrix note below). Do **not** buy "MAX7219 4-in-1 / 8×32" (those are
> horizontal strips, not squares) or any "16×16" module that is RGB/WS2812 or
> 74HC595-driven.
>
> **Power note:** 16 modules draw ~1–3 A at 5 V on typical scoreboard content —
> too much for the old single-LiPo + MT3608 stack. This build uses a **5 V / 3 A
> USB power bank** as the supply (portable, integrated charging + protection).
> Expect **~3 h runtime** on a 10000 mAh bank at typical brightness. To go back
> to the smaller 4-module (32×32, ~6 cm) build, set `SCALE 1` in the firmware
> and the LiPo + TP4056 + MT3608 stack is fine again.

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
| 2 | Dot-matrix display | **MAX7219 16×16 square matrix module** (~70 mm, each module contains four 8×8 MAX7219 tiles) — [Amazon: MusRock 16×16, B0FMQT8PHC](https://www.amazon.com/MusRock-MAX7219-16x16-LED-Module/dp/B0FMQT8PHC) | 16 | **4 per field** (SCALE=2): each field is a 2×2 block of modules = 32×32 px. 4 fields stacked per player: TOP = initials (up to 2 letters), BOTTOM = score; A = left column, B = right column → 64×64 logical px, ~12 cm digits, readable to ~18 m. All 16 modules daisy-chained on one SPI chain (64 tiles total) in the order A-init, A-score, B-init, B-score (see `wiring/wiring_table.md`). Powered from the 5 V rail. **See the matrix sourcing note below before buying.** |
| 3 | Round-indicator LED (green) | **5 mm LED, green** | 3 | Player A games-won tally (one LED per round). Series 220 Ω resistor each. |
| 4 | Round-indicator LED (red) | **5 mm LED, red** | 3 | Player B games-won tally (one LED per round). Series 220 Ω resistor each. |
| 5 | Current-limiting resistor | **220 Ω**, ¼ W | 6 | One in series per LED (GPIOs 26, 27, 14, 4, 16, 17 → resistor → LED → GND). |
| 6 | Buttons | **Momentary push button** (normally-open) | 4 | Two per player: UP (+1 point) and DOWN (-1 point, own player only). |
| 7 | Power switch | **SPDT slide switch** (heavy-duty) | 1 | Master on/off. Wire it in the 5 V feed from the power bank to the rail (see wiring §4). |
| 8 | Power supply | **USB power bank, 5 V / ≥3 A output**, 10000–20000 mAh | 1 | Portable 5 V source for the 16 modules + ESP32. Integrated charging + protection. ~3 h runtime on 10000 mAh. **A single small LiPo + MT3608 cannot supply the ~1–3 A this build needs.** |
| 9 | Power tap | **USB-A → screw-terminal / pigtail** | 1 | Takes 5 V/GND off the power bank to your protoboard rail. |
| — | *(SCALE=1 alt.)* LiPo + TP4056 + MT3608 | — | Only for the smaller 4-module (32×32) build — see the SCALE=1 note in the cost summary. Not used at SCALE=2. |

---

## 2. Passives & build extras (recommended)

These keep the build reliable; quantities are per unit.

| # | Item | Spec | Qty | Notes |
|---|------|------|-----|-------|
| 11 | Capacitor | **1000 µF**, ≥6.3 V electrolytic | 2 | Across the 5 V rail near the modules (inrush smoothing — more important with 16 modules; fit one per column). |
| 12 | Hook-up wire | 24–26 AWG, silicone | — | Button/LED/power wiring. |
| 13 | Protoboard or PCB | Perfboard, ~5×7 cm | 1 | To mount the ESP32 + TP4056 + MT3608 + junctions. |
| 14 | Pin headers / DuPont | 2.54 mm | — | For the dev board + module connections (optional, can solder direct). |
| 15 | USB-C cable | Standard | 1 | Charging (usually user-supplied). |

---

## 3. Enclosure (3D printed)

| # | Item | Spec | Qty | Notes |
|---|------|------|-----|-------|
| 16 | Case — tray + faceplate | 3D print from `enclosure/scoreboard_case.scad` Rev G (export `tray.stl` + `faceplate.stl`) | 1 set | **SCALE=2 footprint ~380 × 346 mm** (active display ~280 × 280 mm — too big for most single-bed printers; print the faceplate in tiles or use a frame). Faceplate: 4 square ~138 mm windows in a 2×2 grid (top: A/B initials; bottom: A/B score), a central 2×3 LED tally (6 holes) between the columns, 4 button holes below the columns (all chamfered), a registration lip, and locating frames behind each window. Tray: corner bosses, PCB standoffs, power-bank space, USB cut-out. Both parts verified manifold (`Simple: yes`) at SCALE=2. |
| 17 | Filament | PLA or PETG (PETG if heat/impact matters) | ~400–700 g | Larger faceplate + tray. |
| 18 | **M3 heat-set inserts** (brass) | M3 × ~4 mm | 4 | Press into the 4 tray corner bosses (bores are sized 4.0 mm for these). Gives durable threads. |
| 19 | **M3 × 8 mm** machine screws | countersunk/pan | 4 | Faceplate → tray. Heads sit flush in the counterbored faceplate holes. |
| 20 | **M2.5/M3 self-tap** screws *(optional)* | × ~4 mm | 4–6 | Fix the ESP32 + power boards onto the tray-floor standoffs (self-tap into the 2.6 mm bores). |

---

## Compatibility & US sourcing

All parts are common and stocked in the USA (Amazon, Adafruit, SparkFun, Micro
Center, DigiKey, Mouser). Read these before ordering:

- **MAX7219 matrix modules — buy 16× the 16×16 SQUARE type.** Recommended US
  part: **[MusRock MAX7219 16×16 LED Module (B0FMQT8PHC)](https://www.amazon.com/MusRock-MAX7219-16x16-LED-Module/dp/B0FMQT8PHC)**
  — four cascaded 8×8 MAX7219 tiles, SPI, chainable. Each module ~70 mm,
  ~$13–18. **4 per field** (2×2), 2 fields per player stacked (initials on top,
  score on bottom); all 16 daisy-chained on one SPI chain (64 tiles). Powered
  from 5 V; the 3.3 V SPI data lines are accepted by the MAX7219 without a level
  shifter in practice. Total display window ~280 × 280 mm.
  - **✅ Confirm each module has:** four MAX7219 chips (one per 8×8 quadrant);
    a 5-pin input (VCC, GND, **DIN, CS, CLK**) **and** a **DOUT** chaining output;
    5 V, red LEDs, ~65–70 mm square.
  - **❌ Do NOT buy:** "MAX7219 **4-in-1 / 8×32**" (those are 128×32 horizontal
    strips, not squares); "16×16 RGB / WS2812" (wrong driver); any "16×16"
    using a single controller or **74HC595** (won't drive with this firmware).
  - **Fallback if 16×16 squares are out of stock:** build each 16×16 from
    **four single 8×8 MAX7219 modules** (64 total) wired 2×2 — same firmware,
    a lot more soldering. US 4-pack:
    **[ALAMSCN 8×8 MAX7219, Pack of 4 (B08SQT9GGB)](https://www.amazon.com/ALAMSCN-MAX7219-Display-Raspberry-Microcontroller/dp/B08SQT9GGB)**
    (buy 4 packs = 16 modules), or
    **[HiLetgo 3-pack 8×8 (B07W6KZR5D)](https://www.amazon.com/HiLetgo-MAX7219-Matrix-Display-Control/dp/B07W6KZR5D)**.
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
- **Power sizing (SCALE=2):** 16 modules + ESP32 draw ~1–3 A at 5 V on typical
  scoreboard content. Use a **USB power bank rated 5 V / ≥3 A** — do **not** use a
  single LiPo + MT3608 (it sags/overheats at this current). Keep the brightness
  modest (`setIntensity(6)` in firmware) to hold current and runtime down. Add a
  bulk cap per column on the 5 V rail.
- **TP4056 / MT3608 / LiPo:** only relevant if you drop back to the SCALE=1
  (4-module) build. Not used at SCALE=2.

## Notes

- **Wiring & pins:** `wiring/wiring_table.md`. **Assembly/usage:** `OPERATING_GUIDE.md`.
- **Power bank:** pick one whose output **stays on under low draw** (some auto-shut-off banks cut power when current dips — pick an "always-on" / passthrough model, or one with a load-detect that the ESP32+modules keep above threshold).
- **Switch placement:** in the 5 V feed between the power-bank tap and the rail, so the device powers off without unplugging the bank.
- Section 1 is the exact stack from the project spec; Sections 2–3 are the build
  consumables needed to actually assemble and house it.
