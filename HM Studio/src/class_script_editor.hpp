#pragma once

#include <vector>
#include <string>
#include <string_view>

#include <wx/filename.h>
#include <wx/progdlg.h>
#include <wx/msgdlg.h>

#include "class_rom_file.hpp"
#include "class_script.hpp"
#include "class_file.hpp"
#include "class_editor.hpp"
#include "class_finder.hpp"
#include "namespace_math.hpp"
#include "window_find_results.hpp"

enum class ScriptFlags
{
	ERROR_OUTOFRANGE,
	ERROR_NOTEXT,
	SUCCESS,
	INSERT_LESS,
	INSERT_VERIFY,
	INSERT_LARGER,
	INSERT_FIND,
	INSERT_NONE
};

class ScriptEditor : public Editor
{
public:
	ScriptEditor(const id& id);

	~ScriptEditor() = default;
public:	
	std::string GetPath(bool translated) const;
	std::string GetPath(size_t number, bool translated) const;	

	const std::string& GetScriptDir() const { return m_ScriptDir; }
public:
	uint32_t GetOffset(bool translated, size_t number);
	uint32_t GetOffset(bool translated);
	uint32_t* GetOffsets(bool translated);
	void SetOffset(uint32_t offset);
	void SetOffsets(uint32_t* offets);
	void SetEvent(uint32_t index);
	inline uint32_t ScriptSize(uint32_t* value);
	inline uint32_t ScriptSize(const uint32_t& offset, bool translated);
	uint32_t ScriptSize(const uint32_t& offset, uint8_t* bytes);	

	void DumpAll(bool translated) override;
	ScriptFlags InsertFile(Script& script, uint32_t number);
	void InsertAll() override;
private:
	void InsertSmaller(Script& script, uint32_t offset, uint32_t oldSize);
	bool InsertLarger(const Script& script, uint32_t oldOffset, uint32_t oldSize, uint32_t newSize);
	void InsertOnly(const Script& script, uint32_t offset);
	void InsertMove(const Script& script, uint32_t oldOffset, uint32_t oldSize, uint32_t newOffset);
	bool InsertVerify(Script& script, uint32_t offset, uint32_t oldSize);
	bool InsertFind(Script& script, uint32_t oldOffset, uint32_t oldSize, uint32_t newSize);	
	void EraseScript(uint32_t offset, uint32_t size);

	inline bool IsInsideBlock(const uint32_t& offset);
	inline bool IsFreeSpace(const uint32_t& offset, const uint32_t& size);
private:
	std::string m_ScriptDir;
	std::string m_PathOrigLeft;
	std::string m_PathTransLeft;
	std::string m_PathRight;
public:
	ScriptFlags OpenScript(size_t scriptNum);
	ScriptFlags CheckAndGoScript(size_t index);
public:
	std::string GetCurOriginal();
	std::string GetCurTranslated();
	size_t GetNumber() { return m_Number; }
	bool IsOpened() { return m_Opened; }
	bool ProxText();
	bool PrevText();
	size_t GetCount() { return m_ScriptOriginal.Count(); }
	bool SaveText(const std::string& text);
	void SetText(const std::vector<std::string>& text);	
	std::vector<std::string>& GetTranlated() { return m_Translated; }
	void SaveScript();
	std::string& operator[](size_t index) { return m_Translated[index]; }	
	size_t GetIndex() { return m_Index; }
	void ReplaceInAllScripts(const std::string& find, const std::string& replace);

	FilesResults FindInScripts(std::string& search, bool useTable, bool translated) const;
	std::vector<size_t> Find(std::string& find, bool useTable, bool translated);
	void Replace(std::string& find, bool useTable, const std::string& replace);

	void UpdateScript();
	Script& GetScript() { return m_ScriptTranslated; }

	FilesResults CheckEOL();
	FilesResults CheckCode();
private:
	virtual void GetRomInfo() override;
private:
	Script m_ScriptOriginal;
	Script m_ScriptTranslated;

	std::vector<std::string> m_Original;
	std::vector<std::string> m_Translated;
	size_t m_Size;
	bool m_Opened = false;
};