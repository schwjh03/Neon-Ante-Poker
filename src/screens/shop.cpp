#include "app.hpp"

namespace neon {
using namespace theme;

void App::drawShop() {
    ui.drawText("BLIND CLEARED", 292, 267, 14, mint, true);
    ui.drawDisplayText("The Night Market", 292, 292, 35, ink);
    ui.drawText("+$" + std::to_string(game.reward), 1137, 285, 35, gold, true);
    ui.drawText("Blind $" + std::to_string(game.bankReward) + "  +  Hands $" +
                    std::to_string(game.hands) + "  +  Interest $" + std::to_string(game.interest),
                292, 339, 17, muted);
    for (size_t i = 0; i < game.offers.size(); ++i) {
        const Offer o = game.offers[i];
        Rectangle r = {292 + i * 193.f, 388, 177, 278};
        Color a = o.type == 0   ? accent[relics[o.id].color]
                  : o.type == 1 ? blue
                  : o.type == 2 ? mint
                                : gold;
        ui.drawBox(r, {25, 34, 38, 255}, Fade(a, o.sold ? .18f : .5f));
        ui.drawText(o.type == 0   ? "RELIC"
                    : o.type == 1 ? "CONSTELLATION"
                    : o.type == 2 ? "DECK UPGRADE"
                                  : "VOUCHER",
                    r.x + 12, r.y + 14, 11, a, true);
        if (o.type == 0) {
            ui.drawRelicEmblem(o.id, {r.x + 88, r.y + 80}, 33, o.sold ? muted : a);
        } else if (o.type == 1) {
            ui.drawStar({r.x + 88, r.y + 75}, 27, a);
            ui.drawStar({r.x + 56, r.y + 51}, 8, Fade(a, .65f));
            ui.drawStar({r.x + 118, r.y + 92}, 8, Fade(a, .65f));
        } else if (o.type == 2) {
            ui.drawCard({14, 3, o.id == 0, o.id == 1}, {r.x + 64, r.y + 42, 48, 71});
        } else {
            DrawRectangleRoundedLinesEx({r.x + 50, r.y + 54, 77, 46}, .1f, 8, 2, a);
            ui.drawCenteredText("+1", r.x + 88, r.y + 61, 28, a, true);
        }
        ui.drawFittedText(game.offerName(o), {r.x + 12, r.y + 127, 153, 24}, 20, ink, true);
        ui.drawWrappedText(game.offerDescription(o), {r.x + 12, r.y + 162, 153, 78}, 16, muted);
        if (ui.button({r.x + 12, r.y + 235, 153, 32},
                      o.sold ? "Sold" : "Buy  $" + std::to_string(o.cost), a, false,
                      !o.sold && game.cash >= o.cost)) {
            finishAction(game.buy(static_cast<int>(i)), 3);
        }
    }
    if (ui.button({292, 721, 212, 51}, "Reroll  $" + std::to_string(3 + game.rerolls), gold, false,
                  game.cash >= 3 + game.rerolls)) {
        finishAction(game.reroll(), 1);
    }
    if (ui.button({982, 721, 270, 51}, "Next blind", mint, true)) {
        finishAction(game.next(), 3);
        cardLift.fill(0);
    }
}
} // namespace neon
