#pragma once

#include <string>
#include <vector>

#include <wx/file.h>

#include "class_rom_file.hpp" 
#include "class_graphics.hpp"
#include "class_util_string.hpp"

class Item
{
public:
	Item(const std::string& name, const std::string& description);
	Item() = default;
private:
	std::string m_Name;
	std::string m_Description;	
	uint32_t m_ImgAdress;
	uint32_t m_PalAdress;
public:
	void SetName(const std::string& name) { m_Name = name; }
	void SetDescription(const std::string& description) { m_Description = description; }

	std::string GetName() const { return m_Name; }
	std::string GetDescription() const { return m_Description; }

	uint32_t GetPalAdress() const { return m_PalAdress; }
	uint32_t GetImgAdress() const { return m_ImgAdress; }

	void SetPalAdress(uint32_t adress) { m_PalAdress = adress; }
	void SetImgAdress(uint32_t adress) { m_ImgAdress = adress; }
};