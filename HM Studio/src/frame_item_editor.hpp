#pragma once

//#include <wx/frame.h>
//#include <wx/stattext.h>
//#include <wx/textctrl.h>
//#include <wx/stc/stc.h>
//#include <wx/sizer.h>

#include <wx/wx.h>
#include <wx\stc\stc.h>

class ItemEditorFrame : public wxFrame
{
public:
	ItemEditorFrame();
	~ItemEditorFrame();


private:
	void CreateGUIControls();
	
	wxStaticText* m_pGUI_whichItensLabel = nullptr;
	wxChoice* m_pGUI_whichItens = nullptr;

	wxBoxSizer* m_pGUI_whichItensSizer = nullptr;

	wxStaticText* m_pGUI_labelOffset = nullptr;
	wxStaticText* m_pGUI_offset = nullptr;

	wxBoxSizer* m_pGUI_offsetSizer = nullptr;

	wxButton* m_pGUI_insert = nullptr;

	wxStaticBox* m_pGUI_blockBox = nullptr;
	wxStaticBoxSizer* m_pGUI_blockBoxSizer = nullptr;

	wxStaticText* m_pGUI_index = nullptr;
	wxStaticText* m_pGUI_indexLabel = nullptr;

	wxTextCtrl* m_pGUI_index_input = nullptr;	

	wxBoxSizer* m_pGUI_itemIndexSizer = nullptr;

	wxComboBox* m_pGUI_goToOriginal = nullptr;
	wxComboBox* m_pGUI_goToTranslated = nullptr;

	wxStaticBox* m_pGUI_itemBox = nullptr;
	wxStaticBoxSizer* m_pGUI_itemBoxSizer = nullptr;

	wxBoxSizer* m_pGUI_lateralSizer = nullptr;

	wxTextCtrl* m_pGUI_transItemName = nullptr;
	wxStyledTextCtrl* m_pGUI_transItemDescription = nullptr;	

	wxButton* m_pGUI_saveText = nullptr;
	wxButton* m_pGUI_prevText = nullptr;
	wxButton* m_pGUI_proxText = nullptr;		

	wxStaticText* m_pGUI_origItemName = nullptr;
	wxStaticText* m_pGUI_origItemDescription = nullptr;

	wxStaticBox* m_pGUI_origBox = nullptr;
	wxStaticBoxSizer* m_pGUI_origBoxSizer = nullptr;

	wxBoxSizer* m_pGUI_navigationSizer = nullptr;
	wxBoxSizer* m_pGUI_mainSizer = nullptr;
	wxBoxSizer* m_pGUI_editorSizer = nullptr;
};