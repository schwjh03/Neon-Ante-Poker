# Third-Party Notices

## raylib 5.5

- Project: https://github.com/raysan5/raylib
- Official prebuilt Windows MinGW-w64 archive: https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_win64_mingw-w64.zip
- Copyright (c) 2013-2024 Ramon Santamaria and contributors.
- License: zlib/libpng, reproduced in `vendor/raylib-5.5_win64_mingw-w64/LICENSE`.
- The original static library and public header are included without modification.

## PokerHandEvaluator

- Project: https://github.com/HenryRLee/PokerHandEvaluator
- Revision: `10be452e4c1ee40a6a56f06457f46bff27ca495a`.
- Copyright 2016-2023 Henry Lee and contributors.
- License: Apache License 2.0, reproduced in `vendor/phevaluator/LICENSE`.
- The five-card evaluator and its required tables are included without modification. The game adapts its results for partial hands, suit-wild cards, and forged duplicates in `src/engine/poker.cpp`.

No third-party card images, music, or commercial game assets are used. Windows supplies its system fonts and audio services.

## Browser edition

The browser edition builds the same C++ game and PokerHandEvaluator with Emscripten 4.0.7 and raylib 5.5 source. Browser sound effects are synthesized with Web Audio. Emscripten's runtime license and the raylib and evaluator licenses are included in `docs/licenses/`.

- Atkinson Hyperlegible Regular and Bold: Braille Institute of America, SIL Open Font License 1.1. Source: https://github.com/google/fonts/tree/main/ofl/atkinsonhyperlegible
- Libre Baskerville: Impallari Type and contributors, SIL Open Font License 1.1. Source: https://github.com/google/fonts/tree/main/ofl/librebaskerville
- Font license texts are included in `web/assets/fonts/` and `docs/licenses/`. These openly licensed fonts are bundled only for the browser edition.
