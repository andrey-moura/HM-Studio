#include "frame_string_editor.hpp"
#include <moon/diagnostics.hpp>
#include <wx/dir.h>

StringEditor::StringEditor(const id& i) : Editor(i, L"String")
{
	wxFileName path;
	path.SetPath(m_RomTranslated.m_HomeDir);
	path.AppendDir(L"String");
	path.SetFullName(L"String_%s.xml");
	m_PathFormat = path.GetFullPath();

	if (!path.DirExists())
	{
		path.Mkdir(411, wxPATH_RMDIR_RECURSIVE);
	}

	m_EditorDir = path.GetPath();

	wxArrayString files;
	wxDir::GetAllFiles(m_EditorDir, &files, L"*.xml");

	size_t index = 0;

	for (const wxString& file : files)
	{
		size_t pos = file.find(L"String_");

		if (pos != std::string::npos)
		{
			wxString _offset = file.substr(pos+7, 8);
			uint32_t offset = std::stoi(_offset.ToStdWstring(), nullptr, 16);
			m_Files.insert(std::pair(index, offset));
			++index;
		}
	}	

	m_Info.Count = files.size();	
}

bool StringEditor::Open(const wxString& path)
{
	wxXmlDocument doc(path, L"UTF-16");

	if (!doc.IsOk())
		return false;

	wxXmlNode* root_node = doc.GetRoot();

	if (!root_node)
	{
		return false;
	}

	m_Info.BlockLenght = std::stoi(root_node->GetAttribute(L"size", L"0").ToStdWstring(), 0, 16);
	m_Strings.clear();

	wxXmlNode* str_node = root_node->GetChildren();

	while (str_node)
	{
		wxString attribute = str_node->GetAttribute(L"references");
		wxXmlNode* content_node = str_node->GetChildren();
		const std::wstring& std_attribute = attribute.ToStdWstring();

		wxString string = content_node->GetContent();		

		string.Replace(L"[END]", L'\x5', true);
		string.Replace(L"[CLEAR]", L'\xc', true);
		m_Strings.push_back(RomString(string));

		size_t attribute_size = std_attribute.size();

		uint32_t references_count = 0;

		if (attribute_size < 8)
		{
			return false;
		}		
		else
		{
			references_count = (attribute_size / 9) + 1;
		}

		for (size_t reference_index = 0; reference_index < references_count; ++reference_index)
		{
			uint32_t reference = std::stoi(std_attribute.substr(reference_index*9, 8), nullptr, 16);
			m_Strings.back().push_reference(reference);
		}		
		
		str_node = str_node->GetNext();
	}

	m_Count = m_Strings.size();
	m_Index = 0;
	m_Info.StartBlock = std::stoi(root_node->GetAttribute(L"start", L"0").ToStdWstring(), nullptr, 16);
	m_Info.BlockLenght = std::stoi(root_node->GetAttribute(L"size", L"0").ToStdWstring(), nullptr, 16);	

	OpenOriginal();

	return true;
}

void FindReferences(const std::vector<char>& rom, const size_t& pointer, std::vector<uint32_t>& output)
{
	const char* cur_byte = rom.data();
	uint32_t rom_size = rom.size();

	for (size_t i = 0; i < rom_size; i += 4, cur_byte +=4)
	{
		if (pointer == *(uint32_t*)cur_byte)
		{
			output.push_back(i);
		}
	}
}

void StringEditor::Open(uint32_t start, uint32_t size)
{
	m_Index = 0;

	wxString path = wxString::Format(m_PathFormat, Moon::BitConverter::TToHexString(start));

	for (auto& file : m_Files)
	{
		if (file.second == start)
		{
			m_Number = file.first;
		}
	}

	if (wxFileExists(path))
	{
		if (Open(path))
			return;

		wxString parse_error_format = L"Error parsing xml file: %s\n"
			L"Getting text from ROM...";

		wxMessageBox(wxString::Format(parse_error_format, path), L"Huh?", wxICON_ERROR);
	}
	
	m_Strings.clear();

	m_Info.StartBlock = start;
	m_Info.BlockLenght = size;

	std::vector<char> rom = m_RomTranslated.ReadBytes<char>(0, m_RomTranslated.Length());	
	m_RomTranslated.GetTable().Input((unsigned char*)rom.data() + start, size);	

	uint32_t offset = start;

	while (offset < start+size)
	{
		if (rom[offset] == 0)
		{
			offset++;
			continue;
		}

		std::string_view str = rom.data() + offset;
		m_Strings.push_back(RomString(rom.data() + offset, str.size()));

		uint32_t pointer = m_RomTranslated.ConvertOffsets(offset);

		FindReferences(rom, pointer, m_Strings.back().references);

		if (m_Strings.back().references.empty())
		{
			wxMessageBox(wxString(L"Failed to load strings. No reference found for string at address") << Moon::BitConverter::TToHexString(offset));
		}

		offset += str.size() + 1;
	}

	m_Number = m_Info.Count;
    m_Info.Count++;

	m_Files.insert(std::pair(m_Number, m_Info.StartBlock));

	m_Count = m_Strings.size();

	OpenOriginal();
}

void StringEditor::OpenOriginal()
{
	m_Original.clear();
	m_Original.reserve(m_Strings.size());

	for (size_t i = 0; i < m_Strings.size(); ++i)
	{
		uint32_t ref = m_Strings[i].references[0];
		uint32_t addr = m_RomOriginal.ReadPointer32(ref);

		m_RomOriginal.Seek(addr);
		m_Original.push_back(RomString(m_RomOriginal.ReadString()));
	}
}

bool StringEditor::Save(const wxString& str)
{
	m_Strings[m_Index] = str;
	return NextText();
}

bool StringEditor::Open(uint32_t number)
{
	if (number >= m_Info.Count)
	{
		return false;
	}

	m_Number = number;
	bool opened = Open(wxString::Format(m_PathFormat, Moon::BitConverter::TToHexString(m_Files[number])));

	if (!opened)
	{
		return false;
	}

	return true;
}

void StringEditor::SaveFile()
{
	wxXmlDocument doc;
	doc.SetFileEncoding(L"UTF-16");
	wxXmlNode* root = new wxXmlNode(nullptr, wxXmlNodeType::wxXML_ELEMENT_NODE, L"ROM-String");
	doc.SetRoot(root);
	root->AddAttribute(L"start", Moon::BitConverter::TToHexString(m_Info.StartBlock));
	root->AddAttribute(L"size", Moon::BitConverter::TToHexString(m_Info.BlockLenght));	

	size_t i = 0;
	wxString string_format = L"String %i";
	wxXmlNodeType mode;

	uint32_t eol = Moon::String::EOLToInt(doc.GetEOL().ToStdWstring());
	wxString empty_chars = doc.GetEOL() << L" ";


	for (int i = m_Strings.size()-1; i >= 0; --i)
	{
		wxXmlNode* str_node = new wxXmlNode(root, wxXmlNodeType::wxXML_ELEMENT_NODE, "String", wxEmptyString, nullptr, nullptr, i + 2);		

		wxString string = m_Strings[i].string;
		Moon::String::ConvertLineEnds(const_cast<std::wstring&>(string.ToStdWstring()), eol);
		string.Replace(L'\x5', L"[END]");
		string.Replace(L'\xc', L"[CLEAR]");

		//The XML file don't allow white spaces only strings
		if (string.find_first_not_of(empty_chars) == std::string::npos)
		{
			mode = wxXmlNodeType::wxXML_CDATA_SECTION_NODE;
		}
		else
		{
			mode = wxXmlNodeType::wxXML_TEXT_NODE;
		}

		str_node->AddChild(new wxXmlNode(mode, L"", string));

		std::string references;

		for (size_t reference_index = 0; reference_index < m_Strings[i].references.size(); ++reference_index)
		{
			references.append(Moon::BitConverter::TToHexString(m_Strings[i].references[reference_index]));
			references.append(",");
		}

		references.pop_back();

		str_node->AddAttribute(L"references", references);
	}
	
	doc.Save(wxString::Format(m_PathFormat, Moon::BitConverter::TToHexString(m_Info.StartBlock)));
}

void StringEditor::InsertFile()
{
	std::string new_block;
	new_block.reserve(m_Info.BlockLenght);	

	std::vector<uint32_t> pointers;
	pointers.reserve(m_Strings.size());

	const Moon::Hacking::Table& table = m_RomTranslated.GetTable();

	for (RomString& str : m_Strings)
	{
		pointers.push_back(new_block.size());
		std::string table_str = str.string.ToStdString();
		table.Output(table_str);
		Moon::String::ConvertLineEnds(table_str, m_RomTranslated.GetEOL());
		new_block.append(table_str);
		new_block.push_back('\0');
	}

	bool moved = false;

	if (new_block.size() > m_Info.BlockLenght)
	{
		uint32_t free_space = m_RomTranslated.FindFreeSpace(new_block.size());

		if (free_space == std::string::npos)
		{
			wxLogError(L"Could not find %i bytes of free space to insert the file.", new_block.size());
			return;			
		}

		m_Info.StartBlock = free_space;
		m_Info.BlockLenght = new_block.size();
		moved = true;
	}

	if (!moved)
		new_block.resize(m_Info.BlockLenght);

	for (uint32_t& pointer : pointers)
	{
		pointer += m_Info.StartBlock;
	}

	m_RomTranslated.ConvertOffsets(pointers.data(), pointers.size());

	std::vector<uint32_t> rom = m_RomTranslated.ReadBytes<uint32_t>(0, m_RomTranslated.Length() / 4);

	if (moved)
	{
		for (const RomString& str : m_Strings)
		{
			char* old_str = (char*)rom.data() + m_RomTranslated.ConvertPointers(rom[str.references[0] / 4]);
			memset(old_str, 0, strlen(old_str));
		}
	}

	for (size_t i = 0; i < m_Strings.size(); ++i)
	{
		for (const uint32_t& reference : m_Strings[i].references)
		{
			rom[reference/4] = pointers[i];
		}
	}

	memcpy(((byte*)rom.data()) + m_Info.StartBlock, new_block.c_str(), new_block.size());
	m_RomTranslated.Seek(0);
	m_RomTranslated.WriteBytes(rom);

	if (moved)
		wxMessageBox(wxString(L"The string block was moved to adress 0x") << Moon::BitConverter::TToHexString(m_Info.StartBlock), L"Warning", wxICON_WARNING);
}

bool StringEditor::GetTextFromPath(const std::string& file)
{
	wxXmlDocument doc(file, L"UTF-16");

	if (!doc.IsOk())
		return false;

	wxXmlNode* root_node = doc.GetRoot();

	if (!root_node)
	{
		return false;
	}

	wxXmlNode* str_node = root_node->GetChildren();
	uint32_t i = 0;

	while (str_node)
	{		
		wxXmlNode* content_node = str_node->GetChildren();		

		const wxString& content = content_node->GetContent();
		m_Strings[i].string = content;

		str_node = str_node->GetNext();
		++i;
	}

	return true;	
}

const RomString& StringEditor::GetCurrentText()
{
	return m_Strings[m_Index];
}

const RomString& StringEditor::GetCurrentOriginal()
{
	return m_Original[m_Index];
}

StringEditorFrame::StringEditorFrame(const id& i) : EditorFrame(new StringEditor(i))
{
	CreateGUIControls();
	CheckBackup();	
}

void StringEditorFrame::OnGoFile()
{
	wxTextEntryDialog dialog(nullptr, L"Enter an offset and size, separated by \",\"");

	if (dialog.ShowModal() == wxID_OK)
	{
		GoFile(dialog.GetValue());
	}
}

void StringEditorFrame::GoFile(const wxString& offset)
{
	std::string std_text = offset.ToStdString();
	size_t start = std::stoi(std_text, 0, 16);
	size_t size = std::stoi(std_text.substr(std_text.find(',') + 1), 0, 16);

	((StringEditor*)m_pEditor)->Open(start, size);
	UpdateText();
}

void StringEditorFrame::OnSaveString()
{
	if (((StringEditor*)m_pEditor)->Save(m_pTextEditor->GetText()))
	{
		wxString backup;
		backup.reserve(m_pEditor->GetEditorInfo().BlockLenght);
		backup.append((const wchar_t*)&m_pEditor->GetEditorInfo().StartBlock, (sizeof uint32_t) / (sizeof WCHAR));		
		backup.append((const wchar_t*)&m_pEditor->GetEditorInfo().BlockLenght, (sizeof uint32_t) / (sizeof WCHAR));

		for (const RomString& str : ((StringEditor*)m_pEditor)->GetStrings())
		{
			backup.append(str.string);
			backup.append(1, '\0');
		}

		BackupText(backup);

		UpdateText();
	}
}

void StringEditorFrame::OnResumeBackup(const wxString& backup)
{
	const wchar_t* backup_data = backup.data();
	EditorInfo& info = m_pEditor->GetEditorInfo();

	((StringEditor*)m_pEditor)->Open(*(uint32_t*)(backup_data), ((uint32_t*)backup_data)[1]);

	backup_data += (2 * sizeof uint32_t) / sizeof backup_data[0];

	for (RomString& str : ((StringEditor*)m_pEditor)->GetStrings())
	{
		str.string = backup_data;
		backup_data += str.string.size() + 1;
	}

	((StringEditor*)m_pEditor)->OpenOriginal();
	UpdateText();
}

void StringEditorFrame::UpdateText()
{
	m_pTextEditor->SetText(((StringEditor*)m_pEditor)->GetCurrentText().string);
	m_pTextOriginal->SetText(((StringEditor*)m_pEditor)->GetCurrentOriginal().string);
}

void StringEditorFrame::OnUpdateFilesClick(wxCommandEvent& event)
{
	for (size_t i = 0; i < m_pEditor->GetCount(); ++i)
	{

	}

	event.Skip();
}

void StringEditorFrame::CreateGUIControls()
{
	CreateMyMenuBar();
	CreateFileMenu();
	CreateStringMenu();
	CreateToolsMenu(false, false, false, false);

#ifdef _DEBUG
	m_pMenuTools->Append(wxID_ANY, L"Update All Files", nullptr);
#endif

	CreateMyToolBar(true, true, true, true, true);
	
	m_pTextEditor = new STC(this, wxID_ANY);	
	m_pTextEditor->SetMaxLineLenght(m_pEditor->GetRom(true).GetLineMax());
	m_pTextOriginal = new STC(this, wxID_ANY);
	m_pTextOriginal->SetMaxLineLenght(m_pEditor->GetRom(false).GetLineMax());
	m_pTextOriginal->SetUseSpellChecker(false);

	CreateButtonsSizer();	

	CreateMyStatusBar();
	StatusToStc(m_pTextEditor);
	StatusToStc(m_pTextOriginal);

	wxBoxSizer* root_sizer = new wxBoxSizer(wxVERTICAL);
	root_sizer->Add(m_pTextEditor, 1, wxEXPAND, 0);
	root_sizer->Add(m_pButtonsSizer, 0, wxEXPAND, 0);
	root_sizer->Add(m_pTextOriginal, 1, wxEXPAND, 0);
	SetSizerAndFit(root_sizer);
}