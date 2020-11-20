#include "frame_main.hpp"
#include <wx/confbase.h>

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "HM Studio")
{
	wxConfigBase* pConfig = wxConfigBase::Get();

	m_DefaultSelection = pConfig->Read(L"/Global/DefaultRom", (long)0);

	if (pConfig->ReadBool(L"/MainFrame/Top", false))
	{
		ToggleWindowStyle(wxSTAY_ON_TOP);		
	}	

	CreateGUIControls();
	Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);

	m_pSelection->Bind(wxEVT_CHOICE, &MainFrame::OnSelectionChange, this);	

#ifdef _DEBUG
#ifdef EDITING_EDITOR_FRAME
	EDITING_EDITOR_FRAME* frame = new EDITING_EDITOR_FRAME(GetCurrentId());
	frame->Show();

	Iconize();
#endif //_EDITING_EDITOR_FRAME
#endif //_DEBUG
}

void MainFrame::OnOpenRomClick(wxCommandEvent& event)
{
	id id = GetCurrentId();
	
	if (wxProcess::Exists(m_Processes[(int)id]))
	{
		wxProcess::Kill(m_Processes[(int)id]);
		m_Processes[(int)id] = 0;
	}

	RomFile rom(id, true);

	/*The below code is from wxWidgets exec sample*/

	wxString ext = wxFileName(rom.Path).GetExt();
	wxFileType* ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
	if (!ft)
	{
		wxLogError("Impossible to determine the file type for extension '%s'",
			ext);
		return;
	}

	wxString cmd;
	bool ok = false;
	const wxFileType::MessageParameters params(rom.Path);
#ifdef __WXMSW__
	// try editor, for instance Notepad if extension is .xml
	cmd = ft->GetExpandedCommand("edit", params);
	ok = !cmd.empty();
#endif
	if (!ok) // else try viewer
		ok = ft->GetOpenCommand(&cmd, params);
	delete ft;
	if (!ok)
	{
		wxLogError("Impossible to find out how to open files of extension '%s'",
			ext);
		return;
	}

	wxProcess* process = new wxProcess(this);
	long pid = wxExecute(cmd, wxEXEC_ASYNC, process);

	if (!pid)
	{
		wxLogError("Execution of '%s' failed.", cmd);

		delete process;
		return;
	}

	m_Processes[(int)id] = pid;

	event.Skip();
}

void MainFrame::OnStatusSize(wxSizeEvent& event)
{
	wxRect rect;
		
	if (m_frameStatusBar->GetFieldRect(1, rect))
	{		
		rect.x += 4;		
		m_pSelection->SetSize(rect.GetSize());
		m_pSelection->SetPosition(rect.GetPosition());
	}

	event.Skip();
}

void MainFrame::OnTestClick(wxCommandEvent& event)
{

}

void MainFrame::OnClose(wxCloseEvent& event)
{
	//Close all opened emulators
	for (const long& pid : m_Processes)
	{
		if(pid != -1)
		{
			if (wxProcess::Exists(pid))
			{
				wxProcess::Kill(pid);
			}
		}		
	}

	wxTheApp->ExitMainLoop();	

	event.Skip();
}

void MainFrame::OnOpenFolderClick(wxCommandEvent& event)
{
	RomFile rom(GetCurrentId(), true);

	if(!wxDirExists(rom.m_HomeDir))
	{
		wxLogError("The project folder does not exists.");
		return;
	}

	wxString path;
	path << "\"" << rom.m_HomeDir << "\"";

#ifdef __WXMSW__
	wxExecute(wxString("explorer ") << path, wxEXEC_ASYNC, NULL);
#elif defined(__WXGTK__)
	wxExecute(wxString("xdg-open  ") << path, wxEXEC_ASYNC, NULL);
#else //Not supported platforms
	wxLogError("This is not implemented in the current platform.");
#endif

	event.Skip();
}

id MainFrame::GetCurrentId()
{
	switch ((id)m_pSelection->GetSelection())
	{
	case id::FoMT:
		return id::FoMT;
		break;
	case id::MFoMT:
		return id::MFoMT;
		break;
	case id::DS:
		return id::DS;
		break;
	default:
		wxMessageBox("This is not suposed to happen...", "Huh?", 5L, this);
		return (id)-1;
		break;
	}
}

void MainFrame::OnSelectionChange(wxCommandEvent& event)
{
	wxMenu* fileMenu = GetMenuBar()->GetMenu(0);

	int id = fileMenu->FindItem(L"Default");

	fileMenu->Check(id, event.GetSelection() == m_DefaultSelection);	

	event.Skip();
}

void MainFrame::OnOpenDefaultClick(wxCommandEvent& event)
{
	wxMenu* fileMenu = GetMenuBar()->GetMenu(0);

	int id = fileMenu->FindItem(L"Default");

	fileMenu->Check(id, true);

	m_DefaultSelection = m_pSelection->GetSelection();
	
	wxConfigBase* pConfig = wxConfigBase::Get();
	pConfig->Write(L"/Global/DefaultRom", m_DefaultSelection);
	pConfig->Flush();

	event.Skip();
}

void MainFrame::OnBackupClick(wxCommandEvent& event)
{
	wxString src = RomFile(GetCurrentId(), true).Path;
	wxString dst = src;
	dst.Replace(L"Rom_Translated", L"Backup", true);

	wxCopyFile(src, dst, true);

	event.Skip();
}

void MainFrame::OnCopyFromOriginal(wxCommandEvent& event)
{
	wxString src = RomFile(GetCurrentId(), false).Path;
	wxString dst = src;
	dst.Replace(L"Rom_Original", L"Rom_Translated", true);
	wxCopyFile(src, dst, true);

	event.Skip();
}

void MainFrame::OnSpellCheckerClick(wxCommandEvent& event)
{
	SpellCheckerDialog().ShowModal();	

	event.Skip();
}

void MainFrame::OnAlwaysOnTopClick(wxCommandEvent& event)
{
	wxConfigBase* pConfig = wxConfigBase::Get();

	pConfig->Write(L"/MainFrame/Top", m_pAlwaysOnTop->IsChecked());
	pConfig->Flush();

	ToggleWindowStyle(wxSTAY_ON_TOP);

	event.Skip();
}

void MainFrame::CreateGUIControls()
{
	wxMenu* fileMenu = new wxMenu();
	fileMenu->Bind(wxEVT_MENU, &MainFrame::OnOpenRomClick, this, fileMenu->Append(wxID_ANY, L"Start", L"Start the ROM with the default emulator")->GetId());
	fileMenu->Bind(wxEVT_MENU, &MainFrame::OnOpenFolderClick, this, fileMenu->Append(wxID_ANY, L"Open Folder", L"Open the project folder")->GetId());
	fileMenu->Bind(wxEVT_MENU, &MainFrame::OnBackupClick, this, fileMenu->Append(wxID_ANY, L"Backup", L"Create a copy of the ROM")->GetId());	
	fileMenu->Bind(wxEVT_MENU, &MainFrame::OnCopyFromOriginal, this, fileMenu->Append(wxID_ANY, L"Copy From Original", L"Copy the original ROM")->GetId());

	wxMenuItem* default_check = fileMenu->AppendCheckItem(wxID_ANY, L"Default", L"Always open HM Studio with this ROM");
	default_check->Check(true);

	fileMenu->Bind(wxEVT_MENU, &MainFrame::OnOpenDefaultClick, this, default_check->GetId());

	wxMenu* editorsMenu = new wxMenu();
	editorsMenu->Bind(wxEVT_MENU, &MainFrame::OnEditorClick<ScriptEditorFrame>, this, editorsMenu->Append(wxID_ANY, L"Script Editor")->GetId());
	editorsMenu->Bind(wxEVT_MENU, &MainFrame::OnEditorClick<ItemEditorFrame>, this, editorsMenu->Append(wxID_ANY, L"Item Editor")->GetId());
	editorsMenu->Bind(wxEVT_MENU, &MainFrame::OnEditorClick<LetterEditorFrame>, this, editorsMenu->Append(wxID_ANY, L"Letter Editor")->GetId());
	editorsMenu->Bind(wxEVT_MENU, &MainFrame::OnEditorClick<StringEditorFrame>, this, editorsMenu->Append(wxID_ANY, L"String Editor")->GetId());
	editorsMenu->Bind(wxEVT_MENU, &MainFrame::OnEditorClick<GraphicsEditorFrame>, this, editorsMenu->Append(wxID_ANY, L"Graphics Editor")->GetId());
	editorsMenu->Bind(wxEVT_MENU, &MainFrame::OnEditorClick<ValueEditorFrame>, this, editorsMenu->Append(wxID_ANY, L"Value Editor")->GetId());
	editorsMenu->Bind(wxEVT_MENU, &MainFrame::OnEditorClick<AnimationEditorFrame>, this, editorsMenu->Append(wxID_ANY, L"Animation Editor")->GetId());

	wxMenu* toolsMenu = new wxMenu();
	toolsMenu->Bind(wxEVT_MENU, &MainFrame::OnSpellCheckerClick, this, toolsMenu->Append(wxID_ANY, L"Spell Checker", L"Spell Checker Settings")->GetId());

	wxMenu* viewMenu = new wxMenu();
	m_pAlwaysOnTop = viewMenu->AppendCheckItem(wxID_ANY, L"Always on Top", L"Show window always on top");
	m_pAlwaysOnTop->Check(GetWindowStyle() & wxSTAY_ON_TOP);

	viewMenu->Bind(wxEVT_MENU, &MainFrame::OnAlwaysOnTopClick, this, m_pAlwaysOnTop->GetId());

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, L"File");
	menuBar->Append(editorsMenu, L"Editors");
	menuBar->Append(toolsMenu, L"Tools");
	menuBar->Append(viewMenu, L"View");
	
	SetMenuBar(menuBar);

	this->SetBackgroundColour(wxColour(240, 240, 240, 255));

	CreateStatusBar(2);
	m_frameStatusBar->SetStatusText(L"HM Studio");

	m_pSelection = new wxChoice(m_frameStatusBar, wxID_ANY);
	m_pSelection->Insert("FoMT", 0);
	m_pSelection->Insert("MFoMT", 1);
	m_pSelection->Insert("DS", 2);
	m_pSelection->SetSelection(m_DefaultSelection);

	int widths[2]{ -1, m_pSelection->GetSize().GetWidth() + 10 };
	m_frameStatusBar->SetStatusWidths(2, widths);

	m_frameStatusBar->Bind(wxEVT_SIZE, &MainFrame::OnStatusSize, this);		

	SetMinSize(wxSize(300, 200));
	SetSize(wxSize(300, 200));
}