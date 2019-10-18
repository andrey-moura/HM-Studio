/***************************************************************
 * Name:      HM_Studio_GUIApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Kirito13 ()
 * Created:   2019-09-29
 * Copyright: Kirito13 ()
 * License:
 **************************************************************/

#include "HM_Studio_GUIApp.h"

//(*AppHeaders
#include "HM_Studio_GUIMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(HM_Studio_GUIApp);

bool HM_Studio_GUIApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	HM_Studio_GUIFrame* Frame = new HM_Studio_GUIFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
