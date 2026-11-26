#ifdef __EMSCRIPTEN__
#include "web_platform.hpp"
#include "app.hpp"
#include <emscripten.h>
#include <fstream>
#include <cstring>

namespace neon {
EM_JS(int, restoreWebFiles, (), {
    FS.mkdirTree('/neon');
    try {
        var raw=localStorage.getItem('neon-ante-browser-v1');
        if(raw && raw.length<180000){
            var save=JSON.parse(raw);
            if(save && typeof save.run==='string' && save.run.length>0 && save.run.length<65536)FS.writeFile('/neon/run-v2.save',save.run);
            if(save && typeof save.library==='string' && save.library.length<65536)FS.writeFile('/neon/build-library.txt',save.library);
        }
        return 1;
    } catch(error) { return 0; }
});

EM_JS(int, persistWebFiles, (), {
    try {
        var run=FS.analyzePath('/neon/run-v2.save').exists?FS.readFile('/neon/run-v2.save',{encoding:'utf8'}):"";
        var library=FS.analyzePath('/neon/build-library.txt').exists?FS.readFile('/neon/build-library.txt',{encoding:'utf8'}):"";
        localStorage.setItem('neon-ante-browser-v1',JSON.stringify({run:run,library:library}));
        var label=document.getElementById('storage-note');
        if(label)label.textContent='Run saved in this browser.';
        return 1;
    } catch(error) {
        var label=document.getElementById('storage-note');
        if(label)label.textContent='Browser storage is unavailable. Keep this tab open to retain this run.';
        return 0;
    }
});

bool restoreBrowserStorage(){return restoreWebFiles()!=0;}
bool persistBrowserStorage(){return persistWebFiles()!=0;}

bool App::beginBrowserRun(int mode, int starter){
    if(mode<0||mode>3||starter<0||starter>2)return false;
    pendingMode=mode;pendingStarter=starter;
    startConfiguredRun();
    return true;
}

void requestBrowserBuildImport(){
    EM_ASM({ document.getElementById('build-upload').click(); });
}
void downloadBrowserBuild(){
    EM_ASM({
        var bytes=FS.readFile('/neon/export.neonbuild');
        var url=URL.createObjectURL(new Blob([bytes],{type:'text/plain'}));
        var link=document.createElement('a');link.href=url;link.download='Neon-Ante-build.neonbuild';
        document.body.appendChild(link);link.click();link.remove();
        setTimeout(function(){URL.revokeObjectURL(url);},1000);
    });
}

void App::exportBrowserBuild(){
    if(exportBuild(captureBuild(game),"/neon/export.neonbuild")){
        downloadBrowserBuild();setupMessage="Build downloaded. Share the file yourself.";
    }else setupMessage="Could not export this build.";
}

void App::importBrowserBuild(const char* text){
    if(!text||std::strlen(text)>8192){setupMessage="That build file is too large.";return;}
    const char* path="/neon/import.neonbuild";
    {std::ofstream file(path);file<<text;if(!file){setupMessage="Could not read that build file.";return;}}
    BuildProfile imported;
    if(!importBuild(path,imported))setupMessage="That is not a valid Neon Ante build. The current run is unchanged.";
    else if(library.add(imported)){
        const bool saved=library.save(libraryPath)&&persistBrowserStorage();
        setupMessage=saved?"Shared build imported. The next Echo run can use it.":"Build imported for this session. Browser storage is unavailable.";
    }else setupMessage="That build is already in the library.";
    modal=Modal::RunSetup;
}
} // namespace neon
#endif
