#include "window_find_results.hpp"

wxDEFINE_EVENT(EVT_FINDRESULT_CLICK, wxCommandEvent);

FindResultsWindow::FindResultsWindow(wxWindow* parent, wxWindowID id) : //wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	wxWindow(parent, id)
{			
	m_pParent = parent;
	//Bind(wxEVT_MY_CUSTOM_COMMAND, &FindResultsWindow::EventBuffer, this);
	CreateGUIControls();
}

void FindResultsWindow::SetResultsInfo(const std::string& search, const size_t numSearchFiles, const std::vector<FindResult>& results)
{	
	m_Results = results;
	m_Search = search;
	m_SearchFiles = numSearchFiles;

	if (m_SubTitle.size() == 0)
		SetSubTitle("Search \"<S>\" (<H> hits in <F> files)");

	if (m_ResultTitle.size() == 0)
		SetResultTile("  <FileName> (<Hits> hit/s)");

	if (m_ResultText.size() == 0)
		SetResultText("   Line <Index>: ");

	wxString eol = wxTextBuffer::GetEOL();

	wxString s;

	s << eol;

	//m_STC->StartStyling(m_STC->GetTextLength());	

	for (int i = 0; i < m_Results.size(); ++i)
	{
		wxString thisText = GetResultTitle(m_Results[i]);
		s << thisText;

		s << GetResultText(m_Results[i]);
	}

	m_STC->AppendText(s);

	size_t curLine = 1;

	for (int i = 0; i < results.size(); ++i)
	{
		m_STC->MarkerAdd(curLine, MARKER_RESULTTITLE_NUM);
		m_STC->MarkerAdd(curLine, MARKER_FOLDHIDE);

		m_Results[i].m_LineInEditor = curLine;

		for (int x = 0; x < results[i].m_Hits.size(); ++x)
		{
			curLine++;
			m_STC->MarkerAdd(curLine, MARKER_FOLDLINE);
		}

		++curLine;		
	}
}

void FindResultsWindow::SetTitle(const wxString& title)
{
	//m_DataView->SetLabel(title);
}

void FindResultsWindow::SetSubTitle(const std::string& subTitle)
{
	m_SubTitle = subTitle;

	StringUtil::Replace("<S>", m_Search, m_SubTitle);
	StringUtil::Replace("<H>", std::to_string(m_Results.size()), m_SubTitle);
	StringUtil::Replace("<F>", std::to_string(m_SearchFiles), m_SubTitle);

	bool needToSetText = (m_STC->GetTextLength() > 0);

	m_STC->SetText(m_SubTitle);
	m_STC->MarkerAdd(0, MARKER_SUBTITLE_NUM);
	m_STC->MarkerAdd(0, MARKER_FOLDHIDE);

	//if(needToSetText)
		//ToDo Update the text
}

void FindResultsWindow::SetResultTile(const wxString& title)
{
	m_ResultTitle = title;	
}

void FindResultsWindow::SetResultText(const wxString& text)
{
	m_ResultText = text;
}

void FindResultsWindow::OnMarginClick(wxStyledTextEvent& event)
{
	if (event.GetMargin() == MARGIN_FOLD)
	{
		size_t line = m_STC->LineFromPosition(event.GetPosition());

		if (line == 0)
		{															
			//m_STC->HideLines(0, m_STC->LineFromPosition(m_STC->GetTextLength()));
			//wxMessageBox(std::to_string(line));
		}
	}		
	event.Skip();
}

void FindResultsWindow::OnDoubleClick(wxStyledTextEvent& event)
{
	size_t line = m_STC->LineFromPosition(event.GetPosition());

	if (line != 0)
	{
		size_t index = 0;
		for (int i = 0; i < m_Results.size(); ++i)
		{
			bool flag = false;
			size_t curLine = m_Results[i].m_LineInEditor;

			if (line == curLine)
				flag = true;
			else
				for (int x = 0; x < m_Results[i].m_Hits.size(); ++x)
				{
					++curLine;
					if (line == curLine)
					{
						flag = true;
						index = m_Results[i].m_Hits[x];
						break;
					}
				}

			if (flag)
			{
				SendEventResultClicked(&m_Results[i], index);
				break;
			}
		}
	}

	event.Skip();
}

void FindResultsWindow::OnCloseButton(wxCommandEvent& event)
{
	this->Hide();
	m_Results.clear();
	m_Search.clear();
	m_Title.clear();
	m_SubTitle.clear();
	m_ResultTitle.clear();
	m_ResultText.clear();
	m_pParent->Layout();
}

void FindResultsWindow::SendEventResultClicked(FindResult* result, size_t index)
{
	wxCommandEvent eventFindResultClick(EVT_FINDRESULT_CLICK, GetId());
	eventFindResultClick.SetEventObject(this);
	eventFindResultClick.SetClientData(result);
	eventFindResultClick.SetInt(index);
	eventFindResultClick.SetString(m_Search);
	ProcessWindowEvent(eventFindResultClick);
}

void FindResultsWindow::CreateGUIControls()
{
	SetupSTC();
	//m_STC->StyleSetBackground(STYLE_RESULTTITLE_NUM, STYLE_RESULTTITLE_BACKCOLOR);
	//m_STC->StyleSetForeground(STYLE_RESULTTITLE_NUM, STYLE_RESULTTITLE_FORECOLOR);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	CreateTopBorder();
	rootSizer->Add(m_pTopBorder, 0, wxEXPAND, 0);
	rootSizer->Add(m_STC, 0, wxEXPAND, 0);

	SetSizer(rootSizer);

	rootSizer->SetSizeHints(this);
	rootSizer->Fit(this);
}

void FindResultsWindow::SetupSTC()
{
	m_STC = new wxStyledTextCtrl(this, wxID_ANY);
	//m_STC->SetEditable(false);

	m_STC->SetLexer(wxSTC_LEX_CONTAINER);

	m_STC->SetMarginType(MARGIN_FOLD, wxSTC_MARGIN_SYMBOL);

	m_STC->MarkerDefine(MARKER_SUBTITLE_NUM, wxSTC_MARK_BACKGROUND, wxNullColour, MARKER_SUBTITLE_BACKCOLOR);
	m_STC->MarkerDefine(MARKER_RESULTTITLE_NUM, wxSTC_MARK_BACKGROUND, wxNullColour, MARKER_RESULTTITLE_BACKCOLOR);

	m_STC->MarkerDefine(MARKER_FOLDHIDE, wxSTC_MARK_BOXMINUSCONNECTED, MARKER_FOLD_FOREGROUND, MARKER_FOLD_BACKGROUND);
	m_STC->MarkerDefine(MARKER_FOLDSHOW, wxSTC_MARK_BOXPLUSCONNECTED , MARKER_FOLD_FOREGROUND, MARKER_FOLD_BACKGROUND);
	m_STC->MarkerDefine(MARKER_FOLDLINE, wxSTC_MARK_VLINE            , MARKER_FOLD_FOREGROUND, MARKER_FOLD_BACKGROUND);

	m_STC->SetMarginSensitive(MARGIN_FOLD, true);

	m_STC->Bind(wxEVT_STC_MARGINCLICK, &FindResultsWindow::OnMarginClick, this);
	m_STC->Bind(wxEVT_STC_DOUBLECLICK, &FindResultsWindow::OnDoubleClick, this);	
}

void FindResultsWindow::CreateTopBorder()
{
	uint8_t* rgbData = new uint8_t[363] { 0xb5, 0x9f, 0xc0, 0xb2, 0x8d, 0xab, 0xb0, 0x83, 0x9f, 0xaf, 0x7f, 0x9b, 0xaf, 0x7b, 0x97, 0xaf, 0x79, 0x95, 0xae, 0x78, 0x93, 0xad, 0x74, 0x8f, 0xac, 0x6f, 0x89, 0xaa, 0x67, 0x81, 0xa7, 0x81, 0xa2, 0xb2, 0x8f, 0xae, 0xb3, 0x97, 0xb7, 0xb1, 0x8c, 0xab, 0xb0, 0x85, 0xa3, 0xb0, 0x81, 0x9f, 0xaf, 0x7e, 0x9b, 0xaf, 0x7c, 0x9a, 0xae, 0x79, 0x95, 0xad, 0x73, 0x8f, 0xab, 0x6b, 0x86, 0x9b, 0x5f, 0x7b, 0xb0, 0x85, 0xa2, 0xb2, 0x8d, 0xac, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xae, 0x79, 0x97, 0xae, 0x75, 0x91, 0xad, 0x71, 0x8c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xab, 0x69, 0x84, 0x9b, 0x61, 0x7d, 0xb0, 0x7f, 0x9c, 0xb1, 0x86, 0xa3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xae, 0x71, 0x8c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xab, 0x69, 0x83, 0x9d, 0x63, 0x7f, 0xaf, 0x7b, 0x97, 0xb0, 0x81, 0x9e, 0xb0, 0x7d, 0x9b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xad, 0x67, 0x7e, 0xac, 0x69, 0x80, 0x9f, 0x62, 0x7e, 0xaf, 0x78, 0x93, 0xb0, 0x7d, 0x9b, 0xb0, 0x7d, 0x9a, 0xaf, 0x79, 0x96, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xae, 0x6a, 0x80, 0xae, 0x69, 0x7d, 0xad, 0x68, 0x7e, 0x9e, 0x61, 0x7c, 0xae, 0x74, 0x8d, 0xaf, 0x7b, 0x97, 0xb0, 0x7c, 0x99, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb1, 0x6b, 0x7d, 0xaf, 0x68, 0x7c, 0xa1, 0x61, 0x7a, 0xae, 0x70, 0x88, 0xaf, 0x78, 0x92, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb1, 0x78, 0x8d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb0, 0x68, 0x78, 0xa2, 0x60, 0x76, 0xae, 0x6e, 0x84, 0xb0, 0x76, 0x8d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb1, 0x79, 0x90, 0xb2, 0x77, 0x8b, 0xb3, 0x75, 0x86, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xb1, 0x67, 0x75, 0xa3, 0x5f, 0x72, 0xac, 0x6b, 0x7e, 0xae, 0x71, 0x86, 0xaf, 0x76, 0x8c, 0xaf, 0x76, 0x8c, 0xaf, 0x75, 0x8a, 0xb0, 0x74, 0x87, 0xb1, 0x72, 0x83, 0xb2, 0x6e, 0x7d, 0xb2, 0x6a, 0x78, 0xaf, 0x65, 0x73, 0x9f, 0x5d, 0x72, 0xac, 0x8c, 0xa6, 0x9e, 0x67, 0x7d, 0x9f, 0x6b, 0x80, 0x9f, 0x6b, 0x81, 0xa0, 0x6a, 0x80, 0xa1, 0x6a, 0x7e, 0xa4, 0x69, 0x7b, 0xa4, 0x66, 0x79, 0xa3, 0x63, 0x75, 0xa1, 0x60, 0x73, 0xa6, 0x84, 0xa1 };

	uint8_t* alphaData = new uint8_t[121] { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	m_pTopBorder = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_SUNKEN);
	m_pTopBorder->Bind(wxEVT_MOTION, &FindResultsWindow::OnMouseMove, this);
	
	wxBitmapButton* closeButton = new wxBitmapButton(m_pTopBorder, wxID_ANY, wxImage(11, 11, rgbData, alphaData, false));	
	closeButton->Bind(wxEVT_BUTTON, &FindResultsWindow::OnCloseButton, this);

	wxBoxSizer* topBorderSizer = new wxBoxSizer(wxHORIZONTAL);	
	topBorderSizer->AddStretchSpacer(1);
	topBorderSizer->Add(closeButton, 0, wxSHAPED, 0);

	m_pTopBorder->SetSizer(topBorderSizer);
}

wxString FindResultsWindow::GetResultTitle(const FindResult& result)
{
	std::string output = m_ResultTitle;

	StringUtil::Replace("<FileName>", result.m_Path, output);
	StringUtil::Replace("<Hits>", std::to_string(result.m_Hits.size()), output);

	return wxString(output) << wxString(wxTextBuffer::GetEOL());
}

wxString FindResultsWindow::GetResultText(const FindResult& result)
{
	wxString output;
	wxString eol = wxTextBuffer::GetEOL();

	for (int i = 0; i < result.m_Hits.size(); ++i)
	{
		std::string text = m_ResultText;
		StringUtil::Replace("<Index>", std::to_string(result.m_Hits[i]), text);

		output << text << eol;
	}

	return output;
}

void FindResultsWindow::OnMouseMove(wxMouseEvent& event)
{
	//size_t y = wxGetMousePosition().y;

	//std::string xs = std::to_string(wxGetMousePosition().x);
	//std::string ys = std::to_string(y);

	//size_t borderY = m_pTopBorder->GetPosition().y + this->GetPosition().y; 

	//ys += " \nm_pTopBorder.y = " + std::to_string(borderY);


	////Lets the user know that can resize the window
	//if (y == borderY && !m_Dragging) {
	//	m_pTopBorder->SetCursor(wxCursor(wxCURSOR_SIZENS));		
	//}
	//else if (y > 3 && !m_Dragging) {
	//	m_pTopBorder->SetCursor(wxCursor(wxCURSOR_DEFAULT));
	//}

	//if (event.LeftIsDown() && !m_Dragging)
	//{
	//	m_Dragging = true;
	//	ys += "\nesta";
	//}
	//else {
	//	m_Dragging = false;
	//}

	//if (m_Dragging)
	//	this->SetPosition(wxPoint(this->GetPosition().x, this->GetPosition().y + 1));

	//m_STC->SetText(wxString("X = ") << xs << " Y = " << ys);
	event.Skip();
}
