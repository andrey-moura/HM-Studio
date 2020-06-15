#include "frame_letter_editor.hpp"

LetterEditorFrame::LetterEditorFrame(const id& id) : m_Editor(id), EditorFrame("Letter Editor", &m_Editor)
{
	CreateGUIControls();
}

LetterEditorFrame::~LetterEditorFrame()
{
}


void LetterEditorFrame::UpdateText()
{	
	m_TextOriginal->SetText(m_Editor.GetRom(true).ReplaceWideChars(m_Editor.GetText(false)));
	m_TextTranslated->SetText(m_Editor.GetRom(true).ReplaceWideChars(m_Editor.GetText(true)));

	UpdateIndex();
}

void LetterEditorFrame::OnSaveFile()
{
	wxString text = m_TextTranslated->GetText();
	m_Editor.GetRom(false).ReplaceWideChars(text);

	Moon::File::WriteAllText(m_Editor.GetPath(true), text.ToStdString());
}

void LetterEditorFrame::OnDumpClick(wxCommandEvent& event)
{
	InsertDumpDialog(&m_Editor).ShowModal();

	event.Skip();
}

void LetterEditorFrame::CreateGUIControls()
{	
	wxMenu* menuLetter = new wxMenu();
	Bind(wxEVT_MENU, &LetterEditorFrame::OnGoFileClick, this, menuLetter->Append(wxID_OPEN, "Open\tCtrl-O")->GetId());
	Bind(wxEVT_MENU, &LetterEditorFrame::OnPrevFileClick, this, menuLetter->Append(wxNewId(), "Previous\tCtrl-Left")->GetId());
	Bind(wxEVT_MENU, &LetterEditorFrame::OnProxFileClick, this, menuLetter->Append(wxNewId(), "Next\tCtrl-Right")->GetId());
	Bind(wxEVT_MENU, &LetterEditorFrame::OnSaveFileClick, this, menuLetter->Append(wxID_SAVE, "Save\tCtrl-S")->GetId());

	wxMenu* menuTools = new wxMenu();
	menuTools->Bind(wxEVT_MENU, &LetterEditorFrame::OnDumpClick, this, menuTools->Append(wxNewId(), "Dump/Insert")->GetId());

	wxMenuBar* menuBar = new wxMenuBar();	
	menuBar->Append(menuLetter, "Letter");
	menuBar->Append(menuTools, "Tools");

	SetMenuBar(menuBar);

	CreateMyToolBar();
	CreateMyStatusBar(1);

	m_TextTranslated = new STC(this, wxID_ANY);
	m_TextOriginal = new STC(this, wxID_ANY);
	int eolMode = m_Editor.GetRom(false).GetEOL() == 0x0a ? wxSTC_EOL_LF : wxSTC_EOL_CRLF;

	m_TextTranslated->SetMaxLineLenght(m_Editor.GetRom(false).GetLineMax());
	m_TextTranslated->SetEOLMode(eolMode);

	m_TextOriginal->SetMaxLineLenght(m_Editor.GetRom(false).GetLineMax());
	m_TextOriginal->SetEOLMode(eolMode);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
	rootSizer->Add(m_TextTranslated, 1, wxEXPAND, 0);
	rootSizer->Add(m_TextOriginal, 1, wxEXPAND, 0);

	SetSizer(rootSizer);
}
