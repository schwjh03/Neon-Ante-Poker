#include "game.hpp"

namespace neon {
const std::array<const char*, 10> handNames = {
    "High Card", "Pair",       "Two Pair",       "Three of a Kind", "Straight",
    "Flush",     "Full House", "Four of a Kind", "Straight Flush",  "Five of a Kind"};
const std::array<int, 10> baseChips = {5, 12, 22, 32, 42, 45, 58, 78, 105, 135};
const std::array<int, 10> baseMult = {1, 2, 2, 3, 4, 4, 5, 7, 9, 12};
const std::array<const char*, 5> bossNames = {"The Tax", "Velvet Lock", "Redout", "Dry Well",
                                              "The Needle"};
const std::array<const char*, 5> bossRules = {
    "First hand costs $2.", "Play exactly five cards.", "Hearts contribute no bonuses.",
    "No discards this blind.", "Target is 28% higher. +$3 reward."};
const std::array<Relic, 16> relics = {
    {{"Chrome Smile", "Face cards give +3 mult each.", 4, 0},
     {"Static Jacket", "Pairs and Two Pairs give +45 chips.", 5, 1},
     {"Glass Dealer", "Flushes score x1.8.", 6, 2},
     {"Lucky Teeth", "Each 7 gives +11 mult.", 5, 3},
     {"Ace Engine", "Aces give +26 chips and +2 mult.", 6, 0},
     {"Signal Crown", "Straights give +70 chips and +3 mult.", 7, 3},
     {"Afterimage", "Five cards played gives +5 mult.", 5, 2},
     {"Debt Collector", "Every $2 held gives +1 mult.", 8, 1},
     {"Blue Wire", "Spades give +9 chips. Clubs give +2 mult.", 5, 0},
     {"Royal Static", "An Ace and King together score x2.1.", 9, 3},
     {"Last Light", "The last hand of a blind scores x3.", 7, 1},
     {"Blood Moon", "Each red card gives +4 mult.", 7, 1},
     {"Compound", "Every hand scores x1.5.", 9, 2},
     {"Pocket Reactor", "Three or fewer cards gives +80 chips.", 6, 0},
     {"Deep Pockets", "Cleared blinds pay an extra $3.", 5, 3},
     {"Feedback Loop", "Gain +1 mult per blind reached.", 8, 2}}};

std::string number(int64_t n) {
    std::string s = std::to_string(n);
    for (int i = static_cast<int>(s.size()) - 3; i > 0; i -= 3) {
        s.insert(i, ",");
    }
    return s;
}

std::string rankName(int r) {
    return r <= 10 ? std::to_string(r) : std::string(1, "JQKA"[r - 11]);
}

} // namespace neon
