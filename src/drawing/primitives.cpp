#include "ui.hpp"
#include <sstream>

namespace neon {
using namespace theme;

void Ui::drawDisplayText(const std::string& s, float x, float y, float size, Color color,
                         bool centered) {
    if (centered) x -= MeasureTextEx(display, s.c_str(), size, 0).x / 2;
    DrawTextEx(display, s.c_str(), {x, y}, size, 0, color);
}

void Ui::drawText(const std::string& s, float x, float y, float size, Color color, bool heavy) {
    DrawTextEx(heavy ? bold : regular, s.c_str(), {x, y}, size, 0, color);
}

float Ui::measureText(const std::string& s, float size, bool heavy) {
    return MeasureTextEx(heavy ? bold : regular, s.c_str(), size, 0).x;
}

void Ui::drawCenteredText(const std::string& s, float x, float y, float size, Color color,
                          bool heavy) {
    drawText(s, x - measureText(s, size, heavy) / 2, y, size, color, heavy);
}

void Ui::drawFittedText(const std::string& s, Rectangle r, float size, Color color, bool heavy) {
    while (measureText(s, size, heavy) > r.width && size > 10) {
        size -= 1;
    }
    drawText(s, r.x, r.y, size, color, heavy);
}

void Ui::drawWrappedText(const std::string& s, Rectangle r, float size, Color color) {
    std::istringstream in(s);
    std::string word, row;
    float y = r.y;
    while (in >> word) {
        std::string next = row.empty() ? word : row + " " + word;
        if (measureText(next, size) > r.width && !row.empty()) {
            drawText(row, r.x, y, size, color);
            y += size + 5;
            row = word;
        } else {
            row = next;
        }
    }
    if (y + size <= r.y + r.height + 1) {
        drawText(row, r.x, y, size, color);
    }
}

void Ui::drawBox(Rectangle r, Color fill, Color edge, float radius) {
    DrawRectangleRounded(r, radius, 8, fill);
    if (edge.a) {
        DrawRectangleRoundedLinesEx(r, radius, 8, 1, edge);
    }
}

bool Ui::isHovered(Rectangle r) {
    return interactive && CheckCollisionPointRec(mouse, r);
}

void Ui::showTooltip(const std::string& s, Rectangle r) {
    if (isHovered(r)) {
        tooltip = s;
    }
}

bool Ui::wasClicked(Rectangle r, bool enabled) {
    if (isHovered(r) && enabled) {
        handCursor = true;
        if (clicked && !consumed) {
            consumed = true;
            return true;
        }
    }
    return false;
}

bool Ui::button(Rectangle r, const std::string& label, Color tint, bool primary, bool enabled) {
    bool hot = isHovered(r) && enabled;
    drawBox({r.x, r.y + 3, r.width, r.height}, {3, 8, 15, 180}, BLANK, .12f);
    Color fill = primary ? tint : panel;
    if (!enabled) {
        fill = {35, 45, 57, 255};
    } else if (hot) {
        fill = ColorBrightness(fill, .16f);
    }
    drawBox(r, fill, enabled ? (primary ? tint : line) : Color{48, 53, 57, 255});
    if (enabled) DrawLineEx({r.x + 6, r.y + 1}, {r.x + r.width - 6, r.y + 1}, 1,
                            Fade(primary ? WHITE : ink, hot ? .25f : .1f));
    drawCenteredText(label, r.x + r.width / 2, r.y + (r.height - 19) / 2, 19,
                     enabled ? (primary ? bg : ink) : Color{108, 118, 120, 255}, true);
    return wasClicked(r, enabled);
}
} // namespace neon
