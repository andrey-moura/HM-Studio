#pragma once
#include <wx/wx.h>

#include "frame_editor.hpp"

#include "dialog_dump_insert.hpp"

#include "class_letter_editor.hpp"
#include "class_stc.hpp"

class LetterEditorFrame : public EditorFrame
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
	//Override from base
	void OnSaveFile();
	void OnDumpClick(wxCommandEvent& event);	
private:
	void CreateGUIControls();
};