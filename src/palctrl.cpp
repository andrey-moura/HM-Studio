#include "palctrl.hpp"

wxIMPLEMENT_DYNAMIC_CLASS(PaletteEvent, wxNotifyEvent);

wxDEFINE_EVENT(EVT_PAL_CHANGED, PaletteEvent);

PalCtrl::PalCtrl(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{	
	CreateGUIControls();
}

void PalCtrl::CreateGUIControls()
{
	m_PalView = new wxWindow(this, wxID_ANY, wxDefaultPosition, wxSize(32 * 8, 32 * 2), wxBORDER_SIMPLE);
	m_PalView->SetMinSize(wxSize(32 * 8, 32 * 2));	
	m_PalView->Bind(wxEVT_PAINT, &PalCtrl::OnPalViewPaint, this, m_PalView->GetId());
	m_PalView->Bind(wxEVT_LEFT_DOWN, &PalCtrl::OnPalViewDown, this, m_PalView->GetId());
	m_PalView->Bind(wxEVT_RIGHT_DOWN, &PalCtrl::OnPalViewDown, this, m_PalView->GetId());

	m_Selector = new wxWindow(this, wxID_ANY, wxDefaultPosition, wxSize(64, 64));
	m_Selector->SetMinSize(wxSize(64, 64));	
	m_Selector->Bind(wxEVT_PAINT, &PalCtrl::OnSelectorPaint, this, m_Selector->GetId());

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(m_Selector);
	sizer->AddSpacer(4);
	sizer->Add(m_PalView);

	SetSizer(sizer);
	sizer->SetSizeHints(this);
}

void PalCtrl::SetPal(const Palette& pal)
{	
	if (m_Count != pal.size())
	{		
		m_Count = pal.size();
		m_Colors = new wxColour[m_Count];
	}

	for (size_t i = 0; i < m_Count; ++i)
	{
		m_Colors[i] = pal[i].GetBGR();
	}

	m_Color1 = 1;
	m_Color2 = 0;

	m_PalView->Refresh();
	m_Selector->Refresh();
}

void PalCtrl::SendSelChangedEvent(char color1, char color2)
{
	PaletteEvent toSend(EVT_PAL_CHANGED, this->GetId());
	toSend.SetEventObject(this);
	toSend.SetFirst(color1);
	toSend.SetSecond(color2);
	ProcessWindowEvent(toSend);	
	
	m_Color1 = color1;
	m_Color2 = color2;

	m_Selector->Refresh();
}

void PalCtrl::FlipSelColours()
{
	SendSelChangedEvent(m_Color2, m_Color1);	
}

void PalCtrl::OnPalViewDraw(wxDC& dc)
{
	dc.Clear();	

	if (m_Count == 0)
		return;

	dc.SetPen(*wxTRANSPARENT_PEN);

	if (m_Count == 2)
	{
		wxBrush brush1(wxColour(m_Colors[0]), wxBRUSHSTYLE_SOLID);
		wxBrush brush2(wxColour(m_Colors[1]), wxBRUSHSTYLE_SOLID);

		dc.SetBrush(brush1);
		dc.DrawRectangle(0, 0, 8 * 32, 2 * 32);

		dc.SetBrush(brush2);
		dc.DrawRectangle(4 * 32, 0, 4 * 32, 2 * 32);

		return;
	}

	wxBrush brush(*wxBLACK, wxBRUSHSTYLE_SOLID);

	for (size_t y = 0; y < 2; ++y)
	{
		for (size_t x = 0; x < 8; ++x)
		{			
			brush.SetColour(m_Colors[x + 8 * y]);
			dc.SetBrush(brush);

			dc.DrawRectangle(wxRect(x * 32, y * 32, 32, 32));
		}
	}	
}

void PalCtrl::OnPalViewPaint(wxPaintEvent& event)
{
	wxPaintDC dc(m_PalView);
	OnPalViewDraw(dc);

	event.Skip();
};

void PalCtrl::OnPalViewDown(wxMouseEvent& event)
{
	wxPoint point = event.GetPosition();	

	size_t x = point.x / 32;
	size_t y = point.y / 32;

	char index = x + 8 * y;

	if (index > 15) return;

	if (event.GetButton() == wxMOUSE_BTN_RIGHT)
		SendSelChangedEvent(m_Color1, index);
	else if (event.GetButton() == wxMOUSE_BTN_LEFT)
		SendSelChangedEvent(index, m_Color2);	

	event.Skip();
}

void PalCtrl::OnSelectorPaint(wxPaintEvent& event)
{
	wxPaintDC dc(m_Selector);
	OnSelectorDraw(dc);

	event.Skip();
}

void PalCtrl::OnSelectorDraw(wxDC& dc)
{
	dc.Clear();

	if (m_Count == 0)
		return;
	
	dc.SetPen(*wxTRANSPARENT_PEN);
	wxBrush brush(GetParent()->GetBackgroundColour(), wxBRUSHSTYLE_SOLID);
	dc.SetBrush(brush);

	dc.DrawRectangle(wxRect(wxPoint(0, 0), GetSize()));
	
	wxBrush brush1(*wxBLACK, wxBRUSHSTYLE_SOLID);	
	wxBrush brush2(*wxBLACK, wxBRUSHSTYLE_SOLID);	

	brush1.SetColour(m_Colors[m_Color1]);
	brush2.SetColour(m_Colors[m_Color2]);

	dc.SetBrush(brush1);
	dc.DrawRectangle(wxRect(0, 0, 32, 32));

	dc.SetBrush(brush2);
	dc.DrawRectangle(wxRect(32, 32, 32, 32));
}

PaletteEvent::PaletteEvent(wxEventType commandType, int id)
	: wxNotifyEvent(commandType, id)
{
	
}

PaletteEvent::PaletteEvent(const PaletteEvent& event) : wxNotifyEvent(event), m_Sel1(event.m_Sel1), m_Sel2(event.m_Sel2)
{
	
}