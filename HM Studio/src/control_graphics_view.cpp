#include "control_graphics_view.hpp"

GraphicsView::GraphicsView(wxWindow* parent) : wxHVScrolledWindow(parent, wxID_ANY), m_BlockSize(2, 2)
{
	Bind(wxEVT_PAINT, &GraphicsView::OnPaintEvent, this);
	SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void GraphicsView::UpdateImage(Graphics& graphics)
{				
	m_Image = graphics.ToImage();	
	SetRowColumnCount(m_Image.GetHeight(), m_Image.GetWidth());

	Refresh();
}

wxImage GraphicsView::GetImage() const
{
	return m_Image.ConvertToImage();
}

void GraphicsView::SetScale(size_t scale)
{
	if (scale == m_Scale)
		return;

	if (scale == 0)
		return;	

	m_Scale = scale;	

	wxVarHScrollHelper::RefreshAll();
	wxVarVScrollHelper::RefreshAll();
	
	//GetParent()->Layout();

	Refresh();
}

void GraphicsView::SetPixelGrid(bool use)
{
	m_PixelGrid = use;
	Refresh();
}

void GraphicsView::SetTileGrid(bool use)
{
	m_TileGrid = use;
	Refresh();
}

void GraphicsView::SetBlockGrid(bool use)
{
	m_BlockGrid = use;
	Refresh();
}

bool GraphicsView::TogglePixelGrid()
{
	SetPixelGrid(!m_PixelGrid);	
	Refresh();
	return GetPixelGrid();
}

bool GraphicsView::ToggleTileGrid()
{
	SetTileGrid(!m_TileGrid);
	Refresh();
	return GetTileGrid();
}

bool GraphicsView::ToggleBlockGrid()
{
	SetBlockGrid(!m_BlockGrid);
	Refresh();
	return GetBlockGrid();
}

void GraphicsView::OnDraw(wxDC& dc)
{
	dc.Clear();

	if (!m_Image.IsOk())
		return;		

	if((m_PixelGrid && m_Scale >= 8))
	{		
		wxPen pixelPen(GRID_PIXEL_COLOR, GRID_PIXEL_SIZE, wxPENSTYLE_SOLID);
		dc.SetPen(pixelPen);
	}
	else
	{
		dc.SetPen(*wxTRANSPARENT_PEN);
	}	

	wxBrush brush = dc.GetBrush();
	brush.SetStyle(wxBRUSHSTYLE_SOLID);

	wxMemoryDC memoryDC(m_Image);

	wxPosition posStart = GetVisibleBegin();
	wxPosition posEnd = GetVisibleEnd();

	for (size_t y = posStart.GetRow(); y < posEnd.GetRow(); ++y)
	{		
		for (size_t x = posStart.GetCol(); x < posEnd.GetCol(); ++x)
		{
			wxColour colour;
			if (memoryDC.GetPixel(x, y, &colour))
			{								
				brush.SetColour(colour);
				dc.SetBrush(brush);				
				dc.DrawRectangle(wxPoint(x * m_Scale, y * m_Scale), wxSize(m_Scale, m_Scale));
			}			
		}
	}

	if (m_TileGrid)
		DrawTileGrid(dc);

	if (m_BlockGrid)
		DrawBlockGrid(dc);
}

void GraphicsView::DrawTileGrid(wxDC& dc)
{
	wxPen pen(GRID_TILE_COLOR, GRID_TILE_SIZE, wxPENSTYLE_SOLID);

	dc.SetPen(pen);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	wxPosition posStart = GetVisibleBegin();
	wxPosition posEnd = GetVisibleEnd();

	size_t colStart = posStart.GetCol();
	size_t rowStart = posStart.GetRow();

	if (colStart == 0)
	{
		colStart = 8;
	}
	else
	{
		while (colStart % 8 != 0)
			++colStart;
	}	

	if (rowStart == 0)
	{
		rowStart = 8;
	}
	else
	{
		while (rowStart % 8 != 0)
			++rowStart;
	}

	//vertical lines
	for (size_t col = colStart; col < posEnd.GetCol(); col += 8)
	{
		dc.DrawLine(wxPoint(col, posStart.GetRow()) * m_Scale, wxPoint(col, posEnd.GetRow()) * m_Scale);
	}

	//horizontal lines
	for (size_t row = rowStart; row < posEnd.GetRow(); row += 8)
	{
		dc.DrawLine(wxPoint(posStart.GetCol(), row) * m_Scale, wxPoint(posEnd.GetCol(), row) * m_Scale);
	}
}

void GraphicsView::DrawBlockGrid(wxDC& dc)
{
	wxPen pen(GRID_BLOCK_COLOR, GRID_BLOCK_SIZE, wxPENSTYLE_SOLID);
	dc.SetPen(pen);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	wxPosition posStart = GetVisibleBegin();
	wxPosition posEnd = GetVisibleEnd();

	size_t colStart = posStart.GetCol();
	size_t rowStart = posStart.GetRow();

	if (colStart == 0)
	{
		colStart = m_BlockSize.GetWidth() * 8;
	}
	else
	{
		while (colStart % (m_BlockSize.GetWidth() * 8) != 0)
			++colStart;
	}

	if (rowStart == 0)
	{
		rowStart = m_BlockSize.GetHeight() * 8;
	}
	else
	{
		while (rowStart % (m_BlockSize.GetHeight() * 8) != 0)
			++rowStart;
	}

	//vertical lines
	for (size_t col = colStart; col < posEnd.GetCol(); col += (m_BlockSize.GetWidth() * 8))
	{
		dc.DrawLine(wxPoint(col, posStart.GetRow()) * m_Scale, wxPoint(col, posEnd.GetRow()) * m_Scale);
	}

	//horizontal lines
	for (size_t row = rowStart; row < posEnd.GetRow(); row += (m_BlockSize.GetHeight() * 8))
	{
		dc.DrawLine(wxPoint(posStart.GetCol(), row) * m_Scale, wxPoint(posEnd.GetCol(), row) * m_Scale);
	}
}

void GraphicsView::OnPaintEvent(wxPaintEvent& event)
{
	wxBufferedPaintDC dc(this);
	PrepareDC(dc);
	OnDraw(dc);
}

wxCoord GraphicsView::OnGetRowHeight(size_t row) const
{
	return m_Scale;
}

wxCoord GraphicsView::OnGetColumnWidth(size_t row) const
{
	return m_Scale;
}