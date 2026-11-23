#include "../src/game.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
using namespace neon;
int checks = 0;

void check(bool value, const std::string& what) {
    ++checks;
    if (!value) {
        throw std::runtime_error(what);
    }
}

std::vector<Card> cards(std::initializer_list<int> ranks, bool flush = false) {
    std::vector<Card> v;
    int s = 0;
    for (int r : ranks) {
        v.push_back({r, flush ? 0 : (s++ % 4), false, false});
    }
    return v;
}

void deal(Game& g, const std::vector<Card>& v) {
    g.hand.clear();
    g.selected.clear();
    g.draw.clear();
    for (size_t i = 0; i < v.size(); ++i) {
        g.deck[i] = v[i];
        g.hand.push_back(static_cast<int>(i));
        g.selected.push_back(static_cast<int>(i));
    }
    for (int i = 51; i >= static_cast<int>(v.size()); --i) {
        g.draw.push_back(i);
    }
}

int main() {
    try {
        check(classify(cards({14, 2, 3, 4, 5})) == Hand::Straight, "ace-low straight");
        check(classify(cards({14, 2, 3, 4, 5}, true)) == Hand::StraightFlush,
              "ace-low straight flush");
        check(classify(cards({10, 11, 12, 13, 14}, true)) == Hand::StraightFlush, "royal flush");
        check(classify(cards({2, 5, 8, 10, 14}, true)) == Hand::Flush, "flush");
        check(classify(cards({14, 14, 14, 14, 14})) == Hand::Five, "forged five of a kind");
        check(classify(cards({10, 10, 10, 4, 4})) == Hand::FullHouse, "full house");
        check(classify(cards({7, 7, 7, 7})) == Hand::Four, "four cards");
        check(classify(cards({8, 8, 4, 4})) == Hand::TwoPair, "two pair with four cards");
        check(classify(cards({8, 8, 8})) == Hand::Three, "trips with three cards");
        check(classify(cards({8, 8})) == Hand::Pair, "two-card pair");
        auto w = cards({2, 5, 8, 10, 14}, true);
        w[0].suit = 3;
        w[0].wild = true;
        check(classify(w) == Hand::Flush, "suit-wild flush");
        auto duplicate = cards({2, 2, 4, 8, 10}, true);
        check(classify(duplicate) == Hand::Pair, "forged exact duplicate cannot corrupt evaluator");
        Game g(123);
        g.owned.clear();
        check(g.evaluate(cards({10, 10})).total == 64, "pair chip formula");
        g.owned = {0};
        check(g.evaluate(cards({14})).mult == 1, "aces are not face cards");
        check(g.evaluate(cards({13})).mult == 4, "face card relic");
        g.owned = {0, 11};
        g.blind = 2;
        g.ante = 3;
        check(g.evaluate({{13, 2, true, false}}).total == 5,
              "debuff suppresses foil and card relic bonuses");
        g.newRun(45);
        g.blind = 2;
        g.ante = 1;
        g.owned = {7};
        g.cash = 12;
        deal(g, cards({13, 13}));
        auto preview = g.preview();
        check(g.play(), "tax play succeeds");
        check(g.last.total == preview.total && g.cash == 10, "tax preview matches actual");
        g.newRun(45);
        g.blind = 2;
        g.ante = 2;
        deal(g, cards({13, 13}));
        check(!g.play() && g.hands == 4, "five-card boss rejects partial hand");
        g.ante = 4;
        g.startBlind();
        g.toggle(g.hand[0]);
        check(g.discards == 0 && !g.discard(), "no-discard boss");
        g.newRun(77);
        auto first = g.hand;
        g.toggle(g.hand[0]);
        g.toggle(g.hand[1]);
        check(g.discard(), "discard succeeds");
        check(g.discards == 2 && g.hand.size() == 8 && g.draw.size() == 42,
              "discard consumes cards without recycling");
        g.newRun(3);
        g.owned.clear();
        g.hands = 1;
        deal(g, cards({2}));
        check(g.play() && g.phase == Phase::Lost, "loss");
        check(!g.next() && !g.play() && !g.buy(0) && !g.reroll(), "loss is terminal");
        g.newRun(5);
        deal(g, cards({10, 11, 12, 13, 14}, true));
        check(g.play() && g.phase == Phase::Shop, "win enters shop");
        auto cash = g.cash;
        check(cash == 6 + 4 + 3 + 1, "payout includes unspent hands and interest");
        g.offers = {{2, 0, 4, false}};
        check(g.buy(0), "foil purchase");
        int foil = 0;
        for (auto c : g.deck) {
            foil += c.foil;
        }
        check(foil == 3, "three upgrades");
        check(!g.buy(0), "sold item cannot be bought twice");
        check(g.next() && g.blind == 1, "advance");
        int retained = 0;
        for (auto c : g.deck) {
            retained += c.foil;
        }
        check(retained == 3, "upgrades persist through new blind");
        g.newRun(5);
        g.phase = Phase::Shop;
        g.owned = {0, 1, 2, 3, 4};
        g.cash = 25;
        g.offers = {{0, 5, 7, false}};
        check(!g.buy(0) && g.cash == 25, "slot limit preserves money");
        check(g.sell(0) && g.buy(0), "sell and replace relic");
        g.newRun(5);
        g.phase = Phase::Shop;
        g.cash = 0;
        g.offers = {{0, 5, 7, false}};
        check(!g.buy(0) && g.cash == 0, "insufficient cash");
        g.newRun(5);
        g.ante = 8;
        g.blind = 2;
        g.round = 24;
        g.score = g.target() - 1;
        deal(g, cards({10, 11, 12, 13, 14}, true));
        check(g.play() && g.phase == Phase::Won, "eighth ante victory");
        check(g.next() && g.endless && g.phase == Phase::Shop, "endless transition");
        check(g.next() && g.ante == 9 && g.blind == 0, "endless ante nine");
        auto path =
            (std::filesystem::temp_directory_path() / "neon-ante-engine-tests.save").string();
        g.newRun(54321);
        g.toggle(g.hand[0]);
        g.discard();
        g.toggle(g.hand[0]);
        g.deck[10].foil = true;
        check(g.save(path), "save");
        Game loaded(7);
        check(loaded.load(path), "load");
        check(g.hand == loaded.hand && g.draw == loaded.draw && g.selected == loaded.selected &&
                  loaded.deck[10].foil,
              "round trip");
        check(g.rng() == loaded.rng(), "rng persisted");
        {
            std::ofstream out(path);
            out << "NEON_ANTE_CPP 1\n3 999999999 99\n";
        }
        auto before = loaded.hand;
        check(!loaded.load(path) && loaded.hand == before, "invalid save leaves state untouched");
        std::filesystem::remove(path);
        // Exhaustive standard-deck frequencies independently verify category boundaries.
        std::array<int64_t, 10> counts{};
        std::vector<Card> v(5);
        for (int a = 0; a < 48; ++a) {
            for (int b = a + 1; b < 49; ++b) {
                for (int c = b + 1; c < 50; ++c) {
                    for (int d = c + 1; d < 51; ++d) {
                        for (int e = d + 1; e < 52; ++e) {
                            const int ids[] = {a, b, c, d, e};
                            for (int i = 0; i < 5; ++i) {
                                v[i] = {ids[i] / 4 + 2, ids[i] % 4, false, false};
                            }
                            ++counts[static_cast<int>(classify(v))];
                        }
                    }
                }
            }
        }
        const std::array<int64_t, 10> expected = {1302540, 1098240, 123552, 54912, 10200,
                                                  5108,    3744,    624,    40,    0};
        check(counts == expected, "all 2,598,960 standard five-card hands match known frequencies");
        std::cout << "PASS: " << checks
                  << " checks, including all 2,598,960 five-card combinations.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
}
