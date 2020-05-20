#pragma once

#include <thread>

#include <wx/frame.h>
#include <wx/clipbrd.h>
#include <wx/txtstrm.h>
#include <wx/textbuf.h>
#include <wx/string.h>

#include "window_find_results.hpp"
#include "frame_search_script.hpp"
#include "dialog_script_text.hpp"

#include "class_rom_file.hpp"
#include "class_script.hpp"
#include "class_stc.hpp"
#include "class_file.hpp"
#include "class_util_string.hpp"
#include "class_script_editor.hpp"
#include "class_spell_checker.hpp"

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
	void OnProxScriptClick(wxCommandEvent& event);
	void GoScript();
	void SaveText();
	void PrevText();
	void ProxText();
	void HorizontalMode();
	void GetTextFrom();	
	void InsertScript();
	void OnSTCLeftDown(wxMouseEvent& event);
	void OnResultClick(wxCommandEvent& event);	
	void SetTextRange();

	bool ScriptMenuTools(int id);
	bool StringMenuTools(int id);
	bool OthersMenuTools(int id);

	void OnMenuClick(wxCommandEvent& event);
	void OnNavigationClick(wxCommandEvent& event);
	void OnToolBarClick(wxCommandEvent& event);
//Text Editor Globals					
	std::pair<int, int> m_IndicatorPos;	

	void UpdateStatusText(wxStyledTextCtrl* stc);	
//Text save
	void ConfigureSTC(STC* stc, const RomFile& rom);
private:
	void BackupText();
	void RestoreText();
//Rom manipulation
private:
	RomFile romOriginal;
	RomFile romTranslated;
//Script manipulation
private:
	void OpenScript(size_t index);
	void SaveScript();		
	void UpdateScript();
	void CheckAllCode();
	void CheckAndGoScript(size_t number, size_t index = 0);
	void FindText();
public:
	void ScriptTextRange(size_t from, size_t to, size_t script);
private:
	ScriptEditor m_Editor;	
//Forms

//Text manipulation
private:
	void UpdateText();		
	std::vector<size_t> m_FindPos;
	size_t m_FindIndex;	
	wxFileName m_ScriptDic;
	size_t m_DicIndex = std::string::npos;
	std::string m_BackupFile;

	void UpdateScriptDic();
//GUI
private:
	void CreateMyToolBar();

	void CreateGUIControls();
	wxToolBar* m_pToolBar = nullptr;
	wxImage m_ImgPrevScript;

	void SetEditorVertical();
	void SetEditorHorizontal();
	void OpenInHexEditor(int id);
	bool m_Vertical = true;	

	wxMenuItem* m_pMenuString_Restore = nullptr;
	wxStatusBar* statusBar = nullptr;
	wxMenuBar* m_pMenuBar = nullptr;

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
		ID_SCRGO,
		ID_SCRPREV,
		ID_SCRPROX,
		ID_SCRSAVE,
		ID_SCRINSERT,
		ID_SCRCODE,
		ID_STRSAVE,
		ID_STRPREV,
		ID_STRPROX,
		ID_STRRESTORE,
		ID_FINDNEXT,
		ID_HVMODE,
		ID_HEXORIG,
		ID_HEXTRANS,
		ID_TEXTRANGE,
		ID_SEARCHWINDOW
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