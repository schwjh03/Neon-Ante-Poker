#include "ui.hpp"
#include <cmath>

namespace neon {
using namespace theme;

void Ui::drawRelicEmblem(int id, Vector2 p, float size, Color c) {
    const float k = size / 32;
    DrawPolyLinesEx(p, 6, size * .95f, 30, 1.5f, Fade(c, .45f));
    switch (id % 8) {
    case 0:
        DrawRectangleRounded({p.x - 20 * k, p.y - 10 * k, 40 * k, 22 * k}, .4f, 8, c);
        for (int i = -2; i < 3; ++i) {
            DrawLineEx({p.x + i * 7 * k, p.y - 7 * k}, {p.x + i * 7 * k, p.y + 7 * k}, 2,
                       {22, 31, 37, 255});
        }
        DrawLineEx({p.x - 16 * k, p.y}, {p.x + 16 * k, p.y}, 2, {22, 31, 37, 255});
        break;
    case 1:
        DrawTriangle({p.x + 5 * k, p.y - 26 * k}, {p.x - 19 * k, p.y + 4 * k},
                     {p.x + 4 * k, p.y + 4 * k}, c);
        DrawTriangle({p.x - 5 * k, p.y + 26 * k}, {p.x + 19 * k, p.y - 4 * k},
                     {p.x - 4 * k, p.y - 4 * k}, c);
        break;
    case 2:
        DrawPolyLinesEx(p, 4, 23 * k, 0, 3, c);
        DrawPolyLinesEx(p, 4, 13 * k, 0, 2, c);
        drawStar(p, 9 * k, c);
        break;
    case 3:
        drawCenteredText("7", p.x, p.y - 27 * k, 53 * k, c, true);
        break;
    case 4:
        DrawCircleLinesV(p, 21 * k, c);
        DrawCircleLinesV(p, 15 * k, c);
        drawSuit(3, p, 10 * k, c);
        for (int i = 0; i < 8; ++i) {
            float a = i * PI / 4;
            DrawLineEx({p.x + std::cos(a) * 23 * k, p.y + std::sin(a) * 23 * k},
                       {p.x + std::cos(a) * 29 * k, p.y + std::sin(a) * 29 * k}, 3, c);
        }
        break;
    case 5:
        for (int i = -1; i <= 1; ++i) {
            DrawTriangle({p.x + i * 15 * k, p.y - 22 * k}, {p.x + (i * 15 - 8) * k, p.y + 9 * k},
                         {p.x + (i * 15 + 8) * k, p.y + 9 * k}, c);
        }
        DrawRectangle(static_cast<int>(p.x - 23 * k), static_cast<int>(p.y + 9 * k),
                      static_cast<int>(46 * k), static_cast<int>(8 * k), c);
        break;
    case 6:
        for (int i = 0; i < 3; ++i) {
            DrawRectangleRoundedLinesEx(
                {p.x - 21 * k + i * 7 * k, p.y - 24 * k + i * 6 * k, 28 * k, 36 * k}, .1f, 8, 2,
                Fade(c, .35f + i * .3f));
        }
        break;
    default:
        drawCenteredText("$", p.x, p.y - 27 * k, 53 * k, c, true);
        break;
    }
}

void Ui::drawRelic(int id, Rectangle r) {
    Color a = accent[relics[id].color];
    drawBox({r.x + 2, r.y + 4, r.width, r.height}, {4, 11, 16, 140});
    drawBox(r, {31, 37, 44, 255}, Fade(a, .65f));
    DrawRectangleRec({r.x + 1, r.y + 1, r.width - 2, 3}, a);
    for (int i = 0; i < 6; ++i) {
        DrawLineEx({r.x + 8 + i * 21, r.y + 5}, {r.x + 2 + i * 21, r.y + 28}, 1, Fade(a, .1f));
    }
    drawRelicEmblem(id, {r.x + r.width / 2, r.y + 46}, 26, a);
    drawFittedText(relics[id].name, {r.x + 10, r.y + r.height - 28, r.width - 20, 22}, 18, ink,
                   true);
}
} // namespace neon
