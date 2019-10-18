#ifndef NEWFRAME_H
#define NEWFRAME_H

//(*Headers(NewFrame)
#include <wx/frame.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class NewFrame: public wxMDIParentFrame
{
	public:

		NewFrame(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~NewFrame();

		//(*Declarations(NewFrame)
		wxNotebook* Notebook1;
		wxPanel* Panel1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxTextCtrl* TextCtrl1;
		wxTextCtrl* TextCtrl2;
		//*)

	protected:

		//(*Identifiers(NewFrame)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_STATICTEXT3;
		static const long ID_PANEL1;
		static const long ID_NOTEBOOK1;
		//*)

	private:

		//(*Handlers(NewFrame)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
