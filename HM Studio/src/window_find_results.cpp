#include "window_find_results.hpp"

wxDEFINE_EVENT(EVT_FINDRESULT_CLICK, wxCommandEvent);

void SearchResult::AppendResult(const wxString& result)
{
	m_Hits.push_back(result);
}

void FilesResults::AppendMatch(const SearchResult& result)
{
	m_Matches.push_back(result);
}

FindResultsWindow::FindResultsWindow(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	CreateGUIControls();	
}

void FindResultsWindow::SetFindResults(const FilesResults& result)
{
	m_Results = result;

	m_ListView->ClearAll();

	m_ListView->AppendText(m_Results.GetSearchTitle() + "\n");
	m_ListView->MarkerAdd(0, MARK_SUBTITLE);
	m_ListView->StartStyling(0);
	m_ListView->SetStyling(m_ListView->GetLineLength(0), STYLE_SUBTITLE);

	size_t curLine = 1;
	
	for (size_t i = 0; i < result.GetCount(); ++i)
	{
		SearchResult& match = m_Results[i];		
		
		m_ListView->AppendText("  " + match.GetTitle() + "\n");
		m_ListView->MarkerAdd(curLine, MARK_RESULTTITLE);
		m_ListView->StartStyling(m_ListView->PositionFromLine(curLine));
		m_ListView->SetStyling(m_ListView->GetLineLength(curLine), STYLE_RESULTTITLE);
		++curLine;

		for (const wxString& r : match.GetHits())
		{
			m_ListView->AppendText("    " + r + "\n");
						
			m_ListView->StartStyling(m_ListView->PositionFromLine(curLine));
			m_ListView->SetStyling(m_ListView->GetLineLength(curLine), 0);

			++curLine;
		}
	}

	m_ListView->DeleteRange(m_ListView->GetTextLength() - 1, 1);
	m_ListView->SetEditable(false);
}

void FindResultsWindow::OnDoubleClick(wxStyledTextEvent& event)
{	
	size_t line = event.GetLine();
	size_t curLine = 1;

	for (size_t i = 0; i < m_Results.GetCount(); ++i)
	{
		curLine++;

		const SearchResult& result = m_Results[i];

		size_t index = 0;

		for (const wxString& r : result.GetHits())
		{
			if (curLine == line)
				SendClickEvent(&result, index);

			++index;
			curLine++;
		}
	}	
}

void FindResultsWindow::SendClickEvent(const SearchResult* result, size_t index)
{
	wxCommandEvent event(EVT_FINDRESULT_CLICK, GetId());

	event.SetEventObject(this);
	event.SetClientData((void*)result);
	event.SetInt(index);
	
	ProcessWindowEvent(event);
}

void FindResultsWindow::CreateGUIControls()
{
	m_ListView = new wxStyledTextCtrl(this, wxID_ANY);
	m_ListView->SetLexer(wxSTC_LEX_CONTAINER);

	m_ListView->MarkerDefine(MARK_SUBTITLE, wxSTC_MARK_BACKGROUND, wxNullColour, BACKGROUND_SUBTITLE);
	m_ListView->MarkerDefine(MARK_RESULTTITLE, wxSTC_MARK_BACKGROUND, wxNullColour, BACKGROUND_RESULTTITLE);
	
	m_ListView->StyleSetForeground(STYLE_SUBTITLE, FOREGROUND_SUBTITLE);
	m_ListView->StyleSetFont(STYLE_SUBTITLE, Studio::GetDefaultFont());
	//m_ListView->StyleSetBold(STYLE_SUBTITLE, true);

	m_ListView->StyleSetForeground(STYLE_RESULTTITLE, FOREGROUND_RESULTTITLE);
	m_ListView->StyleSetFont(STYLE_RESULTTITLE, Studio::GetDefaultFont());
	//m_ListView->StyleSetBold(STYLE_RESULTTITLE, true);

	m_ListView->Bind(wxEVT_STC_DOUBLECLICK, &FindResultsWindow::OnDoubleClick, this);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
	rootSizer->Add(m_ListView, 1, wxEXPAND, 0);

	SetSizer(rootSizer);
}