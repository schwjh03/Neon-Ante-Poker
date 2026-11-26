#include "ui.hpp"
#include "rlgl.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include "raymath.h"
#pragma GCC diagnostic pop
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdint>

namespace neon {
namespace {
#ifdef __EMSCRIPTEN__
const char* vertexShader=R"(#version 100
attribute vec3 vertexPosition; attribute vec2 vertexTexCoord; attribute vec3 vertexNormal; attribute vec4 vertexColor;
uniform mat4 mvp; uniform mat4 matNormal;
varying vec2 uv; varying vec3 normal; varying vec4 color;
void main(){uv=vertexTexCoord;normal=normalize(vec3(matNormal*vec4(vertexNormal,0.0)));color=vertexColor;gl_Position=mvp*vec4(vertexPosition,1.0);})";
const char* fragmentShader=R"(#version 100
precision mediump float;
varying vec2 uv; varying vec3 normal; varying vec4 color;
uniform sampler2D texture0; uniform vec4 colDiffuse; uniform vec4 atlasRect; uniform float paperLight;
void main(){vec4 surface=texture2D(texture0,atlasRect.xy+uv*atlasRect.zw)*colDiffuse*color;
vec3 n=normalize(normal);float key=max(dot(n,normalize(vec3(-.45,.8,.65))),0.0);float fill=max(dot(n,normalize(vec3(.8,.25,-.5))),0.0);
float sheen=pow(max(dot(n,normalize(vec3(-.3,.6,.9))),0.0),22.0)*.13;
gl_FragColor=vec4(surface.rgb*(.46+key*.55+fill*.18+paperLight)+sheen*surface.rgb,surface.a);})";
#else
const char* vertexShader=R"(#version 330
in vec3 vertexPosition; in vec2 vertexTexCoord; in vec3 vertexNormal; in vec4 vertexColor;
uniform mat4 mvp; uniform mat4 matNormal;
out vec2 uv; out vec3 normal; out vec4 color;
void main(){uv=vertexTexCoord;normal=normalize(vec3(matNormal*vec4(vertexNormal,0.0)));color=vertexColor;gl_Position=mvp*vec4(vertexPosition,1.0);})";
const char* fragmentShader=R"(#version 330
in vec2 uv; in vec3 normal; in vec4 color;
uniform sampler2D texture0; uniform vec4 colDiffuse; uniform vec4 atlasRect; uniform float paperLight;
out vec4 finalColor;
void main(){vec4 surface=texture(texture0,atlasRect.xy+uv*atlasRect.zw)*colDiffuse*color;
vec3 n=normalize(normal);float key=max(dot(n,normalize(vec3(-.45,.8,.65))),0.0);float fill=max(dot(n,normalize(vec3(.8,.25,-.5))),0.0);
float sheen=pow(max(dot(n,normalize(vec3(-.3,.6,.9))),0.0),22.0)*.13;
finalColor=vec4(surface.rgb*(.46+key*.55+fill*.18+paperLight)+sheen*surface.rgb,surface.a);})";
#endif

struct Vertex {Vector3 p,n;Vector2 uv;};
Mesh upload(const std::vector<Vertex>& vertices){
    Mesh mesh{};mesh.vertexCount=static_cast<int>(vertices.size());mesh.triangleCount=mesh.vertexCount/3;
    mesh.vertices=static_cast<float*>(MemAlloc(mesh.vertexCount*3*sizeof(float)));
    mesh.normals=static_cast<float*>(MemAlloc(mesh.vertexCount*3*sizeof(float)));
    mesh.texcoords=static_cast<float*>(MemAlloc(mesh.vertexCount*2*sizeof(float)));
    for(int i=0;i<mesh.vertexCount;++i){const auto& v=vertices[i];
        mesh.vertices[i*3]=v.p.x;mesh.vertices[i*3+1]=v.p.y;mesh.vertices[i*3+2]=v.p.z;
        mesh.normals[i*3]=v.n.x;mesh.normals[i*3+1]=v.n.y;mesh.normals[i*3+2]=v.n.z;
        mesh.texcoords[i*2]=v.uv.x;mesh.texcoords[i*2+1]=v.uv.y;
    }
    UploadMesh(&mesh,false);return mesh;
}
// Rounded, extruded solids. The front and the side wall have independent materials.
struct Solid {Model face{},edge{};};
Solid rounded(float width,float height,float radius,float thickness){
    std::vector<Vector2> ring;
    for(int corner=0;corner<4;++corner){
        const float cx=(corner==0||corner==3?1:-1)*(width/2-radius);
        const float cy=(corner<2?1:-1)*(height/2-radius);
        for(int i=0;i<=8;++i){float a=(corner*90+i*90.f/8)*DEG2RAD;ring.push_back({cx+std::cos(a)*radius,cy+std::sin(a)*radius});}
    }
    std::vector<Vertex> front,sides;
    const auto v=[&](Vector2 p,float z,Vector3 n){return Vertex{{p.x,p.y,z},n,{p.x/width+.5f,.5f-p.y/height}};};
    for(size_t i=0;i<ring.size();++i){const auto a=ring[i],b=ring[(i+1)%ring.size()];
        front.insert(front.end(),{v({0,0},0,{0,0,1}),v(a,0,{0,0,1}),v(b,0,{0,0,1})});
        const Vector3 n=Vector3Normalize({b.y-a.y,a.x-b.x,0});
        sides.insert(sides.end(),{v(a,0,n),v(a,-thickness,n),v(b,-thickness,n),v(a,0,n),v(b,-thickness,n),v(b,0,n)});
    }
    return {LoadModelFromMesh(upload(front)),LoadModelFromMesh(upload(sides))};
}
Texture2D grain(Color base,bool wood){
    Image image=GenImageColor(256,256,base);uint32_t noise=120391;
    for(int y=0;y<256;++y)for(int x=0;x<256;++x){noise=noise*1664525u+1013904223u;
        const int n=(static_cast<int>(noise>>28)-8)/(wood?3:4);
        const int streak=wood?static_cast<int>(std::sin(y*.17+std::sin(x*.024)*2)*5):((x+y)%3==0?1:0);
        ImageDrawPixel(&image,x,y,{static_cast<unsigned char>(std::clamp(base.r+n+streak,0,255)),static_cast<unsigned char>(std::clamp(base.g+n+streak,0,255)),static_cast<unsigned char>(std::clamp(base.b+n+streak,0,255)),255});
    }
    Texture2D texture=LoadTextureFromImage(image);UnloadImage(image);GenTextureMipmaps(&texture);SetTextureFilter(texture,TEXTURE_FILTER_TRILINEAR);return texture;
}
void release(Solid& solid){UnloadModel(solid.face);UnloadModel(solid.edge);}
}

struct Scene3D {
    Shader light{};int atlasLocation=-1,paperLocation=-1;
    RenderTexture2D table{},hand{},faces{},back{};
    Solid card{},rail{},felt{},wood{};
    Model cylinder{},cube{};
    Texture2D feltTexture{},woodTexture{},leatherTexture{};
    uint64_t deckHash=0,handHash=0;bool initialized=false,tableDrawn=false;
    int tableCash=-1,tableDeck=-1,tablePhase=-1;
    Camera3D tableCamera{{0,16,17},{0,0,.3f},{0,1,0},40,CAMERA_PERSPECTIVE};
    Camera3D handCamera{{0,0,500},{0,0,0},{0,1,0},77.3196f,CAMERA_PERSPECTIVE};

    void material(Model& model,Texture2D texture={}){
        model.materials[0].shader=light;
        if(texture.id)model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture=texture;
    }
    void rect(float x=0,float y=0,float w=1,float h=1){float value[]={x,y,w,h};SetShaderValue(light,atlasLocation,value,SHADER_UNIFORM_VEC4);}
    void solid(Solid& asset,Vector3 p,Vector3 scale,Color faceColor,Color edgeColor,float rotation=0){
        rect();DrawModelEx(asset.edge,p,{0,1,0},rotation,scale,edgeColor);DrawModelEx(asset.face,p,{0,1,0},rotation,scale,faceColor);
    }
    void disc(Vector3 p,float radius,float height,Color color){rect();DrawModelEx(cylinder,p,{0,1,0},0,{radius,height,radius},color);}
    void chipStack(Vector3 p,int count,Color color){
        disc({p.x,.47f,p.z},.66f,.008f,{2,7,13,90});
        for(int i=0;i<count;++i){const float y=.48f+i*.115f;
            disc({p.x,y,p.z},.47f,.09f,color);
            disc({p.x,y+.035f,p.z},.475f,.018f,{223,216,193,255});
            disc({p.x,y+.081f,p.z},.38f,.012f,ColorBrightness(color,-.22f));
        }
        disc({p.x,.48f+count*.115f-.02f,p.z},.23f,.012f,{221,195,132,255});
    }
    void initialize(Ui& ui){
        light=LoadShaderFromMemory(vertexShader,fragmentShader);
        light.locs[SHADER_LOC_MATRIX_NORMAL]=GetShaderLocation(light,"matNormal");
        atlasLocation=GetShaderLocation(light,"atlasRect");
        paperLocation=GetShaderLocation(light,"paperLight");
        table=LoadRenderTexture(1016,725);hand=LoadRenderTexture(1280,800);faces=LoadRenderTexture(2048,1024);back=LoadRenderTexture(144,200);
        card=rounded(104,145,6,3.2f);rail=rounded(19.6f,13.5f,3,.62f);felt=rounded(17.8f,11.7f,2.3f,.04f);wood=rounded(20.1f,14,3.2f,.65f);
        for(auto* asset:{&rail,&felt,&wood}){asset->face.transform=MatrixRotateX(-PI/2);asset->edge.transform=MatrixRotateX(-PI/2);}
        cylinder=LoadModelFromMesh(GenMeshCylinder(1,1,48));cube=LoadModelFromMesh(GenMeshCube(1,1,1));
        feltTexture=grain({21,79,75,255},false);woodTexture=grain({75,43,32,255},true);leatherTexture=grain({25,30,37,255},false);
        material(card.face,faces.texture);material(card.edge);material(rail.face,leatherTexture);material(rail.edge,leatherTexture);
        material(felt.face,feltTexture);material(felt.edge);material(wood.face,woodTexture);material(wood.edge,woodTexture);material(cylinder);material(cube);
        SetTextureFilter(faces.texture,TEXTURE_FILTER_BILINEAR);SetTextureFilter(hand.texture,TEXTURE_FILTER_BILINEAR);SetTextureFilter(table.texture,TEXTURE_FILTER_BILINEAR);
        BeginTextureMode(back);ClearBackground(BLANK);ui.drawCard({14,3,false,false},{0,0,144,200},false,false,true);EndTextureMode();
        initialized=table.id&&hand.id&&faces.id&&atlasLocation>=0;
    }
    void prepare(Ui& ui,const Game& game,const std::array<float,52>& lifts){
        if(!initialized)return;
        uint64_t hash=1469598103934665603ull;
        for(const auto& c:game.deck){hash^=c.rank+c.suit*16+c.foil*128+c.wild*256;hash*=1099511628211ull;}
        hash^=game.boss()+2;
        if(hash!=deckHash){
            BeginTextureMode(faces);ClearBackground(BLANK);
            for(int id=0;id<52;++id)ui.drawCard(game.deck[id],{(id%13)*152.f,(id/13)*212.f,144,200},false,game.boss()==2&&game.deck[id].suit==2);
            EndTextureMode();deckHash=hash;
        }
        float boost=0;
        // Reuse the static tabletop until its props or material actually change.
        if(!tableDrawn||tableCash!=game.cash||tableDeck!=static_cast<int>(game.draw.size())||tablePhase!=static_cast<int>(game.phase)){
        SetShaderValue(light,paperLocation,&boost,SHADER_UNIFORM_FLOAT);
        BeginTextureMode(table);ClearBackground({10,18,29,255});BeginMode3D(tableCamera);
        solid(wood,{0,-.65f,0},{1,1,1},WHITE,WHITE);
        solid(rail,{0,.05f,0},{1.012f,1.012f,1.012f},{171,132,75,255},{114,80,40,255});
        solid(rail,{0,.16f,0},{1,1,1},WHITE,WHITE);
        solid(felt,{0,.48f,0},{1,1,1},game.phase==Phase::Shop?Color{124,127,187,255}:WHITE,{12,41,42,255});
        chipStack({-6.6f,0,1.0f},4+std::min(game.cash/5,5),{45,133,152,255});
        chipStack({-5.5f,0,2.25f},3+std::min(game.cash/8,4),{181,64,83,255});
        chipStack({-7.3f,0,2.8f},3,{198,157,80,255});
        disc({-5.9f,.5f,4.6f},.67f,.11f,{225,210,163,255});disc({-5.9f,.62f,4.6f},.54f,.025f,{33,68,73,255});
        const int layers=std::clamp(static_cast<int>(game.draw.size())/4,2,13);
        for(int i=0;i<layers;++i){
            rect();DrawModelEx(cube,{6.4f,.53f+i*.028f,3.2f},{0,1,0},-9,{1.55f,.026f,2.15f},i%2?Color{238,228,200,255}:Color{186,184,168,255});
        }
        // The top card is the same original card-back artwork on a real mesh.
        material(card.face,back.texture);card.face.transform=MatrixMultiply(MatrixRotateX(-PI/2),MatrixRotateY(-9*DEG2RAD));
        rect(0,1,1,-1);DrawModelEx(card.face,{6.4f,.56f+layers*.028f,3.2f},{0,1,0},0,{.0149f,.0149f,.0149f},WHITE);
        material(card.face,faces.texture);
        EndMode3D();EndTextureMode();
        tableDrawn=true;tableCash=game.cash;tableDeck=static_cast<int>(game.draw.size());tablePhase=static_cast<int>(game.phase);
        }

        uint64_t nextHand=hash;
        const float handStart=772-(game.hand.size()*117.f-13)/2;
        for(size_t i=0;i<game.hand.size();++i){
            const int id=game.hand[i];
            const bool hover=CheckCollisionPointRec(ui.mouse,{handStart+i*117.f,573-lifts[id],104,145});
            const bool selected=std::find(game.selected.begin(),game.selected.end(),id)!=game.selected.end();
            nextHand^=id+static_cast<int>(lifts[id]*10)*64+hover*16384+selected*32768;
            nextHand*=1099511628211ull;
        }
        // Card models are redrawn during selection/hover/lift changes, not every idle frame.
        if(nextHand==handHash)return;
        handHash=nextHand;
        // A near plane sized for this camera keeps the thin card layers distinct on WebGL.
        // The default .01 near plane loses depth precision at a distance of 500 units.
        const double previousNear=rlGetCullDistanceNear(),previousFar=rlGetCullDistanceFar();
        rlSetClipPlanes(5,1000);
        BeginTextureMode(hand);ClearBackground(BLANK);BeginMode3D(handCamera);
        boost=.2f;SetShaderValue(light,paperLocation,&boost,SHADER_UNIFORM_FLOAT);
        const float start=772-(game.hand.size()*117.f-13)/2;
        // Contact shadows sit below the cards, with individual lifted, beveled models above.
        material(card.face);card.face.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture=card.edge.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture;
        rect();card.face.transform=MatrixIdentity();
        for(size_t i=0;i<game.hand.size();++i){const int id=game.hand[i];DrawModelEx(card.face,{start+i*117.f+52-640+5,400-(573-lifts[id]+72.5f)-10,-9},{0,0,1},0,{1.04f,1.04f,1},{0,0,0,90});}
        material(card.face,faces.texture);
        for(size_t i=0;i<game.hand.size();++i){
            const int id=game.hand[i];const bool selected=std::find(game.selected.begin(),game.selected.end(),id)!=game.selected.end();
            const Rectangle bounds={start+i*117.f,573-lifts[id],104,145};
            const bool hover=CheckCollisionPointRec(ui.mouse,bounds);
            const float pitch=(hover?-9.f:4.f)*DEG2RAD,yaw=(static_cast<float>(i)-3.5f)*.9f*DEG2RAD;
            card.face.transform=card.edge.transform=MatrixRotateXYZ({pitch,yaw,0});
            const Vector3 p={bounds.x+52-640,400-(bounds.y+72.5f)-(hover?-3:0),selected?8.f:0.f};
            if(selected){
                material(card.face,card.edge.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture);
                rect();DrawModelEx(card.face,{p.x,p.y,p.z-4},{0,0,1},0,{1.05f,1.04f,1},theme::gold);
                material(card.face,faces.texture);
            }
            rect();DrawModelEx(card.edge,p,{0,0,1},0,{1,1,1},selected?theme::gold:Color{183,177,156,255});
            rect(id%13*152.f/2048,1-id/13*212.f/1024,144.f/2048,-200.f/1024);
            DrawModelEx(card.face,p,{0,0,1},0,{1,1,1},WHITE);
        }
        EndMode3D();EndTextureMode();
        rlSetClipPlanes(previousNear,previousFar);
    }
    ~Scene3D(){
        release(card);release(rail);release(felt);release(wood);UnloadModel(cylinder);UnloadModel(cube);
        UnloadTexture(feltTexture);UnloadTexture(woodTexture);UnloadTexture(leatherTexture);
        UnloadRenderTexture(table);UnloadRenderTexture(hand);UnloadRenderTexture(faces);UnloadRenderTexture(back);UnloadShader(light);
    }
};
void Ui::prepareScene3D(const Game& game,const std::array<float,52>& lifts){
    if(!scene3D){scene3D=new Scene3D;scene3D->initialize(*this);}scene3D->prepare(*this,game,lifts);
}
void Ui::unloadScene3D(){delete scene3D;scene3D=nullptr;}
bool Ui::hasScene3D()const{return scene3D&&scene3D->initialized;}
void Ui::drawScene3D(){DrawTextureRec(scene3D->table.texture,{0,0,1016,-725},{264,75},WHITE);}
void Ui::drawHand3D(){DrawTextureRec(scene3D->hand.texture,{0,0,1280,-800},{0,0},WHITE);}
} // namespace neon
