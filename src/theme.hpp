#pragma once

#include "raylib.h"

namespace neon::theme {
inline constexpr float canvasWidth = 1280;
inline constexpr float canvasHeight = 800;

inline constexpr Color bg = {14, 21, 31, 255};
inline constexpr Color panel = {22, 32, 44, 255};
inline constexpr Color line = {58, 73, 88, 255};
inline constexpr Color ink = {244, 241, 229, 255};
inline constexpr Color muted = {160, 173, 185, 255};
inline constexpr Color mint = {223, 183, 106, 255};
inline constexpr Color coral = {223, 119, 112, 255};
inline constexpr Color gold = {238, 204, 139, 255};
inline constexpr Color blue = {126, 194, 201, 255};
inline constexpr Color accent[] = {blue, coral, mint, gold};
} // namespace neon::theme
