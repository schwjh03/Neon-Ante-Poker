#ifdef __EMSCRIPTEN__
#include "runtime.hpp"
#include "app.hpp"
#include "web_platform.hpp"
#include <emscripten.h>
#include <algorithm>
#include <memory>
#include <deque>

namespace neon {
namespace {
std::unique_ptr<App> browserApp;
RenderTexture2D browserCanvas{};
std::string lastStatus;
bool browserPlaying = false;
std::deque<Vector2> browserClicks;

void showBrowserTitle(){
    browserPlaying=false;
    browserClicks.clear();
    const auto& g=browserApp->game;
    const std::string summary=g.modeName()+" / "+g.starterName()+" / Ante "+std::to_string(g.ante)+
        " / "+g.blindName()+" / $"+number(g.cash)+
        (g.phase==Phase::Shop?" / Night Market":g.phase==Phase::Won?" / Victory":g.phase==Phase::Lost?" / Run ended":"");
    EM_ASM({if(window.neonGameReady)window.neonGameReady($0,UTF8ToString($1));},
           browserApp->browserRunAvailable,summary.c_str());
}

void drawBrowserFrame(){
    if(!browserPlaying)return;
    auto& app=*browserApp;
    const float scale=std::min(GetScreenWidth()/theme::canvasWidth,GetScreenHeight()/theme::canvasHeight);
    const Vector2 offset={(GetScreenWidth()-theme::canvasWidth*scale)/2,(GetScreenHeight()-theme::canvasHeight*scale)/2};
    const auto mouse=GetMousePosition();
    app.ui.mouse={(mouse.x-offset.x)/scale,(mouse.y-offset.y)/scale};
    // Preserve short DOM pointer presses even when a 3D frame takes longer than the click.
    app.ui.clicked=!browserClicks.empty();
    if(app.ui.clicked){app.ui.mouse=browserClicks.front();browserClicks.pop_front();}
    app.update(std::min(GetFrameTime(),.05f));
    app.ui.prepareScene3D(app.game,app.cardLift);
    BeginTextureMode(browserCanvas);app.render();EndTextureMode();
    BeginDrawing();ClearBackground(theme::bg);
    DrawTexturePro(browserCanvas.texture,{0,0,theme::canvasWidth,-theme::canvasHeight},
                   {offset.x,offset.y,theme::canvasWidth*scale,theme::canvasHeight*scale},{0,0},0,WHITE);
    EndDrawing();

    const auto& g=app.game;
    std::string status="Ante "+std::to_string(g.ante)+" / "+g.blindName()+" / "+number(g.score)+" of "+number(g.target())+
        " / Hands "+std::to_string(g.hands)+" / Discards "+std::to_string(g.discards)+" / $"+number(g.cash);
    if(!g.selected.empty()){
        const auto score=g.preview();
        status+=" / Selected "+std::to_string(g.selected.size())+": "+handNames[static_cast<int>(score.hand)]+" = "+number(score.total);
    }
    if(status!=lastStatus){
        lastStatus=status;
        EM_ASM({document.getElementById('table-status').textContent=UTF8ToString($0);},status.c_str());
    }
    if(app.quitRequested){
        app.quitRequested=false;
        showBrowserTitle();
    }
}
} // namespace

extern "C" EMSCRIPTEN_KEEPALIVE void neon_import_build(const char* text){
    if(browserApp)browserApp->importBrowserBuild(text);
}
extern "C" EMSCRIPTEN_KEEPALIVE void neon_export_build(){
    if(browserApp)browserApp->exportBrowserBuild();
}
extern "C" EMSCRIPTEN_KEEPALIVE void neon_pointer_pressed(float x,float y){
    if(browserPlaying&&browserClicks.size()<16&&x>=0&&x<=1280&&y>=0&&y<=800)browserClicks.push_back({x,y});
}
extern "C" EMSCRIPTEN_KEEPALIVE int neon_begin_run(int mode,int starter){
    if(!browserApp||!browserApp->beginBrowserRun(mode,starter))return 0;
    browserPlaying=true;
    return 1;
}
extern "C" EMSCRIPTEN_KEEPALIVE int neon_resume_run(){
    if(!browserApp||!browserApp->browserRunAvailable)return 0;
    browserApp->modal=Modal::None;
    browserPlaying=true;
    return 1;
}
extern "C" EMSCRIPTEN_KEEPALIVE void neon_return_title(){
    if(!browserApp)return;
    if(browserApp->browserRunAvailable)browserApp->saveRun();
    showBrowserTitle();
}

int runApplication(int, char**){
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1280,800,"Neon Ante Poker");
    if(!IsWindowReady())return 1;
    SetExitKey(KEY_NULL);
    restoreBrowserStorage();
    browserApp=std::make_unique<App>(false);
    browserApp->ui.loadFonts();browserApp->audio.init();
    browserCanvas=LoadRenderTexture(1280,800);
    SetTextureFilter(browserCanvas.texture,TEXTURE_FILTER_BILINEAR);
    showBrowserTitle();
    emscripten_set_main_loop(drawBrowserFrame,0,1);
    return 0;
}
} // namespace neon
#endif
