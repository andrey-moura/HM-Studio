#include "cScriptEditor.h"

wxBEGIN_EVENT_TABLE(cScriptEditor, wxFrame)
//
//wxEVT_STC_MODIFIED(10001, cScriptEditor::tScritpTranslatedOnModified)
//

//EVT_KEY_DOWN(ID_SCRIPT_NAV_INPUT, OnInputKeyDown)

wxEND_EVENT_TABLE()

cScriptEditor::cScriptEditor(id i) : wxFrame(nullptr, wxID_ANY, "Script Editor")
{
	CreateGUIControls();	

	romOriginal = new Rom(i, false);
	romTranslated = new Rom(i, true);

	this->SetTitle(wxString(_("Script Editor - ")) << romOriginal->Name);

	clip_board = new wxClipboard();	
}

cScriptEditor::~cScriptEditor()
{
	delete clip_board;
	delete scriptOriginal;
	delete scriptTranslated;
	delete romOriginal;
	delete romTranslated;
}

void cScriptEditor::OpenScript()
{
	SpriteFile* script = new SpriteFile(FileUtil::ReadAllBytes(romOriginal->GetScriptFullPath(scriptNum)));

	if (script->HaveText())
	{
		delete script;		

		delete scriptOriginal;
		delete scriptTranslated;

		scriptOriginal = new SpriteFile(FileUtil::ReadAllBytes(romOriginal->GetScriptFullPath(scriptNum)));
		scriptTranslated = new SpriteFile(FileUtil::ReadAllBytes(romTranslated->GetScriptFullPath(scriptNum)));

		UpdateScript();
	}		
	else {
		wxMessageBox(_("This script don't have text."), _("Huh?"), 5L, this);
		delete script;		
	}
}

void cScriptEditor::SaveScript()
{	
	std::vector<std::string> text = textTranslated;
	romTranslated->OutputTextWithVariables(text);
	scriptTranslated->UpdateText(text);
	FileUtil::WriteAllBytes(romTranslated->GetScriptFullPath(scriptNum), scriptTranslated->data);
}

void cScriptEditor::ExportScript()
{
	std::string text;

	for (int i = 0; i < textTranslated.size(); ++i)
	{
		text.append(textTranslated[i]);
		text.append(std::string("\n---End text ").append(std::to_string(i).append("---\n")));
	}

	FileUtil::WriteAllText(romTranslated->GetScriptExportedFullPath(scriptNum), text);
}

void cScriptEditor::UpdateScript()
{
	index = 0;

	textOriginal = scriptOriginal->GetText();
	textTranslated = scriptTranslated->GetText();

	//StringUtil::Replace("Eu", "Você", textTranslated[0]);

	romTranslated->InputTextWithVariables(textOriginal, textTranslated);	

	uint32_t pointer = 0;
	romTranslated->GetOffset(pointer, scriptNum);

	std::string s_pointer = "0000000";
	_itoa(pointer, (char*)s_pointer.c_str(), 16);
	
	while (s_pointer.size() < 9)
		s_pointer = '0' + s_pointer;

	script_nav_offset->SetLabel(s_pointer);

	this->SetTitle(wxString() << "Script Editor - " << romOriginal->Name << ": Script" << scriptNum);

	UpdateText();
}

void cScriptEditor::CheckAndGoScript(int index)
{
	if (index >= 0 && index < romOriginal->Offset.Script_count)
	{
		if (index != scriptNum)
		{
			scriptNum = index;
			OpenScript();
		}
	}
	else
	{
		wxMessageBox(wxString("Index out of the range. Index have to be greater than or equal to 0 and less than ")
							<< romOriginal->Offset.Script_count -1 << ".", "Huh?", 5L, this);
	}
}

void cScriptEditor::GetTextFromScriptFile()
{
	if (scriptOriginal == nullptr)
	{
		wxMessageBox("No one script is opened to save the text.", "Huh?", 5L, this);
		return;
	}

	wxFileDialog openFile(this, _("Open Script File"), "", "", "All Suported Files |*.fsf;*.msf;*.dsf", wxFD_OPEN | wxFD_FILE_MUST_EXIST);	

	if (openFile.ShowModal() == wxID_CANCEL)
		return;	

	SpriteFile* script = new SpriteFile(FileUtil::ReadAllBytes(openFile.GetPath().ToStdString()));

	if (script->str_count != scriptOriginal->str_count)
	{
		wxMessageBox("The text count is not the same.", "Huh?", 5L, this);
		return;
	}		
	
	std::vector<std::string> text = script->GetText();
	Table::InputTable(FileUtil::ReadAllText(romTranslated->GetTablePath()), text);
	textTranslated = text;
	index = 0;
	UpdateText();

	delete script;
}

void cScriptEditor::tScriptTranslatedOnStyleNeeded(wxStyledTextEvent& event)
{
	std::vector<size_t> vec;	
	//event.
	// Get All occurrences of the 'is' in the vector 'vec'
	StringUtil::FindAllOccurances(tScriptTranslated->GetText().ToStdString(), "<PlayerName>", vec);
	StringUtil::FindAllOccurances(tScriptTranslated->GetText().ToStdString(), "<ChildName >", vec);

	tScriptTranslated->ClearDocumentStyle();

	for (size_t pos : vec)
	{			
		tScriptTranslated->StartStyling(pos);
		tScriptTranslated->SetStyling(12, 9);
	}		

	event.Skip();
}

void cScriptEditor::tScriptOriginalOnStyleNeeded(wxStyledTextEvent& event)
{
	std::vector<size_t> vec;

	// Get All occurrences of the 'is' in the vector 'vec'
	StringUtil::FindAllOccurances(tScriptOriginal->GetText().ToStdString(), "<PlayerName>", vec);
	StringUtil::FindAllOccurances(tScriptOriginal->GetText().ToStdString(), "<ChildName >", vec);

	tScriptOriginal->ClearDocumentStyle();

	for (size_t pos : vec)
	{
		tScriptOriginal->StartStyling(pos);
		tScriptOriginal->SetStyling(12, 9);
	}

	event.Skip();
}

void cScriptEditor::tScritpTranslatedOnModified(wxStyledTextEvent& event)
{
	statusBar->SetStatusText("Size: " + std::to_string(tScriptTranslated->GetTextLength()), 0);
	statusBar->SetStatusText("Lines: " + std::to_string(tScriptTranslated->GetLineCount()), 1);

	event.Skip();
}

void cScriptEditor::tScriptTranslatedOnUi(wxStyledTextEvent& event)
{
	statusBar->SetStatusText("Ln: " + std::to_string(tScriptTranslated->GetCurrentLine()), 2);

	int end = tScriptTranslated->GetSelectionEnd();
	int start = tScriptTranslated->GetSelectionStart();

	int size = end - start;	

	wxString s = tScriptTranslated->GetTextRange(start, end);
	statusBar->SetStatusText(wxString("Sel: ") << s.size(), 3);	

	statusBar->SetStatusText("Col: " + std::to_string(tScriptTranslated->GetColumn(tScriptTranslated->GetCurrentPos())), 4);

	if (options_nav_scroll->IsChecked())
		tScriptOriginal->ScrollToLine(tScriptTranslated->GetFirstVisibleLine());

	event.Skip();
}

void cScriptEditor::OnInputKeyDown(wxKeyEvent& event)
{
	int keyCode = event.GetKeyCode();

	if (keyCode == wxKeyCode::WXK_RETURN || keyCode == wxKeyCode::WXK_NUMPAD_ENTER)
	{
		CheckAndGoScript(std::stoi(script_nav_input->GetValue().ToStdString(), nullptr));
	}

	event.Skip();
}

void cScriptEditor::OnPrevScriptClick(wxCommandEvent& event)
{
	CheckAndGoScript(scriptNum - 1);

	event.Skip();
}

void cScriptEditor::OnProxScriptClick(wxCommandEvent& event)
{
	CheckAndGoScript(scriptNum + 1);

	event.Skip();
}

void cScriptEditor::OnPrevTextClick(wxCommandEvent& event)
{
	PrevText();
}

void cScriptEditor::OnProxTextClick(wxCommandEvent& event)
{
	ProxText();
}

void cScriptEditor::OnHorizontalModeCheck(wxCommandEvent& event)
{
	if (options_nav_horizontal->IsChecked())
	{
		SetEditorHorizontal();
	}
	else
	{
		SetEditorVertical();
	}

	event.Skip();
}

void cScriptEditor::OnMenuGetTextFromScriptFile(wxCommandEvent& event)
{
	GetTextFromScriptFile();
	event.Skip();
}

void cScriptEditor::OnScriptNavOffsetMouseDown(wxMouseEvent &event)
{			
	script_nav_offset->SetForegroundColour(wxColour(255, 0, 0));
	script_nav_offset->Refresh();
	
	clip_board->Open();
	clip_board->SetData(new wxTextDataObject(script_nav_offset->GetLabel()));
	clip_board->Close();

	event.Skip();
}

void cScriptEditor::OnScriptNavOffsetMouseUP(wxMouseEvent& event)
{
	script_nav_offset->SetForegroundColour(wxColour(0, 0, 255));
	script_nav_offset->Refresh();
	event.Skip();
}

void cScriptEditor::OnStringNavInputKeyDown(wxKeyEvent& event)
{	
	int keyCode = event.GetKeyCode();

	if (keyCode == wxKeyCode::WXK_RETURN || keyCode == wxKeyCode::WXK_NUMPAD_ENTER)
	{
		std::string input = string_nav_input->GetValue().ToStdString();

		if (input.size() == 0)
		{
			wxMessageBox("The field is empty.", "Huh?", 5L, this);
			return;
		}

		int value = std::stoi(input);
		--value;

		if (value >= 0 && value < textOriginal.size())
		{
			index = value;
			UpdateText();
		}
	}

	event.Skip();
}

void cScriptEditor::OnSaveTextClick(wxCommandEvent& event)
{
	SaveText();
	event.Skip();
}

void cScriptEditor::OnSaveScriptClick(wxCommandEvent& event)
{
	SaveScript();
	event.Skip();
}

void cScriptEditor::OnInsertScriptClick(wxCommandEvent& event)
{
	SaveScript();
	switch (romTranslated->InsertScript(scriptNum, scriptTranslated->data))
	{
	case 0:
		wxMessageBox(_("Script inserted with no changes."), "Yeah!", 5L, this);
		break;
	case 1:
		wxMessageBox(_("Script inserted with empty bytes used."), "Yeah!", 5L, this);
		break;
	case -1:
		wxMessageBox(_("The script is lenght is greater than the old. You have to move to another offset."), "Yeah!", 5L, this);
		break;
	default:
		break;
	}	
}

void cScriptEditor::OnExportScript(wxCommandEvent& event)
{
	ExportScript();
	event.Skip();
}

void cScriptEditor::PrevText()
{
	if (index > 0)
	{
		--index;
		UpdateText();
	}		
}

void cScriptEditor::ProxText()
{
	if (index < textOriginal.size() - 1){
		++index;
		UpdateText();		
	}		
}

void cScriptEditor::SaveText()
{
	textTranslated[index] = tScriptTranslated->GetText().ToStdString();
	ProxText();
}

void cScriptEditor::UpdateText()
{
	if (scriptOriginal == nullptr)
	{
		wxMessageBox("There is no script opened.", "Huh?", 5L, this);
		index = 0;
		return;
	}

	tScriptOriginal->SetText(textOriginal[index]);
	tScriptTranslated->SetText(textTranslated[index]);

	string_nav_index->SetLabel(wxString("") << (index + 1) << "/" << textTranslated.size());	
}

void cScriptEditor::CreateGUIControls()
{
	this->SetBackgroundColour(wxColour(240, 240, 240, 255));

/******************************	

	Menu creation starts

*******************************/
	menuBar = new wxMenuBar();

	//wxSub

	menuScript = new wxMenu();
	menuScript->Append(wxID_SAVE, _("Save\tCtrl-Shift-S"), nullptr, _("Save the current script"));
	menuScript->Append(wxID_OPEN, _("Get Text From Other Script"));
	menuBar->Append(menuScript, _("File"));

	menuString = new wxMenu();
	menuString->Append(ID_MENU_STRING_SAVE, _("Save\tCtrl-S"), nullptr, _("Save the current string"));
	menuString->Append(ID_MENU_STRING_PREV, _("Prev\tAlt-Left"), nullptr, _("Save the current string"));
	menuString->Append(ID_MENU_STRING_PROX, _("Next\tAlt-Right"), nullptr, _("Save the current string"));
	menuBar->Append(menuString, _("String"));

	menuEdit = new wxMenu();
	menuEdit->Append(wxID_ANY, _("Move To"));
	menuBar->Append(menuEdit, _("Edit"));

	menuSearch = new wxMenu();
	menuSearch->Append(wxID_ANY, "Find Text");
	menuSearch->Append(wxID_ANY, "Find Next Text");
	menuSearch->Append(wxID_ANY, "Find Next Script");
	menuBar->Append(menuSearch, "Search");

	menuTools = new wxMenu();
	menuTools->Append(wxID_ANY, "Check All Pointers");
	menuTools->Append(wxID_ANY, "Check All Codes");
	menuTools->Append(wxID_ANY, "Show Previwer");
	menuTools->Append(wxID_ANY, "Show One By One");
	menuBar->Append(menuTools, "Tools");

	menuExport = new wxMenu();
	menuExport->Append(ID_MENU_EXPORT_SCRIPT, "Export\tCtrl-Shift-E");
	menuExport->Append(wxID_ANY, "Export All");
	menuBar->Append(menuExport, "Export");

	menuOptions = new wxMenu();
	menuOptions->Append(wxID_ANY, "Always On Top");
	menuBar->Append(menuOptions, "Options");

	menuBar->Bind(wxEVT_MENU, &cScriptEditor::OnMenuGetTextFromScriptFile, this, wxID_OPEN);
	menuBar->Bind(wxEVT_MENU, &cScriptEditor::OnSaveScriptClick, this, wxID_SAVE);

	menuBar->Bind(wxEVT_MENU, &cScriptEditor::OnSaveTextClick, this, ID_MENU_STRING_SAVE);
	menuBar->Bind(wxEVT_MENU, &cScriptEditor::OnPrevTextClick, this, ID_MENU_STRING_PREV);
	menuBar->Bind(wxEVT_MENU, &cScriptEditor::OnProxTextClick, this, ID_MENU_STRING_PROX);	
	menuBar->Bind(wxEVT_MENU, &cScriptEditor::OnExportScript, this, ID_MENU_EXPORT_SCRIPT);

	SetMenuBar(menuBar);

	statusBar = CreateStatusBar(5);
	//statusBar->SetStatusText("Original Size: 0", 0);
	statusBar->SetStatusText("Size: 0", 0);
	statusBar->SetStatusText("Lines: 1", 1);
	statusBar->SetStatusText("Ln: 0", 2);
	statusBar->SetStatusText("Sel: 0", 3);
	statusBar->SetStatusText("Col: 0", 4);


	//--------------------//
	// Menu creation ends //
	//--------------------//	

	wxSize* button_min_size = new wxSize(40, 26);

	script_nav_input = new wxTextCtrl(this, ID_SCRIPT_NAV_INPUT);
	script_nav_input->Bind(wxEVT_KEY_DOWN, &cScriptEditor::OnInputKeyDown, this);

	script_nav_prev = new wxButton(this, ID_SCRIPT_NAV_PREV, "<<");
	script_nav_prev->Bind(wxEVT_BUTTON, &cScriptEditor::OnPrevScriptClick, this);
	script_nav_prev->SetMinSize(*button_min_size);
	script_nav_prox = new wxButton(this, ID_SCRIPT_NAV_PROX, ">>");
	script_nav_prox->Bind(wxEVT_BUTTON, &cScriptEditor::OnProxScriptClick, this);
	script_nav_prox->SetMinSize(*button_min_size);

	script_nav_insert = new wxButton(this, wxID_ANY, _("Insert"));
	script_nav_insert->Bind(wxEVT_BUTTON, &cScriptEditor::OnInsertScriptClick, this);

	script_nav_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
	script_nav_buttons_sizer->Add(script_nav_prev, 1, wxRIGHT | wxEXPAND, 0);
	script_nav_buttons_sizer->AddSpacer(4);
	script_nav_buttons_sizer->Add(script_nav_prox, 1, wxALL | wxEXPAND, 0);

	script_nav_offset_0x = new wxStaticText(this, wxID_ANY, _("Offset: 0x"));
	script_nav_offset = new wxStaticText(this, wxID_ANY, "00000000");
	script_nav_offset->SetForegroundColour(wxColour(0, 0, 255, 255));
	script_nav_offset->SetCursor(wxCursor(wxCURSOR_HAND));
	script_nav_offset->Bind(wxEVT_LEFT_DOWN, &cScriptEditor::OnScriptNavOffsetMouseDown, this);
	script_nav_offset->Bind(wxEVT_LEFT_UP, &cScriptEditor::OnScriptNavOffsetMouseUP, this);

	script_nav_offset_sizer = new wxBoxSizer(wxHORIZONTAL);
	script_nav_offset_sizer->Add(script_nav_offset_0x, 0, wxALL, 0);
	script_nav_offset_sizer->Add(script_nav_offset, 0, wxALL, 0);
	

	script_nav_box = new wxStaticBox(this, wxID_ANY, "Script");

	script_nav_sizer = new wxStaticBoxSizer(script_nav_box, wxVERTICAL);
	script_nav_sizer->Add(script_nav_input, 0, wxEXPAND, 5);
	script_nav_sizer->AddSpacer(4);
	script_nav_sizer->Add(script_nav_buttons_sizer, 0, wxALL | wxEXPAND, 0);
	script_nav_sizer->AddSpacer(4);
	script_nav_sizer->Add(script_nav_offset_sizer, 0, wxALL | wxEXPAND, 0);
	script_nav_sizer->AddSpacer(4);
	script_nav_sizer->Add(script_nav_insert, 0, wxALL | wxEXPAND, 0);

	string_nav_index_is = new wxStaticText(this, wxID_ANY, _("Index:"));
	string_nav_index = new wxStaticText(this, wxID_ANY, "000/000");
	string_nav_input = new wxTextCtrl(this, wxID_ANY);
	string_nav_input->SetMinSize(wxSize(button_min_size->GetX() - 4, script_nav_input->GetSize().GetY()));
	string_nav_input->Bind(wxEVT_KEY_DOWN, &cScriptEditor::OnStringNavInputKeyDown, this);
	string_nav_code = new wxCheckBox(this, wxID_ANY, _("Code"));
	string_nav_later = new wxCheckBox(this, wxID_ANY, _("Later"));

	string_nav_index_sizer = new wxBoxSizer(wxHORIZONTAL);
	string_nav_index_sizer->Add(string_nav_index_is, 0, wxALL | wxEXPAND, 1);
	string_nav_index_sizer->Add(string_nav_index, 0, wxALL | wxEXPAND, 1);
	string_nav_index_sizer->AddSpacer(4);
	string_nav_index_sizer->Add(string_nav_input, 0, wxALL | wxEXPAND, 1);

	string_nav_box = new wxStaticBox(this, wxID_ANY, "String");
	string_nav_sizer = new wxStaticBoxSizer(string_nav_box, wxVERTICAL);
	//string_nav_sizer->AddSpacer(4);
	string_nav_sizer->Add(string_nav_index_sizer, 0, wxALL | wxEXPAND, 0);
	//string_nav_sizer->AddSpacer(4);
	string_nav_sizer->Add(string_nav_code, 0, wxALL | wxEXPAND, 0);
	//string_nav_sizer->AddSpacer(4);
	string_nav_sizer->Add(string_nav_later, 0, wxALL | wxEXPAND, 0);

	options_nav_export = new wxCheckBox(this, wxID_ANY, _("Always Export"));
	options_nav_scroll = new wxCheckBox(this, wxID_ANY, _("Sync Scroll"));
	options_nav_horizontal = new wxCheckBox(this, wxID_ANY, _("Horizontal Mode"));
	options_nav_horizontal->Bind(wxEVT_CHECKBOX, &cScriptEditor::OnHorizontalModeCheck, this);

	options_nav_box = new wxStaticBox(this, wxID_ANY, "Options");
	options_nav_sizer = new wxStaticBoxSizer(options_nav_box, wxVERTICAL);
	options_nav_sizer->Add(options_nav_export, 0, wxALL | wxEXPAND, 1);
	options_nav_sizer->Add(options_nav_scroll, 0, wxALL | wxEXPAND, 1);
	options_nav_sizer->Add(options_nav_horizontal, 0, wxALL | wxEXPAND, 1);

	panel_left_sizer = new wxBoxSizer(wxVERTICAL);
	panel_left_sizer->Add(script_nav_sizer, 0, wxALL | wxEXPAND, 0);
	panel_left_sizer->AddSpacer(4);
	panel_left_sizer->Add(string_nav_sizer, 0, wxALL | wxEXPAND, 0);
	panel_left_sizer->AddSpacer(4);
	panel_left_sizer->Add(options_nav_sizer, 0, wxALL | wxEXPAND, 0);

	tScriptTranslated = new wxStyledTextCtrl(this, wxID_ANY);
	tScriptTranslated->StyleSetFont(0, wxFont(wxFontInfo(10).FaceName("Courier New")));

	int x, y;
	GetTextExtent(_T("ABCDEabcde"), &x, &y, (int*)0, (int*)0, &wxFont(wxFontInfo(10).FaceName("Courier New")));
	//Gets Width of string
	tScriptTranslated->SetSizeHints(((x / 10) * (34 + 2)), -1);

	tScriptTranslated->SetLexer(wxSTC_LEX_CONTAINER);
	tScriptTranslated->StyleSetForeground(9, wxColor(0, 0, 255));
	tScriptTranslated->Bind(wxEVT_STC_STYLENEEDED, &cScriptEditor::tScriptTranslatedOnStyleNeeded, this);
	tScriptTranslated->Bind(wxEVT_STC_MODIFIED, &cScriptEditor::tScritpTranslatedOnModified, this);
	tScriptTranslated->Bind(wxEVT_STC_UPDATEUI, &cScriptEditor::tScriptTranslatedOnUi, this);
	//tScriptTranslated->STYLESETC
	//tScriptTranslated->SetCodePage(0);

	editor_save_text = new wxButton(this, wxID_ANY, "Save");
	editor_save_text->Bind(wxEVT_BUTTON, &cScriptEditor::OnSaveTextClick, this);
	editor_prev_text = new wxButton(this, wxID_ANY, "<<");
	editor_prev_text->Bind(wxEVT_BUTTON, &cScriptEditor::OnPrevTextClick, this);
	editor_prev_text->SetMinSize(*button_min_size);
	editor_next_text = new wxButton(this, wxID_ANY, ">>");
	editor_next_text->Bind(wxEVT_BUTTON, &cScriptEditor::OnProxTextClick, this);
	editor_next_text->SetMinSize(*button_min_size);

	tScriptOriginal = new wxStyledTextCtrl(this, wxID_ANY);
	tScriptOriginal->SetSizeHints(tScriptTranslated->GetMinSize(), tScriptTranslated->GetMaxSize());
	tScriptOriginal->SetLexer(wxSTC_LEX_CONTAINER);
	tScriptOriginal->StyleSetForeground(9, wxColor(0, 0, 255));
	tScriptOriginal->StyleSetFont(0, wxFont(wxFontInfo(10).FaceName("Courier New")));

	editor_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
	editor_buttons_sizer->Add(editor_save_text, 0, wxALL | wxEXPAND, 0);
	editor_buttons_sizer->AddStretchSpacer(1);
	editor_buttons_sizer->Add(editor_prev_text, 0, wxALL | wxEXPAND, 0);
	editor_buttons_sizer->AddSpacer(4);
	editor_buttons_sizer->Add(editor_next_text, 0, wxALL | wxEXPAND, 0);

	editor_sizer = new wxBoxSizer(wxVERTICAL);
	editor_sizer->Add(tScriptTranslated, 2, wxALL | wxEXPAND, 0);
	editor_sizer->Add(editor_buttons_sizer, 0, wxUP | wxBOTTOM | wxEXPAND, 4);
	editor_sizer->Add(tScriptOriginal, 2, wxALL | wxEXPAND, 0);

	global_sizer = new wxBoxSizer(wxHORIZONTAL);
	global_sizer->Add(panel_left_sizer, 0, wxALL | wxEXPAND, 5);
	global_sizer->Add(editor_sizer, 1, wxALL | wxEXPAND, 5);
	SetSizer(global_sizer);
	global_sizer->Fit(this);
	global_sizer->SetSizeHints(this);
}

void cScriptEditor::SetEditorVertical()
{
	if (editor_buttons_sizer != nullptr)
		editor_buttons_sizer->SetOrientation(wxHORIZONTAL);

	if (editor_sizer != nullptr)
		editor_sizer->SetOrientation(wxVERTICAL);

	Layout();

	global_sizer->Fit(this);
	global_sizer->SetSizeHints(this);
}

void cScriptEditor::SetEditorHorizontal()
{
	if (editor_buttons_sizer != nullptr)
		editor_buttons_sizer->SetOrientation(wxVERTICAL);

	if (editor_sizer != nullptr)
		editor_sizer->SetOrientation(wxHORIZONTAL);

	Layout();

	global_sizer->Fit(this);
	global_sizer->SetSizeHints(this);
}
