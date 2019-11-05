#pragma once

#include <wx/wx.h>

#include "frame_tile_editor_child.hpp"

#include "resource_frame_tile_editor.hpp"

class TileEditorFrame : public wxMDIParentFrame
{
public:
	TileEditorFrame();
	~TileEditorFrame();

//Events
private:
	wxDECLARE_EVENT_TABLE();
		
	void OnMenuImport(wxCommandEvent& event);
	void OnMenuExport(wxCommandEvent& event);
	void OnMenuGridPixel(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnColorFlip(wxCommandEvent& event);
	void OnChildFocus(wxFocusEvent& event);
	void OnColorPanelClick(wxMouseEvent& event);
private:
	int selColor = 0;

	int button_color1 = 0;
	int button_color2 = 1;

	void UpdatePalette();
//GUI
private:

	void CreateGUIControls();
	void CreatePalette();

	wxToolBar* gui_toolbar = nullptr;
	wxMenuBar* gui_menu = nullptr;
	wxMenu* gui_menu_file = nullptr;
	wxMenu* gui_menu_view = nullptr;
	wxMenuItem* gui_menu_view_grid = nullptr;

	wxPanel* gui_downBar_color_1 = nullptr;
	wxPanel* gui_downBar_color_2 = nullptr;
	wxBitmapButton* gui_downBar_color_change = nullptr;
	wxBoxSizer* gui_downBar_colorButtons_sizer = nullptr;
	wxBoxSizer* gui_downBar_colorButtons_sizer2 = nullptr;
	wxBoxSizer* gui_downBar_sizer = nullptr;
	wxPanel* gui_downBar = nullptr;	

	std::vector<wxPanel*> gui_palette_color;


	enum ID {
		ID_gui_menu_view_grid = 10001,
		ID_gui_button_flipColor = 10002
	};
};

