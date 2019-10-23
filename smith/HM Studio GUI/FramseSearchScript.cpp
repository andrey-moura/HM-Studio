#include "FramseSearchScript.h"

//(*InternalHeaders(FramseSearchScript)
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(FramseSearchScript)
const long FramseSearchScript::ID_STATICTEXT1 = wxNewId();
const long FramseSearchScript::ID_TEXTCTRL1 = wxNewId();
const long FramseSearchScript::ID_BUTTON1 = wxNewId();
const long FramseSearchScript::ID_RADIOBUTTON1 = wxNewId();
const long FramseSearchScript::ID_RADIOBUTTON2 = wxNewId();
const long FramseSearchScript::ID_CHECKBOX1 = wxNewId();
const long FramseSearchScript::ID_PANEL1 = wxNewId();
const long FramseSearchScript::ID_NOTEBOOK1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(FramseSearchScript,wxDialog)
	//(*EventTable(FramseSearchScript)
	//*)
END_EVENT_TABLE()

FramseSearchScript::FramseSearchScript(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(FramseSearchScript)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer3;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer2;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	Notebook1 = new wxNotebook(this, ID_NOTEBOOK1, wxPoint(0,0), wxSize(400,400), 0, _T("ID_NOTEBOOK1"));
	Notebook1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	Panel1 = new wxPanel(Notebook1, ID_PANEL1, wxPoint(235,229), wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(Panel1, ID_STATICTEXT1, _("Find What:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(StaticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	TextCtrl1 = new wxTextCtrl(Panel1, ID_TEXTCTRL1, _("Text"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer2->Add(TextCtrl1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button1 = new wxButton(Panel1, ID_BUTTON1, _("Find All"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	Button1->SetMinSize(wxSize(60,30));
	BoxSizer2->Add(Button1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND, 10);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, Panel1, _("Mode"));
	RadioButton1 = new wxRadioButton(Panel1, ID_RADIOBUTTON1, _("Normal"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON1"));
	StaticBoxSizer2->Add(RadioButton1, 1, wxALL, 5);
	RadioButton2 = new wxRadioButton(Panel1, ID_RADIOBUTTON2, _("Extended"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_RADIOBUTTON2"));
	StaticBoxSizer2->Add(RadioButton2, 1, wxALL, 5);
	BoxSizer3->Add(StaticBoxSizer2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, Panel1, _("Options"));
	CheckBox1 = new wxCheckBox(Panel1, ID_CHECKBOX1, _("Match Case"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	CheckBox1->SetValue(false);
	StaticBoxSizer1->Add(CheckBox1, 1, wxALL, 5);
	BoxSizer3->Add(StaticBoxSizer1, 0, wxALL|wxEXPAND, 5);
	BoxSizer1->Add(BoxSizer3, 0, wxALL|wxEXPAND, 5);
	Panel1->SetSizer(BoxSizer1);
	BoxSizer1->Fit(Panel1);
	BoxSizer1->SetSizeHints(Panel1);
	Notebook1->AddPage(Panel1, _("Page name"), false);
	//*)
}

FramseSearchScript::~FramseSearchScript()
{
	//(*Destroy(FramseSearchScript)
	//*)
}

