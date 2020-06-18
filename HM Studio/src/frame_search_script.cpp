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

void FrameSearchScript::OnText(wxCommandEvent& event)
{
	bool opened = wxTheClipboard->Open();
	
#ifdef _DEBUG
   _STL_VERIFY(opened, "An error ucurred while opening the clipboard");
#endif
   
	if(opened)
	{
		wxTextDataObject data;
		wxTheClipboard->GetData(data);
				
		wxString text = data.GetText();		
		//Moon::String::ConvertLineEnds(text, *(uint32_t*)m_RawEOL.c_str());				
		text.Replace(m_RawEOL, m_EOL);

		m_pModeBox->SetSelection(text.find(L"\\n") != wxString::npos);

		if (event.GetId() == m_pInputFind->GetId())
			m_pInputFind->AppendText(text);
		else
			m_pInputReplace->AppendText(text);
	
		wxTheClipboard->Close();
	}	
}

wxString FrameSearchScript::GetSearch()
{
	wxString search = m_pInputFind->GetValue();

	if (Extended())
		search.Replace(m_EOL, m_RawEOL);		

	return search;
}

wxString FrameSearchScript::GetReplace()
{
	wxString replace = m_pInputReplace->GetValue();

	if (Extended())
		replace.Replace(m_EOL, m_RawEOL);

	return replace;
}

void FrameSearchScript::OnTextEnter(wxCommandEvent& event)
{
	if (event.GetId() == m_pInputFind->GetId())
	{
		m_pInputFind->AppendText(m_EOL);
	}
	else
	{
		m_pInputReplace->AppendText(m_EOL);
	}

	m_pModeBox->SetSelection(1);

	event.Skip();
}

void FrameSearchScript::CreateGUIControls()
{		
	m_pInputFind = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	m_pInputFind->Bind(wxEVT_TEXT_PASTE, &FrameSearchScript::OnText, this);
	m_pInputFind->Bind(wxEVT_TEXT_ENTER, &FrameSearchScript::OnTextEnter, this);
	wxButton* findButton = new wxButton(this, wxID_FIND, "Find All");
	findButton->Bind(wxEVT_BUTTON, &FrameSearchScript::OnFindAllClick, this);

	wxBoxSizer* findSizer = new wxBoxSizer(wxHORIZONTAL);
	findSizer->Add(new wxStaticText(this, wxID_ANY, "Find What:"), 0, wxALL, 0);
	findSizer->AddSpacer(4);
	findSizer->Add(m_pInputFind, 1, wxALL | wxEXPAND, 0);
	findSizer->AddSpacer(4);
	findSizer->Add(findButton, 0, wxALL, 0);
	
	m_pInputReplace = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	m_pInputReplace->Bind(wxEVT_TEXT_PASTE, &FrameSearchScript::OnText, this);
	m_pInputReplace->Bind(wxEVT_TEXT_ENTER, &FrameSearchScript::OnTextEnter, this);
	wxButton* replaceButton = new wxButton(this, wxID_ANY, "Replace All");
	replaceButton->Bind(wxEVT_BUTTON, &FrameSearchScript::OnReplaceAllClick, this);	
	replaceButton->SetMinSize(findButton->GetMinSize());

	wxBoxSizer* replaceSizer = new wxBoxSizer(wxHORIZONTAL);
	replaceSizer->Add(new wxStaticText(this, wxID_ANY, "Replace With:"));
	replaceSizer->AddSpacer(4);
	replaceSizer->Add(m_pInputReplace, 1, wxALL, 0);
	replaceSizer->AddSpacer(4);
	replaceSizer->Add(replaceButton);

	wxArrayString modes;
	modes.Add("Normal");
	modes.Add("Extended");

	m_pModeBox = new wxRadioBox(this, wxID_ANY, "Mode", wxDefaultPosition, wxDefaultSize, modes, 0, wxRA_VERTICAL);

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
	configSizer->Add(m_pModeBox, 0, wxEXPAND, 0);
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
