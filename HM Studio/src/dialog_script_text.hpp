#pragma once

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/radiobut.h>
#include <wx/msgdlg.h>

#include "class_script.hpp"
#include "class_script_editor.hpp"
#include "class_rom.hpp"
#include "class_file.hpp"
#include "Studio.h"

class TextFromScriptDialog : public wxDialog
{
public:
	TextFromScriptDialog(ScriptEditor& editor);
private:
	ScriptEditor& m_Editor;
	size_t m_ScriptCount;
	size_t m_RomId;

	std::vector<std::string> m_Text;
public:
	std::vector<std::string> GetText() { return m_Text; }
private:
	void CreateGUIControls();
	
	wxTextCtrl* m_pFromNumberText = nullptr;
	wxChoice*   m_pFromNumberChoice = nullptr;	

	wxFilePickerCtrl* m_pFromFilePicker = nullptr;	

	wxButton* m_pGoButton = nullptr;

	wxBoxSizer* m_pRootSizer = nullptr;
private:
	void GetScriptText(const std::string& path);
private:	
	void OnGoButton(wxCommandEvent& event);
};

