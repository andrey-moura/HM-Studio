#pragma once

#include <wx/vscroll.h>
#include <wx/dcbuffer.h>
#include <wx/bitmap.h>

#include "class_graphics.hpp"

#define GRID_PIXEL_COLOR wxColour(128, 128, 128)
#define GRID_PIXEL_SIZE 1
#define GRID_TILE_COLOR wxColour(255, 0, 0)
#define GRID_TILE_SIZE 1
#define GRID_BLOCK_COLOR wxColour(0, 0, 255)
#define GRID_BLOCK_SIZE 2

class GraphicsView : public wxHVScrolledWindow
{
public:
	GraphicsView(wxWindow* parent);
private:	
	size_t m_Scale = 1;
	Graphics* m_Graphic = nullptr;
	bool m_PixelGrid = false;
	bool m_TileGrid = false;
	bool m_BlockGrid = false;

	wxSize m_BlockSize;
public:
	void SetGraphics(Graphics* graphic);	
	void SetScale(size_t scale);
	size_t GetScale() const { return m_Scale; }

	void SetPixelGrid(bool use = true);
	void SetTileGrid(bool use = true);
	void SetBlockGrid(bool use = true);

	bool GetPixelGrid() const { return m_PixelGrid; }
	bool GetTileGrid() const { return m_TileGrid; }
	bool GetBlockGrid() const { return m_BlockGrid; }

	bool TogglePixelGrid();
	bool ToggleTileGrid();
	bool ToggleBlockGrid();

	void DrawTileGrid(wxDC& dc);
	void DrawBlockGrid(wxDC& dc);
//override
private:
	virtual wxCoord OnGetRowHeight(size_t row) const;
	virtual wxCoord OnGetColumnWidth(size_t col) const;
private:
	//void DrawTileGrid(wxDC& dc);
private:
	void OnPaintEvent(wxPaintEvent& event);
	void OnDraw(wxDC& dc);
};