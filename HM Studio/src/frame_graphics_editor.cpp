#include "frame_graphics_editor.hpp"

GraphicsEditorFrame::GraphicsEditorFrame(id i) : wxFrame(nullptr, wxID_ANY, "Graphics Editor"), m_RomOriginal(i, false), m_RomTranslated(i, true)
{
	CreateGUIControls();	
	m_Root = m_pNavigator->AddRoot("Graphics");
	GetGraphicsList();
}

//void GraphicsEditorFrame::RomFromDialog()
//{
//	wxFileDialog dialog(nullptr, "Select a valid Harvest Moon game");
//
//	if (dialog.ShowModal() == wxCANCEL)
//		return;
//
//	std::string path = dialog.GetPath().ToStdString();
//
//	//Rom rom = Rom(path);
//
//	if (rom.IsOpened())
//	{
//		GetGraphicsList();
//	}
//}

void GraphicsEditorFrame::GetGraphicsList()
{	
	GraphicsTreeParent calendary("Calendary");
	calendary.push_back(GraphicsTreeItem("Numbers", GraphicsInfo(0x6F4C58, 0x6F5D1C, 16, 160)));
	AppendGraphics(calendary);

	GraphicsTreeItem clock("Clock", GraphicsInfo(0x70017c, 0x6CDD04, 256, 32));
	AppendGraphics(clock);

	GraphicsTreeItem balloons("Balloons", GraphicsInfo(0x6C316c, 0x6C40F0, 16, 480));
	AppendGraphics(balloons);

/*	wxTreeItemId calendaryRoot = m_pNavigator->AppendItem(root ,"Calendary");
	wxTreeItemId numbers = m_pNavigator->AppendItem(calendaryRoot, "Numbers");	
	wxTreeItemId mon = m_pNavigator->AppendItem(calendaryRoot, "Mon");
	wxTreeItemId tue = m_pNavigator->AppendItem(calendaryRoot, "Tue");
	wxTreeItemId wed = m_pNavigator->AppendItem(calendaryRoot, "Wed");
	wxTreeItemId thu = m_pNavigator->AppendItem(calendaryRoot, "Thu");
	wxTreeItemId fri = m_pNavigator->AppendItem(calendaryRoot, "Fri");
	wxTreeItemId sat = m_pNavigator->AppendItem(calendaryRoot, "Sat");	*/
}

void GraphicsEditorFrame::FromOriginal()
{
	GetGraphics(m_Index->second, m_RomOriginal);
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
			break;
		case wxID_COPY:
			FromOriginal();
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
	uint8_t* bytes = new uint8_t[(info.m_Width * info.m_Height) / 2];
	
	rom.Seek(info.m_ImageAdress);
	rom.Read(bytes, (info.m_Width * info.m_Height) / 2);

	uint16_t* palette = new uint16_t[256];

	rom.Seek(info.m_PaletteAdress);
	rom.Read(palette, 256 * 2);

	m_Graphic = Graphics(bytes, 4, info.m_Width, info.m_Height);
	m_Graphic.DecodePalette((uint8_t*)palette);
		
	m_ImageView->UpdateImage(m_Graphic);
	Layout();
}

void GraphicsEditorFrame::SaveImage()
{
	m_Graphic.Encode(m_ImageView->GetImage());
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

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(menuFile, "File");
	menuBar->Append(menuImage, "Image");
	menuBar->Append(viewMenu, "View");

	menuBar->Bind(wxEVT_MENU, &GraphicsEditorFrame::OnMenuBarClick, this);

	SetMenuBar(menuBar);

	m_pNavigator = new wxTreeCtrl(this, wxID_ANY);
	m_pNavigator->Bind(wxEVT_TREE_SEL_CHANGED, &GraphicsEditorFrame::OnSelChanged, this);

	wxBoxSizer* rootSizer = new wxBoxSizer(wxHORIZONTAL);

	m_ImageView = new GraphicsView(this);	

	rootSizer->Add(m_pNavigator, 0, wxEXPAND, 0);
	rootSizer->Add(m_ImageView,  0, wxEXPAND, 0);

	SetSizer(rootSizer);
}

GraphicsInfo::GraphicsInfo() : 
	m_Width(16), m_Height(16), m_Bpp(4), m_Reversed(false), m_Planar(false)
{

}

GraphicsInfo::GraphicsInfo(uint32_t imageAdress, uint32_t paletteOffset, short width, short height)
	: m_ImageAdress(imageAdress), m_PaletteAdress(paletteOffset), m_Width(width), m_Height(height), m_Bpp(4), m_Reversed(false), m_Planar(false)
{

}

GraphicsTreeItem::GraphicsTreeItem(const std::string& name, const GraphicsInfo& info) : m_Name(name), m_Info(info)
{

}

GraphicsTreeParent::GraphicsTreeParent(const std::string& name) : m_Name(name), std::vector<GraphicsTreeItem>()
{

}