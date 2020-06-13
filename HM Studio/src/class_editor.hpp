#pragma once

#include "class_rom_file.hpp"

class Editor 
{
public:
	Editor(const id& id);
	~Editor() = default;
protected:
	RomFile m_RomOriginal;
	RomFile m_RomTranslated;
public:
	RomFile& GetRom(const bool& translated);
public:		
	virtual void Dump(bool translated) = 0;
};

