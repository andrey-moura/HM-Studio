#include "frame_letter_editor.hpp"

LetterEditorFrame::LetterEditorFrame(const id& id) : m_Editor(id), wxFrame(nullptr, wxID_ANY, "Letter Editor")
{
	CreateGUIControls();
}

LetterEditorFrame::~LetterEditorFrame()
{
}

void LetterEditorFrame::OnOpenClick(wxCommandEvent& event)
{
	wxTextEntryDialog dialog(nullptr, "Choose a number");

	if (dialog.ShowModal() != wxID_CANCEL)
	{
	
	}

	event.Skip();
}

void LetterEditorFrame::OnDumpClick(wxCommandEvent& event)
{
	

	m_Editor.Dump(false);

	event.Skip();
}

void LetterEditorFrame::CreateGUIControls()
{
	wxMenu* menuFile = new wxMenu();
	menuFile->Append(wxID_OPEN, "Open");	
	
	wxMenu* menuTools = new wxMenu();
	menuTools->Bind(wxEVT_MENU, &LetterEditorFrame::OnDumpClick, this, menuTools->Append(wxNewId(), "Dumpper")->GetId());

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, "File");
	menuBar->Append(menuTools, "Tools");

	SetMenuBar(menuBar);
}
