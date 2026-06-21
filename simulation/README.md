# Running the Simulation (Wokwi)

No physical hardware needed. Two ways to run.

## Option A — wokwi.com (fastest)
1. Go to https://wokwi.com → **New Project** → ESP32.
2. The project must have exactly **one** main sketch. Put the contents of
   `../firmware/scoreboard.ino` into the **`sketch.ino`** tab (do NOT also keep a
   separate `scoreboard.ino` tab — two main files cause "redefinition" errors).
3. Add every module as its own tab (the **+** menu → New File). Wokwi compiles
   all of these together:
   - `game_logic.h`, `game_logic.cpp`
   - `display_matrix.h`, `display_matrix.cpp`
   - `indicators.h`, `indicators.cpp`
   - `input.h`, `input.cpp`
   - `web_ui.h`, `web_ui.cpp`
   - `Max72xxPanel.h`, `Max72xxPanel.cpp` (bundled in `firmware/` — this library
     isn't in the Arduino registry, so it ships with the project)
4. Open the `diagram.json` tab, select-all, paste the contents of this folder's
   `diagram.json`.
5. Add the **one** registry library via the Library Manager "+":
   - `Adafruit GFX Library`
   (Max72xxPanel needs no install — it's the source tabs you added in step 3.)
6. Press **▶ Play**.

## Option B — VS Code + Wokwi extension
1. Build the sketch (Arduino CLI / PlatformIO) to produce `scoreboard.ino.bin` + `.elf`.
2. Keep `wokwi.toml` + `diagram.json` beside the build output.
3. Press the Wokwi "Start Simulation" button.

## What you should see
- **4 matrices**, stacked per player: top = initials, bottom = score.
  Left column = Player A, right column = Player B.
- **6 LEDs:** each player's own LEDs (green = A, red = B) light up for the
  rounds they've won (best-of-3, first to 2; the 3rd LED blinks at match over).
- **Buttons:** each player has UP and DOWN. **UP** = +1 point (runs the sport's
  rules). **DOWN** = remove one point from **that player only**.
- Stop & restart the sim → game, names, and scores reload from NVS (persistence).

## Important caveats
- **Wokwi can't fully model the 16×16 squares.** Wokwi's `wokwi-max7219-matrix`
  is a horizontal 8×N strip, so each 16×16 module is approximated by a `chain=4`
  strip and the `Max72xxPanel` 2-D tile mapping won't render as true squares in
  the sim. The button/score/LED **logic** is testable in Wokwi; the final
  square-character layout (and the `TILE_ROTATION` / tile-position constants in
  `display_matrix.cpp`) only verify on **real hardware**.
- **The web page is not testable on free Wokwi.** Reaching a server on the
  simulated ESP32 needs the Wokwi **Private Gateway** (paid), and the SoftAP
  captive portal isn't supported there. Game/name selection is verified on real
  hardware. To smoke-test scoring logic in Wokwi, the game defaults to Table
  Tennis with the stored player names.

## Notes
- Player initials come from the names entered on the web page; the board shows
  the first two letters. Defaults persist in NVS.
- Pins (see `wiring/wiring_table.md`): matrix chain CLK=18, DIN=23, CS=5;
  buttons 32/33 (A up/down), 25/13 (B up/down); LEDs 26/27/14 (A), 4/16/17 (B).
- Libraries: `Adafruit GFX Library` (from the Library Manager) +
  `Max72xxPanel` (BSD, bundled in `firmware/` — no install). The old
  `MD_Parola` / `MD_MAX72xx` / `Adafruit ILI9341` / `Adafruit NeoPixel` stacks
  are **no longer used**.
