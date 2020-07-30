#pragma once

#include "class_rom_file.hpp"
#include "window_find_results.hpp"
#include <wx/progdlg.h>

struct EditorInfo
{
	uint32_t StartPointers;
	uint32_t Count;

	uint32_t StartBlock;
	uint32_t BlockLenght;
};

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
	EditorInfo m_Info;

	std::string m_PathOrigLeft;
	std::string m_PathTransLeft;
	std::string m_PathRight;
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
	virtual bool PreviousText();
	virtual bool NextText();
	virtual bool CheckIntegrity();
	virtual FilesResults CheckEOL() { return FilesResults(); }
	virtual size_t GetIndex() { return m_Index; }
	virtual size_t GetCount() { return m_Count; }
	virtual size_t GetNumber() { return m_Number; }
	virtual bool SetIndex(size_t index);	
	virtual bool IsOpened() { return m_Number != std::string::npos; }
	virtual void GetRomInfo() {};
	virtual const std::string& GetType() const { return m_Type; }
	virtual const std::string& GetExt() const { return m_FileExt; }
	virtual bool GetTextFromNumber(size_t number) { return false; }
	virtual bool GetTextFromPath(const std::string& path) { return false; }

	virtual std::vector<size_t> Find(const std::string& search, bool useTable, bool translated) { return std::vector<size_t>(); }
	virtual FilesResults FindInFiles(std::string& search, bool useTable, bool translated);

	virtual std::string GetPath(const size_t& number, const bool& translated) const;
	virtual std::string GetPath(const bool& translated) const;
public:
	RomFile& GetRom(const bool& translated);
	const RomFile& GetRom(const bool& translated) const;
};

