#include "frame_search_script.hpp"

FrameSearchScript::FrameSearchScript() : wxDialog(nullptr, wxID_ANY, _("Search"))
{
	this->SetSize(wxSize(600, 200));
	CreateGUIControls();
}

FrameSearchScript::~FrameSearchScript()
{
}

void FrameSearchScript::EVT_BTN_FindAllClick(wxCommandEvent& event)
{
	find = m_pGUI_inputFind->GetValue();
	replace = m_pGUI_inputReplace->GetValue();	
	if (m_pGUI_InScript->GetValue())
		this->EndModal(SearchInScripts);
	else 
		this->EndModal(Find);
}

void FrameSearchScript::EVT_BTN_ReplaceAllClick(wxCommandEvent& event)
{
	find = m_pGUI_inputFind->GetValue();
	replace = m_pGUI_inputReplace->GetValue();

	if (m_pGUI_modeExtended->GetValue())
		this->EndModal(ReplaceExtended);
	else
		this->EndModal(Replace);
}

void FrameSearchScript::CreateGUIControls()
{
	this->SetFont(Studio::GetDefaultFont());
	this->SetBackgroundColour(Studio::GetFrameColour());
	this->SetForegroundColour(Studio::GetFontColour());
	//m_pGUI_tableControl = new wxNotebook(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 400));
	//m_pGUI_tableControl->SetBackgroundColour(wxColour(255, 255, 255));

	//m_pGUI_mainPanel = new wxPanel(m_pGUI_tableControl, wxID_ANY, wxPoint(235, 229), wxDefaultSize, wxTAB_TRAVERSAL);

	m_pGUI_findLabel = new wxStaticText(this, wxID_ANY, _("Find What:   "));
	m_pGUI_findLabel->SetFont(Studio::GetDefaultFont());
	m_pGUI_inputFind = new wxTextCtrl(this, wxID_ANY);
	m_pGUI_findAll = new wxButton(this, wxID_FIND, _("Find All"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	m_pGUI_findAll->Bind(wxEVT_BUTTON, &FrameSearchScript::EVT_BTN_FindAllClick, this);

	m_pGUI_replaceLabel = new wxStaticText(this, wxID_ANY, _("Replace With:"));
	m_pGUI_replaceLabel->SetFont(Studio::GetDefaultFont());
	m_pGUI_inputReplace = new wxTextCtrl(this, wxID_ANY);
	m_pGUI_replaceAll = new wxButton(this, wxID_ANY, _("Replace All"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	m_pGUI_replaceAll->Bind(wxEVT_BUTTON, &FrameSearchScript::EVT_BTN_ReplaceAllClick, this);
	//m_pGUI_replaceAll->SetMinSize(wxSize(60, 30));
	m_pGUI_findAll->SetMinSize(m_pGUI_replaceAll->GetMinSize());

	m_pGUI_sizer1 = new wxBoxSizer(wxHORIZONTAL);
	m_pGUI_sizer1->Add(m_pGUI_findLabel, 0, wxALL, 0);
	m_pGUI_sizer1->AddSpacer(4);
	m_pGUI_sizer1->Add(m_pGUI_inputFind, 1, wxALL | wxEXPAND, 0);
	m_pGUI_sizer1->AddSpacer(4);
	m_pGUI_sizer1->Add(m_pGUI_findAll, 0, wxALL, 0);

	m_pGUI_replaceSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pGUI_replaceSizer->Add(m_pGUI_replaceLabel, 0, wxALL, 0);
	m_pGUI_replaceSizer->AddSpacer(4);
	m_pGUI_replaceSizer->Add(m_pGUI_inputReplace, 1, wxALL, 0);
	m_pGUI_replaceSizer->AddSpacer(4);
	m_pGUI_replaceSizer->Add(m_pGUI_replaceAll, 0, wxALL, 0);

	m_pGUI_modeNormal = new wxRadioButton(this, wxID_ANY, _("Normal"));
	m_pGUI_modeExtended = new wxRadioButton(this, wxID_ANY, _("Extended"));

	m_pGUI_boxMode = new wxStaticBoxSizer(wxVERTICAL, this, _("Mode"));
	m_pGUI_boxMode->Add(m_pGUI_modeNormal, 1, wxALL, 0);
	m_pGUI_boxMode->Add(m_pGUI_modeExtended, 1, wxALL, 0);

	m_pGUI_optionsCase = new wxCheckBox(this, wxID_ANY, _("Match Case"));
	m_pGUI_optionsCase->SetValue(false);

	m_pGUI_InScript = new wxCheckBox(this, wxID_ANY, _("In Script"));
	m_pGUI_InScript->SetValue(false);

	m_pGUI_boxOptions = new wxStaticBoxSizer(wxVERTICAL, this, _("Options"));
	m_pGUI_boxOptions->Add(m_pGUI_optionsCase, 1, wxALL, 0);
	m_pGUI_boxOptions->Add(m_pGUI_InScript, 1, wxALL, 0);

	m_pGUI_sizerConfig = new wxBoxSizer(wxHORIZONTAL);
	m_pGUI_sizerConfig->Add(m_pGUI_boxMode, 0, wxALL, 0);
	m_pGUI_sizerConfig->AddSpacer(4);
	m_pGUI_sizerConfig->Add(m_pGUI_boxOptions, 0, wxALL, 0);
	
	m_pGUI_sizerMain = new wxBoxSizer(wxVERTICAL);

	m_pGUI_sizerMain->Add(m_pGUI_sizer1, 0, wxALL | wxEXPAND, 4);		
	m_pGUI_sizerMain->Add(m_pGUI_replaceSizer, 0, wxALL | wxEXPAND, 4);
	m_pGUI_sizerMain->Add(m_pGUI_sizerConfig, 0, wxALL | wxEXPAND, 0);

	//m_pGUI_mainPanel->SetSizer(m_pGUI_sizerMain);
	//m_pGUI_sizerMain->Fit(m_pGUI_mainPanel);
	//m_pGUI_sizerMain->SetSizeHints(m_pGUI_mainPanel);

	this->SetSizer(m_pGUI_sizerMain);
	m_pGUI_sizerMain->Fit(this);
	m_pGUI_sizerMain->SetSizeHints(this);

	//m_pGUI_tableControl->AddPage(m_pGUI_mainPanel, _("Find"), false);
}
