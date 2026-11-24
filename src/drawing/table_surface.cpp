#include "ui.hpp"
#include <cmath>

namespace neon {
using namespace theme;

void Ui::drawChip(Vector2 p, float r, Color c, int suit) {
    DrawCircleV({p.x+1,p.y+4},r,{3,8,19,160});
    DrawCircleV(p,r,ColorBrightness(c,-.45f));
    DrawCircleV({p.x,p.y-2},r,c);
    for(int i=0;i<8;++i){
        const float a=i*PI/4;
        DrawLineEx({p.x+std::cos(a)*(r-2),p.y-2+std::sin(a)*(r-2)},
                   {p.x+std::cos(a)*(r-6),p.y-2+std::sin(a)*(r-6)},3,Fade(WHITE,.8f));
    }
    DrawCircleV({p.x,p.y-2},r*.64f,ColorBrightness(c,-.55f));
    DrawCircleLinesV({p.x,p.y-2},r*.64f,Fade(WHITE,.6f));
    drawSuit(suit,{p.x,p.y-2},r*.35f,ink);
}

void Ui::drawTableSurface(bool market) {
    ClearBackground(bg);
    const Color top=market?Color{29,35,65,255}:Color{23,72,73,255};
    const Color bottom=market?Color{16,23,43,255}:Color{10,37,48,255};
    DrawRectangleGradientV(264,75,1016,725,top,bottom);
    // A padded rail, stitched felt and an engraved brass perimeter.
    drawBox({278,259,988,535},{6,16,28,255},Fade(gold,.45f),.08f);
    drawBox({285,266,974,521},top,Fade(blue,.25f),.08f);
    DrawRectangleGradientV(297,278,950,490,top,bottom);
    for(int x=307;x<1240;x+=16){
        DrawLine(x,271,x+5,271,Fade(gold,.32f));
        DrawLine(x,781,x+5,781,Fade(gold,.32f));
    }
    for(int y=285;y<768;y+=16){DrawLine(290,y,290,y+5,Fade(gold,.32f));DrawLine(1253,y,1253,y+5,Fade(gold,.32f));}
    if(!market){
        DrawEllipse(773,430,430,152,{15,53,59,255});
        DrawEllipseLines(773,430,430,152,Fade(mint,.23f));
        DrawEllipseLines(773,430,423,145,Fade(gold,.28f));
        for(int i=0;i<4;++i)drawSuit(i,{360+i*274.f,420},18,Fade(gold,.12f));
        // A real deck stack and dealer button give the playing surface a sense of place.
        for(int i=3;i>=0;--i)drawCard({14,3,false,false},{1167+i*2.f,452+i*3.f,54,74},false,false,true);
        drawChip({340,491},20,gold,3);
        drawCenteredText("DEALER",395,483,16,muted,true);
        for(int i=0;i<3;++i)drawChip({1178+i*16.f,361-i*3.f},15,i==1?coral:blue,i);
    }
    // Low contrast woven grain lives behind the controls, never across text.
    for(int y=278;y<775;y+=6)for(int x=300;x<1246;x+=9)
        if((x*17u+y*31u)%11u<3)DrawPixel(x,y,{166,210,202,9});
}
} // namespace neon
