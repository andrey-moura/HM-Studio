#include "frame_search_script.hpp"

FrameSearchScript::FrameSearchScript() : wxDialog(nullptr, wxID_ANY, "Search")
{
	this->SetSize(wxSize(600, 200));
	CreateGUIControls();
}

FrameSearchScript::~FrameSearchScript()
{
}

void FrameSearchScript::OnFindAllClick(wxCommandEvent& event)
{
	m_Find = true;		

	EndModal(wxID_OK);
	event.Skip();
}

void FrameSearchScript::OnReplaceAllClick(wxCommandEvent& event)
{
	m_Find = false;

	EndModal(wxID_OK);
	event.Skip();
}

void FrameSearchScript::InScript(bool inScript)
{
	m_pInScript->SetValue(inScript);
	m_pUseTable->SetValue(inScript);
}

void FrameSearchScript::OnInScriptClick(wxCommandEvent& event)
{	
	m_pUseTable->SetValue(m_pInScript->GetValue());

	event.Skip();
}

void FrameSearchScript::CreateGUIControls()
{		
	wxStaticText* findLabel = new wxStaticText(this, wxID_ANY, "Find What:   ");
	
	m_pInputFind = new wxTextCtrl(this, wxID_ANY);
	wxButton* findButton = new wxButton(this, wxID_FIND, "Find All");
	findButton->Bind(wxEVT_BUTTON, &FrameSearchScript::OnFindAllClick, this);

	wxBoxSizer* findSizer = new wxBoxSizer(wxHORIZONTAL);
	findSizer->Add(findLabel, 0, wxALL, 0);
	findSizer->AddSpacer(4);
	findSizer->Add(m_pInputFind, 1, wxALL | wxEXPAND, 0);
	findSizer->AddSpacer(4);
	findSizer->Add(findButton, 0, wxALL, 0);

	wxStaticText* replaceLabel = new wxStaticText(this, wxID_ANY, "Replace With:");	
	m_pInputReplace = new wxTextCtrl(this, wxID_ANY);
	wxButton* replaceButton = new wxButton(this, wxID_ANY, "Replace All");
	replaceButton->Bind(wxEVT_BUTTON, &FrameSearchScript::OnReplaceAllClick, this);	
	replaceButton->SetMinSize(findButton->GetMinSize());

	wxBoxSizer* replaceSizer = new wxBoxSizer(wxHORIZONTAL);
	replaceSizer->Add(replaceLabel);
	replaceSizer->AddSpacer(4);
	replaceSizer->Add(m_pInputReplace, 1, wxALL, 0);
	replaceSizer->AddSpacer(4);
	replaceSizer->Add(replaceButton);

	wxRadioButton* modeNormal = new wxRadioButton(this, wxID_ANY, "Normal");
	modeNormal->SetValue(true);
	m_pModeExtended = new wxRadioButton(this, wxID_ANY, "Extended");

	wxStaticBoxSizer* modeSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Mode");
	modeSizer->Add(modeNormal);
	modeSizer->Add(m_pModeExtended);

	m_pCase = new wxCheckBox(this, wxID_ANY, "Match Case");
	m_pCase->SetValue(true);

	m_pInScript = new wxCheckBox(this, wxID_ANY, "In Script");
	m_pInScript->Bind(wxEVT_CHECKBOX, &FrameSearchScript::OnInScriptClick, this);
	m_pInScript->SetValue(false);

	m_pUseTable = new wxCheckBox(this, wxID_ANY, "Use Table");
	m_pUseTable->SetValue(false);	

	wxStaticBoxSizer* optionsSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Options");
	optionsSizer->Add(m_pCase);
	optionsSizer->Add(m_pInScript);
	optionsSizer->Add(m_pUseTable);

	m_pTranslated = new wxCheckBox(this, wxID_ANY, "Translated");
	m_pTranslated->SetValue(true);

	wxBoxSizer* configSizer = new wxBoxSizer(wxHORIZONTAL);
	configSizer->Add(modeSizer);
	configSizer->AddSpacer(4);
	configSizer->Add(optionsSizer);
	configSizer->AddSpacer(4);
	configSizer->Add(m_pTranslated, 0, wxCENTER, 0);
	
	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	rootSizer->Add(findSizer, 0, wxALL | wxEXPAND, 4);
	rootSizer->Add(replaceSizer, 0, wxALL | wxEXPAND, 4);
	rootSizer->Add(configSizer, 0, wxALL | wxEXPAND, 0);	

	this->SetSizer(rootSizer);
	rootSizer->Fit(this);
	rootSizer->SetSizeHints(this);
}
