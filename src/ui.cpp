#include "ui.hpp"
#include <algorithm>
#include <cstdlib>

namespace neon {
using namespace theme;

void Ui::loadFonts() {
    const char* windowsDirectory = std::getenv("WINDIR");
    std::string fontDirectory =
        std::string(windowsDirectory ? windowsDirectory : "C:/Windows") + "/Fonts/";
    regular = LoadFontEx((fontDirectory + "segoeui.ttf").c_str(), 48, nullptr, 0);
    bold = LoadFontEx((fontDirectory + "segoeuib.ttf").c_str(), 72, nullptr, 0);
    display = LoadFontEx((fontDirectory + "georgiab.ttf").c_str(), 96, nullptr, 0);

    if (!regular.texture.id) {
        regular = GetFontDefault();
    }
    if (!bold.texture.id) {
        bold = GetFontDefault();
    }
    if (!display.texture.id) {
        display = GetFontDefault();
    }
    SetTextureFilter(regular.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(bold.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(display.texture, TEXTURE_FILTER_BILINEAR);
}

void Ui::unloadFonts() {
    if (regular.texture.id != GetFontDefault().texture.id) {
        UnloadFont(regular);
    }
    if (bold.texture.id != GetFontDefault().texture.id) {
        UnloadFont(bold);
    }
    if (display.texture.id != GetFontDefault().texture.id) {
        UnloadFont(display);
    }
}

void Ui::beginFrame(bool allowInteraction) {
    tooltip.clear();
    handCursor = false;
    consumed = false;
    interactive = allowInteraction;
}

void Ui::clearTooltip() {
    tooltip.clear();
}

void Ui::finishFrame() {
    if (!tooltip.empty()) {
        float tooltipWidth = std::min(390.f, measureText(tooltip, 16) + 28);
        float x = std::clamp(mouse.x - tooltipWidth / 2, 8.f, 1272 - tooltipWidth);
        float y = std::clamp(mouse.y - 77, 8.f, 720.f);
        drawBox({x, y, tooltipWidth, 61}, {9, 16, 22, 250}, line);
        drawWrappedText(tooltip, {x + 12, y + 10, tooltipWidth - 24, 46}, 16, ink);
    }
    SetMouseCursor(handCursor ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
}
} // namespace neon
