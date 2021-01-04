#include "class_script_editor.hpp"

ScriptEditor::ScriptEditor(const id& id) : Editor(id, L"Script")
{
	GetRomInfo();

	wxFileName pathLeft;
	pathLeft.SetPath(m_RomOriginal.m_HomeDir);
	pathLeft.AppendDir("Script");

	m_ScriptDir = pathLeft.GetFullPath().ToStdString();		 
	
	pathLeft.AppendDir("Original");
	pathLeft.SetName("Script_Original_");
	m_PathOrigLeft = pathLeft.GetFullPath();
	pathLeft.RemoveLastDir();
	pathLeft.AppendDir("Translated");
	pathLeft.SetName("Script_Translated_");
	m_PathTransLeft = pathLeft.GetFullPath();

	switch (m_RomOriginal.Id)
	{
	case id::FoMT:
		m_PathRight = ".fsf";
		break;
	case id::MFoMT:
		m_PathRight = ".msf";
		break;
	case id::DS:
		m_PathRight = ".dsf";
		break;
	default:
		break;
	}
}

bool ScriptEditor::Open(uint32_t number)
{
	std::vector<uint8_t> dataOri = Moon::File::ReadAllBytes<uint8_t>(GetPath(number, false));

	Script script(dataOri);
		
	std::vector<uint8_t> data = Moon::File::ReadAllBytes<uint8_t>(GetPath(number, true));

	m_ScriptOriginal.SetData(dataOri);
	m_ScriptTranslated.SetData(data);

	if (!m_ScriptTranslated.HaveText())
		m_ScriptTranslated.SetData(dataOri);

	m_Original = m_ScriptOriginal.GetText();
	m_Translated = m_ScriptTranslated.GetText();

	m_RomTranslated.InputTextWithVariables(m_Translated);
	m_RomOriginal.InputTextWithVariables(m_Original);

	m_Index = 0;
	m_Number = number;
	m_Count = m_Original.size();

	m_Opened = true;	

	return true;
}

std::string ScriptEditor::GetCurOriginal()
{
	return m_Original[m_Index];
}

std::string ScriptEditor::GetCurTranslated()
{
	return m_Translated[m_Index];
}

bool ScriptEditor::SaveText(const std::string& text)
{
	m_Translated[m_Index] = text;
	return NextText();
}

void ScriptEditor::UpdateScript()
{
	std::vector<std::string> text = m_Translated;
	m_RomTranslated.OutputTextWithVariables(text);
	m_ScriptTranslated.UpdateText(text);
}

void ScriptEditor::SetText(const std::vector<std::string>& text)
{
	m_Translated = text;
	m_RomTranslated.InputTextWithVariables(m_Translated);
}

void ScriptEditor::SaveFile()
{
	std::vector<std::string> buffer = m_Translated;

	m_RomTranslated.OutputTextWithVariables(buffer);

	m_ScriptTranslated.UpdateText(buffer);

	Moon::File::WriteAllBytes(GetPath(true), m_ScriptTranslated.GetData(), m_ScriptTranslated.GetRiffLenght());
}

bool ScriptEditor::GetTextFromPath(const std::string& path)
{
	Script script = Moon::File::ReadAllBytes<uint8_t>(path);

	if (script.Count() != m_ScriptTranslated.Count())
		return false;

	m_Translated = script.GetText();
	return true;
}

bool ScriptEditor::GetTextFromNumber(size_t number)
{
	return GetTextFromPath(GetPath(number, true));
}

void ScriptEditor::ReplaceInAllScripts(const std::string& find, const std::string& replace)
{
	wxFileName outPut(m_RomOriginal.Path);
	outPut.RemoveLastDir();
	outPut.AppendDir("Script");
	outPut.AppendDir("Backup");

	if (!outPut.DirExists())
		outPut.Mkdir(511, wxPATH_MKDIR_FULL);

	for (size_t i = 0; i < m_Info.Count; ++i)
	{
		std::string path = GetPath(true);	
		outPut.SetFullName(GetPath(i, true));

		Script script(Moon::File::ReadAllBytes<uint8_t>(path));

		if (!script.HaveText())
			continue;

		std::string_view view = (const char*)script.GetStartText();

		if (view.find(find) == std::string::npos)
			continue;

		wxCopyFile(path, outPut.GetFullPath());

		std::vector<std::string> text = script.GetText();

		m_RomTranslated.InputTextWithVariables(text);

		for (std::string& s : text)
		{
			Moon::String::Replace(s, find, replace);
		}

		m_RomTranslated.OutputTextWithVariables(text);
		script.UpdateText(text);

		Moon::File::WriteAllBytes(path, script.GetData(), script.GetRiffLenght());
	}
}

void ScriptEditor::GetRomInfo()
{
	switch (m_RomOriginal.Id)
	{
	case id::FoMT:
		m_Info.StartPointers = 0xF89D8;
		m_Info.Count = 1328;
		m_Info.StartBlock = 0x2AC0B0;
		m_Info.BlockLenght = 0x24D01C;
		break;
	case id::MFoMT:
		m_Info.StartPointers = 0x1014C0;
		m_Info.Count = 1415;
		m_Info.StartBlock = 0x701694;
		m_Info.BlockLenght = 0x2A9980;
		break;
	case id::DS:
		m_Info.StartPointers = 0x24C2E04;
		m_Info.Count = 1296 - 1;//I don't know what happened, but the ROM is missing the last script...
		m_Info.StartBlock = 0x24C4244;
		m_Info.BlockLenght = 0x2E9C10;
		break;
	default:
		break;
	}	
}

uint32_t ScriptEditor::GetOffset(bool translated, size_t number)
{			
	RomFile& rom = GetRom(translated);
	uint32_t pointer = rom.ReadPointer32(m_Info.StartPointers + (number * 4));

	if (rom.Console == console::DS)
	{
		pointer += m_Info.StartPointers - 4;
	}

	return pointer;
}

uint32_t ScriptEditor::GetOffset(bool translated)
{
	return GetOffset(translated, m_Number);
}

uint32_t* ScriptEditor::GetOffsets(bool translated)
{	
	RomFile& rom = GetRom(translated);

	uint32_t* output = new uint32_t[m_Info.Count];

	rom.Seek(m_Info.StartPointers);
	rom.Read(output, m_Info.Count * 4);

	if (rom.Console == console::DS)
	{
		for(size_t i = 0; i < m_Info.Count; ++i)
		{
			output += m_Info.StartPointers - 4;
		}
	}
	else {		

		for (size_t i = 0; i < m_Info.Count; ++i)
		{
			output[i] &= ROM_BUS_NOT;
		}
	}

	return output;
}

void ScriptEditor::SetOffset(uint32_t offset)
{	
	if (m_RomTranslated.Console == console::DS)
	{
		offset += 0x1444;
	}
	else
	{
		offset |= ROM_BUS;
	}

	m_RomTranslated.WriteUInt32(offset, m_Info.StartPointers + (m_Number * 4));
}

void ScriptEditor::SetOffsets(uint32_t* offets)
{
	if (m_RomTranslated.Console == console::DS)
	{
		for (size_t i = 0; i < m_Info.Count; ++i)
		{
			offets[i] += 0x1444;
		}
	}
	else {

		for (size_t i = 0; i < m_Info.Count; ++i)
		{
			offets[i] |= ROM_BUS;
		}
	}
}

void ScriptEditor::SetEvent(uint32_t index)
{
	uint32_t offset = GetOffset(true);
	uint32_t offsetAdress = m_Info.StartPointers + (index * 4);

	m_RomTranslated.WriteUInt32(offset, offsetAdress);
	m_RomTranslated.Flush();
}

inline size_t ScriptEditor::ScriptSize(uint32_t* value)
{
	if (value[0] != SCRIPT_RIFF)
		return std::string::npos;

	return value[1];
}

inline uint32_t ScriptEditor::ScriptSize(const uint32_t& offset, bool translated)
{
	RomFile& rom = GetRom(translated);
	uint64_t data = rom.ReadUint64(offset);
	return ScriptSize((uint32_t*)&data);
}

inline uint32_t ScriptEditor::ScriptSize(const uint32_t& offset, uint8_t* bytes)
{		
	return ScriptSize((uint32_t*)(&bytes[offset]));
}

inline bool ScriptEditor::IsInsideBlock(const uint32_t& offset)
{
	return offset >= m_Info.StartBlock && offset < (m_Info.StartBlock + m_Info.BlockLenght);	
}

inline bool ScriptEditor::IsFreeSpace(const uint32_t& offset, const uint32_t& size)
{
	uint8_t* data = new uint8_t[size];

	m_RomTranslated.Seek(offset);
	m_RomTranslated.Read(data, size);

	bool inside = IsInsideBlock(offset);
	bool free = Finder::VerifyFilled(inside ? 0x00 : 0xff, data, size);

	delete[] data;

	return free;
}

FilesResults ScriptEditor::FindInScripts(std::string& search, bool useTable, bool translated) const
{
	FilesResults results;

	const char* ignore = "\x5\xff\r\n";

	wxString state = translated ? "Translated" : "Original";	

	wxString hit = " hit";
	wxString hits = " hits";

	size_t totalCount = 0;

	wxProgressDialog dialog("Find in scripts in progress...", "", m_Info.Count);
	dialog.Show();

	const Moon::Hacking::Table& table = GetRom(translated).GetTable();

	if (useTable)
	{
		table.Output(search);
	}

	for (size_t i = 0; i < m_Info.Count; ++i)
	{
		wxString name = wxString("Script_") << state << "_" << i << m_PathRight;
		dialog.Update(i, name);

		Script script = Moon::File::ReadAllBytes<uint8_t>(GetPath(i, translated));

		//No text, next
		if (!script.HaveText())
			continue;		

		//No find, continue

		uint32_t lenght = script.GetTextSize();

		if (std::string_view((const char*)script.GetStartText(), lenght).find(search) == std::string::npos)
			continue;				

		SearchResult match;
		
		for (size_t z = 0; z < script.Count(); ++z)
		{
			const std::string_view& str = script[z];

			size_t pos = str.find(search);

			//No find, continue
			if (pos == std::string::npos)
				continue;			

			size_t end = str.find_first_of(ignore, pos);

			if (end != std::string::npos)
			{
				end -= pos;
			}

			else
			{
				end = str.size();
			}

			std::string pattern = std::string(str.substr(pos, end));

			if (useTable)
			{
				table.Input(pattern);
			}

			match.AppendResult(wxString("String ") << z << ": " << pattern);
		}

		size_t count = match.GetCount();

		if (count == 0)
			continue;

		totalCount += count;

		match.SetTitle(name << " (" << count <<  (count == 1 ? hit : hits) << ")");
		results.AppendMatch(match);
	}

	dialog.Close();

	results.SetSearchTitle(wxString("Search \"") << search << "\" (" << totalCount << " " << (totalCount == 1 ? hit : hits) << " in " << results.GetCount() << " script" << (results.GetCount() > 1 ? "s)" : ")"));

	return results;
}

std::vector<size_t> ScriptEditor::Find(std::string& find, bool useTable, bool translated)
{
	const std::vector<std::string> text = translated ? m_Translated : m_Original;
	std::vector<size_t> result;

	const Moon::Hacking::Table &table = GetRom(translated).GetTable();

	if (useTable)
	{
		table.Input(find);
	}

	for (size_t i = 0; i < text.size(); ++i)
	{
		size_t pos = text[i].find(find);

		if (pos != std::string::npos)
		{
			result.push_back(i);
		}
	}

	return result;
}

void ScriptEditor::Replace(std::string& find, bool useTable, const std::string& replace)
{	
	const Moon::Hacking::Table& table = GetRom(true).GetTable();

	if (useTable)
	{
		table.Input(find);
	}

	for (std::string& str : m_Translated)
	{
		Moon::String::Replace(str, find, replace);
	}
}

void ScriptEditor::DumpAll(bool translated)
{
	uint32_t* offsets = GetOffsets(translated);

	//More memmory, less seeks
	uint8_t* scriptBlock = new uint8_t[m_Info.BlockLenght];

	RomFile& rom = GetRom(translated);

	rom.Seek(m_Info.StartBlock);
	rom.Read(scriptBlock, m_Info.BlockLenght);	

	uint32_t maxPosition = m_Info.StartBlock + m_Info.BlockLenght;

	wxFileName fn;
	fn.SetPath(m_ScriptDir);
	fn.AppendDir(translated ? L"Translated" : L"Original");

	if(!fn.DirExists())
	{
		fn.Mkdir();
	}	

	for (uint32_t i = 0; i < m_Info.Count; ++i)
	{
		size_t size = 0;
		std::string path = GetPath(i, translated); 

		if (!IsInsideBlock(offsets[i]))
		{
			//The script has been moved to outside of the block
			size = ScriptSize(offsets[i], translated);

			if (size != 0xffffffff)
			{								
				Moon::File::WriteAllBytes(path, rom.ReadBytes<uint8_t>(offsets[i], size));
			}

			continue;
		}

		size = ScriptSize(offsets[i] - m_Info.StartBlock, scriptBlock);

		if (size == 0xffffffff)
			return;

		Moon::File::WriteAllBytes(path, scriptBlock + (offsets[i] - m_Info.StartBlock), size);
	}

	delete[] offsets;
	delete[] scriptBlock;
}

void ScriptEditor::EraseScript(uint32_t offset, uint32_t size)
{
	if (size != 0xffffffff)
	{
		bool inside = IsInsideBlock(offset);

		std::string zeroBlock;
		zeroBlock.resize(size, inside ? 0x00 : 0xff);

		m_RomTranslated.Seek(offset);
		m_RomTranslated.Write(zeroBlock.data(), size);
	}
}

void ScriptEditor::InsertOnly(const Script& script, uint32_t offset)
{
	m_RomTranslated.Seek(offset);
	m_RomTranslated.Write(script.GetData(), script.GetRiffLenght());
	m_RomTranslated.Flush();
}

void ScriptEditor::InsertSmaller(Script& script, uint32_t offset, uint32_t oldSize)
{	
	uint8_t* data = new uint8_t[oldSize];

	memset(data, 0x00, oldSize);	

	memcpy(data, script.GetData(), script.GetRiffLenght());

	m_RomTranslated.Seek(offset);
	m_RomTranslated.Write(data, oldSize);
	m_RomTranslated.Flush();

	delete[] data;
}

bool ScriptEditor::InsertVerify(Script& script, uint32_t offset, uint32_t newSize)
{
	bool free = IsFreeSpace(offset, newSize);

	if (free)
		InsertOnly(script, offset);

	return free;
}

bool ScriptEditor::InsertLarger(const Script& script, uint32_t oldOffset, uint32_t oldSize, uint32_t newSize)
{
	size_t result = newSize - oldSize;
	size_t endOffset = oldOffset + oldSize;

	bool flag = IsFreeSpace(endOffset, result);

	if (flag)
	{
		m_RomTranslated.Seek(oldOffset);
		m_RomTranslated.Write(script.GetData(), newSize);
		m_RomTranslated.Flush();
	}

	return flag;
}

void ScriptEditor::InsertMove(const Script& script, uint32_t oldOffset, uint32_t oldSize, uint32_t newOffset)
{
	SetOffset(newOffset);
	EraseScript(oldOffset, oldSize);
	InsertOnly(script, newOffset);
}

bool ScriptEditor::InsertFind(Script& script, uint32_t oldOffset, uint32_t oldSize, uint32_t newSize)
{
	uint8_t* scriptBlock = new uint8_t[m_Info.BlockLenght];

	m_RomTranslated.Seek(m_Info.StartBlock);
	m_RomTranslated.Read(scriptBlock, m_Info.BlockLenght);	

	if (oldSize != std::string::npos)
	{		
		if (IsInsideBlock(oldOffset))
			memset(scriptBlock + (oldOffset - m_Info.StartBlock), 0x00, oldSize);
	}		

	bool trying = true;

	uint32_t freeSpace = 0;

	uint32_t riff = SCRIPT_RIFF;

	bool flag = false;

	while (1)
	{
		freeSpace = Finder::FindFilledAlingBlock(0x00, newSize, scriptBlock, m_Info.BlockLenght, freeSpace, 4);

		if (freeSpace == std::string::npos)
		{
			break;
		}

		uint32_t lastScript = Finder::FindBack(scriptBlock, m_Info.BlockLenght, &riff, 4, freeSpace);
		uint32_t lastSize = ScriptSize((uint32_t*)&scriptBlock[lastScript]);

		if ((lastScript + lastSize) > freeSpace)
		{
			freeSpace = lastScript + lastSize;			
			continue;
		}
	
		flag = true;
		break;
	}

	delete[] scriptBlock;

	if (flag)
	{
		InsertMove(script, oldOffset, oldSize, freeSpace + m_Info.StartBlock);
	}

	return flag;
}

void ScriptEditor::InsertFile()
{	
	uint32_t oldOffset = GetOffset(true);
	uint32_t oldSize = ScriptSize(oldOffset, true);
	uint32_t newSize = m_ScriptTranslated.GetRiffLenght();	

	if (newSize <= oldSize)
	{
		if (oldSize != std::string::npos)
		{
			InsertSmaller(m_ScriptTranslated, oldOffset, oldSize);
			return;// ScriptFlags::INSERT_LESS;
		}

		if (InsertVerify(m_ScriptTranslated, oldOffset, newSize))
			return;// ScriptFlags::INSERT_VERIFY;
	}
	else if (newSize > oldSize)
	{
		if (InsertLarger(m_ScriptTranslated, oldOffset, oldSize, newSize))
			return; // ScriptFlags::INSERT_LARGER;
	}	 

	if (InsertFind(m_ScriptTranslated, oldOffset, oldSize, newSize))
		return; // ScriptFlags::INSERT_FIND;

	//return ScriptFlags::INSERT_NONE;
}

void ScriptEditor::InsertAll()
{
	std::string script_block;
	script_block.reserve(m_Info.BlockLenght);

	std::vector<uint32_t> scripts_pointers;
	scripts_pointers.reserve(m_Info.Count);

	uint32_t oldOffset = 0;

	//We'll use this buffer to fill scripts in the end of the
	//ROM. But we'll not delete and allocate again, we'll alocate
	//once, and grow if we need a larger buffer.
	std::string ffbuffer;

	for (size_t script_number = 0; script_number < m_Info.Count; ++script_number)
	{
		oldOffset = GetOffset(true, script_number);

		if (m_RomTranslated.IsInsideFreeSpace(oldOffset))
		{
			m_RomTranslated.Seek(oldOffset);

			if (m_RomTranslated.ReadUInt32() == SCRIPT_RIFF)
			{
				uint32_t size = m_RomTranslated.ReadUInt32();

				if (size > ffbuffer.size())
				{
					ffbuffer.resize(size, 0xff);
				}

				m_RomTranslated.Seek(oldOffset);
				m_RomTranslated.WriteBytes(ffbuffer.data(), size);
			}
		}

		Script script = Moon::File::ReadAllBytes<uint8_t>(GetPath(script_number, true));

		if (!script)
		{
			wxMessageBox(wxString(L"Failed to load script ") << script_number << L".", L"Huh?", wxICON_ERROR);
			return;
		}

		scripts_pointers.push_back(script_block.size()+m_Info.StartBlock);
		script_block.append((const char*)script.GetData(), script.GetRiffLenght());

		while (script_block.size() % 4 != 0)
			script_block.push_back('\0');
	}

	if (script_block.size() > m_Info.BlockLenght)
	{
		wxMessageBox(wxString(L"Failed to insert. No enough space."), L"Huh?", wxICON_ERROR);
		return;
	}

	script_block.resize(m_Info.BlockLenght, '\0');
	m_RomOriginal.ConvertOffsets(scripts_pointers.data(), m_Info.Count);

	m_RomTranslated.Seek(m_Info.StartPointers);
	m_RomTranslated.WriteBytes(scripts_pointers);
	m_RomTranslated.Seek(m_Info.StartBlock);
	m_RomTranslated.Write(script_block.data(), script_block.size());
}

FilesResults ScriptEditor::CheckEOL()
{
	FilesResults result;	
	const char* ignore = "\x5\xff\r\n";
	uint32_t totalCount = 0;		

	wxProgressDialog dialog("Checking end lines...", wxEmptyString, m_Info.BlockLenght);
	dialog.Show();

	for (size_t number = 0; number < m_Info.Count; ++number)
	{
		wxString name = wxString("Script_Translated") << "_" << number << m_PathRight;
		dialog.Update(number, name);

		std::string path = GetPath(number, true);
		Script script = Moon::File::ReadAllBytes<uint8_t>(path);

		if (script.HaveText())
		{
			SearchResult match;

			for (size_t index = 0; index < script.Count(); ++index)
			{
				std::string_view str = script[index];
				bool good = false;				

				if (m_RomOriginal.Console == console::GBA)
				{
					good = Moon::String::CheckEndLineCRLF(str);
				}
				else
				{
					good = Moon::String::CheckEndLineLF(str);
				}

				if (good)
					continue;					
											
				match.AppendResult(wxString("String ") << index << ": " << std::string(str.substr(0, str.find_first_of(ignore))));
			}

			size_t count = match.GetCount();

			if (count == 0)
				continue;
			
			totalCount += count;
			match.SetTitle(name << " (" << count << (count == 0 ?  " error" : " errors") << ")");
			result.AppendMatch(match);
		}
	}

	result.SetSearchTitle(wxString("EOL Checker (") << totalCount << (totalCount == 1 ? " error" : " errors") << " in " << result.GetCount() << " scripts)");

	return result;
}

FilesResults ScriptEditor::CheckCode()
{
	FilesResults result;	

	wxProgressDialog dialog("Checking code...", wxEmptyString, m_Info.BlockLenght);
	dialog.Show();

	for (size_t number = 0; number < m_Info.Count; ++number)
	{
		wxString name = wxString("Script_Translated") << "_" << number << m_PathRight;
		dialog.Update(number, name);
		
		Script original = Moon::File::ReadAllBytes<uint8_t>(GetPath(number, false));
		Script translated = Moon::File::ReadAllBytes<uint8_t>(GetPath(number, true));

		if (!original.CompareCode(translated))
		{
			SearchResult match;
			match.SetTitle(name << " has code errors");
			result.AppendMatch(match);
		}
	}

	size_t count = result.GetCount();
	std::string script;

	result.SetSearchTitle(wxString("Code Checker (") << (count == 0 ? "none" : std::to_string(count)) << " bad script" << (count == 1 ? "" : "s") << "" << ")");
	dialog.Close();

	return result;
}