#include "app.hpp"
#include <algorithm>

namespace neon {
using namespace theme;

void App::drawHeader() {
    DrawRectangleGradientV(0,0,1280,74,{32,30,54,255},bg);
    DrawLine(24, 74, 1256, 74, line);
    DrawCircleLines(42,36,23,Fade(gold,.7f));
    ui.drawSuit(3, {42, 35}, 16, mint);
    ui.drawDisplayText("Neon Ante", 78, 12, 32, ink);
    ui.drawText("THE MIDNIGHT CARD CLUB", 80, 49, 16, muted, true);
    if(ui.button({440,19,245,37},game.modeName()+" / Modes",mint)){pendingMode=static_cast<int>(game.mode);pendingStarter=game.starter;modal=Modal::RunSetup;}
    ui.drawCenteredText("ANTE " + std::to_string(game.ante) + (game.endless ? "" : " / 8"), 876, 24,
                        24, gold, true);
    if (ui.button({1000, 19, 56, 37}, audio.muted ? "M" : "~", mint)) {
        audio.muted = !audio.muted;
    }
    ui.showTooltip(audio.muted ? "Unmute sound (M)" : "Mute sound (M)", {1000, 19, 56, 37});
    if (ui.button({1066, 19, 56, 37}, "?", mint)) {
        modal = Modal::HandReference;
        audio.play(0);
    }
    ui.showTooltip("Hand reference (H)", {1066, 19, 56, 37});
    if (ui.button({1132, 19, 120, 37}, "Menu", mint)) {
        modal = Modal::Menu;
        audio.play(0);
    }
}

void App::drawSidebar() {
    DrawRectangleGradientV(0,75,264,725,{28,35,54,255},{15,24,41,255});
    DrawLine(264, 75, 264, 800, line);
    ui.drawText("CURRENT BLIND", 24, 99, 16, muted, true);
    Color c = game.blind == 2 ? coral : game.blind == 1 ? gold : mint;
    DrawCircle(49, 151, 25, Fade(c, .12f));
    DrawCircleLines(49, 151, 25, c);
    ui.drawSuit(game.blind == 2 ? 3 : 1, {49, 151}, 16, c);
    ui.drawFittedText(game.blindName(), {87, 132, 158, 28}, 25, ink, true);
    ui.drawText(game.blind == 2   ? "BOSS BLIND"
                : game.blind == 1 ? "SECOND BLIND"
                                  : "OPENING BLIND",
                87, 162, 16, c, true);
    ui.drawWrappedText(game.boss() < 0 ? "Reach the target before your hands run out."
                                       : bossRules[game.boss()],
                       {24, 199, 216, 68}, 16, muted);
    ui.drawText("TARGET", 24, 276, 16, muted, true);
    ui.drawFittedText(number(game.target()), {24, 297, 216, 40}, 37, ink, true);
    ui.drawBox({24, 346, 216, 8}, {11, 21, 23, 255}, BLANK, .7f);
    float fill = std::min(1.0, game.score / static_cast<double>(game.target()));
    if (fill > 0) {
        ui.drawBox({24, 346, 216 * fill, 8}, c, BLANK, .7f);
    }
    ui.drawFittedText(number(game.score) + " scored", {24, 368, 216, 24}, 21, c, true);
    DrawLine(24, 411, 240, 411, line);
    ui.drawText("HANDS", 24, 432, 16, muted, true);
    ui.drawText("DISCARDS", 135, 432, 16, muted, true);
    ui.drawText(std::to_string(game.hands), 24, 455, 40, blue, true);
    ui.drawText(std::to_string(game.discards), 135, 455, 40, coral, true);
    DrawLine(24, 515, 240, 515, line);
    ui.drawText("BANK", 24, 539, 16, muted, true);
    ui.drawText("$" + number(game.cash), 24, 561, 40, gold, true);
    ui.drawText("Interest  $" + std::to_string(std::min(5, game.cash / 5)) + " / blind", 24, 612,
                16, muted);
    if(game.phase==Phase::Shop){
        const auto& chapter=clubChapters[std::min(7,game.ante-1)];
        if(ui.button({24,644,216,36},game.clubStoryPending()?"Meet "+std::string(chapter.character):"Club journal",gold,true)){clubPage=std::min(7,game.ante-1);modal=Modal::ClubStory;}
    }else if(game.canChooseContract()){
        if(ui.button({24,644,216,36},"Table challenge",gold)){modal=Modal::Contract;}
    }else ui.drawFittedText(game.contract?game.contractName():game.starterName()+" / round "+std::to_string(game.round),{24,650,216,27},18,game.contract?gold:ink,true);
    ui.drawText(game.mode==RunMode::Echo?"Echo target +"+std::to_string(static_cast<int>((game.echoScale-1)*100+.5))+"%":"Seed "+(game.runSeed?std::to_string(game.runSeed):"legacy"),24,695,16,muted);
    if (ui.button({24, 725, 216, 42}, "Deck  " + std::to_string(game.draw.size()) + " / 52",
                  mint)) {
        modal = Modal::Deck;
        audio.play(0);
    }
}

void App::drawRelicRow() {
    int moveIndex=-1,moveDirection=0;
    ui.drawText("YOUR RELICS", 292, 92, 16, ink, true);
    ui.drawText(std::to_string(game.owned.size()) + " / 5", 1209, 92, 16, muted, true);
    for (int i = 0; i < 5; ++i) {
        Rectangle r = {292 + i * 193.f, 118, 177, 114};
        if (i < static_cast<int>(game.owned.size())) {
            int id = game.owned[i];
            ui.drawRelic(id, r);
            if(ui.button({r.x+6,r.y+6,25,23},"<",mint,false,i>0)){moveIndex=i;moveDirection=-1;}
            if(ui.button({r.x+r.width-31,r.y+6,25,23},">",mint,false,i+1<static_cast<int>(game.owned.size()))){moveIndex=i;moveDirection=1;}
            ui.showTooltip(std::string(relics[id].description)+" / Move slots to change scoring order.", r);
            if (ui.wasClicked(r)) {
                modal = Modal::Relic;
                relicDetailIndex = i;
                audio.play(0);
            }
        } else {
            ui.drawBox(r, Fade(bg, .25f), Fade(line, .65f), .12f);
            DrawCircleLines(r.x+r.width/2,165,17,Fade(gold,.2f));
            ui.drawDisplayText(std::to_string(i+1),r.x+r.width/2,153,22,Fade(gold,.6f),true);
            ui.drawCenteredText("RELIC SLOT", r.x + r.width / 2, 202, 11, muted);
        }
    }
    ui.drawText("Effects resolve left to right. Move a relic with its corner controls.",292,240,16,muted);
    if(moveIndex>=0)finishAction(game.moveRelic(moveIndex,moveDirection));
}
} // namespace neon
