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
	STC* m_TextOriginal = nullptr;
	STC* m_TextTranslated = nullptr;

	wxString m_TypingControl;
	bool m_AlreadyTyping;
	bool m_Garbage;
private:
	void UpdateText();
private:
	//Override from base
	void OnSaveFile();
	void FindText(const wxString& search, bool useTable, bool translated);
    void ReplaceTxt(const wxString& search, const wxString& replace, bool useTable);	
	void RemovePaddingSpaces();
	void STCKeyDown(wxKeyEvent& event);
	void STCKeyUp(wxKeyEvent& event);
	void StcOnUi(wxStyledTextEvent& event);
private:
	void CreateGUIControls();
};