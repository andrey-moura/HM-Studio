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

#define MAP std::map<wxTreeItemId, GraphicsInfo>

class GraphicsEditorFrame;

struct GraphicsInfo
{
	short m_Width;
	short m_Height;
	uint32_t m_ImageAdress;
	uint32_t m_PaletteAdress;
	void(GraphicsEditorFrame::*m_Function)(const RomFile&) = nullptr;
	char m_Bpp;
	bool m_Reversed;
	bool m_Planar;		

	GraphicsInfo();
	GraphicsInfo(uint32_t imageAdress, uint32_t paletteOffset, short width = 16, short height = 16);
};

struct GraphicsTreeItem
{
	std::string m_Name;
	GraphicsInfo m_Info;

	GraphicsTreeItem(const std::string& name, const GraphicsInfo& info);	
};

class GraphicsTreeParent : public std::vector<GraphicsTreeItem>
{
public:
	GraphicsTreeParent(const std::string& name);
private:
	std::string m_Name;
public:
	std::string GetName() const { return m_Name; }
};

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