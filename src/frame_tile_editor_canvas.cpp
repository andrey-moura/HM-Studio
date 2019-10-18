#include "frame_tile_editor_canvas.hpp"

wxBEGIN_EVENT_TABLE(TileEditorCanvas, wxHVScrolledWindow)

EVT_PAINT(TileEditorCanvas::OnPaint)
EVT_LEFT_DOWN(TileEditorCanvas::OnMouseLeftDown)
EVT_MOTION(TileEditorCanvas::OnMouseMotion)

wxEND_EVENT_TABLE()

TileEditorCanvas::TileEditorCanvas(wxWindow* parent) : wxHVScrolledWindow(parent, wxID_ANY)
{
	SetRowColumnCount(40, 40);
	SetBackgroundStyle(wxBG_STYLE_PAINT);

	palette.resize(16, wxColour(0, 0, 0));
	
	palette[1] = wxColour(0, 0, 255);
	palette[2] = wxColour(0, 255, 0);
	palette[3] = wxColour(255, 0, 0);
	palette[4] = wxColour(255, 255, 255);
	palette[5] = wxColour(255, 0, 255);
	palette[6] = wxColour(0, 255, 255);
	palette[7] = wxColour(255, 255, 0);
	palette[8] = wxColour(50, 0, 255);
	palette[9] = wxColour(0, 50, 255);
	palette[10] = wxColour(255, 0, 50);
	palette[11] = wxColour(255, 50, 0);
	palette[11] = wxColour(0, 125, 255);
	palette[12] = wxColour(125, 0, 255);
	palette[13] = wxColour(0, 25, 7);
	palette[14] = wxColour(5, 4, 44);

}

TileEditorCanvas::~TileEditorCanvas()
{
}

void TileEditorCanvas::SetPixelSize(int s)
{
	pixelSize = s;
	wxVarHScrollHelper::RefreshAll();
	wxVarVScrollHelper::RefreshAll();
	Refresh();
}

void TileEditorCanvas::SetPixels(int rows, int columns, unsigned char* pixels)
{
	pPixels = pixels;
	this->SetRowColumnCount(rows, columns);
}

wxCoord TileEditorCanvas::OnGetColumnWidth(size_t col) const
{
	return wxCoord(pixelSize);
}

wxCoord TileEditorCanvas::OnGetRowHeight(size_t row) const
{
	return wxCoord(pixelSize);
}

void TileEditorCanvas::SetColour(int c)
{
	selColor = c;
}

void TileEditorCanvas::ShowPixelGrid()
{
	usePixelGrid = true;
	this->Refresh(false);
}

void TileEditorCanvas::OnDraw(wxDC& dc)
{	
	dc.Clear();

	wxBrush brush = dc.GetBrush();
	wxPen pen = dc.GetPen();

	wxPosition begin = GetVisibleBegin();
	wxPosition end = GetVisibleEnd();

	pen.SetStyle(wxPENSTYLE_SOLID);
	pen.SetWidth(1);
	pen.SetColour(wxColour(200, 200, 200));
	dc.SetPen(pen);

	if (!usePixelGrid) dc.SetPen(*wxTRANSPARENT_PEN);

	for (int y = begin.GetRow(); y < end.GetRow(); ++y)
	{
		for (int x = begin.GetCol(); x < end.GetCol(); ++x)
		{
			brush.SetColour(palette[pPixels[y * GetColumnCount() + x]]);
			brush.SetStyle(wxBRUSHSTYLE_SOLID);
			dc.SetBrush(brush);			
			dc.DrawRectangle(x * pixelSize, y * pixelSize, pixelSize, pixelSize);
		}
	}

}

void TileEditorCanvas::OnPaint(wxPaintEvent& event)
{
	wxBufferedPaintDC dc(this);
	this->PrepareDC(dc);
	this->OnDraw(dc);
}

void TileEditorCanvas::OnMouseLeftDown(wxMouseEvent& event)
{
	wxPosition begin = GetVisibleBegin();
	pPixels[(event.GetY() / pixelSize + begin.GetRow()) * this->GetColumnCount() + (event.GetX() / pixelSize + begin.GetCol())] = selColor;

	this->Refresh(false);
	event.Skip();
}

void TileEditorCanvas::OnMouseMotion(wxMouseEvent& event)
{
	if (event.Dragging())
	{
		wxPosition begin = GetVisibleBegin();
		pPixels[(event.GetY() / pixelSize + begin.GetRow()) * this->GetColumnCount() + (event.GetX() / pixelSize + begin.GetCol())] = selColor;

		this->Refresh(false);
	}

	event.Skip();
}
