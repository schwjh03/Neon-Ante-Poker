#include "ui.hpp"
#include <sstream>
#include <algorithm>
#include <vector>

namespace neon {
using namespace theme;

void Ui::drawDisplayText(const std::string& s, float x, float y, float size, Color color,
                         bool centered) {
    if (centered) x -= MeasureTextEx(display, s.c_str(), size, 0).x / 2;
    DrawTextEx(display, s.c_str(), {x, y}, size, 0, color);
}

void Ui::drawText(const std::string& s, float x, float y, float size, Color color, bool heavy) {
    size = std::max(16.f, size);
    DrawTextEx(heavy ? bold : regular, s.c_str(), {x, y}, size, .25f, color);
}

float Ui::measureText(const std::string& s, float size, bool heavy) {
    return MeasureTextEx(heavy ? bold : regular, s.c_str(), std::max(16.f, size), .25f).x;
}

void Ui::drawCenteredText(const std::string& s, float x, float y, float size, Color color,
                          bool heavy) {
    drawText(s, x - measureText(s, size, heavy) / 2, y, size, color, heavy);
}

void Ui::drawFittedText(const std::string& s, Rectangle r, float size, Color color, bool heavy) {
    size = std::max(16.f, size);
    while (measureText(s, size, heavy) > r.width && size > 16) --size;
    std::string shown=s;
    if(measureText(shown,size,heavy)>r.width){
        while(!shown.empty()&&measureText(shown+"...",size,heavy)>r.width)shown.pop_back();
        shown+="...";showTooltip(s,r);
    }
    drawText(shown, r.x, r.y, size, color, heavy);
}

void Ui::drawWrappedText(const std::string& s, Rectangle r, float size, Color color) {
    size=std::max(18.f,size);
    std::istringstream in(s);std::string word,row;std::vector<std::string> rows;
    while(in>>word){const std::string next=row.empty()?word:row+" "+word;
        if(measureText(next,size)>r.width&&!row.empty()){rows.push_back(row);row=word;}
        else row=next;
    }
    if(!row.empty())rows.push_back(row);
    const int capacity=std::max(1,static_cast<int>((r.height+5)/(size+5)));
    for(int i=0;i<std::min(capacity,static_cast<int>(rows.size()));++i){
        if(i==capacity-1&&static_cast<int>(rows.size())>capacity){
            auto text=rows[i];while(!text.empty()&&measureText(text+"...",size)>r.width)text.pop_back();
            drawText(text+"...",r.x,r.y+i*(size+5),size,color);showTooltip(s,r);
        }else drawText(rows[i],r.x,r.y+i*(size+5),size,color);
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
    const float textSize=r.width<40?18.f:20.f;
    drawCenteredText(label, r.x + r.width / 2, r.y + (r.height - textSize) / 2, textSize,
                     enabled ? (primary ? bg : ink) : Color{170, 184, 200, 255}, true);
    return wasClicked(r, enabled);
}
} // namespace neon
