#include "control_graphics_view.hpp"

GraphicsView::GraphicsView(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	Bind(wxEVT_PAINT, &GraphicsView::OnPaintEvent, this);
	SetBackgroundColour(wxColour(00, 00, 128));
}

void GraphicsView::UpdateView()
{	
	PaintNow();
}

void GraphicsView::UpdateImage(Graphics& graphics)
{	
	if (m_Image.GetSize() != wxSize(graphics.m_width, graphics.m_height))
		ClearWindow();

	m_Image = graphics.ToImage();
	this->SetMinSize(m_Image.GetSize() * m_Scale);
	
	UpdateView();
}

wxImage GraphicsView::GetImage()
{
	return m_Image.ConvertToImage();
}

void GraphicsView::SetScale(double scale)
{
	if (scale == m_Scale)
		return;

	if (scale == 0)
		return;

	if(scale < m_Scale)
		ClearWindow();

	m_Scale = scale;

	wxSize imageSize = m_Image.GetSize();

	this->SetMinSize(m_Image.GetSize() * m_Scale);
	
	GetParent()->Layout();

	UpdateView();
}

void GraphicsView::OnPaintEvent(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	Render(dc);
}

void GraphicsView::Render(wxDC& dc)
{
	dc.SetUserScale(m_Scale, m_Scale);

	if (m_Image.IsOk())
		dc.DrawBitmap(m_Image, wxPoint(0, 0));
}

void GraphicsView::PaintNow()
{
	wxClientDC dc(this);
	Render(dc);
}

void GraphicsView::ClearWindow()
{
	wxClientDC dc(this);
	dc.Clear();	
}