#pragma once

#include <wx/wx.h>
#include <wx/filepicker.h>
#include "class_spell_checker.hpp"

class RemoveDictionaryDialog : public wxDialog
{
public:
	RemoveDictionaryDialog(const wxArrayInt& removed);
private:
	std::vector<wxCheckBox*> m_CheckBoses;
	const wxArrayInt& m_RemovedDics;
public:
	wxArrayInt GetSelections();
private:
	void CreateGUIControls();
};

class SpellCheckerDialog : public wxDialog
{
public:
	SpellCheckerDialog();
private:
	wxTextCtrl* m_pDictionariesDir;	
	wxArrayInt m_RemovedDics;
	wxString m_SelectedDic;
	bool m_Applied = false;

	void Apply();
private:
	void OnResetDefaultPath(wxCommandEvent& event);
	void OnRemoveDictionary(wxCommandEvent& event);
	void OnSelectDictionary(wxCommandEvent& event);	
	void OnApply(wxCommandEvent& event);	
private:
	void CreateGUIControls();
};