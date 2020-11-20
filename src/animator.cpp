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

void Animator::LoadFromRom(RomFile& rom, uint32_t offset)
{
    m_Animations.clear();
    m_Frames.clear();

    rom.Seek(offset);

    uint32_t animCount = rom.ReadUInt32();
    
    m_Animations.reserve(animCount);
    
    m_AnimRanges.reserve(animCount);

    for(size_t i = 0; i < animCount; ++i)
    {
        m_AnimRanges.push_back( { rom.ReadUInt16(), rom.ReadUInt16() } );
    }

    m_FrameCount = rom.ReadUInt32();

    m_FrameInfos.resize(m_FrameCount);    

    for(size_t i = 0; i < m_FrameCount; ++i)
    {
        rom.Read(&m_FrameInfos[i], sizeof(FrameInfo));
    }

    uint32_t oamCount = rom.ReadUInt32();

    m_Attributes.resize(oamCount);

    for(size_t i = 0; i < oamCount; ++i)
    {
        rom.Read(&m_Attributes[i], sizeof(SpriteAttribute));
    }
    
    uint32_t tileCount = rom.ReadUInt32();

    m_Tiles.reserve(tileCount);

    for(size_t i = 0; i < tileCount; ++i)
    {        
        Graphics tile;
        tile.Create(8, 8);

        rom.Read(tile.GetData(), tile.GetLenght());

        m_Tiles.push_back(tile);
    }

    uint32_t palette_count = rom.ReadUInt32();    

    m_Palettes.reserve(palette_count);

    for(size_t i = 0; i < palette_count; ++i)
    {
        uint16_t* raw_palette = new uint16_t[16];
        rom.Read(raw_palette, 32);

        m_Palettes.push_back(Palette(raw_palette, 4));        
    }    

    uint32_t affCount = rom.ReadUInt32();
    m_Affines.resize(affCount);

    for(size_t i = 0; i < affCount; ++i)
    {
        rom.Read(&m_Affines[i], sizeof(AnimAff));
    }    

    uint32_t animInsrCount = rom.ReadUInt32();

    m_Instructions.resize(animInsrCount);

    for(size_t i = 0; i < animInsrCount; ++i)
    {
        rom.Read(&m_Instructions[i], sizeof(AnimInstruction));
    }

    GenerateFrames();

    m_OldSize = rom.Tell()-offset;
}

void Animator::WriteToRom(RomFile& file, const uint32_t& offset)
{
    file.Seek(offset);

    file.WriteT<uint32_t>(m_AnimRanges.size());

    for(const AnimRange& range : m_AnimRanges)
    {
        file.WriteT(range);
    }

    file.WriteT<uint32_t>(m_FrameCount);
    
    for(const FrameInfo& info : m_FrameInfos)
    {
        file.WriteT(info);
    }

    file.WriteT<uint32_t>(m_Attributes.size());

    for(const SpriteAttribute& attr : m_Attributes)
    {
        file.WriteT(attr);
    }

    file.WriteT<uint32_t>(m_Tiles.size());

    for(const Graphics& tile : m_Tiles)
    {
        file.Write(tile.GetData(), tile.GetLenght());
    }

    file.WriteT<uint32_t>(m_Palettes.size());
    file.Seek(file.Tell()+(m_Palettes.size()*32)); //Not save palettes changes ToDo

    file.WriteT<uint32_t>(m_Affines.size());

    for(const AnimAff& aff : m_Affines)
    {
        file.WriteT(aff);
    }

    file.WriteT<uint32_t>(m_Instructions.size());
    
    for(const AnimInstruction& inst : m_Instructions)
    {
        file.WriteT(inst);
    }

    uint32_t newSize = file.Tell()-offset;

    while(newSize < m_OldSize)
    {
        file.WriteT<uint8_t>(0);
        newSize++;
    }
}   

void Animator::GenerateFrames()
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