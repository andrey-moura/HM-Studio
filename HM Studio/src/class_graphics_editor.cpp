#include "class_graphics_editor.hpp"


GraphicsInfo::GraphicsInfo() :
	m_Width(16), m_Height(16), m_Bpp(4), m_Reversed(true), m_Planar(false), m_TileWidth(8), m_TileHeight(8)
{

}

GraphicsInfo::GraphicsInfo(uint32_t imageAdress, uint32_t paletteOffset, short width, short height , char bpp, bool reversed, bool planar, short tilewidth, short tileheight)
	: m_ImageAdress(imageAdress), m_PaletteAdress(paletteOffset), m_Width(width), m_Height(height), m_Bpp(bpp), m_Reversed(reversed), m_Planar(planar), m_TileWidth(tilewidth), m_TileHeight(tileheight)
{

}

GraphicsTreeItem::GraphicsTreeItem(const std::string& name, const GraphicsInfo& info) : m_Name(name), m_Info(info)
{

}

GraphicsTreeParent::GraphicsTreeParent(const std::string& name, bool mount) : m_Name(name), m_CanMount(mount)
{

}