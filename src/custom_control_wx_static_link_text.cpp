#include "custom_control_wx_static_link_text.hpp"

wxStaticLinkText::wxStaticLinkText(wxWindow* parent,
								   wxWindowID id,
								   const wxString& label,
								   const wxPoint& pos,
								   const wxSize& size,
								   long style,
								   const wxString& name) : wxStaticText(parent, id, label, pos, size, style, name)
{
	this->SetForegroundColour(wxColour(0, 0, 255, 255));
	this->SetCursor(wxCursor(wxCURSOR_HAND));
}
