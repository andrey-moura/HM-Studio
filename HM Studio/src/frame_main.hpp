#pragma once

#include <wx/frame.h>
#include <wx/button.h>
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>
//Frames
#include "frame_script_editor.hpp"
#include "frame_item_editor.hpp"

//tests
#ifdef _DEBUG
#include "class_spell_checker.hpp"
#endif

class MainFrame : public wxFrame
{
public:
	MainFrame();
	~MainFrame();

private:	
	void OnChoiceChanged(wxCommandEvent& event);
	void scriptEditor_onClick(wxCommandEvent& event);
	void OnButtonTeste1_Click(wxCommandEvent& event);
	void OnButtonDumpOriginal(wxCommandEvent& event);
	void OnButtonDumpTranslated(wxCommandEvent& event);
	void EVT_BUTTON_ItemEditorClick(wxCommandEvent& event);

private:
	uint8_t cur_choice = 0;	
private:
	id GetCurrentId();
//UI
private:
	void CreateGUIControls();

	wxBoxSizer* sizer0 = nullptr;
	wxBoxSizer* sizer1 = nullptr;
	wxBoxSizer* sizer2 = nullptr;

	wxStaticBox* romBox = nullptr;
	wxStaticBoxSizer* romSizer = nullptr;
	wxButton* openRom = nullptr;
	wxButton* romBackup = nullptr;

	wxStaticBox* scriptBox = nullptr;
	wxStaticBoxSizer* scriptSizer = nullptr;
	wxButton* dumpTranslated = nullptr;
	wxButton* dumpOriginal = nullptr;
	wxButton* scriptEditor = nullptr;

	wxStaticBox* currentBox = nullptr;
	wxStaticBoxSizer* currentSizer = nullptr;
	wxChoice* currentChoice = nullptr;
	wxCheckBox* currentDefault = nullptr;	

	wxButton* teste_button_1 = nullptr;
	wxStaticBox* teste_box = nullptr;
	wxStaticBoxSizer* teste_sizer = nullptr;

	wxButton* m_pGUI_openItemEditor = nullptr;
	wxStaticBox* m_pGUI_boxItemEditor = nullptr;
	wxStaticBoxSizer* m_pGUI_boxSizerItemEditor = nullptr;	
};

