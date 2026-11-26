#pragma once

#include "game.hpp"
#include "theme.hpp"
#include <string>
#include <array>

namespace neon {
struct Scene3D;

// Shared immediate-mode drawing and pointer state for one application window.
class Ui {
public:
    Vector2 mouse{};
    bool clicked = false;
    bool interactive = true;

    void loadFonts();
    void unloadFonts();
    void prepareScene3D(const Game& game, const std::array<float,52>& lifts);
    void unloadScene3D();
    bool hasScene3D() const;
    void drawScene3D();
    void drawHand3D();
    void beginFrame(bool allowInteraction);
    void clearTooltip();
    void finishFrame();

    void drawText(const std::string& label, float x, float y, float size, Color color = theme::ink,
                  bool heavy = false);
    void drawDisplayText(const std::string& label, float x, float y, float size, Color color,
                         bool centered = false);
    float measureText(const std::string& label, float size, bool heavy = false);
    void drawCenteredText(const std::string& label, float x, float y, float size,
                          Color color = theme::ink, bool heavy = false);
    void drawFittedText(const std::string& label, Rectangle bounds, float size,
                        Color color = theme::ink, bool heavy = false);
    void drawWrappedText(const std::string& label, Rectangle bounds, float size,
                         Color color = theme::muted);
    void drawBox(Rectangle bounds, Color fill, Color edge = BLANK, float radius = .08f);

    bool isHovered(Rectangle bounds);
    void showTooltip(const std::string& label, Rectangle bounds);
    bool wasClicked(Rectangle bounds, bool enabled = true);
    bool button(Rectangle bounds, const std::string& label, Color tint = theme::mint,
                bool primary = false, bool enabled = true);

    void drawTableSurface(bool market);
    void drawChip(Vector2 center, float radius, Color color, int suit = 3);
    void drawSuit(int suit, Vector2 center, float size, Color color);
    void drawStar(Vector2 center, float radius, Color color);
    void drawCard(const Card& card, Rectangle bounds, bool selected = false, bool dim = false,
                  bool back = false);
    void drawRelicEmblem(int relicId, Vector2 center, float size, Color color);
    void drawRelic(int relicId, Rectangle bounds);

private:
    Scene3D* scene3D = nullptr;
    Font regular{};
    Font bold{};
    Font display{};
    bool consumed = false;
    bool handCursor = false;
    std::string tooltip;
};
} // namespace neon
