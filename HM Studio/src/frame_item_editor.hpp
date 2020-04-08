#pragma once

#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/statbox.h>
#include <wx/msgdlg.h>
#include <wx/image.h>
#include <wx/statbmp.h>
#include <wx/menu.h>

#include "class_stc.hpp"
#include "class_item.hpp"
#include "class_rom_file.hpp"

#define m_Item m_Itens[m_Index]
#define m_ItemOriginal m_ItensOriginais[m_Index]

class ItemEditorFrame : public wxFrame
{
public:
	ItemEditorFrame(id id);
	~ItemEditorFrame();

private:
	std::vector<Item> m_Itens;
	std::vector<Item> m_ItensOriginais;
	wxImage m_ItemIcon;

	RomFile m_RomOriginal;
	RomFile m_RomTranslated;

	size_t m_Index;
	ITEM_DATA m_ItemData;

	void UpdateItem();
	void GetItens(size_t index);
//Events
private:
	void OnFileClick(wxCommandEvent& event);
	void OnProxClick(wxCommandEvent& event);
	void OnPrevClick(wxCommandEvent& event);
private:
	void CreateGUIControls();
	
	wxTextCtrl* m_pItemName = nullptr;
	STC* m_pItemText = nullptr;	

	wxButton* m_pSaveText = nullptr;
	wxButton* m_pPrevText = nullptr;
	wxButton* m_pProxText = nullptr;

	wxStaticText* m_pOrigItemName = nullptr;
	wxStaticText* m_pOrigItemDescription = nullptr;

	wxStaticBitmap* m_pItemIconView = nullptr;	

	wxStaticBox* m_pOrigBox = nullptr;
	wxStaticBoxSizer* m_pOrigBoxSizer = nullptr;
	
	wxBoxSizer* m_pRootSizer = nullptr;
};