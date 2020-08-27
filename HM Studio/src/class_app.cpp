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

#ifdef _DEBUG
	//This was very annoying, the debug ini file always overrides the release one.
	fn.SetName(L"HM Studio");
	wxString release_file = fn.GetFullPath();
	fn.SetName(L"HM Studio(d)");
	wxString file = fn.GetFullPath();
	wxCopyFile(release_file, file);
#endif // _DEBUG

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

