#pragma once

#include <wx/app.h>

#include "frame_main.hpp"

class App : public wxApp
{
public:
	App();
	~App();
public:
	virtual bool OnInit();
};
