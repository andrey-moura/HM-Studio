#pragma once

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/radiobut.h>
#include <wx/msgdlg.h>
#include <wx/button.h>

#include "class_script.hpp"
#include "class_script_editor.hpp"
#include "class_rom_file.hpp"

class GetTextFromDialog : public wxDialog
{
public:
	GetTextFromDialog(Editor* editor);
private:
	Editor* m_pEditor;
	size_t m_ScriptCount;
	size_t m_RomId;
	bool m_ChangedByDialog = false;
public:
	bool FromNumber() { return m_pFromNumberText->GetLineLength(0); }
	wxFileName GetFileName() { return m_pFromFilePicker->GetFileName(); }
	size_t GetNumber() { return std::stoi(m_pFromNumberText->GetValue().ToStdString()); }
private:
	void CreateGUIControls();
	
	wxTextCtrl* m_pFromNumberText = nullptr;
	wxFilePickerCtrl* m_pFromFilePicker = nullptr;
	wxButton* m_pGoButton = nullptr;
	wxBoxSizer* m_pRootSizer = nullptr;
private:
	void OnNumberChange(wxCommandEvent& event);
	void OnFilePickerChange(wxCommandEvent& event);
};

