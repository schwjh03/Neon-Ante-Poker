#include "app.hpp"
#include <algorithm>

namespace neon {
using namespace theme;
void App::startConfiguredRun(){
    const RunMode mode=static_cast<RunMode>(pendingMode);
    const uint32_t seed=mode==RunMode::Daily?dailySeedFor(utcDay()):previewSession?32491:std::random_device{}();
    game.newRun(seed,mode,pendingStarter);
    if(mode==RunMode::Echo){
        const auto* echo=library.strongest();game.echoScale=echo?echo->strength(library.profiles.size()):1.1;
        game.echoHand=echo?echo->favoredHand():-1;
        game.message="Echo table. Target +"+std::to_string(static_cast<int>((game.echoScale-1)*100+.5))+"%.";
    }
    cardLift.fill(0);particles.clear();screenTransition=0;modal=Modal::None;finishAction(true,3);
}
void App::drawRunSetup(){
    ui.drawBox({140,65,1000,675},panel,line);
    ui.drawDisplayText("Choose your next run",168,88,32,ink);
    if(ui.button({1080,86,36,34},"X",mint)){modal=Modal::Menu;return;}
    const char* titles[]={"Classic","Daily seed","Relic draft","Echo table"};
    const char* descriptions[]={"The original eight-ante circuit. Build a run at your pace.","A repeatable seed for each UTC day. Replay and compare builds.","Draft one free relic after each blind. Shape a synergy as you go.","Harder targets shaped by saved runs and builds you import."};
    for(int i=0;i<4;++i){
        Rectangle r={168+i*237.f,148,218,125};ui.drawBox(r,pendingMode==i?Color{39,59,66,255}:bg,pendingMode==i?gold:line);
        ui.drawDisplayText(titles[i],r.x+14,r.y+13,23,pendingMode==i?gold:ink);
        ui.drawWrappedText(descriptions[i],{r.x+14,r.y+51,190,65},15,muted);
        if(ui.wasClicked(r))pendingMode=i;
    }
    ui.drawText(pendingMode==1?"Daily seed: "+utcDay()+" UTC / "+std::to_string(dailySeedFor(utcDay())):"Modes change the run rules. Starting styles change how you build.",168,289,15,muted);
    ui.drawText("STARTING STYLE",168,330,13,gold,true);
    const char* starters[]={"Balanced","Planner","Performer"};
    const char* perks[]={"$6 and Chrome Smile. A flexible start.","$4, Chrome Smile, and one extra discard every blind.","$6 and Chrome Smile. Alternate hand types to build a score bonus, up to +36%."};
    for(int i=0;i<3;++i){
        Rectangle r={168+i*316.f,356,296,107};ui.drawBox(r,pendingStarter==i?Color{39,59,66,255}:bg,pendingStarter==i?gold:line);
        ui.drawText(starters[i],r.x+15,r.y+12,21,ink,true);ui.drawWrappedText(perks[i],{r.x+15,r.y+45,264,52},15,muted);
        if(ui.wasClicked(r))pendingStarter=i;
    }
    const auto* echo=library.strongest();
    ui.drawText("BUILD LIBRARY  /  "+std::to_string(library.profiles.size())+" of 8 snapshots",168,489,13,gold,true);
    ui.drawText(echo?echo->summary():"No completed runs yet. Echo starts at a +10% target.",168,516,18,ink);
    ui.drawWrappedText("Completed runs stay on this computer. Import a friend's .neonbuild file to include their deck, relics, and hand history. No automatic uploads or online leaderboard.",{168,545,900,42},15,muted);
    if(ui.button({168,600,190,38},"Export current build",blue)){
        const auto file=chooseBuildFile(true);if(!file.empty())setupMessage=exportBuild(captureBuild(game),file)?"Build exported. Share the file yourself.":"Export failed. Check the selected folder.";
    }
    if(ui.button({372,600,190,38},"Import shared build",blue)){
        const auto file=chooseBuildFile(false);if(!file.empty()){
            BuildProfile imported;
            if(!importBuild(file,imported))setupMessage="That file is not a valid Neon Ante build. The current run is unchanged.";
            else if(library.add(imported))setupMessage=library.save(libraryPath)?"Build imported. Your next Echo run can use it.":"Build imported for this session; the library could not be saved.";
            else setupMessage="That build is already in the library.";
        }
    }
    if(ui.button({838,652,276,47},"Start this run",gold,true))modal=Modal::ConfirmNewRun;
    if(ui.button({168,652,190,43},"Keep playing",mint))modal=Modal::None;
    ui.drawFittedText(setupMessage,{580,601,530,38},15,muted);
}
void App::drawContractDialog(){
    ui.drawBox({140,83,1000,630},panel,line);ui.drawDisplayText("A little extra on the table",168,106,31,ink);
    ui.drawText("Optional challenges pay a bonus on a clear. Choose before playing or discarding.",168,151,16,muted);
    const char* names[]={"First impression  /  +$4","Mix it up  /  +$4","Pocket hand  /  +$3","No swaps  /  +$3","Over the top  /  +$5"};
    const char* rules[]={"Clear the blind with your first played hand.","Play at least two different hand categories before clearing.","Clear with a final hand of three cards or fewer.","Clear without using a discard.","Finish with at least 150% of the target score."};
    for(int i=0;i<5;++i){
        Rectangle r={168+(i%2)*474.f,196+(i/2)*128.f,450,108};
        ui.drawBox(r,bg,line);ui.drawText(names[i],r.x+17,r.y+15,21,gold,true);ui.drawWrappedText(rules[i],{r.x+17,r.y+51,414,44},16,muted);
        if(ui.wasClicked(r)){finishAction(game.chooseContract(i+1));modal=Modal::None;}
    }
    if(ui.button({168,624,230,44},"No challenge",mint)){finishAction(game.chooseContract(0));modal=Modal::None;}
    if(ui.button({890,624,220,44},"Decide later",mint))modal=Modal::None;
}
void App::drawDraftDialog(){
    ui.drawBox({140,105,1000,590},panel,line);ui.drawDisplayText("Make the build yours",168,132,33,ink);
    ui.drawText("Take one free relic. Five slots maximum: sell an owned relic first, or pass.",168,184,17,muted);
    for(size_t i=0;i<game.draftChoices.size();++i){
        const int id=game.draftChoices[i];Rectangle r={168+i*316.f,231,296,320};Color tint=accent[relics[id].color];
        ui.drawBox(r,bg,Fade(tint,.7f));ui.drawRelicEmblem(id,{r.x+148,r.y+74},37,tint);
        ui.drawFittedText(relics[id].name,{r.x+19,r.y+137,258,29},25,ink,true);
        ui.drawWrappedText(relics[id].description,{r.x+19,r.y+185,258,70},18,muted);
        const bool owned=std::find(game.owned.begin(),game.owned.end(),id)!=game.owned.end();
        if(ui.button({r.x+19,r.y+266,258,38},owned?"Already owned":"Take for free",tint,true,game.owned.size()<5&&!owned)){
            finishAction(game.takeDraft(static_cast<int>(i)),3);if(game.draftTaken)modal=Modal::None;
        }
    }
    if(ui.button({168,601,230,44},"Pass this draft",mint)){finishAction(game.passDraft());modal=Modal::None;}
    if(ui.button({890,601,220,44},"Back to market",mint))modal=Modal::None;
}
} // namespace neon
