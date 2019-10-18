#pragma once


#include "wx/wx.h"

#include "cMain.h"
#include "cScriptEditor.h"

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



