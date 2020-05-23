#include "dialog_dump_insert.hpp"

InsertDumpDialog::InsertDumpDialog(ScriptEditor& editor) : wxDialog(nullptr, wxID_ANY, "Dump/Insert Scripts"), m_Editor(editor)
{
	CreateGUIControls();
}

void InsertDumpDialog::CreateGUIControls()
{
	wxArrayString fromToOptions;

	fromToOptions.Add("All");
	fromToOptions.Add("From/To");

	wxRadioBox* rangeBox = new wxRadioBox(this, wxID_ANY, "Range", wxDefaultPosition, wxDefaultSize, fromToOptions, 0, wxRA_VERTICAL);
	rangeBox->SetSelection(0);
	rangeBox->Bind(wxEVT_RADIOBOX, &InsertDumpDialog::OnRangeTypeChanged, this);
	
	wxTextCtrl* fromInput = new wxTextCtrl(this, wxID_ANY);
	
	wxTextCtrl* toInput = new wxTextCtrl(this, wxID_ANY);

	wxBoxSizer* fromSizer = new wxBoxSizer(wxHORIZONTAL);
	fromSizer->Add(new wxStaticText(this, wxID_ANY, "From: "));
	fromSizer->Add(fromInput);

	wxBoxSizer* toSizer = new wxBoxSizer(wxHORIZONTAL);
	toSizer->Add(new wxStaticText(this, wxID_ANY, "To:"));
	toSizer->AddStretchSpacer(1);
	toSizer->Add(toInput);

	wxBoxSizer* fromToSizer = new wxBoxSizer(wxVERTICAL);
	fromToSizer->Add(toSizer, 0, wxEXPAND);
	fromToSizer->Add(fromSizer);

	wxArrayString states;
	states.Add("Original");
	states.Add("Translated");

	wxRadioBox* stateBox = new wxRadioBox(this, wxID_ANY, "State", wxDefaultPosition, wxDefaultSize, states, 0, wxRA_VERTICAL);
	stateBox->Bind(wxEVT_RADIOBOX, &InsertDumpDialog::OnStateChanged, this);

	wxButton* dump = new wxButton(this, wxID_ANY, "Dump");
	dump->Bind(wxEVT_BUTTON, &InsertDumpDialog::OnDumpClick, this);

	wxButton* insert = new wxButton(this, wxID_ANY, "Insert");
	insert->Bind(wxEVT_BUTTON, &InsertDumpDialog::OnInsertClick, this);

	m_RootSizer = new wxBoxSizer(wxVERTICAL);
	m_RootSizer->Add(rangeBox, 0, wxEXPAND);
	m_RootSizer->Add(fromToSizer, 0, wxEXPAND);
	m_RootSizer->Add(stateBox, 0, wxEXPAND);
	m_RootSizer->Add(dump);
	m_RootSizer->Add(insert);

	wxBoxSizer* globalSizer = new wxBoxSizer(wxVERTICAL);
	globalSizer->Add(m_RootSizer, 1, wxEXPAND | wxALL, 4);

	globalSizer->SetSizeHints(this);
	SetSizer(globalSizer);

	m_RootSizer->Show(1, false);
}

void InsertDumpDialog::OnRangeTypeChanged(wxCommandEvent& event)
{			
	m_RootSizer->Show(1, m_RootSizer->GetItem(1)->IsShown());

	Layout();
	event.Skip();
}

void InsertDumpDialog::OnStateChanged(wxCommandEvent& event)
{
	m_Translated = (bool)event.GetSelection();

	event.Skip();
}

void InsertDumpDialog::OnDumpClick(wxCommandEvent& event)
{
	if (m_Translated)
	{
		if (wxMessageDialog(this, "This will replace the translated script, if any. Are you sure?", "Huh?", wxYES_NO).ShowModal() != wxID_YES)
		{
			event.Skip();
			return;
		}
	}

	m_Editor.Dump(m_Translated);

	event.Skip();
}

void InsertDumpDialog::OnInsertClick(wxCommandEvent& event)
{

	event.Skip();
}