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
	std::map<size_t, uint32_t> m_Files;
	wxString m_PathFormat;	
public:
	const RomString& GetCurrentText();
	void Open(uint32_t start, uint32_t size);
	bool Open(const wxString& path);
	bool Save(const wxString& str);	
public:
	virtual bool Open(uint32_t number) override;
	virtual void SaveFile() override;
	virtual void InsertFile() override;
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
private:
	void GoFile(const wxString& offset);
private:
	STC* m_pTextEditor = nullptr;	
private:
	void CreateGUIControls();
};

