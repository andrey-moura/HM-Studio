#pragma once
#include <wx/wx.h>

#include "class_editor.hpp"
#include "frame_search_script.hpp"
#include "window_find_results.hpp"
#include "dialog_dump_insert.hpp"
#include "window_find_results.hpp"
#include "dialog_get_text_from.hpp"

class EditorFrame : public wxFrame
{
public:
	EditorFrame(Editor* editor);	
protected:
	Editor* m_pEditor = nullptr;
protected:
//	void CreateMenu();
	void CreateMyToolBar(bool prev = true, bool next = true, bool go = true, bool save = true, bool insert = true);
	void CreateMyMenuBar();
	void CreateMyStatusBar(size_t count);
	void CreateFileMenu();
	void CreateStringMenu();
	void CreateSearchMenu();
	void CreateToolsMenu(bool padding, bool inserter, bool integrity, bool eol);
	void CreateViewMenu();
	void CreateButtonsSizer();
	void UpdateIndex();	
	void ShowResultWindow(const FilesResults& results);
protected:
	wxToolBar* m_pToolBar = nullptr;
	wxMenuBar* m_pMenuBar = nullptr;
	wxMenu* m_pMenuSearch = nullptr;
	wxMenu* m_pMenuTools = nullptr;
	wxMenu* m_pMenuView = nullptr;
	wxMenu* m_pMenuFile = nullptr;
	wxMenu* m_pMenuString = nullptr;
	wxStatusBar* m_pStatusBar = nullptr;
	wxButton* m_pPrevTextButton = nullptr;
	wxButton* m_pNextTextButton = nullptr;
	wxButton* m_pSaveTextButton = nullptr;
	wxBoxSizer* m_pButtonsSizer = nullptr;
	FindResultsWindow* m_pFindResultsWindow = nullptr;

	std::vector<size_t> m_FindPositions;
	size_t m_FindIndex;
//Handlers
protected:
//Override this
	virtual void OnPrevString();
//Override this
	virtual void OnProxString();
//Override this
	virtual void OnSaveString() {};
//Override this
	virtual void OnPrevFile();
//Override this
	virtual void OnProxFile();
//Override this
	virtual void OnGetTextFrom();
//Override this
	virtual void OnGoFile();
//Override this
	virtual void OnSaveFile();
//Override this
	virtual void OnDumperInserter();
//Override this
	virtual void OnCheckIntegrity();
//Override this
	virtual void OnEolCheckClick();
//Override this
	virtual void UpdateText() {}
//Override this
	virtual void OnFindText();
//Override this
	virtual void OnFindNext() {}
//Override this
	virtual void FindText(const wxString& search, bool useTable, bool translated);
//Override this
	virtual void ReplaceTxt(const wxString& search, const wxString& replace, bool useTable);
//Override this
	virtual void SetIndex(size_t index);
//Override this
	virtual void RemovePaddingSpaces() {}
//Override this
	virtual void OnInsertFile();
//Override this
	virtual void OnAlwaysOnTop() { ToggleWindowStyle(wxSTAY_ON_TOP); }
//Override this
	virtual void OnShowResultWindow();
//Override this
	virtual void OnOpenHexEditor(bool translated);
//Override this
	virtual void OnResultClick() {};
//Events	
protected:
	void OnPrevStringClick(wxCommandEvent& event);
	void OnProxStringClick(wxCommandEvent& event);
	void OnSaveStringClick(wxCommandEvent& event);
	void OnPrevFileClick(wxCommandEvent& event);
	void OnProxFileClick(wxCommandEvent& event);
	void OnOriginalHexEditor(wxCommandEvent& event);
	void OnTranslatedHexEditor(wxCommandEvent& event);
	void OnGetTextFromClick(wxCommandEvent& event);
	void OnGoFileClick(wxCommandEvent& event);
	void OnSaveFileClick(wxCommandEvent& event);
	void OnDumperInserterClick(wxCommandEvent& event);
	void OnCheckIntegrityClick(wxCommandEvent& event);
	void OnEolCheckClick(wxCommandEvent& event);
	void OnFindTextClick(wxCommandEvent& event);	
	void OnFindNextClick(wxCommandEvent& event);
	void OnRemoveSpacesClick(wxCommandEvent& event);
	void OnInsertFileClick(wxCommandEvent& event);
	void OnAlwaysOnTopClick(wxCommandEvent& event);
	void OnResultClick(wxCommandEvent& event);
	void OnShowFindResultClick(wxCommandEvent& event);
};

