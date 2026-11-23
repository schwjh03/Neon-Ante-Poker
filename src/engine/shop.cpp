#include "game.hpp"
#include <algorithm>

namespace neon {
void Game::makeShop() {
    offers.clear();
    std::vector<int> pool;
    for (int i = 0; i < static_cast<int>(relics.size()); ++i) {
        if (std::find(owned.begin(), owned.end(), i) == owned.end()) {
            pool.push_back(i);
        }
    }
    std::shuffle(pool.begin(), pool.end(), rng);
    for (int i = 0; i < std::min(2, static_cast<int>(pool.size())); ++i) {
        offers.push_back({0, pool[i], relics[pool[i]].cost, false});
    }
    offers.push_back({1, static_cast<int>(rng() % 9), 3, false});
    offers.push_back({2, static_cast<int>(rng() % 3), 4, false});
    offers.push_back({3, static_cast<int>(rng() % 2), 10, false});
}

bool Game::buy(int index) {
    if (phase != Phase::Shop || index < 0 || index >= static_cast<int>(offers.size())) {
        return false;
    }
    auto& o = offers[index];
    if (o.sold) {
        return false;
    }
    if (cash < o.cost) {
        message = "Not enough cash.";
        return false;
    }
    if (o.type == 0 && owned.size() >= 5) {
        message = "Relic slots are full. Sell one to make room.";
        return false;
    }
    if (o.type == 3 && (o.id == 0 ? handBonus : discardBonus) >= 2) {
        message = "That upgrade is already at its limit.";
        return false;
    }
    std::vector<int> eligible;
    if (o.type == 2) {
        for (int i = 0; i < 52; ++i) {
            if (o.id == 0 ? !deck[i].foil : o.id == 1 ? !deck[i].wild : deck[i].rank < 14) {
                eligible.push_back(i);
            }
        }
        if (eligible.empty()) {
            message = "No eligible cards remain.";
            return false;
        }
    }
    cash -= o.cost;
    o.sold = true;
    message = "Bought " + offerName(o) + ".";
    if (o.type == 0) {
        owned.push_back(o.id);
    }
    if (o.type == 1) {
        ++levels[o.id];
    }
    if (o.type == 3) {
        ++(o.id == 0 ? handBonus : discardBonus);
    }
    if (o.type == 2) {
        std::shuffle(eligible.begin(), eligible.end(), rng);
        for (int i = 0; i < std::min(3, static_cast<int>(eligible.size())); ++i) {
            Card& c = deck[eligible[i]];
            if (o.id == 0) {
                c.foil = true;
            } else if (o.id == 1) {
                c.wild = true;
            } else {
                ++c.rank;
            }
        }
        message = "Three deck cards upgraded permanently.";
    }
    return true;
}

bool Game::sell(int index) {
    if (phase != Phase::Shop || index < 0 || index >= static_cast<int>(owned.size())) {
        return false;
    }
    cash += std::max(1, relics[owned[index]].cost / 2);
    message = "Sold " + std::string(relics[owned[index]].name) + ".";
    owned.erase(owned.begin() + index);
    return true;
}

bool Game::reroll() {
    if (phase != Phase::Shop || cash < 3 + rerolls) {
        return false;
    }
    cash -= 3 + rerolls;
    ++rerolls;
    makeShop();
    message = "A fresh selection.";
    return true;
}

std::string Game::offerName(const Offer& o) const {
    if (o.type == 0) {
        return relics[o.id].name;
    }
    if (o.type == 1) {
        return std::string(handNames[o.id]) + " +1";
    }
    if (o.type == 2) {
        return std::array<const char*, 3>{"Foil Press", "Wild Ink", "Rank Forge"}[o.id];
    }
    return o.id == 0 ? "Extra Hand" : "Extra Discard";
}

std::string Game::offerDescription(const Offer& o) const {
    if (o.type == 0) {
        return relics[o.id].description;
    }
    if (o.type == 1) {
        return "Level up this hand: +" + std::to_string(10 + 5 * o.id) + " base chips and +1 mult.";
    }
    if (o.type == 2) {
        return std::array<const char*, 3>{"Three random cards gain +35 chips. Permanent.",
                                          "Three random cards become suit-wild. Permanent.",
                                          "Three random cards gain one rank. Permanent."}[o.id];
    }
    return o.id == 0 ? "One extra hand every blind. Maximum +2."
                     : "One extra discard every blind. Maximum +2.";
}

} // namespace neon
