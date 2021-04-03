#include "class_script_editor.hpp"

#include <fstream>

#define char_pointer(x) (char*)(&x)

std::vector<Operation> ScriptEditor::s_operations
	{
		{ "nop",0 },
		{ "equ",0 },
		{ "addequ",0 },
		{ "subequ",0 },
		{ "mulequ",0 },
		{ "divequ",0 },
		{ "modequ",0 },
		{ "add",0 },
		{ "sub",0 },
		{ "mul",0 },
		{ "div",0 },
		{ "mod",0 },
		{ "and",0 },
		{ "or",0 },
		{ "inc",0 },
		{ "dec",0 },
		{ "neg",0 },
		{ "not",0 },
		{ "cmp",0 },
		{ "pushm",4 },
		{ "popm",4 },
		{ "dup",0 },
		{ "pop",0 },
		{ "push",4 },
		{ "b",4 },
		{ "blt",4 },
		{ "ble",4 },
		{ "beq",4 },
		{ "bne",4 },
		{ "bge",4 },
		{ "bgt",4 },
		{ "bi",0 },
		{ "end",0 },
		{ "call",4 },
		{ "push",2 },
		{ "push",1 },
		{ "switch",4 }
	};

std::map<int32_t, Function> ScriptEditor::s_functions
{
	{ 0x02, { "setActorPosition" } },
	{ 0x03, { "getActorX" } },
	{ 0x04, { "getActorY" } },
	{ 0x05, { "setActorFacing" } },
	{ 0x06, { "getActorFacing" } },
	{ 0x0D, { "setActorAnim" } },
	{ 0x0F, { "disableActor" } },
	{ 0x13, { "oppositeFacing" } },
	{ 0x14, { "getActorLocation" } },
	{ 0x19, { "playBgm" } },
	{ 0x1A, { "stopBgm" } },
	{ 0x1B, { "playSong" } },
	{ 0x1C, { "stopAllSounds" } },
	{ 0x1D, { "stopBgmFaded" } },
	{ 0x1E, { "talkEnd" } },
	{ 0x1F, { "talkBegin" } },	
	{ 0x20, { "talkBegin2" } },
	{ 0x21, { "talkClose" } },
	{ 0x22, { "talkDisplay" } },
	{ 0x23, { "talkDisplaySlow" } },
	{ 0x24, { "talkDisplayFast" } },
	{ 0x28, { "choose2" } },
	{ 0x29, { "choose3" } }
};

template<typename function>
void for_each_operation(const unsigned char* start, const size_t& len, function f)
{
	uint32_t i = 0;
	std::vector<Operation> operations = ScriptEditor::GetOperations();
	while(i < len)
	{
		uint32_t op_index = start[i];
		uint32_t op_addr = i;
		Operation& operation = operations[op_index];
		++i;
		int32_t operand;

		switch (operation.size)
		{
		case 0:
			operand = 0;
			break;
		case 1:
			operand = *((int8_t*)(start+i));
			break;
		case 2:
			operand = *((int16_t*)(start+i));
		break;
		case 4:
			operand = *((int32_t*)(start+i));
		break;
		default:
			break;
		}

		i += operation.size;		
		f(op_index, operand, op_addr);
	}
}

ScriptEditor::ScriptEditor(const id& id) : Editor(id, L"Script")
{
	std::string test;

	for(int i = 0; i < 0x24; ++i)
	{
		//test += "{ \"" + s_script_operations[i] + "\"," + std::to_string(s_imm_size[i]) + " },\n";
	}

	Moon::File::WriteAllText(std::string("test.txt"), test);

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
		m_PathRight = ".s";
		break;
	case id::MFoMT:
		m_PathRight = ".s";
		break;
	case id::DS:
		m_PathRight = ".s";
		break;
	default:
		break;
	}
}

bool ScriptEditor::Open(uint32_t number)
{
	// std::vector<uint8_t> dataOri = Moon::File::ReadAllBytes<uint8_t>(GetPath(number, false));

	// Script script(dataOri);
		
	// std::vector<uint8_t> data = Moon::File::ReadAllBytes<uint8_t>(GetPath(number, true));

	// m_ScriptOriginal.SetData(dataOri);
	// m_ScriptTranslated.SetData(data);

	// if (!m_ScriptTranslated.HaveText())
	// 	m_ScriptTranslated.SetData(dataOri);

	// m_Original = m_ScriptOriginal.GetText();
	// m_Translated = m_ScriptTranslated.GetText();

	// m_RomTranslated.InputTextWithVariables(m_Translated);
	// m_RomOriginal.InputTextWithVariables(m_Original);

	// m_Index = 0;
	// m_Number = number;
	// m_Count = m_Original.size();

	// m_Opened = true;	

	wxFile file;
	file.Open(GetPath(number, true));

	if( !file.IsOpened() )
		return false;

	file.ReadAll(&m_ScriptDisassembly, wxCSConv(wxFONTENCODING_CP1252));

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
	Moon::File::WriteAllText(GetPath(true), m_ScriptDisassembly.ToStdString(wxCSConv(wxFONTENCODING_CP1252)));
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

	//rom.Seek(m_Info.StartBlock);
	//rom.Read(scriptBlock, m_Info.BlockLenght);	

	uint32_t maxPosition = m_Info.StartBlock + m_Info.BlockLenght;

	wxFileName fn;
	fn.SetPath(m_ScriptDir);
	fn.AppendDir(translated ? L"Translated" : L"Original");

	if(!fn.DirExists())
	{
		fn.Mkdir();
	}	

	//the string is declared here so we don't have to create a new one every i
	std::string script_disassembly;
	//reserve 10kb of text for the disassembly, so we do not have to worry about reallocating the string every push_back, append or operator +
	script_disassembly.reserve(1024*10);

	for (uint32_t i = 0; i < m_Info.Count; ++i)
	{
		std::string path = GetPath(i, translated); 

		Script script;

		rom.Seek(offsets[i]);

		if(rom.ReadUInt32() != SCRIPT_RIFF)
			continue;

		uint32_t riff_size = rom.ReadUInt32();		

		std::vector<uint8_t> bytes = rom.ReadBytes<uint8_t>(offsets[i], riff_size);

		script.SetData(bytes);		

		std::vector<std::string> text = script.GetText();

		script_disassembly += ".section str\n\n";

		for(int str_index = 0; str_index < text.size(); ++str_index) {
			std::string str = text[str_index];
			Moon::String::Replace(str, "\r", "\\r");
			Moon::String::Replace(str, "\n", "\\n");
			Moon::String::Replace(str, "\"", "\\\"");

			script_disassembly.push_back('\t');
			script_disassembly += "string";
			script_disassembly += std::to_string(str_index);
			script_disassembly.push_back(':');
			script_disassembly += " db ";
			script_disassembly.push_back('\"');
			script_disassembly += str;
			script_disassembly.push_back('\"');
			script_disassembly.push_back('\n');
		}

		const unsigned char* start = script.GetData() + 0x18;
		const unsigned char* end = start + *(uint32_t*)(script.GetData() + 0x14);
		const unsigned char* pos = start;
		int count = end - start;

		//ToDo: Find a faster way of searching for labels
		std::vector<uint32_t> labels;

		uint8_t* data = script.GetData();
		const char* strOrJump = (const char*)(data + count + 0x18);
		
		if(memcmp(strOrJump, "JUMP", 4) == 0) {
			script_disassembly.push_back('\n');
			script_disassembly.push_back('\n');
			script_disassembly += ".section jump";
			script_disassembly.push_back('\n');
			script_disassembly.push_back('\n');

			const uint32_t* jumpLenght = (uint32_t*)(strOrJump+4);

			if (*jumpLenght) {
				const uint32_t* jumpCount = jumpLenght + 1;
				const uint32_t* jumpPointers = jumpCount + 1;

				for (uint32_t jumpIndex = 0; jumpIndex < *jumpCount; ++jumpIndex) {
					script_disassembly += "Table";
					script_disassembly += std::to_string(jumpIndex);
					script_disassembly.push_back(':');
					script_disassembly.push_back('\n');

					const uint32_t* caseCount = (uint32_t*)(strOrJump + 12 + jumpPointers[jumpIndex]);
					const int32_t* defaultCase = (int32_t*)(caseCount+1);

					if (*defaultCase != -1) {
						script_disassembly += "\tdefault:\n";
						script_disassembly += "\t\tb LAB_";
						script_disassembly += Moon::BitConverter::ToHexString(*defaultCase);
						script_disassembly += "\n";

						labels.push_back(*defaultCase);
					}

					const std::pair<uint32_t, uint32_t>* value_offset = (std::pair<uint32_t, uint32_t>*)(defaultCase+1);

					for (uint32_t caseIndex = 0; caseIndex < *caseCount; ++caseIndex) {
						script_disassembly += "\tcase ";
						script_disassembly += std::to_string(value_offset[caseIndex].first);
						script_disassembly += ":\n";
						script_disassembly += "\t\tb LAB_";
						script_disassembly += Moon::BitConverter::ToHexString(value_offset[caseIndex].second);
						script_disassembly += "\n";						

						labels.push_back(value_offset[caseIndex].second);
					}
				}

			}
		}

		script_disassembly.push_back('\n');
		script_disassembly.push_back('\n');
		script_disassembly += ".section scr";
		script_disassembly.push_back('\n');
		script_disassembly.push_back('\n');		

		for_each_operation(start, count, [&labels](const OpCode& op, const int32_t operand, const uint32_t& addr) {
			if(op >= OpCode::B && op <= OpCode::BGT) {
				labels.push_back(operand);
			}
		});

		for_each_operation(start, count, [&labels, &script_disassembly](const OpCode& op, const int32_t operand, const uint32_t& addr) {
			if(std::find(labels.begin(), labels.end(), addr) != labels.end()) {
				script_disassembly += "LAB_";
				script_disassembly += Moon::BitConverter::ToHexString(addr);
				script_disassembly += ":\n";
			}

			Operation& operation = s_operations[op];

			script_disassembly += operation.name;
			script_disassembly.push_back(' ');

			if(operation.size > 0) {
				if (op >= OpCode::B && op <= OpCode::BGT) {
					script_disassembly += "LAB_";
					script_disassembly += Moon::BitConverter::ToHexString(operand);
				} else if(op == OpCode::CALL) {
					auto it = s_functions.find(operand);

					if(it == s_functions.end()) {
						std::string operand_string = Moon::BitConverter::ToHexString(operand);

						while(operand_string.size() > 1 && operand_string[0] == '0') {
							operand_string.erase(operand_string.begin());
						}

						std::string name = "function"+operand_string;						
						s_functions.insert({ operand, name });
						script_disassembly += name;
					}
					else {
						script_disassembly += it->second.name;
					}
				}
				else if (op == OpCode::SWITCH) {
					script_disassembly += "Table";
					script_disassembly += std::to_string(operand);					
				}
				else {
					script_disassembly += std::to_string(operand);
				}
			}

			script_disassembly += "\n";			
		});

		Moon::File::WriteAllText(path, script_disassembly);

		script_disassembly.clear();
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

void ScriptEditor::InsertOnly(const void* data, uint32_t size, uint32_t offset)
{
	m_RomTranslated.Seek(offset);
	m_RomTranslated.Write(data, size);
	m_RomTranslated.Flush();
}

void ScriptEditor::InsertSmaller(const void* data, uint32_t size, uint32_t offset, uint32_t oldSize)
{	
	uint8_t* _data = new uint8_t[oldSize];
	memset(_data, 0x00, oldSize);
	memcpy(_data, data, size);

	m_RomTranslated.Seek(offset);
	m_RomTranslated.Write(_data, oldSize);
	m_RomTranslated.Flush();

	delete[] _data;
}

bool ScriptEditor::InsertVerify(const void* data, uint32_t size, uint32_t offset)
{
	bool free = IsFreeSpace(offset, size);

	if (free) {
		InsertOnly(data, size, offset);
	}

	return free;
}

bool ScriptEditor::InsertLarger(const void* data, uint32_t size, uint32_t oldOffset, uint32_t oldSize)
{
	size_t result = size - oldSize;
	size_t endOffset = oldOffset + oldSize;

	bool flag = IsFreeSpace(endOffset, result);

	if (flag) {
		m_RomTranslated.Seek(oldOffset);
		m_RomTranslated.Write(data, size);
		m_RomTranslated.Flush();
	}

	return flag;
}

void ScriptEditor::InsertMove(const void* data, uint32_t size, uint32_t oldOffset, uint32_t oldSize, uint32_t newOffset)
{
	SetOffset(newOffset);
	EraseScript(oldOffset, oldSize);
	InsertOnly(data, size, newOffset);
}

bool ScriptEditor::InsertFind(const void* data, uint32_t size, uint32_t oldOffset, uint32_t oldSize)
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
		freeSpace = Finder::FindFilledAlingBlock(0x00, size, scriptBlock, m_Info.BlockLenght, freeSpace, 4);

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

	if (flag) {
		InsertMove(data, size, oldOffset, oldSize, freeSpace + m_Info.StartBlock);
	}

	return flag;
}

void ScriptEditor::InsertFile()
{	
	uint32_t oldOffset = GetOffset(true);
	uint32_t oldSize = ScriptSize(oldOffset, true);	

	std::string data;
	std::stringstream stream;
	std::string output_msg = Compile(stream);

	if(!output_msg.empty()) {
		wxMessageBox(output_msg, "Error", wxICON_ERROR);
		return;
	}

	uint32_t newSize = data.size();

	if (newSize <= oldSize) {
		if (oldSize != std::string::npos) {
			InsertSmaller(data.c_str(), data.size(), oldOffset, oldSize);
			return;// ScriptFlags::INSERT_LESS;
		}

		if (InsertVerify(data.c_str(), data.size(), oldOffset)) {
			return;// ScriptFlags::INSERT_VERIFY;
		}
	}
	else if (newSize > oldSize) {
		if (InsertLarger(data.c_str(), data.size(), oldOffset, oldSize)) {
			return; // ScriptFlags::INSERT_LARGER;
		}
	}	 

	if (InsertFind(data.c_str(), data.size(), oldOffset, oldSize)) {
		return; // ScriptFlags::INSERT_FIND;
	}

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

std::string ScriptEditor::Compile(std::ostream& stream) {	
	//std::stringstream str;
	//str << "STR ";

	//ToDo: Show line in error/warning and more information about the error

	stream << "RIFF";
	stream.write("\0\0\0", 4);
	stream << "SCR CODE";
	stream.write("\0\0\0", 4);
	stream.write("\0\0\0", 4);

	size_t src_start = stream.tellp();

	auto lines = Moon::String::GetLines(m_ScriptDisassembly.ToStdString(), false);

	std::vector<std::string> jumpTables;

	auto jump_it = std::find(lines.begin(), lines.end(), ".section jump");

	if (jump_it != lines.end()) {
		
	}

	auto scr_it = std::find(lines.begin(), lines.end(), ".section scr");

	if( scr_it == lines.end() ) {
		return "error: undefined scr section";
	}

	std::string labels;
	std::vector<std::pair<uint32_t, size_t>> jumps;

	for(int i = (scr_it - lines.begin())+1; i < lines.size(); ++i) { 
		std::string line = lines[i];
		std::string operation = line.substr(0, lines[i].find(' '));
		bool written = false;
		bool end = false;

		for(uint32_t op = 0; op < s_operations.size(); ++op) {
			if(s_operations[op].name == operation) {
				//push     1
				//to
				//     1
				//and then to
				//1
				line.erase(0, operation.size());
				while(line.size() && line[0] == ' ') {
					line.erase(line.begin());
				}

				if(!line.size() && s_operations[op].size) {
					return "error: missing instruction argument";
				}

				if(line.size() && !s_operations[op].size) {
					return "error: unexpected argument";
				}

				uint8_t op_code = op;
				int32_t argument;
				
				if(s_operations[op].size) {
					std::string arg;

					if(op == OpCode::CALL || op == OpCode::SWITCH) {
						arg = line.substr(0, line.find(' '));
					} else {
						if(line[0] == '-') {
							line.erase(line.begin());
							arg.push_back('-');
						}
						for(const char& c : line) {
							if(c == ' ') {
								break;
							}
							else if(!isdigit(c)) {
								return "error: unexpected token '" + std::to_string(c) + "' in argument";
							} else {
								arg.push_back(c);
							}
						}
					}

					line.erase(0, arg.size());

					if(line.size()) {
						return "error: too many arguments";
					}

					if(op == OpCode::CALL) {
						int fn = -1;
						for(const auto& pair : s_functions)
							if(pair.second.name == arg) {
								fn = pair.first;
								break;
							}
						if(fn == -1) {
							return "error: unexpected token '" + arg + "' in argument";
						}
						argument = fn;
					}
					else if (op == OpCode::SWITCH) {

					}
					else {

						int arg_max = 0;
						int arg_min = 0;
						int arg_max_digits = 0;
						int arg_size;

						bool auto_size = false;

						//push has 32, 16 and 8-bits signed arguments
						if(operation == "push") {
							arg_size = 4;
							auto_size = true;
						} else {
							arg_size = s_operations[op].size;
						}

						switch (s_operations[op].size)
						{
						case 1:
							arg_max = INT8_MAX;
							arg_min = INT8_MIN;
							arg_max_digits = 3;
							break;
						case 2:
							arg_max = INT16_MAX;
							arg_min = INT16_MIN;
							arg_max_digits = 5;
							break;
						case 4:
							arg_max = INT32_MAX;
							arg_min = INT32_MIN;
							arg_max_digits = 10;
							break;
						default:
							break;
						}					

						if(arg.size() > arg_max_digits) {
							return "error: argument overflow";
						}					
						
						auto temp_argument = std::stoll(arg);

						std::cout << "Temp argument: " << temp_argument << "\n";

						if(temp_argument > arg_max || temp_argument < arg_min) {
							return "error: argument overflow";
						}

						if(auto_size) {
							if(temp_argument >= INT8_MIN && temp_argument <= INT8_MAX) {
								arg_size = 1;
							} else if(temp_argument >= INT16_MIN && temp_argument <= INT16_MAX) {
								arg_size = 2;
							}
							//else arg_size = 32 by default
						}

						if(s_operations[op].name == "push") {
							switch (arg_size)
							{
							case 1:
								//push8
								op_code = 0x23;
								break;
							case 2:
								//push16
								op_code = 0x22;
								break;
							case 4:
								//push
								op_code = 0x17;
								break;
							default:
								break;
							}
						}

						argument = temp_argument;
					}
				}

				if(op == op == OpCode::END) {
					end = true;
				}

				stream.write(char_pointer(op_code), 1);

				if(s_operations[op_code].size) {
					switch (s_operations[op_code].size)
					{
					case 1:
						{
							uint8_t arg = argument;
							stream.write(char_pointer(arg), 1);
						}
						break;
					case 2:
						{
							uint16_t arg = argument;
							stream.write(char_pointer(arg), 2);
						}
						break;
					case 4:
						{							
							stream.write(char_pointer(argument), 4);
						}
						break;
					default:
						break;
					}
				}

				written = true;
				break;
			}
		}

		if(end) {
			break;
		}
		if(written) {
			continue;
		}

		return "error: undefined instruction '" + operation + "'";
	}

	size_t src_end = stream.tellp();
	uint32_t code_lenght = src_end - src_start;
	uint32_t src_lenght = code_lenght+4;

	stream.seekp(0x10);
	stream.write(char_pointer(src_lenght), 4);
	stream.write(char_pointer(code_lenght), 4);

	stream.seekp(src_end);

//String compilation

	stream << "STR ";	

	auto str_it = std::find(lines.begin(), lines.end(), ".section str");

	if( str_it == lines.end() ) {
		uint32_t len = 4;
		stream.write(char_pointer(len), 4);
		stream.write("\0\0\0", 4);

		return "";
	}

	std::vector<uint32_t> pointers;
	std::string strings;

	for(int i = (str_it - lines.begin())+1; i < lines.size(); ++i) { 
		std::string& line = lines[i];

		// size_t dots = line.find(":");
		// std::string name = line.substr(dots);

		// while((*name.begin() == ' ' || *name.begin() == '\t') && name.size()) {
		// 	name.erase(name.begin());
		// }

		// for(const char& c : name) {
		// 	if(c >= 'a'&& c <= 'z') {
		// 		break;
		// 	} else if(c >= 'A' && c <= 'Z') {
		// 		break;
		// 	} else if(c >= '0' && c <= '9') {
		// 		break;
		// 	}

		// 	return "error: undefined token '" + std::to_string(c) + "' in string name '" + '';
		// }

		size_t start = line.find("\"");

		if(start == std::string::npos) {
			continue;
		}

		size_t end = line.find("\"", start+1);		

		if(end == std::string::npos) {
			return "error: missing matching \"";
		}

		pointers.push_back(strings.size());

		std::string str = line.substr(start+1, (end-start)-1);
		Moon::String::Replace(str, "\\n", "\n");
		Moon::String::Replace(str, "\\r", "\r");

		strings.append(str);
		strings.push_back('\0');
	}

	if(!pointers.size()) {
		uint32_t len = 4;
		stream.write(char_pointer(len), 4);
		stream.write("\0\0\0", 4);
		return "";
	}

	uint32_t str_len = 0;
	uint32_t str_count = pointers.size();

	str_len += str_count*4;
	str_len += 4;
	str_len += strings.size();

	stream.write(char_pointer(str_len), 4);
	stream.write(char_pointer(str_count), 4);

	for(const uint32_t& pointer : pointers) {
		stream.write(char_pointer(pointer), 4);
	}

	stream.write(strings.c_str(), strings.size());

	uint32_t riff_size = stream.tellp();

	stream.seekp(4);
	stream.write(char_pointer(riff_size), 4);

	return "";
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