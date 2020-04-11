#pragma once

#include <wx/window.h>
#include <wx/stattext.h>
#include <wx/sizer.h>

#include "class_graphics.hpp"
#include "class_graphics_editor.hpp"
#include "class_bit_converter.hpp"

class GraphicsInfoViewer : public wxWindow
{	
public:
	GraphicsInfoViewer(wxWindow* parent);	
	void SetInfo(const GraphicsInfo& info);	
private:
	void CreateGUIControls();
	wxStaticText* m_Texts[8];
	wxBoxSizer* m_pRootSizer = nullptr;
};
