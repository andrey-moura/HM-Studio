#pragma once

#include <wx/app.h>

#include "frame_main.hpp"

class App : public wxApp
{
public:
	App();
	~App();

private:
	MainFrame* frame1 = nullptr;
public:
	virtual bool OnInit();
};



