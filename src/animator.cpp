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

uint32_t Animator::GetLength() const
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

void Animator::LoadFromFile(wxFile& file)
{
    uint32_t animCount;
    file.Read(&animCount, sizeof(uint32_t));    
    
    m_AnimRanges.resize(animCount);

    for(size_t i = 0; i < animCount; ++i)
    {        
        file.Read(&m_AnimRanges[i], sizeof(AnimRange));        
    }
    
    uint32_t frameCount;
    file.Read(&frameCount, sizeof(uint32_t));
    
    m_FrameInfos.resize(frameCount);

    for(size_t i = 0; i < frameCount; ++i)
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

    m_Tiles.resize(tileCount);

    for(size_t i = 0; i < tileCount; ++i)
    {        
        m_Tiles[i].Create(8, 8);
        file.Read(m_Tiles[i].GetData(), m_Tiles[i].GetLenght());
    }

    uint32_t palette_count;
    file.Read(&palette_count, sizeof(uint32_t));

    m_Palettes.resize(palette_count);

    for(size_t i = 0; i < palette_count; ++i)
    {
        std::vector<uint16_t> raw_palette;
        raw_palette.resize(16);
        
        file.Read(raw_palette.data(), 16*2);                

        m_Palettes[i] = gba_to_palette(raw_palette);
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

    m_Frames2.reserve(frameCount);    
    
    for(size_t frame_index = 0; frame_index < m_FrameInfos.size(); ++frame_index)    
    {
        std::vector<std::pair<int, int>> positions = GetPositions(frame_index);

        int max_x = 0;
        int max_y = 0;

        //the max_x and max_y position. aka width and height
        for (size_t oam_index = 0; oam_index < m_FrameInfos[frame_index].oam.length; ++oam_index)
        {
            SpriteAttribute& oam = m_Attributes[oam_index + m_FrameInfos[frame_index].oam.start];

            auto size = sprite_sizes[oam.size + (oam.shape * 4)];

            if (size.first + positions[oam_index].first > max_x)
            {
                max_x = size.first + positions[oam_index].first;
            }

            if (size.second + positions[oam_index].second > max_y)
            {
                max_y = size.second + positions[oam_index].second;
            }
        }        

        Frame frame;
        frame.x = 0;
        frame.y = 0;

        frame.w = max_x;
        frame.h = max_y;

        frame.pieces.resize(m_FrameInfos[frame_index].oam.length);

        for (size_t oam_index = 0; oam_index < m_FrameInfos[frame_index].oam.length; ++oam_index)
        {
            SpriteAttribute& oam = m_Attributes[oam_index + m_FrameInfos[frame_index].oam.start];

            frame.pieces[oam_index].x = positions[oam_index].first;
            frame.pieces[oam_index].y = positions[oam_index].second;
            frame.pieces[oam_index].palette = m_Palettes[oam.palette+m_FrameInfos[frame_index].color.start];

            auto size = sprite_sizes[oam.size + (oam.shape * 4)];

            frame.pieces[oam_index].graphics.Create(size.first, size.second);

            AnimRange tile_range;
            tile_range.start = oam.tile + m_FrameInfos[frame_index].tile.start;
            tile_range.length = (size.first/8)*(size.second/8);

            int max_tile_x = size.second / 8;

            int x = 0;
            int y = 0;

            for(size_t tile_i = 0; tile_i < tile_range.length; ++tile_i)
            {
                frame.pieces[oam_index].graphics.BlitTile(m_Tiles[tile_i+tile_range.start], x, y);

                x++;

                if(x == max_tile_x)
                {
                    x = 0;
                    y++;
                }
            }
        }

        m_Frames2.push_back(frame);
    }

    GenerateFrames();    
}

void Animator::WriteToFile(wxFile& file)
{
    uint32_t animCount = m_AnimRanges.size();
    file.Write(&animCount, sizeof(uint32_t));

    for(const AnimRange& range : m_AnimRanges)
    {
        file.Write(&range, sizeof(AnimRange));
    }

    uint32_t frameCount = m_FrameInfos.size();
    file.Write(&frameCount, sizeof(uint32_t));
    
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

    for(size_t i = 0; i < palCount; ++i)    
    {
        std::vector<uint16_t> raw_pal = color_to_gba(m_Palettes[i]);

        file.Write(raw_pal.data(), raw_pal.size()*2);
    }

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

uint32_t Animator::FindAnimatorOffset(wxFile& file)
{
    file.Seek(file.Tell() - 4);

    uint32_t attrCount = 1;
    uint32_t buffer;

    while(1)
    {
        file.Seek(file.Tell() - sizeof(SpriteAttribute) - 4);
        file.Read(&buffer, sizeof(uint32_t));

        if(buffer == attrCount)
            break;

        ++attrCount;        
    }

    file.Seek(file.Tell()-4);

    uint32_t frameCount = 1;    

    while(1)
    {
        file.Seek(file.Tell() - sizeof(FrameInfo) - 4);
        file.Read(&buffer, sizeof(uint32_t));

        if(buffer == frameCount)
            break;

        ++frameCount;        
    }

    file.Seek(file.Tell()-4);

    uint32_t animCount = 1;

    while(1)
    {
        file.Seek(file.Tell()- sizeof(uint32_t) - 4);
        file.Read(&buffer, sizeof(uint32_t));

        if(buffer == animCount)
            break;

        ++animCount;        
    }    

    return file.Tell() - 4;
}

std::pair<uint16_t, uint16_t>* Animator::GetSizeList()
{
    return sprite_sizes;
}

Palette& Animator::GetFramePalette(size_t n)
{
    FrameInfo& info = m_FrameInfos[n];

    return m_Palettes[info.color.start + m_Attributes[info.oam.start].palette];
}

void Animator::GenerateFrame(size_t n)
{
    FrameInfo& info = GetFrameInfo(n);

    uint16_t max_x = 0;
    uint16_t max_y = 0;

    std::vector<std::pair<int, int>> positions = GetPositions(n);        

    //the max_x and max_y position. aka width and height
    for (size_t oam_i = 0; oam_i < info.oam.length; ++oam_i)
    {
        SpriteAttribute& oam = m_Attributes[oam_i + info.oam.start];

        auto size = sprite_sizes[oam.size + (oam.shape * 4)];

        if (size.first + positions[oam_i].first > max_x)
        {
            max_x = size.first + positions[oam_i].first;
        }

        if (size.second + positions[oam_i].second > max_y)
        {
            max_y = size.second + positions[oam_i].second;
        }
    }

    if (max_x != m_Frames[n].GetWidth() || max_y != m_Frames[n].GetHeight())
    {
        m_Frames[n].Create(max_x, max_y);
    }    

    //Fills the frame with transparent color. This is useful when the x and y position are negative
    memset(m_Frames[n].GetData(), 0, m_Frames[n].GetLenght());

     for (size_t oam_i = 0; oam_i < info.oam.length; ++oam_i)
    {
        SpriteAttribute& oam = m_Attributes[oam_i + info.oam.start];

        auto size = sprite_sizes[oam.size + (oam.shape * 4)];
        auto tile_size = std::pair<uint16_t, uint16_t>(size.first / 8, size.second / 8);
        uint16_t tile_count = tile_size.first * tile_size.second;

        uint16_t tile_x = positions[oam_i].first / 8;
        uint16_t tile_y = positions[oam_i].second / 8;

        uint16_t tile_x_start = tile_x;

        AnimRange tile_range;
        tile_range.start = info.tile.start + oam.tile;
        tile_range.length = tile_count;

        for (size_t tile_i = 0; tile_i < tile_range.length; ++tile_i)
        {
            m_Frames[n].BlitTile(m_Tiles[tile_i + tile_range.start], tile_x, tile_y);

            tile_x++;

            if (tile_x == tile_size.first + tile_x_start)
            {
                tile_x = positions[oam_i].first / 8;
                tile_y++;
            }
        }
    }    
}

void Animator::GenerateFrames()
{
    m_Frames.clear();
    m_Frames.resize(m_FrameInfos.size(), Graphics(0, 0, 0));

    for(size_t i = 0; i < m_FrameInfos.size(); ++i)    
    {   
        GenerateFrame(i);
    }
}

std::vector<std::pair<int, int>> Animator::GetPositions(size_t n)
{
    std::vector<std::pair<int, int>> positions;
    std::pair<int, int> minor_pos;

    FrameInfo& info = GetFrameInfo(n);    

    for (size_t oam_i = 0; oam_i < info.oam.length; ++oam_i)
    {
        SpriteAttribute& oam = m_Attributes[oam_i + info.oam.start];

        positions.push_back({ (int)oam.x, (int)oam.y });

        minor_pos.first = std::min(minor_pos.first, oam.x);
        minor_pos.second = std::min(minor_pos.second, oam.y);
    }    

    if (minor_pos.first == 0 && minor_pos.second == 0)
        return positions;

    //minor_pos will be {0, 0} and all positions are relative to it.
    for (auto& pos : positions)
    {
        pos.first -= minor_pos.first;
        pos.second -= minor_pos.second;
    }

    return positions;
}