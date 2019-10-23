#ifndef FRAMSESEARCHSCRIPT_H
#define FRAMSESEARCHSCRIPT_H

//(*Headers(FramseSearchScript)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class FramseSearchScript: public wxDialog
{
	public:

		FramseSearchScript(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~FramseSearchScript();

		//(*Declarations(FramseSearchScript)
		wxButton* Button1;
		wxCheckBox* CheckBox1;
		wxNotebook* Notebook1;
		wxPanel* Panel1;
		wxRadioButton* RadioButton1;
		wxRadioButton* RadioButton2;
		wxStaticText* StaticText1;
		wxTextCtrl* TextCtrl1;
		//*)

	protected:

		//(*Identifiers(FramseSearchScript)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON2;
		static const long ID_CHECKBOX1;
		static const long ID_PANEL1;
		static const long ID_NOTEBOOK1;
		//*)

	private:

		//(*Handlers(FramseSearchScript)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
