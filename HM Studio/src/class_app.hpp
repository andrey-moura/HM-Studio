#pragma once

#include <wx/app.h>

#include "frame_main.hpp"
#include "frame_script_editor.hpp"

class App : public wxApp
{
public:
	App();
	~App();

private:
	cMain* frame1 = nullptr;
	cScriptEditor* scriptEditor = nullptr;

public:
	virtual bool OnInit();
};



