#pragma once

#ifdef __EMSCRIPTEN__
namespace neon {
bool restoreBrowserStorage();
bool persistBrowserStorage();
void requestBrowserBuildImport();
void downloadBrowserBuild();
}
#endif
