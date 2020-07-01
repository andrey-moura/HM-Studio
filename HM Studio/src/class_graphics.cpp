#include "class_graphics.hpp"

Graphics::Graphics(uint32_t width, uint32_t height, uint8_t bpp, bool reversed, bool planar) :
	m_Bpp(bpp), m_Width(width), m_Height(height), m_Reversed(reversed), m_Planar(planar)
{

}

Graphics::~Graphics()
{
	delete[] m_8bppData;
}

void Graphics::LoadFromRom(RomFile& file)
{
	uint8_t bits = 8 / m_Bpp;
	uint8_t mask = (1 << m_Bpp) - 1;
	uint32_t newSize = m_Width * m_Height;
	uint32_t rawSize = newSize / bits;
	uint32_t palSize = (1 << m_Bpp) * 2;	

	uint8_t* linear8bpp = new uint8_t[newSize];

	file.Seek(m_ImgOffset);

	if (m_Bpp == 8)
	{
		file.Read(linear8bpp, rawSize);
	}
	else
	{
		uint8_t* raw = new uint8_t[rawSize];
		file.Read(raw, rawSize);

		uint32_t curByte = 0;

		for (size_t i = 0; i < rawSize; ++i)
		{
			uint8_t byte = raw[i];

			if (!m_Reversed)
				byte = Moon::BitConverter::ReverseBits(byte);

			for (size_t bit = 0; bit < bits; ++bit)
			{				
				uint8_t shiftCount = (m_Bpp * bit);

				linear8bpp[curByte] = (byte & (mask << shiftCount)) >> shiftCount;			
				++curByte;
			}
		}

		delete[] raw;
	}

	if (GetAllocation())
	{
		if (m_8bppData != nullptr)
			delete m_8bppData;

		m_8bppData = new uint8_t[newSize];
	}	

	OrderTiles(linear8bpp, m_8bppData);

	delete[] linear8bpp;

	uint8_t* rawPal = new uint8_t[palSize];
	file.Seek(m_PalOffset);
	file.Read(rawPal, palSize);

	m_Palette.DecodeColors((uint16_t*)rawPal, m_Bpp);

	ClearAllocation();
}

void Graphics::SaveToRom(RomFile& file)
{
	uint8_t bits = 8 / m_Bpp;
	uint8_t mask = (1 << m_Bpp) - 1;
	uint32_t newSize = m_Width * m_Height;
	uint32_t rawSize = newSize / bits;
	uint32_t palSize = (1 << m_Bpp) * 2;

	uint8_t* linear8bpp = new uint8_t[newSize];

	OrderTiles(m_8bppData, linear8bpp);

	uint8_t* raw = new uint8_t[rawSize];

	uint32_t curByte = 0;

	for (size_t i = 0; i < rawSize; ++i)
	{		
		raw[i] = 0;

		for (size_t bit = 0; bit < bits; ++bit)
		{
			uint8_t byte = linear8bpp[curByte];

			uint8_t shiftCount = (m_Bpp * bit);

			raw[i] |= (byte << shiftCount);
			++curByte;
		}
	}

	File::WriteAllBytes("8bpp", raw, rawSize);
}

void Graphics::OrderTiles(uint8_t* src, uint8_t* dst)
{	
	size_t xCount = m_Width / 8;

	for (size_t tileY = 0; tileY < m_Height / 8; ++tileY)
	{
		for (size_t y = 0; y < 8; ++y)
		{
			for (size_t tileX = 0; tileX < xCount; ++tileX)
			{
				memcpy(dst, src + (tileX * 64) + (y * 8) + (tileY * (64 * xCount)), 8);
				dst += 8;
			}
		}
	}
}

Palette::Palette(uint16_t* colors, uint8_t bpp)
{
	DecodeColors(colors, bpp);
}

void Palette::SetCount(uint8_t count)
{
	if (count != m_Count)
	{
		delete[] m_Colors;
		m_Colors = new Color[count];
		m_Count = count;
	}
}

void Palette::DecodeColors(uint16_t* colors, uint8_t bpp)
{
	char palCount = 1 << bpp;

	SetCount(palCount);	

	m_Colors = new Color[palCount];

	for (size_t index = 0; index < palCount; ++index)
	{
		SetColor(index, (colors[index] & 31) << 3, ((colors[index] >> 5) & 31) << 3, ((colors[index] >> 10) & 31) << 3);
	}

	delete[] colors;
}

size_t Palette::FindRGB(uint32_t rgb) const
{			
	Color* color = (Color*)(&rgb);

	for (size_t i = 0; i < m_Count; ++i)
	{
		if (m_Colors[i] == *color)
		{
			return i;
		}
	}

	return std::string::npos;
}

void Graphics::SetWidth(uint32_t width)
{
	VerifyAndSet(width, m_Width);
}

void Graphics::SetHeight(uint32_t height)
{
	VerifyAndSet(height, m_Height);
}

void Graphics::SetImgOffset(uint32_t offset)
{
	m_ImgOffset = offset;
}

void Graphics::SetPalOffset(uint32_t offset)
{
	m_PalOffset = offset;
}

void Graphics::SetBpp(uint8_t bpp)
{
	VerifyAndSet(bpp, m_Bpp);
}

void Graphics::SetPlanar(bool planar)
{
	m_Planar = planar;
}

void Graphics::SetReversed(bool reversed)
{
	m_Reversed = reversed;
}

template <class T>
inline void Graphics::VerifyAndSet(const T& verify, T& set)
{
	if (verify != set)
	{
		SetAllocation();
		set = verify;
	}
}