#pragma once

#include <map>

#include <wx/frame.h>
#include <wx/treectrl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/filedlg.h>
#include <wx/menu.h>
#include <wx/statbmp.h>

#include "class_rom_file.hpp"
#include "class_graphics.hpp"

#include "control_graphics_view.hpp"
#include "graphicsinfoviewer.hpp"
#include "class_graphics_editor.hpp"
#include "palctrl.hpp"

#define MAP std::map<wxTreeItemId, GraphicsInfo>

class GraphicsEditorFrame : public wxFrame
{
public:
	GraphicsEditorFrame(id i);
	~GraphicsEditorFrame() = default;
private:
	//void RomFromDialog();

	void GetGraphicsList();	
	
	RomFile m_RomOriginal;
	RomFile m_RomTranslated;	
	Graphics m_Graphic;

	void ExportImage();
	void ImportImage();
	void FromOriginal();
	void SaveImage();
	void Zoom(int zoomMode);
	void ToggleMenu(int id, bool check);
	void GetGraphics(const GraphicsInfo& info, RomFile& rom);
	inline void AppendGraphics(const GraphicsTreeItem& item, const wxTreeItemId& id);
	void AppendGraphics(const GraphicsTreeItem& item);
	void AppendGraphics(const GraphicsTreeParent& parent);
private:
	void OnMenuBarClick(wxCommandEvent& event);
	void OnSelChanged(wxTreeEvent& event);
private:
	void CreateGUIControls();	

	wxTreeCtrl* m_pNavigator = nullptr;	
	wxTreeItemId m_Root = 0;
	wxPanel* m_pLeftPanel;

	GraphicsView* m_ImageView = nullptr;
	GraphicsInfoViewer* m_InfoViewer = nullptr;
	PalCtrl* m_PalCtrl = nullptr;

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
		ID_PIXEL_GRID,
		ID_TILE_GRID,
		ID_BLOCK_GRID,
	};
};