#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "class_rom_file.hpp"

class GraphicsEditorFrame;

struct GraphicsInfo
{
	short m_Width;
	short m_Height;
	uint32_t m_ImageAdress;
	uint32_t m_PaletteAdress;
	void(GraphicsEditorFrame::* m_Function)(const RomFile&) = nullptr;
	char m_Bpp;
	bool m_Reversed;
	bool m_Planar;

	GraphicsInfo();
	GraphicsInfo(uint32_t imageAdress, uint32_t paletteOffset, short width = 16, short height = 16, char bpp = 4, bool reversed = true, bool planar = false);
};

struct GraphicsTreeItem
{
	std::string m_Name;
	GraphicsInfo m_Info;

	GraphicsTreeItem(const std::string& name, const GraphicsInfo& info);
};

class GraphicsTreeParent : public std::vector<GraphicsTreeItem>
{
public:
	GraphicsTreeParent(const std::string& name);
private:
	std::string m_Name;
public:
	std::string GetName() const { return m_Name; }
};

class GraphicsEditor
{
};

