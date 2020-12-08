#include "animator.hpp"

std::pair<uint16_t,uint16_t> sprite_sizes[] = 
{
    { 8,  8 },
	{ 16, 16 },
	{ 32, 32 },
	{ 64, 64 },

	{ 16, 8 },
	{ 32, 8 },
	{ 32, 16 },
	{ 64, 32 },

	{ 8,  16 },
	{ 8,  32 },
	{ 16, 32 },
	{ 32, 64 }
};

using range = std::vector<std::pair<uint16_t,uint16_t>>;

uint32_t AnimatorEditor::GetLength() const
{
    uint32_t size = 7 * 4;
    size += m_AnimRanges.size()*sizeof(AnimRange);
    size += m_FrameInfos.size()*sizeof(FrameInfo);
    size += m_Attributes.size()*sizeof(SpriteAttribute);
    size += m_Tiles.size()*32;
    size += m_Palettes.size()*32;
    size += m_Affines.size()*sizeof(AnimAff);
    size += m_Instructions.size()*sizeof(AnimInstruction);

    return size; 
}

void AnimatorEditor::LoadFromFile(wxFile& file)
{
    m_Animations.clear();
    m_Frames.clear();    

    uint32_t animCount;
    file.Read(&animCount, sizeof(uint32_t));
    
    m_Animations.reserve(animCount);
    
    m_AnimRanges.resize(animCount);

    for(size_t i = 0; i < animCount; ++i)
    {        
        file.Read(&m_AnimRanges[i], sizeof(AnimRange));        
    }
    
    file.Read(&m_FrameCount, sizeof(uint32_t));

    m_FrameInfos.resize(m_FrameCount);    

    for(size_t i = 0; i < m_FrameCount; ++i)
    {
        file.Read(&m_FrameInfos[i], sizeof(FrameInfo));
    }

    uint32_t oamCount;
    file.Read(&oamCount, sizeof(uint32_t));

    m_Attributes.resize(oamCount);

    for(size_t i = 0; i < oamCount; ++i)
    {
        file.Read(&m_Attributes[i], sizeof(SpriteAttribute));
    }
    
    uint32_t tileCount;
    file.Read(&tileCount, sizeof(uint32_t));

    m_Tiles.reserve(tileCount);

    for(size_t i = 0; i < tileCount; ++i)
    {        
        Graphics tile;
        tile.Create(8, 8);

        file.Read(tile.GetData(), tile.GetLenght());

        m_Tiles.push_back(tile);
    }

    uint32_t palette_count;
    file.Read(&palette_count, sizeof(uint32_t));

    m_Palettes.reserve(palette_count);

    for(size_t i = 0; i < palette_count; ++i)
    {
        uint16_t* raw_palette = new uint16_t[16];
        file.Read(raw_palette, 32);

        m_Palettes.push_back(Palette(raw_palette, 4));        
    }    

    uint32_t affCount;
    file.Read(&affCount, sizeof(uint32_t));

    m_Affines.resize(affCount);

    for(size_t i = 0; i < affCount; ++i)
    {
        file.Read(&m_Affines[i], sizeof(AnimAff));
    }    

    uint32_t animInsrCount;
    file.Read(&animInsrCount, sizeof(uint32_t));

    m_Instructions.resize(animInsrCount);

    for(size_t i = 0; i < animInsrCount; ++i)
    {
        file.Read(&m_Instructions[i], sizeof(AnimInstruction));
    }

    GenerateFrames();    
}

void AnimatorEditor::WriteToFile(wxFile& file)
{
    uint32_t animCount = m_AnimRanges.size();
    file.Write(&animCount, sizeof(uint32_t));

    for(const AnimRange& range : m_AnimRanges)
    {
        file.Write(&range, sizeof(AnimRange));
    }

    file.Write(&m_FrameCount, sizeof(uint32_t));
    
    for(const FrameInfo& info : m_FrameInfos)
    {
        file.Write(&info, sizeof(FrameInfo));
    }

    uint32_t attrCount = m_Attributes.size();
    file.Write(&attrCount, sizeof(uint32_t));

    for(const SpriteAttribute& attr : m_Attributes)
    {
        file.Write(&attr, sizeof(SpriteAttribute));
    }

    uint32_t tileCount = m_Tiles.size();
    file.Write(&tileCount, sizeof(uint32_t));

    for(const Graphics& tile : m_Tiles)
    {
        file.Write(tile.GetData(), tile.GetLenght());
    }

    uint32_t palCount = m_Palettes.size();
    file.Write(&palCount, sizeof(uint32_t));
    file.Seek(file.Tell()+(m_Palettes.size()*32)); //Not save palettes changes ToDo

    uint32_t affCount = m_Affines.size();
    file.Write(&affCount, sizeof(uint32_t));

    for(const AnimAff& aff : m_Affines)
    {
        file.Write(&aff, sizeof(AnimAff));
    }

    uint32_t instrCount = m_Instructions.size();
    file.Write(&instrCount, sizeof(uint32_t));
    
    for(const AnimInstruction& inst : m_Instructions)
    {
        file.Write(&inst, sizeof(AnimInstruction));
    }    
}   

void AnimatorEditor::GenerateFrames()
{
    m_Frames.clear();
    m_Frames.reserve(m_FrameCount);

    for(FrameInfo& info : m_FrameInfos)
    {        
        for(size_t oam_i = 0; oam_i < info.oam.length; ++oam_i)
        {
            SpriteAttribute& oam = m_Attributes[oam_i+info.oam.start];
            auto size = sprite_sizes[oam.GetSize() + (oam.GetShape()*4)];
            auto tile_size = std::pair<uint16_t, uint16_t>(size.first / 8, size.second / 8);
            auto tile_count = tile_size.first*tile_size.second;

            AnimRange tile_range;
            tile_range.start = info.tile.start + oam.GetTile();
            tile_range.length = tile_count;

            Graphics graphics;
            graphics.Create(size.first, size.second);

            uint32_t tile_x = 0;
            uint32_t tile_y = 0;

            for(size_t tile_i = 0; tile_i < tile_range.length; ++tile_i)
            {
                graphics.BlitTile(m_Tiles[tile_i+tile_range.start], tile_x, tile_y);

                tile_x++;

                if(tile_x == tile_size.first)
                {
                    tile_x = 0;
                    tile_y++;
                }
            }

            m_Frames.push_back(graphics);
        }
    }
}