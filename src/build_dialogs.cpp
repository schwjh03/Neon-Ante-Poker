#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#include "build_profiles.hpp"

namespace neon {
std::filesystem::path chooseBuildFile(bool saving){
    wchar_t name[32768]=L"Neon-Ante-build.neonbuild";OPENFILENAMEW dialog{};
    dialog.lStructSize=sizeof(dialog);dialog.lpstrFile=name;dialog.nMaxFile=32768;
    dialog.lpstrFilter=L"Neon Ante build (*.neonbuild)\0*.neonbuild\0\0";
    dialog.lpstrDefExt=L"neonbuild";dialog.lpstrTitle=saving?L"Export a Neon Ante build":L"Import a shared Neon Ante build";
    dialog.Flags=OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST|(saving?OFN_OVERWRITEPROMPT:OFN_FILEMUSTEXIST);
    return (saving?GetSaveFileNameW(&dialog):GetOpenFileNameW(&dialog))?std::filesystem::path(name):std::filesystem::path{};
}
} // namespace neon
