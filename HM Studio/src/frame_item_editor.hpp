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

#include "frame_graphics_editor.hpp"

#include "class_stc.hpp"
#include "control_graphics_view.hpp"

#include "frame_editor.hpp"
#include "class_item.hpp"
#include "class_rom_file.hpp"
#include "class_item_editor.hpp"

class ItemEditorFrame : public EditorFrame
{
public:
	ItemEditorFrame(id id);
	~ItemEditorFrame();
private:
	Graphics m_Graphics;
	bool m_IsOpen = false;
	GraphicsInfo GetInfo();
	void AddToGraphicsFrame();
private:
	virtual void OnGetTextFrom() override;
	virtual void UpdateText() override;
//Events
private:
	void OnFileClick(wxCommandEvent& event);
	void OnInsertClick(wxCommandEvent& event);
	void OnImageDoubleClick(wxMouseEvent& event);
	void OnDumpClick(wxCommandEvent& event);
	void OnDumpImgClick(wxCommandEvent& event);
private:
	void CreateGUIControls();
	
	wxTextCtrl* m_pItemName = nullptr;
	STC* m_pItemText = nullptr;	

	wxButton* m_pSaveText = nullptr;
	wxButton* m_pPrevText = nullptr;
	wxButton* m_pProxText = nullptr;

	wxStaticText* m_pOrigItemName = nullptr;
	wxStaticText* m_pOrigItemDescription = nullptr;

	GraphicsView* m_pItemIconView = nullptr;

	wxStaticBox* m_pOrigBox = nullptr;
	wxStaticBoxSizer* m_pOrigBoxSizer = nullptr;
	
	wxBoxSizer* m_pRootSizer = nullptr;

	GraphicsEditorFrame* m_GraphicsFrame = nullptr;
};