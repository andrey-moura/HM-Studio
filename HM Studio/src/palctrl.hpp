#pragma once

#include <wx/window.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/sizer.h>

#include "class_graphics.hpp"

class PaletteEvent : public wxNotifyEvent
{
public:
	PaletteEvent(wxEventType commandType = wxEVT_NULL, int id = 0);
	PaletteEvent(const PaletteEvent& event);

	virtual wxEvent* Clone() const override { return new PaletteEvent(*this); }

private:
	uint8_t m_Sel1;
	uint8_t m_Sel2;
public:
	void SetFirst(uint8_t color) { m_Sel1 = color; }
	void SetSecond(uint8_t color) { m_Sel2 = color; }

	uint8_t GetFirst() { return m_Sel1; }
	uint8_t GetSecond() { return m_Sel2; }

	wxDECLARE_DYNAMIC_CLASS(PaletteEvent);
};

typedef void (wxEvtHandler::*PalEventFunction)(PaletteEvent&);

wxDECLARE_EVENT(EVT_PAL_CHANGED, PaletteEvent);

class PalCtrl : public wxWindow
{
public:
	PalCtrl(wxWindow* parent);
//private members
private:
	wxColour* m_Colours = nullptr;
	uint8_t m_Color1 = 1;
	uint8_t m_Color2 = 0;
//public accesors	
public:
	void SetPal(const Palette& pal);
	uint8_t GetFirst() const { return m_Color1; }
	uint8_t GetSecond() const { return m_Color2; }
	void FlipSelColours();
//GUI
private:
	void CreateGUIControls();
	wxWindow* m_PalView = nullptr;
	wxWindow* m_Selector = nullptr;	
//PalCtrlEvents
	void SendSelChangedEvent(char color1, char color2);
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