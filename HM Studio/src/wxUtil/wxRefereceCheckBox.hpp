#pragma once
#include "wx/checkbox.h"

class wxRefereceCheckBox : public wxCheckBox
{
public:
	wxRefereceCheckBox(bool& ref, wxWindow* parent,
		wxWindowID id,
		const wxString& label,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxCheckBoxNameStr);
	~wxRefereceCheckBox();

private:
	bool& m_refVar;
private:
	void OnChecked(wxCommandEvent& event);	
	//DECLARE_EVENT_TABLE()
};

