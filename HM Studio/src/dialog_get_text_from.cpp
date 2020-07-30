#include "dialog_get_text_from.hpp"

GetTextFromDialog::GetTextFromDialog(Editor* editor) : wxDialog(nullptr, wxID_ANY, "Select script"), m_pEditor(editor)
{
	SetFont(wxFontInfo(10).FaceName("Courier New"));
	CreateGUIControls();
}

void GetTextFromDialog::CreateGUIControls()
{
	m_pFromNumberText = new wxTextCtrl(this, wxID_ANY);
	m_pFromNumberText->SetMaxLength(4);
	m_pFromNumberText->Bind(wxEVT_TEXT, &GetTextFromDialog::OnNumberChange, this);
	
	wxBoxSizer* fromNumberSizer = new wxBoxSizer(wxHORIZONTAL);
	fromNumberSizer->Add(new wxStaticText(this, wxID_ANY, "Number:   "));
	fromNumberSizer->Add(m_pFromNumberText, 1, wxEXPAND, 0);

	m_pFromFilePicker = new wxFilePickerCtrl(this, wxID_ANY);
	m_pFromFilePicker->SetMinSize({ 300, m_pFromFilePicker->GetMinSize().GetHeight() });
	m_pFromFilePicker->Bind(wxEVT_FILEPICKER_CHANGED, &GetTextFromDialog::OnFilePickerChange, this);
	
	wxBoxSizer* fromFileSizer = new wxBoxSizer(wxHORIZONTAL);
	fromFileSizer->Add(new wxStaticText(this, wxID_ANY, "From Path:"));
	fromFileSizer->Add(m_pFromFilePicker, 0, wxEXPAND, 0);

	m_pRootSizer = new wxBoxSizer(wxVERTICAL);	
	m_pRootSizer->Add(fromNumberSizer, 1, wxEXPAND, 0);
	m_pRootSizer->Add(fromFileSizer);
	m_pRootSizer->Add(CreateSeparatedButtonSizer(wxCANCEL | wxOK), 0, wxEXPAND, 0);
	SetSizer(m_pRootSizer);
	m_pRootSizer->Fit(this);
	m_pRootSizer->SetSizeHints(this);
}

void GetTextFromDialog::OnNumberChange(wxCommandEvent& event)
{	
	if (!m_ChangedByDialog)
		m_pFromFilePicker->SetPath(wxEmptyString);

	m_ChangedByDialog = false;

	event.Skip();
}

void GetTextFromDialog::OnFilePickerChange(wxCommandEvent& event)
{	
	m_ChangedByDialog = true;
	m_pFromNumberText->Clear();

	event.Skip();
}