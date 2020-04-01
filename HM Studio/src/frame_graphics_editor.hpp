#pragma once

#include <map>

#include <wx/frame.h>
#include <wx/treectrl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/filedlg.h>
#include <wx/menu.h>
#include <wx/statbmp.h>

#include "class_rom.hpp"
#include "class_graphics.hpp"

#include "control_graphics_view.hpp"

#define MAP std::map<wxTreeItemId, GraphicsInfo>

class GraphicsEditorFrame;

struct GraphicsInfo
{
	short m_Width;
	short m_Height;
	uint32_t m_ImageAdress;
	uint32_t m_PaletteAdress;
	void(GraphicsEditorFrame:: * m_Function)(const Rom&) = nullptr;
	char m_Bpp;
	bool m_Reversed;
	bool m_Planar;	

	GraphicsInfo();
	GraphicsInfo(uint32_t imageAdress, uint32_t paletteOffset, short width = 16, short height = 16);
};


class GraphicsEditorFrame : public wxFrame
{
public:
	GraphicsEditorFrame(id i);
	~GraphicsEditorFrame() = default;
private:
	//void RomFromDialog();

	void GetGraphicsList();	
	
	Rom m_RomOriginal;
	Rom m_RomTranslated;	
	Graphics m_Graphic;

	void ExportImage();
	void ImportImage();
	void FromOriginal();
	void SaveImage();
	void Zoom(int zoomMode);
	void GetGraphics(const GraphicsInfo& info, Rom& rom);
private:
	void OnMenuBarClick(wxCommandEvent& event);
	void OnSelChanged(wxTreeEvent& event);
private:
	void CreateGUIControls();	

	wxTreeCtrl* m_pNavigator = nullptr;	
	wxPanel* m_pLeftPanel;

	GraphicsView* m_ImageView = nullptr;

	MAP m_LookUp;
	MAP::iterator m_Index;

	enum ID
	{
		ID_IMPORT = 1100,
		ID_EXPORT,		
		ID_ZOOM_IN,
		ID_ZOOM_OUT,
		ID_ZOOM_1,
		ID_ZOOM_2,
		ID_ZOOM_4,
		ID_ZOOM_8,
		ID_ZOOM_16,
		ID_ZOOM_32,
	};
};