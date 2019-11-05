#pragma once

#include <wx/wx.h>
#include <wx/vscroll.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>

class TileEditorCanvas : public wxHVScrolledWindow
{
public:
	TileEditorCanvas(wxWindow* parent);
	~TileEditorCanvas();

private:
	int pixelSize = 8;	
	bool usePixelGrid = false;
	unsigned char* pPixels = nullptr;	
	int selColor = 0;

public:
	void SetPixelSize(int s);
	void SetPixels(int rows, int columns, unsigned char* pixels);
	void SetColour(int c);
	void ShowPixelGrid();

	std::vector<wxColour> palette;

private:
	virtual wxCoord OnGetColumnWidth(size_t col) const;
	virtual wxCoord OnGetRowHeight(size_t col) const;
	void OnDraw(wxDC& dc);
	void OnPaint(wxPaintEvent& event);
	void OnMouseLeftDown(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
//Events
private:
	wxDECLARE_EVENT_TABLE();
};

