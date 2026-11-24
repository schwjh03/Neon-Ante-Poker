#include "ui.hpp"
#include <cmath>

namespace neon {
using namespace theme;

void Ui::drawRelicEmblem(int id, Vector2 p, float size, Color c) {
    const float k = size / 32;
    DrawCircleV({p.x+2,p.y+4},size*1.05f,{3,8,19,150});
    DrawCircleV(p,size*1.05f,ColorBrightness(c,-.82f));
    DrawCircleLinesV(p,size*1.05f,Fade(c,.55f));
    DrawCircleLinesV(p,size*.89f,Fade(c,.18f));
    for(int i=0;i<4;++i){const float a=i*PI/2+PI/4;drawStar({p.x+std::cos(a)*size*.93f,p.y+std::sin(a)*size*.93f},2*k,Fade(ink,.8f));}
    switch (id) {
    case 0:
        DrawRectangleRounded({p.x - 20 * k, p.y - 10 * k, 40 * k, 22 * k}, .4f, 8, c);
        for (int i = -2; i < 3; ++i) {
            DrawLineEx({p.x + i * 7 * k, p.y - 7 * k}, {p.x + i * 7 * k, p.y + 7 * k}, 2,
                       {22, 31, 37, 255});
        }
        DrawLineEx({p.x - 16 * k, p.y}, {p.x + 16 * k, p.y}, 2, {22, 31, 37, 255});
        break;
    case 1:
        DrawTriangle({p.x + 5 * k, p.y - 26 * k}, {p.x - 19 * k, p.y + 4 * k},
                     {p.x + 4 * k, p.y + 4 * k}, c);
        DrawTriangle({p.x - 5 * k, p.y + 26 * k}, {p.x + 19 * k, p.y - 4 * k},
                     {p.x - 4 * k, p.y - 4 * k}, c);
        break;
    case 2:
        DrawPolyLinesEx(p, 4, 23 * k, 0, 3, c);
        DrawPolyLinesEx(p, 4, 13 * k, 0, 2, c);
        drawStar(p, 9 * k, c);
        break;
    case 3:
        drawCenteredText("7", p.x, p.y - 27 * k, 53 * k, c, true);
        break;
    case 4:
        DrawCircleLinesV(p, 21 * k, c);
        DrawCircleLinesV(p, 15 * k, c);
        drawSuit(3, p, 10 * k, c);
        for (int i = 0; i < 8; ++i) {
            float a = i * PI / 4;
            DrawLineEx({p.x + std::cos(a) * 23 * k, p.y + std::sin(a) * 23 * k},
                       {p.x + std::cos(a) * 29 * k, p.y + std::sin(a) * 29 * k}, 3, c);
        }
        break;
    case 5:
        for (int i = -1; i <= 1; ++i) {
            DrawTriangle({p.x + i * 15 * k, p.y - 22 * k}, {p.x + (i * 15 - 8) * k, p.y + 9 * k},
                         {p.x + (i * 15 + 8) * k, p.y + 9 * k}, c);
        }
        DrawRectangle(static_cast<int>(p.x - 23 * k), static_cast<int>(p.y + 9 * k),
                      static_cast<int>(46 * k), static_cast<int>(8 * k), c);
        break;
    case 6:
        for (int i = 0; i < 3; ++i) {
            DrawRectangleRoundedLinesEx(
                {p.x - 21 * k + i * 7 * k, p.y - 24 * k + i * 6 * k, 28 * k, 36 * k}, .1f, 8, 2,
                Fade(c, .35f + i * .3f));
        }
        break;
    case 7:
        DrawRectangleRounded({p.x-20*k,p.y-19*k,40*k,37*k},.2f,8,ColorBrightness(c,-.35f));
        DrawRectangleRoundedLinesEx({p.x-20*k,p.y-19*k,40*k,37*k},.2f,8,2*k,c);
        drawCenteredText("$",p.x,p.y-23*k,40*k,gold,true);break;
    case 8: // Blue Wire: soldered circuit, not another Chrome Smile.
        for(int side:{-1,1}){
            const float x=p.x+side*16*k;
            DrawLineEx({x,p.y-20*k},{x,p.y+20*k},3*k,c);
            DrawCircleV({x,p.y-20*k},5*k,c);DrawCircleV({x,p.y+20*k},5*k,c);
            DrawCircleV({x,p.y-20*k},2*k,bg);DrawCircleV({x,p.y+20*k},2*k,bg);
        }
        DrawLineEx({p.x-16*k,p.y-8*k},{p.x+16*k,p.y+8*k},3*k,c);
        drawStar(p,9*k,ink);break;
    case 9: // Royal Static.
        for(int i=-1;i<=1;++i){DrawTriangle({p.x+i*14*k,p.y-22*k},{p.x+(i*14-7)*k,p.y+8*k},{p.x+(i*14+7)*k,p.y+8*k},c);DrawCircleV({p.x+i*14*k,p.y-22*k},3*k,ink);}
        DrawRectangleRounded({p.x-23*k,p.y+7*k,46*k,10*k},.3f,8,c);drawStar({p.x,p.y+10*k},5*k,bg);break;
    case 10: // Last Light.
        DrawCircleV({p.x,p.y-7*k},15*k,Fade(gold,.25f));DrawCircleLinesV({p.x,p.y-7*k},15*k,c);
        DrawLineEx({p.x-7*k,p.y+7*k},{p.x+7*k,p.y+7*k},3*k,c);
        for(int j=0;j<3;++j)DrawLineEx({p.x-7*k,p.y+(13+j*4)*k},{p.x+7*k,p.y+(13+j*4)*k},2*k,c);
        DrawLineEx({p.x-6*k,p.y-13*k},{p.x,p.y+6*k},2*k,ink);DrawLineEx({p.x+6*k,p.y-13*k},{p.x,p.y+6*k},2*k,ink);break;
    case 11: // Blood Moon.
        DrawCircleV(p,24*k,c);DrawCircleV({p.x+11*k,p.y-7*k},21*k,ColorBrightness(c,-.82f));
        drawStar({p.x+12*k,p.y+4*k},7*k,ink);break;
    case 12: // Compound: growing coin stack.
        for(int i=0;i<3;++i){const float x=p.x+(i-1)*17*k,y=p.y+(9-i*10)*k;
            for(int j=0;j<=i;++j){DrawCircleV({x,y+j*5*k},10*k,ColorBrightness(c,-.3f));DrawCircleLinesV({x,y+j*5*k},10*k,c);}DrawCircleV({x,y},9*k,c);drawStar({x,y},4*k,bg);}
        break;
    case 13: // Pocket Reactor.
        for(int i=0;i<3;++i){const float a=i*PI/3;DrawEllipseLines(static_cast<int>(p.x),static_cast<int>(p.y),25*k,(8+i*4)*k,Fade(c,.7f));DrawCircleV({p.x+std::cos(a)*23*k,p.y+std::sin(a)*17*k},3*k,ink);}
        DrawCircleV(p,8*k,c);drawStar(p,6*k,ink);break;
    case 14: // Deep Pockets.
        DrawRectangleRounded({p.x-23*k,p.y-17*k,46*k,34*k},.15f,8,ColorBrightness(c,-.45f));
        DrawRectangleRoundedLinesEx({p.x-23*k,p.y-17*k,46*k,34*k},.15f,8,2*k,c);
        DrawRectangleRounded({p.x+5*k,p.y-7*k,22*k,16*k},.3f,8,c);DrawCircleV({p.x+14*k,p.y+1*k},3*k,bg);
        DrawCircleV({p.x-9*k,p.y-20*k},10*k,gold);DrawCircleLinesV({p.x-9*k,p.y-20*k},7*k,ink);break;
    case 15: // Feedback Loop.
        DrawCircleLinesV({p.x-11*k,p.y},15*k,c);DrawCircleLinesV({p.x+11*k,p.y},15*k,c);
        DrawCircleLinesV({p.x-11*k,p.y},12*k,Fade(c,.6f));DrawCircleLinesV({p.x+11*k,p.y},12*k,Fade(c,.6f));
        drawStar({p.x-24*k,p.y-7*k},5*k,ink);drawStar({p.x+24*k,p.y+7*k},5*k,ink);break;
    default: drawStar(p,22*k,c);break;
    }
}

void Ui::drawRelic(int id, Rectangle r) {
    Color a = accent[relics[id].color];
    drawBox({r.x+2,r.y+5,r.width,r.height},{3,8,18,200});
    drawBox(r,ColorBrightness(a,-.84f),Fade(a,.7f));
    DrawRectangleRoundedLinesEx({r.x+4,r.y+4,r.width-8,r.height-8},.08f,8,1,Fade(a,.2f));
    drawRelicEmblem(id,{r.x+r.width/2,r.y+43},29,a);
    drawBox({r.x+5,r.y+r.height-33,r.width-10,28},{13,21,37,245},Fade(a,.15f),.12f);
    drawFittedText(relics[id].name,{r.x+10,r.y+r.height-29,r.width-20,24},19,ink,true);
}
} // namespace neon
