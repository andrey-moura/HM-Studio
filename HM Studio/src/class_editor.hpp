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

	size_t m_Number;
	size_t m_Index;

	size_t m_Count;
public:
	bool NextFile();
	bool PreviousFile();

	virtual bool Open(uint32_t index) { return false; }
	virtual void UpdateText() {};
	virtual void SaveFile() {};
	virtual void Insert() {};

	virtual size_t GetIndex() { return m_Index; }
	virtual size_t GetCount() { return m_Count; }
public:
	RomFile& GetRom(const bool& translated);
public:		
	virtual void Dump(bool translated) { };
};

