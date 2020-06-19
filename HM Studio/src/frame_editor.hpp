#pragma once
#include <wx/wx.h>

#include "class_editor.hpp"
#include "frame_search_script.hpp"
#include "window_find_results.hpp"
#include "dialog_dump_insert.hpp"

class EditorFrame : public wxFrame
{
public:
	EditorFrame(const wxString& type, Editor* editor);	
protected:
	Editor* m_pEditor = nullptr;
protected:
//	void CreateMenu();
	void CreateMyToolBar();
	void CreateMyMenuBar();
	void CreateMyStatusBar(size_t count);
	void CreateSearchMenu();
	void CreateToolsMenu();
	void UpdateIndex();
protected:
	wxToolBar* m_pToolBar = nullptr;
	wxMenuBar* m_pMenuBar = nullptr;
	wxMenu* m_pMenuSearch = nullptr;
	wxMenu* m_pMenuTools = nullptr;
	wxStatusBar* m_pStatusBar = nullptr;

	wxString m_Type;

	std::vector<size_t> m_FindPositions;
	size_t m_FindIndex;
//Handlers
protected:
//Override this
	virtual void OnPrevFile();
//Override this
	virtual void OnProxFile();
//Override this
	virtual void OnGoFile();
//Override this
	virtual void OnSaveFile();
//Override this
	virtual void OnDumperInserter();
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
//Events	
private:	
	void OnPrevFileClick(wxCommandEvent& event);
	void OnProxFileClick(wxCommandEvent& event);
	void OnGoFileClick(wxCommandEvent& event);
	void OnSaveFileClick(wxCommandEvent& event);
	void OnDumperInserterClick(wxCommandEvent& event);	
	void OnFindTextClick(wxCommandEvent& event);	
	void OnFindNextClick(wxCommandEvent& event);
	void OnRemoveSpacesClick(wxCommandEvent& event);
};

