#include "frame_string_editor.hpp"
#include <wx/dir.h>

StringEditor::StringEditor(const id& i) : Editor(i, L"String")
{
	wxFileName path;
	path.SetPath(m_RomTranslated.m_HomeDir);
	path.AppendDir(L"String");
	path.SetFullName(L"String_%s.bin");
	m_PathFormat = path.GetFullPath();

	if (!path.DirExists())
	{
		path.Mkdir(411, wxPATH_RMDIR_RECURSIVE);
	}

	m_EditorDir = path.GetPath();

	wxArrayString files;
	wxDir::GetAllFiles(m_EditorDir, &files, L"*.bin");

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

bool StringEditor::LoadFile(const wxString& path, std::vector<RomString>& strings, uint32_t& blocklength, uint32_t& blockstart)
{
	if(!wxFileExists(path))
		return false;

	strings.clear();

	wxFile file(path);

	uint32_t count;

	file.Read(&blockstart, sizeof(count));
	file.Read(&blocklength, sizeof(uint32_t));
	file.Read(&count, sizeof(count));

	strings.resize(count);

	for(size_t i = 0; i < count; ++i)
	{
		uint32_t ref_count;

		file.Read(&ref_count, sizeof(uint32_t));

		strings[i].references.resize(ref_count);

		for(uint32_t& ref : strings[i].references)
		{
			file.Read(&ref, sizeof(uint32_t));
		}
	}

	for(size_t i = 0; i < count; ++i)
	{
		std::string str;
		char c;

		file.Read(&c, sizeof(char));

		while(c)
		{
			str.push_back(c);
			file.Read(&c, sizeof(char));
		}

		m_RomTranslated.InputTextWithVariables(str);

		strings[i].string =	wxString(str.c_str(), wxCSConv(wxFONTENCODING_CP1252), str.size());
	}

	//m_Original.push_back(RomString(wxString(str.c_str(), wxCSConv(wxFONTENCODING_CP1252), str.size())));

	// wxXmlDocument doc(path, L"UTF-16");

	// if (!doc.IsOk())
	// 	return false;

	// wxXmlNode* root_node = doc.GetRoot();

	// if (!root_node)
	// {
	// 	return false;
	// }

	// strings.clear();

	// blocklength = std::stoi(root_node->GetAttribute(L"size").ToStdWstring(), nullptr, 16);
	// blockstart = std::stoi(root_node->GetAttribute(L"start").ToStdWstring(), nullptr, 16);

	// wxXmlNode* str_node = root_node->GetChildren();

	// while (str_node)
	// {
	// 	wxString attribute = str_node->GetAttribute(L"references");
	// 	wxXmlNode* content_node = str_node->GetChildren();
	// 	const std::wstring& std_attribute = attribute.ToStdWstring();

	// 	wxString string = content_node->GetContent();

	// 	string.Replace(L"[END]", L'\x5', true);
	// 	string.Replace(L"[CLEAR]", L'\xc', true);
	// 	strings.push_back(RomString(string));

	// 	size_t attribute_size = std_attribute.size();

	// 	uint32_t references_count = 0;

	// 	if (attribute_size < 8)
	// 	{
	// 		return false;
	// 	}
	// 	else
	// 	{
	// 		references_count = (attribute_size / 9) + 1;
	// 	}

	// 	for (size_t reference_index = 0; reference_index < references_count; ++reference_index)
	// 	{
	// 		uint32_t reference = std::stoi(std_attribute.substr(reference_index * 9, 8), nullptr, 16);
	// 		strings.back().push_reference(reference);
	// 	}

	// 	str_node = str_node->GetNext();
	// }	

	return true;
}

bool StringEditor::Open(const wxString& path)
{
	if (!LoadFile(path, m_Strings, m_Info.BlockLenght, m_Info.StartBlock))
		return false;

	m_Count = m_Strings.size();
	m_Index = 0;

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

	wxString path = wxString::Format(m_PathFormat, Moon::BitConverter::ToHexString(start));

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

		std::string str = rom.data() + offset;
		size_t str_size = str.size();
		m_RomTranslated.InputTextWithVariables(str);
		wxString wxStr = m_RomTranslated.ReplaceWideChars(str);

		m_Strings.push_back(RomString(wxStr));

		uint32_t pointer = m_RomTranslated.ConvertOffsets(offset);

		FindReferences(rom, pointer, m_Strings.back().references);

		if (m_Strings.back().references.empty())
		{			
			if (m_Strings.size() == 1)
			{
				wxString message = wxString::Format(L"Failed to load strings. No reference found for the first string \"%s\"", wxStr);
				wxMessageBox(message, L"Huh?", wxICON_ERROR);
				m_Count = 0;
				return;
			}

			wxString message = wxString::Format(L"No reference found for string \"%s\" at address 0x%s."
				"loaded block was reduced.", m_Strings.back().string, Moon::BitConverter::ToHexString(offset));

			wxMessageBox(message, L"Huh?", wxICON_WARNING);

			m_Info.BlockLenght = offset - start;
			m_Strings.erase(m_Strings.end() - 1);
			break;
		}

		offset += str_size + 1;
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

		std::string str = m_RomOriginal.ReadString();		

		m_Original.push_back(RomString(wxString(str.c_str(), wxCSConv(wxFONTENCODING_CP1252), str.size())));
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
	bool opened = Open(wxString::Format(m_PathFormat, Moon::BitConverter::ToHexString(m_Files[number])));

	if (!opened)
	{
		return false;
	}

	return true;
}

void StringEditor::SaveFile()
{
	wxString path = wxString::Format(m_PathFormat, Moon::BitConverter::ToHexString(m_Files[m_Number]));

	wxFile file;
	file.Create(path, true);
	file.Open(path, wxFile::OpenMode::write);

	file.Write(&m_Info.StartBlock, sizeof(uint32_t));
	file.Write(&m_Info.BlockLenght, sizeof(uint32_t));

	file.Write(&m_Count, sizeof(m_Count));

	for(const RomString& str : m_Strings)
	{
		uint32_t count = str.references.size();
		file.Write(&count, sizeof(count));

		for(const uint32_t& ref : str.references)
		{
			file.Write(&ref, sizeof(ref));
		}
	}

	for(const RomString& str : m_Strings)
	{
		std::string s = str.string.ToStdString(wxCSConv(wxFONTENCODING_CP1252));
		m_RomTranslated.OutputTextWithVariables(s);		
		file.Write(s.c_str(), s.size()+1);
	}
}

bool StringEditor::InsertFile(size_t index, uint32_t* outsize, uint32_t* outstart)
{	
	uint32_t start = 0;
	uint32_t size = 0;
	std::vector<RomString> strings;

	LoadFile(wxString::Format(m_PathFormat, Moon::BitConverter::ToHexString(m_Files.at(index))), strings, size, start);

	std::string new_block;

	std::vector<uint32_t> pointers;
	pointers.reserve(strings.size());

	const Moon::Hacking::Table& table = m_RomTranslated.GetTable();

	for (RomString& str : strings)
	{
		pointers.push_back(new_block.size());

		wxString wxStr = str.string;
		m_RomTranslated.ReplaceWideChars(wxStr);
		std::string table_str = wxStr.ToStdString(wxCSConv(wxFONTENCODING_CP1252));
		Moon::String::ConvertLineEnds(table_str, m_RomTranslated.GetEOL());
		m_RomTranslated.OutputTextWithVariables(table_str);		
		new_block.append(table_str);
		new_block.push_back('\0');
	}

	bool moved = false;

	if (new_block.size() > size)
	{
		uint32_t free_space = m_RomTranslated.FindFreeSpace(new_block.size());		

		start = free_space;
		size = new_block.size();

		if (outsize != nullptr)
		{
			*outsize = size;
		}
		if (outstart != nullptr)
		{
			*outstart = start;
		}

		if (free_space == std::string::npos)
		{			
			return false;
		}

		moved = true;
	}	

	if (!moved)
		new_block.resize(size);

	for (uint32_t& pointer : pointers)
	{
		pointer += start;
	}

	m_RomTranslated.ConvertOffsets(pointers.data(), pointers.size());

	if (moved)
	{		
		std::string buffer;

		for (const RomString& str : strings)
		{
			char cur_char = 1;			
			uint32_t str_size = 0;

			m_RomTranslated.Seek(str.references[0]);		
			uint32_t pointer = m_RomTranslated.ConvertPointers(m_RomTranslated.ReadUInt32());
			m_RomTranslated.Seek(pointer);

			while (cur_char)
			{
				cur_char = m_RomTranslated.ReadInt8();
				++str_size;
			}
			
			m_RomTranslated.Seek(pointer);

			if (str_size > buffer.size())
				buffer.resize(0x00, str_size);

			m_RomTranslated.Write(buffer.data(), str_size);
		}
	}

	for (size_t i = 0; i < strings.size(); ++i)
	{
		for (const uint32_t& reference : strings[i].references)
		{
			m_RomTranslated.Seek(reference);
			m_RomTranslated.WriteUInt32(pointers[i]);
		}
	}

	m_RomTranslated.Seek(start);
	m_RomTranslated.Write(new_block.data() , new_block.size());	

	return true;
}

void StringEditor::InsertFile()
{
	uint32_t size = 0;
	uint32_t start = 0;
	bool success = InsertFile(m_Number, &size, &start);

	if (success)
	{
		if (start != 0)
		{
			m_Info.StartBlock = start;
			m_Info.BlockLenght = size;
			wxMessageBox(wxString(L"The string block was moved to adress 0x") << Moon::BitConverter::ToHexString(start), L"Warning", wxICON_WARNING);
		}

		m_RomTranslated.Flush();
	}
	else
	{
		if (start == std::string::npos)
			wxLogError(L"Could not find %i bytes of free space to insert the file.", size);
	}
}

void StringEditor::InsertAll()
{
	for (const auto& file : m_Files)
	{
		InsertFile(file.first, nullptr, nullptr);
	}

	m_RomTranslated.Flush();
}

bool StringEditor::GetTextFromPath(const std::string& file)
{
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

	if(m_pEditor->GetRom(false).Console == console::DS)
	{
		m_pTextEditor->SetEOLMode(0x0a);
	} else 
	{
		m_pTextEditor->SetEOLMode(0x0d0a);
	}
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
	std::string std_text = offset.Upper().ToStdString();
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
		backup.append((const wchar_t*)&m_pEditor->GetEditorInfo().StartBlock, sizeof(uint32_t) / sizeof(wchar_t));
		backup.append((const wchar_t*)&m_pEditor->GetEditorInfo().BlockLenght, sizeof(uint32_t) / sizeof(wchar_t));

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

	backup_data += (2 * sizeof(uint32_t)) / sizeof(wchar_t);

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
	if (!m_pEditor->IsOpened())
		return;

	m_pTextEditor->SetText(((StringEditor*)m_pEditor)->GetCurrentText().string);
	m_pTextOriginal->SetText(((StringEditor*)m_pEditor)->GetCurrentOriginal().string);
}

void StringEditorFrame::UpdateFile()
{
	SetTitle(wxString::Format(L"String at %s", Moon::BitConverter::ToHexString(((StringEditor*)m_pEditor)->GetEditorInfo().StartBlock)));
}

void StringEditorFrame::OnUpdateFilesClick(wxCommandEvent& event)
{
	for (size_t i = 0; i < m_pEditor->GetEditorInfo().Count; ++i)
	{
		((StringEditor*)m_pEditor)->Open(i);

		for(RomString& str : ((StringEditor*)m_pEditor)->GetStrings())
		{
			str.string.Replace("\x05\x0c\r\n\r\n", "\x05\x0c\r\n");
		}

		((StringEditor*)m_pEditor)->SaveFile();
	}

	event.Skip();
}

void StringEditorFrame::CreateGUIControls()
{
	CreateMyMenuBar();
	CreateFileMenu();
	CreateStringMenu();
	CreateViewMenu();
	CreateToolsMenu(false, true, false, false);

	Bind(wxEVT_MENU, &StringEditorFrame::OnUpdateFilesClick, this, m_pMenuTools->Append(wxID_ANY, L"Update Files")->GetId());

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
	
	if (m_RestoredSize)
	{
		SetSizer(root_sizer);
		SetMinSize(root_sizer->GetMinSize());
	}
	else
	{
		SetSizerAndFit(root_sizer);
		root_sizer->SetSizeHints(this);
	}
}