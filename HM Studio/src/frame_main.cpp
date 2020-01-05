#include "frame_main.hpp"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)

EVT_CHOICE(ID_currentChoice, MainFrame::OnChoiceChanged)
EVT_BUTTON(ID_scriptEditor, MainFrame::scriptEditor_onClick)
EVT_BUTTON(ID_tileEditor, MainFrame::OnButtonTileEditor_Click)
EVT_BUTTON(ID_teste1, MainFrame::OnButtonTeste1_Click)

wxEND_EVENT_TABLE()

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
	//currentChoice->Bind(wxEVT_CHOICE, &MainFrame::OnChoiceChanged, this);
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

	m_pGUI_openItemEditor = new wxButton(this, wxID_ANY, _("Item Editor"));
	m_pGUI_openItemEditor->Bind(wxEVT_BUTTON, &MainFrame::EVT_BUTTON_ItemEditorClick, this);
	m_pGUI_boxItemEditor = new wxStaticBox(this, wxID_ANY, _("Item"));
	m_pGUI_boxSizerItemEditor = new wxStaticBoxSizer(m_pGUI_boxItemEditor, wxVERTICAL);
	m_pGUI_boxSizerItemEditor->Add(m_pGUI_openItemEditor, 0, 4);

	teste_button_1 = new wxButton(this, ID_teste1, "Teste 1");
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

void MainFrame::OnButtonTileEditor_Click(wxCommandEvent& event)
{
	tileEditorFrame = new TileEditorFrame();
	tileEditorFrame->Show();

	event.Skip();
} 

void MainFrame::OnButtonTeste1_Click(wxCommandEvent& event)
{		
	Rom rom = Rom(GetCurrentId(), false);

	//unsigned int adress1 = 0x607de0;
	//unsigned int adress2 = 0x61a2f0;
	//unsigned int adress3 = 0x6085c4;
	//unsigned int adress4 = 0x60A8E4;

	//rom.Seek(0xF876C);
	//short item_id = rom.ReadUInt16();

	//unsigned int offset = (item_id << 2) + adress1;

	//rom.Seek(offset + 2);
	//short flag = rom.ReadUInt16();

	//rom.Seek(offset);
	//short flag2 = rom.ReadUInt16();

	//flag = flag << 2;
	//unsigned int offset2 = adress2 + flag; //0861A9C4

	//offset2 = offset2 + 0;

	//rom.Seek(offset2);
	//short flag3 = rom.ReadUInt16(); //195	

	//flag3 = flag3 << 4;

	//unsigned int offset3 = adress3 + flag3;

	//rom.Seek(offset3 + 2);
	//short flag4 = rom.ReadUInt16();

	//rom.Seek(offset3);
	//short flag5 = rom.ReadUInt16();

	//rom.Seek(offset3 + 6);
	//unsigned short flag6 = rom.ReadUInt16();
	//flag6 = flag6 << 5;

	//unsigned int item_image = adress4 + flag6;

	std::vector<Item> itens;
	Item::GetItems(rom, ITEM_TYPE::SHELF, itens);

	event.Skip();
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
	ItemEditorFrame* itemEditorFrame = new ItemEditorFrame();
	itemEditorFrame->Show();
	GetCurrentId();
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
