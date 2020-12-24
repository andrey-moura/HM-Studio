#include "class_graphics.hpp"

size_t Palette::find_color(const Color& c) const
{
	const Color* colors = data();

	for(size_t i = 0; i < size(); ++i)
	{
		if(c == colors[i])
		{
			return i;
		}
	}

	return std::string::npos;
}

size_t Palette::find_nearest(const Color& c) const
{
	const Color* colors = data();
	size_t nearest;
	size_t near_diff = std::string::npos;

	for(size_t i = 0; i < size(); ++i)
	{
		int diff = 0;
		diff += std::max(c.red, colors[i].red) - std::min(c.red, colors[i].red);
		diff += std::max(c.green, colors[i].green) - std::min(c.green, colors[i].green);
		diff += std::max(c.blue, colors[i].blue) - std::min(c.blue, colors[i].blue);

		if(diff < near_diff)
		{
			near_diff = diff;
			nearest = i;
		}
	}

	if(near_diff == std::string::npos)
	{
		return std::string::npos;
	} else 
	{
		return nearest;
	}	
}

//-------------------------------------------------//

Graphics::Graphics()
	: m_Width(16), m_Height(16), m_Reversed(true), m_Planar(false)
{
	SetBpp(4);
}

Graphics::Graphics(uint32_t width, uint32_t height, uint8_t bpp, bool reversed, bool planar)
	: m_Width(width), m_Height(height), m_Reversed(reversed), m_Planar(planar)
{
	SetBpp(bpp);
}

Graphics::Graphics(const Graphics& graphics)
	: m_Width(graphics.GetWidth()), m_Height(graphics.GetHeight()), m_Reversed(graphics.IsReversed()), m_Planar(graphics.IsPlanar())
{
	*this = graphics;
}

Graphics::~Graphics()
{
	delete[] m_pRawData;
}

void Graphics::SetBpp(uint8_t bpp)
{	
	if (!bpp)
		return;

	m_Bpp = bpp;
	m_PixelsPerByte = 8/bpp;
	m_Mask = (1 << m_Bpp) - 1;
}

void Graphics::SetPlanar(bool planar)
{
	m_Planar = planar;
}

void Graphics::SetReversed(bool reversed)
{
	m_Reversed = reversed;
}

void Graphics::Create(uint32_t width, uint32_t height, uint8_t bpp, bool reversed, bool planar)
{
	m_Width = width;
	m_Height = height;
	m_Reversed = reversed;
	m_Planar = planar;

	SetBpp(bpp);

	delete[] m_pRawData;

	m_pRawData = new uint8_t[GetLenght()];
}

void Graphics::Blit(const Graphics& other, const uint32_t& x, const uint32_t& y)
{	
	//ToDo
}

void Graphics::BlitTile(const Graphics& tile, const uint32_t& tile_x, const uint32_t& tile_y)
{
	size_t n = tile_x * 64;
	n += tile_y * (64*(m_Width/8));

	BlitTile(tile, n/m_PixelsPerByte);
}

void Graphics::BlitTile(const Graphics& tile, const size_t& pos)
{
	memcpy(m_pRawData + pos, tile.m_pRawData, 64 / m_PixelsPerByte);
}

Graphics Graphics::GetTile(int tile_x, int tile_y)
{
	Graphics tile;
	tile.Create(8, 8);

	size_t n = tile_x * 64;
	n += tile_y * (64 * (m_Width / 8));

	memcpy(tile.GetData(), GetData() + (n / m_PixelsPerByte), 64 / m_PixelsPerByte);

	return tile;
}

uint8_t Graphics::GetPixel(size_t n) const
{		
	return GetPixel(n % m_Width, n / m_Width);
}

uint8_t Graphics::GetPixel(size_t x, size_t y) const
{
	size_t n;	

	if(x < 8 && y < 8)
	{
		n = x+(y*8);
	} else 
	{
		size_t tile_x = x / 8;
		size_t tile_y = y / 8;
		size_t tile_index = tile_x  + (tile_y*(m_Width/8));		

		n = tile_x * 64;
		n += x-(tile_x*8);
		n += tile_y * (64*(m_Width/8));
		n += (y-(tile_y*8)) * 8;
	}

	uint8_t b = m_pRawData[n/m_PixelsPerByte];
	uint8_t s = (n%m_PixelsPerByte) * m_Bpp;

	return (b & (m_Mask<<s)) >> s;
}

void Graphics::SetPixel(size_t n, uint8_t p)
{
	SetPixel(n % m_Width, n / m_Width, p);
}

void Graphics::SetPixel(size_t x, size_t y, uint8_t p)
{
	size_t n;	

	if(x < 8 && y < 8)
	{
		n = x+(y*8);
	} else 
	{
		size_t tile_x = x / 8;
		size_t tile_y = y / 8;
		size_t tile_index = tile_x  + (tile_y*(m_Width/8));		

		n = tile_x * 64;
		n += x-(tile_x*8);
		n += tile_y * (64*(m_Width/8));
		n += (y-(tile_y*8)) * 8;
	}

	uint8_t b = m_pRawData[n/m_PixelsPerByte];
	uint8_t s = (n%m_PixelsPerByte) * m_Bpp;

	b = (b & (~(m_Mask<<s))) | p<<s;
	m_pRawData[n/m_PixelsPerByte] = b;	
}

bool Graphics::operator==(const Graphics& other) const
{
	if(GetWidth() != other.GetWidth())		
		return false;

	if(GetHeight() != other.GetHeight())
		return false;

	if(GetBpp() == other.GetBpp())
	{
		return memcmp(GetData(), other.GetData(), GetLenght()) == 0;
	} else 
	{
		//ToDo:
		//Pixel to pixel comparison
	}
}

bool Graphics::operator!=(const Graphics& other) const
{
	return !(other == *this);
}

Graphics& Graphics::operator=(const Graphics& other)
{
	m_Width = other.GetWidth();
	m_Height = other.GetHeight();
	SetPlanar(other.IsPlanar());
	SetReversed(other.IsReversed());

	SetBpp(other.GetBpp());

	if(!other.GetData())
		return *this;

	uint32_t lenght = GetLenght();

	m_pRawData = new uint8_t[lenght];

	memcpy(m_pRawData, other.GetData(), GetLenght());

	return *this;
}

Color* Graphics::ToImage24(const Graphics& graphics, const Palette& palette)
{
	const uint8_t* data = graphics.GetData();

	if(!data)
		return nullptr;

	if(graphics.GetWidth() == 0 || graphics.GetHeight() == 0)
		return nullptr;

	size_t count = graphics.GetWidth()*graphics.GetHeight();
	
	Color* colors = new Color[count];

	size_t i = 0;

	while(i < count)
	{
		colors[i] = palette[graphics.GetPixel(i)];
		++i;
	}

	return colors;
}

uint8_t* Graphics::ToImage32(const Graphics& graphics, const Palette& palette)
{
	const uint8_t* data = graphics.GetData();

	if (!data)
		return nullptr;

	if (graphics.GetWidth() == 0 || graphics.GetHeight() == 0)
		return nullptr;

	size_t count = graphics.GetWidth() * graphics.GetHeight();

	uint8_t* colors = new uint8_t[count * 4];

	size_t it = 0;
	
	for(size_t color_it = 0; color_it < count; ++color_it)
	{
		Color c = palette[graphics.GetPixel(color_it)];
		colors[it++] = c.red;
		colors[it++] = c.green;
		colors[it++] = c.blue;
		colors[it++] = 255;
	}

	return colors;
}