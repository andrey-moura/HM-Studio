#include "frame_script_editor.hpp"

wxBEGIN_EVENT_TABLE(cScriptEditor, wxFrame)
//
//wxEVT_STC_MODIFIED(10001, cScriptEditor::tScritpTranslatedOnModified)
//

//EVT_KEY_DOWN(ID_SCRIPT_NAV_INPUT, OnInputKeyDown)

//EVT_STC_

wxEND_EVENT_TABLE()

cScriptEditor::cScriptEditor(id i) : wxFrame(nullptr, wxID_ANY, "Script Editor"), romOriginal(i, false), romTranslated(i, true)
{

	CreateGUIControls();
	SetupRom();

#ifdef Testing
	m_hunspell = new Hunspell("C:/Users/Moonslate/AppData/Roaming/Notepad++/plugins/config/Hunspell/pt_BR.aff", "C:/Users/Moonslate/AppData/Roaming/Notepad++/plugins/config/Hunspell/pt_BR.dic");
	bool test = m_hunspell->spell(std::string("testando"));
	wxMessageBox(wxString() << (test ? "true" : "false"));
#endif // Testing

}

cScriptEditor::~cScriptEditor()
{

}

void cScriptEditor::SetupRom()
{
	switch (romOriginal.Console)
	{
	case console::DS:
		m_lineEnding = "\n";
		m_lineLineEnding = "\\n";
		m_maxLineLenght = 30;
		tScriptTranslated->SetEOLMode(wxEOL_UNIX);
		break;
	case console::GBA:
		m_lineEnding = "\r\n";
		m_lineLineEnding = "\\n";
		m_maxLineLenght = 28;
		tScriptTranslated->SetEOLMode(wxEOL_DOS);
		break;
	default:
		break;
	}

	this->SetTitle(wxString(_("Script Editor - ")) << romOriginal.Name);
}

void cScriptEditor::BackupText()
{
	m_textSave = tScriptTranslated->GetText();
	m_stringBackup = true;
	m_indexBackup = index;
	m_pMenuString_Restore->Enable(true);
	log_text->SetForegroundColour(wxColour(255, 0, 0));
	log_text->SetLabel(wxString("Text ") << m_indexBackup << " not saved. See String>Restore");		
}

void cScriptEditor::RestoreText()
{
	index = m_indexBackup;
	UpdateText();
	tScriptTranslated->SetText(m_textSave);
}

void cScriptEditor::SolveProblem()
{
	std::vector<uint8_t> bytes = FileUtil::ReadAllBytes(romOriginal.GetScriptFullPath(scriptNum));
	std::vector<uint8_t> bytes2 = FileUtil::ReadAllBytes(romTranslated.GetScriptFullPath(scriptNum));

	Script script = Script();
	script.SetData(bytes);

	if (!script.HaveText())
	{
		wxMessageBox(_("This script don't have text."), _("Huh?"), 5L, this);
		return;
	}

	scriptTranslated.SetData(bytes2);
	scriptOriginal.SetData(bytes);
}

void cScriptEditor::OpenScript()
{
	SolveProblem();
	UpdateScript();
}

void cScriptEditor::SaveScript()
{	
	std::vector<std::string> text = textTranslated;
	romTranslated.OutputTextWithVariables(text);
	scriptTranslated.UpdateText(text);
	//FileUtil::WriteAllBytes(romTranslated.GetScriptFullPath(scriptNum), scriptTranslated.data);
}

void cScriptEditor::ExportScript()
{
	std::string text;

	for (int i = 0; i < textTranslated.size(); ++i)
	{
		text.append(textTranslated[i]);
		text.append(std::string("\n---End text ").append(std::to_string(i).append("---\n")));
	}

	FileUtil::WriteAllText(romTranslated.GetScriptExportedFullPath(scriptNum), text);
}

void cScriptEditor::UpdateScript()
{
	index = 0;

	m_indexesString.clear();

	textOriginal = scriptOriginal.GetText();
	textTranslated = scriptTranslated.GetText();

	romTranslated.InputTextWithVariables(textOriginal, textTranslated);

	uint32_t pointer = 0;
	romTranslated.GetOffset(pointer, scriptNum);

	std::string s_pointer = "0000000";
	_itoa(pointer, (char*)s_pointer.c_str(), 16);
	
	while (s_pointer.size() < 9)
		s_pointer = '0' + s_pointer;

	script_nav_offset->SetLabel(s_pointer);

	this->SetTitle(wxString() << "Script Editor - " << romOriginal.Name << ": Script" << scriptNum);

	UpdateText();
}

void cScriptEditor::CheckAndGoScript(int index)
{
	if (index >= 0 && index < romOriginal.Offset.Script_count)
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
							<< romOriginal.Offset.Script_count -1 << ".", "Huh?", 5L, this);
	}
}

void cScriptEditor::GetTextFromScriptFile()
{
	//if (scriptOriginal == nullptr)
	//{
	//	wxMessageBox("No one script is opened to save the text.", "Huh?", 5L, this);
	//	return;
	//}

	wxFileDialog openFile(this, _("Open Script File"), "", "", "All Suported Files |*.fsf;*.msf;*.dsf", wxFD_OPEN | wxFD_FILE_MUST_EXIST);	

	if (openFile.ShowModal() == wxID_CANCEL)
		return;	

	std::vector<uint8_t> bytes = FileUtil::ReadAllBytes(openFile.GetPath().ToStdString());
	Script script = Script();
	script.SetData(bytes);

	if (script.size() != scriptOriginal.size())
	{
		wxMessageBox("The text count is not the same.", "Huh?", 5L, this);
		return;
	}		
	
	std::vector<std::string> text = script.GetText();
	Table::InputTable(FileUtil::ReadAllText(romTranslated.GetTablePath()), text);
	textTranslated = text;
	index = 0;
	UpdateText();
}

void cScriptEditor::FindText()
{
	FrameSearchScript* search_text = new FrameSearchScript();

	int result = search_text->ShowModal();

	if (result == FrameSearchScript::SearchMode::Find)
	{
		m_indexesString.clear();
		m_curIndexString = 0;
		StringUtil::FindAllOccurances(textOriginal, search_text->find, m_indexesString);

		UpdateText();
	}
	else if (result == FrameSearchScript::SearchMode::Replace || result == FrameSearchScript::SearchMode::ReplaceExtended)
	{
		if (result == FrameSearchScript::SearchMode::ReplaceExtended)
		{
			StringUtil::Replace(m_lineLineEnding, m_lineEnding, search_text->find);
			StringUtil::Replace(m_lineLineEnding, m_lineEnding, search_text->replace);
		}

		for (int i = 0; i < textTranslated.size(); ++i)
		{
			m_indexesString.clear();
			m_curIndexString = 0;
			StringUtil::Replace(search_text->find, search_text->replace, textTranslated[i]);
		}

		UpdateText();
	}

	search_text->Destroy();
	delete search_text;
}

void cScriptEditor::tScriptTranslatedOnStyleNeeded(wxStyledTextEvent& event)
{
	int line = tScriptTranslated->GetCurrentLine();
	int start = tScriptTranslated->PositionFromLine(line);
	int end = tScriptTranslated->GetLineEndPosition(line);

	tScriptTranslated->StartStyling(start);
	tScriptTranslated->SetStyling(tScriptTranslated->LineLength(line), 0);

#ifdef Testing
	double now = (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count() * 0.001);

	double elapsed = now - m_lastTyped;

	if (elapsed > 500 && !m_endTyping)
	{
		m_endTyping = true;	

		DoSpelling(tScriptTranslated, tScriptTranslated->WordStartPosition(m_typedMinPos, false), tScriptTranslated->WordEndPosition(m_typedMaxPos, false));
	}
#endif // Testing



	FindAndHighlightAllVars(tScriptTranslated, start, end);

	VerifyCurLineLenght(tScriptTranslated);

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
		tScriptOriginal->SetStyling(12, VarStyle);
	}	

	event.Skip();
}

void cScriptEditor::tScritpTranslatedOnModified(wxStyledTextEvent& event)
{
	/*statusBar->SetStatusText(wxString("Size: ") << tScriptTranslated->GetTextLength(), 0);
	statusBar->SetStatusText(wxString("Lines: ") << tScriptTranslated->GetLineCount(), 1);*/
	
	event.Skip();
}

void cScriptEditor::tScriptTranslatedOnUi(wxStyledTextEvent& event)
{		
//	if (options_nav_scroll->IsChecked())
		//tScriptOriginal->ScrollToLine(tScriptTranslated->GetFirstVisibleLine());*/

	//UpdateStatusText(tScriptTranslated);
	CallAfter(&cScriptEditor::UpdateStatusText, tScriptTranslated);

	event.Skip();	
}

#ifdef Testing
void cScriptEditor::tScriptTranslatedCharAdded(wxStyledTextEvent& event)
{
	m_lastTyped = (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count() * 0.001);

	size_t curPos = tScriptTranslated->GetCurrentPos();

	if (m_endTyping)
	{
		m_typedMinPos = curPos;
		m_typedMaxPos = curPos;
	}

	m_endTyping = false;

	if (m_typedMinPos > curPos)
		m_typedMinPos = curPos;

	if (m_typedMaxPos < curPos)
		m_typedMaxPos = curPos;

	CallAfter(&cScriptEditor::UpdateStatusText, tScriptTranslated);

	event.Skip();
}
#endif // Testing

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
	
	clip_board.Open();
	clip_board.SetData(new wxTextDataObject(script_nav_offset->GetLabel()));
	clip_board.Close();

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
	/*switch (romTranslated.InsertScript(scriptNum, scriptTranslated.data))
	{
	case 0:
		wxMessageBox(_("Script inserted with no changes."), "Yeah!", 5L, this);
		break;
	case 1:
		wxMessageBox(_("Script inserted with empty bytes used."), "Yeah!", 5L, this);
		break;
	case -1:
		wxMessageBox(_("The script is lenght is greater than the old. You have to move to another offset."), "...", 5L, this);
		break;
	default:
		break;
	}	*/
}

void cScriptEditor::OnExportScript(wxCommandEvent& event)
{
	ExportScript();
	event.Skip();
}

void cScriptEditor::EVT_MENU_FindText(wxCommandEvent& event)
{
	FindText();
}

void cScriptEditor::EVT_MENU_FindNextText(wxCommandEvent& event)
{
	if (m_curIndexString == m_indexesString.size())
	{
		index = 0;
		UpdateText();
	}
	else {
		index = m_indexesString[m_curIndexString];
		m_curIndexString++;
		UpdateText();
	}
}

void cScriptEditor::EVT_MENU_RestoreString(wxCommandEvent& event)
{
	RestoreText();
	event.Skip();
}

void cScriptEditor::OnClosing(wxCloseEvent& event)
{
	event.Skip();
}

inline void cScriptEditor::VerifyLineLenght(wxStyledTextCtrl* stc, int line)
{	
	int lenght = stc->GetLineLength(line);

	if (lenght > m_maxLineLenght) {
		if (!(stc->MarkerGet(line) & LINEERROR_MASK)) //Verify if the line already has that marker
			stc->MarkerAdd(line, 0);
	}
	else {
		if (stc->MarkerGet(line) & LINEERROR_MASK)
			stc->MarkerDelete(line, 0);
	}
}

inline void cScriptEditor::VerifyCurLineLenght(wxStyledTextCtrl* stc)
{
	VerifyLineLenght(stc, stc->GetCurrentLine());
}

inline void cScriptEditor::VerifyAllLinesLenght(wxStyledTextCtrl* stc)
{
	for (int i = 0; i < stc->GetLineCount(); ++i)
	{
		VerifyLineLenght(stc, i);
	}
}

inline void cScriptEditor::FindAndHighlightAllVars(wxStyledTextCtrl* stc, const size_t start, const size_t end)
{
	std::vector<size_t> pos;
	STCFindAll(stc, start, end, "<PlayerName>", pos);
	STCFindAll(stc, start, end, "<ChildName >", pos);

	HighlightAll(stc, pos, 12, VarStyle);

	pos.clear();

	STCFindAll(stc, start, end, "|", pos);
	STCFindAll(stc, start, end, "²", pos);

	HighlightAll(stc, pos, 1, SimbolStyle);
}

inline void cScriptEditor::UpdateStyle(wxStyledTextCtrl* stc)
{
	stc->ClearDocumentStyle();
	stc->MarkerDeleteAll(0);
	VerifyAllLinesLenght(stc);
	FindAndHighlightAllVars(stc, 0, stc->GetTextLength());
}

#ifdef Testing
void cScriptEditor::DoSpelling(wxStyledTextCtrl* stc, const size_t start, const size_t end)
{
	std::string s = stc->GetTextRange(start, end).ToStdString();

	size_t npos = std::string::npos;

	tScriptTranslated->IndicatorClearRange(start, end - start);

	for (size_t first = s.find_first_of(letters, 0); first != npos; first = s.find_first_of(letters, first))
	{
		size_t last = s.find_first_not_of(letters, first);

		std::string word = s.substr(first, last != npos ? last - first : s.size() - first);

		bool isGood = m_hunspell->spell(s);

		if (!isGood)
			tScriptTranslated->IndicatorFillRange(start + first, last != npos ? last - first : s.size() - first);

		first = last;
	}
}
#endif // Testing

void cScriptEditor::SetupStyles(wxStyledTextCtrl* stc)
{
	stc->SetLexer(wxSTC_LEX_CONTAINER);

	stc->StyleSetBackground(NormalStyle, Studio::GetControlBackgroundColor());
	stc->StyleSetBackground(VarStyle, Studio::GetControlBackgroundColor());
	stc->StyleSetBackground(SimbolStyle, Studio::GetControlBackgroundColor());
	stc->StyleSetBackground(32, Studio::GetControlBackgroundColor()); //The editor background

	stc->StyleSetFont(NormalStyle, Studio::GetDefaultFont());
	stc->StyleSetFont(VarStyle, Studio::GetDefaultFont());
	stc->StyleSetFont(SimbolStyle, Studio::GetDefaultFont());

	stc->StyleSetForeground(NormalStyle, Studio::GetFontColour());
	stc->StyleSetForeground(VarStyle, wxColour(86, 156, 214));
	stc->StyleSetForeground(SimbolStyle, wxColour(78, 201, 176));

	stc->MarkerDefineBitmap(0, m_DeleteIcon);

	stc->SetMarginWidth(0, 32);	
	stc->SetMarginType(0, wxSTC_MARGIN_NUMBER);		
	stc->SetCaretForeground(Studio::GetFontColour());

	stc->IndicatorSetStyle(0, wxSTC_INDIC_SQUIGGLE);

	/*stc->SetMarginBackground(0, wxColor(0xff, 0, 0));
	b 
	wxColour color = stc->GetMarginBackground(0);
	wxString text = wxString("r = ") << (int)color.Red() << " g = " << (int)color.Green() << " b = " << (int)color.Blue();*/

	//stc->margin

	//wxMessageBox(text);
}

void cScriptEditor::UpdateStatusText(wxStyledTextCtrl* stc)
{
	statusBar->SetStatusText(wxString("Size: ") << (m_endTyping ? "True" : "False"), 1);

	statusBar->SetStatusText(wxString("Ln: ") << stc->GetCurrentLine() + 1, 2);

	int selSize = (stc->GetSelectionEnd() - stc->GetSelectionStart());

	if (selSize > 0)
		selSize = stc->GetSelectedText().size();

	statusBar->SetStatusText(wxString("Sel: ") << selSize, 3);

	statusBar->SetStatusText(wxString("Col: ") << tScriptTranslated->GetColumn(tScriptTranslated->GetCurrentPos()), 4);
}

inline void cScriptEditor::STCFindAll(wxStyledTextCtrl* stc, const size_t start, const size_t end, const std::string& toFind, std::vector<size_t>& output)
{
	size_t pos = stc->FindText(start, end, toFind);

	while (pos != -1)
	{
		output.push_back(pos);
		pos = stc->FindText(pos + 12, end, toFind);
	}
}

inline void cScriptEditor::HighlightAll(wxStyledTextCtrl* stc, std::vector<size_t>& pos, const size_t size, const int style)
{
	for (size_t start : pos)
	{
		stc->StartStyling(start);
		stc->SetStyling(size, style);
	}
}

void cScriptEditor::PrevText()
{
	if (index > 0)
	{
		if (!m_stringSaved && m_stringChange)
			BackupText();
		--index;
		UpdateText();		
	}		
}

void cScriptEditor::ProxText()
{
	if (index < textOriginal.size() - 1){

		/*if (m_stringChange && !m_stringSaved)
		{
			SetStatusText(_("String not saved. Ctrl-Alt-Z to reload. Ctrl+ESC to dimiss."), 0);
		}*/

		if (!m_stringSaved && m_stringChange)
			BackupText();
		++index;
		UpdateText();		
	}		
}

void cScriptEditor::SaveText()
{
	m_stringSaved = true;
	m_pMenuString_Restore->Enable(false);
	log_text->SetLabel("");
	m_textSave.clear();

	textTranslated[index] = tScriptTranslated->GetText().ToStdString();
	ProxText();
}

void cScriptEditor::UpdateText()
{
	//if (scriptOriginal == nullptr)
	//{
	//	wxMessageBox("There is no script opened.", "Huh?", 5L, this);
	//	index = 0;
	//	return;
	//}	

	tScriptOriginal->SetText(textOriginal[index]);

	tScriptTranslated->SetText(textTranslated[index]);
	UpdateStyle(tScriptTranslated);
		
	m_stringChange = false;
	m_stringSaved = false;

	string_nav_index->SetLabel(wxString("") << (index + 1) << "/" << textTranslated.size());	
}

void cScriptEditor::CreateGUIControls()
{
//images

	uint8_t* delete_icon_data = new uint8_t[796]{ 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10,
											 0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x02, 0xe3, 0x49, 0x44, 0x41,
											 0x54, 0x78, 0xda, 0x6d, 0x53, 0x4d, 0x48, 0x1b, 0x41, 0x14, 0x7e, 0x6f, 0x13, 0xb3, 0x49, 0xc4, 0x5a, 0x93, 0xc6, 0xe4,
											 0x20, 0x14, 0x44, 0x14, 0x2f, 0x2a, 0x11, 0x41, 0x4b, 0x31, 0x46, 0xeb, 0x2f, 0x52, 0x3c, 0x54, 0x34, 0x0a, 0x62, 0x25,
											 0x5a, 0x0f, 0x36, 0xe2, 0xa9, 0xf4, 0x56, 0xf0, 0xd6, 0x8b, 0x28, 0x16, 0x4d, 0x14, 0x51, 0x0f, 0xde, 0x14, 0xb4, 0x85,
											 0xe8, 0x49, 0x53, 0x94, 0x78, 0x50, 0x91, 0x9c, 0x44, 0x3c, 0x28, 0xb5, 0x51, 0x34, 0x8d, 0xb1, 0x34, 0x3f, 0x66, 0xb3,
											 0xbb, 0x9d, 0x99, 0xac, 0x52, 0xa1, 0x0f, 0x86, 0x9d, 0x9d, 0x79, 0xdf, 0x37, 0xdf, 0xfb, 0x43, 0x50, 0x8c, 0x23, 0xcb,
											 0xa2, 0xd5, 0xa2, 0xab, 0xb0, 0xd0, 0xfa, 0x26, 0x2f, 0x6f, 0xc0, 0xc8, 0xf3, 0x76, 0x72, 0xf4, 0x5c, 0xb9, 0x3e, 0xbb,
											 0x49, 0x26, 0x37, 0x97, 0xcf, 0xcf, 0x3d, 0xe3, 0xc7, 0xc7, 0x07, 0xc1, 0x78, 0x5c, 0x96, 0x94, 0x0b, 0xbc, 0x07, 0xbf,
											 0x32, 0x9b, 0xf5, 0x5f, 0xac, 0xd6, 0xf1, 0xa7, 0x1a, 0x4d, 0x2f, 0x97, 0x3e, 0x42, 0x62, 0xcc, 0x49, 0x96, 0x65, 0xf6,
											 0x21, 0x20, 0xe9, 0x77, 0x32, 0x39, 0xef, 0x3a, 0x3c, 0x1c, 0xf6, 0x5e, 0x5c, 0xc4, 0x28, 0x09, 0x52, 0xcf, 0xba, 0xdc,
											 0x5c, 0xfd, 0x7c, 0x45, 0x85, 0x57, 0xa3, 0x56, 0xbf, 0x44, 0x0a, 0x54, 0xc8, 0x15, 0x20, 0x28, 0x44, 0xec, 0x8f, 0x2c,
											 0x59, 0x10, 0xc5, 0xed, 0xfe, 0xbd, 0xbd, 0x66, 0xef, 0xe5, 0x65, 0x0c, 0xcd, 0x3c, 0x8f, 0x5b, 0xd5, 0xd5, 0x1e, 0x83,
											 0x56, 0xdb, 0xc7, 0x71, 0x1c, 0x6a, 0x5a, 0x5a, 0x90, 0xba, 0x26, 0xbd, 0x5e, 0xf2, 0x9e, 0x22, 0x54, 0xad, 0x06, 0xbe,
											 0xad, 0x0d, 0xa4, 0x50, 0x08, 0x04, 0x9f, 0x4f, 0x96, 0x08, 0xf3, 0x6d, 0x32, 0x39, 0x67, 0xf7, 0xf9, 0x06, 0xf0, 0x53,
											 0x71, 0x71, 0xf9, 0xbb, 0xfc, 0xfc, 0x5d, 0x0e, 0x51, 0xc5, 0xb7, 0xb6, 0xa2, 0xae, 0xa7, 0x87, 0x69, 0xb8, 0x5b, 0x5d,
											 0x85, 0xc4, 0xd2, 0x12, 0x09, 0x86, 0x03, 0xdd, 0xe0, 0x20, 0x68, 0x6c, 0x36, 0x12, 0x83, 0x04, 0xb1, 0x89, 0x09, 0x10,
											 0x76, 0x76, 0x28, 0x89, 0xb8, 0x70, 0x7a, 0x5a, 0x89, 0x81, 0xda, 0x5a, 0xb7, 0x89, 0xe7, 0x9d, 0x34, 0x60, 0x6c, 0x6a,
											 0xc2, 0x6c, 0xa7, 0x93, 0xc4, 0xc5, 0x31, 0xc1, 0x77, 0x6b, 0x6b, 0x80, 0xd9, 0xd9, 0xa0, 0xa9, 0xa9, 0x61, 0x42, 0xa4,
											 0x54, 0x0a, 0x6e, 0xc7, 0xc6, 0x00, 0xfc, 0x7e, 0x99, 0x5a, 0x44, 0x10, 0x66, 0x31, 0xd8, 0xd8, 0x78, 0x4c, 0xee, 0x0a,
											 0x58, 0xa0, 0x04, 0x88, 0xed, 0xed, 0x90, 0xd3, 0xd1, 0x91, 0x26, 0xb9, 0xcf, 0x06, 0x21, 0x93, 0x44, 0x11, 0xc2, 0x33,
											 0x33, 0xc0, 0x6d, 0x6c, 0xa4, 0x93, 0x9a, 0x4e, 0xd0, 0x09, 0xfe, 0x6c, 0x68, 0xb8, 0x23, 0x9b, 0x0c, 0x56, 0x11, 0x0a,
											 0x20, 0x40, 0xce, 0xe1, 0x00, 0x03, 0x25, 0x41, 0x64, 0x7e, 0x54, 0xfa, 0x2f, 0xb7, 0x1b, 0x80, 0x80, 0x59, 0x5e, 0x94,
											 0xaa, 0x90, 0x25, 0xe0, 0x0f, 0x4a, 0x20, 0xcb, 0x8c, 0x80, 0xbd, 0x97, 0x91, 0x01, 0x59, 0x43, 0x43, 0xa0, 0xad, 0xab,
											 0x83, 0x07, 0x23, 0x80, 0xd8, 0xca, 0x0a, 0x44, 0x17, 0x16, 0x18, 0x81, 0xac, 0x9c, 0x12, 0x84, 0x80, 0x67, 0x34, 0x04,
											 0x51, 0x2c, 0xa0, 0xe5, 0x03, 0x95, 0x0a, 0x9e, 0xb8, 0x5c, 0xa0, 0x53, 0xc0, 0x12, 0x71, 0xa6, 0x2a, 0x58, 0x19, 0x09,
											 0x49, 0x74, 0x79, 0x19, 0xfe, 0x2c, 0x2e, 0xde, 0x93, 0xc8, 0xc4, 0xff, 0x04, 0x77, 0xeb, 0xeb, 0xdd, 0x96, 0x54, 0xca,
											 0x49, 0x62, 0xc6, 0x4c, 0x87, 0x03, 0xb3, 0xba, 0xbb, 0x99, 0x16, 0x1a, 0xf3, 0xf5, 0xf4, 0x34, 0x70, 0x39, 0x39, 0xf0,
											 0xac, 0xb3, 0x33, 0x9d, 0x13, 0x02, 0x8c, 0x4c, 0x4e, 0x42, 0x62, 0x7d, 0x9d, 0x25, 0x31, 0xac, 0xd1, 0xcc, 0xe2, 0x87,
											 0x92, 0x92, 0xf2, 0x7e, 0xa3, 0x71, 0x97, 0x5c, 0xab, 0xd4, 0x76, 0x3b, 0x9a, 0x46, 0x46, 0x18, 0xc1, 0xd5, 0xd4, 0x14,
											 0x48, 0xb4, 0x17, 0x68, 0x1b, 0x74, 0x75, 0x81, 0x89, 0xe4, 0x45, 0x12, 0x04, 0xb8, 0x1e, 0x1d, 0x05, 0x71, 0x7f, 0x5f,
											 0x96, 0x10, 0xc5, 0xc5, 0x48, 0xa4, 0x12, 0x8d, 0xa4, 0xff, 0xbf, 0xd9, 0x6c, 0x1e, 0x53, 0x22, 0xd1, 0x47, 0x55, 0x60,
											 0x55, 0x15, 0x6b, 0x24, 0xd9, 0xef, 0x67, 0xb2, 0x69, 0x5e, 0x64, 0x5a, 0x1d, 0xbb, 0x1d, 0xe4, 0x70, 0x18, 0xf0, 0xf0,
											 0x90, 0x35, 0xd2, 0x8d, 0x4e, 0x37, 0xd7, 0x4a, 0x1b, 0x89, 0x3a, 0xbc, 0xb0, 0x58, 0xf4, 0xd3, 0x65, 0x65, 0x5e, 0x3e,
											 0x1a, 0x7d, 0xd4, 0xca, 0xf0, 0xd8, 0x1e, 0x5a, 0x39, 0x99, 0x99, 0xb9, 0xfd, 0x3e, 0x10, 0x68, 0xde, 0x0a, 0x06, 0x63,
											 0x0f, 0xc3, 0x54, 0x45, 0x48, 0x3e, 0x97, 0x96, 0x8e, 0x1b, 0xe2, 0xf1, 0x5e, 0x95, 0x24, 0x71, 0xff, 0x23, 0x10, 0x39,
											 0x4e, 0x8a, 0xe8, 0xf5, 0xf3, 0x1f, 0x03, 0x81, 0xe1, 0xef, 0x04, 0x2c, 0xfd, 0xeb, 0x44, 0x37, 0x06, 0x32, 0x17, 0x6f,
											 0x8b, 0x8a, 0xac, 0xaf, 0xcd, 0xe6, 0x81, 0x5c, 0x44, 0x3b, 0xa6, 0x52, 0x6c, 0x9c, 0x65, 0xb5, 0xfa, 0x2c, 0x04, 0xb0,
											 0xf9, 0xf5, 0xea, 0xca, 0x33, 0x77, 0x74, 0x74, 0x10, 0x4a, 0x24, 0x94, 0x31, 0x03, 0xf8, 0x0b, 0x74, 0x9a, 0x4e, 0xf3,
											 0x6a, 0xfb, 0x10, 0x9c, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82 };
	
	m_DeleteIcon = wxBitmap::NewFromPNGData(delete_icon_data, 796);

	delete[] delete_icon_data;

	this->SetBackgroundColour(Studio::GetFrameColour());
	this->SetForegroundColour(Studio::GetFontColour());

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
	menuString->Append(ID_MENU_STRING_PREV, _("Prev\tAlt-Left"), nullptr, _("Loads the next string"));
	menuString->Append(ID_MENU_STRING_PROX, _("Next\tAlt-Right"), nullptr, _("Loads the previous string"));
	m_pMenuString_Restore = menuString->Append(ID_MENU_STRING_RESTORE, _("Restore\tAlt-Z"), nullptr, _("Restore a text not saved"));
	menuBar->Append(menuString, _("String"));

	m_pMenuString_Restore->Enable(false);

	menuEdit = new wxMenu();
	menuEdit->Append(wxID_ANY, _("Move To"));
	menuBar->Append(menuEdit, _("Edit"));

	menuSearch = new wxMenu();
	menuSearch->Append(wxID_FIND, "Find Text\tCtrl-F");
	menuSearch->Append(ID_MENU_FIND_NEXT, "Find Next Text\tF3");
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
	menuBar->Bind(wxEVT_MENU, &cScriptEditor::EVT_MENU_FindText, this, wxID_FIND);
	menuBar->Bind(wxEVT_MENU, &cScriptEditor::EVT_MENU_FindNextText, this, ID_MENU_FIND_NEXT);
	menuBar->Bind(wxEVT_MENU, &cScriptEditor::EVT_MENU_RestoreString, this, ID_MENU_STRING_RESTORE);

	SetMenuBar(menuBar);

	statusBar = CreateStatusBar(5);
	//statusBar->SetStatusText("Original Size: 0", 0);
	statusBar->SetStatusText("Size: 0", 0);
	statusBar->SetStatusText("Lines: 1", 1);
	statusBar->SetStatusText("Ln: 0", 2);
	statusBar->SetStatusText("Sel: 0", 3);
	statusBar->SetStatusText("Col: 0", 4);

	int widths[5] = { -5, -1, -1, -1, -1};

	statusBar->SetStatusWidths(statusBar->GetFieldsCount(), widths);

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
	SetupStyles(tScriptTranslated);

	tScriptOriginal = new wxStyledTextCtrl(this, wxID_ANY);
	SetupStyles(tScriptOriginal);
	
	tScriptTranslated->Bind(wxEVT_STC_STYLENEEDED, &cScriptEditor::tScriptTranslatedOnStyleNeeded, this);
	tScriptTranslated->Bind(wxEVT_STC_CHANGE, &cScriptEditor::tScritpTranslatedOnModified, this);
	tScriptTranslated->Bind(wxEVT_STC_UPDATEUI, &cScriptEditor::tScriptTranslatedOnUi, this);
	
#ifdef Testing
	tScriptTranslated->Bind(wxEVT_STC_CHARADDED, &cScriptEditor::tScriptTranslatedCharAdded, this);
#endif // Testing

	editor_save_text = new wxButton(this, wxID_ANY, "Save");
	editor_save_text->Bind(wxEVT_BUTTON, &cScriptEditor::OnSaveTextClick, this);
	editor_prev_text = new wxButton(this, wxID_ANY, "<<");
	editor_prev_text->Bind(wxEVT_BUTTON, &cScriptEditor::OnPrevTextClick, this);
	editor_prev_text->SetMinSize(*button_min_size);
	editor_next_text = new wxButton(this, wxID_ANY, ">>");
	editor_next_text->Bind(wxEVT_BUTTON, &cScriptEditor::OnProxTextClick, this);
	editor_next_text->SetMinSize(*button_min_size);

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

	horizontal_sizer = new wxBoxSizer(wxHORIZONTAL);
	horizontal_sizer->Add(panel_left_sizer, 0, wxALL | wxEXPAND, 5);
	horizontal_sizer->Add(editor_sizer, 1, wxALL | wxEXPAND, 5);

	log_text = new wxStaticText(this, wxID_ANY, "");

	global_sizer = new wxBoxSizer(wxVERTICAL);
	global_sizer->Add(horizontal_sizer, 1, wxALL | wxEXPAND, 0);
	global_sizer->Add(log_text, 0, wxALL, 4);

	this->Bind(wxEVT_CLOSE_WINDOW, &cScriptEditor::OnClosing, this);

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
