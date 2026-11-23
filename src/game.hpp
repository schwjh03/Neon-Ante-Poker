#pragma once
#include <array>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

namespace neon {
enum class Phase { Play, Shop, Lost, Won };
enum class Hand {
    High,
    Pair,
    TwoPair,
    Three,
    Straight,
    Flush,
    FullHouse,
    Four,
    StraightFlush,
    Five
};

struct Card {
    int rank = 2;
    int suit = 0;
    bool foil = false;
    bool wild = false;
};

struct Relic {
    const char* name;
    const char* description;
    int cost;
    int color;
};

struct Offer {
    int type = 0;
    int id = 0;
    int cost = 0;
    bool sold = false;
};

struct Score {
    Hand hand = Hand::High;
    int chips = 0;
    int mult = 0;
    double factor = 1;
    int64_t total = 0;
    std::vector<std::string> effects;
};

extern const std::array<Relic, 16> relics;
extern const std::array<const char*, 10> handNames;
extern const std::array<int, 10> baseChips, baseMult;
extern const std::array<const char*, 5> bossNames, bossRules;
std::string number(int64_t n);
std::string rankName(int rank);
Hand classify(const std::vector<Card>& cards);

class Game {
public:
    Phase phase = Phase::Play;
    int ante = 1;
    int blind = 0;
    int cash = 6;
    int hands = 4;
    int discards = 3;
    int handBonus = 0;
    int discardBonus = 0;
    int round = 1;
    int rerolls = 0;
    int reward = 0;
    int interest = 0;
    int bankReward = 0;
    int64_t score = 0;
    int64_t best = 0;
    int64_t lifetime = 0;
    bool taxed = false;
    bool endless = false;
    std::array<int, 10> levels{};
    std::array<Card, 52> deck{};
    std::vector<int> draw;
    std::vector<int> hand;
    std::vector<int> selected;
    std::vector<int> owned;
    std::vector<Offer> offers;
    std::vector<Card> lastCards;
    Score last;
    std::string message;
    std::mt19937 rng;

    explicit Game(uint32_t seed = std::random_device{}());
    void newRun(uint32_t seed);
    int boss() const;
    int64_t target() const;
    std::string blindName() const;
    void startBlind();
    void fillHand();
    void sort(bool bySuit);
    bool toggle(int id);
    std::vector<Card> selectedCards() const;
    Score evaluate(const std::vector<Card>& cards) const;
    Score preview() const;
    bool play();
    bool discard();
    bool next();
    void makeShop();
    bool buy(int index);
    bool sell(int index);
    bool reroll();
    std::string offerName(const Offer& o) const;
    std::string offerDescription(const Offer& o) const;
    bool save(const std::string& path) const;
    bool load(const std::string& path);

private:
    void removeSelected();
    bool valid() const;
};
} // namespace neon
