#pragma once

#include <vector>
#include <string>

#include "class_item.hpp"
#include "class_rom_file.hpp"
#include "namespace_math.hpp"

struct ItemInfo
{
	ItemInfo(const std::string& name, uint32_t wpi, uint8_t idIndex, uint8_t descriptionIndex);

	std::string m_Name;

	uint32_t m_StartPointers;
	uint32_t m_Count;
	uint32_t m_WPI;

	uint8_t m_IdIndex;
	uint8_t m_DescriptionIndex;

	uint32_t m_StartBlock;
	uint32_t m_BlockLenght;

	uint32_t m_Adress1;
	uint32_t m_Adress2;
	uint32_t m_Adress3;
	uint32_t m_Adress4;
	uint32_t m_Adress5;
};

class ItemEditor
{	
public:
	ItemEditor(RomFile& original, RomFile& translated);

	void OpenItens(uint8_t index);

	void PrevItem();
	void ProxItem();

	std::string GetName(bool translated) const;
	std::string GetDescription(bool translated) const;
	uint32_t GetImgAdress(bool translated) const;
	uint32_t GetPalAdress(bool translated) const;

private:
	void GetItensInfos();
	void GetItens(bool translated);

	bool IsInsideBlock(uint32_t offset);
private:
	RomFile& m_RomOriginal;
	RomFile& m_RomTranslated;

	std::vector<ItemInfo> m_Infos;
	uint8_t m_InfoIndex;

	std::vector<Item> m_Original;
	std::vector<Item> m_Translated;

	uint32_t m_Adresses[5];

	uint8_t m_Index;
};

