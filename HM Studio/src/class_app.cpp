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
	wxImage::AddHandler(new wxPNGHandler);

	frame1 = new MainFrame();
	frame1->Show();
	
	return true;
}

