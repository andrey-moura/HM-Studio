#include "App.h"

wxIMPLEMENT_APP(App);

App::App()
{
}

App::~App()
{
}

bool App::OnInit()
{
	wxImage::AddHandler(new wxPNGHandler);

	frame1 = new cMain();
	frame1->Show();

	//scriptEditor = new cScriptEditor();
	//scriptEditor->Show();
	return true;
}

