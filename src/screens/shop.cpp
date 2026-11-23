#include "app.hpp"

namespace neon {
using namespace theme;
void App::drawShop() {
    ui.drawDisplayText("The Night Market",292,264,32,ink);
    ui.drawText("+$"+std::to_string(game.reward),1137,266,32,gold,true);
    ui.drawText("Blind $"+std::to_string(game.bankReward)+"  +  Hands $"+std::to_string(game.hands)+"  +  Interest $"+std::to_string(game.interest)+(game.contractReward?"  +  Challenge $"+std::to_string(game.contractReward):""),292,310,16,muted);
    ui.drawText("RELICS",292,344,13,gold,true);
    ui.drawText("CONSTELLATION",718,344,13,blue,true);
    ui.drawText("DECK WORKSHOP",718,481,13,mint,true);
    int relicColumn=0;
    for(size_t i=0;i<game.offers.size();++i){
        const Offer& o=game.offers[i];Color tint=o.type==0?accent[relics[o.id].color]:o.type==1?blue:o.type==2?mint:gold;
        bool allowed=!o.sold&&game.cash>=o.cost;
        if(o.type==0)allowed=allowed&&game.owned.size()<5;
        if(o.type==3)allowed=allowed&&(o.id==0?game.handBonus:game.discardBonus)<2;
        Rectangle buy{};
        if(o.type==0){
            Rectangle r={292+relicColumn++*210.f,367,194,329};ui.drawBox(r,panel,Fade(tint,.6f));
            ui.drawRelicEmblem(o.id,{r.x+97,r.y+65},36,o.sold?muted:tint);
            ui.drawFittedText(game.offerName(o),{r.x+14,r.y+123,166,28},23,ink,true);
            ui.drawWrappedText(game.offerDescription(o),{r.x+14,r.y+170,166,70},17,muted);
            buy={r.x+14,r.y+257,166,36};
        }else if(o.type==1){
            ui.drawBox({718,367,534,97},panel,Fade(tint,.45f));ui.drawStar({754,410},20,tint);
            ui.drawFittedText(game.offerName(o),{792,380,290,26},22,ink,true);
            ui.drawWrappedText(game.offerDescription(o),{792,414,280,42},15,muted);
            buy={1100,398,136,38};
        }else if(o.type==2){
            ui.drawBox({718,504,534,129},panel,Fade(tint,.45f));
            ui.drawCard({14,3,o.id==0,o.id==1},{737,524,46,69});
            ui.drawFittedText(game.offerName(o),{807,520,275,27},23,ink,true);
            ui.drawWrappedText(game.offerDescription(o),{807,556,270,61},16,muted);
            buy={1100,556,136,38};
        }else{
            ui.drawBox({718,648,534,48},panel,Fade(tint,.4f));ui.drawText(game.offerName(o),736,660,18,ink,true);
            ui.showTooltip(game.offerDescription(o),{718,648,350,48});buy={1100,655,136,33};
        }
        if(ui.button(buy,o.sold?"Sold":"Buy  $"+std::to_string(o.cost),tint,false,allowed))finishAction(game.buy(static_cast<int>(i)),3);
    }
    if(ui.button({292,721,212,51},"Reroll  $"+std::to_string(3+game.rerolls),gold,false,game.cash>=3+game.rerolls))finishAction(game.reroll(),1);
    if(game.mode==RunMode::Draft&&!game.draftTaken&&ui.button({524,721,230,51},"Free relic draft",blue,true))modal=Modal::Draft;
    if(ui.button({982,721,270,51},"Next blind",mint,true,game.draftTaken)){finishAction(game.next(),3);cardLift.fill(0);}
}
} // namespace neon
