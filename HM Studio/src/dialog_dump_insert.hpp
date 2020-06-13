#pragma once

#include <wx/wx.h>

#include "class_editor.hpp"

class InsertDumpDialog : public wxDialog
{
public:
	InsertDumpDialog(Editor* editor);
private:
	Editor* m_Editor = nullptr;
	bool m_Translated = false;
//GUI
private:
	wxBoxSizer* m_RootSizer = nullptr;

	void CreateGUIControls();
//Events
private:
	void OnRangeTypeChanged(wxCommandEvent& event);
	void OnStateChanged(wxCommandEvent& event);
	void OnDumpClick(wxCommandEvent& event);
	void OnInsertClick(wxCommandEvent& event);
};
