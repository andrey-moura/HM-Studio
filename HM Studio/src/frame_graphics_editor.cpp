#include "frame_graphics_editor.hpp"

GraphicsEditorFrame::GraphicsEditorFrame(id i) : wxFrame(nullptr, wxID_ANY, "Graphics Editor"), m_RomOriginal(i, false), m_RomTranslated(i, true)
{
	CreateGUIControls();
}

void GraphicsEditorFrame::GetGraphicsList()
{	
	GraphicsTreeItem balloons("Balloons", GraphicsInfo(0x6C316c, 0x6C40F0, 16, 496));
	AppendGraphics(balloons);

	GraphicsTreeParent calendary("Calendary");
	calendary.push_back(GraphicsTreeItem("Numbers", GraphicsInfo(0x6F4C58, 0x6F5D1C, 16, 160)));
	AppendGraphics(calendary);

	GraphicsTreeItem clock("Clock", GraphicsInfo(0x70017c, 0x6CDD04, 256, 32));
	AppendGraphics(clock);	
	
	GraphicsTreeParent font("Font");
}

void GraphicsEditorFrame::FromOriginal()
{
	GetGraphics(m_Index->second, m_RomOriginal);
}

void GraphicsEditorFrame::ExportImage()
{
	wxFileDialog dialog(nullptr, "Select output path", wxEmptyString, "image.png");

	if (dialog.ShowModal() != wxID_CANCEL)
	{
		const uint8_t* data = m_Graphic.GetData();
		const Palette& palette = m_Graphic.GetPalette();

		wxBitmap bitmap(wxSize(m_Graphic.GetWidth(), m_Graphic.GetHeight()), 24);
	
		wxMemoryDC dc(bitmap);

		dc.SetPen(*wxTRANSPARENT_PEN);

		wxBrush brush(*wxBLACK, wxBRUSHSTYLE_SOLID);

		for (size_t y = 0; y < m_Graphic.GetHeight(); ++y)
		{
			for (size_t x = 0; x < m_Graphic.GetWidth(); ++x)
			{								
				brush.SetColour(palette[*(data + x + y * m_Graphic.GetWidth())].GetBGR());
				dc.SetBrush(brush);

				dc.DrawRectangle(x, y, 1, 1);
			}
		}

		bitmap.SaveFile(dialog.GetPath(), wxBitmapType::wxBITMAP_TYPE_PNG);
	}
}

void GraphicsEditorFrame::OnMenuBarClick(wxCommandEvent& event)
{
	int id = event.GetId();

	if (id >= ID_ZOOM_IN && id <= ID_ZOOM_32)
	{
		Zoom(id);
	}
	else
	{
		switch (id)
		{		
		case wxID_OPEN:
			break;
		case wxID_SAVE:
			SaveImage();
			break;
		case wxID_CLOSE:
			break;
		case ID_IMPORT:
			break;
		case ID_EXPORT:
			ExportImage();
			break;
		case wxID_COPY:
			FromOriginal();
			break;
		case ID_PIXEL_GRID:			
			ToggleMenu(id, m_ImageView->TogglePixelGrid());
			break;
		case ID_TILE_GRID:			
			ToggleMenu(id, m_ImageView->ToggleTileGrid());
			break;
		case ID_BLOCK_GRID:			
			ToggleMenu(id, m_ImageView->ToggleBlockGrid());
			break;
		default:
			break;
		}
	}

	event.Skip();
}

void GraphicsEditorFrame::OnSelChanged(wxTreeEvent& event)
{
	wxTreeItemId id = event.GetItem();	

	MAP::iterator it = m_LookUp.find(id);

	if (it != m_LookUp.end())
	{
		m_Index = it;
		GetGraphics(it->second, m_RomTranslated);
	}

	event.Skip();
}

void GraphicsEditorFrame::GetGraphics(const GraphicsInfo& info, RomFile& rom)
{	
	m_Graphic.SetWidth(info.m_Width);
	m_Graphic.SetHeight(info.m_Height);
	m_Graphic.SetImgOffset(info.m_ImageAdress);
	m_Graphic.SetPalOffset(info.m_PaletteAdress);
	m_Graphic.SetBpp(info.m_Bpp);
	m_Graphic.SetReversed(info.m_Reversed);
	m_Graphic.SetPlanar(info.m_Planar);

	m_Graphic.LoadFromRom(rom);

	m_ImageView->SetGraphics(&m_Graphic);
	m_PalCtrl->SetPal(m_Graphic.GetPalette());
	m_InfoViewer->SetInfo(info);
}

void GraphicsEditorFrame::SaveImage()
{
	//m_Graphic.Encode(m_ImageView->GetImage());
}

inline void GraphicsEditorFrame::AppendGraphics(const GraphicsTreeItem& item, const wxTreeItemId& id)
{
	m_LookUp.insert(std::make_pair(m_pNavigator->AppendItem(id, item.m_Name), item.m_Info));
}

void GraphicsEditorFrame::AppendGraphics(const GraphicsTreeItem& item)
{	
	AppendGraphics(item, m_Root);
}

void GraphicsEditorFrame::AppendGraphics(const GraphicsTreeParent& parent)
{
	wxTreeItemId id = m_pNavigator->AppendItem(m_Root, parent.GetName());

	for (const GraphicsTreeItem& item : parent)
	{
		AppendGraphics(item, id);
	}
}

void GraphicsEditorFrame::Zoom(int zoomMode)
{
	int newZoom = 0;

	if (zoomMode == ID_ZOOM_OUT || zoomMode == ID_ZOOM_IN)
	{
		int currentZoom = m_ImageView->GetScale();		

		if (zoomMode == ID_ZOOM_OUT && currentZoom > 1)
		{
			newZoom = currentZoom / 2;
		}			

		if (zoomMode == ID_ZOOM_IN && currentZoom < 32)
		{
			newZoom = currentZoom * 2;
		}		
	}
	else
	{
		newZoom = pow(2, (zoomMode - ID_ZOOM_1));
	}	
	
	m_ImageView->SetScale(newZoom);
}

void GraphicsEditorFrame::ToggleMenu(int id, bool check)
{
	GetMenuBar()->FindItem(id)->Check(check);	
}

void GraphicsEditorFrame::OnPalChanged(PaletteEvent& event)
{
	m_ImageView->SetColor1(event.GetFirst());
	m_ImageView->SetColor2(event.GetSecond());

	event.Skip();
}

void GraphicsEditorFrame::CreateGUIControls()
{
	wxMenu* menuFile = new wxMenu();
	menuFile->Append(wxID_OPEN, "Open");
	menuFile->Append(wxID_SAVE, "Save");
	menuFile->Append(wxID_CLOSE, "Close");

	wxMenu* menuImage = new wxMenu();
	menuImage->Append(ID_IMPORT, "Import");
	menuImage->Append(ID_EXPORT, "Export");
	menuImage->Append(wxID_COPY, "From Original");

	wxMenu* zoomMenu = new wxMenu();
	zoomMenu->Append(ID_ZOOM_IN, "In");
	zoomMenu->Append(ID_ZOOM_OUT, "Out");
	zoomMenu->Append(ID_ZOOM_1, "1x");
	zoomMenu->Append(ID_ZOOM_2, "2x");
	zoomMenu->Append(ID_ZOOM_4, "4x");
	zoomMenu->Append(ID_ZOOM_8, "8x");
	zoomMenu->Append(ID_ZOOM_16, "16x");
	zoomMenu->Append(ID_ZOOM_32, "32x");

	wxMenu* viewMenu = new wxMenu();
	viewMenu->AppendSubMenu(zoomMenu, "Zoom");
	viewMenu->AppendSeparator();
	viewMenu->AppendCheckItem(ID_PIXEL_GRID, "Pixel Grid");
	viewMenu->AppendCheckItem(ID_TILE_GRID, "Tile Grid");
	viewMenu->AppendCheckItem(ID_BLOCK_GRID, "Block Grid");

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, "File");
	menuBar->Append(menuImage, "Image");
	menuBar->Append(viewMenu, "View");
	menuBar->Bind(wxEVT_MENU, &GraphicsEditorFrame::OnMenuBarClick, this);

	SetMenuBar(menuBar);

	SetBackgroundColour(wxColour(240, 240, 240));

	m_pLeftPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_RAISED);

	m_pNavigator = new wxTreeCtrl(m_pLeftPanel, wxID_ANY);
	m_Root = m_pNavigator->AddRoot("Graphics");
	m_pNavigator->Bind(wxEVT_TREE_SEL_CHANGED, &GraphicsEditorFrame::OnSelChanged, this);
	GetGraphicsList();

	m_InfoViewer = new GraphicsInfoViewer(m_pLeftPanel);

	wxBoxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);
	leftSizer->Add(m_pNavigator, 1, wxEXPAND);
	leftSizer->Add(m_InfoViewer, 0, wxEXPAND);

	m_pLeftPanel->SetSizer(leftSizer);
	leftSizer->SetSizeHints(m_pLeftPanel);

	m_ImageView = new GraphicsView(this);

	wxPanel* bottomPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_RAISED);
	bottomPanel->SetBackgroundColour(*wxWHITE);

	m_PalCtrl = new PalCtrl(bottomPanel);
	m_PalCtrl->SetBackgroundColour(wxColour(0xff, 0xff, 0xff));
	m_PalCtrl->Bind(EVT_PAL_CHANGED, &GraphicsEditorFrame::OnPalChanged, this);

	wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);
	bottomSizer->Add(m_PalCtrl, 0, wxTOP | wxBOTTOM, 4);

	bottomPanel->SetSizer(bottomSizer);
	bottomSizer->SetSizeHints(bottomPanel);

	wxBoxSizer* editorSizer = new wxBoxSizer(wxVERTICAL);
	editorSizer->Add(m_ImageView, 1, wxEXPAND, 0);
	editorSizer->Add(bottomPanel, 0, wxEXPAND, 0);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);
	rootSizer->Add(m_pLeftPanel, 0, wxEXPAND);
	rootSizer->Add(editorSizer, 1, wxEXPAND);

	SetSizer(rootSizer);
	rootSizer->SetSizeHints(this);
}
