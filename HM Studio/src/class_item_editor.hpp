#pragma once

#include <vector>
#include <string>

#include "class_item.hpp"
#include "class_editor.hpp"
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
};

class ItemEditor : public Editor
{	
public:
	ItemEditor(const id& i);

	void OpenItens(uint8_t index);

	void PrevItem();
	void ProxItem();

	std::string GetName(bool translated) const;
	std::string GetDescription(bool translated) const;
	uint32_t GetImgAdress(bool translated) const;
	uint32_t GetPalAdress(bool translated) const;

	void SetName(const std::string& name);
	void SetDescription(const std::string& description);

	void Save(const std::string& name, const std::string& description);

	void GetItensInfos();
	const std::vector<ItemInfo> GetInfos() { return m_Infos; }
	void InsertItens();

	size_t GetInfoIndex() { return m_InfoIndex; }
	const ItemInfo& GetInfo() const { return m_Infos[m_InfoIndex]; }
	const std::vector<Item>& GetCurItens(bool translated) const { return translated ? m_Translated : m_Original; }
	void Dump();
	void DumpImages();
public:	
	virtual bool GetTextFromPath(const std::string& path);
private:
	void GetItens(bool translated);

	bool IsInsideBlock(uint32_t offset);
private:
	std::vector<ItemInfo> m_Infos;
	uint8_t m_InfoIndex;

	std::vector<Item> m_Original;
	std::vector<Item> m_Translated;

	uint32_t m_Adresses[5];
};

