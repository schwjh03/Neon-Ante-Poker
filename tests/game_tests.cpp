#include "../src/game.hpp"
#include "../src/build_profiles.hpp"
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

        g.newRun(123,RunMode::Classic,1);
        check(g.cash==4 && g.discards==4 && g.starter==1, "Planner trades starting cash for an extra discard");
        g.newRun(dailySeedFor("2026-11-23"),RunMode::Daily,0);
        Game dailyCopy(1);dailyCopy.newRun(dailySeedFor("2026-11-23"),RunMode::Daily,0);
        check(g.hand==dailyCopy.hand && g.draw==dailyCopy.draw && dailySeedFor("2026-11-23")!=dailySeedFor("2026-11-24"),"daily seeds reproduce deals and change on the next date");
        g.newRun(4,RunMode::Classic,2);g.owned.clear();g.lastHandType=0;g.rhythm=2;
        check(g.evaluate(cards({10,10})).total==87,"Performer preview applies the next varied-hand rhythm bonus");
        g.lastHandType=1;check(g.evaluate(cards({10,10})).total==64,"repeated hand type breaks Performer rhythm");
        g.newRun(5);check(g.chooseContract(1)&&!g.chooseContract(4),"challenge choice is committed once");
        deal(g,cards({10,11,12,13,14},true));check(g.play()&&g.contractReward==4&&g.reward==12,"first-hand challenge pays once within the blind payout");
        g.newRun(7);g.toggle(g.hand[0]);g.discard();check(!g.chooseContract(1),"cannot choose a challenge after discarding");
        g.newRun(5,RunMode::Draft);deal(g,cards({10,11,12,13,14},true));g.play();
        check(!g.draftTaken&&g.draftChoices.size()==3&&!g.next(),"draft mode requires a choice or explicit pass before advancing");
        const auto choices=g.draftChoices;const int draftCash=g.cash;g.reroll();
        check(g.draftChoices==choices&&!g.draftTaken,"shop rerolls do not refresh the free draft");
        g.cash=draftCash;check(g.takeDraft(0)&&g.draftTaken&&g.cash==draftCash&&!g.takeDraft(1),"free draft gives exactly one relic and does not spend cash");
        g.newRun(5,RunMode::Draft);deal(g,cards({10,11,12,13,14},true));g.play();g.owned={0,1,2,3,4};
        check(!g.takeDraft(0)&&!g.draftTaken&&g.passDraft()&&g.next(),"full relic slots preserve draft until the player passes or makes room");
        g.newRun(5,RunMode::Echo,2);g.echoScale=1.37;g.echoHand=1;g.contract=3;g.contractChosen=true;g.handUse[1]=7;
        check(g.save(path),"new modes save");Game restored(1);
        check(restored.load(path)&&restored.mode==RunMode::Echo&&restored.starter==2&&restored.echoScale==1.37&&restored.echoHand==1&&restored.handUse[1]==7&&restored.contract==3,"mode, echo, challenge, and history survive save reload");
        restored.owned.clear();restored.starter=0;check(restored.evaluate(cards({10,10})).total==54,"Echo counteracts the profile's favored hand in score previews");
        {std::ifstream input(path);std::string saved((std::istreambuf_iterator<char>(input)),{});saved.replace(0,16,"NEON_ANTE_CPP 1\n");const auto at=saved.find("MODES ");saved.resize(at);std::ofstream oldSave(path);oldSave<<saved;}
        Game legacy(1);check(legacy.load(path)&&legacy.mode==RunMode::Classic&&legacy.runSeed==0,"version-one runs migrate to Classic without losing the original cards");
        g.newRun(55);g.handUse[5]=8;g.deck[0].foil=true;auto profile=captureBuild(g);
        const auto buildPath=std::filesystem::path(path+".neonbuild");
        check(exportBuild(profile,buildPath),"export shareable build");BuildProfile imported;
        check(importBuild(buildPath,imported)&&imported.deck[0].foil&&imported.favoredHand()==5&&imported.owned==g.owned,"shared builds preserve actual deck, relics, and hand usage");
        {std::ofstream bad(buildPath);bad<<"NEON_ANTE_BUILD 1\n123 0 -90\n";}
        const auto oldSeed=imported.seed;check(!importBuild(buildPath,imported)&&imported.seed==oldSeed,"invalid build import leaves previous profile untouched");
        BuildLibrary library;check(library.add(profile)&&!library.add(profile),"library deduplicates identical shared builds");
        auto strongest=profile;strongest.seed=999;strongest.ante=20;library.add(strongest);
        for(int i=0;i<12;++i){auto weaker=profile;weaker.seed=100+i;library.add(weaker);}
        check(library.profiles.size()==8&&library.strongest()->seed==999&&library.strongest()->strength(8)<=1.8,"bounded archive retains strong builds and caps adaptive difficulty");
        check(library.save(buildPath),"save build archive");BuildLibrary archiveCopy;check(archiveCopy.load(buildPath)&&archiveCopy.profiles.size()==8,"build archive round trip");
        std::filesystem::remove(path);std::filesystem::remove(buildPath);

        g.newRun(6);g.owned={0,12};const auto beforeReorder=g.evaluate(cards({13})).total;
        check(beforeReorder==90&&g.moveRelic(0,1)&&g.evaluate(cards({13})).total==68,"moving an additive relic after a multiplier changes actual score");
        check(!g.moveRelic(0,-1)&&!g.moveRelic(1,1)&&!g.moveRelic(0,0),"relic movement rejects invalid boundaries and direction");
        check(g.save(path)&&loaded.load(path)&&loaded.owned==g.owned,"relic slot order survives save reload");std::filesystem::remove(path);
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
