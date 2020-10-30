#pragma once

#include "wx/wx.h"

#include <vector>

class FishEditor : public wxFrame
{
public:
	FishEditor();
	~FishEditor();

//GUI	
private:
	void CreateGUIControls();

	wxBoxSizer* m_mainSizer = nullptr;
	
};

