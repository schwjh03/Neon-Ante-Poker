#include "app.hpp"

namespace neon {
using namespace theme;

void App::drawEndScreen() {
    bool won = game.phase == Phase::Won;
    Color c = won ? gold : coral;
    ui.drawRelicEmblem(won ? 5 : 2, {772, 345}, 54, c);
    ui.drawCenteredText(won ? "The house is yours." : "The house holds.", 772, 427, 43, ink, true);
    ui.drawCenteredText(won ? "Eight antes. One champion."
                            : "Ante " + std::to_string(game.ante) + "  /  " + game.blindName(),
                        772, 489, 20, muted);
    ui.drawCenteredText("Best hand  " + number(game.best) + "     Total scored  " +
                            number(game.lifetime),
                        772, 534, 20, c, true);
    if (won) {
        if (ui.button({652, 604, 240, 52}, "Play endless  >", mint, true)) {
            finishAction(game.next(), 3);
        }
    } else if (ui.button({652, 604, 240, 52}, "New run", mint, true)) {
        game.newRun(std::random_device{}());
        cardLift.fill(0);
        finishAction(true, 3);
    }
}
} // namespace neon
