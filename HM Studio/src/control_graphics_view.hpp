#pragma once

#include <wx/window.h>
#include <wx/dcclient.h>
#include <wx/bitmap.h>

//remove later
#include <wx/msgdlg.h>

#include "class_graphics.hpp"

class GraphicsView : public wxWindow
{
public:
	GraphicsView(wxWindow* parent);
private:
	wxBitmap m_Image;
	double m_Scale = 1;	
public:
	void UpdateView();
	void UpdateImage(Graphics& graphics);
	wxImage GetImage();
	void SetScale(double scale);
	double GetScale() { return m_Scale; }
private:
	void OnPaintEvent(wxPaintEvent& event);
	void Render(wxDC& dc);
	void PaintNow();
	void ClearWindow();
};