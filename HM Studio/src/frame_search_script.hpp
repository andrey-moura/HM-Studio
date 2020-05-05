#pragma once

#include <wx/wx.h>
#include <wx/notebook.h>

#include "Studio.h"

class FrameSearchScript : public wxDialog
{
public:
	FrameSearchScript();
	~FrameSearchScript();
private:
	std::string m_Search;
	std::string m_Replace;
	bool m_Extended = false;
	bool m_Find = false;
	bool m_InScripts = false;
	bool m_MatchCase = false;
public:
	bool Find() { return m_Find; }
	bool Replace() { return !Find(); }
	bool Extended() { return m_Extended; }
	bool InScript() { return m_InScripts; }

	const std::string& GetSearch() { return m_Search; }
	const std::string& GetReplace() { return m_Replace; }
private:
	void GetValues();
	void EndAndSkip(wxCommandEvent& event);
private:
	void OnFindAllClick(wxCommandEvent& event);
	void OnReplaceAllClick(wxCommandEvent& event);
private:
	void CreateGUIControls();;

	//-----------------------//
	
	wxTextCtrl* m_pInputFind = nullptr;
	
	wxTextCtrl* m_pInputReplace = nullptr;
	
	wxRadioButton* m_pModeExtended = nullptr;

	wxCheckBox* m_pCase = nullptr;
	wxCheckBox* m_pInScript = nullptr;
};

