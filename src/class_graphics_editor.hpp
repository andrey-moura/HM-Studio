#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <wx/treebase.h>

#include "class_rom_file.hpp"

class GraphicsEditorFrame;

struct GraphicsInfo
{
	short m_Width;
	short m_Height;
	short m_TileWidth;
	short m_TileHeight;
	uint32_t m_ImageAdress;
	uint32_t m_PaletteAdress;
	void(GraphicsEditorFrame::* m_Function)(const RomFile&) = nullptr;
	char m_Bpp;
	bool m_Reversed;
	bool m_Planar;

	GraphicsInfo();
	GraphicsInfo(uint32_t imageAdress, uint32_t paletteOffset, short width = 16, short height = 16, char bpp = 4, bool reversed = true, bool planar = false, short tilewidth = 8, short tileheight = 8);
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
	GraphicsTreeParent(const std::string& name, bool mount = false);
private:
	std::string m_Name;
	bool m_CanMount = false;
	std::vector<GraphicsTreeParent> m_Childs;
public:	
	wxTreeItemId m_Id;
	bool CanMount() const { return m_CanMount; }
	std::string GetName() const { return m_Name; }
	void append(const GraphicsTreeParent& parent)
	{
		m_Childs.push_back(parent);
	}

	size_t child_size()
	{
		return m_Childs.size();
	}

	std::vector<GraphicsTreeParent>& get_childs()
	{
		return m_Childs;
	}
};

class GraphicsEditor
{
};

