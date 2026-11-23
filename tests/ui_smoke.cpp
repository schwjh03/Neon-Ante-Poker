#include "ui_smoke.hpp"
#include "app.hpp"
#include <algorithm>

namespace neon {
UiSmokeTest::UiSmokeTest(App& application, bool runTest) : app(application), enabled(runTest) {
    if (!enabled) {
        return;
    }

    steps = {{"select first card",
              {360, 650},
              [this] {
                  return app.game.selected.size() == 1;
              }},
             {"select second card",
              {480, 650},
              [this] {
                  return app.game.selected.size() == 2;
              }},
             {"select third card",
              {594, 650},
              [this] {
                  return app.game.selected.size() == 3;
              }},
             {"select fourth card",
              {711, 650},
              [this] {
                  return app.game.selected.size() == 4;
              }},
             {"select fifth card",
              {827, 650},
              [this] {
                  return app.game.selected.size() == 5 && app.game.preview().total == 488;
              }},
             {"open scoring details",
              {1222, 357},
              [this] {
                  return app.modal == Modal::ScoreDetails;
              }},
             {"close scoring details",
              {640, 608},
              [this] {
                  return app.modal == Modal::None;
              }},
             {"play winning hand",
              {405, 760},
              [this] {
                  return app.game.phase == Phase::Shop && app.game.score == 488;
              }},
             {"buy relic",
              {573, 640},
              [this] {
                  return app.game.owned.size() == 2;
              }},
             {"buy permanent foil",
              {945, 640},
              [this] {
                  return std::count_if(app.game.deck.begin(), app.game.deck.end(), [](Card card) {
                             return card.foil;
                         }) == 3;
              }},
             {"reroll shop",
              {390, 749},
              [this] {
                  return app.game.rerolls == 1;
              }},
             {"advance blind",
              {1100, 748},
              [this] {
                  return app.game.phase == Phase::Play && app.game.blind == 1;
              }},
             {"select discard",
              {360, 650},
              [this] {
                  return app.game.selected.size() == 1;
              }},
             {"discard",
              {620, 760},
              [this] {
                  return app.game.discards == 2 && app.game.selected.empty();
              }},
             {"open menu",
              {1190, 38},
              [this] {
                  return app.modal == Modal::Menu;
              }},
             {"open hand reference",
              {640, 466},
              [this] {
                  return app.modal == Modal::HandReference;
              }},
             {"close reference",
              {949, 118},
              [this] {
                  return app.modal == Modal::None;
              }},
             {"open deck",
              {125, 746},
              [this] {
                  return app.modal == Modal::Deck;
              }},
             {"close deck",
              {1116, 112},
              [this] {
                  return app.modal == Modal::None;
              }},
             {"reopen menu",
              {1190, 38},
              [this] {
                  return app.modal == Modal::Menu;
              }},
             {"new run confirmation",
              {640, 346},
              [this] {
                  return app.modal == Modal::ConfirmNewRun;
              }},
             {"cancel new run",
              {640, 490},
              [this] {
                  return app.modal == Modal::Menu && app.game.blind == 1;
              }},
             {"resume", {640, 280}, [this] {
                  return app.modal == Modal::None;
              }}};
    report.open("ui-smoke.txt");
}

void UiSmokeTest::beforeFrame(int frame) {
    if (!enabled) {
        return;
    }
    acting = frame % 4 == 0 && app.screenTransition == 0 && nextStep < steps.size();
    app.ui.clicked = acting;
    app.ui.mouse = acting ? steps[nextStep].position : Vector2{-100, -100};
}

void UiSmokeTest::afterFrame(int frame) {
    if (!enabled) {
        return;
    }
    if (acting) {
        const auto& step = steps[nextStep++];
        bool passed = step.verify();
        report << (passed ? "PASS " : "FAIL ") << step.label << '\n';
        if (!passed) {
            TakeScreenshot("ui-smoke-failure.png");
            result = 2;
            done = true;
            return;
        }
    }
    if (nextStep == steps.size() || frame > 600) {
        result = nextStep == steps.size() ? 0 : 3;
        done = true;
    }
}

bool UiSmokeTest::finished() const {
    return done;
}

int UiSmokeTest::exitCode() const {
    return result;
}
} // namespace neon
