#pragma once
#include <wx/wx.h>

#include "dialog_dump_insert.hpp"

#include "class_letter_editor.hpp"

class LetterEditorFrame : public wxFrame 
{
public:
	LetterEditorFrame(const id& id);
	~LetterEditorFrame();
private:
	LetterEditor m_Editor;
private:
	void OnOpenClick(wxCommandEvent& event);
	void OnDumpClick(wxCommandEvent& event);
private:
	void CreateGUIControls();
};