#pragma once

#include <string>
#include <vector>

#include <wx/file.h>

#include "class_rom.hpp" 
#include "class_graphics.hpp"
#include "class_util_string.hpp"

struct ITEM_DATA
{
	ITEM_DATA(size_t bpi, size_t start, size_t count, size_t description, size_t image) :
		m_BPI(bpi), m_Start(start), m_Count(count), m_DescriptionIndex(description), m_ImageID(image)
	{

	}

	ITEM_DATA() = default;

	size_t m_BPI;
	size_t m_Start;
	size_t m_Count;

	size_t m_DescriptionIndex;
	size_t m_ImageID;	
};

enum class ITEM_TYPE {
	TOOL,
	FRIDGE,
	SHELF
};

class Item
{
public:
	Item(const std::string& name, const std::string& description);
	Item() = default;
private:
	std::string m_Name;
	std::string m_Description;	
	uint32_t m_BitmapOff;
	uint32_t m_PaletteOff;
public:
	void SetName(const std::string& name) { m_Name = name; }
	void SetDescription(const std::string& description) { m_Description = description; }
	void SetImage(uint32_t image, uint32_t palette) { m_BitmapOff = image; m_PaletteOff = palette; }

	std::string GetName() { return m_Name; }
	std::string GetDescription() { return m_Description; }
	uint32_t GetPalOffset() { return m_PaletteOff; }
	uint32_t GetImageOffset() { return m_BitmapOff; }
private:
	static ITEM_DATA GetMFoMTData(ITEM_TYPE type);
private:
	static uint32_t m_sAdress1;
	static uint32_t m_sAdress2;
	static uint32_t m_sAdress3;
	static uint32_t m_sAdress4;
	static uint32_t m_sAdress5;
public:
	static ITEM_DATA GetItems(Rom& rom, ITEM_TYPE type, std::vector<Item>& output);
	static void GetItensNames(std::vector<Item>& itens, const ITEM_DATA& data, Rom& rom);
	static void GetItensDescription(std::vector<Item>& itens, const ITEM_DATA& data, Rom& rom);
	static void GetItemImageOffset(std::vector<Item>& itens, const ITEM_DATA& data, Rom& rom);
};