#pragma once

#include <wx/frame.h>
#include <wx/button.h>
#include <wx/statbox.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/mimetype.h>

#include "class_rom_file.hpp"
//Frames
#include "frame_script_editor.hpp"
#include "frame_item_editor.hpp"
#include "frame_letter_editor.hpp"
#include "frame_graphics_editor.hpp"
#include "frame_string_editor.hpp"
#include "frame_value_editor.hpp"
#include "animation_frame.hpp"

#include "dialog_spellchecker.hpp"

class MainFrame : public wxFrame
{
public:
	MainFrame();
	~MainFrame() = default;

private:	
	void OnEditorClick(wxCommandEvent& event);	
	void OnTestClick(wxCommandEvent& event);
	void OnStatusSize(wxSizeEvent& event);
	void OnOpenRomClick(wxCommandEvent& event);
	void OnOpenFolderClick(wxCommandEvent& event);
	void OnSelectionChange(wxCommandEvent& event);
	void OnOpenDefaultClick(wxCommandEvent& event);
	void OnBackupClick(wxCommandEvent& event);
	void OnCopyFromOriginal(wxCommandEvent& event);
	void OnSpellCheckerClick(wxCommandEvent& event);
	void SetWorkingFolder(wxCommandEvent& event);
	void OnAlwaysOnTopClick(wxCommandEvent& event);	
	void OnClose(wxCloseEvent& event);
private:
	int m_DefaultSelection;
	id GetCurrentId();
	long m_Processes[3] { -1, -1, -1 };

	template<typename T>
	void OnEditorClick(wxCommandEvent& event)
	{
		T* t = new T(GetCurrentId());
		t->Show();

		event.Skip();
	}
//GUI
private:
	void CreateGUIControls();
	wxChoice* m_pSelection = nullptr;
	wxMenuItem* m_pAlwaysOnTop = nullptr;
};

