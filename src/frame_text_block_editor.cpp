#include "frame_text_block_editor.hpp"

TextBlockEditor::TextBlockEditor(const id i) : wxFrame(nullptr, wxID_ANY, "Text block editor"), m_RomTranslated(i, true), m_RomOriginal(i, false)
{
	CreateGUIControls();
}

void TextBlockEditor::CreateGUIControls()
{
	wxMenu* fileMenu = new wxMenu();
	fileMenu->Append(wxID_OPEN, "Set block");

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, "File");
	menuBar->Bind(wxEVT_MENU, &TextBlockEditor::OnMenuClick, this);

	SetMenuBar(menuBar);

	m_pSTC = new STC(this, wxID_ANY);
	m_pSTCOriginal = new STC(this, wxID_ANY);

	m_pSTC->SetUserCanAddLine(false);
	m_pSTCOriginal->SetUserCanAddLine(false);

	m_pRootSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pRootSizer->Add(m_pSTC, 1, wxEXPAND, 0);
	m_pRootSizer->Add(m_pSTCOriginal, 1, wxEXPAND, 0);

	SetSizerAndFit(m_pRootSizer);
}

void TextBlockEditor::OnMenuClick(wxCommandEvent& event)
{
	if (event.GetId() == wxID_OPEN)
	{
		SetBlockDialog dialog;
		if (dialog.ShowModal() == wxOK)
		{
			std::string();
		}
	}

	event.Skip();
}

SetBlockDialog::SetBlockDialog() : wxDialog(nullptr, wxID_ANY, "Set Block")
{
	CreateGUIControls();
}

void SetBlockDialog::OnGoButton(wxCommandEvent& event)
{
	start = std::stoi(m_pStartInput->GetValue().ToStdString());
	end = std::stoi(m_pEndInput->GetValue().ToStdString());
	pointers = std::stoi(m_pPointersInput->GetValue().ToStdString());

	EndModal(wxOK);

	event.Skip();
}

void SetBlockDialog::CreateGUIControls()
{
	wxStaticText* startLabel = new wxStaticText(this, wxID_ANY, "Start: ");
	m_pStartInput = new wxTextCtrl(this, wxID_ANY);

	wxBoxSizer* startSizer = new wxBoxSizer(wxHORIZONTAL);
	startSizer->Add(startLabel, 0, wxEXPAND);
	startSizer->Add(m_pStartInput, 0, wxEXPAND);

	wxStaticText* endLabel = new wxStaticText(this, wxID_ANY, "End: ");
	m_pEndInput = new wxTextCtrl(this, wxID_ANY);

	wxBoxSizer* endSizer = new wxBoxSizer(wxHORIZONTAL);
	endSizer->Add(endLabel, 0, wxEXPAND);
	endSizer->Add(m_pEndInput, 0, wxEXPAND);

	wxStaticText* pointersLabel = new wxStaticText(this, wxID_ANY, "Pointers: ");
	m_pPointersInput = new wxTextCtrl(this, wxID_ANY);

	wxBoxSizer* pointersSizer = new wxBoxSizer(wxHORIZONTAL);
	pointersSizer->Add(pointersLabel, 0, wxEXPAND);
	pointersSizer->Add(m_pPointersInput, 0, wxEXPAND);

	wxButton* setButton = new wxButton(this, wxID_ANY, "Go");
	setButton->Bind(wxEVT_BUTTON, &SetBlockDialog::OnGoButton, this);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	rootSizer->Add(startSizer, 0, wxEXPAND);
	rootSizer->Add(endSizer, 0, wxEXPAND);
	rootSizer->Add(pointersSizer, 0, wxEXPAND);
	rootSizer->Add(setButton, 0, wxEXPAND);

	SetSizerAndFit(rootSizer);
}