#include "class_graphics.hpp"

#pragma once

inline uint16_t color_to_gba(const Color& c)
{
    return (((c.red >> 3) & 31) | (((c.green >> 3) & 31) << 5) | (((c.blue >> 3) & 31) << 10));
}

inline std::vector<uint16_t> color_to_gba(const std::vector<Color>& colors)
{
    std::vector<uint16_t> gba_colors;
    gba_colors.reserve(colors.size());

    for(const Color& c : colors)
    {
        gba_colors.push_back(color_to_gba(c));
    }

    return gba_colors;
}

inline Color gba_to_color(const uint16_t color)
{
    return Color((color & 31) << 3, ((color >> 5) & 31) << 3, ((color >> 10) & 31) << 3);
}

inline Palette gba_to_palette(const std::vector<uint16_t> gba_colors)
{
    Palette palette;
    palette.reserve(gba_colors.size());

    for(const uint16_t& c : gba_colors)
    {
        palette.push_back(gba_to_color(c));
    }

    return palette;
}