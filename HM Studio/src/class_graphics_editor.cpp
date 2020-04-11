#include "class_graphics_editor.hpp"


GraphicsInfo::GraphicsInfo() :
	m_Width(16), m_Height(16), m_Bpp(4), m_Reversed(false), m_Planar(false)
{

}

GraphicsInfo::GraphicsInfo(uint32_t imageAdress, uint32_t paletteOffset, short width, short height , char bpp, bool reversed, bool planar)
	: m_ImageAdress(imageAdress), m_PaletteAdress(paletteOffset), m_Width(width), m_Height(height), m_Bpp(bpp), m_Reversed(reversed), m_Planar(planar)
{

}

GraphicsTreeItem::GraphicsTreeItem(const std::string& name, const GraphicsInfo& info) : m_Name(name), m_Info(info)
{

}

GraphicsTreeParent::GraphicsTreeParent(const std::string& name) : m_Name(name), std::vector<GraphicsTreeItem>()
{

}