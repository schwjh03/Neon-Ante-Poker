#include "app.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace neon {
using namespace theme;

void App::drawDeckDialog() {
    ui.drawBox({108, 78, 1064, 654}, panel, line);
    ui.drawText("Your deck", 136, 99, 29, ink, true);
    ui.drawText("52 cards", 310, 110, 16, muted);
    if (ui.button({1084, 95, 59, 36}, "X", mint)) {
        modal = Modal::None;
        return;
    }
    for (int s = 0; s < 4; ++s) {
        for (int r = 0; r < 13; ++r) {
            int id = r * 4 + s;
            Rectangle q = {136 + r * 77.f, 160 + s * 135.f, 64, 97};
            bool available = std::find(game.draw.begin(), game.draw.end(), id) != game.draw.end();
            bool held = std::find(game.hand.begin(), game.hand.end(), id) != game.hand.end();
            ui.drawCard(game.deck[id], q, false, !available && !held);
            ui.drawCenteredText(held        ? "IN HAND"
                                : available ? "IN DECK"
                                            : "USED",
                                q.x + 32, q.y + 104, 10, held ? mint : muted, true);
            std::string mods = game.deck[id].foil ? "Foil: +35 chips" : "Standard";
            if (game.deck[id].wild) {
                mods += " | Wild suit";
            }
            ui.showTooltip(mods, q);
        }
    }
}

void App::drawHandReferenceDialog() {
    ui.drawBox({270, 79, 740, 655}, panel, line);
    ui.drawText("Hand reference", 302, 103, 30, ink, true);
    if (ui.button({919, 100, 58, 36}, "X", mint)) {
        modal = Modal::None;
        return;
    }
    ui.drawText("HAND", 304, 163, 13, muted, true);
    ui.drawText("LEVEL", 611, 163, 13, muted, true);
    ui.drawText("CHIPS  x  MULT", 749, 163, 13, muted, true);
    for (int i = 9; i >= 0; --i) {
        float y = 198 + (9 - i) * 38;
        DrawLine(302, static_cast<int>(y + 30), 978, static_cast<int>(y + 30), Fade(line, .65f));
        ui.drawText(handNames[i], 304, y, 20, ink, true);
        ui.drawText(std::to_string(game.levels[i]), 628, y, 20, mint);
        ui.drawText(std::to_string(baseChips[i] + (game.levels[i] - 1) * (10 + 5 * i)) + "  x  " +
                        std::to_string(baseMult[i] + game.levels[i] - 1),
                    752, y, 20, blue, true);
    }
    ui.drawWrappedText(
        "All played cards add chips. Number cards use their rank, faces add 10, and Aces add 11. "
        "Relics apply after card bonuses. Win eight antes to unlock endless play.",
        {302, 592, 668, 95}, 17, muted);
    ui.drawText(
        "1-8 Select   Enter Play   Backspace Discard   R/S Sort   Esc Menu   F11 Fullscreen", 302,
        696, 14, muted);
}

void App::drawScoreDialog() {
    Score r = game.selected.empty() ? game.last : game.preview();
    ui.drawBox({359, 145, 562, 510}, panel, line);
    ui.drawText(handNames[static_cast<int>(r.hand)], 391, 175, 30, ink, true);
    ui.drawText(number(r.chips) + " chips  x  " + multiplierText(r.mult) + " mult", 391, 229, 25,
                blue, true);
    if (r.factor != 1) {
        std::ostringstream f;
        f << "Mode modifier: x" << std::setprecision(3) << r.factor;
        ui.drawText(f.str(), 391, 271, 20, coral, true);
    }
    for (size_t i = 0; i < r.effects.size(); ++i) {
        ui.drawFittedText(r.effects[i], {391, 320 + i * 34.f, 498, 26}, 18, muted);
    }
    if (r.effects.empty()) {
        ui.drawText("No relic bonuses on this hand.", 391, 320, 18, muted);
    }
    ui.drawFittedText("TOTAL  " + number(r.total), {391, 526, 498, 42}, 31, gold, true);
    if (ui.button({391, 587, 498, 42}, "Close", mint)) {
        modal = Modal::None;
    }
}

void App::drawRelicDialog() {
    int id = game.owned[relicDetailIndex];
    Color a = accent[relics[id].color];
    ui.drawBox({418, 170, 444, 432}, panel, a);
    ui.drawRelicEmblem(id, {640, 254}, 45, a);
    ui.drawCenteredText(relics[id].name, 640, 328, 31, ink, true);
    ui.drawWrappedText(relics[id].description, {461, 378, 358, 45}, 17, muted);
    if(ui.button({458,435,171,32},"Move left",mint,false,relicDetailIndex>0)){
        finishAction(game.moveRelic(relicDetailIndex,-1));--relicDetailIndex;
    }
    if(ui.button({651,435,171,32},"Move right",mint,false,relicDetailIndex+1<static_cast<int>(game.owned.size()))){
        finishAction(game.moveRelic(relicDetailIndex,1));++relicDetailIndex;
    }
    if (game.phase == Phase::Shop) {
        if (ui.button({458, 477, 364, 44}, "Sell  $" + std::to_string(relics[id].cost / 2),
                      coral)) {
            finishAction(game.sell(relicDetailIndex), 3);
            modal = Modal::None;
        }
    }
    if (ui.button({458, 536, 364, 40}, "Close", mint)) {
        modal = Modal::None;
    }
}

void App::drawNewRunDialog() {
    ui.drawBox({428, 239, 424, 321}, panel, line);
    ui.drawCenteredText("Start a new run?", 640, 272, 28, ink, true);
    ui.drawWrappedText("Your current run will be replaced.", {467, 328, 346, 60}, 19, muted);
    if (ui.button({468, 401, 344, 47}, "Start new run", coral, true)) {
        startConfiguredRun();
    }
    if (ui.button({468, 469, 344, 43}, "Keep playing", mint)) {
        modal = Modal::Menu;
    }
}

void App::drawMenuDialog() {
    ui.drawBox({428, 130, 424, 540}, panel, line);
    ui.drawCenteredText("NEON ANTE", 640, 164, 36, ink, true);
    ui.drawCenteredText("POKER", 640, 210, 15, mint, true);
    if (ui.button({468, 259, 344, 48}, "Resume", mint, true)) {
        modal = Modal::None;
    }
    if (ui.button({468, 324, 344, 45}, "New run", gold)) {
        pendingMode=static_cast<int>(game.mode);pendingStarter=game.starter;modal = Modal::ConfirmNewRun;
    }
    if (ui.button({468, 385, 344, 45}, audio.muted ? "Sound off" : "Sound on", blue)) {
        audio.muted = !audio.muted;
    }
    if (ui.button({468, 446, 344, 45}, "Hand reference", mint)) {
        modal = Modal::HandReference;
    }
    if (ui.button({468, 507, 344, 45}, "Save & quit", coral)) {
        saveRun();
        if (!saveFailed) {
            quitRequested = true;
        }
    }
    ui.drawCenteredText(saveFailed       ? "Could not save. Check folder access."
                        : previewSession ? "Preview session"
                                         : "Your run saves automatically.",
                        640, 600, 16, saveFailed ? coral : muted);
}
} // namespace neon
