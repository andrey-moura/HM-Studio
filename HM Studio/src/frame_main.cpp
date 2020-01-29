#include "frame_main.hpp"

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "HM Studio")
{
	CreateGUIControls();	
}

MainFrame::~MainFrame()
{
}

void MainFrame::CreateGUIControls()
{	
	this->SetBackgroundColour(wxColour(240, 240, 240, 255));

	sizer0 = new wxBoxSizer(wxHORIZONTAL);
	sizer1 = new wxBoxSizer(wxVERTICAL);

	romBox = new wxStaticBox(this, wxID_ANY, "ROM");
	romSizer = new wxStaticBoxSizer(romBox, wxVERTICAL);

	openRom = new wxButton(this, wxID_ANY, "Open");
	romBackup = new wxButton(this, wxID_ANY, "Backup");

	romSizer->AddSpacer(5);
	romSizer->Add(openRom, 0, wxRIGHT | wxLEFT | wxEXPAND, 5);
	romSizer->AddSpacer(5);
	romSizer->Add(romBackup, 0, wxRIGHT | wxLEFT | wxEXPAND, 5);
	romSizer->AddSpacer(5);

	scriptBox = new wxStaticBox(this, wxID_ANY, "Script");
	scriptSizer = new wxStaticBoxSizer(scriptBox, wxVERTICAL);

	dumpOriginal = new wxButton(this, wxID_ANY, "Dump Original");
	dumpOriginal->Bind(wxEVT_BUTTON, &MainFrame::OnButtonDumpOriginal, this);
	dumpTranslated = new wxButton(this, wxID_ANY, "Dump Translated");
	dumpTranslated->Bind(wxEVT_BUTTON, &MainFrame::OnButtonDumpTranslated, this);
	scriptEditor = new wxButton(this, wxID_ANY, "Script Editor");
	scriptEditor->Bind(wxEVT_BUTTON, &MainFrame::scriptEditor_onClick, this);

	scriptSizer->AddSpacer(5);
	scriptSizer->Add(dumpOriginal,   0,  wxRIGHT | wxLEFT | wxEXPAND, 5);
	scriptSizer->AddSpacer(5);
	scriptSizer->Add(dumpTranslated, 0, wxRIGHT | wxLEFT		    , 5); //This will be the largest, and the others will expand to match width.
	scriptSizer->AddSpacer(5);
	scriptSizer->Add(scriptEditor,   0, wxRIGHT | wxLEFT | wxEXPAND, 5);
	scriptSizer->AddSpacer(5);	

	currentBox = new wxStaticBox(this, wxID_ANY, "Selected ROM");
	currentSizer = new wxStaticBoxSizer(currentBox, wxVERTICAL);

	currentChoice = new wxChoice(this, wxID_ANY);
	currentChoice->Insert("FoMT", 0);
	currentChoice->Insert("MFoMT", 1);
	currentChoice->Insert("DS", 2);
	currentChoice->SetSelection(0);
	currentChoice->Bind(wxEVT_CHOICE, &MainFrame::OnChoiceChanged, this);
	currentDefault = new wxCheckBox(this, wxID_ANY, "Default");

	currentSizer->AddSpacer(5);
	currentSizer->Add(currentChoice, 0, wxRIGHT | wxLEFT | wxEXPAND, 5);
	currentSizer->AddSpacer(5);
	currentSizer->Add(currentDefault, 0, wxRIGHT | wxLEFT, 5);
	currentSizer->AddSpacer(5);

	sizer1->AddSpacer(5);
	sizer1->Add(romSizer, 0, wxEXPAND, 0);
	sizer1->AddSpacer(5);
	sizer1->Add(scriptSizer); //This will be the largest, and the others will expand to match width.
	sizer1->AddSpacer(5);
	sizer1->Add(currentSizer, 0, wxEXPAND, 0);

	m_pGUI_openItemEditor = new wxButton(this, wxID_ANY, _("Item Editor"));
	m_pGUI_openItemEditor->Bind(wxEVT_BUTTON, &MainFrame::EVT_BUTTON_ItemEditorClick, this);
	m_pGUI_boxItemEditor = new wxStaticBox(this, wxID_ANY, _("Item"));
	m_pGUI_boxSizerItemEditor = new wxStaticBoxSizer(m_pGUI_boxItemEditor, wxVERTICAL);
	m_pGUI_boxSizerItemEditor->Add(m_pGUI_openItemEditor, 0, 4);

	teste_button_1 = new wxButton(this, wxID_ANY, "Teste 1");
	teste_button_1->Bind(wxEVT_BUTTON, &MainFrame::OnButtonTeste1_Click, this);
	teste_box = new wxStaticBox(this, wxID_ANY, "Teste");

	teste_sizer = new wxStaticBoxSizer(teste_box, wxVERTICAL);
	teste_sizer->Add(teste_button_1);

	sizer2 = new wxBoxSizer(wxVERTICAL);
	sizer2->AddSpacer(5);
	sizer2->Add(teste_sizer, 0, wxEXPAND, 0);
	sizer2->Add(m_pGUI_boxSizerItemEditor, 0, wxEXPAND, 0);

	sizer0->AddSpacer(5);
	sizer0->Add(sizer1);
	sizer0->AddSpacer(5);
	sizer0->Add(sizer2);
	SetSizer(sizer0);
}

void MainFrame::OnChoiceChanged(wxCommandEvent& event)
{
	cur_choice = currentChoice->GetSelection();

	event.Skip();
}

void MainFrame::scriptEditor_onClick(wxCommandEvent& event)
{
	cScriptEditor* formScriptEditor = new cScriptEditor(GetCurrentId());
	formScriptEditor->Show();

	event.Skip();
}

void MainFrame::OnButtonTeste1_Click(wxCommandEvent& event)
{		
	//wxFileName path(wxStandardPaths::Get().GetExecutablePath());
	//path.AppendDir("Spellchecker");
	//path.AppendDir("Dics");

	//SpellChecker spellChecker("pt_BR", path.GetPath(true).ToStdString());

	//spellChecker.add_to_user("porra");
}

void MainFrame::OnButtonDumpOriginal(wxCommandEvent& event)
{	
	Rom rom = Rom(GetCurrentId(), true);
	if (rom.IsOpened())
	rom.Dump();
	else
		wxMessageBox("Cant dump the ROM because it's not opened... Maybe don't exists...", "Huh?", 5L, this);	

	event.Skip();
}

void MainFrame::OnButtonDumpTranslated(wxCommandEvent& event)
{
	if (wxMessageBox(_("Are you sure? It will erase your scripts if you already made changes..."), "Huh?", wxICON_QUESTION | wxYES_NO, this) == wxID_CANCEL)
	return;

	Rom* rom = new Rom(GetCurrentId(), true);
	if (rom->IsOpened())
		rom->Dump();
	else
		wxMessageBox(_("Cant dump the ROM because it's not opened... Maybe don't exists..."), "Huh?", 5L, this);
	delete rom;

	event.Skip();
}

void MainFrame::EVT_BUTTON_ItemEditorClick(wxCommandEvent& event)
{
	ItemEditorFrame* itemEditorFrame = new ItemEditorFrame(GetCurrentId());
	itemEditorFrame->Show();	
	event.Skip();
}

id MainFrame::GetCurrentId()
{
	switch (cur_choice)
	{
	case (int)id::FoMT:
		return id::FoMT;
		break;
	case (int)id::MFoMT:
		return id::MFoMT;
		break;
	case (int)id::DS:
		return id::DS;
		break;
	default:
		wxMessageBox("This is not suposed to happen...", "Huh?", 5L, this);
		return (id)-1;
		break;
	}
}
