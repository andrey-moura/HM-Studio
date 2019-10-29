#pragma once

#include <string>

#include "class_rom.hpp"

enum class ITEM_TYPE {
	TOOL,
	FRIDGE,
	SHELF
};

class Item
{
public:
	Item(const std::string& name, const std::string& description);
	std::string m_name;
	std::string m_description;

public:
	static void GetItems(const Rom& rom, ITEM_TYPE type, std::vector<Item>& output);
};

