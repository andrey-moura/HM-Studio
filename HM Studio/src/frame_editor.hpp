#pragma once
#include <wx/wx.h>

#include "class_editor.hpp"

class EditorFrame : public wxFrame
{
public:
	EditorFrame(const char* title, Editor* editor);	
protected:
	Editor* m_pEditor = nullptr;
protected:
//	void CreateMenu();
	void CreateMyToolBar();
	void CreateMyStatusBar(size_t count);

	void UpdateIndex();
protected:
	wxToolBar* m_pToolBar = nullptr;
	wxStatusBar* m_pStatusBar = nullptr;
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
	virtual void OnInsertFile();
//Override this
	virtual void UpdateText() {};
//Events	
protected:
	void OnPrevFileClick(wxCommandEvent& event);
	void OnProxFileClick(wxCommandEvent& event);
	void OnGoFileClick(wxCommandEvent& event);
	void OnSaveFileClick(wxCommandEvent& event);
	void OnInsertFileClick(wxCommandEvent& event);	
};

