#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "audio.hpp"
#include <cmath>
#include <cstring>
// The multimedia API requires Windows types to be declared first.
#include <windows.h>
#include <mmsystem.h>

void Audio::init() {
    const double frequencies[] = {600, 280, 720, 880, 160};
    for (int j = 0; j < 5; ++j) {
        const int samples = j == 2 ? 14000 : 6000;
        auto& bytes = waves[j];
        bytes.resize(44 + samples * 2);
        auto u16 = [&](int at, uint16_t n) {
            std::memcpy(bytes.data() + at, &n, 2);
        };
        auto u32 = [&](int at, uint32_t n) {
            std::memcpy(bytes.data() + at, &n, 4);
        };
        std::memcpy(bytes.data(), "RIFF", 4);
        u32(4, static_cast<uint32_t>(bytes.size() - 8));
        std::memcpy(bytes.data() + 8, "WAVEfmt ", 8);
        u32(16, 16);
        u16(20, 1);
        u16(22, 1);
        u32(24, 22050);
        u32(28, 44100);
        u16(32, 2);
        u16(34, 16);
        std::memcpy(bytes.data() + 36, "data", 4);
        u32(40, samples * 2);
        for (int i = 0; i < samples; ++i) {
            const double t = i / 22050.0, env = std::pow(1.0 - i / static_cast<double>(samples), 2);
            const double f = frequencies[j] * (j == 2 ? 1 + 1.3 * t : 1);
            auto value = static_cast<int16_t>(
                (std::sin(6.2831853 * f * t) + .3 * std::sin(6.2831853 * f * 1.5 * t)) * env *
                2300);
            std::memcpy(bytes.data() + 44 + i * 2, &value, 2);
        }
    }
}

void Audio::play(int index) {
    if (muted || index < 0 || index >= 5 || waves[index].empty()) {
        return;
    }
    PlaySoundA(reinterpret_cast<const char*>(waves[index].data()), nullptr,
               SND_ASYNC | SND_MEMORY | SND_NODEFAULT);
}

Audio::~Audio() {
    PlaySoundA(nullptr, nullptr, 0);
}
