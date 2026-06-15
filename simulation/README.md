# Running the Simulation (Wokwi)

No physical hardware needed. Two ways to run:

## Option A — wokwi.com (fastest)
1. Go to https://wokwi.com → **New Project** → ESP32.
2. Paste the contents of `../firmware/scoreboard.ino` into `sketch.ino`.
3. Open the `diagram.json` tab, select-all, paste the contents of `diagram.json` here.
4. Add the libraries (click the **Library Manager** "+" in the editor):
   - `Adafruit GFX Library`
   - `Adafruit ILI9341`
   - `Adafruit NeoPixel`
5. Press **▶ Play**.

## Option B — VS Code + Wokwi extension
1. Build the sketch (Arduino CLI / PlatformIO) to produce `scoreboard.ino.bin` + `.elf`.
2. Keep `wokwi.toml` + `diagram.json` beside the build output.
3. Press the Wokwi "Start Simulation" button.

## What you should see
- **Left 2 screens:** `P1` initials + Player A's live score (red).
- **Right 2 screens:** Player B's live score (green) + `P2` initials.
- **Centre LED stick:** lights one LED per game won (red from left = A, green from right = B).
- Click the **red button** → Player A +1. Click the **green button** → Player B +1.
- First to **11 (win by 2)** → that player's games-won LED lights, a win-flash plays, round resets to 0–0.
- **Hold both buttons ~2s** → full match reset.
- Stop & restart the sim → scores reload from NVS (proves persistence).

## Notes
- Player initials (`P1`/`P2`), win score, win-by-two, and games shown are all
  `#define`s at the top of `scoreboard.ino` — change and re-run.
- The 8 single NeoPixels chained in the diagram emulate the WS2812B 8-LED stick.
- Display part = `wokwi-ili9341`. Pin names that matter: `SCK MOSI D/C CS VCC GND`.
- Wokwi's ILI9341 has **no functional RST or LED pin** in simulation (the panel
  auto-resets internally), so the diagram leaves them unwired and the firmware
  uses a software reset (`rst = -1`). On **real hardware** you DO wire RST→GPIO22
  and the backlight LED→3V3 per `wiring/wiring_table.md`.
