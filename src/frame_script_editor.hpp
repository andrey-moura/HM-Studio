#pragma once

#include <thread>
#include <sstream>

#include <wx/frame.h>
#include <wx/clipbrd.h>
#include <wx/txtstrm.h>
#include <wx/textbuf.h>
#include <wx/string.h>

#include "window_find_results.hpp"
#include "frame_search_script.hpp"
#include "dialog_dump_insert.hpp"

#include "class_rom_file.hpp"
#include "class_script.hpp"
#include "class_stc.hpp"
#include "class_script_editor.hpp"
#include "class_spell_checker.hpp"
#include "frame_editor.hpp"

class ScriptEditorFrame : public EditorFrame
{
public:
	ScriptEditorFrame(id i);
	~ScriptEditorFrame();	

	void SetupRom(); 
//Events
private:
	void OnResultClick(wxCommandEvent& event);
	void OnConvertEOLClick(wxCommandEvent& event);
	void OnCheckCodeClick(wxCommandEvent& event);
	void OnSetEventClick(wxCommandEvent& event);
	void SetTextRange();
	void OnHorizontalModeClick(wxCommandEvent& event);
	void OnCodeStyleNeeded(wxStyledTextEvent& event);	
private:
//Text save
	void ConfigureSTC(STC* stc, const RomFile& rom);	
	size_t GetNumberFromResult(const SearchResult& result);
	size_t GetIndexFromResult(const SearchResult& result, size_t clicked);
private:
	void BackupText();
	void RestoreText();
//Script manipulation
private:
	void OpenScript(size_t index);
	void SaveScript();		
	void UpdateScript();	
	void FindText();
public:
	void ScriptTextRange(size_t from, size_t to, size_t script);
public:
	virtual void OnSaveString() override;
	virtual void UpdateText() override;
	virtual void UpdateFile() override;
//Forms

//Text manipulation
private:	
	std::vector<size_t> m_FindPos;
	size_t m_FindIndex;	
	wxFileName m_ScriptDic;
	size_t m_DicIndex = std::string::npos;
	std::string m_BackupFile;

	void UpdateScriptDic();
//GUI
private:
	void CreateGUIControls();
	wxToolBar* m_pToolBar = nullptr;
	wxImage m_ImgPrevScript;

	void SetEditorVertical();
	void SetEditorHorizontal();	
	bool m_Vertical = true;	

	//Editor	
	STC* tScriptTranslated = nullptr;
	wxButton* editor_save_text = nullptr;
	
	wxButton* editor_prev_text = nullptr;
	wxButton* editor_next_text = nullptr;
	wxBoxSizer* editor_sizer = nullptr;
	wxBoxSizer* editor_buttons_sizer = nullptr;
	STC* tScriptOriginal = nullptr;

	STC* m_pSrcCodeOutput = nullptr;
	STC* m_pDecompiler = nullptr;

	wxBoxSizer* global_sizer = nullptr;	
	FindResultsWindow* m_pFindResultsWindow = nullptr;	
};

class DialogTextRange : public wxDialog
{
public:
	DialogTextRange(ScriptEditorFrame* parent);
	~DialogTextRange() = default;

	ScriptEditorFrame* m_pParent = nullptr;
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