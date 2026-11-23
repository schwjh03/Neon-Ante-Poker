#include "ui.hpp"
#include <cmath>

namespace neon {
using namespace theme;

void Ui::drawSuit(int which, Vector2 p, float s, Color c) {
    if (which == 1) {
        DrawTriangle({p.x, p.y - s}, {p.x - s * .7f, p.y}, {p.x, p.y + s}, c);
        DrawTriangle({p.x, p.y - s}, {p.x, p.y + s}, {p.x + s * .7f, p.y}, c);
    } else if (which == 2) {
        DrawCircleV({p.x - s * .35f, p.y - s * .27f}, s * .47f, c);
        DrawCircleV({p.x + s * .35f, p.y - s * .27f}, s * .47f, c);
        DrawTriangle({p.x - s * .78f, p.y - s * .14f}, {p.x, p.y + s * .84f},
                     {p.x + s * .78f, p.y - s * .14f}, c);
    } else if (which == 3) {
        DrawCircleV({p.x - s * .32f, p.y + s * .16f}, s * .43f, c);
        DrawCircleV({p.x + s * .32f, p.y + s * .16f}, s * .43f, c);
        DrawTriangle({p.x, p.y - s}, {p.x - s * .72f, p.y + s * .1f},
                     {p.x + s * .72f, p.y + s * .1f}, c);
        DrawTriangle({p.x, p.y + s * .1f}, {p.x - s * .3f, p.y + s}, {p.x + s * .3f, p.y + s}, c);
    } else {
        DrawCircleV({p.x, p.y - s * .46f}, s * .43f, c);
        DrawCircleV({p.x - s * .43f, p.y + s * .05f}, s * .43f, c);
        DrawCircleV({p.x + s * .43f, p.y + s * .05f}, s * .43f, c);
        DrawTriangle({p.x, p.y + s * .12f}, {p.x - s * .32f, p.y + s}, {p.x + s * .32f, p.y + s},
                     c);
    }
}

void Ui::drawStar(Vector2 p, float radius, Color c) {
    for (int i = 0; i < 4; ++i) {
        float a = i * PI / 2;
        Vector2 v = {p.x + std::cos(a) * radius, p.y + std::sin(a) * radius};
        Vector2 l = {p.x + std::cos(a - .65f) * radius * .28f,
                     p.y + std::sin(a - .65f) * radius * .28f};
        Vector2 r = {p.x + std::cos(a + .65f) * radius * .28f,
                     p.y + std::sin(a + .65f) * radius * .28f};
        DrawTriangle(v, l, r, c);
    }
}

void Ui::drawCard(const Card& c, Rectangle r, bool selected, bool dim, bool back) {
    const float k = r.width / 92.f;
    const float cx = r.x + r.width / 2, cy = r.y + r.height * .51f;
    const Color paper = dim ? Color{187, 191, 184, 255} : Color{250, 247, 235, 255};
    const Color paint = c.wild ? Color{26, 112, 117, 255}
        : (c.suit == 1 || c.suit == 2) ? Color{174, 49, 54, 255} : Color{27, 42, 57, 255};
    drawBox({r.x + 1, r.y + 7, r.width, r.height}, {2, 7, 15, 125}, BLANK, .1f);
    drawBox({r.x, r.y + 3, r.width, r.height}, {5, 12, 20, 180}, BLANK, .1f);
    if (selected) drawBox({r.x - 3, r.y - 3, r.width + 6, r.height + 6}, gold, BLANK, .1f);
    drawBox(r, back ? Color{32, 54, 72, 255} : paper,
            selected ? gold : Color{202, 199, 184, 255}, .1f);
    if (back) {
        DrawRectangleRoundedLinesEx({r.x + 5*k, r.y + 5*k, r.width - 10*k, r.height - 10*k},
                                    .07f, 8, k, gold);
        for (float y=r.y+12*k; y<r.y+r.height-10*k; y+=9*k) {
            for (float x=r.x+11*k; x<r.x+r.width-8*k; x+=9*k) {
                DrawPolyLinesEx({x,y},4,3*k,0,.6f*k,Fade(gold,.2f));
            }
        }
        DrawCircleV({cx,cy},25*k,{22,38,54,255});
        DrawCircleLinesV({cx,cy},25*k,gold);
        drawSuit(3,{cx,cy-2*k},17*k,gold);
        return;
    }
    DrawLineEx({r.x+7*k,r.y+2*k},{r.x+r.width-7*k,r.y+2*k},k,WHITE);
    drawDisplayText(rankName(c.rank),r.x+7*k,r.y+3*k,23*k,paint);
    drawSuit(c.suit,{r.x+14*k,r.y+36*k},6*k,paint);
    drawDisplayText(rankName(c.rank),r.x+r.width-14*k,r.y+r.height-25*k,20*k,paint,true);
    drawSuit(c.suit,{r.x+r.width-14*k,r.y+r.height-36*k},5.5f*k,paint);
    if(c.rank>=11 && c.rank<=13) {
        Rectangle frame={cx-24*k,cy-37*k,48*k,74*k};
        DrawRectangleRec(frame,Fade(paint,.065f));
        DrawRectangleLinesEx(frame,.8f*k,Fade(paint,.5f));
        DrawLineEx({cx-23*k,cy},{cx+23*k,cy},1*k,Fade(paint,.35f));
        // Two mirrored court portraits, constructed from original geometric artwork.
        for(int side: {-1,1}) {
            const float sy=static_cast<float>(side);
            const float headY=cy+sy*16*k;
            DrawTriangle({cx-18*k,cy+sy*2*k},{cx,cy+sy*20*k},{cx+18*k,cy+sy*2*k},paint);
            DrawCircleV({cx,headY},9*k,paper);
            DrawCircleLinesV({cx,headY},9*k,paint);
            DrawLineEx({cx+3*k,headY-sy*3*k},{cx+6*k,headY-sy*3*k},1.2f*k,paint);
            DrawLineEx({cx+5*k,headY-sy*1*k},{cx+7*k,headY+sy*3*k},1.2f*k,paint);
            DrawLineEx({cx+1*k,headY+sy*5*k},{cx+5*k,headY+sy*5*k},1*k,paint);
            const float crownY=headY+sy*10*k;
            if(c.rank==11) {
                DrawLineEx({cx-9*k,crownY},{cx+10*k,crownY},3*k,paint);
                DrawLineEx({cx-7*k,crownY},{cx+7*k,crownY+sy*12*k},2*k,paint);
                DrawCircleV({cx+7*k,crownY+sy*12*k},2*k,gold);
            } else {
                DrawLineEx({cx-10*k,crownY},{cx+10*k,crownY},3*k,paint);
                for(int peak=-1;peak<=1;++peak) {
                    const float px=cx+peak*7*k;
                    DrawLineEx({px,crownY},{px,crownY+sy*(peak==0?9:6)*k},3*k,paint);
                    DrawCircleV({px,crownY+sy*(peak==0?9:6)*k},1.8f*k,gold);
                }
                if(c.rank==13) DrawLineEx({cx-5*k,headY+sy*5*k},{cx+3*k,headY+sy*7*k},3*k,paint);
            }
        }
        drawSuit(c.suit,{cx,cy},5*k,gold);
    } else if(c.rank==14) {
        drawSuit(c.suit,{cx,cy},23*k,paint);
        DrawLineEx({cx-12*k,cy+35*k},{cx+12*k,cy+35*k},.8f*k,Fade(paint,.45f));
    } else {
        auto pip=[&](float x,float y){drawSuit(c.suit,{cx+x*k,cy+y*k},6.8f*k,paint);};
        if(c.rank<=3) {
            pip(0,-27);pip(0,27);if(c.rank==3)pip(0,0);
        } else {
            pip(-18,-27);pip(18,-27);pip(-18,27);pip(18,27);
            if(c.rank==5 || c.rank==9)pip(0,0);
            if(c.rank>=6) {
                if(c.rank>=9) {
                    pip(-18,-9);pip(18,-9);pip(-18,9);pip(18,9);
                    if(c.rank==10){pip(0,-19);pip(0,19);}
                } else {
                    pip(-18,0);pip(18,0);
                    if(c.rank==7)pip(0,-14);
                    if(c.rank==8){pip(0,-14);pip(0,14);}
                }
            }
        }
    }
    if(c.foil) {
        for(int i=0;i<4;++i)DrawLineEx({r.x+4+i*3.f,r.y+5},{r.x+4+i*3.f,r.y+r.height-5},1,Fade(accent[i],.8f));
        drawStar({r.x+r.width-11*k,r.y+14*k},5*k,gold);
    }
    if(c.wild)for(int i=0;i<4;++i)DrawCircleV({cx+(i-1.5f)*8*k,r.y+r.height-9*k},2*k,accent[i]);
    if(dim)DrawLineEx({r.x+8*k,r.y+r.height-8*k},{r.x+r.width-8*k,r.y+8*k},1.2f*k,Fade(coral,.35f));
}
} // namespace neon
