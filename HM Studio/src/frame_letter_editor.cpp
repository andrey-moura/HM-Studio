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
	const wchar_t* data = text.wc_str();

	m_Editor.GetRom(true).ReplaceWideChars(text);

	Moon::File::WriteAllText(m_Editor.GetPath(true), text.ToStdString());
}

void LetterEditorFrame::FindText(const wxString& search, bool useTable, bool translated)
{
	STC* stc = translated ? m_TextTranslated : m_TextOriginal;
	
	size_t pos = stc->FindText(0, stc->GetTextLength(), search);

	//while (pos != std::string::npos)
	//{
	//	
	//}
}

void LetterEditorFrame::ReplaceTxt(const wxString& search, const wxString& replace, bool useTable)
{
	wxString text = m_TextTranslated->GetText();
	text.Replace(search, replace, true);
	m_TextTranslated->SetText(text);
}

void LetterEditorFrame::RemovePaddingSpaces()
{
	for (size_t i = 0; i < m_TextTranslated->GetLineCount(); ++i)
	{
		wxString line = m_TextTranslated->GetLineText(i);

		if (line.ends_with(L' '))
		{
			line.RemoveLast();

			while (line.ends_with(L' '))
			{
				line.RemoveLast();
			}

			m_TextTranslated->ReplaceLineText(line, i);
		}
	}
}

void LetterEditorFrame::OnDumpClick(wxCommandEvent& event)
{
	InsertDumpDialog(&m_Editor).ShowModal();

	event.Skip();
}

void LetterEditorFrame::STCKeyDown(wxKeyEvent& event)
{	
	int keyCode = event.GetKeyCode();

	if (event.GetModifiers() != wxMOD_CONTROL || m_Garbage || keyCode == wxKeyCode::WXK_CONTROL)
	{
		event.Skip();
		return;
	}	
	if (keyCode >= WXK_NUMPAD0)
	{
		if (keyCode <= WXK_NUMPAD9)
		{
			m_TypingControl.append(1, (char)(keyCode - 276));
			m_AlreadyTyping = true;
		}
	}
	else if (keyCode >= '0' && keyCode <= '9')
	{
		m_TypingControl.append(1, (char)keyCode);
		m_AlreadyTyping = true;
	}
	else
	{
		m_Garbage = true;
	}

	event.Skip();
}

void LetterEditorFrame::STCKeyUp(wxKeyEvent& event)
{
	int keyCode = event.GetKeyCode();

	if (keyCode != WXK_CONTROL || !m_TypingControl.size())
	{
		event.Skip();
		return;
	}

	m_Garbage = false;
	long line = 0;
	if (!m_TypingControl.ToLong(&line))
	{
		event.Skip();
		return;
	}

	if (line == 0)
	{
		wxMessageBox(wxT("Please, do not use 0 based line index."), wxT("Huh?"), wxICON_WARNING);
		line = 1;
	}

	--line;

	if (m_TextOriginal->GetLineCount() >= line)
	{		
		m_TextTranslated->ReplaceLineText(m_TextOriginal->GetLineText(line), m_TextTranslated->GetCurrentLine());
		m_TextTranslated->LineEnd();
	}

	m_TypingControl.clear();
	event.Skip();
}

void LetterEditorFrame::CreateGUIControls()
{	
	//wxMenu* menuLetter = new wxMenu();
	//Bind(wxEVT_MENU, &LetterEditorFrame::OnGoFileClick, this, menuLetter->Append(wxID_OPEN, "Open\tCtrl-O")->GetId());
	//Bind(wxEVT_MENU, &LetterEditorFrame::OnPrevFileClick, this, menuLetter->Append(wxNewId(), "Previous\tCtrl-Left")->GetId());
	//Bind(wxEVT_MENU, &LetterEditorFrame::OnProxFileClick, this, menuLetter->Append(wxNewId(), "Next\tCtrl-Right")->GetId());
	//Bind(wxEVT_MENU, &LetterEditorFrame::OnSaveFileClick, this, menuLetter->Append(wxID_SAVE, "Save\tCtrl-S")->GetId());

	//wxMenu* menuTools = new wxMenu();
	//menuTools->Bind(wxEVT_MENU, &LetterEditorFrame::OnDumpClick, this, menuTools->Append(wxNewId(), "Dump/Insert")->GetId());

	//wxMenuBar* menuBar = new wxMenuBar();	
	//menuBar->Append(menuLetter, "Letter");
	//menuBar->Append(menuTools, "Tools");

	//SetMenuBar(menuBar);

	CreateMyMenuBar();
	CreateSearchMenu();
	CreateToolsMenu();
	CreateMyToolBar();
	CreateMyStatusBar(1);

	m_TextTranslated = new STC(this, wxID_ANY);
	m_TextOriginal = new STC(this, wxID_ANY);
	int eolMode = m_Editor.GetRom(false).GetEOL() == 0x0a ? wxSTC_EOL_LF : wxSTC_EOL_CRLF;

	m_TextTranslated->SetMaxLineLenght(m_Editor.GetRom(false).GetLineMax());
	m_TextTranslated->SetEOLMode(eolMode);

	m_TextOriginal->SetMaxLineLenght(m_Editor.GetRom(false).GetLineMax());
	m_TextOriginal->SetEOLMode(eolMode);

	m_TextTranslated->Bind(wxEVT_KEY_DOWN, &LetterEditorFrame::STCKeyDown, this);
	m_TextTranslated->Bind(wxEVT_KEY_UP, &LetterEditorFrame::STCKeyUp, this);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
	rootSizer->Add(m_TextTranslated, 1, wxEXPAND, 0);
	rootSizer->Add(m_TextOriginal, 1, wxEXPAND, 0);

	SetSizer(rootSizer);
}
