#ifndef __EMSCRIPTEN__
#include "runtime.hpp"
#include "../tests/ui_smoke.hpp"
#include "app.hpp"
#include "preview.hpp"
#include <algorithm>
#include <string>

namespace neon {
namespace {
struct LaunchOptions {
    bool testing = false;
    bool smokeTest = false;
    std::string capturePath;
    std::string previewScene;
    int windowWidth = 1280;
    int windowHeight = 800;
};

LaunchOptions parseOptions(int argc, char** argv) {
    LaunchOptions options;
    for (int i = 1; i < argc; ++i) {
        std::string argument = argv[i];
        if (argument == "--test-session") {
            options.testing = true;
        } else if (argument == "--smoke-test") {
            options.smokeTest = true;
            options.testing = true;
        } else if (argument == "--capture" && i + 1 < argc) {
            options.capturePath = argv[++i];
            options.testing = true;
        } else if (argument == "--scene" && i + 1 < argc) {
            options.previewScene = argv[++i];
        } else if (argument == "--small") {
            options.windowWidth = 960;
            options.windowHeight = 600;
        }
    }
    return options;
}

bool openWindow(const LaunchOptions& options) {
    SetTraceLogLevel(LOG_WARNING);
    bool hidden = !options.capturePath.empty() || options.smokeTest;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | (hidden ? FLAG_WINDOW_HIDDEN : 0));
    InitWindow(options.windowWidth, options.windowHeight, "Neon Ante Poker - C++ Edition");
    if (!IsWindowReady()) {
        return false;
    }

    SetWindowMinSize(800, 500);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    Image icon = GenImageColor(64, 64, {18, 30, 34, 255});
    ImageDrawCircle(&icon, 32, 32, 23, theme::mint);
    SetWindowIcon(icon);
    UnloadImage(icon);
    return true;
}

int runFrames(App& app, RenderTexture2D canvas, const LaunchOptions& options) {
    UiSmokeTest smoke(app, options.smokeTest);
    int frame = 0;
    while (!WindowShouldClose() && !app.quitRequested) {
        float scale = std::min(GetScreenWidth() / theme::canvasWidth,
                               GetScreenHeight() / theme::canvasHeight);
        Vector2 offset = {(GetScreenWidth() - theme::canvasWidth * scale) / 2,
                          (GetScreenHeight() - theme::canvasHeight * scale) / 2};
        Vector2 pointer = GetMousePosition();
        app.ui.mouse = {(pointer.x - offset.x) / scale, (pointer.y - offset.y) / scale};
        app.ui.clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        app.update(std::min(GetFrameTime(), .05f));
        smoke.beforeFrame(frame);

        BeginTextureMode(canvas);
        app.render();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(theme::bg);
        DrawTexturePro(
            canvas.texture, {0, 0, theme::canvasWidth, -theme::canvasHeight},
            {offset.x, offset.y, theme::canvasWidth * scale, theme::canvasHeight * scale}, {0, 0},
            0, WHITE);
        EndDrawing();

        ++frame;
        smoke.afterFrame(frame);
        if (smoke.finished()) {
            return smoke.exitCode();
        }
        if (!options.capturePath.empty() && frame >= 4) {
            TakeScreenshot(options.capturePath.c_str());
            break;
        }
    }
    return 0;
}
} // namespace

int runApplication(int argc, char** argv) {
    LaunchOptions options = parseOptions(argc, argv);
    if (!openWindow(options)) {
        return 1;
    }

    int exitCode;
    {
        App app(options.testing);
        app.ui.loadFonts();
        RenderTexture2D canvas = LoadRenderTexture(static_cast<int>(theme::canvasWidth),
                                                   static_cast<int>(theme::canvasHeight));
        SetTextureFilter(canvas.texture, TEXTURE_FILTER_BILINEAR);
        app.audio.init();
        if (options.smokeTest || !options.capturePath.empty()) {
            app.audio.muted = true;
        }
        preparePreview(app, options.previewScene);

        exitCode = runFrames(app, canvas, options);
        app.saveRun();
        UnloadRenderTexture(canvas);
        app.ui.unloadFonts();
    }
    CloseWindow();
    return exitCode;
}
} // namespace neon

#endif
