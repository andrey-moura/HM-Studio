#pragma once

#include <map>

#include <wx/wx.h>
#include <wx/treectrl.h>

#include "class_rom_file.hpp"
#include "class_graphics.hpp"

#include "control_graphics_view.hpp"
#include "palctrl.hpp"

class GraphicsEditorFrame;

struct GraphicsInfo
{
	short m_Width;
	short m_Height;
	short m_TileWidth;
	short m_TileHeight;
	uint32_t m_ImageAdress;
	uint32_t m_PaletteAdress;
	void(GraphicsEditorFrame::* m_Function)(const RomFile&) = nullptr;
	char m_Bpp;
	bool m_Reversed;
	bool m_Planar;

	GraphicsInfo();
	GraphicsInfo(uint32_t imageAdress, uint32_t paletteOffset, short width = 16, short height = 16, char bpp = 4, bool reversed = true, bool planar = false, short tilewidth = 8, short tileheight = 8);
};

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

struct GraphicsTreeItem
{
	std::string m_Name;
	GraphicsInfo m_Info;

	GraphicsTreeItem(const std::string& name, const GraphicsInfo& info);
};

class GraphicsTreeParent : public std::vector<GraphicsTreeItem>
{
public:
	GraphicsTreeParent(const std::string& name, bool mount = false);
private:
	std::string m_Name;
	bool m_CanMount = false;
	std::vector<GraphicsTreeParent> m_Childs;
public:	
	wxTreeItemId m_Id;
	bool CanMount() const { return m_CanMount; }
	std::string GetName() const { return m_Name; }
	void append(const GraphicsTreeParent& parent)
	{
		m_Childs.push_back(parent);
	}

	size_t child_size()
	{
		return m_Childs.size();
	}

	std::vector<GraphicsTreeParent>& get_childs()
	{
		return m_Childs;
	}
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