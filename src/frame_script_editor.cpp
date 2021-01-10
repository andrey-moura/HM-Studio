#include "frame_script_editor.hpp"

#include <wx/popupwin.h>
#include <wx/statline.h>

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
	SetTitle("Script Editor");

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
	UpdateFile();
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

void ScriptEditorFrame::UpdateFile()
{
	SetTitle("Script " + std::to_string(m_pEditor->GetNumber()));
		
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

void ScriptEditorFrame::OnSaveString()
{
	if (tScriptTranslated->GetModify())
	{				
		m_pEditor->SaveText(tScriptTranslated->GetText().ToStdString(wxCSConv(wxFONTENCODING_CP1252)));
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

struct ScriptFunction
{
	ScriptFunction(const char* format, bool returns )
		: format(format), returns(returns)
		{
			for(int i = 0; i < strlen(format); ++i)			
				if (format[i] == '%')
					++arguments;
		}

	const char* format;	

	bool returns = false;
	int arguments = 0;
};

#include <map>

std::map<uint32_t, ScriptFunction> functions;
std::map<uint16_t, std::string> sfx;

struct StackVariable
{
	std::string name;
	int32_t value;
	bool constant;

	StackVariable(const std::string& name)
		: value(0), name(name), constant(false)
	{

	}

	StackVariable(const char* name)
		: value(0), name(name), constant(false)
	{

	}

	StackVariable(int32_t value)
		: value(value), constant(true)
	{

	}

	std::string to_string()
	{
		return constant ? std::to_string(value) : name;
	}
};

using Stack = std::vector<StackVariable>;

void ScriptEditorFrame::UpdateText()
{
	ScriptEditor* editor = (ScriptEditor*)m_pEditor;

	if(editor->GetScript().HaveText())
	{
		std::string std_translated = ((ScriptEditor*)m_pEditor)->GetCurTranslated();
		wxString translated(std_translated.c_str(), wxCSConv(wxFONTENCODING_CP1252), std_translated.size());

		std::string std_original = ((ScriptEditor*)m_pEditor)->GetCurOriginal();
		wxString original(std_original.c_str(), wxCSConv(wxFONTENCODING_CP1252), std_original.size());

		tScriptOriginal->SetText(original);
		tScriptTranslated->SetText(translated);

		m_pStatusBar->SetStatusText(wxString("Index: ") << std::to_string(m_pEditor->GetIndex() + 1) << "/" << std::to_string(m_pEditor->GetCount()));
			
		tScriptTranslated->SetModified(false);
	}	

	m_pSrcCodeOutput->SetText(editor->GetScript().GetSrcCode());

	if(!functions.size())
	{
		functions.insert({0x06,  { "get_actor_direction(%x)", true} } );
		functions.insert({0x11,  { "playanim 0x%x, 0x%x, 0x%x", false} });
		functions.insert({0x1b,  { "playsfx 0x%x, 0x%x", false} });
		functions.insert({0x1f,  { "msgbox", false} });
		functions.insert({0x20,  { "msgbox1", false} });
		functions.insert({0x21,  { "closemsg", false} });
		functions.insert({0x22,  { "msg 0x%x", false} });
		functions.insert({0x23,  { "msg_slow 0x%x", false} });
		functions.insert({0x22,  { "msg_fast 0x%x", false} });
		functions.insert({0x28,  { "choose_text(0x%x, 0x%x)", false} });
		functions.insert({0x29,  { "choose_text(0x%x, 0x%x, 0x%x)", false} });
		functions.insert({0x3e,  { "fun_3e(0x%x)", true } });
		functions.insert({0x106, { "fun_106()", false } });

		std::string names;

		for(auto& fun : functions)
		{
			std::string_view name = fun.second.format;

			size_t pos = name.find(' ');

			if(pos == std::string::npos)
			{
				pos = name.find(')');
			}

			names += name.substr(0, pos);
			names += " ";
		}

		m_pDecompiler->SetKeyWords(1, names);
	}

	if(!sfx.size())
	{
		sfx.insert({0x00A1, "CowMoo"});
	}

	std::string disasm;	

	const Script& script = editor->GetScript();

	const unsigned char* start = script.GetData() + 0x18;
	const unsigned char* end = start + *(uint32_t*)(script.GetData() + 0x14);
	const unsigned char* pos = start;
	
	Stack stack;
	std::vector<uint32_t> labels;

	char buffer[100];	
	
	while (pos < end)
	{
		//ToDo: use for loop
		uint32_t address = pos - start;
		if(std::find(labels.begin(), labels.end(), address) != labels.end())
		{
			disasm += "LAB_";
			disasm += Moon::BitConverter::ToHexString(address);
			disasm += ":\n";
		}

		switch (*pos)
		{
		case 0x12: //cmp			
			disasm += "compare ";
			disasm += stack[stack.size()-2].to_string();
			disasm += ", ";
			disasm += stack[stack.size()-1].to_string();
			disasm += "\n";
			pos++;			
		break;
		case 0x17: //push
			pos++;
			stack.push_back(*(int32_t*)pos);
			pos+=4;
			break;
		case 0x18:
			pos++;
			disasm += "jump LAB_";
			disasm += Moon::BitConverter::ToHexString(*(uint32_t*)pos);
			disasm += "\n";				
			labels.push_back(*(uint32_t*)pos);
			std::sort(labels.begin(), labels.end());
			pos += 4;
		break;
		case 0x1B: //beq
			disasm += "if equal jump LAB_";
			++pos;			
			disasm += Moon::BitConverter::ToHexString(*(uint32_t*)pos);
			disasm += "\n";
			labels.push_back(*(uint32_t*)pos);
			std::sort(labels.begin(), labels.end());
			pos += 4;
		break;
		case 0x21: // call
			{
				memset(buffer, 0, sizeof(buffer));
				pos++;

				uint32_t id = *(uint32_t*)pos;

				auto it = functions.find(id);

				if(it == functions.end())
				{
					sprintf(buffer, "fun_%x();\n", id);
					disasm += buffer;
				} else
				{
					ScriptFunction& function = functions.at(id);

					switch(function.arguments)
					{
						case 0:							
							strcpy(buffer, function.format);
						break;
						case 1:
							sprintf(buffer, function.format, stack.back().value);
							stack.pop_back();							
						break;
						case 2:
							sprintf(buffer, function.format, stack[stack.size()-2].value, stack[stack.size()-1].value);
							stack.pop_back();
							stack.pop_back();							
						break;
						case 3:
							sprintf(buffer, function.format, stack[stack.size()-3].value, stack[stack.size()-2].value, stack[stack.size()-1].value);
							stack.pop_back();
							stack.pop_back();
							stack.pop_back();							
						break;
					}

					if(function.returns)
					{
						stack.push_back("LASTRESULT");
					}
					
					disasm += buffer;
					disasm += "\n";					
				}
								
				pos+=4;
			}
		break;
		case 0x22: //push16
			pos++;			
			stack.push_back(*(int16_t*)pos);
			pos+=2;
			break;
		case 0x23: //push8
			pos++;
			stack.push_back(*(int8_t*)pos);
			pos++;
		break;
		default:
			pos++;
			break;
		}		
	}

	//Testing with MFoMT 350

	m_pDecompiler->SetText(disasm);
}

void ScriptEditorFrame::CreateGUIControls()
{
	wxMenu* menuEdit = new wxMenu();
	menuEdit->Append(wxID_ANY, "Move To");
	menuEdit->Bind(wxEVT_MENU, &ScriptEditorFrame::OnConvertEOLClick, this, menuEdit->Append(wxNewId(), "Convert EOL")->GetId());
	
	CreateMyMenuBar();
	CreateFileMenu();
	CreateStringMenu();
	m_frameMenuBar->Append(menuEdit, L"Edit");
	CreateSearchMenu();
	CreateToolsMenu(false, true, true, true);
	m_pMenuTools->Bind(wxEVT_MENU, &ScriptEditorFrame::OnCheckCodeClick, this, m_pMenuTools->Append(wxNewId(), "Code Checker", "Checking Tool")->GetId());
	CreateViewMenu();
	m_pMenuView->Bind(wxEVT_MENU, &ScriptEditorFrame::OnHorizontalModeClick, this, m_pMenuView->Append(wxID_ANY, L"Horizontal Mode")->GetId());	

	//int widths[5] = { -5, -1, -1, -1, -1 };

	//m_pStatusBar->SetStatusWidths(m_pStatusBar->GetFieldsCount(), widths);

//-----------------------------------------------------------------//

	CreateMyToolBar();

	tScriptTranslated = new STC(this, wxID_ANY);
	tScriptOriginal = new STC(this, wxID_ANY);
	m_pSrcCodeOutput = new STC(this, wxID_ANY);

	m_pSrcCodeOutput->SetUseSpellChecker(false);
	tScriptOriginal->SetUseSpellChecker(false);
	CreateButtonsSizer();

	for (size_t i = 0; i < Script::GetScriptOperationsCount(); ++i)
	{
		m_pSrcCodeOutput->AddVar(Script::GetScriptOperations()[i]);
	}

	m_pSrcCodeOutput->StyleSetFont(5, m_pSrcCodeOutput->GetFont());
	m_pSrcCodeOutput->StyleSetForeground(5, wxColour(255, 128, 0));
	m_pSrcCodeOutput->SetMarginType(0, wxSTC_MARGIN_TEXT);
	m_pSrcCodeOutput->SetMarginCount(1);
	m_pSrcCodeOutput->SetMarginWidth(0, 72);
	m_pSrcCodeOutput->Bind(wxEVT_STC_STYLENEEDED, &ScriptEditorFrame::OnCodeStyleNeeded, this);

	m_pDecompiler = new STC(this, wxID_ANY);
	m_pDecompiler->SetLexer(wxSTC_LEX_CPP);  

	m_pDecompiler->StyleSetForeground(wxSTC_C_COMMENT, wxColour(80, 240, 60));
	m_pDecompiler->StyleSetForeground(wxSTC_C_WORD, wxColour(0, 0, 255));
	m_pDecompiler->StyleSetForeground(wxSTC_C_WORD2, wxColour(121, 94, 38));
	m_pDecompiler->StyleSetForeground(wxSTC_C_STRING, wxColour(165, 57, 130));
	m_pDecompiler->StyleSetForeground(wxSTC_C_NUMBER, wxColour(255, 128, 0));

	m_pDecompiler->SetKeyWords(0,
	"call compare LASTRESULT if equal greater smaller jump");

	editor_sizer = new wxBoxSizer(wxVERTICAL);	
	editor_sizer->Add(tScriptTranslated, 2, wxALL | wxEXPAND, 0);
	editor_sizer->Add(m_pButtonsSizer, 0, wxUP | wxBOTTOM | wxEXPAND, 4);
	editor_sizer->Add(tScriptOriginal, 2, wxALL | wxEXPAND, 0);

	global_sizer = new wxBoxSizer(wxHORIZONTAL);	
	global_sizer->Add(editor_sizer, 1, wxALL | wxEXPAND, 0);
	global_sizer->Add(m_pSrcCodeOutput, 0, wxALL | wxEXPAND, 0);
	global_sizer->Add(m_pDecompiler, 1, wxALL | wxEXPAND, 0);

	CreateMyStatusBar();
	StatusToStc(tScriptOriginal);
	StatusToStc(tScriptTranslated);

	if (m_RestoredSize)
	{
		SetSizer(global_sizer);
		SetMinSize(global_sizer->GetMinSize());
	}	
	else
	{
		SetSizerAndFit(global_sizer);
		global_sizer->SetSizeHints(this);
	}
}

void ScriptEditorFrame::OnCodeStyleNeeded(wxStyledTextEvent& event)
{
	m_pSrcCodeOutput->ApplyStyle();

	size_t pos = m_pSrcCodeOutput->FindText(0, m_pSrcCodeOutput->GetTextLength(), L"0x", wxSTC_FIND_MATCHCASE);	

	while (pos != -1)
	{
		size_t i = 2;
		int c = m_pSrcCodeOutput->GetCharAt(pos+2);

		while (isxdigit(c))
		{
			++i;
			c = m_pSrcCodeOutput->GetCharAt(pos + i);
		}

		m_pSrcCodeOutput->StartStyling(pos);
		m_pSrcCodeOutput->SetStyling(i, 5);

		pos = m_pSrcCodeOutput->FindText(pos+i, m_pSrcCodeOutput->GetTextLength(), L"0x", wxSTC_FIND_MATCHCASE);
	}

	size_t lines = m_pSrcCodeOutput->GetLineCount();
	size_t ln = 0;
	uint32_t offset = 0;

	while (ln < lines)
	{
		m_pSrcCodeOutput->MarginSetText(ln, Moon::BitConverter::ToHexString(offset));

		wxString line = m_pSrcCodeOutput->GetLine(ln);
		wxString op = line.substr(0, line.find(' '));

		size_t opcode = -1;

		for (size_t i = 0; i < Script::GetScriptOperationsCount(); ++i)
		{
			if (Script::GetScriptOperations()[i] == op)
			{
				opcode = i;
				break;
			}
		}

		++offset;

		if (opcode != -1)
		{
			offset += Script::GetOperandSize(opcode);
		}

		++ln;
	}
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
