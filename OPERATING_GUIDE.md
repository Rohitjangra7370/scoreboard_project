# Smart A4 Scoreboard — Operating Guide

A portable, battery-powered, two-player digital scoreboard. Four screens show
each player's initials and live score; a central LED column tracks games won.
Four built-in scoring modes (Table Tennis, Tennis, Badminton, Free Count), full
undo, and a menu to switch games — all driven by just two buttons.

---

## 1. At a glance

```
[ P1 initials ][ P1 score ]   ||  LED column  ||   [ P2 score ][ P2 initials ]
   (red)          (red)         games won (R | G)      (green)       (green)
        RED arcade button = Player 1            GREEN arcade button = Player 2
```

- **P1 = red**, **P2 = green** (matches the two arcade buttons).
- Centre LED stick: red LEDs fill from the left for P1 games won, green LEDs
  fill from the right for P2 games won (up to 4 each).
- Everything (scores, games, selected game) is saved automatically and survives
  power-off or a flat battery.

---

## 2. Controls — button cheat-sheet

There are only two buttons. What they do depends on a short tap, a long hold, or
holding both.

| Action | What it does (during play) |
|---|---|
| **Red — quick tap** | +1 point to Player 1 |
| **Green — quick tap** | +1 point to Player 2 |
| **Red — hold ~0.6 s** | **UNDO** the last point (step back; can undo many times) |
| **Green — hold ~0.6 s** | **RESET** the match (scores + games back to 0) |
| **Both buttons — hold ~2 s** | Open / close the **GAME MENU** |

| In the GAME MENU | What it does |
|---|---|
| **Red — tap** | Highlight the next game (cycles through the 4) |
| **Green — tap** | Select the highlighted game (starts it, scores reset to 0) |
| **Both — hold ~2 s** | Close the menu without changing the game |

> Undo is generous: it even reverses a point that *won* a round/game — the
> games-won LED switches back off and the score returns to what it was. A reset
> can also be undone (one step), in case you reset by mistake.

---

## 3. The four games

Switch games via the menu (hold both buttons ~2 s). Selecting a game clears the
scores. Your choice is remembered after power-off.

### Table Tennis
- First to **11 points wins the game**, but you must **win by 2**.
- At 10–10 it continues until someone leads by two (e.g. 12–10, 13–11).
- Winning a game lights one LED and resets the points to 0–0.

### Badminton
- First to **21 points, win by 2**.
- **Hard cap at 30:** if the score reaches 30, that point wins regardless of the
  margin (e.g. 30–29).
- Winning a game lights one LED and resets to 0–0.

### Tennis
- Points are shown the tennis way: **0 → 15 → 30 → 40 → game**.
- **Deuce / advantage:** at 40–40 the screen shows `40`–`40` (deuce). The next
  point shows **`AD`** (advantage) for that player. If they score again they win
  the game; if the other player scores, it goes back to deuce.
- Winning a game lights one LED. (Sets are not tracked — the LED column counts
  games won, like the other modes.)

  *Worked example:* P1 leads 40–30 and scores → P1 wins the game (LED +1).
  Or: 40–40 (deuce) → P1 scores → `AD`–`40` → P1 scores → P1 wins. But if at
  `AD`–`40` P2 scores, it returns to `40`–`40` deuce.

### Free Count
- Plain count-up, **no win rule** — just two tally counters.
- Use **Green long-press (reset)** or re-select the game in the menu to clear it.

### Pickleball
- First to **11 points, win by 2** (rally scoring — every tap scores).

### Volleyball
- First to **25 points, win by 2**.

### Basketball 21
- Pickup/streetball style: first to **21 points, win by 2**.

### Cornhole
- First to **21 points, win by 2**. (The optional "bust back" rule is not
  included — it's a simple count to 21.)

### Reaction Game — the F1 start-light duel
A real-time reflex game instead of a scoreboard. Open it from the menu (hold both
buttons ~2 s, then cycle to the last entry, **"Reaction"**, and select with green).

How a round works:
1. The 5 LEDs light up **one by one**, F1-grid style (`READY` on the screens).
2. After a short random wait, **all the lights go OUT** (`GO!` on the screens).
3. The **first player to tap their button wins** — their reaction time in
   milliseconds shows on their screen (e.g. `215`).
4. **Jump start:** tap *before* the lights go out and you lose the round instantly
   (your screen shows `JUMP`, the other player wins).
5. The round winner lights one LED on the tally, then the **next round starts
   automatically**.

Hold **both buttons ~2 s** to leave the Reaction game and return to the menu.
Reaction wins are a quick duel — they are **not** saved across power-off.

---

## 4. Power & charging

- **On / off:** the slide switch on the side is the master power switch.
- **Charging:** plug a **USB-C** cable into the charge port (TP4056 module). It
  charges the internal 3.7 V lithium battery safely; unplug when full.
- **Brightness:** a boost converter (MT3608) keeps the screens and LEDs at full
  brightness even as the battery drains.
- **Memory:** scores, games won, and the selected game are written to the ESP32's
  flash (NVS) on every change, so turning the device off — or a dead battery —
  never loses the score. (The undo history is the one exception: it clears on
  power-off.)

---

## 5. Running the simulation (no hardware needed)

The whole device runs in a browser via Wokwi. See `simulation/README.md` for
step-by-step instructions: paste `firmware/scoreboard.ino` and
`simulation/diagram.json` into wokwi.com, **add `game_logic.h`, `game_logic.cpp`,
`reaction.h`, and `reaction.cpp` as separate file tabs** (the build needs them),
add the three Adafruit libraries, and press Play. Click the on-screen red/green
buttons to play; hold a button (click and keep held) for long-press actions.

## 6. Flashing to real hardware

1. Arduino IDE → install the **ESP32** board package and the libraries:
   *Adafruit GFX*, *Adafruit ILI9341*, *Adafruit NeoPixel*.
2. Keep `scoreboard.ino`, `game_logic.h` / `game_logic.cpp`, and
   `reaction.h` / `reaction.cpp` together in the same sketch folder (the sketch
   needs all the core files beside it).
3. Wire everything per `wiring/wiring_table.md`.
4. Select **"ESP32 Dev Module"**, choose the COM port, and **Upload**.
5. Print the case: open `enclosure/scoreboard_case.scad` in OpenSCAD, set
   `PART="tray"` then `PART="faceplate"`, export each as STL, and print.

---

## 7. Customising (simple code tweaks)

| What | Where | Default |
|---|---|---|
| Player initials shown on screen | `INITIALS_A` / `INITIALS_B` in `scoreboard.ino` | `"P1"` / `"P2"` |
| Long-press time (undo/reset) | `LONG_PRESS_MS` in `scoreboard.ino` | `600` ms |
| Menu open/close hold time | `MENU_HOLD_MS` in `scoreboard.ino` | `2000` ms |
| Any sport's win score / cap | the `GAMES[]` table in `game_logic.cpp` (`winScore`, `hardCap` columns) | e.g. Table Tennis `11, 0`; Badminton `21, 30` |
| Reaction countdown speed | `COUNTDOWN_STEP_MS` in `reaction.h` | `1000` ms per light |
| Reaction random wait range | `HOLD_MIN_MS` / `HOLD_MAX_MS` in `reaction.h` | `1000`–`3000` ms |
| Reaction result display time | `RESULT_MS` in `reaction.h` | `2500` ms |

After any change, re-upload the sketch (or re-run the Wokwi simulation).

---

## 8. Troubleshooting

| Symptom | Check |
|---|---|
| A screen stays black | Confirm that screen's **CS** wire and the shared **SCK / MOSI / DC** wires match `wiring/wiring_table.md`. (RST/LED aren't used in the Wokwi simulation — that's normal.) |
| Buttons do nothing | Buttons wire to GPIO + GND with internal pull-ups; a quick tap scores, a ~0.6 s hold is undo/reset. |
| Score reset itself on its own | That's a game win (e.g. table tennis 11) — the points reset to 0–0 and a games-won LED lights. |
| Wrong game on power-up | The device remembers the last selected game; open the menu (hold both) and pick another. |
| Score didn't save after power-off | Scores save on every change; undo history is intentionally not saved across power-off. |

---

*Built for Swapna. Firmware by Rohit Jangra — ESP32, four ILI9341 displays,
WS2812B LED stick, all logic unit-tested.*
