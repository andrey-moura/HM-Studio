#pragma once

#include <wx/wx.h>
#include <moon/file.hpp>
#include <string>
#include <wx/notebook.h>

class FrameSearchScript : public wxDialog
{
public:
	FrameSearchScript();
	~FrameSearchScript();
private:
	std::string m_Search;
	std::string m_Replace;	
	bool m_Find = false;	
public:
	bool Find() { return m_Find; }	
	bool Extended() { return m_pModeExtended->GetValue(); }
	bool InScript() { return m_pInScript->GetValue(); }
	bool Translated() { return m_pTranslated->GetValue(); }
	bool UseTable() { return m_pUseTable->GetValue(); }
	void InScript(bool inScript);

	std::string GetSearch() { return m_pInputFind->GetValue().ToStdString(); }
	std::string GetReplace() { return m_pInputReplace->GetValue().ToStdString(); }
private:
	void OnFindAllClick(wxCommandEvent& event);
	void OnReplaceAllClick(wxCommandEvent& event);	
	void OnInScriptClick(wxCommandEvent& event);
private:
	void CreateGUIControls();;

	//-----------------------//
	
	wxTextCtrl* m_pInputFind = nullptr;
	
	wxTextCtrl* m_pInputReplace = nullptr;
	
	wxRadioButton* m_pModeExtended = nullptr;

	wxCheckBox* m_pCase = nullptr;
	wxCheckBox* m_pUseTable = nullptr;
	wxCheckBox* m_pInScript = nullptr;
	wxCheckBox* m_pTranslated = nullptr;
};

