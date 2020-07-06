#include "frame_graphics_editor.hpp"

GraphicsEditorFrame::GraphicsEditorFrame(id i) : wxFrame(nullptr, wxID_ANY, "Graphics Editor"), m_RomOriginal(i, false), m_RomTranslated(i, true)
{
	CreateGUIControls();

	wxFileName fn;
	fn.SetPath(m_RomTranslated.m_HomeDir);
	fn.AppendDir(L"Graphics");

	m_RootDir = fn.GetFullPath();
	Moon::File::MakeDir(m_RootDir.ToStdString());
}

void GraphicsEditorFrame::GetGraphicsList()
{		
	SetRootName(L"Graphics");

	AppendGraphics(GraphicsTreeItem("Balloons", GraphicsInfo(0x6C316c, 0x6C40F0, 16, 496)));
	
	AppendGraphics(GraphicsTreeItem("Menu", GraphicsInfo(0x6C427c, 0x6C4680, 16, 128)));
	
	GraphicsTreeParent tv("TV Channels");
	tv.push_back(GraphicsTreeItem("My Dear Princess",    GraphicsInfo(0x6C5400, 0x6CB0A4, 32, 48)));
	tv.push_back(GraphicsTreeItem("Fishing Hour",        GraphicsInfo(0x6C5D00, 0x6CB184, 32, 96)));
	tv.push_back(GraphicsTreeItem("Goddess Game Show",   GraphicsInfo(0x6C6800, 0x6CB224, 32, 64)));
	tv.push_back(GraphicsTreeItem("Life On The Farm",    GraphicsInfo(0x6C6C00, 0x6CB244, 32, 96)));
	tv.push_back(GraphicsTreeItem("Aaron Changes",       GraphicsInfo(0x6C7200, 0x6CB284, 32, 48)));
	tv.push_back(GraphicsTreeItem("Mechabot Ultra",    	 GraphicsInfo(0x6C7500, 0x6CB2A9, 32, 48)));
	tv.push_back(GraphicsTreeItem("News",                GraphicsInfo(0x6c7E00, 0x6CB384, 32, 80)));		
	tv.push_back(GraphicsTreeItem("New Year Game Show",  GraphicsInfo(0x6c8300, 0x6CB3A4, 32, 48)));
	tv.push_back(GraphicsTreeItem("TV Shopping",         GraphicsInfo(0x6C9400, 0x6CB504, 32, 48)));
	tv.push_back(GraphicsTreeItem("Forecast",            GraphicsInfo(0x6C9700, 0x6CB524, 32, 352)));
	AppendGraphics(tv);	
	
	AppendGraphics(GraphicsTreeItem("Clock", GraphicsInfo(0x70017c, 0x6CDD04, 256, 32)));

	AppendGraphics(GraphicsTreeItem("Font", GraphicsInfo(0x9ab020, 0x6F5D1C, 192, 228, 1, false, false, 8, 12)));

	GraphicsTreeParent miss("Miss", true);
	miss.push_back(GraphicsTreeItem("Piece 1", GraphicsInfo(0x6E3130, 0x6E3834, 64, 32)));
	miss.push_back(GraphicsTreeItem("Piece 2", GraphicsInfo(0x6E3530, 0x6E3834, 32, 32)));
	miss.push_back(GraphicsTreeItem("Piece 2", GraphicsInfo(0x6E3730, 0x6E3834, 16, 32)));
	AppendGraphics(miss);

	GraphicsTreeParent pause("Pause", true);
	pause.push_back(GraphicsTreeItem("Piece 1", GraphicsInfo(0x6E389c, 0x6E3FA0, 64, 32)));
	pause.push_back(GraphicsTreeItem("Piece 2", GraphicsInfo(0x6E3c9c, 0x6E3FA0, 32, 32)));
	pause.push_back(GraphicsTreeItem("Piece 2", GraphicsInfo(0x6E3e9c, 0x6E3FA0, 16, 32)));
	AppendGraphics(pause);

	GraphicsTreeParent start("Start", true);
	start.push_back(GraphicsTreeItem("Piece 1", GraphicsInfo(0x6E4008, 0x6E470C, 64, 32)));
	start.push_back(GraphicsTreeItem("Piece 2", GraphicsInfo(0x6E4408, 0x6E470C, 32, 32)));
	start.push_back(GraphicsTreeItem("Piece 2", GraphicsInfo(0x6E4608, 0x6E470C, 16, 32)));
	AppendGraphics(start);
	
	GraphicsTreeParent calendar_stations("Calendar Stations");	

	GraphicsTreeParent calendar_spring("Spring", true);
	calendar_spring.push_back(GraphicsTreeItem("Piece 1", GraphicsInfo(0x6F5258, 0x6F5D1C, 32, 16)));
	calendar_spring.push_back(GraphicsTreeItem("Piece 2", GraphicsInfo(0x6F5358, 0x6F5D1C, 16, 16)));

	GraphicsTreeParent calendar_summer("Summer", true);
	calendar_summer.push_back(GraphicsTreeItem("Piece 1", GraphicsInfo(0x6F53d8, 0x6F5D1C, 32, 16)));
	calendar_summer.push_back(GraphicsTreeItem("Piece 2", GraphicsInfo(0x6F54d8, 0x6F5D1C, 16, 16)));

	GraphicsTreeParent calendar_fall("Fall", true);
	calendar_fall.push_back(GraphicsTreeItem("Piece 1", GraphicsInfo(0x6F5558, 0x6F5D1C, 16, 16)));
	calendar_fall.push_back(GraphicsTreeItem("Piece 2", GraphicsInfo(0x6F55d8, 0x6F5D1C, 8, 16)));
	calendar_fall.push_back(GraphicsTreeItem("Piece 3", GraphicsInfo(0x6F5618, 0x6F5D1C, 8, 16)));

	GraphicsTreeParent calendar_winter("Winter", true);
	calendar_winter.push_back(GraphicsTreeItem("Piece 1", GraphicsInfo(0x6F5658, 0x6F5D1C, 32, 16)));
	calendar_winter.push_back(GraphicsTreeItem("Piece 2", GraphicsInfo(0x6F5758, 0x6F5D1C, 16, 16)));

	calendar_stations.append(calendar_spring);
	calendar_stations.append(calendar_summer);
	calendar_stations.append(calendar_fall);
	calendar_stations.append(calendar_winter);
	AppendGraphics(calendar_stations);

	// AppendGraphics();
}

void GraphicsEditorFrame::FromOriginal()
{
	if (m_IsPieces)
	{
		GetGraphicsPieces(m_MountGraphics[m_MountIndex], m_RomOriginal);
	}
	else
	{
		GetGraphics(m_Index->second, m_RomOriginal);
	}
}

void GraphicsEditorFrame::ExportImage()
{
	wxString name = m_IsPieces ? m_MountGraphics[m_MountIndex].GetName() : m_pNavigator->GetItemText(m_Index->first);

	wxString path = wxSaveFileSelector(L"Image", L"png", name);

	if (path.empty())
		return;

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

	bitmap.SaveFile(path, wxBitmapType::wxBITMAP_TYPE_PNG);
}

void GraphicsEditorFrame::SaveImage()
{
	wxFileName fn;
	fn.SetPath(m_RootDir);
	fn.SetName(m_pNavigator->GetItemText(m_Index->first));
	fn.SetExt(L"bin");	

	std::vector<uint8_t> bytes;
	bytes.resize(((m_Index->second.m_Width * m_Index->second.m_Height) / (8 / m_Index->second.m_Bpp)));

	GRAPHICS_HEADER* header = (GRAPHICS_HEADER*)bytes.data();
	header->riff = 0x46464952;
	header->size = ((m_Index->second.m_Width * m_Index->second.m_Height) / (8 / m_Index->second.m_Bpp));
	header->bpp = m_Index->second.m_Bpp;

	m_RomTranslated.Seek(m_Index->second.m_ImageAdress);
	m_RomTranslated.Read(bytes.data() + sizeof GRAPHICS_HEADER, header->size);
	Moon::File::WriteAllBytes(fn.GetFullPath().ToStdString(), bytes);
}

void GraphicsEditorFrame::ImportImage()
{
	wxString path = wxFileSelector("Select a image to import", wxEmptyString, wxEmptyString, ".png", wxFileSelectorDefaultWildcardStr, OFN_FILEMUSTEXIST);

	wxBitmap bitmap(path, wxBitmapType::wxBITMAP_TYPE_PNG);

	wxMemoryDC dc(bitmap);

	const Palette& pal = m_Graphic.GetPalette();
	uint8_t* data = m_Graphic.GetData();

	for (size_t y = 0; y < bitmap.GetHeight(); ++y)
	{
		for (size_t x = 0; x < bitmap.GetWidth(); ++x)
		{
			wxColour color;
			dc.GetPixel(x, y, &color);

			size_t pos = pal.FindRGB(color.GetRGB());
			
			if (pos == std::string::npos)
			{
				//Todo: Find the closest color
				continue;
			}

			data[x + (y * m_Graphic.GetWidth())] = pos;
		}
	}

	m_ImageView->Refresh();
}

void GraphicsEditorFrame::OnSaveFileClick(wxCommandEvent& event)
{
	SaveImage();
	event.Skip();
}

void GraphicsEditorFrame::OnInsertClick(wxCommandEvent& event)
{
	InsertImage();
	event.Skip();
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
			ImportImage();
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
	event.Skip();

	wxTreeItemId id = event.GetItem();	

	for (size_t i = 0; i < m_MountGraphics.size(); ++i)
	{
		if (m_MountGraphics[i].m_Id == id)
		{
			GetGraphicsPieces(m_MountGraphics[i], m_RomTranslated);
			m_MountIndex = i;
			m_IsPieces = true;
			return;
		}
	}

	MAP::iterator it = m_LookUp.find(id);

	if (it != m_LookUp.end())
	{
		m_Index = it;
		m_IsPieces = false;
		GetGraphics(it->second, m_RomTranslated);
	}	
}

void GraphicsEditorFrame::SetRootName(const wxString& name)
{
	m_Root = m_pNavigator->AddRoot(name);
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
	m_Graphic.SetTileWidth(info.m_TileWidth);
	m_Graphic.SetTileHeight(info.m_TileHeight);

	m_Graphic.LoadFromRom(rom);

	m_ImageView->SetGraphics(&m_Graphic);
	m_PalCtrl->SetPal(m_Graphic.GetPalette());
	m_InfoViewer->SetInfo(info);
}

void GraphicsEditorFrame::GetGraphicsPieces(const GraphicsTreeParent& parent, RomFile& rom)
{
	wxSize size;
	std::vector<Graphics> graphics;

	size.y = parent[0].m_Info.m_Height;

	for (const GraphicsTreeItem& item : parent)
	{
		size.x += item.m_Info.m_Width;

		graphics.push_back(Graphics(item.m_Info.m_Width, item.m_Info.m_Height, item.m_Info.m_Bpp, item.m_Info.m_Reversed, item.m_Info.m_Planar, item.m_Info.m_TileWidth, item.m_Info.m_TileHeight));
		graphics.back().SetImgOffset(item.m_Info.m_ImageAdress);
		graphics.back().SetPalOffset(item.m_Info.m_PaletteAdress);
		graphics.back().SetPalOffset(item.m_Info.m_PaletteAdress);
	}

	for (Graphics& cur_graphics : graphics)
		cur_graphics.LoadFromRom(rom);

	unsigned char* bytes = new unsigned char[size.GetWidth() * size.GetHeight()];

	for (size_t y = 0, x = 0; y < size.GetHeight(); ++y, x = 0)
	{
		for (const Graphics& cur_graphics : graphics)
		{
			memcpy(bytes + (x + (y * size.GetWidth())), cur_graphics.GetData() + (y * cur_graphics.GetWidth()), cur_graphics.GetWidth());
			x += cur_graphics.GetWidth();
		}
	}

	m_Graphic = graphics[0];

	uint32_t palSize = (1 << m_Graphic.GetBpp()) * 2;
	uint8_t* rawPal = new uint8_t[palSize];
	rom.Seek(m_Graphic.GetPalOffset());
	rom.Read(rawPal, palSize);

	m_Graphic.GetPalette().DecodeColors((uint16_t*)rawPal, m_Graphic.GetBpp());

	m_Graphic.SetWidth(size.GetWidth());
	m_Graphic.SetHeight(size.GetHeight());
	m_Graphic.SetData(bytes);
	m_ImageView->SetGraphics(&m_Graphic);
	m_PalCtrl->SetPal(m_Graphic.GetPalette());
}

void GraphicsEditorFrame::InsertImage()
{
	m_Graphic.InsertImage(m_RomTranslated);
}

void GraphicsEditorFrame::AppendGraphics(const GraphicsTreeItem& item, const wxTreeItemId& id)
{
	m_LookUp.insert(std::make_pair(m_pNavigator->AppendItem(id, item.m_Name), item.m_Info));
}

void GraphicsEditorFrame::AppendGraphics(const GraphicsTreeItem& item)
{	
	AppendGraphics(item, m_Root);
}

void GraphicsEditorFrame::AppendGraphics(GraphicsTreeParent& parent)
{	
	AppendGraphics(parent, m_Root);

	//if (parent.CanMount())
	//{
	//	parent.m_Id = id;
	//	m_MountGraphics.push_back(parent);
	//}

	//for (const GraphicsTreeItem& item : parent)
	//{
	//	AppendGraphics(item, id);
	//}

	//if (parent.child_size() > 0)
	//{
	//	for (GraphicsTreeParent& child_parent : parent.get_childs())
	//	{
	//		id = m_pNavigator->AppendItem(id, child_parent.GetName());

	//		if (child_parent.CanMount())
	//		{
	//			child_parent.m_Id = id;
	//			m_MountGraphics.push_back(child_parent);
	//		}

	//		for (const GraphicsTreeItem& item : child_parent)
	//		{
	//			AppendGraphics(item, id);
	//		}			
	//	}
	//}
}

void GraphicsEditorFrame::AppendGraphics(GraphicsTreeParent& parent, wxTreeItemId gran_parent)
{
	wxTreeItemId id = m_pNavigator->AppendItem(gran_parent, parent.GetName());

	if (parent.CanMount())
	{
		parent.m_Id = id;
		m_MountGraphics.push_back(parent);
	}

	for (const GraphicsTreeItem& item : parent)
	{
		AppendGraphics(item, id);
	}

	if (parent.child_size() > 0)
	{
		for (GraphicsTreeParent& item : parent.get_childs())
		{
			AppendGraphics(item, id);
		}
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
	wxMenu* menuImage = new wxMenu();
	menuImage->Bind(wxEVT_MENU, &GraphicsEditorFrame::OnSaveFileClick, this, menuImage->Append(wxID_SAVE, "Save")->GetId());
	menuImage->Append(ID_IMPORT, "Import");
	menuImage->Append(ID_EXPORT, "Export");
	menuImage->Append(wxID_COPY, "From Original");
	menuImage->Bind(wxEVT_MENU, &GraphicsEditorFrame::OnInsertClick, this, menuImage->Append(wxNewId(), "Insert")->GetId());

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
	menuBar->Append(menuImage, "Image");
	menuBar->Append(viewMenu, "View");
	menuBar->Bind(wxEVT_MENU, &GraphicsEditorFrame::OnMenuBarClick, this);

	SetMenuBar(menuBar);

	SetBackgroundColour(wxColour(240, 240, 240));

	m_pLeftPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_RAISED);

	m_pNavigator = new wxTreeCtrl(m_pLeftPanel, wxID_ANY);
	m_pNavigator->SetSize(wxSize(200, m_pNavigator->GetSize().GetHeight()));
	m_pNavigator->SetMinSize(wxSize(200, m_pNavigator->GetSize().GetHeight()));	
	m_pNavigator->Bind(wxEVT_TREE_SEL_CHANGED, &GraphicsEditorFrame::OnSelChanged, this);

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