#pragma once
#include "game.hpp"
#include <filesystem>

namespace neon {
struct BuildProfile {
    uint32_t seed=0;
    int mode=0,ante=1,round=1;
    int64_t best=0,lifetime=0;
    std::array<int,10> levels{},handUse{};
    std::array<Card,52> deck{};
    std::vector<int> owned;
    bool valid() const;
    int favoredHand() const;
    double strength(size_t historyCount) const;
    std::string summary() const;
};
BuildProfile captureBuild(const Game& game);
bool exportBuild(const BuildProfile& profile,const std::filesystem::path& path);
bool importBuild(const std::filesystem::path& path,BuildProfile& profile);
class BuildLibrary {
public:
    std::vector<BuildProfile> profiles;
    bool add(const BuildProfile& profile);
    bool load(const std::filesystem::path& path);
    bool save(const std::filesystem::path& path) const;
    const BuildProfile* strongest() const;
};
uint32_t dailySeedFor(const std::string& day);
std::string utcDay();
std::filesystem::path chooseBuildFile(bool saving);
} // namespace neon
