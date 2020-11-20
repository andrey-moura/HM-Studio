#pragma once

#include <vector>
#include <ctype.h>

#include "class_graphics.hpp"

class Animation
{
public:
    Animation(RomFile& file, uint32_t offset);
    Animation() = default;
private:
    std::vector<Graphics> m_Frames;
    size_t m_CurrentFrame;
public:
    void LoadFromRom(RomFile& file, uint32_t offset);
    void NextFrame();
};