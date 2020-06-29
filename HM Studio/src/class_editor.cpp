#include "class_editor.hpp"

Editor::Editor(const id& id) : m_RomOriginal(id, false), m_RomTranslated(id, true)
{
	
}

RomFile& Editor::GetRom(const bool& translated)
{
	return (translated ? m_RomTranslated : m_RomOriginal);
}

const RomFile& Editor::GetRom(const bool& translated) const
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

bool Editor::SetIndex(size_t index)
{
	if (index < m_Count)
	{
		m_Index = index;
		return true;
	}

	return false;
}