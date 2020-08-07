#include "frame_script_editor.hpp"

ScriptEditorFrame::ScriptEditorFrame(id i) : EditorFrame(new ScriptEditor(i))
{
	CreateGUIControls();
	SetupRom(); 
	ConfigureSTC(tScriptOriginal, m_pEditor->GetRom(false));
	ConfigureSTC(tScriptTranslated, m_pEditor->GetRom(true));
	RestoreText();	
}

ScriptEditorFrame::~ScriptEditorFrame()
{

}

void ScriptEditorFrame::SetupRom()
{	
	this->SetTitle("ScriptEditor::" + m_pEditor->GetRom(false).Name);

	wxFileName fn(((ScriptEditor*)m_pEditor)->GetScriptDir());
	fn.AppendDir("Backup");
	fn.SetName("backup");
	fn.SetExt("temp");
	m_BackupFile = fn.GetFullPath().ToStdString();
	
	m_ScriptDic.SetPath(((ScriptEditor*)m_pEditor)->GetScriptDir());
	m_ScriptDic.AppendDir("Dics");	
	m_ScriptDic.SetExt("usr");

	if (!m_ScriptDic.DirExists())
	{
		m_ScriptDic.Mkdir();
	}
}

void ScriptEditorFrame::ConfigureSTC(STC* stc, const RomFile& rom)
{		
	const VarTable& table = rom.GetVarTable();	
	
	for(size_t i = 0; i < table.Size(); ++i)
	{		
		stc->AddVar(std::string(table.GetName(i)));
	}	
	
	int eolMode = rom.GetEOL() == 0x0a ? wxSTC_EOL_LF : wxSTC_EOL_CRLF;

	stc->SetMaxLineLenght(rom.GetLineMax());
	stc->SetEOLMode(eolMode);
	stc->InsertOnCtrlKey(std::string(1, (char)0x05), 'E');
	stc->InsertOnCtrlKey(std::string(1, (char)0x0c) + stc->GetEOL().ToStdString(), 'R');
}

void ScriptEditorFrame::BackupText()
{	
	size_t max = 0;

	for (const std::string& str : ((ScriptEditor*)m_pEditor)->GetTranlated())
	{
		max += str.size() + 1;
	}

	size_t size = sizeof(size_t);

	std::string buffer;
	buffer.reserve(max + (size * 2));

	for (const std::string& str : ((ScriptEditor*)m_pEditor)->GetTranlated())
	{
		buffer.append(str);
		buffer.push_back('\0');
	}

	size_t number = m_pEditor->GetNumber();
	size_t index = m_pEditor->GetIndex();

	if (index > 0)
		index--;

	buffer.append((const char*)&number, size);
	buffer.append((const char*)&index, size);

	Moon::File::WriteAllText(m_BackupFile, buffer);
}

void ScriptEditorFrame::RestoreText()
{
	bool good = true;

	if (wxFile::Exists(m_BackupFile))
	{		
		std::string buffer = Moon::File::ReadAllText(m_BackupFile);
		size_t size = buffer.size();

		if (size > 8)
		{
			uint32_t* number = (uint32_t*)(buffer.c_str() + size - 8);
			uint32_t* index = (uint32_t*)(buffer.c_str() + size - 4);

			if (m_pEditor->Open(*number))
			{
				if (m_pEditor->SetIndex(*index))
				{
					UpdateText();
				}
			}

			if (m_pEditor->GetIndex() != *index)
				return;

			std::vector<std::string> text;
			text.reserve(m_pEditor->GetCount());

			size_t cursor = 0;

			for (size_t i = 0; i < m_pEditor->GetCount(); ++i)
			{
				if (cursor >= size)
				{
					good = false;
					break;
				}

				text.push_back(std::string(buffer.c_str() + cursor));
				cursor += text[i].size() + 1;
			}

			((ScriptEditor*)m_pEditor)->GetTranlated() = text;
			UpdateText();
		}
		else
		{
			good = false;
		}

		if (!good)
			wxMessageDialog(nullptr, "Corrupted backup file", "Huh?", wxICON_ERROR).ShowModal();
	}
}

void ScriptEditorFrame::OpenScript(size_t index)
{	
	m_pEditor->Open(index);
	UpdateScript();
}

void ScriptEditorFrame::SaveScript()
{		
	m_pEditor->SaveFile();

	if (wxFile::Exists(m_BackupFile))
	{
		wxRemoveFile(m_BackupFile);
	}
}

void ScriptEditorFrame::UpdateScriptDic()
{	
	if (m_DicIndex != std::string::npos)
	{
		SpellChecker::RemoveUserDic(m_DicIndex);
		tScriptTranslated->RemoveDicToMenu(m_DicIndex);
	}	

	m_ScriptDic.SetName(std::to_string(m_pEditor->GetNumber()));

	m_DicIndex = SpellChecker::AddUserDic(m_ScriptDic.GetFullPath().ToStdString());
	tScriptTranslated->AppendDicToMenu("Script", m_DicIndex);
}

void ScriptEditorFrame::UpdateScript()
{
	this->SetTitle(wxString("ScriptEditor::") << m_pEditor->GetRom(false).Name << " - " << m_pEditor->GetNumber());
		
	UpdateScriptDic();
	UpdateText();
}

void ScriptEditorFrame::FindText()
{
	FrameSearchScript dialog;

	if (!m_pEditor->IsOpened())
		dialog.InScript(true);

	if (m_pEditor->GetRom(true).Console == console::DS)
	{
		dialog.SetEol("\\n", "\n");
	}
	else
	{
		dialog.SetEol("\\r\\n", "\r\n");
	}

	if (dialog.ShowModal() != wxID_CANCEL)
	{
		std::string search = dialog.GetSearch().ToStdString();
		std::string replace = dialog.GetReplace().ToStdString();
		bool inScript = dialog.InScript();
		bool extended = dialog.Extended();
		bool translated = dialog.Translated();
		bool find = dialog.Find();
		bool useTable = dialog.UseTable();

		if (find)
		{
			if (!inScript)
			{
				m_FindPos = m_pEditor->Find(search, useTable, translated);
				m_FindIndex = 0;
				m_pEditor->SetIndex(m_FindPos[m_FindIndex]);
			}
			else
			{				
				ShowResultWindow(m_pEditor->FindInFiles(search, useTable, translated));
			}
		}
		else
		{
			if (!inScript)
			{
				//ToDo
				//m_pEditor->Replace(search, useTable, replace);
				UpdateText();

				m_FindPos.clear();
				m_FindIndex = 0;
			}
			else
			{
				wxMessageBox("Replace in script is not yet supported in this version.", "Huh?", wxICON_WARNING);
			}
		}
	}
	
	Layout();
}

void ScriptEditorFrame::tScritpTranslatedOnModified(wxStyledTextEvent& event)
{	
	event.Skip();
}

void ScriptEditorFrame::tScriptTranslatedOnUi(wxStyledTextEvent& event)
{		
	CallAfter(&ScriptEditorFrame::UpdateStatusText, tScriptTranslated);

	event.Skip();	
}

void ScriptEditorFrame::OnSaveString()
{
	if (tScriptTranslated->GetModify())
	{
		m_pEditor->SaveText(tScriptTranslated->GetText().ToStdString());
		BackupText();
	}
	else
	{
		m_pEditor->NextText();
	}

	UpdateText();
}

void ScriptEditorFrame::OnSetEventClick(wxCommandEvent& event)
{
	wxTextEntryDialog ted(this, "Got to script ", "Enter a number");
	ted.SetTextValidator(wxFILTER_NUMERIC);
	ted.SetMaxLength(4);

	if (ted.ShowModal() == wxID_OK)
	{		
		((ScriptEditor*)m_pEditor)->SetEvent(std::stoi(ted.GetValue().ToStdString()));
	}

	event.Skip();
}

void ScriptEditorFrame::OnResultClick(wxCommandEvent& event)
{	
	const SearchResult& result = *(SearchResult*)event.GetClientData();	
	
	size_t number = GetNumberFromResult(result);
	size_t index = GetIndexFromResult(result, event.GetInt());
			
	if (m_pEditor->Open(number))
	{
		if (m_pEditor->SetIndex(index))
		{
			UpdateText();
		}
	}	
			
	event.Skip();
}

size_t ScriptEditorFrame::GetNumberFromResult(const SearchResult& result)
{
	return std::stoi(result.GetTitle().substr(result.GetTitle().find("_", 7) + 1).ToStdString());
}

size_t ScriptEditorFrame::GetIndexFromResult(const SearchResult& result, size_t clicked)
{
	return std::stoi(result.GetHits()[clicked].substr(7).ToStdString());
}

void ScriptEditorFrame::SetTextRange()
{
	DialogTextRange dialog(this);
	
	dialog.ShowModal();	
}

void ScriptEditorFrame::UpdateStatusText(wxStyledTextCtrl* stc)
{
	m_pStatusBar->SetStatusText(wxString("Size: ") << std::to_string(stc->GetTextLength()), 1);

	m_pStatusBar->SetStatusText(wxString("Ln: ") << stc->GetCurrentLine() + 1, 2);

	int selSize = (stc->GetSelectionEnd() - stc->GetSelectionStart());

	if (selSize > 0)
		selSize = stc->GetSelectedText().size();

	m_pStatusBar->SetStatusText(wxString("Sel: ") << selSize, 3);

	m_pStatusBar->SetStatusText(wxString("Col: ") << tScriptTranslated->GetColumn(tScriptTranslated->GetCurrentPos()), 4);
}

void ScriptEditorFrame::UpdateText()
{	
	tScriptOriginal->SetText(((ScriptEditor*)m_pEditor)->GetCurOriginal());
	tScriptTranslated->SetText(((ScriptEditor*)m_pEditor)->GetCurTranslated());

	m_pStatusBar->SetStatusText(wxString("Index: ") << std::to_string(m_pEditor->GetIndex() + 1) << "/" << std::to_string(m_pEditor->GetCount()));
		
	tScriptTranslated->SetModified(false);
}

void ScriptEditorFrame::CreateGUIControls()
{
	this->SetBackgroundColour(wxColour(240, 240, 240));

	wxMenu* menuEdit = new wxMenu();
	menuEdit->Append(wxID_ANY, "Move To");
	menuEdit->Bind(wxEVT_MENU, &ScriptEditorFrame::OnConvertEOLClick, this, menuEdit->Append(wxNewId(), "Convert EOL")->GetId());
	
	CreateMyMenuBar();
	CreateFileMenu();
	CreateStringMenu();
	m_pMenuBar->Append(menuEdit, L"Edit");
	CreateSearchMenu();
	CreateToolsMenu(false, true, true, true);
	m_pMenuTools->Bind(wxEVT_MENU, &ScriptEditorFrame::OnCheckCodeClick, this, m_pMenuTools->Append(wxNewId(), "Code Checker", nullptr, "Checking Tool")->GetId());
	CreateViewMenu();
	m_pMenuView->Bind(wxEVT_MENU, &ScriptEditorFrame::OnHorizontalModeClick, this, m_pMenuView->Append(wxID_ANY, L"Horizontal Mode")->GetId());
	CreateMyStatusBar(5);

	int widths[5] = { -5, -1, -1, -1, -1 };

	m_pStatusBar->SetStatusWidths(m_pStatusBar->GetFieldsCount(), widths);

	//--------------------//
	// Menu creation ends //
	//--------------------//	

//-----------------------------------------------------------------//

	CreateMyToolBar();

	tScriptTranslated = new STC(this, wxID_ANY);
	tScriptOriginal = new STC(this, wxID_ANY);
	
	tScriptTranslated->Bind(wxEVT_STC_CHANGE, &ScriptEditorFrame::tScritpTranslatedOnModified, this);
	tScriptTranslated->Bind(wxEVT_STC_UPDATEUI, &ScriptEditorFrame::tScriptTranslatedOnUi, this);	

	CreateButtonsSizer();

	editor_sizer = new wxBoxSizer(wxVERTICAL);	
	editor_sizer->Add(tScriptTranslated, 2, wxALL | wxEXPAND, 0);
	editor_sizer->Add(m_pButtonsSizer, 0, wxUP | wxBOTTOM | wxEXPAND, 4);
	editor_sizer->Add(tScriptOriginal, 2, wxALL | wxEXPAND, 0);

	global_sizer = new wxBoxSizer(wxVERTICAL);	
	global_sizer->Add(editor_sizer, 1, wxALL | wxEXPAND, 0);		

	SetSizer(global_sizer);
	global_sizer->Fit(this);
	global_sizer->SetSizeHints(this);
}

void ScriptEditorFrame::OnConvertEOLClick(wxCommandEvent& event)
{
	for (std::string& text : ((ScriptEditor*)(m_pEditor))->GetTranlated())
		Moon::String::ConvertLineEnds(text, 0x0d0a);

	event.Skip();
}

void ScriptEditorFrame::OnCheckCodeClick(wxCommandEvent& event)
{
	ShowResultWindow(((ScriptEditor*)(m_pEditor))->CheckCode());
	event.Skip();
}

void ScriptEditorFrame::OnHorizontalModeClick(wxCommandEvent& event)
{
	if (m_Vertical) SetEditorHorizontal();
	else SetEditorVertical();

	m_Vertical = !m_Vertical;
}

void ScriptEditorFrame::SetEditorVertical()
{	
	if (editor_sizer != nullptr)
		editor_sizer->SetOrientation(wxVERTICAL);

	Layout();

	global_sizer->Fit(this);
	global_sizer->SetSizeHints(this);
}

void ScriptEditorFrame::SetEditorHorizontal()
{
	if (editor_buttons_sizer != nullptr)
	{
		editor_sizer->Detach(1);
		global_sizer->Add(editor_buttons_sizer, 0, wxUP | wxBOTTOM | wxEXPAND, 4);
	}	

	if (editor_sizer != nullptr)
		editor_sizer->SetOrientation(wxHORIZONTAL);

	Layout();

	global_sizer->Fit(this);
	global_sizer->SetSizeHints(this);
}

void ScriptEditorFrame::ScriptTextRange(size_t from, size_t to, size_t script)
{

}

DialogTextRange::DialogTextRange(ScriptEditorFrame* parent) : wxDialog(nullptr, wxID_ANY, "Select range"), m_pParent(parent)
{
	CreateGUIControls();
}

void DialogTextRange::CreateGUIControls()
{
	wxStaticText* dialogLabelFrom = new wxStaticText(this, wxID_ANY, "From:  ");
	dialogInputFrom = new wxTextCtrl(this, wxID_ANY);

	wxBoxSizer* dialogFromSizer = new wxBoxSizer(wxHORIZONTAL);
	dialogFromSizer->AddSpacer(4);
	dialogFromSizer->Add(dialogLabelFrom, 0, wxEXPAND, 0);
	dialogFromSizer->AddSpacer(4);
	dialogFromSizer->Add(dialogInputFrom, 0, wxEXPAND, 0);

	wxStaticText* dialogLabelTo = new wxStaticText(this, wxID_ANY, "To: ");
	dialogInputTo = new wxTextCtrl(this, wxID_ANY);

	wxBoxSizer* dialogToSizer = new wxBoxSizer(wxHORIZONTAL);
	dialogFromSizer->AddSpacer(4);
	dialogFromSizer->Add(dialogLabelTo, 0, wxEXPAND, 0);
	dialogFromSizer->AddSpacer(4);
	dialogFromSizer->Add(dialogInputTo, 0, wxEXPAND, 0);
	
	wxStaticText* dialogLabelScript = new wxStaticText(this, wxID_ANY, "Script:");
	dialogInputScript = new wxTextCtrl(this, wxID_ANY);
	dialogGoButton = new wxButton(this, wxID_ANY, "Go!");
	dialogGoButton->Bind(wxEVT_BUTTON, &DialogTextRange::OnGoButton, this);
	
	wxBoxSizer*   dialogScriptSizer = new wxBoxSizer(wxHORIZONTAL);
	dialogScriptSizer->AddSpacer(4);
	dialogScriptSizer->Add(dialogLabelScript, 0, wxEXPAND, 0);
	dialogScriptSizer->AddSpacer(4);
	dialogScriptSizer->Add(dialogInputScript, 0, wxEXPAND, 0);
	dialogScriptSizer->AddStretchSpacer(1);
	dialogScriptSizer->Add(dialogGoButton, 0, wxEXPAND, 0);

	wxBoxSizer* dialogRootSizer = new wxBoxSizer(wxVERTICAL);
	dialogRootSizer->AddSpacer(4);
	dialogRootSizer->Add(dialogFromSizer, 0, wxEXPAND, 0);
	dialogRootSizer->AddSpacer(4);
	dialogRootSizer->Add(dialogToSizer, 0, wxEXPAND, 0);
	dialogRootSizer->AddSpacer(4);
	dialogRootSizer->Add(dialogScriptSizer, 0, wxEXPAND, 0);	

	this->SetSizer(dialogRootSizer);
	dialogRootSizer->SetSizeHints(this);
	dialogRootSizer->Fit(this);	
}

void DialogTextRange::OnGoButton(wxCommandEvent& event)
{
	if(dialogInputFrom->GetLineLength(0) == 0)
	{
		wxMessageBox("We need a number to start from...", "Huh?");
		event.Skip();
		return;
	}
	
	if(dialogInputTo->GetLineLength(0) == 0)
	{
		wxMessageBox("We need a number to end from...", "Huh?");
		event.Skip();
		return;
	}
	
	if(dialogInputScript->GetLineLength(0) == 0)
	{
		wxMessageBox("We need a script to get the text from...", "Huh?");
		event.Skip();
		return;
	}
	
	size_t from = std::stoi(dialogInputFrom->GetValue().ToStdString());
	size_t to  = std::stoi(dialogInputTo->GetValue().ToStdString());
	size_t script = std::stoi(dialogInputScript->GetValue().ToStdString());
		
	m_pParent->ScriptTextRange(from, to, script);
	
	this->EndModal(wxID_OK);
	event.Skip();
}
