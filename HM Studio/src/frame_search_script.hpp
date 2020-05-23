#pragma once

#include <wx/wx.h>
#include <string>
#include <wx/notebook.h>
#include <wx/clipbrd.h>

#include <moon/file.hpp>
#include <moon/string.hpp>

#include "class_util_string.hpp"

class FrameSearchScript : public wxDialog
{
public:
	FrameSearchScript();
	~FrameSearchScript();
private:
	bool m_Find = false;	
public:
	bool Find() { return m_Find; }	
	bool Extended() { return m_pModeBox->GetSelection(); }
	bool InScript() { return m_pInScript->GetValue(); }
	bool Translated() { return m_pTranslated->GetValue(); }
	bool UseTable() { return m_pUseTable->GetValue(); }
	void InScript(bool inScript);

	std::string m_RawEOL;
	std::string m_EOL;
	
	void SetEol(const std::string& eol, const std::string& raw) { m_EOL = eol; m_RawEOL = raw; }

	std::string GetSearch();
	std::string GetReplace();
	
private:
	void OnFindAllClick(wxCommandEvent& event);
	void OnReplaceAllClick(wxCommandEvent& event);	
	void OnInScriptClick(wxCommandEvent& event);
	void OnText(wxCommandEvent& event);
	void OnTextEnter(wxCommandEvent& event);
private:
	void CreateGUIControls();;

	//-----------------------//
	
	wxTextCtrl* m_pInputFind = nullptr;
	
	wxTextCtrl* m_pInputReplace = nullptr;
	
	wxRadioBox* m_pModeBox = nullptr;

	wxCheckBox* m_pCase = nullptr;
	wxCheckBox* m_pUseTable = nullptr;
	wxCheckBox* m_pInScript = nullptr;
	wxCheckBox* m_pTranslated = nullptr;
};

