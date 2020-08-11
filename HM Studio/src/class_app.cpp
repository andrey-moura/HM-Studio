#include "class_app.hpp"
#include <wx/confbase.h>
#include <wx/fileconf.h>

wxIMPLEMENT_APP(App);

App::App()
{
}

App::~App()
{
}

bool App::OnInit()
{
	SetVendorName("Moonslate");
	SetAppName("HM Studio"); // not needed, it's the default value

	wxFileConfig* configs = new wxFileConfig();
	wxConfigBase::Set(configs);

	configs->SetRecordDefaults();
	configs->EnableAutoSave();	

	MainFrame* frame = new MainFrame();
	frame->Show();
	
	SetTopWindow(frame);

	wxImage::AddHandler(new wxPNGHandler());	

	return true;
}

