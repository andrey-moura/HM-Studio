#include "control_graphics_view.hpp"

#include <moon/diagnostics.hpp>

GraphicsView::GraphicsView(wxWindow* parent) : wxHVScrolledWindow(parent, wxID_ANY), m_BlockSize(2, 2)
{
	Bind(wxEVT_PAINT, &GraphicsView::OnPaintEvent, this);
	SetBackgroundColour(parent->GetBackgroundColour());	
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetDoubleBuffered(true);	
	SetViewOnly(m_ViewOnly);
}

void GraphicsView::SetGraphics(Graphics* graphic)
{				
	m_Graphic = graphic;
	m_8bppData = graphic->GetData();
	SetRowColumnCount(m_Graphic->GetHeight(), m_Graphic->GetWidth());

	m_Bitmap.Create(wxSize(graphic->GetWidth(), graphic->GetHeight()), 24);

	wxNativePixelData data(m_Bitmap);
	wxNativePixelData::Iterator it = data.GetPixels();

	const Palette& pal = graphic->GetPalette();	

	uint32_t graphics_index = 0;

	for (size_t y = 0; y < data.m_height; ++y)
	{
		wxNativePixelData::Iterator rowStart = it;

		for (size_t x = 0; x < data.m_width; ++x)
		{			
			it.Red() = pal[m_8bppData[graphics_index]].red;
			it.Green() = pal[m_8bppData[graphics_index]].green;
			it.Blue() = pal[m_8bppData[graphics_index]].blue;

			++graphics_index;
			++it;
		}

		it = rowStart;
		it.OffsetY(data, 1);
	}

	Refresh();
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

	Refresh();
}

void GraphicsView::SetPixelGrid(bool use)
{
	m_PixelGrid = use;

	float time = Moon::Diagnostics::FunctionTimerMember(&GraphicsView::Refresh, this, false, nullptr) / 1000.f;

	std::string();
}

void GraphicsView::SetTileGrid(bool use)
{
	m_UseTileGrid = use;
	Refresh();
}

void GraphicsView::SetBlockGrid(bool use)
{
	m_UseBlockGrid = use;
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
	SetTileGrid(!m_UseTileGrid);
	Refresh();
	return GetTileGrid();
}

bool GraphicsView::ToggleBlockGrid()
{
	SetBlockGrid(!m_UseBlockGrid);
	Refresh();
	return GetBlockGrid();
}

void GraphicsView::SetViewOnly(bool set)
{
	if (set)
	{
		SetCursor(wxCURSOR_OPEN_HAND);		
		Unbind(wxEVT_LEFT_DOWN, &GraphicsView::OnMouseDown, this);
		Unbind(wxEVT_RIGHT_DOWN, &GraphicsView::OnMouseDown, this);
		Unbind(wxEVT_MOTION, &GraphicsView::OnMouseDown, this);
	}
	else
	{
		SetCursor(wxCURSOR_DEFAULT);		
		Bind(wxEVT_LEFT_DOWN, &GraphicsView::OnMouseDown, this);
		Bind(wxEVT_RIGHT_DOWN, &GraphicsView::OnMouseDown, this);
		Bind(wxEVT_MOTION, & GraphicsView::OnMouseDown, this);
	}
}

void GraphicsView::OnDraw(wxDC& dc)
{
	dc.Clear();

	if (m_Graphic == nullptr)
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

	wxPosition posStart = GetVisibleBegin();
	wxPosition posEnd = GetVisibleEnd();

	dc.SetUserScale(m_Scale, m_Scale);
	dc.DrawBitmap(m_Bitmap, {0,0});

	dc.SetUserScale(1, 1);

	if (m_UseTileGrid)
	{	
		DrawTileGrid(dc);
	}		

	if (m_UseBlockGrid)
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
		colStart = m_Graphic->GetTileWidth();
	}
	else
	{
		while (colStart % m_Graphic->GetTileWidth() != 0)
			++colStart;
	}	

	if (rowStart == 0)
	{
		rowStart = m_Graphic->GetTileHeight();
	}
	else
	{
		while (rowStart % m_Graphic->GetTileHeight() != 0)
			++rowStart;
	}

	//vertical lines
	for (size_t col = colStart; col < posEnd.GetCol(); col += m_Graphic->GetTileWidth())
	{
		dc.DrawLine(wxPoint(col, posStart.GetRow()) * m_Scale, wxPoint(col, posEnd.GetRow()) * m_Scale);
	}

	//horizontal lines
	for (size_t row = rowStart; row < posEnd.GetRow(); row += m_Graphic->GetTileHeight())
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

void GraphicsView::OnMouseDown(wxMouseEvent& event)
{
	if (m_Graphic != nullptr)
	{
		wxPosition startPos = GetVisibleBegin();
		wxPosition curPos((event.GetPosition().y / m_Scale) + startPos.GetRow(), (event.GetPosition().x / m_Scale) + startPos.GetCol());

		if (curPos.GetColumn() < m_Graphic->GetWidth() && curPos.GetRow() < m_Graphic->GetHeight())
		{
			if (!event.Moving())
			{
				uint8_t newColor = event.ButtonIsDown(wxMOUSE_BTN_LEFT) ? m_Color1 : m_Color2;

				size_t index = (curPos.GetCol() + (curPos.GetRow() * m_Graphic->GetWidth()));
				uint8_t oldColor = m_8bppData[index];

				if (oldColor != newColor)
				{
					m_8bppData[index] = newColor;

					const Palette& pal = m_Graphic->GetPalette();

					wxNativePixelData data(m_Bitmap);
					wxNativePixelData::Iterator it = data.GetPixels();

					it.MoveTo(data, curPos.GetCol(), curPos.GetRow());

					it.Red() = pal[m_8bppData[index]].red;
					it.Green() = pal[m_8bppData[index]].green;
					it.Blue() = pal[m_8bppData[index]].blue;

					Refresh();
				}
			}
		}
	}

	event.Skip();
}

bool GraphicsView::IsTouchingGrid(wxPoint pixel, size_t grid)
{
	return (pixel.x != 0 && pixel.x % grid == 0) ||
		(pixel.y != 0 && pixel.y % grid == 0) ||
		((pixel.x + 1) != 0 && (pixel.x + 1) % grid == 0) ||
		((pixel.y + 1) != 0 && (pixel.y + 1) % grid == 0);
}

wxCoord GraphicsView::OnGetRowHeight(size_t row) const
{
	return m_Scale;
}

wxCoord GraphicsView::OnGetColumnWidth(size_t row) const
{
	return m_Scale;
}