#include "app.hpp"
#include <algorithm>

namespace neon {
using namespace theme;

void App::drawHeader() {
    DrawRectangle(0, 0, 1280, 74, bg);
    DrawLine(24, 74, 1256, 74, line);
    DrawCircleLines(42,36,23,Fade(gold,.7f));
    ui.drawSuit(3, {42, 35}, 15, mint);
    ui.drawDisplayText("Neon Ante", 78, 12, 32, ink);
    ui.drawText("POKER  /  CARD CLUB", 80, 49, 11, muted, true);
    ui.drawText(game.endless ? "ENDLESS TABLE" : "THE EIGHT-ANTE CIRCUIT", 440, 29, 15, muted,
                true);
    ui.drawCenteredText("ANTE " + std::to_string(game.ante) + (game.endless ? "" : " / 8"), 876, 24,
                        24, gold, true);
    if (ui.button({1000, 19, 56, 37}, audio.muted ? "M" : "~", mint)) {
        audio.muted = !audio.muted;
    }
    ui.showTooltip(audio.muted ? "Unmute sound (M)" : "Mute sound (M)", {1000, 19, 56, 37});
    if (ui.button({1066, 19, 56, 37}, "?", mint)) {
        modal = Modal::HandReference;
        audio.play(0);
    }
    ui.showTooltip("Hand reference (H)", {1066, 19, 56, 37});
    if (ui.button({1132, 19, 120, 37}, "Menu", mint)) {
        modal = Modal::Menu;
        audio.play(0);
    }
}

void App::drawSidebar() {
    DrawRectangle(0, 75, 264, 725, panel);
    DrawLine(264, 75, 264, 800, line);
    ui.drawText("CURRENT BLIND", 24, 99, 13, muted, true);
    Color c = game.blind == 2 ? coral : game.blind == 1 ? gold : mint;
    DrawCircle(49, 151, 25, Fade(c, .12f));
    DrawCircleLines(49, 151, 25, c);
    ui.drawSuit(game.blind == 2 ? 3 : 1, {49, 151}, 13, c);
    ui.drawFittedText(game.blindName(), {87, 132, 158, 28}, 25, ink, true);
    ui.drawText(game.blind == 2   ? "BOSS BLIND"
                : game.blind == 1 ? "SECOND BLIND"
                                  : "OPENING BLIND",
                87, 162, 11, c, true);
    ui.drawWrappedText(game.boss() < 0 ? "Reach the target before your hands run out."
                                       : bossRules[game.boss()],
                       {24, 199, 216, 68}, 16, muted);
    ui.drawText("TARGET", 24, 276, 13, muted, true);
    ui.drawFittedText(number(game.target()), {24, 297, 216, 40}, 37, ink, true);
    ui.drawBox({24, 346, 216, 8}, {11, 21, 23, 255}, BLANK, .7f);
    float fill = std::min(1.0, game.score / static_cast<double>(game.target()));
    if (fill > 0) {
        ui.drawBox({24, 346, 216 * fill, 8}, c, BLANK, .7f);
    }
    ui.drawFittedText(number(game.score) + " scored", {24, 368, 216, 24}, 21, c, true);
    DrawLine(24, 411, 240, 411, line);
    ui.drawText("HANDS", 24, 432, 13, muted, true);
    ui.drawText("DISCARDS", 135, 432, 13, muted, true);
    ui.drawText(std::to_string(game.hands), 24, 455, 40, blue, true);
    ui.drawText(std::to_string(game.discards), 135, 455, 40, coral, true);
    DrawLine(24, 515, 240, 515, line);
    ui.drawText("BANK", 24, 539, 13, muted, true);
    ui.drawText("$" + number(game.cash), 24, 561, 40, gold, true);
    ui.drawText("Interest  $" + std::to_string(std::min(5, game.cash / 5)) + " / blind", 24, 612,
                15, muted);
    ui.drawText("ROUND " + std::to_string(game.round), 24, 654, 13, muted, true);
    ui.drawText("Best hand  " + number(game.best), 24, 678, 16, ink);
    if (ui.button({24, 725, 216, 42}, "Deck  " + std::to_string(game.draw.size()) + " / 52",
                  mint)) {
        modal = Modal::Deck;
        audio.play(0);
    }
}

void App::drawRelicRow() {
    ui.drawText("RELICS", 292, 92, 13, muted, true);
    ui.drawText(std::to_string(game.owned.size()) + " / 5", 1209, 92, 13, muted, true);
    for (int i = 0; i < 5; ++i) {
        Rectangle r = {292 + i * 193.f, 118, 177, 114};
        if (i < static_cast<int>(game.owned.size())) {
            int id = game.owned[i];
            ui.drawRelic(id, r);
            ui.showTooltip(relics[id].description, r);
            if (ui.wasClicked(r)) {
                modal = Modal::Relic;
                relicDetailIndex = i;
                audio.play(0);
            }
        } else {
            ui.drawBox(r, Fade(bg, .25f), Fade(line, .65f), .12f);
            DrawCircleLines(r.x+r.width/2,165,17,Fade(gold,.2f));
            ui.drawDisplayText(std::to_string(i+1),r.x+r.width/2,153,22,Fade(gold,.35f),true);
            ui.drawCenteredText("RELIC SLOT", r.x + r.width / 2, 202, 11, Fade(muted, .65f));
        }
    }
}
} // namespace neon
