#pragma once
#include <wx/wx.h>

#include "dialog_dump_insert.hpp"

#include "class_letter_editor.hpp"
#include "class_stc.hpp"

class LetterEditorFrame : public wxFrame 
{
public:
	LetterEditorFrame(const id& id);
	~LetterEditorFrame();
private:
	LetterEditor m_Editor;

	STC* m_TextOriginal = nullptr;
	STC* m_TextTranslated = nullptr;
private:
	void UpdateText();
private:
	void OnOpenClick(wxCommandEvent& event);
	void OnSaveClick(wxCommandEvent& event);
	void OnDumpClick(wxCommandEvent& event);
private:
	void CreateGUIControls();
};