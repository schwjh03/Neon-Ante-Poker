#include "game.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <set>
#include <iomanip>

namespace neon {
template <class T> static void writeVector(std::ostream& out, const std::vector<T>& v) {
    out << v.size();
    for (auto x : v) {
        out << ' ' << x;
    }
    out << '\n';
}

template <class T> static bool readVector(std::istream& in, std::vector<T>& v, size_t max) {
    size_t size = 0;
    if (!(in >> size) || size > max) {
        return false;
    }
    v.resize(size);
    for (auto& x : v) {
        if (!(in >> x)) {
            return false;
        }
    }
    return true;
}

bool Game::save(const std::string& path) const {
    std::ofstream out(path + ".tmp");
    if (!out) {
        return false;
    }
    out << "NEON_ANTE_CPP 2\n"
        << static_cast<int>(phase) << ' ' << ante << ' ' << blind << ' ' << cash << ' ' << hands
        << ' ' << discards << ' ' << handBonus << ' ' << discardBonus << ' ' << round << ' '
        << rerolls << ' ' << reward << ' ' << interest << ' ' << bankReward << ' ' << score << ' '
        << best << ' ' << lifetime << ' ' << taxed << ' ' << endless << '\n';
    for (int n : levels) {
        out << n << ' ';
    }
    out << '\n';
    for (auto c : deck) {
        out << c.rank << ' ' << c.suit << ' ' << c.foil << ' ' << c.wild << '\n';
    }
    writeVector(out, draw);
    writeVector(out, hand);
    writeVector(out, selected);
    writeVector(out, owned);
    out << offers.size() << '\n';
    for (auto o : offers) {
        out << o.type << ' ' << o.id << ' ' << o.cost << ' ' << o.sold << '\n';
    }
    out << rng << '\n';
    out << std::setprecision(17);
    out << "MODES " << static_cast<int>(mode) << ' ' << runSeed << ' ' << starter << ' ' << lastHandType << ' ' << rhythm << ' ' << playedMask << ' ' << discardsSpent << ' ' << contract << ' ' << contractChosen << ' ' << contractReward << ' ' << draftTaken << ' ' << archived << ' ' << echoScale << ' ' << echoHand << '\n';
    for(int n:handUse){out<<n<<' ';}
    out<<'\n';writeVector(out,draftChoices);
    out<<"STORY 1";for(int choice:clubChoices)out<<' '<<choice;out<<'\n';
    out.close();
    if (!out) {
        return false;
    }
    std::error_code ec;
    // Keep the last complete save until the temporary file has been flushed.
    std::filesystem::copy_file(path + ".tmp", path,
                               std::filesystem::copy_options::overwrite_existing, ec);
    if (ec) {
        return false;
    }
    std::filesystem::remove(path + ".tmp", ec);
    return true;
}

bool Game::load(const std::string& path) {
    std::ifstream in(path);
    std::string magic;
    int version = 0, p = 0;
    Game g(1);
    if (!(in >> magic >> version) || magic != "NEON_ANTE_CPP" || (version != 1 && version != 2)) {
        return false;
    }
    if (!(in >> p >> g.ante >> g.blind >> g.cash >> g.hands >> g.discards >> g.handBonus >>
          g.discardBonus >> g.round >> g.rerolls >> g.reward >> g.interest >> g.bankReward >>
          g.score >> g.best >> g.lifetime >> g.taxed >> g.endless) ||
        p < 0 || p > 3) {
        return false;
    }
    g.phase = static_cast<Phase>(p);
    for (int& n : g.levels) {
        if (!(in >> n)) {
            return false;
        }
    }
    for (auto& c : g.deck) {
        if (!(in >> c.rank >> c.suit >> c.foil >> c.wild)) {
            return false;
        }
    }
    if (!readVector(in, g.draw, 52) || !readVector(in, g.hand, 8) ||
        !readVector(in, g.selected, 5) || !readVector(in, g.owned, 5)) {
        return false;
    }
    size_t n = 0;
    if (!(in >> n) || n > 5) {
        return false;
    }
    g.offers.resize(n);
    for (auto& o : g.offers) {
        if (!(in >> o.type >> o.id >> o.cost >> o.sold)) {
            return false;
        }
    }
    if (!(in >> g.rng)) return false;
    if(version==1)g.runSeed=0;
    if(version==2){
        std::string marker;int modeValue=0;
        if(!(in>>marker>>modeValue>>g.runSeed>>g.starter>>g.lastHandType>>g.rhythm>>g.playedMask>>g.discardsSpent>>g.contract>>g.contractChosen>>g.contractReward>>g.draftTaken>>g.archived>>g.echoScale>>g.echoHand)||marker!="MODES"||modeValue<0||modeValue>3)return false;
        g.mode=static_cast<RunMode>(modeValue);
        for(int& n:g.handUse)if(!(in>>n))return false;
        if(!readVector(in,g.draftChoices,3))return false;
        in>>std::ws;
        if(in.peek()!=std::char_traits<char>::eof()){
            std::string storyMarker;int storyVersion=0;
            if(!(in>>storyMarker>>storyVersion)||storyMarker!="STORY"||storyVersion!=1)return false;
            for(int& choice:g.clubChoices)if(!(in>>choice))return false;
        }
    }
    if (!g.valid()) {
        return false;
    }
    g.last = {};
    g.lastCards.clear();
    g.message = "Run resumed.";
    *this = std::move(g);
    return true;
}

bool Game::valid() const {
    for(size_t i=0;i<clubChoices.size();++i)if(clubChoices[i]<0||clubChoices[i]>2||(static_cast<int>(i)>=ante&&clubChoices[i]!=0))return false;
    if(static_cast<int>(mode)<0||static_cast<int>(mode)>3||starter<0||starter>2||lastHandType< -1||lastHandType>9||rhythm<0||rhythm>3||playedMask<0||playedMask>1023||discardsSpent<0||discardsSpent>5||contract<0||contract>5||contractReward<0||contractReward>5||!(echoScale>=1&&echoScale<=1.8)||echoHand< -1||echoHand>9)return false;
    for(int n:handUse)if(n<0||n>1000000)return false;
    std::set<int> draftIds;for(int id:draftChoices)if(id<0||id>=16||!draftIds.insert(id).second)return false;
    if(mode!=RunMode::Draft && !draftTaken)return false;
    if (ante < 1 || ante > 10000 || blind < 0 || blind > 2 || cash < 0 || cash > 100000000 ||
        handBonus < 0 || handBonus > 2 || discardBonus < 0 || discardBonus > 2 || hands < 0 ||
        hands > 6 || discards < 0 || discards > 5 || round != ((ante - 1) * 3 + blind + 1) ||
        rerolls < 0 || rerolls > 10000 || score < 0 || best < 0 || lifetime < 0) {
        return false;
    }
    if (phase == Phase::Play && (hands == 0 || hand.empty() || score >= target())) {
        return false;
    }
    if ((phase == Phase::Shop || phase == Phase::Won) && score < target()) {
        return false;
    }
    if (phase == Phase::Won && (ante != 8 || blind != 2 || endless)) {
        return false;
    }
    for (auto c : deck) {
        if (c.rank < 2 || c.rank > 14 || c.suit < 0 || c.suit > 3) {
            return false;
        }
    }
    for (int n : levels) {
        if (n < 1 || n > 100000) {
            return false;
        }
    }
    std::set<int> seen;
    for (const auto* list : {&draw, &hand}) {
        for (int id : *list) {
            if (id < 0 || id >= 52 || !seen.insert(id).second) {
                return false;
            }
        }
    }
    seen.clear();
    for (int id : selected) {
        if (std::find(hand.begin(), hand.end(), id) == hand.end() || !seen.insert(id).second) {
            return false;
        }
    }
    seen.clear();
    for (int id : owned) {
        if (id < 0 || id >= 16 || !seen.insert(id).second) {
            return false;
        }
    }
    for (auto o : offers) {
        if (o.type < 0 || o.type > 3 || o.id < 0 ||
            o.id >= (o.type == 0   ? 16
                     : o.type == 1 ? 10
                     : o.type == 2 ? 3
                                   : 2) ||
            o.cost < 0 || o.cost > 100) {
            return false;
        }
    }
    return true;
}

} // namespace neon
