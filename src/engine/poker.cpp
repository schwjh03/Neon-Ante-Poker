#include "game.hpp"
#include <algorithm>
#include <set>

extern "C" int evaluate_5cards(int, int, int, int, int);

namespace neon {
Hand classify(const std::vector<Card>& cards) {
    if (cards.empty() || cards.size() > 5) {
        return Hand::High;
    }
    std::array<int, 15> counts{};
    for (auto c : cards) {
        ++counts[c.rank];
    }
    int pairs = 0, most = 0;
    for (int n : counts) {
        most = std::max(most, n);
        pairs += n == 2;
    }
    if (most == 5) {
        return Hand::Five;
    }
    if (cards.size() < 5) {
        if (most == 4) {
            return Hand::Four;
        }
        if (most == 3) {
            return Hand::Three;
        }
        if (pairs == 2) {
            return Hand::TwoPair;
        }
        return pairs ? Hand::Pair : Hand::High;
    }
    // PH Evaluator handles ordinary poker. Suit-wild cards are resolved first;
    // forged duplicate cards receive equivalent distinct suits for rank lookup.
    std::array<int, 4> suits{};
    for (auto c : cards) {
        if (!c.wild) {
            ++suits[c.suit];
        }
    }
    int suit = static_cast<int>(std::max_element(suits.begin(), suits.end()) - suits.begin());
    std::array<int, 5> ids{};
    std::set<int> used;
    for (size_t i = 0; i < cards.size(); ++i) {
        int s = cards[i].wild ? suit : cards[i].suit;
        int base = (cards[i].rank - 2) * 4;
        while (used.count(base + s)) {
            s = (s + 1) % 4;
        }
        ids[i] = base + s;
        used.insert(ids[i]);
    }
    int v = evaluate_5cards(ids[0], ids[1], ids[2], ids[3], ids[4]);
    if (v <= 10) {
        return Hand::StraightFlush;
    }
    if (v <= 166) {
        return Hand::Four;
    }
    if (v <= 322) {
        return Hand::FullHouse;
    }
    if (v <= 1599) {
        return Hand::Flush;
    }
    if (v <= 1609) {
        return Hand::Straight;
    }
    if (v <= 2467) {
        return Hand::Three;
    }
    if (v <= 3325) {
        return Hand::TwoPair;
    }
    if (v <= 6185) {
        return Hand::Pair;
    }
    return Hand::High;
}
} // namespace neon
