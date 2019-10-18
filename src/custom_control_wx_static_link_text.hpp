#pragma once

#include "wx/stattext.h"
#include "wx/window.h"

class wxStaticLinkText : public wxStaticText
{
public:
	wxStaticLinkText(wxWindow* parent,
					 wxWindowID id,
					 const wxString& label,
					 const wxPoint& pos = wxDefaultPosition,
					 const wxSize& size = wxDefaultSize,
					 long style = 0,
					 const wxString& name = wxStaticTextNameStr);

	~wxStaticLinkText();
};

