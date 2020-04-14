#pragma once

#include "class_rom_file.hpp"

struct Color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	Color(const uint8_t& r, const uint8_t& g, const uint8_t& b)
	{
		red = r;
		green = g;
		blue = b;
	}

	Color()
	{
		red = 00;
		green = 00;
		blue = 00;
	}

	uint32_t GetRGB() const { return (red << 16) | (green << 8) | blue; }
	uint32_t GetBGR() const { return (blue << 16) |(green << 8) | red; }
};

struct Palette {
	Color* m_Colors = nullptr;

	Palette(uint16_t* colors, uint8_t bpp);
	Palette() = default;

	~Palette() { delete[] m_Colors; }

	void DecodeColors(uint16_t* colors, uint8_t bpp);

	void SetColor(const int& index, const uint8_t& red, const uint8_t& green, const uint8_t& blue)
	{
		m_Colors[index] = Color(red, green, blue);
	}

	const Color& operator[](const int& index) const
	{
		return m_Colors[index];
	}
};

class Graphics
{
public:
	Graphics(uint8_t* bytes, const uint8_t bpp, const uint32_t width, const uint32_t height, const bool reversed = true, const bool planar = false);
	Graphics() = default;
	~Graphics();
private:
	uint32_t m_Width;
	uint32_t m_Height;
	Palette m_Pal;	
	uint32_t m_ImgOffset;
	uint32_t m_PalOffset;

	Palette m_Palette;
	uint8_t* m_8bppData = nullptr;
	uint8_t m_Bpp;
	bool m_Reversed = false;
	bool m_Planar = false;
	bool m_New = true;
/*
	Bitmaps should load very fast, so we're going to not allocate new data
	if not needed. See below.
*/
private:
	bool GetAllocation() { return m_New; }
	void SetAllocation() { m_New = true; }
	void ClearAllocation() { m_New = false; }

	template <class T>
	inline void VerifyAndSet(const T& verify, T& set);
public:
	size_t GetWidth() const { return m_Width; }
	size_t GetHeight() const { return m_Height; }
	uint32_t GetImgOffset() const { return m_ImgOffset; }
	uint32_t GetPalOffset() const { return m_PalOffset; }
	uint8_t GetBpp() const { return m_Bpp; }
	const Palette& GetPalette() const { return m_Palette; }
	uint8_t* GetData() { return m_8bppData; }
	bool IsPlanar() const { return m_Planar; }
	bool IsReversed() const { return m_Reversed; }

	void SetWidth(uint32_t width);
	void SetHeight(uint32_t height);
	void SetImgOffset(uint32_t offset);
	void SetPalOffset(uint32_t offset);
	void SetBpp(uint8_t bpp);
	void SetPlanar(bool planar);
	void SetReversed(bool reversed);

	void LoadFromRom(RomFile& file);
};
