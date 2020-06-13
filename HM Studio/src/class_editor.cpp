#include "class_editor.hpp"

Editor::Editor(const id& id) : m_RomOriginal(id, false), m_RomTranslated(id, true)
{
	
}

RomFile& Editor::GetRom(const bool& translated)
{
	return (translated ? m_RomTranslated : m_RomOriginal);
}