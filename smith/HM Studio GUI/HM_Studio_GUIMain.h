/***************************************************************
 * Name:      HM_Studio_GUIMain.h
 * Purpose:   Defines Application Frame
 * Author:    Kirito13 ()
 * Created:   2019-09-29
 * Copyright: Kirito13 ()
 * License:
 **************************************************************/

#ifndef HM_STUDIO_GUIMAIN_H
#define HM_STUDIO_GUIMAIN_H

//(*Headers(HM_Studio_GUIFrame)
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>
//*)

class HM_Studio_GUIFrame: public wxFrame
{
    public:

        HM_Studio_GUIFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~HM_Studio_GUIFrame();

    private:

        //(*Handlers(HM_Studio_GUIFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        //*)

        //(*Identifiers(HM_Studio_GUIFrame)
        static const long ID_TEXTCTRL1;
        static const long ID_BUTTON1;
        static const long ID_BUTTON2;
        static const long ID_STATICTEXT1;
        static const long ID_STATICTEXT2;
        static const long ID_BUTTON3;
        static const long ID_PANEL1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(HM_Studio_GUIFrame)
        wxButton* Button1;
        wxButton* Button2;
        wxButton* Button3;
        wxPanel* side_panel;
        wxStaticText* StaticText1;
        wxStaticText* StaticText2;
        wxStatusBar* StatusBar1;
        wxTextCtrl* TextCtrl1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // HM_STUDIO_GUIMAIN_H
