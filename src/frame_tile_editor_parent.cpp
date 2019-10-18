#include "frame_tile_editor_parent.hpp"

wxBEGIN_EVENT_TABLE(TileEditorFrame, wxMDIParentFrame)

EVT_MENU(wxID_OPEN, TileEditorFrame::OnMenuImport)
EVT_MENU(wxID_SAVE, TileEditorFrame::OnMenuExport)
EVT_MENU(ID_gui_menu_view_grid, TileEditorFrame::OnMenuGridPixel)
EVT_SIZE(TileEditorFrame::OnSize)
EVT_BUTTON(ID_gui_button_flipColor, TileEditorFrame::OnColorFlip)

wxEND_EVENT_TABLE()

TileEditorFrame::TileEditorFrame() : wxMDIParentFrame(nullptr, wxID_ANY, "Tile Editor")
{
	CreateGUIControls();	
}

TileEditorFrame::~TileEditorFrame()
{

}

void TileEditorFrame::OnMenuExport(wxCommandEvent& event)
{
	event.Skip();
}

void TileEditorFrame::OnMenuGridPixel(wxCommandEvent& event)
{
	if (GetActiveChild() != nullptr)
	{
		((TileEditorChildFrame*)GetActiveChild())->canvas->ShowPixelGrid();
	}
	event.Skip();
}

void TileEditorFrame::OnSize(wxSizeEvent& event)
{
	int w, h;
	GetClientSize(&w, &h);
	
	GetClientWindow()->SetSize(0, 0, w, h - 100);

	if (gui_downBar != nullptr) //The window is resized before the panel creation...
	{
		gui_downBar->SetSize(w, 100);
		gui_downBar->SetPosition(wxPoint(0, h - 100));
	}			

	// FIXME: On wxX11, we need the MDI frame to process this
	// event, but on other platforms this should not
	// be done.
#ifdef __WXUNIVERSAL__
	event.Skip();
#else
	wxUnusedVar(event);
#endif
}

void TileEditorFrame::OnColorFlip(wxCommandEvent& event)
{
	//gui_downBar_color_1->SetBackgroundColour(((TileEditorChildFrame*)GetActiveChild())->canvas->palette[button_color2]);
	//gui_downBar_color_2->SetBackgroundColour(((TileEditorChildFrame*)GetActiveChild())->canvas->palette[button_color1]);

	if (GetActiveChild() != nullptr)
	{
		button_color1++;
		((TileEditorChildFrame*)GetActiveChild())->canvas->SetColour(button_color1);
	}	
}

void TileEditorFrame::OnChildFocus(wxFocusEvent& event)
{
	UpdatePalette();
	event.Skip();
}

void TileEditorFrame::OnColorPanelClick(wxMouseEvent& event)
{
	if (GetActiveChild() != nullptr)
	{
		((TileEditorChildFrame*)GetActiveChild())->canvas->SetColour(event.GetId() - 3000);

		//
		//Stopped here
		//Start to make a fuction to update the canvas color, and change the color 0 in the button.
		//
	}		

	event.Skip();
}

void TileEditorFrame::OnMenuImport(wxCommandEvent& event)
{
	TileEditorChildFrame* frame = new TileEditorChildFrame(this, "Test");
	frame->Bind(wxEVT_SET_FOCUS, &TileEditorFrame::OnChildFocus, this);
	frame->Show();
	event.Skip();
}

void TileEditorFrame::CreatePalette()
{
	gui_palette_color.resize(16, nullptr);

	for (int i = 0; i < 16; ++i)
	{
		gui_palette_color[i] = new wxPanel(gui_downBar, 3000 + i, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
		gui_palette_color[i]->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
		gui_palette_color[i]->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
		gui_palette_color[i]->SetMinSize(wxSize(32, 32));
		gui_palette_color[i]->Bind(wxEVT_LEFT_DOWN, &TileEditorFrame::OnColorPanelClick, this);
		gui_downBar_sizer->Add(gui_palette_color[i], 0, wxRIGHT, 5);		
	}
}

void TileEditorFrame::UpdatePalette()
{
	if (GetActiveChild() != nullptr)
	{
		for (int i = 0; i < 16; ++i)
		{
			gui_palette_color[i]->SetBackgroundColour(((TileEditorChildFrame*)GetActiveChild())->canvas->palette[i]);
			gui_palette_color[i]->Refresh();
		}
	}

}

void TileEditorFrame::CreateGUIControls()
{
	gui_menu_file = new wxMenu();		
	gui_menu_file->Append(wxID_OPEN, _("Import"));
	gui_menu_file->Append(wxID_SAVE, _("Export"));	
	
	gui_menu_view = new wxMenu();
	gui_menu_view_grid = gui_menu_view->AppendCheckItem(ID_gui_menu_view_grid, _("Grid > Pixels"));	

	gui_menu = new wxMenuBar();
	gui_menu->Append(gui_menu_file, _("File"));
	gui_menu->Append(gui_menu_view, _("View"));
	this->SetMenuBar(gui_menu);	

	wxBitmap gui_color_change_bimap = wxBitmap::NewFromPNGData(resource_button_change_color, (sizeof(resource_button_change_color) / sizeof(resource_button_change_color[0])));

	int w, h;
	GetClientSize(&w, &h);
	
	gui_downBar = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	gui_downBar->SetSize(w, 100);
	gui_downBar->SetBackgroundColour(wxColour(255, 255, 255));
	gui_downBar->SetPosition(wxPoint(0, h - 100));

	gui_downBar_color_1 = new wxPanel(gui_downBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	gui_downBar_color_1->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	gui_downBar_color_1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));

	gui_downBar_color_2 = new wxPanel(gui_downBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	gui_downBar_color_2->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
	gui_downBar_color_2->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));

	gui_downBar_color_change = new wxBitmapButton(gui_downBar, ID_gui_button_flipColor, gui_color_change_bimap);

	gui_downBar_colorButtons_sizer = new wxBoxSizer(wxVERTICAL);
	gui_downBar_colorButtons_sizer->Add(gui_downBar_color_1, 1, wxEXPAND, 5);
	gui_downBar_colorButtons_sizer->Add(gui_downBar_color_change, 1, wxALIGN_LEFT, 5);

	gui_downBar_colorButtons_sizer2 = new wxBoxSizer(wxVERTICAL);
	gui_downBar_colorButtons_sizer2->Add(32, 32, 1, wxALIGN_LEFT, 5);
	gui_downBar_colorButtons_sizer2->Add(gui_downBar_color_2, 1, wxEXPAND, 5);	
	
	gui_downBar_sizer = new wxBoxSizer(wxHORIZONTAL);
	gui_downBar_sizer->Add(gui_downBar_colorButtons_sizer, 0, wxALIGN_LEFT, 5);
	gui_downBar_sizer->Add(gui_downBar_colorButtons_sizer2, 0, wxALIGN_LEFT, 5);

	//gui_palette_color.resize(16, nullptr);

	//for (int i = 0; i < 16; ++i)
	//{
	//	gui_palette_color.data()[i] = new wxPanel(gui_downBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	//	gui_palette_color.data()[i]->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
	//	gui_palette_color.data()[i]->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));

	//	gui_downBar_sizer->Add(gui_palette_color.data()[i], 0, wxALIGN_LEFT, 5);
	//}

	//for (int i = 0; i < 16; ++i)
	//{
	//	wxPanel* pa = new wxPanel(gui_downBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	//	//pa->SetSize();
	//	pa->SetMinSize(wxSize(32, 32));
	//	pa->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
	//	pa->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));

	//	gui_downBar_sizer->Add(pa, 0, wxRIGHT, 5);
	//}

	CreatePalette();

	gui_downBar->SetSizer(gui_downBar_sizer);
	gui_downBar->Layout();
}