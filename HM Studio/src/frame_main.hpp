#pragma once

#include <wx/frame.h>
#include <wx/button.h>
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>
//Frames
#include "frame_script_editor.hpp"
#include "frame_item_editor.hpp"
#include "frame_text_block_editor.hpp"

class MainFrame : public wxFrame
{
public:
	MainFrame();
	~MainFrame() = default;

private:	
	void OnEditorClick(wxCommandEvent& event);	
	void OnTestClick(wxCommandEvent& event);
private:
	id GetCurrentId();
//UI
private:
	void CreateGUIControls();
		
	wxButton* m_pOpenRom = nullptr;
	wxButton* m_pBackup = nullptr;
	wxChoice* m_pSelection = nullptr;
	wxCheckBox* m_pDefault = nullptr;

	wxButton* m_pEditorScript = nullptr;
	wxButton* m_pEditorItem = nullptr;
	wxButton* m_pEditorText = nullptr;

#ifdef _DEBUG
	wxButton* m_pTestButton = nullptr;
#endif
};

