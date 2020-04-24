#include "class_item_editor.hpp"

ItemInfo::ItemInfo(const std::string& name, uint32_t wpi, uint8_t idIndex, uint8_t descriptionIndex) :
					m_Name(name), m_WPI(wpi), m_IdIndex(idIndex), m_DescriptionIndex(descriptionIndex)
{

}

ItemEditor::ItemEditor(RomFile& original, RomFile& translated) : m_RomOriginal(original), m_RomTranslated(translated)
{
	GetItensInfos();
}

void ItemEditor::PrevItem()
{
	if (m_Index == 0)
	{
		m_Index = m_Original.size() - 1;
	}

	else
		m_Index--;
}

void ItemEditor::ProxItem()
{
	m_Index++;

	if (m_Index == m_Original.size())
		m_Index = 0;
}

std::string ItemEditor::GetName(bool translated) const
{
	std::vector<Item> itens = translated ? m_Translated : m_Original;

	return itens[m_Index].GetName();
}

std::string ItemEditor::GetDescription(bool translated) const
{
	std::vector<Item> itens = translated ? m_Translated : m_Original;

	return itens[m_Index].GetDescription();
}

uint32_t ItemEditor::GetImgAdress(bool translated) const
{
	std::vector<Item> itens = translated ? m_Translated : m_Original;

	return itens[m_Index].GetImgAdress();
}

uint32_t ItemEditor::GetPalAdress(bool translated) const
{
	std::vector<Item> itens = translated ? m_Translated : m_Original;

	return itens[m_Index].GetPalAdress();
}

void ItemEditor::OpenItens(uint8_t index)
{
	m_Index = 0;
	m_InfoIndex = index;

	GetItens(true);
	GetItens(false);
}

void ItemEditor::GetItens(bool translated)
{
	uint8_t bpi = m_Infos[m_InfoIndex].m_WPI * 4;

	char* textBlock = new char[m_Infos[m_InfoIndex].m_BlockLenght];

	RomFile& rom = translated ? m_RomTranslated : m_RomOriginal;

	rom.Seek(m_Infos[m_InfoIndex].m_StartBlock);
	rom.Read(textBlock, m_Infos[m_InfoIndex].m_BlockLenght);

	uint32_t* dataBlock = new uint32_t[m_Infos[m_InfoIndex].m_Count * bpi];

	rom.Seek(m_Infos[m_InfoIndex].m_StartPointers);
	rom.Read(dataBlock, bpi * m_Infos[m_InfoIndex].m_Count);

	std::vector<Item>& itens = translated ? m_Translated : m_Original;
	itens.resize(m_Infos[m_InfoIndex].m_Count);

	for (size_t i = 0; i < m_Infos[m_InfoIndex].m_Count; ++i)
	{	
		uint32_t* curData = dataBlock + (i * m_Infos[m_InfoIndex].m_WPI);

		uint32_t nameOffset = curData[0] & 0x00FFFFFF;
		uint32_t descriptionOffset = curData[m_Infos[m_InfoIndex].m_DescriptionIndex] & 0x00FFFFFF;

		std::string name;
		std::string description;

		Item item;

		if (IsInsideBlock(nameOffset))
		{
			name = (textBlock + (nameOffset - m_Infos[m_InfoIndex].m_StartBlock));
		}
		else
		{
			rom.Seek(nameOffset);
			name = rom.ReadString();
		}
		if (IsInsideBlock(descriptionOffset))
		{
			description = (textBlock + (descriptionOffset - m_Infos[m_InfoIndex].m_StartBlock));
		}
		else
		{
			rom.Seek(descriptionOffset);
			description = rom.ReadString();
		}

		itens[i].SetName(name);
		itens[i].SetDescription(description);

		uint16_t id = *(uint16_t*)(curData + m_Infos[m_InfoIndex].m_IdIndex);

		uint32_t offset3 = (rom.ReadUInt16((rom.ReadUInt16(((id << 2) + m_Adresses[0]) + 2) << 2) + m_Adresses[1]) << 4) + m_Adresses[2]; //It's crazy, isn't? There are more!
		uint32_t image = (rom.ReadUInt16(offset3 + 6) << 5) + m_Adresses[3]; //O.o There is more yet!
		uint32_t palette = (rom.ReadUInt16(offset3 + 0x0a) << 5) + m_Adresses[4]; //Phew...

		itens[i].SetImgAdress(image);
		itens[i].SetPalAdress(palette);
	}

	delete[] textBlock;
	delete[] dataBlock;
}

bool ItemEditor::IsInsideBlock(uint32_t offset)
{
	return MathUtil::IsInsideBlock(offset, m_Infos[m_InfoIndex].m_StartBlock, m_Infos[m_InfoIndex].m_BlockLenght);
}

void ItemEditor::GetItensInfos()
{
	if (m_RomOriginal.Console == console::GBA)
	{
		m_Infos.push_back(ItemInfo("Tools",  3, 1, 2));
		m_Infos.push_back(ItemInfo("Fridge", 4, 2, 3));
		m_Infos.push_back(ItemInfo("Shelf",  3, 1, 2));

		if (m_RomOriginal.Id == id::MFoMT)
		{
			m_Infos[0].m_StartPointers = 0xF305c;
			m_Infos[0].m_Count = 0x51;			
			m_Infos[0].m_StartBlock = 0xF1BB4;
			m_Infos[0].m_BlockLenght = 0x149E;

			m_Infos[1].m_StartPointers = 0xF6228;
			m_Infos[1].m_Count = 0xAB;
			m_Infos[1].m_StartBlock = 0xF3428;
			m_Infos[1].m_BlockLenght = 0x2DFC;

			m_Infos[2].m_StartPointers = 0xF8648;
			m_Infos[2].m_Count = 0x6A;			
			m_Infos[2].m_StartBlock = 0xF6CD8;
			m_Infos[2].m_BlockLenght = 0x196C;

			m_Adresses[0] = 0x607de0;
			m_Adresses[1] = 0x61a2f0;
			m_Adresses[2] = 0x6085c4;
			m_Adresses[3] = 0x60A8E4;
			m_Adresses[4] = 0x617768;
		}

		else if (m_RomOriginal.Id == id::FoMT)
		{
			m_Infos[0].m_StartPointers = 0xEAB0C;
			m_Infos[0].m_Count = 0x51;
			m_Infos[0].m_StartBlock = 0xE9668;
			m_Infos[0].m_BlockLenght = 0x14A0;

			m_Infos[1].m_StartPointers = 0xEDCD8;
			m_Infos[1].m_Count = 0xAB;
			m_Infos[1].m_StartBlock = 0xEAED8;
			m_Infos[1].m_BlockLenght = 0x2DFC;

			m_Infos[2].m_StartPointers = 0xEFED4;
			m_Infos[2].m_Count = 0x5F;
			m_Infos[2].m_StartBlock = 0xEE788;
			m_Infos[2].m_BlockLenght = 0x1748;

			m_Adresses[0] = 0x6678a4;
			m_Adresses[1] = 0x679898;
			m_Adresses[2] = 0x66805c;
			m_Adresses[3] = 0x66a2cc;
			m_Adresses[4] = 0x676dd0;
		}
	}
}