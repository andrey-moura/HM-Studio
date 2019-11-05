#include "frame_tile_editor_child.hpp"

wxBEGIN_EVENT_TABLE(TileEditorChildFrame, wxMDIChildFrame)

EVT_SLIDER(ID_slider, TileEditorChildFrame::OnZoomChange)

wxEND_EVENT_TABLE()

TileEditorChildFrame::TileEditorChildFrame(wxMDIParentFrame* parent, wxString sName) : wxMDIChildFrame(parent, wxID_ANY, sName)
{
	canvas = new TileEditorCanvas(this);
	
	statusBar = this->CreateStatusBar(2, wxSTB_DEFAULT_STYLE, wxID_ANY);
	slider = new wxSlider(statusBar, ID_slider, 8, 1, 32);

	New(16, 16);
}

TileEditorChildFrame::~TileEditorChildFrame()
{

}

void TileEditorChildFrame::OnZoomChange(wxCommandEvent& event)
{
	statusBar->SetStatusText(wxString("Zoom: ") << slider->GetValue(), 1);
	canvas->SetPixelSize(slider->GetValue());
	event.Skip();
}

void TileEditorChildFrame::SetColour(int c)
{
}

bool TileEditorChildFrame::Save(wxString filename)
{
	return false;
}

bool TileEditorChildFrame::Open(wxString filename)
{
	return false;
}

bool TileEditorChildFrame::New(int r, int c)
{
	delete[] pixels;
	pixels = new unsigned char[r * c]{ 0 };
	pixels[32] = 0x05;
	canvas->SetPixels(r, r, pixels);

	return false;
}