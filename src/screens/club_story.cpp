#include "app.hpp"
#include <algorithm>
namespace neon {
using namespace theme;
void App::drawClubStory(){
    clubPage=std::clamp(clubPage,0,std::min(7,game.ante-1));
    const auto& chapter=clubChapters[clubPage];
    const bool current=clubPage==game.ante-1,chosen=game.clubChoices[clubPage]!=0;
    ui.drawBox({140,75,1000,650},panel,line);
    ui.drawText("THE MIDNIGHT CLUB / CHAPTER "+std::to_string(clubPage+1)+" OF 8",168,100,17,gold,true);
    ui.drawDisplayText(chapter.title,168,137,31,ink);
    ui.drawSuit(clubPage%4,{1080,147},21,gold);
    ui.drawText(chapter.character,168,191,22,mint,true);
    ui.drawWrappedText(chapter.story,{168,232,930,174},21,ink);
    for(int i=0;i<2;++i){
        Rectangle box={168+i*474.f,433,450,159};const auto& option=chapter.choices[i];
        const bool selected=game.clubChoices[clubPage]==i+1;
        ui.drawBox(box,selected?Color{36,61,63,255}:bg,selected?gold:line);
        std::string reward=option.reward;
        if(option.kind==4&&game.discardBonus==2)reward="Extra discards already maxed. Receive $3 instead.";
        if((option.kind==2||option.kind==3)&&std::all_of(game.deck.begin(),game.deck.end(),[&](Card c){return option.kind==2?c.foil:c.wild;}))reward="Every card already has this upgrade. Receive $2 instead.";
        ui.drawWrappedText(reward,{box.x+18,box.y+19,414,54},18,muted);
        if(ui.button({box.x+18,box.y+91,414,46},selected?"Your choice: "+std::string(option.title):option.title,mint,selected,current&&!chosen&&game.phase==Phase::Shop))finishAction(game.chooseClubStory(i),3);
    }
    ui.drawText(chosen?"A memory added to this run. Rewards are collected once.":current?"Choose a reply, or return to the market and decide later.":"You can read earlier chapters; their choices close when you move on.",168,611,16,muted);
    if(ui.button({168,652,120,40},"Previous",blue,false,clubPage>0))--clubPage;
    if(ui.button({303,652,120,40},"Next",blue,false,clubPage<std::min(7,game.ante-1)))++clubPage;
    if(ui.button({860,647,254,46},"Return to market",mint,true))modal=Modal::None;
}
} // namespace neon
