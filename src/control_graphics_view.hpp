#pragma once

#include <wx/vscroll.h>
#include <wx/dcbuffer.h>
#include <wx/bitmap.h>
#include <wx/rawbmp.h>

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
	Graphics* m_Graphics = nullptr;
	Palette* m_pPalette = nullptr;
	uint8_t* m_8bppData = nullptr;
	bool m_PixelGrid = false;
	bool m_UseTileGrid = false;
	bool m_UseBlockGrid = false;

	wxSize m_BlockSize;

	uint8_t m_Color1 = 1;
	uint8_t m_Color2 = 0;

	bool m_ViewOnly = false;

	void FlushBitmap();

	wxBitmap m_Bitmap;
	wxBitmap m_TileGrid;
	wxBitmap m_BlockGrid;
public:
	void SetGraphics(Graphics* graphic);	
	void SetPalette(Palette* palette);

	const wxBitmap& GetBitmap() { return m_Bitmap; }

	void SetScale(size_t scale);
	size_t GetScale() const { return m_Scale; }

	void SetPixelGrid(bool use = true);
	void SetTileGrid(bool use = true);
	void SetBlockGrid(bool use = true);

	void SetColor1(uint8_t color) { m_Color1 = color; }
	void SetColor2(uint8_t color) { m_Color2 = color; }

	bool GetPixelGrid() const { return m_PixelGrid; }
	bool GetTileGrid() const { return m_UseTileGrid; }
	bool GetBlockGrid() const { return m_UseBlockGrid; }

	bool TogglePixelGrid();
	bool ToggleTileGrid();
	bool ToggleBlockGrid();

	void DrawTileGrid(wxDC& dc);
	void DrawBlockGrid(wxDC& dc);

	void SetViewOnly(bool set);
	bool IsViewOnly() { return m_ViewOnly; }
//static helpers
	static bool IsTouchingGrid(wxPoint pixel, size_t grid);
	static wxBitmap ConvertToBitmap(const Graphics& graphics, const Palette& palette);
//override
private:
	virtual wxCoord OnGetRowHeight(size_t row) const;
	virtual wxCoord OnGetColumnWidth(size_t col) const;	
//drawing
private:
	void OnDraw(wxDC& dc);
//events
private:
	void OnPaintEvent(wxPaintEvent& event);	
	void OnMouseDown(wxMouseEvent& event);	
};