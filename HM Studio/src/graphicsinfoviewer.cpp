#include "graphicsinfoviewer.hpp"

GraphicsInfoViewer::GraphicsInfoViewer(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	CreateGUIControls();
	SetBackgroundColour(wxColour(255, 255, 255));
}

void GraphicsInfoViewer::SetInfo(const GraphicsInfo& info)
{
	m_Texts[0]->SetLabel(std::to_string(info.m_Width));
	m_Texts[1]->SetLabel(std::to_string(info.m_Height));
	m_Texts[2]->SetLabel("0x" + BitConverter::ToHexString(info.m_ImageAdress));
	m_Texts[3]->SetLabel("0x" + BitConverter::ToHexString(info.m_PaletteAdress));
	m_Texts[4]->SetLabel(BitConverter::ToBooleanString(info.m_Function == nullptr));
	m_Texts[5]->SetLabel(std::to_string((int)info.m_Bpp) + "bpp");
	m_Texts[6]->SetLabel(info.m_Reversed ? "Reversed" : "No-reversed");
	m_Texts[7]->SetLabel(info.m_Planar ? "Planar" : "Linear");
}

void GraphicsInfoViewer::CreateGUIControls()
{
	m_pRootSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticText* labels[8];

	for (size_t index = 0; index < 8; ++index)
	{
		labels[index] = new wxStaticText(this, wxID_ANY, "");
		m_Texts[index] = new wxStaticText(this, wxID_ANY, "");

		wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->AddSpacer(5);
		sizer->Add(labels[index]);
		sizer->AddStretchSpacer(1);
		sizer->Add(m_Texts[index]);
		sizer->AddSpacer(5);

		m_pRootSizer->Add(sizer, 0, wxEXPAND, 0);
	}

	labels[0]->SetLabel("Width: ");
	labels[1]->SetLabel("Height: ");
	labels[2]->SetLabel("Img addr: ");
	labels[3]->SetLabel("Pal addr: ");
	labels[4]->SetLabel("Good: ");
	labels[5]->SetLabel("Bpp: ");
	labels[6]->SetLabel("Order: ");
	labels[7]->SetLabel("Type: ");

	GraphicsInfo info(0, 0);
	info.m_Bpp = 0;
	info.m_Reversed = false;
	info.m_Planar = false;

	SetInfo(info);

	SetSizerAndFit(m_pRootSizer);
	m_pRootSizer->SetSizeHints(this);
}