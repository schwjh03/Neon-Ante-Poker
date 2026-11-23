#include "game.hpp"
#include <algorithm>
#include <numeric>

namespace neon {
std::string Game::modeName() const {
    return std::array<const char*,4>{"Classic", "Daily seed", "Relic draft", "Echo table"}[static_cast<int>(mode)];
}
std::string Game::starterName() const {
    return std::array<const char*,3>{"Balanced", "Planner", "Performer"}[starter];
}
bool Game::canChooseContract() const {
    return phase==Phase::Play && !contractChosen && hands==4+handBonus && discardsSpent==0;
}
bool Game::chooseContract(int id) {
    if (!canChooseContract() || id<0 || id>5) return false;
    contract=id;contractChosen=true;message=id?"Table challenge: "+contractName()+".":"No table challenge this blind.";
    return true;
}
std::string Game::contractName() const {
    return std::array<const char*,6>{"None", "First impression", "Mix it up", "Pocket hand", "No swaps", "Over the top"}[contract];
}
int Game::completedContract(const Score&, size_t count) const {
    const int played=4+handBonus-hands;
    int kinds=0;for(int i=0;i<10;++i)kinds+=(playedMask>>i)&1;
    switch(contract){
    case 1:return played==1?4:0;
    case 2:return kinds>=2?4:0;
    case 3:return count<=3?3:0;
    case 4:return discardsSpent==0?3:0;
    case 5:return score>=target()*1.5?5:0;
    default:return 0;
    }
}
void Game::prepareDraft(){
    draftChoices.clear();draftTaken=mode!=RunMode::Draft;
    if(draftTaken)return;
    std::vector<int> pool;
    for(int id=0;id<static_cast<int>(relics.size());++id)
        if(std::find(owned.begin(),owned.end(),id)==owned.end())pool.push_back(id);
    std::shuffle(pool.begin(),pool.end(),rng);
    for(size_t i=0;i<std::min<size_t>(3,pool.size());++i)draftChoices.push_back(pool[i]);
    if(draftChoices.empty())draftTaken=true;
}
bool Game::takeDraft(int index){
    if(mode!=RunMode::Draft||phase!=Phase::Shop||draftTaken||index<0||index>=static_cast<int>(draftChoices.size()))return false;
    if(owned.size()>=5){message="Sell a relic first, or pass on this draft.";return false;}
    const int id=draftChoices[index];
    if(std::find(owned.begin(),owned.end(),id)!=owned.end()){message="You already own this relic. Choose another, or pass.";return false;}
    owned.push_back(id);draftTaken=true;message="Drafted "+std::string(relics[id].name)+" for free.";return true;
}
bool Game::passDraft(){
    if(mode!=RunMode::Draft||phase!=Phase::Shop||draftTaken)return false;
    draftTaken=true;message="Draft passed.";return true;
}
} // namespace neon
