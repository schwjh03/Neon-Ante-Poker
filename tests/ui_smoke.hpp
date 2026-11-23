#pragma once

#include "raylib.h"
#include <fstream>
#include <functional>
#include <vector>

namespace neon {
class App;

// Injects clicks into the same Ui context used by real mouse input.
class UiSmokeTest {
public:
    explicit UiSmokeTest(App& app, bool enabled);
    void beforeFrame(int frame);
    void afterFrame(int frame);
    bool finished() const;
    int exitCode() const;

private:
    struct Step {
        const char* label;
        Vector2 position;
        std::function<bool()> verify;
    };

    App& app;
    bool enabled;
    bool acting = false;
    bool done = false;
    int result = 0;
    size_t nextStep = 0;
    std::vector<Step> steps;
    std::ofstream report;
};
} // namespace neon
