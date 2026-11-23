#include "preview.hpp"
#include "app.hpp"

namespace neon {
void preparePreview(App& app, const std::string& scene) {
    if (scene == "shop") {
        app.game.phase = Phase::Shop;
        app.game.score = app.game.target();
        app.game.cash = 22;
        app.game.reward = 8;
        app.game.bankReward = 4;
        app.game.hands = 3;
        app.game.interest = 1;
        app.game.makeShop();
    }
    if (scene == "deck") {
        app.modal = Modal::Deck;
    }
    if (scene == "rules") {
        app.modal = Modal::HandReference;
    }
    if (scene == "lost") {
        app.game.phase = Phase::Lost;
    }
    if (scene == "won") {
        app.game.phase = Phase::Won;
        app.game.ante = 8;
        app.game.blind = 2;
    }
    if (scene == "selected") {
        for (int i = 0; i < 5; ++i) {
            app.game.toggle(app.game.hand[i]);
        }
        for (int cardId : app.game.selected) {
            app.cardLift[cardId] = 20;
        }
    }
}
} // namespace neon
