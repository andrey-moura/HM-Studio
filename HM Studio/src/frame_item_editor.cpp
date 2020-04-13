#include "frame_item_editor.hpp"

ItemEditorFrame::ItemEditorFrame(id id) : wxFrame(nullptr, wxID_ANY, "Item Editor"), m_RomOriginal(id, false), m_RomTranslated(id, true)
{
	CreateGUIControls();	
}

ItemEditorFrame::~ItemEditorFrame()
{
}

void ItemEditorFrame::GetItens(size_t index)
{	
	ITEM_TYPE type = (ITEM_TYPE)index;	

	Item::GetItems(m_RomOriginal, type, m_ItensOriginais);
	m_ItemData = Item::GetItems(m_RomTranslated, type, m_Itens);

	m_Index = 0;
	UpdateItem();
}

void ItemEditorFrame::UpdateItem()
{
	//m_pOrigItemName->SetLabel(m_ItemOriginal.GetName());
	//m_pOrigItemDescription->SetLabel(m_ItemOriginal.GetDescription());

	//m_pItemName->SetLabel(m_Item.GetName());	
	//
	//m_pItemText->SetText(m_Item.GetDescription());

	//uint8_t* image = new uint8_t[256];
 //   uint8_t* pal = new uint8_t[256];

	//m_RomTranslated.Seek(m_Item.GetImageOffset());
	//m_RomTranslated.Read(image, 256);

	//m_RomTranslated.Seek(m_Item.GetPalOffset());
	//m_RomTranslated.Read(pal, 256);

	//Graphics graphics(image, 4, 16, 16);
	//graphics.DecodePalette(pal);
	//m_ItemIcon = graphics.ToImage().Scale(64, 64);
	//m_pItemIconView->SetBitmap(m_ItemIcon);
}

void ItemEditorFrame::OnFileClick(wxCommandEvent& event)
{
	GetItens(event.GetId() - 5051);

	event.Skip();
}

void ItemEditorFrame::OnProxClick(wxCommandEvent& event)
{
	m_Index++;

	if (m_Index == m_ItemData.m_Count) m_Index = 0;

	UpdateItem();
}

void ItemEditorFrame::OnPrevClick(wxCommandEvent& event)
{
	m_Index--;
	
	if (m_Index == -1) m_Index = m_ItemData.m_Count - 1;

	UpdateItem();
}

void ItemEditorFrame::CreateGUIControls()
{
//*******************************************************************************//
//										MENU
//*******************************************************************************//
	wxMenuBar* menu = new wxMenuBar();

	wxMenu* menuFile = new wxMenu();
	menuFile->Append(wxID_FILE1, "Tool");
	menuFile->Append(wxID_FILE2, "Fridge");
	menuFile->Append(wxID_FILE3, "Shelf");

	menu->Append(menuFile, "File");
	menu->Bind(wxEVT_MENU, &ItemEditorFrame::OnFileClick, this, wxID_FILE1);
	menu->Bind(wxEVT_MENU, &ItemEditorFrame::OnFileClick, this, wxID_FILE2);
	menu->Bind(wxEVT_MENU, &ItemEditorFrame::OnFileClick, this, wxID_FILE3);

	SetMenuBar(menu);

	this->SetBackgroundColour(wxColour(240, 240, 240, 255));

	m_pItemName = new wxTextCtrl(this, wxID_ANY);
	m_pItemText = new STC(this, wxID_ANY);
	m_pItemText->SetMaxLineLenght(28);

	m_pSaveText = new wxButton(this, wxID_ANY, _("Save"));

	wxSize buttons_size(0, 0);
	buttons_size.SetWidth(m_pSaveText->GetTextExtent(_("Save")).GetWidth() * 2);
	buttons_size.SetHeight(m_pSaveText->GetSize().GetHeight());	

	m_pSaveText->SetMinSize(buttons_size);
	m_pPrevText = new wxButton(this, wxID_ANY, "<<");
	m_pPrevText->Bind(wxEVT_BUTTON, &ItemEditorFrame::OnPrevClick, this);
	m_pPrevText->SetMinSize(buttons_size);
	m_pProxText = new wxButton(this, wxID_ANY, ">>");
	m_pProxText->Bind(wxEVT_BUTTON, &ItemEditorFrame::OnProxClick, this);
	m_pProxText->SetMinSize(buttons_size);

	wxBoxSizer* m_pGUI_navigationSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pGUI_navigationSizer->Add(m_pSaveText, 0, 0, 0);
	m_pGUI_navigationSizer->AddStretchSpacer();
	m_pGUI_navigationSizer->Add(m_pPrevText, 0, 0, 0);
	m_pGUI_navigationSizer->AddSpacer(4);
	m_pGUI_navigationSizer->Add(m_pProxText, 0, 0, 0);

	m_pOrigItemName = new wxStaticText(this, wxID_ANY, "Original Item Name");
	m_pOrigItemDescription = new wxStaticText(this, wxID_ANY, "Original Item Description \n\n\n");

	m_pItemIconView = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap);	

	wxBoxSizer* sizer10 = new wxBoxSizer(wxVERTICAL);
	sizer10->Add(m_pOrigItemName, 0, wxALL, 4);
	sizer10->Add(m_pOrigItemDescription, 0, wxALL, 4);	

	m_pOrigBox = new wxStaticBox(this, wxID_ANY, "Original");
	m_pOrigBoxSizer = new wxStaticBoxSizer(m_pOrigBox, wxHORIZONTAL);
	m_pOrigBoxSizer->Add(sizer10);
	m_pOrigBoxSizer->AddStretchSpacer(1);
	m_pOrigBoxSizer->Add(m_pItemIconView);

	m_pRootSizer = new wxBoxSizer(wxVERTICAL);	
	m_pRootSizer->Add(m_pItemName, 0, wxALL | wxEXPAND, 4);
	m_pRootSizer->Add(m_pItemText, 1, wxALL | wxEXPAND, 4);
	m_pRootSizer->Add(m_pGUI_navigationSizer, 0, wxALL | wxEXPAND, 4);
	//m_pGUI_mainSizer->AddSpacer(4);
	m_pRootSizer->Add(m_pOrigBoxSizer, 0, wxALL | wxEXPAND, 4);

	SetSizer(m_pRootSizer);
	m_pRootSizer->Fit(this);
	m_pRootSizer->SetSizeHints(this);
}
