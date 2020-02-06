#include "frame_main.hpp"

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "HM Studio")
{
	CreateGUIControls();	
}

void MainFrame::CreateGUIControls()
{	
	this->SetBackgroundColour(wxColour(240, 240, 240, 255));	

	wxStaticBox* romBox = new wxStaticBox(this, wxID_ANY, "ROM");
	wxStaticBoxSizer* romSizer = new wxStaticBoxSizer(romBox, wxVERTICAL);

	m_pOpenRom = new wxButton(this, wxID_ANY, "Open");
	m_pBackup = new wxButton(this, wxID_ANY, "Backup");

	m_pSelection = new wxChoice(this, wxID_ANY);
	m_pSelection->Insert("FoMT", 0);
	m_pSelection->Insert("MFoMT", 1);
	m_pSelection->Insert("DS", 2);
	m_pSelection->SetSelection(0);	

	m_pDefault = new wxCheckBox(this, wxID_ANY, "Default");

	romSizer->AddSpacer(4);
	romSizer->Add(m_pOpenRom, 0, wxRIGHT | wxLEFT | wxEXPAND, 5);
	romSizer->AddSpacer(4);
	romSizer->Add(m_pBackup, 0, wxRIGHT | wxLEFT | wxEXPAND, 5);
	romSizer->AddSpacer(4);
	romSizer->Add(m_pSelection, 0, wxRIGHT | wxLEFT | wxEXPAND, 5);
	romSizer->AddSpacer(4);
	romSizer->Add(m_pDefault, 0, wxRIGHT | wxLEFT, 5);
	romSizer->AddSpacer(4);	

	m_pEditorScript = new wxButton(this, wxID_ANY, "Script Editor");
	m_pEditorItem = new wxButton(this, wxID_ANY, "Item Editor");

	wxStaticBox* editorsBox = new wxStaticBox(this, wxID_ANY, "Editors");
	wxStaticBoxSizer* editorsSizer = new wxStaticBoxSizer(editorsBox, wxVERTICAL);

	m_pEditorScript->Bind(wxEVT_BUTTON, &MainFrame::OnEditorClick, this);
	m_pEditorItem->Bind(wxEVT_BUTTON, &MainFrame::OnEditorClick, this);

	editorsSizer->AddSpacer(4);
	editorsSizer->Add(m_pEditorScript, 0, wxRIGHT | wxLEFT | wxEXPAND, 5);	
	editorsSizer->AddSpacer(4);
	editorsSizer->Add(m_pEditorItem, 0, wxRIGHT | wxLEFT | wxEXPAND, 5);
	editorsSizer->AddSpacer(4);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxVERTICAL);
	rootSizer->AddSpacer(4);
	rootSizer->Add(romSizer, 0, wxEXPAND | wxALL, 4);
	rootSizer->AddSpacer(4);
	rootSizer->Add(editorsSizer, 0, wxEXPAND | wxALL, 4);

#ifdef _DEBUG	
	m_pTestButton = new wxButton(this, wxID_ANY, "Test");
	m_pTestButton->Bind(wxEVT_BUTTON, &MainFrame::OnTestClick, this);

	rootSizer->Add(m_pTestButton);
#endif

	SetSizerAndFit(rootSizer);
	rootSizer->SetSizeHints(this);	
}

void MainFrame::OnTestClick(wxCommandEvent& event)
{
	//wxFileName path(wxStandardPaths::Get().GetExecutablePath());
	//path.AppendDir("Spellchecker");
	//path.AppendDir("Dics");

	//SpellChecker spellChecker("pt_BR", path.GetPath(true).ToStdString());
}

id MainFrame::GetCurrentId()
{
	switch ((id)m_pSelection->GetSelection())
	{
	case id::FoMT:
		return id::FoMT;
		break;
	case id::MFoMT:
		return id::MFoMT;
		break;
	case id::DS:
		return id::DS;
		break;
	default:
		wxMessageBox("This is not suposed to happen...", "Huh?", 5L, this);
		return (id)-1;
		break;
	}
}

void MainFrame::OnEditorClick(wxCommandEvent& event)
{
	wxWindowID id = event.GetId();
	
	if (id == m_pEditorScript->GetId())
	{
		cScriptEditor* scriptEditor = new cScriptEditor(GetCurrentId());
		scriptEditor->Show();
	}
	else if (id == m_pEditorItem->GetId())
	{
		ItemEditorFrame* itemEditor = new ItemEditorFrame(GetCurrentId());
		itemEditor->Show();
	}

	event.Skip();
}