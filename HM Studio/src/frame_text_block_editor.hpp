#pragma once

#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>

#include "class_rom.hpp"
#include "class_stc.hpp"

class TextBlockEditor : public wxFrame
{
public:
	TextBlockEditor(const id i);
	~TextBlockEditor() = default;
private:
	Rom m_RomTranslated;
	Rom m_RomOriginal;
//Events
private:
	void OnMenuClick(wxCommandEvent& event);
private:
	void CreateGUIControls();

	STC* m_pSTC = nullptr;
	STC* m_pSTCOriginal = nullptr;

	wxBoxSizer* m_pRootSizer = nullptr;
};

class SetBlockDialog : public wxDialog
{
public:
	SetBlockDialog();
	~SetBlockDialog() = default;
private:
	void OnGoButton(wxCommandEvent& event);
public:
	uint32_t start;
	uint32_t end;
	uint32_t pointers;
private:
	void CreateGUIControls();
public:
	wxTextCtrl* m_pStartInput = nullptr;
	wxTextCtrl* m_pEndInput = nullptr;
	wxTextCtrl* m_pPointersInput = nullptr;
};