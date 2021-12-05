#include "dialog_spellchecker.hpp"
#include <wx/fileconf.h>

RemoveDictionaryDialog::RemoveDictionaryDialog(const wxArrayInt& removed) :
	wxDialog(nullptr, wxID_ANY, L"Remove Dictionaries"), m_RemovedDics(removed)
{
	CreateGUIControls();
}

wxArrayInt RemoveDictionaryDialog::GetSelections()
{
	wxArrayInt array;

	for (size_t i = 0; i < m_CheckBoses.size(); ++i)
	{
		if (m_CheckBoses[i]->IsChecked())
			array.push_back(1+i);
	}

	return array;
}

void RemoveDictionaryDialog::CreateGUIControls()
{
	wxBoxSizer* root_sizer = new wxBoxSizer(wxVERTICAL);

	root_sizer->Add(new wxStaticText(this, wxID_ANY, L"Select Dictionaries for Removal:"));
	root_sizer->AddSpacer(4);
	
	wxPanel* dictionaries_list = new wxPanel(this, wxID_ANY);	
	wxBoxSizer* dictionaries_sizer = new wxBoxSizer(wxVERTICAL);

	wxArrayString dictionaries = SpellChecker::GetDictionaries();

	for(size_t i = 1; i < dictionaries.size(); ++i)	
	{
		bool check = m_RemovedDics.Index(i) != -1;

		m_CheckBoses.push_back(new wxCheckBox(dictionaries_list, wxID_ANY, dictionaries[i]));
		dictionaries_sizer->Add(m_CheckBoses.back(), 0, wxBOTTOM, 4);
		m_CheckBoses.back()->SetValue(check);
	}

	dictionaries_list->SetSizer(dictionaries_sizer);

	root_sizer->Add(dictionaries_list);
	root_sizer->Add(CreateButtonSizer(wxCANCEL | wxOK));
	SetSizerAndFit(root_sizer);
}

SpellCheckerDialog::SpellCheckerDialog() : wxDialog(nullptr, wxID_ANY, L"SpellChecker Settings")
{
	if (!SpellChecker::m_sInitialized)
	{
		SpellChecker::Initialize();
	}

	CreateGUIControls();

	Bind(wxEVT_BUTTON, &SpellCheckerDialog::OnApply, this, wxID_OK);
	Bind(wxEVT_BUTTON, &SpellCheckerDialog::OnApply, this, wxID_APPLY);
}

void SpellCheckerDialog::Apply()
{
	bool changes = false;

	wxString dic_dir = m_pDictionariesDir->GetValue();	

	wxConfigBase* pConfig = wxConfigBase::Get();	

	if (dic_dir != SpellChecker::GetDictionariesPath())
	{
		changes = true;		
		pConfig->Write(L"/SpellChecker/DictionariesDir", dic_dir);
	}
	if (m_SelectedDic != SpellChecker::GetDictionary())
	{
		changes = true;
		pConfig->Write(L"/SpellChecker/Dictionary", m_SelectedDic);
	}
	if (!m_RemovedDics.empty())
	{
		wxFileName fn;
		fn.SetPath(SpellChecker::GetDictionariesPath());
		wxArrayString dics = SpellChecker::GetDictionaries();

		for (size_t i = 0; i < m_RemovedDics.size(); ++i)
		{
			fn.SetName(dics[m_RemovedDics[i]]);
			fn.SetExt(L"aff");

			wxRemoveFile(fn.GetFullPath());

			fn.SetExt(L"dic");

			wxRemoveFile(fn.GetFullPath());
		}

		changes = true;
	}
	
	if (changes)
		pConfig->Flush();
	
	wxFileConfig* pFile = (wxFileConfig*)wxConfigBase::Get();
	wxString file;

	wxFileName fn = pFile->GetGlobalFile(file);	

	if (changes)
		SpellChecker::Initialize();
}

void SpellCheckerDialog::OnResetDefaultPath(wxCommandEvent& event)
{
	wxFileName fileName;

	wxString exeDir = wxPathOnly(wxStandardPaths::Get().GetExecutablePath());
	fileName.SetPath(exeDir);
	fileName.AppendDir("SpellChecker");

	m_pDictionariesDir->SetValue(fileName.GetPath());

	event.Skip();
}

void SpellCheckerDialog::OnRemoveDictionary(wxCommandEvent& event)
{
	RemoveDictionaryDialog dialog(m_RemovedDics);	

	if (dialog.ShowModal() == wxID_OK)
	{
		m_RemovedDics = dialog.GetSelections();
	}	

	event.Skip();
}

void SpellCheckerDialog::OnSelectDictionary(wxCommandEvent& event)
{
	m_SelectedDic = SpellChecker::GetDictionaries()[event.GetSelection()];

	event.Skip();
}

void SpellCheckerDialog::OnApply(wxCommandEvent& event)
{
	Apply();
	event.Skip();
}

void SpellCheckerDialog::CreateGUIControls()
{
	SetBackgroundColour(wxColour(240, 240, 240, 255));

	wxDirPickerCtrl* dictionary_path = new wxDirPickerCtrl(this, wxID_ANY, SpellChecker::GetDictionariesPath(), wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL | wxDIRP_DIR_MUST_EXIST | wxDIRP_SMALL);

	m_pDictionariesDir = dictionary_path->GetTextCtrl();
	m_pDictionariesDir->SetValue(wxConfigBase::Get()->Read("DictionariesDir", L""));
	m_pDictionariesDir->SetMinSize(wxSize(235, m_pDictionariesDir->GetMinSize().GetHeight()));	

	wxStaticBoxSizer* dictionary_path_sizer = new wxStaticBoxSizer(wxVERTICAL, this, L"Dictionaries Folder");
	dictionary_path_sizer->Add(dictionary_path);
	dictionary_path_sizer->AddSpacer(4);
	dictionary_path_sizer->Add(new wxButton(this, wxID_ANY, L"Reset to Default"))->GetWindow()->Bind(wxEVT_BUTTON, &SpellCheckerDialog::OnResetDefaultPath, this);

	wxBoxSizer* dictionary_add_sizer = new wxBoxSizer(wxHORIZONTAL);
	dictionary_add_sizer->Add(new wxButton(this, wxID_ANY, L"Download..."))->GetWindow()->Enable(false);
	dictionary_add_sizer->AddStretchSpacer(1);
	dictionary_add_sizer->Add(new wxButton(this, wxID_ANY, L"Remove..."))->GetWindow()->Bind(wxEVT_BUTTON, &SpellCheckerDialog::OnRemoveDictionary, this);

	wxBoxSizer* dictionary_language = new wxBoxSizer(wxHORIZONTAL);
	dictionary_language->Add(new wxStaticText(this, wxID_ANY, L"Language:"), 0, wxALIGN_CENTER_VERTICAL, 0);
	dictionary_language->AddStretchSpacer(1);
	dictionary_language->Add(new wxComboBox(this, wxID_ANY, SpellChecker::GetDictionary(), wxDefaultPosition, wxDefaultSize, SpellChecker::GetDictionaries()))->GetWindow()->Bind(wxEVT_COMBOBOX, &SpellCheckerDialog::OnSelectDictionary, this);

	wxStaticBoxSizer* dictionary_sizer = new wxStaticBoxSizer(wxVERTICAL, this, L"Dictionary");	
	dictionary_sizer->Add(dictionary_path_sizer);
	dictionary_sizer->Add(dictionary_add_sizer, 0, wxEXPAND, 0);
	dictionary_sizer->Add(dictionary_language, 0, wxEXPAND, 0);

	wxBoxSizer* root_sizer = new wxBoxSizer(wxVERTICAL);
	root_sizer->Add(dictionary_sizer);
	root_sizer->Add(CreateButtonSizer(wxCANCEL | wxOK | wxAPPLY));	

	SetSizerAndFit(root_sizer);
}