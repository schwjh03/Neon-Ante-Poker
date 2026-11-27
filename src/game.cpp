#include "game.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace neon {
Game::Game(uint32_t seed) {
    newRun(seed);
}

void Game::newRun(uint32_t seed, RunMode newMode, int newStarter) {
    mode=newMode;starter=std::clamp(newStarter,0,2);runSeed=seed;archived=false;handUse.fill(0);echoScale=1;echoHand=-1;
    clubChoices.fill(0);
    rng.seed(seed);
    phase = Phase::Play;
    ante = round = 1;
    blind = 0;
    cash = 6;
    handBonus = 0;discardBonus = starter==1?1:0;
    if(starter==1)cash=4;
    score = best = lifetime = 0;
    endless = false;
    levels.fill(1);
    owned = {0};
    offers.clear();
    for (int i = 0; i < 52; ++i) {
        deck[i] = {i / 4 + 2, i % 4, false, false};
    }
    startBlind();
    message = "Chrome Smile joins the table.";
}

int Game::boss() const {
    return blind == 2 ? (ante - 1) % 5 : -1;
}

int64_t Game::target() const {
    const int b[] = {300, 475, 720};
    return static_cast<int64_t>(std::round(
        std::min(9e14, b[blind] * echoScale * std::pow(1.55, ante - 1) * (boss() == 4 ? 1.28 : 1.0))));
}

std::string Game::blindName() const {
    return blind == 2 ? bossNames[boss()] : (blind == 0 ? "Small Blind" : "Big Blind");
}

void Game::startBlind() {
    phase = Phase::Play;
    lastHandType=-1;rhythm=0;playedMask=0;discardsSpent=0;contract=0;contractChosen=false;contractReward=0;draftChoices.clear();draftTaken=true;
    score = 0;
    hands = 4 + handBonus;
    discards = boss() == 3 ? 0 : 3 + discardBonus;
    taxed = false;
    draw.resize(52);
    std::iota(draw.begin(), draw.end(), 0);
    std::shuffle(draw.begin(), draw.end(), rng);
    hand.clear();
    selected.clear();
    lastCards.clear();
    last = {};
    offers.clear();
    fillHand();
    sort(false);
    message = blindName() + ". Target: " + number(target()) + ".";
}

void Game::fillHand() {
    while (hand.size() < 8 && !draw.empty()) {
        hand.push_back(draw.back());
        draw.pop_back();
    }
}

void Game::sort(bool bySuit) {
    std::stable_sort(hand.begin(), hand.end(), [&](int a, int b) {
        if (bySuit && deck[a].suit != deck[b].suit) {
            return deck[a].suit < deck[b].suit;
        }
        if (deck[a].rank != deck[b].rank) {
            return deck[a].rank > deck[b].rank;
        }
        return deck[a].suit < deck[b].suit;
    });
}

bool Game::toggle(int id) {
    if (phase != Phase::Play || std::find(hand.begin(), hand.end(), id) == hand.end()) {
        return false;
    }
    auto it = std::find(selected.begin(), selected.end(), id);
    if (it != selected.end()) {
        selected.erase(it);
        return true;
    }
    if (selected.size() >= 5) {
        message = "Five cards maximum.";
        return false;
    }
    selected.push_back(id);
    return true;
}

std::vector<Card> Game::selectedCards() const {
    std::vector<Card> cards;
    for (int id : selected) {
        cards.push_back(deck[id]);
    }
    return cards;
}

void Game::removeSelected() {
    hand.erase(std::remove_if(hand.begin(), hand.end(),
                              [&](int id) {
                                  return std::find(selected.begin(), selected.end(), id) !=
                                         selected.end();
                              }),
               hand.end());
    selected.clear();
    fillHand();
}

bool Game::play() {
    if (phase != Phase::Play || hands <= 0 || selected.empty()) {
        return false;
    }
    if (boss() == 1 && selected.size() != 5) {
        message = "Velvet Lock demands exactly five cards.";
        return false;
    }
    contractChosen=true;
    lastCards = selectedCards();
    last = evaluate(lastCards);
    const int handType=static_cast<int>(last.hand);
    rhythm=lastHandType>=0&&lastHandType!=handType?std::min(3,rhythm+1):0;lastHandType=handType;
    playedMask|=1<<handType;++handUse[handType];
    if (boss() == 0 && !taxed) {
        cash = std::max(0, cash - 2);
        taxed = true;
    }
    score += last.total;
    lifetime = std::min<int64_t>(9000000000000000000LL,lifetime+last.total);
    best = std::max(best, last.total);
    --hands;
    removeSelected();
    message =
        std::string(handNames[static_cast<int>(last.hand)]) + " scored " + number(last.total) + ".";
    if (score >= target()) {
        bankReward = (blind == 0 ? 4 : blind == 1 ? 6 : 9) + (boss() == 4 ? 3 : 0);
        if (std::find(owned.begin(), owned.end(), 14) != owned.end()) {
            bankReward += 3;
        }
        interest = std::min(5, cash / 5);
        contractReward=completedContract(last,lastCards.size());
        reward = bankReward + hands + interest + contractReward;
        cash += reward;
        phase = (ante == 8 && blind == 2 && !endless) ? Phase::Won : Phase::Shop;
        rerolls = 0;
        makeShop();
        prepareDraft();
        message = "Blind cleared. +$" + std::to_string(reward) + " earned.";
    } else if (hands == 0 || hand.empty()) {
        phase = Phase::Lost;
        message = "The house holds. Your next run is waiting.";
    }
    return true;
}

bool Game::discard() {
    if (phase != Phase::Play || discards <= 0 || selected.empty()) {
        return false;
    }
    contractChosen=true;++discardsSpent;
    --discards;
    removeSelected();
    message = "Fresh cards dealt.";
    return true;
}

bool Game::next() {
    if (phase == Phase::Won) {
        endless = true;archived=false;
        phase = Phase::Shop;
        return true;
    }
    if (phase != Phase::Shop) {
        return false;
    }
    if(mode==RunMode::Draft&&!draftTaken){message="Choose a free relic draft or pass first.";return false;}
    ++blind;
    ++round;
    if (blind == 3) {
        blind = 0;
        ++ante;
    }
    startBlind();
    return true;
}

} // namespace neon
