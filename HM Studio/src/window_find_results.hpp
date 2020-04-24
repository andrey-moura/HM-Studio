#pragma once

#include <vector>

#include <wx/window.h>
#include <wx/stc/stc.h>
#include <wx/sizer.h>

#include "Studio.h"

#define BACKGROUND_SUBTITLE wxColour(187, 187, 255)
#define FOREGROUND_SUBTITLE wxColour(0, 0, 128)

#define BACKGROUND_RESULTTITLE wxColour(213, 255, 213)
#define FOREGROUND_RESULTTITLE wxColour(0, 128, 0)

#define STYLE_SUBTITLE 20
#define STYLE_RESULTTITLE 21

#define MARK_SUBTITLE 20
#define MARK_RESULTTITLE 21

struct SearchResult
{
private:
	wxString m_Title;
	wxArrayString m_Hits;
public:
	void AppendResult(const wxString& result);
	size_t GetCount() const { return m_Hits.GetCount(); }

	const wxString& GetTitle() const { return m_Title; }
	const wxArrayString& GetHits() const { return m_Hits; }

	void SetTitle(const wxString& title) { m_Title = title; }	
};

struct FilesResults
{
private:
	wxString m_SearchTitle;
	std::vector<SearchResult> m_Matches;
public:
	void AppendMatch(const SearchResult& result);
	size_t GetCount() const { return m_Matches.size(); }
	void SetSearchTitle(const wxString& title) { m_SearchTitle = title; }
	wxString GetSearchTitle() const { return m_SearchTitle; }

	SearchResult& operator[](size_t i) { return m_Matches.at(i); }
};

wxDECLARE_EVENT(EVT_FINDRESULT_CLICK, wxCommandEvent);

class FindResultsWindow : public wxWindow
{
public:
	FindResultsWindow(wxWindow* parent);
private:
	wxStyledTextCtrl* m_ListView = nullptr;

	FilesResults m_Results;

	void OnDoubleClick(wxStyledTextEvent& event);
	void SendClickEvent(const SearchResult* result, size_t index);
public:
	void SetFindResults(const FilesResults& result);
private:
	void CreateGUIControls();
};

