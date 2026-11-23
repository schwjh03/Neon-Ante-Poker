# Code Structure

The game has three main parts: a graphics-independent engine, a presentation layer, and a small runtime that opens the window and drives frames. The layout, scoring rules, prices, controls, and saved-run format are the same as before this reorganization.

## File Tree

```text
src/
  main.cpp                 Program entry point: calls runApplication()
  runtime.hpp/.cpp         Window lifecycle, launch options, frame loop
  app.hpp/.cpp             Input, animations, current dialog, screen dispatch
  theme.hpp                Colors and logical canvas dimensions
  ui.hpp/.cpp              Drawing API, fonts, pointer state, tooltips
  game.hpp/.cpp            Game data and turn/round progression
  audio.hpp/.cpp           Generated sounds and Windows playback
  preview.hpp/.cpp         Screenshot/demo scene setup
  drawing/
    primitives.cpp         Text, boxes, buttons, hit testing
    cards.cpp              Suit symbols, stars, card faces and backs
    relics.cpp             Relic illustrations and tiles
  screens/
    hud.cpp                Header, blind statistics, owned relics
    table.cpp              Hand preview, player cards, play/discard controls
    shop.cpp               Night Market display and purchase buttons
    end_screen.cpp         Victory, loss, and endless-mode screen
    dialogs.cpp            Deck, hand reference, scoring, relic, and menu dialogs
  engine/
    content.cpp            Relic descriptions, prices, hand bases, boss descriptions
    poker.cpp              PokerHandEvaluator adapter and custom hand categories
    scoring.cpp            Chips, multipliers, debuffs, and relic effects
    shop.cpp               Shop inventory, purchases, selling, upgrades, rerolls
    save.cpp               Version-1 save/load and validation
tests/
  game_tests.cpp           Engine checks and all standard five-card combinations
  ui_smoke.hpp/.cpp        Automated clicks and assertions for the actual controls
build.ps1                  Builds every src/**/*.cpp plus the graphical checks
.clang-format              Consistent source formatting rules
```

## One Frame

1. `runtime.cpp` maps the physical mouse into the fixed 1280-by-800 game canvas.
2. `App::update()` advances animations and handles keyboard input.
3. `App::render()` draws the HUD and current screen using `Ui`.
4. Screen controls call `Game` actions. `App::finishAction()` plays feedback and saves successful actions.
5. `Ui::finishFrame()` draws the tooltip and chooses the pointer cursor.
6. The runtime displays the canvas at the current window size.

`Modal` names the open dialog (`Deck`, `Menu`, `ScoreDetails`, and so on). `Ui` owns pointer and font state, so drawing no longer depends on file-global mouse flags or font variables.

## Where to Edit

| Change | File |
| --- | --- |
| Relic names, descriptions, or prices | `src/engine/content.cpp` |
| Relic scoring behavior | `src/engine/scoring.cpp` |
| Target scores, rewards, hands, or blind progression | `src/game.cpp` |
| What shops offer and how purchases apply | `src/engine/shop.cpp` |
| Poker categories or wild-card handling | `src/engine/poker.cpp` |
| Save/load fields or validation | `src/engine/save.cpp` |
| Table layout or play/discard buttons | `src/screens/table.cpp` |
| Menu, deck viewer, or hand reference | `src/screens/dialogs.cpp` |
| Colors | `src/theme.hpp` |
| Card faces | `src/drawing/cards.cpp` |
| Font loading or tooltip placement | `src/ui.cpp` |
| Keyboard shortcuts | `src/app.cpp` |
| Window settings or launch options | `src/runtime.cpp` |

The screen files define `App` drawing methods; the engine files define `Game` methods. Their shared class declarations live in `app.hpp` and `game.hpp`. These files divide existing responsibilities without introducing separate frameworks or changing ownership of the game state.

## Build and Verify

Run `./build.ps1 -Test` from PowerShell in the project folder. The build discovers source files recursively, so another `.cpp` file under `src/` is included automatically. Engine-only tests compile `game.cpp` and the files under `src/engine/`; they do not need a window or audio device.

Keep the vendor files unchanged. Apply the included `.clang-format` to first-party C++ files when using an editor or formatter that supports it.

## Modes and shared builds

`src/engine/modes.cpp` handles one-time table challenges and relic drafts. `src/screens/run_modes.cpp` draws run setup, challenge choices, and draft selection. `src/engine/build_profiles.cpp` validates and stores deck snapshots; `src/build_dialogs.cpp` supplies Windows file dialogs. Echo difficulty uses local and deliberately imported snapshots, rather than a server or automatic telemetry.

Relic scoring iterates the owned slot order. Multiplication changes the running multiplier immediately, so later additive bonuses are not retroactively multiplied.
