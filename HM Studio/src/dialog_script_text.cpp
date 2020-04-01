#include "dialog_script_text.hpp"

TextFromScriptDialog::TextFromScriptDialog(ScriptEditor& editor) : wxDialog(nullptr, wxID_ANY, "Select script"), m_Editor(editor)
{
	CreateGUIControls();
}

void TextFromScriptDialog::CreateGUIControls()
{
	SetBackgroundColour(Studio::GetControlBackgroundColor());
	SetForegroundColour(Studio::GetFontColour());
	SetFont(Studio::GetDefaultFont());

	wxStaticText* fromNumberLabel = new wxStaticText(this, wxID_ANY, "Number: ");

	m_pFromNumberText = new wxTextCtrl(this, wxID_ANY);
	m_pFromNumberText->SetMaxLength(4);	

	m_pFromNumberChoice = new wxChoice(this, wxID_ANY);
	m_pFromNumberChoice->AppendString("FoMT");
	m_pFromNumberChoice->AppendString("MFoMT");
	m_pFromNumberChoice->AppendString("DS");
	m_pFromNumberChoice->SetSelection((int)m_Editor.GetRom(true).Id);

	wxBoxSizer* fromNumberSizer = new wxBoxSizer(wxHORIZONTAL);
	fromNumberSizer->Add(fromNumberLabel);
	fromNumberSizer->Add(m_pFromNumberText);
	fromNumberSizer->AddSpacer(4);
	fromNumberSizer->Add(m_pFromNumberChoice);	

	wxStaticText* fromFileLabel = new wxStaticText(this, wxID_ANY, "Path:   ");
	m_pFromFilePicker = new wxFilePickerCtrl(this, wxID_ANY);	
	
	wxBoxSizer* fromFileSizer = new wxBoxSizer(wxHORIZONTAL);
	fromFileSizer->Add(fromFileLabel);
	fromFileSizer->Add(m_pFromFilePicker);	

	m_pGoButton = new wxButton(this, wxID_ANY, "Go!");
	m_pGoButton->Bind(wxEVT_BUTTON, &TextFromScriptDialog::OnGoButton, this);

	wxBoxSizer* goSizer = new wxBoxSizer(wxHORIZONTAL);
	goSizer->AddStretchSpacer(1);
	goSizer->Add(m_pGoButton);

	m_pRootSizer = new wxBoxSizer(wxVERTICAL);
	m_pRootSizer->Add(fromNumberSizer);
	m_pRootSizer->Add(fromFileSizer);
	m_pRootSizer->Add(goSizer);
	SetSizer(m_pRootSizer);
	m_pRootSizer->Fit(this);
	m_pRootSizer->SetSizeHints(this);
}

void TextFromScriptDialog::GetScriptText(const std::string& path)
{
	Script script;
	script.SetData(File::ReadAllBytes(path));

	if (script.Count() != m_ScriptCount)
	{
		wxMessageBox("Script count is not the same.", "Huh?", wxICON_ERROR | wxSTAY_ON_TOP, this);
		return;
	}
	m_Text = script.GetText();
	
	EndModal(wxOK);
}

void TextFromScriptDialog::OnGoButton(wxCommandEvent& event)
{
	std::string numberText = m_pFromNumberText->GetValue().ToStdString();
	std::string pathText = m_pFromFilePicker->GetPath().ToStdString();

	if (numberText.size() && pathText.size())
	{		
		wxMessageBox("Choose one option only.", "Huh?", wxICON_ERROR | wxSTAY_ON_TOP, this);
		return;
	}

	std::string path;

	if (numberText.size())
	{
		id i = (id)m_pFromNumberChoice->GetSelection();

		size_t number = std::stoi(numberText);

		if (m_Editor.GetRom(true).Id == i)
		{
			path = m_Editor.GetPath(number, true);
		}
		else
		{
			RomFile rom(i, true);
			RomFile romOri(i, false);
			ScriptEditor editor(romOri, rom);
			path = editor.GetPath(number, true);
		}	
	}
	else if (pathText.size())
	{
		path = m_pFromFilePicker->GetPath();
	}

	GetScriptText(path);

	event.Skip();
}