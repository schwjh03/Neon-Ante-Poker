#include "game.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace neon {
Score Game::evaluate(const std::vector<Card>& cards) const {
    Score r;
    if (cards.empty()) {
        return r;
    }
    r.hand = classify(cards);
    int h = static_cast<int>(r.hand);
    r.chips = baseChips[h] + (levels[h] - 1) * (10 + 5 * h);
    r.mult = baseMult[h] + levels[h] - 1;
    std::vector<Card> active;
    for (auto c : cards) {
        if (boss() == 2 && c.suit == 2) {
            continue;
        }
        active.push_back(c);
        r.chips += c.rank == 14 ? 11 : std::min(c.rank, 10);
        if (c.foil) {
            r.chips += 35;
        }
    }
    int money = std::max(0, cash - ((boss() == 0 && !taxed) ? 2 : 0));
    for (int id : owned) {
        int oldC = r.chips, oldM = r.mult;
        double oldF = r.factor;
        switch (id) {
        case 0:
            for (auto c : active) {
                if (c.rank >= 11 && c.rank <= 13) {
                    r.mult += 3;
                }
            }
            break;
        case 1:
            if (r.hand == Hand::Pair || r.hand == Hand::TwoPair) {
                r.chips += 45;
            }
            break;
        case 2:
            if (r.hand == Hand::Flush || r.hand == Hand::StraightFlush) {
                r.factor *= 1.8;
            }
            break;
        case 3:
            for (auto c : active) {
                if (c.rank == 7) {
                    r.mult += 11;
                }
            }
            break;
        case 4:
            for (auto c : active) {
                if (c.rank == 14) {
                    r.chips += 26;
                    r.mult += 2;
                }
            }
            break;
        case 5:
            if (r.hand == Hand::Straight || r.hand == Hand::StraightFlush) {
                r.chips += 70;
                r.mult += 3;
            }
            break;
        case 6:
            if (cards.size() == 5) {
                r.mult += 5;
            }
            break;
        case 7:
            r.mult += money / 2;
            break;
        case 8:
            for (auto c : active) {
                if (c.suit == 3) {
                    r.chips += 9;
                }
                if (c.suit == 0) {
                    r.mult += 2;
                }
            }
            break;
        case 9: {
            bool ace = false, king = false;
            for (auto c : active) {
                ace |= c.rank == 14;
                king |= c.rank == 13;
            }
            if (ace && king) {
                r.factor *= 2.1;
            }
            break;
        }
        case 10:
            if (hands == 1) {
                r.factor *= 3;
            }
            break;
        case 11:
            for (auto c : active) {
                if (c.suit == 1 || c.suit == 2) {
                    r.mult += 4;
                }
            }
            break;
        case 12:
            r.factor *= 1.5;
            break;
        case 13:
            if (cards.size() <= 3) {
                r.chips += 80;
            }
            break;
        case 15:
            r.mult += round;
            break;
        }
        if (oldC != r.chips || oldM != r.mult || oldF != r.factor) {
            std::ostringstream effect;
            effect << relics[id].name << "  ";
            if (r.chips != oldC) {
                effect << "+" << r.chips - oldC << " chips ";
            }
            if (r.mult != oldM) {
                effect << "+" << r.mult - oldM << " mult ";
            }
            if (r.factor != oldF) {
                effect << "x" << std::setprecision(3) << r.factor / oldF;
            }
            r.effects.push_back(effect.str());
        }
    }
    r.total = std::max<int64_t>(1, static_cast<int64_t>(std::round(std::min(
                                       9e15, r.chips * static_cast<double>(r.mult) * r.factor))));
    return r;
}

Score Game::preview() const {
    return evaluate(selectedCards());
}

} // namespace neon
