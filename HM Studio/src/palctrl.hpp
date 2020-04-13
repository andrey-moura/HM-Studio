#pragma once

#include <wx/window.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/sizer.h>

#include "class_graphics.hpp"

class PalCtrl : public wxWindow
{
public:
	PalCtrl(wxWindow* parent);
//private members
private:
	wxColour* m_Colours = nullptr;
	char m_Color1 = 1;
	char m_Color2 = 0;
//public accesors	
public:
	void SetPal(const Palette& pal);
	size_t GetSelColour1() const;
	size_t GetSelColour2() const;
	void FlipSelColours();
//GUI
private:
	void CreateGUIControls();
	wxWindow* m_PalView = nullptr;
	wxWindow* m_Selector = nullptr;	
//Pal view functions
private:
	void OnPalViewDraw(wxDC& dc);
//PalView events
private:
	void OnPalViewDown(wxMouseEvent& event);
	void OnPalViewPaint(wxPaintEvent& event);
//ColorSelector functions
private:
	void OnSelectorDraw(wxDC& dc);
//ColorSelector events
	void OnSelectorPaint(wxPaintEvent& event);
};