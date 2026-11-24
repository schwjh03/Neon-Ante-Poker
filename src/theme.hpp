#pragma once

#include "raylib.h"

namespace neon::theme {
inline constexpr float canvasWidth = 1280;
inline constexpr float canvasHeight = 800;

inline constexpr Color bg = {13, 17, 31, 255};
inline constexpr Color panel = {25, 33, 52, 255};
inline constexpr Color line = {82, 101, 124, 255};
inline constexpr Color ink = {249, 248, 242, 255};
inline constexpr Color muted = {199, 211, 225, 255};
inline constexpr Color mint = {120, 232, 205, 255};
inline constexpr Color coral = {255, 145, 164, 255};
inline constexpr Color gold = {238, 204, 139, 255};
inline constexpr Color blue = {124, 208, 255, 255};
inline constexpr Color accent[] = {blue, coral, mint, gold};
} // namespace neon::theme
