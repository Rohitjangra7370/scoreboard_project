# Smart A4 Scoreboard — Operating Guide

A portable, battery-powered, two-player digital scoreboard. Each player has two
dedicated MAX7219 16×16 square matrix modules — the top square shows their
initials (up to 2 letters) and the bottom square shows their live score (00–99
or tennis AD); six indicator LEDs (3 green, 3 red) track games won per player.
Setup is done from your phone over Wi-Fi — no app required.

---

## 1. At a glance

```
    Player A                    Player B
┌──────────────┐            ┌──────────────┐
│  A-initials  │   ● ●      │  B-initials  │   top row = 16×16 modules
├──────────────┤   ● ●      ├──────────────┤   centre = 6 LEDs (2×3 tally):
│   A-score    │   ● ●      │   B-score    │     left col green = A's rounds
└──────────────┘            └──────────────┘     right col red  = B's rounds
     A↑  A↓                      B↑  B↓          buttons below each column
```

- **Player A = left column (2 squares) + green LEDs**, **Player B = right column (2 squares) + red LEDs**.
- Green LEDs (left side) light up for each round Player A wins; red LEDs (right
  side) light up for each round Player B wins.
- Everything (scores, games won, player names, selected game) is saved
  automatically and survives power-off or a flat battery.

---

## 2. First-time setup

1. **Power on** using the slide switch on the side.
2. On your phone, open Wi-Fi settings and connect to the network named
   **`Scoreboard-XXXX`** (no password).
3. **Turn off mobile data** for this step. The scoreboard's Wi-Fi has no
   internet, so with mobile data on, the phone may send the page's requests
   over cellular instead of to the board — the page loads but the buttons
   appear dead. (The firmware also tells the phone the network is "OK" to keep
   it on Wi-Fi, but disabling mobile data is the sure fix.)
4. Open a browser and visit **`http://192.168.4.1`** (use a normal browser
   tab, not the auto sign-in popup, which blocks the page's scripts).
5. On the setup page:
   - Pick the **game** from the dropdown.
   - Type **Player A's name** and **Player B's name** (the board shows each
     player's initials).
   - Tap **Apply** (you'll see a spinner, then a green "Applied" flash).
6. The board immediately shows each player's initials and resets scores to 0.

Your settings are remembered after power-off — you only need to redo setup if
you want to change the game or player names.

---

## 3. Scoring

Each player has **two buttons**:

| Button | What it does |
|--------|-------------|
| **UP** (Player A or B) | +1 point to that player (follows the sport's scoring rules) |
| **DOWN** (Player A or B) | Removes one point from **that player only** — corrects a misclick |

- Each DOWN button affects **only its own player's** score; it never changes
  the opponent's score. Press it as many times as needed (it stops at 0).
- DOWN steps a normal point back (e.g. tennis 40 → 30). It does not reverse an
  already-completed round win — fix a wrongly-awarded round from the web page's
  **Reset** instead.

---

## 4. Match format and round indicators

- Match is **Best of 3**: first player to win **2 rounds** wins the match.
- A round ends when a player reaches the game's winning score (see §5).
- Each round win lights the next LED on that player's side:
  - Player A round wins → green LEDs (GPIOs 26 / 27 / 14, left to right).
  - Player B round wins → red LEDs (GPIOs 4 / 16 / 17, left to right).
- When a player wins their **2nd round** (match over), the **3rd LED on their
  side blinks** to signal the match is complete.
- To start a new match, use the setup page (reconnect to `Scoreboard-XXXX`),
  pick a game and tap Apply — this resets scores and round wins to zero.

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

- **On / off:** the slide switch on the side is the master power switch (it cuts
  the 5 V feed from the power bank).
- **Power:** the unit runs from an internal **USB power bank (5 V / ≥3 A)** — the
  16 modules need more current than a single small battery can give. Expect
  **~3 h** on a 10000 mAh bank at the default brightness.
- **Charging:** charge the power bank through its own USB port (it has built-in
  charging + protection). Use an "always-on" bank so it doesn't auto-shut-off.
- **Brightness:** firmware holds the panels at a modest level (`setIntensity(6)`)
  to keep current and runtime sensible; raise it in `display_matrix.cpp` if you
  have power to spare.
- **Memory:** scores, games won, player names, and the selected game are written
  to the ESP32's flash (NVS) on every change, so turning the device off — or a
  dead battery — never loses the score.

---

## 7. Flashing to hardware

1. Arduino IDE → install the **ESP32** board package and **one** library:
   - **Adafruit GFX Library** — via Arduino Library Manager (search "Adafruit GFX").
   - **Max72xxPanel** by Mark Ruys (BSD) is **bundled with the project**
     (`firmware/Max72xxPanel.h` / `.cpp`) — no separate install needed.
   *(MD_Parola and MD_MAX72xx are no longer used.)*
2. Keep `scoreboard.ino` and all `.h` / `.cpp` files (including the bundled
   `Max72xxPanel.*`) together in the same sketch folder.
3. Wire everything per `wiring/wiring_table.md`.
4. Select **"ESP32 Dev Module"**, choose the COM port, and **Upload**.
5. Print the case: open `enclosure/scoreboard_case.scad` (Rev F) in OpenSCAD,
   set `PART="tray"` then `PART="faceplate"`, export each as STL, and print.

---

## 7a. Mechanical assembly

Print orientation: print both parts on their flat outer face (the bottom edge
has a small chamfer to avoid "elephant's foot"). PETG recommended for a
portable/outdoor unit.

1. **Heat-set inserts:** press an **M3 brass insert** into each of the 4 tray
   corner bosses with a soldering iron (the 4.0 mm bores are sized for them).
   *(No inserts? Set `USE_HEATSET = false` in the `.scad` and re-export — the
   bores become 2.6 mm for self-tapping M3 screws.)*
2. **Boards:** mount the **ESP32** on the central floor standoffs and tuck the
   **USB power bank** into its pocket (M2.5/M3 self-tap for the ESP32). Confirm
   your exact board hole spacing against the `PCB_POSTS` positions in the `.scad`
   first — adjust if needed before printing.
3. **Matrices:** drop each **16×16 module** into its locating frame behind the
   matching faceplate window — **4 modules per window (2×2)**, 4 windows total
   (initials top, score bottom; A left, B right). Each module is captured by the
   window edge; the frame keeps the block aligned. Wire the 16 in one daisy chain
   in the order given in `wiring/wiring_table.md`.
4. **LEDs & buttons:** push the 6 LEDs and 4 buttons into their chamfered
   faceplate holes; wire per `wiring/wiring_table.md`.
5. **Close up:** seat the faceplate — its registration lip self-aligns into the
   tray — and fasten with the 4 **M3 × 8 mm** screws (heads sit flush in the
   counterbores). The USB-C port lines up with the side cut-out.

---

## 8. Troubleshooting

| Symptom | Check |
|---------|-------|
| Matrix panel stays blank | Confirm CLK (GPIO 18), DIN (GPIO 23), and CS (GPIO 5) wires match `wiring/wiring_table.md`. Check daisy-chain: mod 1 DOUT → mod 2 DIN → mod 3 DIN → mod 4 DIN. |
| Wrong content on wrong square | Check module order: mod 1 = A-initials (top-left), mod 2 = A-score (bottom-left), mod 3 = B-initials (top-right), mod 4 = B-score (bottom-right). |
| Buttons do nothing | Buttons wire to GPIO + GND with internal pull-ups. Check that the correct GPIO is used (A-up=32, A-down=33, B-up=25, B-down=13). |
| Score reset itself on its own | That's a round win (e.g. table tennis at 11) — points reset to 0–0 and a round LED lights. |
| Setup page doesn't open | Connect to `Scoreboard-XXXX` Wi-Fi, then manually navigate to `http://192.168.4.1`. |
| Settings lost after power-off | Scores save on every change via NVS. If this happens, the flash write may have failed — re-flash the firmware. |
| 3rd LED blinking | Match is over — that player won the Best-of-3. Start a new match via the setup page. |

---

*Built for Swapna. Firmware by Rohit Jangra — ESP32, 4× MAX7219 16×16 square
matrix modules, 6 round-indicator LEDs, all logic unit-tested.*
