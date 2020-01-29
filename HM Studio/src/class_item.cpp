#include "class_item.hpp"

uint32_t Item::m_sAdress1 = 0x607de0;
uint32_t Item::m_sAdress2 = 0x61a2f0;
uint32_t Item::m_sAdress3 = 0x6085c4;
uint32_t Item::m_sAdress4 = 0x60A8E4;
uint32_t Item::m_sAdress5 = 0x617768;

Item::Item(const std::string& name, const std::string& description) : m_Name(name), m_Description(description)
{
}

ITEM_DATA Item::GetItems(Rom& rom, ITEM_TYPE type, std::vector<Item>& output)
{	
	ITEM_DATA data;

	switch (rom.Id)
	{
	case id::FoMT:		
		break;
	case id::MFoMT:
		data = GetMFoMTData(type);
		break;
	case id::DS:

		break;
	default:
		break;
	}

	output.resize(data.m_Count);

	GetItensNames(output, data, rom);
	GetItensDescription(output, data, rom);
	GetItemImageOffset(output, data, rom);

	return data;
}

void Item::GetItensNames(std::vector<Item>& itens, const ITEM_DATA& data, Rom& rom)
{
	int32_t index = 0;

	if (rom.Console == console::GBA)
	{
		index = -134217728;
	}

	for (size_t i = 0; i < itens.size(); ++i)
	{
		rom.Seek(data.m_Start + (i * data.m_BPI));

		uint32_t offset = rom.ReadInt32() + index;

		rom.Seek(offset);

		itens[i].SetName(rom.ReadString());
	}
}

void Item::GetItensDescription(std::vector<Item>& itens, const ITEM_DATA& data, Rom& rom)
{
	int32_t index = 0;

	if (rom.Console == console::GBA)
	{
		index = -134217728;
	}

	for (size_t i = 0; i < data.m_Count; ++i)
	{
		rom.Seek(data.m_Start + (i * data.m_BPI) + (data.m_DescriptionIndex * 4));

		uint32_t offset = rom.ReadInt32() + index;

		rom.Seek(offset);

		itens[i].SetDescription(rom.ReadString());
	}
}

void Item::GetItemImageOffset(std::vector<Item>& itens, const ITEM_DATA& data, Rom& rom)
{
	for (size_t i = 0; i < data.m_Count; ++i)
	{
		rom.Seek(data.m_Start + (i * data.m_BPI) + (data.m_ImageID * 4));
		uint16_t imageID = rom.ReadUInt16();		

		uint32_t offset3 = (rom.ReadUInt16((rom.ReadUInt16(((imageID << 2) + m_sAdress1) + 2) << 2) + m_sAdress2) << 4) + m_sAdress3; //It's crazy, isn't? There is more!
		uint32_t image = (rom.ReadUInt16(offset3 + 6) << 5) + m_sAdress4; //O.o There is more yet!
		uint32_t palette = (rom.ReadUInt16(offset3 + 0x0a) << 5) + m_sAdress5; //Phew...;		

		itens[i].SetImage(image, palette);
	}	
}

ITEM_DATA Item::GetMFoMTData(ITEM_TYPE type)
{
	switch (type)
	{
	case ITEM_TYPE::TOOL:
		return ITEM_DATA(12, 0xF305c, 0x51, 2, 1);
		break;
	case ITEM_TYPE::FRIDGE:
		return ITEM_DATA(16, 0xF6228, 0xAB, 3, 2);
		break;
	case ITEM_TYPE::SHELF:
		return ITEM_DATA(12, 0xF8648, 0x6A, 2, 1);
		break;
	default:
		return ITEM_DATA();
		break;
	}
}