#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>

#include "Studio.h"

class FrameSearchScript : public wxDialog
{
public:
	FrameSearchScript();
	~FrameSearchScript();

	static enum SearchMode {
		Find = -500,
		Replace,
		ReplaceExtended
	};

	std::string find;
	std::string replace;

private:
	void EVT_BTN_FindAllClick(wxCommandEvent& event);
	void EVT_BTN_ReplaceAllClick(wxCommandEvent& event);
private:
	void CreateGUIControls();;

	//-----------------------//

	wxNotebook* m_pGUI_tableControl = nullptr;

	wxStaticText* m_pGUI_findLabel = nullptr;
	wxTextCtrl* m_pGUI_inputFind = nullptr;
	wxButton* m_pGUI_findAll = nullptr;
	wxBoxSizer* m_pGUI_sizer1 = nullptr;

	wxStaticText* m_pGUI_replaceLabel = nullptr;
	wxTextCtrl* m_pGUI_inputReplace = nullptr;
	wxButton* m_pGUI_replaceAll = nullptr;
	wxBoxSizer* m_pGUI_replaceSizer = nullptr;

	wxRadioButton* m_pGUI_modeNormal = nullptr;
	wxRadioButton* m_pGUI_modeExtended = nullptr;
	wxStaticBoxSizer* m_pGUI_boxMode = nullptr;

	wxCheckBox* m_pGUI_optionsCase = nullptr;
	wxStaticBoxSizer* m_pGUI_boxOptions = nullptr;

	wxBoxSizer* m_pGUI_sizerConfig = nullptr;

	wxBoxSizer* m_pGUI_sizerMain = nullptr;

	//wxPanel* m_pGUI_mainPanel = nullptr;
};

