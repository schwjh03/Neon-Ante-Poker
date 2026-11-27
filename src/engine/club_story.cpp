#include "game.hpp"
#include <algorithm>
#include <numeric>

namespace neon {
// Reward kinds: 0 cash, 1 hand level, 2 foil cards, 3 wild cards, 4 extra discard.
const std::array<ClubChapter,8> clubChapters={{
 {"Vale","A table with one light on","Vale has been dealing to empty chairs. The Midnight Club closed after its regulars drifted away, but the last lamp still works. You ask why the table is set. He turns over a blank invitation. 'Because someone might come back.'",{{{"Invite the old regulars","The invitation fund returns $2.",0,2},{"Learn Vale's opening hand","Pair gains one constellation level.",1,1}}}},
 {"Mara","The coat no one collected","Mara kept a musician's coat behind the counter for years. In its pocket is a set list, folded around a card with the club's old address. 'I could send it back,' she says, 'or ask him to play it here one more time.'",{{{"Send the coat and a letter","A grateful regular leaves $3.",0,3},{"Set a place for the musician","Flush gains one constellation level.",1,5}}}},
 {"Rook","The sign above the door","Rook has brought a box of salvaged bulbs. Half the neon sign is missing, and the working half spells a word nobody remembers. He asks if a restored club should look exactly as it did before. Outside, someone pauses beneath the dark glass.",{{{"Keep the old lettering","Rook foils two unenhanced cards.",2,2},{"Leave room for a new name","Rook inks one card suit-wild.",3,1}}}},
 {"Iris","Something warm after midnight","Iris used to cook for the last table of the night. She arrives with a pot and no menu. 'I remembered what everyone liked,' she says. 'Then I worried they'd changed.' The pot smells good enough to make the whole room quieter.",{{{"Make enough for late arrivals","Gain one permanent extra discard, up to +2.",4,1},{"Let the regulars write the menu","Full House gains one constellation level.",1,6}}}},
 {"Quill","An album with an empty page","Quill lays an old album beside the chips. Some photographs have names on the back; others only have a date. There is one empty page at the end. 'I kept saving it for a proper reopening,' she admits. From the kitchen, Iris asks what counts as proper.",{{{"Ask everyone for a memory","Two Pair gains one constellation level.",1,2},{"Take a photograph of tonight","One card becomes suit-wild.",3,1}}}},
 {"Vale","The house rules","The lamp over the table flickers. Vale finally tells you the club was never won in a single game. Each dealer left a rule for the next: keep the cards fair, keep a seat open, and know when to let the night end. He slides you a clean sheet of paper.",{{{"Keep a seat for a beginner","High Card gains one constellation level.",1,0},{"Make room for an unfamiliar game","Straight gains one constellation level.",1,4}}}},
 {"Mara","A song in the doorway","The musician has come back. He asks for a quiet corner rather than a stage. Mara looks at the coat hook, then at a new visitor who has stopped outside. 'There were nights when I needed an invitation too,' she says.",{{{"Welcome the visitor to the table","Pair gains one constellation level.",1,1},{"Ask the musician for a new song","Four of a Kind gains one constellation level.",1,7}}}},
 {"The regulars","The last lamp","The sign is lit. Iris is packing leftovers, Rook is pretending not to watch the last blind, and Quill has finally used the empty page. Vale leaves the dealer's chair beside you. Nobody asks what you intend to win. Mara asks what you intend to keep.",{{{"Keep the door open after the game","The regulars contribute $5.",0,5},{"Keep a page for the next arrival","Full House gains one constellation level.",1,6}}}}
}};

bool Game::clubStoryPending()const{return phase==Phase::Shop&&ante>=1&&ante<=8&&clubChoices[ante-1]==0;}
bool Game::chooseClubStory(int choice){
    if(!clubStoryPending()||choice<0||choice>1)return false;
    const auto& option=clubChapters[ante-1].choices[choice];
    clubChoices[ante-1]=choice+1;
    if(option.kind==0)cash=std::min(100000000,cash+option.amount);
    else if(option.kind==1)levels[option.amount]=std::min(100000,levels[option.amount]+1);
    else if(option.kind==4){if(discardBonus<2)++discardBonus;else cash=std::min(100000000,cash+3);}
    else{
        std::vector<int> eligible;
        for(int i=0;i<52;++i)if(option.kind==2?!deck[i].foil:!deck[i].wild)eligible.push_back(i);
        std::shuffle(eligible.begin(),eligible.end(),rng);
        if(eligible.empty())cash=std::min(100000000,cash+2);
        for(int i=0;i<std::min(option.amount,static_cast<int>(eligible.size()));++i){if(option.kind==2)deck[eligible[i]].foil=true;else deck[eligible[i]].wild=true;}
    }
    message=std::string(clubChapters[ante-1].character)+" will remember that choice.";
    return true;
}
std::string Game::clubEnding()const{
    const int chapters=static_cast<int>(std::count_if(clubChoices.begin(),clubChoices.end(),[](int n){return n>0;}));
    if(chapters==0)return "Beyond the last blind, the regulars are still waiting for their stories to be heard.";
    const int invitations=static_cast<int>(std::count(clubChoices.begin(),clubChoices.end(),1));
    return invitations*2>=chapters?"The club stays open after the last hand. There is a seat for the next person who needs one.":"Quill leaves a new page in the album. The club has room to become something none of you has imagined yet.";
}
} // namespace neon
