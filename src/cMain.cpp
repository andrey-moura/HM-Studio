#include "cMain.h"

wxBEGIN_EVENT_TABLE(cMain, wxFrame)

EVT_CHOICE(ID_currentChoice, cMain::OnChoiceChanged)
EVT_BUTTON(ID_scriptEditor, cMain::scriptEditor_onClick)
EVT_BUTTON(ID_tileEditor, cMain::OnButtonTileEditor_Click)
EVT_BUTTON(ID_teste1, cMain::OnButtonTeste1_Click)

wxEND_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "HM Studio")
{
	CreateGUIControls();
}

cMain::~cMain()
{
}

void cMain::CreateGUIControls()
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
	dumpOriginal->Bind(wxEVT_BUTTON, &cMain::OnButtonDumpOriginal, this);
	dumpTranslated = new wxButton(this, wxID_ANY, "Dump Translated");
	dumpTranslated->Bind(wxEVT_BUTTON, &cMain::OnButtonDumpTranslated, this);
	scriptEditor = new wxButton(this, ID_scriptEditor, "Script Editor");

	scriptSizer->AddSpacer(5);
	scriptSizer->Add(dumpOriginal,   0,  wxRIGHT | wxLEFT | wxEXPAND, 5);
	scriptSizer->AddSpacer(5);
	scriptSizer->Add(dumpTranslated, 0, wxRIGHT | wxLEFT		    , 5); //This will be the largest, and the others will expand to match width.
	scriptSizer->AddSpacer(5);
	scriptSizer->Add(scriptEditor,   0, wxRIGHT | wxLEFT | wxEXPAND, 5);
	scriptSizer->AddSpacer(5);	

	save_editor_button = new wxButton(this, ID_saveEditor, "Save Editor");	
	save_editor_box = new wxStaticBox(this, wxID_ANY, "Save");
	save_editor_sizer = new wxStaticBoxSizer(save_editor_box, wxVERTICAL);
	save_editor_sizer->Add(save_editor_button, 0, wxRIGHT | wxLEFT | wxEXPAND, 5);

	graphic_button_tile_editor = new wxButton(this, ID_tileEditor, _("Tile Editor"));
	graphic_static_box = new wxStaticBox(this, wxID_ANY, "Graphic");
	graphic_static_sizer = new wxStaticBoxSizer(graphic_static_box, wxVERTICAL);
	graphic_static_sizer->Add(graphic_button_tile_editor, 0, wxRIGHT | wxLEFT | wxEXPAND, 5);

	currentBox = new wxStaticBox(this, wxID_ANY, "Selected ROM");
	currentSizer = new wxStaticBoxSizer(currentBox, wxVERTICAL);

	currentChoice = new wxChoice(this, ID_currentChoice);
	currentChoice->Insert("FoMT", 0);
	currentChoice->Insert("MFoMT", 1);
	currentChoice->Insert("DS", 2);
	currentChoice->SetSelection(0);
	//currentChoice->Bind(wxEVT_CHOICE, &cMain::OnChoiceChanged, this);
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
	sizer1->Add(save_editor_sizer, 0, wxEXPAND, 0);
	sizer1->AddSpacer(5);
	sizer1->Add(graphic_static_sizer, 0, wxEXPAND, 0);
	sizer1->AddSpacer(5);
	sizer1->Add(currentSizer, 0, wxEXPAND, 0);

	teste_button_1 = new wxButton(this, ID_teste1, "Teste 1");
	teste_box = new wxStaticBox(this, wxID_ANY, "Teste");

	teste_sizer = new wxStaticBoxSizer(teste_box, wxVERTICAL);	
	teste_sizer->Add(teste_button_1);

	sizer2 = new wxBoxSizer(wxVERTICAL);
	sizer2->AddSpacer(5);
	sizer2->Add(teste_sizer, 0, wxEXPAND, 0);

	sizer0->AddSpacer(5);
	sizer0->Add(sizer1);
	sizer0->AddSpacer(5);
	sizer0->Add(sizer2);
	SetSizer(sizer0);
}

void cMain::OnChoiceChanged(wxCommandEvent& event)
{
	cur_choice = currentChoice->GetSelection();

	event.Skip();
}

void cMain::scriptEditor_onClick(wxCommandEvent& event)
{
	cScriptEditor* formScriptEditor = new cScriptEditor(GetCurrentId());
	formScriptEditor->Show();

	event.Skip();
}

void cMain::OnButtonTileEditor_Click(wxCommandEvent& event)
{
	tileEditorFrame = new TileEditorFrame();
	tileEditorFrame->Show();

	event.Skip();
}

void cMain::OnButtonTeste1_Click(wxCommandEvent& event)
{
	Rom* rom = new Rom(id::MFoMT, true);
	Rom* romOriginal = new Rom(id::MFoMT, false);

	std::vector<std::string> vec;
	vec.push_back("Voc) #");
	vec.push_back("Vou { cl<nica");

	std::string s;

	s = FileUtil::ReadAllText(rom->GetTablePath());
	
	Table::InputTable(s, vec);

	wxMessageBox(rom->GetScriptExportedFullPath(4));	

	//SpriteFile* script = new SpriteFile(FileUtil::ReadAllBytes(romOriginal->GetScriptFullPath(4)));
	//SpriteFile* newScript = new SpriteFile(FileUtil::ReadAllBytes(rom->GetScriptFullPath(4)));	

	//wxFile* fileOut = new wxFile();
	//fileOut->Create("teste.bin", true);
	//fileOut->Open("teste.bin", wxFile::read_write);

	//newScript->UpdateText(script->GetText());
	//
	//fileOut->Write(newScript->data.data(), newScript->data.size());

	delete rom;
	delete romOriginal;
	//delete script;
	//delete newScript;
	//delete fileOut;
	event.Skip();
}

void cMain::OnButtonDumpOriginal(wxCommandEvent& event)
{	
	Rom* rom = new Rom(GetCurrentId(), true);
	if (rom->IsOpened())
	rom->Dump();
	else
		wxMessageBox("Cant dump the ROM because it's not opened... Maybe don't exists...", "Huh?", 5L, this);
	delete rom;

	event.Skip();
}

void cMain::OnButtonDumpTranslated(wxCommandEvent& event)
{
	if (wxMessageBox(_("Are you sure? It will erase your scripts if you already made changes..."), "Huh?", 5L, this) == wxID_CANCEL)
	return;

	Rom* rom = new Rom(GetCurrentId(), true);
	if (rom->IsOpened())
		rom->Dump();
	else
		wxMessageBox(_("Cant dump the ROM because it's not opened... Maybe don't exists..."), "Huh?", 5L, this);
	delete rom;

	event.Skip();
}

id cMain::GetCurrentId()
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
