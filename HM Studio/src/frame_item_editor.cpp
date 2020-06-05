#include "frame_item_editor.hpp"

ItemEditorFrame::ItemEditorFrame(id id) : wxFrame(nullptr, wxID_ANY, "Item Editor"), m_RomOriginal(id, false), m_RomTranslated(id, true), m_Editor(m_RomOriginal, m_RomTranslated), m_Graphics(16, 16)
{
	CreateGUIControls();	
}

ItemEditorFrame::~ItemEditorFrame()
{
}

void ItemEditorFrame::UpdateItem()
{		
	if (m_RomOriginal.Console == console::GBA)
	{
		m_pOrigItemName->SetLabel(m_Editor.GetName(false));
	}

	m_pOrigItemDescription->SetLabel(m_Editor.GetDescription(false));

	if (m_RomOriginal.Console == console::GBA)
	{
		m_pItemName->SetValue(m_Editor.GetName(true));
	}

	m_pItemText->SetText(m_Editor.GetDescription(true));

	if (m_RomOriginal.Console == console::GBA)
	{
		m_Graphics.SetImgOffset(m_Editor.GetImgAdress(true));
		m_Graphics.SetPalOffset(m_Editor.GetPalAdress(true));

		m_Graphics.LoadFromRom(m_RomTranslated);
		m_pItemIconView->SetGraphics(&m_Graphics);
	}	
}

void ItemEditorFrame::OnFileClick(wxCommandEvent& event)
{	
	m_Editor.OpenItens(event.GetId() - 5051);
	UpdateItem();

	if (!m_IsOpen)
	{
		Layout();
	}

	m_IsOpen = true;

	event.Skip();
}

void ItemEditorFrame::OnProxClick(wxCommandEvent& event)
{
	m_Editor.ProxItem();
	UpdateItem();
}

void ItemEditorFrame::OnPrevClick(wxCommandEvent& event)
{
	m_Editor.PrevItem();
	UpdateItem();
}

void ItemEditorFrame::OnSaveClick(wxCommandEvent& event)
{
	m_Editor.Save(m_pItemName->GetValue().ToStdString(), m_pItemText->GetText().ToStdString());
	UpdateItem();

	event.Skip();
}

void ItemEditorFrame::OnInsertClick(wxCommandEvent& event)
{
	m_Editor.InsertItens();

	event.Skip();
}

void ItemEditorFrame::CreateGUIControls()
{
	//*******************************************************************************//
	//										MENU
	//*******************************************************************************//	

	wxMenuBar* menu = new wxMenuBar();

	wxMenu* menuFile = new wxMenu();

    const std::vector<ItemInfo> infos = m_Editor.GetInfos();

	for (size_t i = 0; i < infos.size(); ++i)
	{
		menuFile->Append(wxID_FILE1 + i, infos[i].m_Name);
		menuFile->Bind(wxEVT_MENU, &ItemEditorFrame::OnFileClick, this, wxID_FILE1 + i);
	}	

   menuFile->Bind(wxEVT_MENU, &ItemEditorFrame::OnInsertClick, this, menuFile->Append(wxNewId(), "Insert", "Insert the itens")->GetId());

   menu->Append(menuFile, "File");

	SetMenuBar(menu);

	this->SetBackgroundColour(wxColour(240, 240, 240, 255));

	if (m_RomOriginal.Console == console::GBA)
		m_pItemName = new wxTextCtrl(this, wxID_ANY);
	
	m_pItemText = new STC(this, wxID_ANY);
	m_pItemText->SetMaxLineLenght(28);

	m_pSaveText = new wxButton(this, wxID_ANY, "Save");
	m_pSaveText->Bind(wxEVT_BUTTON, &ItemEditorFrame::OnSaveClick, this);

	wxSize buttons_size(0, 0);
	buttons_size.SetWidth(m_pSaveText->GetTextExtent("Save").GetWidth() * 2);
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

	if (m_RomOriginal.Console == console::GBA)
		m_pOrigItemName = new wxStaticText(this, wxID_ANY, "");

	m_pOrigItemDescription = new wxStaticText(this, wxID_ANY, "");

	m_pItemIconView = new GraphicsView(this);
	m_pItemIconView->SetMinSize(wxSize(64, 64));
	m_pItemIconView->SetSize(wxSize(64, 64));
	m_pItemIconView->SetScale(4);

	wxBoxSizer* sizer10 = new wxBoxSizer(wxVERTICAL);
	if (m_RomOriginal.Console == console::GBA)
		sizer10->Add(m_pOrigItemName, 0, wxALL, 4);
	sizer10->Add(m_pOrigItemDescription, 0, wxALL, 4);	

	m_pOrigBox = new wxStaticBox(this, wxID_ANY, "Original");
	m_pOrigBoxSizer = new wxStaticBoxSizer(m_pOrigBox, wxHORIZONTAL);
	m_pOrigBoxSizer->Add(sizer10, 1, wxEXPAND);
	m_pOrigBoxSizer->AddStretchSpacer(1);
	m_pOrigBoxSizer->Add(m_pItemIconView);

	m_pRootSizer = new wxBoxSizer(wxVERTICAL);

	if (m_RomOriginal.Console == console::GBA)
		m_pRootSizer->Add(m_pItemName, 0, wxALL | wxEXPAND, 4);
	m_pRootSizer->Add(m_pItemText, 1, wxALL | wxEXPAND, 4);
	m_pRootSizer->Add(m_pGUI_navigationSizer, 0, wxALL | wxEXPAND, 4);	
	m_pRootSizer->Add(m_pOrigBoxSizer, 0, wxALL | wxEXPAND, 4);

	SetSizer(m_pRootSizer);
	m_pRootSizer->Fit(this);
	m_pRootSizer->SetSizeHints(this);
}
