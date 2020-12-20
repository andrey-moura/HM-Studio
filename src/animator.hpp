#pragma once

#include <wx/wx.h>
#include <wx/dir.h>

#include <vector>
#include <ctype.h>

#include "class_editor.hpp"
#include "class_graphics.hpp"
#include "gba_graphics.hpp"

struct AnimRange
{
    uint16_t length;
    uint16_t start;
};

struct AnimInstruction
{
    uint16_t frame;
    uint16_t time;
};

struct AnimAff
{
    uint16_t data[4];
};

struct FrameInfo
{
    AnimRange oam;
    AnimRange tile;
    AnimRange color;
    AnimRange affine;
};

struct SpriteAttribute
{
    signed y : 8;
    unsigned : 6;
    unsigned shape : 2;

    signed x : 9;
    unsigned : 5;
    unsigned size : 2;

    unsigned tile : 10;
    unsigned : 2;
    unsigned palette : 4;

    unsigned unused : 16;
};

struct FramePiece
{
    Graphics graphics;
    Palette palette;
    int x;
    int y;
};

struct Frame
{
    int x;
    int y;

    int w;
    int h;

    std::vector<FramePiece> pieces;

    void update_size()
    {
        w = 0;
        h = 0;

        for (FramePiece& piece : pieces)
        {
            w = std::max(w, piece.x + (int)piece.graphics.GetWidth());
            h = std::max(w, piece.y + (int)piece.graphics.GetHeight());
        }
    }
};

class Animator
{    
public:
    std::vector<Frame> m_Frames;
private:
    std::vector<AnimRange> m_AnimRanges;    
    std::vector<FrameInfo> m_FrameInfos;        
    std::vector<SpriteAttribute> m_Attributes;
    std::vector<Graphics> m_Tiles;
    std::vector<Palette> m_Palettes;
    std::vector<AnimAff> m_Affines;
    std::vector<AnimInstruction> m_Instructions;    
public:    
    void LoadFromFile(wxFile& file);
    void WriteToFile(wxFile& file);    

    void GenerateFrame(size_t n);
    void GenerateFrames();

    Frame& GetFrame(uint32_t n) { return m_Frames[n]; }
    std::vector<Frame>& GetFrames() { return m_Frames; }

    uint32_t GetLength() const;    

    FrameInfo& GetFrameInfo(size_t n) { return m_FrameInfos[n]; }
    std::vector<FrameInfo>& GetFramesInfo() { return m_FrameInfos; }

    size_t GetTileCount() { return m_Tiles.size(); }
    Graphics& GetTile(size_t n) { return m_Tiles[n]; }    
    std::vector<Graphics>& GetTiles() { return m_Tiles; } 

    Palette& GetPalette(size_t n) { return m_Palettes[n]; }
    Palette& GetFramePalette(size_t n);

    SpriteAttribute& GetAttribute(size_t n) { return m_Attributes[n]; }

    void SetRange(size_t n, const AnimRange& range) { m_AnimRanges[n] = range; }
    AnimRange GetAnimRange(size_t n) { return m_AnimRanges[n]; }
    std::vector<AnimRange>& GetAnimRanges() { return m_AnimRanges; }
    size_t GetAnimCount() { return m_AnimRanges.size(); }

    void SetInstruction(size_t n, const AnimInstruction& instruction) { m_Instructions[n] = instruction; }
    AnimInstruction GetInstruction(size_t n) { return m_Instructions[n]; }
    std::vector<AnimInstruction>& GetInstructions() { return m_Instructions; }    

    std::vector<std::pair<int, int>> GetPositions(size_t n, int* frame_x = nullptr, int* frame_y = nullptr);
public:
    static std::pair<uint16_t, uint16_t>* GetSizeList();
    static uint32_t FindAnimatorOffset(wxFile& file);
};