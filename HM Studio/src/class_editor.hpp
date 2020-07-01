#pragma once

#include "class_rom_file.hpp"
#include "window_find_results.hpp"
#include <wx/progdlg.h>

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

	std::string m_Type;
	std::string m_FileExt;
public:
	bool NextFile();
	bool PreviousFile();

	virtual bool Open(uint32_t number) { return false; }
	virtual void UpdateText() {};
	virtual void SaveFile() {};
	virtual void InsertAll() {};
	virtual void DumpAll(bool translated) { };
	virtual void InsertFile() {};
	virtual bool SaveText(const std::string& text) { return true; }
	virtual size_t GetIndex() { return m_Index; }
	virtual bool SetIndex(size_t index);
	virtual size_t GetCount() { return m_Count; }

	virtual bool IsOpened() { return m_Number != std::string::npos; }

	virtual std::vector<size_t> Find(const std::string& search, bool useTable, bool translated) { return std::vector<size_t>(); }
	virtual FilesResults FindInFiles(std::string& search, bool useTable, bool translated);

	virtual std::string GetPath(const size_t& number, const bool& translated) const { return ""; }
	virtual std::string GetPath(const bool& translated) const { return ""; }
public:
	RomFile& GetRom(const bool& translated);
	const RomFile& GetRom(const bool& translated) const;
};

