#pragma once
#include "wx/wx.h"

#include "frame_tile_editor_canvas.hpp"

class TileEditorChildFrame : public wxMDIChildFrame
{
public:
	TileEditorChildFrame(wxMDIParentFrame* parent, wxString sName);
	~TileEditorChildFrame();

//Events/Override
private:
	wxDECLARE_EVENT_TABLE();
	void OnZoomChange(wxCommandEvent& event);

public:
	TileEditorCanvas* canvas = nullptr;
	void SetColour(int c);
	bool Save(wxString filename);
	bool Open(wxString filename);
	bool New(int r, int c);

private:
	unsigned char* pixels = nullptr;

private:	
	wxStatusBar* statusBar = nullptr;
	wxSlider* slider = nullptr;	

	enum ID {
		ID_slider = 10001
	};
};

