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
	SetAppName("HM Studio");

	wxFileName fn(wxStandardPaths::Get().GetExecutablePath());
	fn.SetExt(L"ini");	

	wxFileConfig* configs = new wxFileConfig(wxEmptyString, wxEmptyString, fn.GetFullPath());
	configs->SetRecordDefaults();
	configs->EnableAutoSave();	

	wxConfigBase::Set(configs);	

	MainFrame* frame = new MainFrame();
	frame->Show();
	
	SetTopWindow(frame);

	wxImage::AddHandler(new wxPNGHandler());	

	return true;
}

