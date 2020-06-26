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

	size_t m_Number = std::string::npos;
	size_t m_Index;

	size_t m_Count;
public:
	bool NextFile();
	bool PreviousFile();

	virtual bool Open(uint32_t number) { return false; }
	virtual void UpdateText() {};
	virtual void SaveFile() {};
	virtual void InsertAll() {};
	virtual void DumpAll(bool translated) { };
	virtual void InsertFile() {};
	virtual void SaveText(const std::string& text) {};
	virtual size_t GetIndex() { return m_Index; }
	virtual void SetIndex(size_t index) { m_Index = index; }
	virtual size_t GetCount() { return m_Count; }

	virtual size_t IsOpened() { return m_Number != std::string::npos; }

	virtual std::vector<size_t> Find(const std::string& search, bool useTable, bool translated) { return std::vector<size_t>(); }
public:
	RomFile& GetRom(const bool& translated);
};

