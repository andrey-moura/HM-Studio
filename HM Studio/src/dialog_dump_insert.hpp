#include <wx/dialog.h>
#include <wx/radiobut.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/msgdlg.h>

#include "class_script_editor.hpp"

class InsertDumpDialog : public wxDialog
{
public:
	InsertDumpDialog(ScriptEditor& editor);	
private:
	ScriptEditor& m_Editor;	
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
