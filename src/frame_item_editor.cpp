#include "frame_item_editor.hpp"

ItemEditorFrame::ItemEditorFrame(id id) : EditorFrame(new ItemEditor(id)), m_Graphics(16, 16)
{
	CreateGUIControls();	
}

ItemEditorFrame::~ItemEditorFrame()
{
}

GraphicsInfo ItemEditorFrame::GetInfo()
{
	return GraphicsInfo(((ItemEditor*)m_pEditor)->GetImgAdress(true), ((ItemEditor*)m_pEditor)->GetPalAdress(true));
}

void ItemEditorFrame::OnFileClick(wxCommandEvent& event)
{	
	((ItemEditor*)m_pEditor)->OpenItens(event.GetId() - 5051);
	UpdateText();

	if (!m_IsOpen)
	{
		Layout();
	}

	m_IsOpen = true;

	event.Skip();
}

void ItemEditorFrame::OnInsertClick(wxCommandEvent& event)
{
	((ItemEditor*)m_pEditor)->InsertItens();

	event.Skip();
}

GraphicsTreeParent ItemEditorFrame::GetGraphicsList()
{
	GraphicsTreeParent parent(((ItemEditor*)m_pEditor)->GetInfo().m_Name);

	const auto& itens = ((ItemEditor*)m_pEditor)->GetCurItens(true);

	for (const Item& item : itens)
	{
		parent.push_back(GraphicsTreeItem(item.GetName(), GraphicsInfo(item.GetImgAdress(), item.GetPalAdress())));
	}

	return parent;
}

void ItemEditorFrame::OnGetTextFrom()
{
	wxString path = wxLoadFileSelector(L"Item", L"itens");

	if (!path.empty())
		m_pEditor->GetTextFromPath(path.ToStdString());
}

void ItemEditorFrame::UpdateText()
{
	if (m_pEditor->GetRom(true).Console == console::GBA)
	{
		m_pOrigItemName->SetLabel(((ItemEditor*)m_pEditor)->GetName(false));
	}

	m_pOrigItemDescription->SetLabel(((ItemEditor*)m_pEditor)->GetDescription(false));

	if (m_pEditor->GetRom(true).Console == console::GBA)
	{
		m_pItemName->SetValue(((ItemEditor*)m_pEditor)->GetName(true));
	}

	m_pItemText->SetText(((ItemEditor*)m_pEditor)->GetDescription(true));

	if (m_pEditor->GetRom(true).Console == console::GBA)
	{
		m_Graphics.SetImgOffset(((ItemEditor*)m_pEditor)->GetImgAdress(true));
		m_Graphics.SetPalOffset(((ItemEditor*)m_pEditor)->GetPalAdress(true));

		m_Graphics.LoadFromRom(m_pEditor->GetRom(true));
		m_pItemIconView->SetGraphics(&m_Graphics);
	}
}

void ItemEditorFrame::OnImageDoubleClick(wxMouseEvent& event)
{
	if (m_GraphicsFrame == nullptr)
	{
		auto graphics = GetGraphicsList();
		m_GraphicsFrame = new GraphicsEditorFrame(m_pEditor->GetRom(true).Id, graphics);
		m_GraphicsFrame->SetTitle("Item Images");
	}

	m_GraphicsFrame->Show();

	event.Skip();
}

void ItemEditorFrame::OnDumpClick(wxCommandEvent& event)
{
	((ItemEditor*)m_pEditor)->Dump();

	event.Skip();
}

void ItemEditorFrame::OnDumpImgClick(wxCommandEvent& event)
{
	((ItemEditor*)m_pEditor)->DumpImages();

	event.Skip();
}

void ItemEditorFrame::CreateGUIControls()
{
	wxMenu* menuFile = new wxMenu();
	menuFile->Bind(wxEVT_MENU, &ItemEditorFrame::OnGetTextFromClick, this, menuFile->Append(wxID_ANY, L"Get Text From...")->GetId());

	const std::vector<ItemInfo> infos = ((ItemEditor*)m_pEditor)->GetInfos();

	for (size_t i = 0; i < infos.size(); ++i)
	{
		menuFile->Append(wxID_FILE1 + i, infos[i].m_Name);
		menuFile->Bind(wxEVT_MENU, &ItemEditorFrame::OnFileClick, this, wxID_FILE1 + i);
	}

	menuFile->Bind(wxEVT_MENU, &ItemEditorFrame::OnInsertClick, this, menuFile->Append(wxNewId(), "Insert", "Insert the itens")->GetId());

	wxMenu* menuTools = new wxMenu();
	menuTools->Bind(wxEVT_MENU, &ItemEditorFrame::OnDumpClick, this, menuTools->Append(wxNewId(), "Dumper", "Export the itens")->GetId());
	menuTools->Bind(wxEVT_MENU, &ItemEditorFrame::OnDumpImgClick, this, menuTools->Append(wxNewId(), "Dump images", "Export images")->GetId());

	CreateMyMenuBar();
	m_frameMenuBar->Append(menuFile, "File");
	m_frameMenuBar->Append(menuTools, "Tools");

	CreateViewMenu();

	SetMenuBar(m_frameMenuBar);

	if (m_pEditor->GetRom(true).Console == console::GBA)
		m_pItemName = new wxTextCtrl(this, wxID_ANY);

	m_pItemText = new STC(this, wxID_ANY);
	m_pItemText->SetMaxLineLenght(28);

	CreateButtonsSizer();

	if (m_pEditor->GetRom(true).Console == console::GBA)
		m_pOrigItemName = new wxStaticText(this, wxID_ANY, "");

	m_pOrigItemDescription = new wxStaticText(this, wxID_ANY, "");

	m_pItemIconView = new GraphicsView(this);
	m_pItemIconView->SetMinSize(wxSize(64, 64));
	m_pItemIconView->SetSize(wxSize(64, 64));
	m_pItemIconView->SetScale(4);
	m_pItemIconView->SetViewOnly(true);
	m_pItemIconView->Bind(wxEVT_LEFT_DCLICK, &ItemEditorFrame::OnImageDoubleClick, this);

	wxBoxSizer* sizer10 = new wxBoxSizer(wxVERTICAL);
	if (m_pEditor->GetRom(true).Console == console::GBA)
		sizer10->Add(m_pOrigItemName, 0, wxALL, 4);
	sizer10->Add(m_pOrigItemDescription, 0, wxALL, 4);

	m_pOrigBox = new wxStaticBox(this, wxID_ANY, "Original");
	m_pOrigBoxSizer = new wxStaticBoxSizer(m_pOrigBox, wxHORIZONTAL);
	m_pOrigBoxSizer->Add(sizer10, 1, wxEXPAND);
	m_pOrigBoxSizer->AddStretchSpacer(1);
	m_pOrigBoxSizer->Add(m_pItemIconView);

	m_pRootSizer = new wxBoxSizer(wxVERTICAL);

	if (m_pEditor->GetRom(true).Console == console::GBA)
		m_pRootSizer->Add(m_pItemName, 0, wxALL | wxEXPAND, 4);
	m_pRootSizer->Add(m_pItemText, 1, wxALL | wxEXPAND, 4);
	m_pRootSizer->Add(m_pButtonsSizer, 0, wxALL | wxEXPAND, 4);
	m_pRootSizer->Add(m_pOrigBoxSizer, 0, wxALL | wxEXPAND, 4);

	CreateMyStatusBar();
	StatusToStc(m_pItemText);

	if (m_RestoredSize)
	{
		SetSizer(m_pRootSizer);
		SetMinSize(m_pRootSizer->GetMinSize());
	}
	else
	{
		SetSizerAndFit(m_pRootSizer);
		m_pRootSizer->SetSizeHints(this);
	}
}
