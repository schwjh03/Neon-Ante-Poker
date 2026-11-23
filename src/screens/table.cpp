#include "app.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace neon {
using namespace theme;

void App::drawScorePreview() {
    bool choosing = !game.selected.empty();
    Score result = choosing ? game.preview() : game.last;
    std::string name =
        result.total ? handNames[static_cast<int>(result.hand)] : "The table is open";
    ui.drawCenteredText(choosing       ? "SELECTED HAND"
                        : result.total ? "LAST HAND"
                                       : "ROUND " + std::to_string(game.round),
                        772, 317, 13, muted, true);
    ui.drawDisplayText(name, 772, 338, 34, result.total ? ink : Fade(ink, .8f), true);
    if (result.total) {
        std::string chips = number(result.chips), mult = std::to_string(result.mult);
        ui.drawBox({552, 386, 112, 48}, {25, 63, 72, 255}, Fade(blue,.6f));
        ui.drawCenteredText(chips, 608, 393, 29, blue, true);
        ui.drawCenteredText("x", 684, 397, 23, muted);
        ui.drawBox({704, 386, 91, 48}, {72, 37, 42, 255}, Fade(coral,.6f));
        ui.drawCenteredText(mult, 749, 393, 29, coral, true);
        std::ostringstream factor;
        factor << "x" << std::fixed << std::setprecision(1) << result.factor;
        ui.drawText(result.factor > 1 ? factor.str() + "  =" : "=", 812, 399, 20, muted);
        ui.drawFittedText(number(result.total), {902, 390, 286, 45}, 35, gold, true);
        ui.drawCenteredText("CHIPS",608,438,11,muted,true);
        ui.drawCenteredText("MULTIPLIER",749,438,11,muted,true);
        ui.drawText("HAND SCORE",902,438,11,muted,true);
        if (!choosing && scoreAnimation > 0) {
            ui.drawCenteredText("+" + number(result.total), 1080, 450 - scoreAnimation * 35, 28,
                                Fade(gold, scoreAnimation), true);
        }
        if (ui.button({1204, 341, 36, 32}, "i", mint)) {
            modal = Modal::ScoreDetails;
        }
        ui.showTooltip("Score details", {1204, 341, 36, 32});
    } else {
        for (int i = 0; i < 4; ++i) {
            ui.drawSuit(i, {727 + i * 30.f, 409}, 9, Fade(muted, .65f));
        }
    }
    if (!game.lastCards.empty()) {
        float cw = 46, gap = 8, span = game.lastCards.size() * (cw + gap) - gap;
        for (size_t i = 0; i < game.lastCards.size(); ++i) {
            ui.drawCard(game.lastCards[i], {772 - span / 2 + i * (cw + gap), 448, cw, 67}, false,
                        true);
        }
    } else {
        DrawLine(636, 471, 908, 471, Fade(muted, .14f));
        ui.drawCenteredText("NEON ANTE  /  CARD CLUB", 772, 480, 13, Fade(muted, .65f), true);
    }
}

void App::drawTable() {
    for (int i = 0; i < 3; ++i) {
        float x = 292 + i * 321.f;
        Color c = i == game.blind ? mint : Fade(muted, .4f);
        ui.drawText(std::to_string(i + 1) + "  " +
                        std::array<const char*, 3>{"SMALL BLIND", "BIG BLIND", "BOSS BLIND"}[i],
                    x, 267, 13, c, true);
        DrawRectangle(static_cast<int>(x), 294, 302, 2, i == game.blind ? mint : Fade(line, .7f));
    }
    drawScorePreview();
    ui.drawText("YOUR HAND", 292, 537, 13, muted, true);
    ui.drawText(std::to_string(game.selected.size()) + " / 5 SELECTED", 1062, 537, 13, muted, true);
    float cw = 104, gap = 13, span = game.hand.size() * (cw + gap) - gap, start = 772 - span / 2;
    for (size_t i = 0; i < game.hand.size(); ++i) {
        int id = game.hand[i];
        bool sel = std::find(game.selected.begin(), game.selected.end(), id) != game.selected.end();
        Rectangle r = {start + i * (cw + gap), 577 - cardLift[id], cw, 145};
        bool hot = ui.isHovered(r);
        if (hot) {
            r.y -= 4;
        }
        ui.drawCard(game.deck[id], r, sel, game.boss() == 2 && game.deck[id].suit == 2);
        ui.drawBox({r.x+cw/2-9, 724,18,14},Fade(bg,.8f),Fade(line,.7f),.15f);
        ui.drawCenteredText(std::to_string(i+1),r.x+cw/2,725,10,muted,true);
        if (ui.wasClicked(r) && screenTransition <= 0) {
            finishAction(game.toggle(id));
        }
        if (hot) {
            std::string info = rankName(game.deck[id].rank) + " of " +
                               std::array<const char*, 4>{"Clubs", "Diamonds", "Hearts",
                                                          "Spades"}[game.deck[id].suit];
            if (game.deck[id].foil) {
                info += " | Foil: +35 chips";
            }
            if (game.deck[id].wild) {
                info += " | Wild suit";
            }
            ui.showTooltip(info, r);
        }
    }
    bool valid = game.phase == Phase::Play && !game.selected.empty() && screenTransition <= 0;
    if (ui.button({292, 742, 230, 42}, "Play hand", mint, true,
                  valid && !(game.boss() == 1 && game.selected.size() != 5))) {
        playSelectedHand();
    }
    if (ui.button({535, 742, 174, 42}, "Discard", coral, false, valid && game.discards > 0)) {
        finishAction(game.discard(), 1);
    }
    ui.drawText("SORT", 755, 755, 13, muted, true);
    if (ui.button({805, 742, 92, 42}, "Rank", blue, !sortBySuit)) {
        game.sort(false);
        sortBySuit = false;
        finishAction(true);
    }
    if (ui.button({907, 742, 92, 42}, "Suit", blue, sortBySuit)) {
        game.sort(true);
        sortBySuit = true;
        finishAction(true);
    }
    if (ui.button({1080, 742, 172, 42}, "Clear", mint, false, valid)) {
        game.selected.clear();
        finishAction(true);
    }
}
} // namespace neon
