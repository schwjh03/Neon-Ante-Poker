#pragma once

#include "audio.hpp"
#include "game.hpp"
#include "ui.hpp"
#include "build_profiles.hpp"
#include <array>
#include <string>
#include <vector>

namespace neon {

enum class Modal { None, Menu, HandReference, Deck, Relic, ConfirmNewRun, ScoreDetails, RunSetup, Contract, Draft };

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float life;
    Color color;
};

// The engine owns run rules; App owns input, screen state, and presentation.
class App {
public:
    explicit App(bool testing);
    void update(float deltaTime);
    void render();
    void saveRun();

    Game game;
    Ui ui;
    Audio audio;
    Modal modal = Modal::None;
    bool quitRequested = false;
    float screenTransition = 0;
    std::array<float, 52> cardLift{};

private:
    int relicDetailIndex = -1;
    bool sortBySuit = false;
    bool previewSession = false;
    bool saveFailed = false;
    float scoreAnimation = 0;
    float noticeTimer = 0;
    std::vector<Particle> particles;
    std::string savePath;
    std::filesystem::path libraryPath;
    BuildLibrary library;
    int pendingMode=0, pendingStarter=0;
    std::string setupMessage;

    void updateAnimations(float deltaTime);
    void handleKeyboard();
    void burst(Color color);
    void finishAction(bool succeeded, int sound = 0);
    void playSelectedHand();

    void drawHeader();
    void drawSidebar();
    void drawRelicRow();
    void drawScorePreview();
    void drawTable();
    void drawShop();
    void drawEndScreen();
    void drawDialogs();
    void drawDeckDialog();
    void drawHandReferenceDialog();
    void drawScoreDialog();
    void drawRelicDialog();
    void drawNewRunDialog();
    void drawMenuDialog();
    void drawRunSetup();
    void drawContractDialog();
    void drawDraftDialog();
    void startConfiguredRun();
};
} // namespace neon
