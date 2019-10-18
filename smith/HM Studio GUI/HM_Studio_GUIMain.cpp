/***************************************************************
 * Name:      HM_Studio_GUIMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Kirito13 ()
 * Created:   2019-09-29
 * Copyright: Kirito13 ()
 * License:
 **************************************************************/

#include "HM_Studio_GUIMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(HM_Studio_GUIFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(HM_Studio_GUIFrame)
const long HM_Studio_GUIFrame::ID_TEXTCTRL1 = wxNewId();
const long HM_Studio_GUIFrame::ID_BUTTON1 = wxNewId();
const long HM_Studio_GUIFrame::ID_BUTTON2 = wxNewId();
const long HM_Studio_GUIFrame::ID_STATICTEXT1 = wxNewId();
const long HM_Studio_GUIFrame::ID_STATICTEXT2 = wxNewId();
const long HM_Studio_GUIFrame::ID_BUTTON3 = wxNewId();
const long HM_Studio_GUIFrame::ID_PANEL1 = wxNewId();
const long HM_Studio_GUIFrame::idMenuQuit = wxNewId();
const long HM_Studio_GUIFrame::idMenuAbout = wxNewId();
const long HM_Studio_GUIFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(HM_Studio_GUIFrame,wxFrame)
    //(*EventTable(HM_Studio_GUIFrame)
    //*)
END_EVENT_TABLE()

HM_Studio_GUIFrame::HM_Studio_GUIFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(HM_Studio_GUIFrame)
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* global_sizer;
    wxBoxSizer* size_panel_sizer;
    wxMenu* Menu1;
    wxMenu* Menu2;
    wxMenuBar* MenuBar1;
    wxMenuItem* MenuItem1;
    wxMenuItem* MenuItem2;
    wxStaticBoxSizer* StaticBoxSizer1;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    global_sizer = new wxBoxSizer(wxHORIZONTAL);
    side_panel = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    size_panel_sizer = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, side_panel, _("Script"));
    TextCtrl1 = new wxTextCtrl(side_panel, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    StaticBoxSizer1->Add(TextCtrl1, 0, wxEXPAND, 5);
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    Button1 = new wxButton(side_panel, ID_BUTTON1, _("<<"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    Button1->SetMinSize(wxSize(40,26));
    BoxSizer1->Add(Button1, 0, wxALIGN_LEFT, 5);
    BoxSizer1->Add(-1,-1,0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(side_panel, ID_BUTTON2, _(">>"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    Button2->SetMinSize(wxSize(40,26));
    BoxSizer1->Add(Button2, 0, wxALIGN_LEFT, 5);
    StaticBoxSizer1->Add(BoxSizer1, 0, wxALIGN_LEFT, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    StaticText1 = new wxStaticText(side_panel, ID_STATICTEXT1, _("Offset: 0x"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    BoxSizer2->Add(StaticText1, 0, wxALIGN_LEFT, 5);
    StaticText2 = new wxStaticText(side_panel, ID_STATICTEXT2, _("00000000"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    StaticText2->SetForegroundColour(wxColour(0,0,255));
    BoxSizer2->Add(StaticText2, 0, wxALIGN_LEFT, 5);
    StaticBoxSizer1->Add(BoxSizer2, 0, wxEXPAND, 0);
    Button3 = new wxButton(side_panel, ID_BUTTON3, _("Label"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    StaticBoxSizer1->Add(Button3, 0, wxEXPAND, 5);
    size_panel_sizer->Add(StaticBoxSizer1, 0, wxEXPAND, 5);
    side_panel->SetSizer(size_panel_sizer);
    size_panel_sizer->Fit(side_panel);
    size_panel_sizer->SetSizeHints(side_panel);
    global_sizer->Add(side_panel, 0, wxEXPAND, 5);
    SetSizer(global_sizer);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    global_sizer->Fit(this);
    global_sizer->SetSizeHints(this);

    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HM_Studio_GUIFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&HM_Studio_GUIFrame::OnAbout);
    //*)
}

HM_Studio_GUIFrame::~HM_Studio_GUIFrame()
{
    //(*Destroy(HM_Studio_GUIFrame)
    //*)
}

void HM_Studio_GUIFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void HM_Studio_GUIFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}
