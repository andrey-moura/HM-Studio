#include "class_item.hpp"

Item::Item(const std::string& name, const std::string& description) : m_name(name), m_description(description)
{
}

void Item::GetItems(const Rom& rom, ITEM_TYPE type, std::vector<Item>& output)
{
	int start = 0;
	int count = 0;
	int use = 0;
	
	switch (rom.Id)
	{
	case id::FoMT:
		switch (type)
		{
		case ITEM_TYPE::TOOL:
			break;
		case ITEM_TYPE::FRIDGE:
			break;
		case ITEM_TYPE::SHELF:
			break;
		default:
			break;
		}
	case id::MFoMT:
		switch (type)
		{
		case ITEM_TYPE::TOOL:
			start = 0xF305c;
			count = 0x51;
			use = 1;
			break;
		case ITEM_TYPE::FRIDGE:
			start = 0xF6228;
			count = 0xAB;
			use = 2;
			break;
		case ITEM_TYPE::SHELF:
			start = 0xf8648;
			count = 0x6A;
			use = 1;
			break;
		default:
			break;
		}
		break;
	case id::DS:
		break;
	default:
		break;
	}


}
