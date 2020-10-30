#pragma once

#include <map>

#include <wx/frame.h>
#include <wx/treectrl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/filedlg.h>
#include <wx/menu.h>
#include <wx/statbmp.h>
#include <wx/dcmemory.h>

#include "class_rom_file.hpp"
#include "class_graphics.hpp"

#include "control_graphics_view.hpp"
#include "graphicsinfoviewer.hpp"
#include "class_graphics_editor.hpp"
#include "palctrl.hpp"

struct GRAPHICS_HEADER
{	
	uint32_t riff;
	uint32_t size;
	uint32_t bpp;
};

#define MAP std::map<wxTreeItemId, GraphicsInfo>

class GraphicsEditorFrame : public wxFrame
{
public:
	GraphicsEditorFrame(id i);
	GraphicsEditorFrame(id i, GraphicsTreeParent& graphics);
	~GraphicsEditorFrame() = default;

	void GetGraphicsList();
private:
	//void RomFromDialog();	
	
	RomFile m_RomOriginal;
	RomFile m_RomTranslated;	
	Graphics m_Graphic;

	wxString m_RootDir;

	std::vector<GraphicsTreeParent> m_MountGraphics;
	size_t m_MountIndex = 0;

	bool m_IsPieces = false;

	void ExportImage();
	void ImportImage();
	void FromOriginal();
	void SaveImage();
	void InsertImage();
	void Zoom(int zoomMode);
	void ToggleMenu(int id, bool check);
	void GetGraphics(const GraphicsInfo& info, RomFile& rom);
	void GetGraphicsPieces(const GraphicsTreeParent& parent, RomFile& rom);	
private:
	void OnMenuBarClick(wxCommandEvent& event);
	void OnSaveFileClick(wxCommandEvent& event);
	void OnInsertClick(wxCommandEvent& event);
	void OnSelChanged(wxTreeEvent& event);
	void OnPalChanged(PaletteEvent& event);	
public:
	void SetRootName(const wxString& name);

	void AppendGraphics(const GraphicsTreeItem& item, const wxTreeItemId& id);
	void AppendGraphics(const GraphicsTreeItem& item);
	void AppendGraphics(GraphicsTreeParent& parent);
	void AppendGraphics(GraphicsTreeParent& parent, wxTreeItemId gran_parent);
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