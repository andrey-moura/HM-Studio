#include "class_graphics.hpp"

Graphics::Graphics(uint8_t* bytes, const uint8_t bitsPerPixel, const uint32_t width, const uint32_t height, const bool reversed, const bool planar)
	: m_bpp(bitsPerPixel), m_width(width), m_height(height), m_reversed(reversed), m_planar(planar), m_data(bytes), m_pal()
{	

}

Graphics::~Graphics()
{

}

Tile* Graphics::Decode()
{	
	uint8_t bits = 8 / m_bpp;
	uint8_t mask = (1 << m_bpp) - 1;

	uint32_t totalBytes = m_width * m_height;
	uint32_t totalEncodedBytes = totalBytes / bits;
	uint32_t totalTileBytes = 64;
	uint32_t tileCount = totalBytes / totalTileBytes;

	Color* bpp24 = new Color[totalBytes * 3];

	int index = 0;

	for (size_t i = 0; i < totalEncodedBytes; ++i)
	{
		uint8_t byte = m_data[i];

		for (int z = 0; z < bits; ++z)
		{
			bpp24[index] = m_pal[(byte & mask) + (m_palIndex * 16)];
			byte = byte >> m_bpp;
			++index;
		}
	}	

	Tile* tiles = (Tile*)bpp24;

	return tiles;
}

void Graphics::DecodePalette(uint8_t* bytes)
{
	uint8_t mask = 31;

	uint16_t* bytes16 = (uint16_t*)bytes;

	for (int i = 0; i < 256; ++i)
	{
		uint16_t byte = bytes16[i];

		m_pal.SetColor(i, (byte & mask) << 3, ((byte >> 5) & mask) << 3, ((byte >> 10) & mask) << 3);
	}
}

wxImage Graphics::ToImage()
{
	wxBitmap background = wxBitmap(m_width, m_height, 24);

	Tile* offset = Decode();

	{
		wxMemoryDC dc(background);

		wxPoint point(0, 0);	

		dc.DrawBitmap(wxBitmap(wxImage(8, 8, (uint8_t*)offset, true)), 0, 0);

		for (size_t i = 0; i < (m_width * m_height) / 64; ++i)
		{			
			++offset;

			++point.x;

			if (point.x >= (m_width / 8))
			{
				point.y++;
				point.x = 0;
			}			

			dc.DrawBitmap(wxBitmap(wxImage(8, 8, (uint8_t*)offset, true)), point.x * 8, point.y * 8);
		}
		      
		dc.SelectObject(wxNullBitmap);
	}
	
	offset -= (m_width * m_height / 64);
	delete[] offset;	

	return background.ConvertToImage();
}
