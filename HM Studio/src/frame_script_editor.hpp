#pragma once

#include <sstream>

#include <wx/frame.h>
#include <wx/clipbrd.h>
#include <wx/txtstrm.h>
#include <wx/textbuf.h>
#include <wx/string.h>

#include "frame_search_script.hpp"
#include "dialog_script_text.hpp"

#include "class_rom.hpp"
#include "class_script.hpp"
#include "Studio.h"
#include "class_stc.hpp"

#include "class_file.hpp"
#include "class_util_string.hpp"
#include "class_table.hpp"
#include "wxUtil/window_find_results.hpp"

class ScriptEditor
{
public:
	ScriptEditor(Rom& original, Rom& translated) : m_RomOriginal(original), m_RomTranslated(translated)
	{

	}	

	~ScriptEditor() = default;

public:
	void SetData(const std::vector<uint8_t>& original, const std::vector<uint8_t>& translated, size_t scriptNum);
	std::string GetCurOriginal();
	std::string GetCurTranslated();
	size_t GetNumber() { return m_Number; }
	bool IsOpened() { return m_Opened; }
	bool ProxText();
	bool PrevText();
	size_t GetCount() { return m_ScriptOriginal.Count(); }
	bool SaveText(const std::string& text);
	void BackupText(const std::string& text);
	std::string GetBackupText() { return m_TextBackup; }
	void SetText(const std::vector<std::string>& text);
	void ReleaseBackup();
	void SetChanged(bool changed) { m_Changed = changed; }
	void SetSaved(bool saved) { m_Saved = saved; }
	std::vector<std::string>& GetTranlated() { return m_Translated; }
	void SaveScript();
	std::string& operator[](size_t index) { return m_Translated[index]; }	
	void SetIndex(size_t index) { m_Index = index; }
	size_t GetIndex() { return m_Index; }
	void ReplaceInAllScripts(const std::string& find, const std::string& replace);

	void UpdateScript();
	Script& GetScript() { return m_ScriptTranslated; }
private:
	Script m_ScriptOriginal;
	Script m_ScriptTranslated;

	std::vector<std::string> m_Original;
	std::vector<std::string> m_Translated;

	size_t m_Index = 0;	
	size_t m_Size;
	size_t m_Number;

	Rom& m_RomOriginal;
	Rom& m_RomTranslated;

	std::string m_TextBackup;
	size_t m_IndexBackup = -1;
	bool m_Changed = false;
	bool m_Saved = false;

	bool m_Opened = false;
};

class cScriptEditor : public wxFrame
{
public:
	cScriptEditor(id i);
	~cScriptEditor();	

	void SetupRom(); 
//Events
private:	
	void tScritpTranslatedOnModified(wxStyledTextEvent& event);
	void tScriptTranslatedOnUi(wxStyledTextEvent& event);
	void OnPrevScriptClick(wxCommandEvent& event);
	void OnProxScriptClick(wxCommandEvent& event);
	void OnGoScriptClick(wxCommandEvent& event);
	void OnSaveTextClick(wxCommandEvent& event);
	void OnPrevTextClick(wxCommandEvent& event);
	void OnProxTextClick(wxCommandEvent& event);
	void OnMenuHorizontalMode(wxCommandEvent& event);
	void OnMenuGetTextFrom(wxCommandEvent& event);
	void OnSaveScriptClick(wxCommandEvent& event);	
	void OnInsertScriptClick(wxCommandEvent& event);
	void OnExportScript(wxCommandEvent& event);
	void EVT_MENU_FindText(wxCommandEvent& event);
	void EVT_MENU_FindNextText(wxCommandEvent& event);
	void EVT_MENU_RestoreString(wxCommandEvent& event);
	void OnMenuAlwaysOnTop(wxCommandEvent& event);
	void OnCheckAllCode(wxCommandEvent& event);
	void OnClosing(wxCloseEvent& event);
	void OnOpenInHexEditorClick(wxCommandEvent& event);
	void OnSTCLeftDown(wxMouseEvent& event);
	void OnResultClick(wxCommandEvent& event);	
	void OnSetTextRange(wxCommandEvent& event);
//Text Editor Globals					
	std::pair<int, int> m_IndicatorPos;	

	void UpdateStatusText(wxStyledTextCtrl* stc);	
//Text save
	void ConfigureSTC(size_t maxLine, int eol, std::vector<std::string> vars, STC* stc);
private:
	void BackupText();
	void RestoreText();

//Script manipulation
private:
	void OpenScript(size_t index);
	void SaveScript();	
	void ExportScript();	
	void UpdateScript();
	void CheckAllCode();
	void CheckAndGoScript(size_t index);
	void FindText();
public:
	void ScriptTextRange(size_t from, size_t to, size_t script);

	ScriptEditor m_Editor;

//Rom manipulation
private:
	Rom romOriginal;
	Rom romTranslated;
//Forms

//Text manipulation
private:
	void UpdateText();	

	std::string m_lineEnding;
	std::string m_lineLineEnding;
private:
	wxBitmap m_DeleteIcon;
//GUI
private:
	void CreateMyToolBar();

	void CreateGUIControls();
	wxToolBar* m_pToolBar = nullptr;
	wxImage m_ImgPrevScript;

	void SetEditorVertical();
	void SetEditorHorizontal();
	bool m_Vertical = true;

	wxMenuBar* menuBar = nullptr;
	wxStatusBar* statusBar = nullptr;

	wxMenu* menuScript = nullptr;
	wxMenu* menuString = nullptr;
	wxMenuItem* m_pMenuString_Restore = nullptr;
	wxMenu* menuExport = nullptr;
	wxMenu* menuEdit = nullptr; 
	wxMenu* menuSearch = nullptr;
	wxMenu* menuTools = nullptr;
	wxMenu* menuOptions = nullptr;

	//Editor	
	STC* tScriptTranslated = nullptr;
	wxButton* editor_save_text = nullptr;
	
	wxButton* editor_prev_text = nullptr;
	wxButton* editor_next_text = nullptr;
	wxBoxSizer* editor_sizer = nullptr;
	wxBoxSizer* editor_buttons_sizer = nullptr;
	STC* tScriptOriginal = nullptr;

	wxBoxSizer* global_sizer = nullptr;	
	FindResultsWindow* m_pFindResultsWindow = nullptr;	

	enum ID_MENU {

		TOOLS_TEXTRANGE = 11000
	};

	enum ID {
		ID_SCRIPT_NAV_INPUT = 10001,
		ID_SCRIPT_NAV_GO,
		ID_SCRIPT_NAV_PREV,
		ID_SCRIPT_NAV_PROX,
		ID_SCRIPT_NAV_SAVE,
		ID_SCRIPT_NAV_INSERT,
		ID_SCRIPT_NAV_CODE,
		ID_MENU_STRING_SAVE,
		ID_MENU_STRING_PREV,
		ID_MENU_STRING_PROX,
		ID_MENU_STRING_RESTORE,
		ID_MENU_EXPORT_SCRIPT,
		ID_MENU_FIND_NEXT,		
		ID_MENU_HVMODE,
		ID_MENU_OPENHEX_ORIGINAL,
		ID_MENU_OPENHEX_TRANSLATED,
	};
};

class DialogTextRange : public wxDialog
{
public:
	DialogTextRange(cScriptEditor* parent);
	~DialogTextRange() = default;

	cScriptEditor* m_pParent = nullptr;
//Events
private:
	void OnGoButton(wxCommandEvent& event);
private:
	void CreateGUIControls();
		
	wxTextCtrl* dialogInputFrom = nullptr;
	wxTextCtrl* dialogInputTo = nullptr;
	wxTextCtrl* dialogInputScript = nullptr;
	wxButton* 	dialogGoButton = nullptr;
};