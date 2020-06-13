#include "frame_letter_editor.hpp"

LetterEditorFrame::LetterEditorFrame(const id& id) : m_Editor(id), wxFrame(nullptr, wxID_ANY, "Letter Editor")
{
	CreateGUIControls();
}

LetterEditorFrame::~LetterEditorFrame()
{
}


void LetterEditorFrame::UpdateText()
{
	m_TextOriginal->SetText(m_Editor.GetText(false));
	m_TextTranslated->SetText(m_Editor.GetText(true));

}

void LetterEditorFrame::OnOpenClick(wxCommandEvent& event)
{
	wxTextEntryDialog dialog(nullptr, "Choose a number");

	if (dialog.ShowModal() != wxID_CANCEL)
	{
		uint32_t number = std::stoi(dialog.GetValue().ToStdString());
		if (m_Editor.Open(number))
		{
			UpdateText();
		}
	}

	event.Skip();
}

void LetterEditorFrame::OnDumpClick(wxCommandEvent& event)
{
	InsertDumpDialog(&m_Editor).ShowModal();	

	event.Skip();
}

void LetterEditorFrame::OnSaveClick(wxCommandEvent& event)
{
	m_Editor.SaveToFile();

	event.Skip();
}

void LetterEditorFrame::CreateGUIControls()
{
	wxMenu* menuFile = new wxMenu();
	menuFile->Bind(wxEVT_MENU, &LetterEditorFrame::OnOpenClick, this, menuFile->Append(wxNewId(), "Open")->GetId());
	menuFile->Bind(wxEVT_MENU, &LetterEditorFrame::OnSaveClick, this, menuFile->Append(wxNewId(), "Save")->GetId());
	
	wxMenu* menuTools = new wxMenu();
	menuTools->Bind(wxEVT_MENU, &LetterEditorFrame::OnDumpClick, this, menuTools->Append(wxNewId(), "Dump/Insert")->GetId());	

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, "File");
	menuBar->Append(menuTools, "Tools");

	SetMenuBar(menuBar);

	m_TextTranslated = new STC(this, wxID_ANY);
	m_TextOriginal = new STC(this, wxID_ANY);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
	rootSizer->Add(m_TextTranslated, 1, wxEXPAND, 0);
	rootSizer->Add(m_TextOriginal, 1, wxEXPAND, 0);

	SetSizer(rootSizer);
}
