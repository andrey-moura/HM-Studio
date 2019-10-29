#include "frame_item_editor.hpp"

ItemEditorFrame::ItemEditorFrame() : wxFrame(nullptr, wxID_ANY, "Item Editor")
{
	CreateGUIControls();
}

ItemEditorFrame::~ItemEditorFrame()
{
}

void ItemEditorFrame::CreateGUIControls()
{
	this->SetBackgroundColour(wxColour(240, 240, 240, 255));

	m_pGUI_whichItensLabel = new wxStaticText(this, wxID_ANY, _("Itens: "));
	m_pGUI_whichItens = new wxChoice(this, wxID_ANY);	
	
	m_pGUI_whichItensSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pGUI_whichItensSizer->Add(m_pGUI_whichItensLabel, 0, 0, 0);
	m_pGUI_whichItensSizer->Add(m_pGUI_whichItens, 0, 0, 0);

	m_pGUI_labelOffset = new wxStaticText(this, wxID_ANY, _("Offset: 0x"));
	m_pGUI_offset = new wxStaticText(this, wxID_ANY, _("00000000"));

	m_pGUI_offsetSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pGUI_offsetSizer->Add(m_pGUI_labelOffset);
	m_pGUI_offsetSizer->Add(m_pGUI_offset);

	m_pGUI_insert = new wxButton(this, wxID_ANY, _("Insert"));

	m_pGUI_blockBox = new wxStaticBox(this, wxID_ANY, _("Item Block"));
	m_pGUI_blockBoxSizer = new wxStaticBoxSizer(m_pGUI_blockBox, wxVERTICAL);

	m_pGUI_blockBoxSizer->Add(m_pGUI_whichItensSizer, 0, 0, 0);
	m_pGUI_blockBoxSizer->AddSpacer(4);
	m_pGUI_blockBoxSizer->Add(m_pGUI_offsetSizer, 0, 0, 0);
	m_pGUI_blockBoxSizer->AddSpacer(4);
	m_pGUI_blockBoxSizer->Add(m_pGUI_insert, 0, wxEXPAND, 0);

	m_pGUI_indexLabel = new wxStaticText(this, wxID_ANY, _("Index: "));
	m_pGUI_index = new wxStaticText(this, wxID_ANY, _("0/0"));

	m_pGUI_index_input = new wxTextCtrl(this, wxID_ANY);	

	m_pGUI_itemIndexSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pGUI_itemIndexSizer->Add(m_pGUI_indexLabel);
	m_pGUI_itemIndexSizer->Add(m_pGUI_index);
	m_pGUI_itemIndexSizer->AddSpacer(4);
	m_pGUI_itemIndexSizer->Add(m_pGUI_index_input);

	m_pGUI_goToOriginal = new wxComboBox(this, wxID_ANY);
	m_pGUI_goToTranslated = new wxComboBox(this, wxID_ANY);

	m_pGUI_itemBox = new wxStaticBox(this, wxID_ANY, _("Navigation"));
	m_pGUI_itemBoxSizer = new wxStaticBoxSizer(m_pGUI_itemBox, wxVERTICAL);
	m_pGUI_itemBoxSizer->Add(m_pGUI_itemIndexSizer);
	m_pGUI_itemBoxSizer->Add(m_pGUI_goToOriginal, 0, wxEXPAND, 0);
	m_pGUI_itemBoxSizer->Add(m_pGUI_goToTranslated, 0, wxEXPAND, 0);

	m_pGUI_lateralSizer = new wxBoxSizer(wxVERTICAL);
	m_pGUI_lateralSizer->AddSpacer(4);
	m_pGUI_lateralSizer->Add(m_pGUI_blockBoxSizer);
	m_pGUI_lateralSizer->AddSpacer(4);
	m_pGUI_lateralSizer->Add(m_pGUI_itemBoxSizer, 0, wxEXPAND, 0);

	m_pGUI_origItemName = new wxStaticText(this, wxID_ANY, wxEmptyString);

	m_pGUI_transItemName = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
	m_pGUI_transItemDescription = new wxStyledTextCtrl(this, wxID_ANY);

	m_pGUI_saveText = new wxButton(this, wxID_ANY, _("Save"));

	wxSize buttons_size(0, 0);
	buttons_size.SetWidth(m_pGUI_saveText->GetTextExtent(_("Save")).GetWidth() * 2);
	buttons_size.SetHeight(m_pGUI_saveText->GetSize().GetHeight());
	m_pGUI_index_input->SetMinSize(buttons_size);	

	m_pGUI_saveText->SetMinSize(buttons_size);
	m_pGUI_prevText = new wxButton(this, wxID_ANY, "<<");
	m_pGUI_prevText->SetMinSize(buttons_size);
	m_pGUI_proxText = new wxButton(this, wxID_ANY, ">>");
	m_pGUI_proxText->SetMinSize(buttons_size);

	m_pGUI_navigationSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pGUI_navigationSizer->Add(m_pGUI_saveText, 0, 0, 0);
	m_pGUI_navigationSizer->AddStretchSpacer();
	m_pGUI_navigationSizer->Add(m_pGUI_prevText, 0, 0, 0);
	m_pGUI_navigationSizer->AddSpacer(4);
	m_pGUI_navigationSizer->Add(m_pGUI_proxText, 0, 0, 0);

	m_pGUI_origItemName = new wxStaticText(this, wxID_ANY, "Original Item Name");
	m_pGUI_origItemDescription = new wxStaticText(this, wxID_ANY, "Original Item Description \n\n\n");

	m_pGUI_origBox = new wxStaticBox(this, wxID_ANY, "Original");
	m_pGUI_origBoxSizer = new wxStaticBoxSizer(m_pGUI_origBox, wxVERTICAL);
	m_pGUI_origBoxSizer->Add(m_pGUI_origItemName, 0, wxALL, 4);
	m_pGUI_origBoxSizer->Add(m_pGUI_origItemDescription, 0, wxALL, 4);

	m_pGUI_editorSizer = new wxBoxSizer(wxVERTICAL);	
	m_pGUI_editorSizer->Add(m_pGUI_transItemName, 0, wxALL | wxEXPAND, 4);
	m_pGUI_editorSizer->Add(m_pGUI_transItemDescription, 1, wxALL | wxEXPAND, 4);
	m_pGUI_editorSizer->Add(m_pGUI_navigationSizer, 0, wxALL | wxEXPAND, 4);
	//m_pGUI_mainSizer->AddSpacer(4);
	m_pGUI_editorSizer->Add(m_pGUI_origBoxSizer, 0, wxALL | wxEXPAND, 4);

	m_pGUI_mainSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pGUI_mainSizer->AddSpacer(4);
	m_pGUI_mainSizer->Add(m_pGUI_lateralSizer, 0, wxEXPAND, 0);
	m_pGUI_mainSizer->AddSpacer(4);
	m_pGUI_mainSizer->Add(m_pGUI_editorSizer, 1, wxEXPAND, 0);

	SetSizer(m_pGUI_mainSizer);
	m_pGUI_mainSizer->Fit(this);
	m_pGUI_mainSizer->SetSizeHints(this);
}