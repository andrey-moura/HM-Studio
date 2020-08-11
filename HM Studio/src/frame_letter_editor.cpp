#include "frame_letter_editor.hpp"

LetterEditorFrame::LetterEditorFrame(const id& id) : EditorFrame(new LetterEditor(id))
{
	CreateGUIControls();	
}

LetterEditorFrame::~LetterEditorFrame()
{
}

void LetterEditorFrame::UpdateText()
{	
	m_TextOriginal->SetText(((LetterEditor*)m_pEditor)->GetRom(false).ReplaceWideChars(((LetterEditor*)m_pEditor)->GetText(false)));
	m_TextTranslated->SetText(((LetterEditor*)m_pEditor)->GetRom(true).ReplaceWideChars(((LetterEditor*)m_pEditor)->GetText(true)));	

	UpdateIndex();
}

void LetterEditorFrame::OnSaveFile()
{	
	wxString text = m_TextTranslated->GetText();	
	((LetterEditor*)m_pEditor)->GetRom(true).ReplaceWideChars(text);	
	m_pEditor->SaveText(text.ToStdString());
	m_pEditor->SaveFile();
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

void LetterEditorFrame::StcOnUi(wxStyledTextEvent& event)
{
	m_pEditor->SetIndex(((STC*)event.GetEventObject())->GetCurrentLine()+1);
	m_pEditor->SetCount(((STC*)event.GetEventObject())->GetLineCount());

	UpdateIndex();

	event.Skip();
}

void LetterEditorFrame::CreateGUIControls()
{
	CreateMyMenuBar();
	CreateSearchMenu();
	CreateViewMenu();
	CreateToolsMenu(true, true, true, true);
	CreateMyToolBar();
	CreateMyStatusBar();

	m_TextTranslated = new STC(this, wxID_ANY);
	m_TextOriginal = new STC(this, wxID_ANY);
	int eolMode = ((LetterEditor*)m_pEditor)->GetRom(false).GetEOL() == 0x0a ? wxSTC_EOL_LF : wxSTC_EOL_CRLF;

	m_TextTranslated->SetMaxLineLenght(((LetterEditor*)m_pEditor)->GetRom(false).GetLineMax());
	m_TextTranslated->SetEOLMode(eolMode);

	m_TextOriginal->SetMaxLineLenght(((LetterEditor*)m_pEditor)->GetRom(false).GetLineMax());
	m_TextOriginal->SetEOLMode(eolMode);
	m_TextOriginal->SetUseSpellChecker(false);

	m_TextTranslated->Bind(wxEVT_KEY_DOWN, &LetterEditorFrame::STCKeyDown, this);
	m_TextTranslated->Bind(wxEVT_KEY_UP, &LetterEditorFrame::STCKeyUp, this);
	m_TextTranslated->Bind(wxEVT_STC_UPDATEUI, &LetterEditorFrame::StcOnUi, this);

	m_TextOriginal->Bind(wxEVT_STC_UPDATEUI, &LetterEditorFrame::StcOnUi, this);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
	rootSizer->Add(m_TextTranslated, 1, wxEXPAND, 0);
	rootSizer->Add(m_TextOriginal, 1, wxEXPAND, 0);

	StatusToStc(m_TextOriginal);
	StatusToStc(m_TextTranslated);

	SetSizer(rootSizer);
}
