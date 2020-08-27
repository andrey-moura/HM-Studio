#pragma once

#include <map>

#include <wx/splitter.h>
#include <wx/xml/xml.h>
#include <wx/sstream.h>

#include "class_editor.hpp"
#include "frame_editor.hpp"
#include "class_stc.hpp"
#include "class_rom_file.hpp"

struct RomString
{
public:
	RomString() = default;

	RomString(const wxString& str)
		: string(str) {}

	RomString(const char* str)
		: string(str) {}

	RomString(const char* str, const size_t& size)
		: string(str, size) {}

	wxString string;
	std::vector<uint32_t> references;

	void push_reference(const uint32_t& offset) { references.push_back(offset); }

	void operator=(const wxString& str)
	{
		string = str;
	}
};

class StringEditor : public Editor
{
public:
	StringEditor(const id& i);
private:
	std::vector<RomString> m_Strings;
	std::vector<RomString> m_Original;
	std::map<size_t, uint32_t> m_Files;	
	wxString m_PathFormat;	
public:
	const RomString& GetCurrentText();
	const RomString& GetCurrentOriginal();
	const std::vector<RomString>& GetStrings() const { return m_Strings; }
	const std::vector<RomString>& GetOriginal() const { return m_Original; }
	std::vector<RomString>& GetStrings() { return m_Strings; }
	void Open(uint32_t start, uint32_t size);
	bool LoadFile(const wxString& path, std::vector<RomString>& strings, uint32_t& blocklength, uint32_t& blockstart);
	bool Open(const wxString& path);
	bool Save(const wxString& str);
	bool InsertFile(size_t index, uint32_t* outsize = nullptr, uint32_t* outstart = nullptr);	
	void OpenOriginal();
public:
	virtual bool Open(uint32_t number) override;
	virtual void SaveFile() override;
	virtual void InsertFile() override;	
	virtual void InsertAll() override;
	virtual bool GetTextFromPath(const std::string& file) override;	
};

class StringEditorFrame : public EditorFrame
{
public:
	StringEditorFrame(const id& i);
private:
	virtual void OnGoFile() override;
	virtual void OnSaveString() override;
	virtual void UpdateText() override;	
	virtual void OnResumeBackup(const wxString& bakcup) override;
private:
	void GoFile(const wxString& offset);
private:
	void OnUpdateFilesClick(wxCommandEvent& event);
private:
	STC* m_pTextEditor = nullptr;
	STC* m_pTextOriginal = nullptr;
private:
	void CreateGUIControls();
};

