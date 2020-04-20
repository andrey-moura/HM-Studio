#include "class_app.hpp"

wxIMPLEMENT_APP(App);

App::App()
{
}

App::~App()
{
}

bool App::OnInit()
{
	MainFrame* frame = new MainFrame();
	frame->Show();
	
	SetTopWindow(frame);

	return true;
}

