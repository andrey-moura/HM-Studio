#include "wxRefereceCheckBox.hpp"

//BEGIN_EVENT_TABLE(wxRefereceCheckBox, wxCheckBox)
//
//EVT_CHECKBOX(10001, wxRefereceCheckBox::OnChecked)
//
//END_EVENT_TABLE()

wxRefereceCheckBox::wxRefereceCheckBox(bool& ref, wxWindow* parent,
	wxWindowID id,
	const wxString& label,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxValidator& validator,
	const wxString& name) : wxCheckBox(parent, id, label, pos, size, style, validator, name), m_refVar(ref)
{
	Bind(wxEVT_CHECKBOX, &wxRefereceCheckBox::OnChecked, this);
}

wxRefereceCheckBox::~wxRefereceCheckBox()
{
}

void wxRefereceCheckBox::OnChecked(wxCommandEvent& event)
{
	m_refVar = this->GetValue();
}
