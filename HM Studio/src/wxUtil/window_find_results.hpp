#pragma once

#include <vector>

#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/window.h>
#include <wx/stc/stc.h>
#include <wx/textbuf.h>
#include <wx/bmpbuttn.h>
#include <wx/statline.h>
#include <wx/msgdlg.h>
#include <wx/event.h>

#include "../class_util_string.hpp"

#define MARKER_SUBTITLE_NUM 0
#define MARKER_SUBTITLE_BACKCOLOR wxColour(187, 187, 255)
#define STYLE_SUBTITLE_NUM 0
#define STYLE_SUBTITLE_FORECOLOR wxColour(0, 0, 128)

#define STYLE_RESULTTITLE_NUM 1
#define STYLE_RESULTTITLE_FORECOLOR wxColour(0, 128, 0)
#define MARKER_RESULTTITLE_NUM 1
#define MARKER_RESULTTITLE_BACKCOLOR wxColour(213, 255, 213)

#define MARKER_FOLDHIDE 2
#define MARKER_FOLDSHOW 3
#define MARKER_FOLDLINE 4
#define MARKER_FOLD_FOREGROUND wxColour(240, 240, 240)
#define MARKER_FOLD_BACKGROUND wxColour(255, 0, 0)

#define MARGIN_FOLD 1

#define STYLE_BACKGROUND 32

struct FindResult
{
	FindResult() = default;
	FindResult(const std::string& path, const std::vector<size_t>& hits) : m_Path(path), m_Hits(hits) {}
	size_t m_LineInEditor;
	size_t m_Id;
	std::string m_Path;
	std::vector<size_t> m_Hits;
};

wxDECLARE_EVENT(EVT_FINDRESULT_CLICK, wxCommandEvent);

class FindResultsWindow : public wxWindow
{
public:
	FindResultsWindow(wxWindow* parent, wxWindowID id = wxID_ANY);
	~FindResultsWindow() = default;

	wxWindow* m_pParent;
private:
	std::vector<FindResult> m_Results;

	size_t m_SearchFiles;	

	std::string m_Search;

	wxString m_Title;
	std::string m_SubTitle;	
	std::string m_ResultTitle;
	std::string m_ResultText;
public:
	void SetResultsInfo(const std::string& search, const size_t numSearchFiles, const std::vector<FindResult>& results);

	void SetTitle(const wxString& title);
	void SetSubTitle(const std::string& subTitle);

	void SetResultTile(const wxString& title);
	void SetResultText(const wxString& text);
	wxString GetResultTitle(const FindResult& result);
	wxString GetResultText(const FindResult& result);
private:
	void OnMouseMove(wxMouseEvent& event);
	bool m_Dragging = false;

	void OnMarginClick(wxStyledTextEvent& event);
	void OnDoubleClick(wxStyledTextEvent& event);	
	void OnCloseButton(wxCommandEvent& event);
public:
	void EventBuffer(wxCommandEvent& event) { wxMessageBox("event on window"); };
	void SendEventResultClicked(FindResult* result, size_t index);
private:
	void CreateGUIControls();
	void SetupSTC();
	void CreateTopBorder();
	wxStyledTextCtrl* m_STC = nullptr;

	wxPanel* m_pTopBorder = nullptr;
};

