#include "app.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>

namespace neon {
using namespace theme;

App::App(bool testing) : previewSession(testing) {
    if (previewSession) {
        game.newRun(32491);
        return;
    }

    const char* localAppData = std::getenv("LOCALAPPDATA");
    auto directory = localAppData ? std::filesystem::path(localAppData) / "NeonAntePoker"
                                  : std::filesystem::path(GetApplicationDirectory());

    std::error_code error;
    std::filesystem::create_directories(directory, error);
    savePath = (directory / "run-v2.save").string();
    libraryPath=directory / "build-library.txt";library.load(libraryPath);
    if(!game.load(savePath)){
        const bool migrate=!std::filesystem::exists(savePath)&&game.load((directory/"run.save").string());
        if(!migrate)modal=Modal::RunSetup;
    }
    pendingMode=static_cast<int>(game.mode);pendingStarter=game.starter;
    saveFailed = static_cast<bool>(error);
}

void App::saveRun() {
    if (!previewSession) {
        bool archiveOK=true;
        if((game.phase==Phase::Lost||game.phase==Phase::Won)&&!game.archived){
            BuildLibrary nextLibrary=library;
            if(!nextLibrary.add(captureBuild(game)))game.archived=true;
            else if(nextLibrary.save(libraryPath)){library=std::move(nextLibrary);game.archived=true;}
            else archiveOK=false;
        }
        saveFailed = !game.save(savePath)||!archiveOK;
    }
}

void App::finishAction(bool succeeded, int sound) {
    if (succeeded) {
        audio.play(sound);
        saveRun();
    } else {
        audio.play(4);
    }
    noticeTimer = 3;
}

void App::playSelectedHand() {
    if (!game.play()) {
        finishAction(false);
        return;
    }

    scoreAnimation = 1;
    screenTransition = game.phase != Phase::Play ? 1.3f : 0;
    burst(game.phase == Phase::Lost ? coral : gold);
    finishAction(true, 2);
}

void App::burst(Color color) {
    for (int i = 0; i < 60; ++i) {
        particles.push_back({{780, 380},
                             {static_cast<float>(GetRandomValue(-190, 190)),
                              static_cast<float>(GetRandomValue(-220, 20))},
                             static_cast<float>(GetRandomValue(50, 140)) / 100,
                             color});
    }
}

void App::updateAnimations(float deltaTime) {
    scoreAnimation = std::max(0.f, scoreAnimation - deltaTime);
    screenTransition = std::max(0.f, screenTransition - deltaTime);
    noticeTimer = std::max(0.f, noticeTimer - deltaTime);

    for (auto& particle : particles) {
        particle.life -= deltaTime;
        particle.position.x += particle.velocity.x * deltaTime;
        particle.position.y += particle.velocity.y * deltaTime;
        particle.velocity.y += 250 * deltaTime;
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
                                   [](const Particle& particle) {
                                       return particle.life <= 0;
                                   }),
                    particles.end());

    for (int cardId = 0; cardId < 52; ++cardId) {
        bool selected =
            std::find(game.selected.begin(), game.selected.end(), cardId) != game.selected.end();
        float destination = selected ? 20 : 0;
        cardLift[cardId] += (destination - cardLift[cardId]) * std::min(1.f, deltaTime * 16);
    }
}

void App::handleKeyboard() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        modal = modal == Modal::None ? Modal::Menu : Modal::None;
        relicDetailIndex = -1;
    }
    if (IsKeyPressed(KEY_F11)) {
        ToggleBorderlessWindowed();
    }
    if (IsKeyPressed(KEY_M)) {
        audio.muted = !audio.muted;
    }
    if (modal != Modal::None || screenTransition > 0) {
        return;
    }

    if (IsKeyPressed(KEY_H)) {
        modal = Modal::HandReference;
    }
    if (IsKeyPressed(KEY_TAB)) {
        modal = Modal::Deck;
    }
    if (game.phase != Phase::Play) {
        return;
    }

    for (int index = 0; index < 8; ++index) {
        if (IsKeyPressed(KEY_ONE + index) && index < static_cast<int>(game.hand.size())) {
            finishAction(game.toggle(game.hand[index]));
        }
    }
    if (IsKeyPressed(KEY_ENTER)) {
        playSelectedHand();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        finishAction(game.discard(), 1);
    }
    if (IsKeyPressed(KEY_R)) {
        sortBySuit = false;
        game.sort(false);
        saveRun();
    }
    if (IsKeyPressed(KEY_S)) {
        sortBySuit = true;
        game.sort(true);
        saveRun();
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        game.selected.clear();
        saveRun();
    }
}

void App::update(float deltaTime) {
    updateAnimations(deltaTime);
    handleKeyboard();
}

void App::render() {
    ui.beginFrame(modal == Modal::None && screenTransition <= 0);
    ui.drawTableSurface(game.phase==Phase::Shop);

    drawHeader();
    drawSidebar();
    drawRelicRow();

    if (game.phase == Phase::Play || screenTransition > 0) {
        drawTable();
    } else if (game.phase == Phase::Shop) {
        drawShop();
    } else {
        drawEndScreen();
    }

    for (const auto& particle : particles) {
        DrawRectanglePro({particle.position.x, particle.position.y, 4, 8}, {2, 4},
                         particle.life * 190, Fade(particle.color, std::min(1.f, particle.life)));
    }
    if (noticeTimer > 0 && modal == Modal::None) {
        float noticeWidth = std::min(730.f, ui.measureText(game.message, 17) + 38);
        ui.drawBox({772 - noticeWidth / 2, 79, noticeWidth, 30}, {11, 22, 25, 245},
                   Fade(mint, .3f));
        ui.drawCenteredText(game.message, 772, 84, 17, ink);
    }
    if (modal != Modal::None) {
        ui.clearTooltip();
        drawDialogs();
    }
    ui.finishFrame();
}

void App::drawDialogs() {
    DrawRectangle(0, 0, 1280, 800, {4, 9, 12, 210});
    ui.interactive = true;

    switch (modal) {
    case Modal::Deck:
        drawDeckDialog();
        break;
    case Modal::HandReference:
        drawHandReferenceDialog();
        break;
    case Modal::ScoreDetails:
        drawScoreDialog();
        break;
    case Modal::Relic:
        if (relicDetailIndex >= 0 && relicDetailIndex < static_cast<int>(game.owned.size())) {
            drawRelicDialog();
        } else {
            drawMenuDialog();
        }
        break;
    case Modal::RunSetup:drawRunSetup();break;
    case Modal::Contract:drawContractDialog();break;
    case Modal::Draft:drawDraftDialog();break;
    case Modal::ConfirmNewRun:
        drawNewRunDialog();
        break;
    default:
        drawMenuDialog();
        break;
    }
}
} // namespace neon
