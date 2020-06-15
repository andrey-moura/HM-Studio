#include "class_editor.hpp"

Editor::Editor(const id& id) : m_RomOriginal(id, false), m_RomTranslated(id, true)
{
	
}

RomFile& Editor::GetRom(const bool& translated)
{
	return (translated ? m_RomTranslated : m_RomOriginal);
}

bool Editor::NextFile()
{	
	return Open(m_Number+1);
}

bool Editor::PreviousFile()
{
	return Open(m_Number - 1);
}