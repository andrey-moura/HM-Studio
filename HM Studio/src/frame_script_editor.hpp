#pragma once

#define NormalStyle 0
#define VarStyle 1
#define SimbolStyle 2

#define WaitInput 500

#define LINEERROR_MASK 0b00000001

#define INDIC_FINDSTRING 1

//#include "wx/wx.h"
#include "wx/frame.h"
#include <wx/stc/stc.h>
#include <wx/clipbrd.h>
#include <wx/txtstrm.h>
#include <wx/popupwin.h>
#include <wx/textbuf.h>
#include <wx/process.h>

#include "class_rom.hpp"
#include "class_script.hpp"

#include "Studio.h"

#include "class_util_wx_file.hpp"
#include "class_util_string.hpp"
#include "class_table.hpp"
#include "frame_search_script.hpp"
#include "wxUtil/window_find_results.hpp"

#include <chrono>
#include <sstream>

#ifdef Testing
#define HUNSPELL_STATIC


#include "include/hunspell/hunspell.hxx"


const char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
#endif // Testing


class cScriptEditor : public wxFrame
{
public:
	cScriptEditor(id i);
	~cScriptEditor();	

	void SetupRom();
//Events
private:

	//wxDECLARE_EVENT_TABLE();

	void tScriptTranslatedOnStyleNeeded(wxStyledTextEvent& event);
	void tScriptOriginalOnStyleNeeded(wxStyledTextEvent& event);
	void tScritpTranslatedOnModified(wxStyledTextEvent& event);
	void tScriptTranslatedOnUi(wxStyledTextEvent& event);
#ifdef Testing
	void tScriptTranslatedCharAdded(wxStyledTextEvent& event);
#endif // Testing	
	void OnPrevScriptClick(wxCommandEvent& event);
	void OnProxScriptClick(wxCommandEvent& event);
	void OnGoScriptClick(wxCommandEvent& event);
	void OnSaveTextClick(wxCommandEvent& event);
	void OnPrevTextClick(wxCommandEvent& event);
	void OnProxTextClick(wxCommandEvent& event);
	void OnMenuHorizontalMode(wxCommandEvent& event);
	void OnMenuGetTextFromScriptFile(wxCommandEvent& event);
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
	int m_maxLineLenght;
	double m_lastTyped;
	size_t m_typedMinPos;
	size_t m_typedMaxPos;
	bool m_endTyping;	
	std::pair<int, int> m_IndicatorPos;
	size_t m_VarSize = 0;
	std::string m_PlayerVar;
	std::string m_FarmVar;

	wxPopupWindow* m_pSTCMenu = nullptr;
//STC Functions
	inline void STCFindAll(wxStyledTextCtrl* stc, const size_t start, const size_t end, const std::string& toFind, std::vector<size_t>& output);
	inline void HighlightAll(wxStyledTextCtrl* stc, std::vector<size_t>& pos, const size_t size, const int style);	
//STC Script Funcions
	inline void VerifyLineLenght(wxStyledTextCtrl* stc, int lenght);
	inline void VerifyCurLineLenght(wxStyledTextCtrl* stc);	
	inline void VerifyAllLinesLenght(wxStyledTextCtrl* stc);
	inline void FindAndHighlightAllVars(wxStyledTextCtrl* stc, const size_t start, const size_t end);
	inline void UpdateStyle(wxStyledTextCtrl* stc);
#ifdef Testing
	void DoSpelling(wxStyledTextCtrl* stc, const size_t start, const size_t end);
#endif // Testing

	void SetupStyles(wxStyledTextCtrl* stc);

	void UpdateStatusText(wxStyledTextCtrl* stc);

//Spell
private:
#ifdef Testing
	Hunspell* m_hunspell = nullptr;
#endif // Testing

//others
private:	
	void SolveProblem();
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
	void CheckAllCode();
	void CheckAndGoScript(size_t index);
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

	wxBoxSizer* global_sizer = nullptr;	

	//Editor
	wxStyledTextCtrl* tScriptTranslated = nullptr;
	wxButton* editor_save_text = nullptr;
	wxButton* editor_prev_text = nullptr;
	wxButton* editor_next_text = nullptr;	
	wxBoxSizer* editor_sizer = nullptr;
	wxBoxSizer* editor_buttons_sizer = nullptr;
	wxStyledTextCtrl* tScriptOriginal = nullptr;

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