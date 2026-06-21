# Smart A4 Scoreboard — Operating Guide

A portable, battery-powered, two-player digital scoreboard. Each player has a
dedicated MAX7219 dot-matrix panel showing their initials and live score; six
indicator LEDs (3 green, 3 red) track games won per player. Setup is done from
your phone over Wi-Fi — no app required.

---

## 1. At a glance

```
[ A matrix: initials + score ]  [green LEDs]  ||  [red LEDs]  [B matrix: score + initials]
   Player A                    rounds won (A)      rounds won (B)              Player B
         A-up / A-down buttons                        B-up / B-down buttons
```

- **Player A = left panel + green LEDs**, **Player B = right panel + red LEDs**.
- Green LEDs (left side) light up for each round Player A wins; red LEDs (right
  side) light up for each round Player B wins.
- Everything (scores, games won, player names, selected game) is saved
  automatically and survives power-off or a flat battery.

---

## 2. First-time setup

1. **Power on** using the slide switch on the side.
2. On your phone, open Wi-Fi settings and connect to the network named
   **`Scoreboard-XXXX`** (no password).
3. A setup page should open automatically (captive portal). If it doesn't,
   open a browser and visit **`http://192.168.4.1`**.
4. On the setup page:
   - Pick the **game** from the dropdown.
   - Type **Player A's name** and **Player B's name** (the board shows each
     player's initials).
   - Tap **Apply**.
5. The board immediately shows each player's initials and resets scores to 0.

Your settings are remembered after power-off — you only need to redo setup if
you want to change the game or player names.

---

## 3. Scoring

Each player has **two buttons**:

| Button | What it does |
|--------|-------------|
| **UP** (Player A or B) | +1 point to that player |
| **DOWN** (Player A or B) | Undo the last point scored (can undo multiple times) |

- Undo works across game boundaries — if a round just ended, pressing DOWN
  reverses it and restores the previous score.

---

## 4. Match format and round indicators

- Match is **Best of 3**: first player to win **2 rounds** wins the match.
- A round ends when a player reaches the game's winning score (see §5).
- Each round win lights the next LED on that player's side:
  - Player A round wins → green LEDs (GPIOs 26 / 27 / 14, left to right).
  - Player B round wins → red LEDs (GPIOs 4 / 16 / 17, left to right).
- When a player wins their **2nd round** (match over), the **3rd LED on their
  side blinks** to signal the match is complete.
- To start a new match, use the setup page (reconnect to `Scoreboard-XXXX`) and
  tap Apply, or press either DOWN button to undo back through the match.

---

## 5. Games available

Switch games via the setup page. Selecting a new game clears the scores. Your
choice is remembered after power-off.

| Game | Win rule |
|------|----------|
| **Table Tennis** | First to **11 points, win by 2** (deuce at 10–10) |
| **Badminton** | First to **21 points, win by 2** (hard cap at 30) |
| **Tennis** | Standard tennis scoring: 0 → 15 → 30 → 40 → game; deuce/advantage |
| **Free Count** | Plain count-up, no win rule — two tally counters |
| **Pickleball** | First to **11 points, win by 2** |
| **Volleyball** | First to **25 points, win by 2** |
| **Basketball 21** | First to **21 points, win by 2** |
| **Cornhole** | First to **21 points, win by 2** |

---

## 6. Power & charging

- **On / off:** the slide switch on the side is the master power switch.
- **Charging:** plug a **USB-C** cable into the charge port (TP4056 module). It
  charges the internal 3.7 V lithium battery safely; unplug when full.
- **Brightness:** a boost converter (MT3608) keeps the matrix panels and LEDs at
  full brightness even as the battery drains.
- **Memory:** scores, games won, player names, and the selected game are written
  to the ESP32's flash (NVS) on every change, so turning the device off — or a
  dead battery — never loses the score. (The undo history is the one exception:
  it clears on power-off.)

---

## 7. Flashing to hardware

1. Arduino IDE → install the **ESP32** board package and the libraries:
   *MD_MAX72xx*, *MD_Parola*.
2. Keep `scoreboard.ino` and all `.h` / `.cpp` files together in the same
   sketch folder.
3. Wire everything per `wiring/wiring_table.md`.
4. Select **"ESP32 Dev Module"**, choose the COM port, and **Upload**.
5. Print the case: open `enclosure/scoreboard_case.scad` in OpenSCAD, set
   `PART="tray"` then `PART="faceplate"`, export each as STL, and print.

---

## 8. Troubleshooting

| Symptom | Check |
|---------|-------|
| Matrix panel stays blank | Confirm CLK (GPIO 18), DIN (GPIO 23), and CS (GPIO 5) wires match `wiring/wiring_table.md`. Check daisy-chain: Panel A DOUT → Panel B DIN. |
| Wrong player on wrong panel | Check module order: modules 0–3 = Player A (left panel), 4–7 = Player B (right panel). |
| Buttons do nothing | Buttons wire to GPIO + GND with internal pull-ups. Check that the correct GPIO is used (A-up=32, A-down=33, B-up=25, B-down=13). |
| Score reset itself on its own | That's a round win (e.g. table tennis at 11) — points reset to 0–0 and a round LED lights. |
| Setup page doesn't open | Connect to `Scoreboard-XXXX` Wi-Fi, then manually navigate to `http://192.168.4.1`. |
| Settings lost after power-off | Scores save on every change via NVS. If this happens, the flash write may have failed — re-flash the firmware. |
| 3rd LED blinking | Match is over — that player won the Best-of-3. Start a new match via the setup page. |

---

*Built for Swapna. Firmware by Rohit Jangra — ESP32, MAX7219 dot-matrix panels,
6 round-indicator LEDs, all logic unit-tested.*
