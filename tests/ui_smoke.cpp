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
             {"meet the club dealer",{132,662},[this]{return app.modal==Modal::ClubStory;}},
             {"reply to the dealer once",{400,547},[this]{return app.game.clubChoices[0]==1;}},
             {"return to the market after the story",{980,670},[this]{return app.modal==Modal::None;}},
             {"buy relic",
              {573, 640},
              [this] {
                  return app.game.owned.size() == 2;
              }},
             {"buy permanent foil",
              {1163, 574},
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
              }},
             {"open run modes",{550,38},[this]{return app.modal==Modal::RunSetup;}},
             {"choose draft without replacing current run",{750,200},[this]{return app.modal==Modal::RunSetup&&app.game.mode==RunMode::Classic;}},
             {"choose Planner before confirmation",{620,400},[this]{return app.game.starter==0;}},
             {"configured run asks for confirmation",{975,675},[this]{return app.modal==Modal::ConfirmNewRun;}},
             {"start draft Planner run",{640,425},[this]{return app.modal==Modal::None&&app.game.mode==RunMode::Draft&&app.game.starter==1&&app.game.discards==4;}},
             {"open table challenges",{125,660},[this]{return app.modal==Modal::Contract;}},
             {"choose first-hand challenge",{360,245},[this]{return app.modal==Modal::None&&app.game.contract==1;}},
             {"draft run select one",{360,650},[this]{return app.game.selected.size()==1;}},
             {"draft run select two",{480,650},[this]{return app.game.selected.size()==2;}},
             {"draft run select three",{594,650},[this]{return app.game.selected.size()==3;}},
             {"draft run select four",{711,650},[this]{return app.game.selected.size()==4;}},
             {"draft run select five",{827,650},[this]{return app.game.selected.size()==5;}},
             {"draft run wins challenge",{405,760},[this]{return app.game.phase==Phase::Shop&&app.game.contractReward==4&&!app.game.draftTaken;}},
             {"open free relic draft",{640,748},[this]{return app.modal==Modal::Draft;}},
             {"take a free relic",{315,515},[this]{return app.modal==Modal::None&&app.game.draftTaken&&app.game.owned.size()==2;}},
             {"move first relic to the right",{450,135},[this]{return app.game.owned[1]==0;}},
             {"move second relic back left",{503,135},[this]{return app.game.owned[0]==0;}},
             {"advance with drafted build",{1100,748},[this]{return app.game.phase==Phase::Play&&app.game.blind==1&&app.game.discards==4;}},
             {"reopen mode picker",{550,38},[this]{return app.modal==Modal::RunSetup;}},
             {"close picker without changing run",{1095,103},[this]{return app.modal==Modal::Menu&&app.game.blind==1;}},
             {"resume expanded run",{640,280},[this]{return app.modal==Modal::None;}}
             };
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
    if (nextStep == steps.size() || frame > 1100) {
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
