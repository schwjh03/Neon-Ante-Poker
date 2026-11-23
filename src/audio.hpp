#pragma once
#include <array>
#include <cstdint>
#include <vector>

class Audio {
    std::array<std::vector<uint8_t>, 5> waves;

public:
    bool muted = false;
    void init();
    void play(int index);
    ~Audio();
};
