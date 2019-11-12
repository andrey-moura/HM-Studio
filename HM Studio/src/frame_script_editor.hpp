#pragma once

#include "wx/wx.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"
#include <wx/stc/stc.h>
#include <wx/clipbrd.h>

#include "class_rom.hpp"
#include "class_script.hpp"

#include "Studio.h"

#include "class_util_wx_file.hpp"
#include "class_util_string.hpp"
#include "class_table.hpp"

#include "frame_search_script.hpp"

class cScriptEditor : public wxFrame
{
public:
	cScriptEditor(id i);
	~cScriptEditor();	

	void SetupRom();
//Events
private:

	wxDECLARE_EVENT_TABLE();

	void tScriptTranslatedOnStyleNeeded(wxStyledTextEvent& event);
	void tScriptOriginalOnStyleNeeded(wxStyledTextEvent& event);
	void tScritpTranslatedOnModified(wxStyledTextEvent& event);
	void tScriptTranslatedOnUi(wxStyledTextEvent& event);
	void OnInputKeyDown(wxKeyEvent& event);
	void OnPrevScriptClick(wxCommandEvent& event);
	void OnProxScriptClick(wxCommandEvent& event);
	void OnSaveTextClick(wxCommandEvent& event);
	void OnPrevTextClick(wxCommandEvent& event);
	void OnProxTextClick(wxCommandEvent& event);
	void OnHorizontalModeCheck(wxCommandEvent& event);
	void OnMenuGetTextFromScriptFile(wxCommandEvent& event);
	void OnScriptNavOffsetMouseDown(wxMouseEvent& event);
	void OnScriptNavOffsetMouseUP(wxMouseEvent& event);
	void OnStringNavInputKeyDown(wxKeyEvent& event);	
	void OnSaveScriptClick(wxCommandEvent& event);	
	void OnInsertScriptClick(wxCommandEvent& event);
	void OnExportScript(wxCommandEvent& event);
	void EVT_MENU_FindText(wxCommandEvent& event);
	void EVT_MENU_FindNextText(wxCommandEvent& event);
	void EVT_MENU_RestoreString(wxCommandEvent& event);
//others
private:
	wxClipboard clip_board;

	//bool m_stringChange = false;
	//bool m_stringSaved = false;
	//bool m_stringExitWithoutSave = false;

//Text save
private:
	std::string m_textSave;
	int m_indexBackup = 0;
	bool m_stringSaved = false;
	bool m_stringBackup = false;
	bool m_stringChange = false;
	void BackupText();
	void RestoreText();

//Script manipulation
private:
	void OpenScript();
	void SaveScript();
	void ExportScript();
	void UpdateScript();
	void CheckAndGoScript(int index);
	void GetTextFromScriptFile();
	void FindText();

	Script scriptOriginal;
	Script scriptTranslated;

	int scriptNum = 0;

//Rom manipulation
private:
	Rom romOriginal;
	Rom romTranslated;
//Forms

//Text manipulation
private:
	void PrevText();
	void ProxText();	
	void SaveText();
	void UpdateText();

	int index = 0;

	std::vector<std::string> textOriginal;
	std::vector<std::string> textTranslated;

	std::vector<size_t> m_indexesString;
	size_t m_curIndexString = 0;

	std::string m_lineEnding;
	std::string m_lineLineEnding;

//UI
private:
	void CreateGUIControls();
	void SetEditorVertical();
	void SetEditorHorizontal();

	//wxToolBar* toolBar = nullptr;
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

	wxBoxSizer* global_sizer = nullptr;
	wxBoxSizer* panel_left_sizer = nullptr;	
	wxBoxSizer* horizontal_sizer = nullptr;
	
	//Lateral bar, script navigation
	wxTextCtrl* script_nav_input = nullptr;
	wxButton* script_nav_prev = nullptr;
	wxButton* script_nav_prox = nullptr;
	wxStaticText* script_nav_offset_0x = nullptr;
	wxStaticText* script_nav_offset = nullptr;
	wxBoxSizer* script_nav_offset_sizer = nullptr;
	wxButton* script_nav_insert = nullptr;
	wxBoxSizer* script_nav_buttons_sizer = nullptr;
	wxStaticBox* script_nav_box = nullptr;
	wxStaticBoxSizer* script_nav_sizer = nullptr;

	//Lateral bar, string navigation
	wxStaticText* string_nav_index_is = nullptr;
	wxStaticText* string_nav_index = nullptr;
	wxCheckBox* string_nav_code = nullptr;
	wxCheckBox* string_nav_later = nullptr;
	wxTextCtrl* string_nav_input = nullptr;
	wxStaticBox* string_nav_box = nullptr;
	wxStaticBoxSizer* string_nav_sizer = nullptr;
	wxBoxSizer* string_nav_index_sizer = nullptr;

	//Lateral bar, options
	wxCheckBox* options_nav_export = nullptr;
	wxCheckBox* options_nav_scroll = nullptr;
	wxCheckBox* options_nav_horizontal = nullptr;
	wxStaticBox* options_nav_box = nullptr;
	wxStaticBoxSizer* options_nav_sizer = nullptr;	

	//Editor
	wxStyledTextCtrl* tScriptTranslated = nullptr;
	wxButton* editor_save_text = nullptr;
	wxButton* editor_prev_text = nullptr;
	wxButton* editor_next_text = nullptr;	
	wxBoxSizer* editor_sizer = nullptr;
	wxBoxSizer* editor_buttons_sizer = nullptr;
	wxStyledTextCtrl* tScriptOriginal = nullptr;

	//Log
	wxStaticText* log_text = nullptr;

	enum ID {
		ID_SCRIPT_NAV_INPUT = 10001,
		ID_SCRIPT_NAV_PREV,
		ID_SCRIPT_NAV_PROX,
		ID_MENU_STRING_SAVE,
		ID_MENU_STRING_PREV,
		ID_MENU_STRING_PROX,
		ID_MENU_STRING_RESTORE,
		ID_MENU_EXPORT_SCRIPT,
		ID_MENU_FIND_NEXT,		
	};
};