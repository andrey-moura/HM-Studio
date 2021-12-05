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
		f((OpCode)op_index, operand, op_addr);
	}
}

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

	// for(int i = 0; i < m_Info.Count; ++i) {		
	// 	std::string original;
	// 	Moon::File::ReadAllText("Original/Script_Original_"+std::to_string(i)+".msf", original);

	// 	std::string translated;
	// 	Moon::File::ReadAllText("Script/script_"+std::to_string(i)+".bin", translated);

	// 	if(original.size() == translated.size()) {
	// 		size_t diff = std::string::npos;
	// 		for(int i = 0; i < original.size(); ++i) {
	// 			if(original[i] != translated[i]) {
	// 				diff = i;
	// 				break;
	// 			}
	// 		}
	// 		if(diff == std::string::npos) {				
	// 			wxRemoveFile("Script/script_"+std::to_string(i)+".bin");				
	// 		} else {
	// 			std::cout << "Script " << i << ": difference at byte " << diff << "\n";
	// 		}
	// 	} else {
	// 		std::cout << "Script " << i << ": different size\n";
	// 	}
	// }

	// const Moon::Hacking::Table& table = m_RomTranslated.GetTable();

	// for(int i = 0; i < m_Info.Count; ++i) {
	// 	std::string script;
	// 	std::string path = GetPath(i, true);
	// 	Moon::File::ReadAllText(path, script);

	// 	bool ignore = false;
	// 	for(char& c : script) {
	// 		if((unsigned char)c == 0x81 || (unsigned char)c == 0xff) {
	// 			ignore = true;
	// 		} else {
	// 			if(!ignore) {
	// 				table.Input(c);
	// 			}
	// 			ignore = false;
	// 		}			
	// 	}		

	// 	Moon::File::WriteAllText(path, script);
	// }
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

	m_Index = 0;
	m_Number = number;
	// m_Count = m_Original.size();

	// m_Opened = true;		

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
	std::vector<uint8_t> bytes = m_RomTranslated.ReadBytes<uint8_t>(offset, size);	
	for (const uint8_t& b : bytes) {
		if (b != 0x00 && b != 0xff) {
			return false;
		}
	}

	return true;
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

		if(text.size()) {
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
						script_disassembly += "\tdefault: LAB_";						
						script_disassembly += Moon::BitConverter::ToHexString(*defaultCase);
						script_disassembly += "\n";

						labels.push_back(*defaultCase);
					}

					const std::pair<int32_t, uint32_t>* value_offset = (std::pair<int32_t, uint32_t>*)(defaultCase+1);

					for (uint32_t caseIndex = 0; caseIndex < *caseCount; ++caseIndex) {
						script_disassembly += "\tcase ";
						script_disassembly += std::to_string(value_offset[caseIndex].first);
						script_disassembly.push_back(':');
						script_disassembly += " LAB_";
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

		for_each_operation(start, count, [&labels, &script_disassembly, &i](const OpCode& op, const int32_t operand, const uint32_t& addr) {
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

						while(operand_string.starts_with('0')) {
							operand_string.erase(operand_string.begin());
						}
						
						script_disassembly += "0x";
						script_disassembly += operand_string;
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
	std::vector<char> bytes = m_RomTranslated.ReadBytes<char>(m_Info.StartBlock, m_Info.BlockLenght);

	std::string buffer(size+8, '\0');

	auto pos = std::search(bytes.begin(), bytes.end(), buffer.begin(), buffer.end());
	uint32_t freespace;

	if (pos == bytes.end()) {
		memset(buffer.data(), 0xff, size+8);
		bytes = m_RomTranslated.ReadBytes<char>(m_RomTranslated.m_FreeSpace, m_RomTranslated.Length() - m_RomTranslated.m_FreeSpace);
		pos = std::search(bytes.begin(), bytes.end(), buffer.begin(), buffer.end());

		if (pos == bytes.end()) {
			return false;
		}
		else {
			freespace = (pos - bytes.begin()) + m_RomTranslated.m_FreeSpace;
		}
	}
	else {
		freespace = (pos - bytes.begin()) + m_Info.StartBlock;
	}

	InsertMove(data, size, oldOffset, oldSize, freespace+4);
	return true;
}

void ScriptEditor::InsertFile(const std::string& file)
{	
	uint32_t oldOffset = GetOffset(true);
	uint32_t oldSize = ScriptSize(oldOffset, true);	
	
	std::stringstream stream;
	std::string output_msg = Compile(stream, file);

	if(!output_msg.empty()) {
		wxMessageBox(output_msg, "Error", wxICON_ERROR);
		return;
	}

	std::string data = stream.str();	

	Moon::File::WriteAllBytes(std::string("test.obj"), data.c_str(), data.size());

	//return;

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
 
	for (size_t script_number = 0; script_number < m_Info.Count; ++script_number)
	{		
		std::string file = Moon::File::ReadAllText(GetPath(script_number, true));		
		std::stringstream stream;		
		std::string output_msg = Compile(stream, file);

		if(!output_msg.empty()) {
			wxMessageBox(wxString(L"script ") << script_number << ": " << output_msg);
			return;
		}

		std::string data = stream.str();		
		//Moon::File::WriteAllText("Script/script_"+std::to_string(script_number)+".bin", data);

		scripts_pointers.push_back(script_block.size()+m_Info.StartBlock);
		script_block += data;

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

std::string ScriptEditor::Compile(std::ostream& output, const std::string& input) {
	//std::stringoutput str;
	//str << "STR ";

	//ToDo: Show line in error/warning and more information about the error

	output << "RIFF";
	output.write("\0\0\0", 4);
	output << "SCR CODE";
	output.write("\0\0\0", 4);
	output.write("\0\0\0", 4);

	size_t src_start = output.tellp();

	auto lines = Moon::String::GetLines(input, false);	

	auto jump_it = std::find(lines.begin(), lines.end(), ".section jump");
	std::vector<std::pair<std::string, std::vector<std::pair<uint32_t, std::string>>>> jumpTables;
	std::vector<std::string> defaultLabels;

	if (jump_it != lines.end()) {				
		for (int i = (jump_it - lines.begin()) + 1; i < lines.size(); ++i) {
			std::string defaultLabel;
			std::vector<std::pair<uint32_t, std::string>> cases;
			std::string line = lines[i];

			if(line.empty()) {
				continue;
			}

			if(line.starts_with(".section")) {
				break;
			}
			
			size_t label_pos = line.find(':');

			if (label_pos != std::string::npos) {
				std::string tableLabel = line.substr(0, label_pos);

				while(tableLabel.starts_with(' ') || tableLabel.starts_with('\t')) {
					tableLabel.erase(0, 1);
				}

				++i;

				while(i < lines.size()) {
					line = lines[i];

					if(line.empty()) {
						continue;
					}

					while(line.starts_with(' ') || line.starts_with('\t')) {
						line.erase(0, 1);
					}
					//case x:
					if(line.starts_with("case")) {						
						line.erase(0, 4);
						while(line.starts_with(' ') || line.starts_with('\t')) {
							line.erase(0, 1);
						}

						size_t ncasePos = line.find(':');
						if(ncasePos == std::string::npos) {
							return "error: missing case terminator ':'";
						}

						std::string ncaseStr = line.substr(0, ncasePos);
						bool negative = false;
						if(ncaseStr.starts_with('-')) {
							negative = true;
							ncaseStr.erase(ncaseStr.begin());
						}
						for(const char& c : ncaseStr) {
							if(!isdigit(c)) {
								return "error: unexpected token '" + std::string(&c, 1) + "' in case value";
							}
						}

						int32_t ncase = std::stoi(ncaseStr);
						if(negative) {
							ncase *= -1;
						}

						line.erase(0, ncasePos+1);
						while(line.starts_with(' ') || line.starts_with('\t')) {
							line.erase(0, 1);
						}							

						std::string label;

						for(const char& c : line) {
							if(c == ' ' || c == '\t') {
								break;
							}

							label.push_back(c);
						}

						cases.push_back({ ncase, label });
					} else if(line.starts_with("default:")) {
						line.erase(0, 8);
						while(line.starts_with(' ') || line.starts_with('\t')) {
							line.erase(line.begin());
						}
						for(const char& c : line) {
							if(c == ' ' || c == '\t') {
								break;
							}

							defaultLabel.push_back(c);
						}
					} else {
						//process last line again
						--i;
						break;
					}					
					++i;
				}
				defaultLabels.push_back(defaultLabel);
				jumpTables.push_back({tableLabel, cases});
			}
		}
	}

	auto scr_it = std::find(lines.begin(), lines.end(), ".section scr");

	if (scr_it == lines.end()) {
		return "error: undefined scr section";
	}

	std::vector<std::pair<std::string, uint32_t>> labels_addrs;
	std::vector<std::pair<std::string, uint32_t>> labels_jumps;

	for (int i = (scr_it - lines.begin()) + 1; i < lines.size(); ++i) {
		std::string line = lines[i];

		while (line.starts_with(' ') || line.ends_with('\t')) {
			line.erase(0, 1);
		}

		if (line.empty()) {
			continue;
		}

		size_t label_pos = line.find(':');

		if (label_pos != std::string::npos) {
			std::string label = line.substr(0, label_pos);

			for (const char& c : label) {
				if (!isalnum(c) && c != '_') {
					return "error: unexpected token '" + std::string(&c, 1) + "' in label name";
				}
			}

			line.erase(0, label.size() + 1);

			if (!line.empty()) {
				return "error: syntax error";
			}

			for (int label_index = 0; label_index < labels_addrs.size(); ++label_index) {
				if (labels_addrs[label_index].first == label) {
					return "error: redefinition of label '" + label + "'";
				}
			}

			labels_addrs.push_back(std::pair<std::string, uint32_t>(label, (uint32_t)output.tellp()-0x18));
			continue;
		}

		std::string operation = line.substr(0, lines[i].find(' '));
		bool written = false;		

		for (uint32_t op = 0; op < s_operations.size(); ++op) {
			if (s_operations[op].name == operation) {
				//push     1
				//to
				//     1
				//and then to
				//1
				line.erase(0, operation.size());
				while (line.starts_with(' ')) {
					line.erase(line.begin());
				}

				if (!line.size() && s_operations[op].size) {
					return "error: missing instruction argument";
				}

				if (line.size() && !s_operations[op].size) {
					return "error: unexpected argument";
				}

				uint8_t op_code = op;
				int32_t argument;

				if (s_operations[op].size) {
					std::string arg;

					if (op == OpCode::CALL || op == OpCode::SWITCH || (op >= OpCode::B && op <= OpCode::BGT)) {
						arg = line.substr(0, line.find(' '));
					}
					else {
						if (line.starts_with('-')) {
							line.erase(line.begin());
							arg.push_back('-');
						}
						for (const char& c : line) {
							if (c == ' ') {
								break;
							}
							else if (!isdigit(c)) {
								return "error: unexpected token '" + std::string(c, 1) + "' in argument";
							}
							else {
								arg.push_back(c);
							}
						}
					}

					line.erase(0, arg.size());

					if (line.size()) {
						return "error: too many arguments";
					}

					if (op == OpCode::CALL) {
						if(arg.starts_with("0x")) {
							arg.erase(0, 2);
							for(const char& c : arg) {
								if(!isxdigit(c)) {
									return "error: unexpected token '" + std::string(c, 1) + "' in argument";
								}
							}
							argument = std::stoi(arg, nullptr, 16);
						} else if(isdigit(arg[0])) {
							for(const char& c : arg) {
								if(!isdigit(c)) {
									return "error: unexpected token '" + std::string(c, 1) + "' in argument";
								}
							}
							argument = std::stoi(arg);
						} else {
							int fn = -1;						
							for (const auto& pair : s_functions) {
								if (pair.second.name == arg) {
									fn = pair.first;
									break;
								}
							}
							if (fn == -1) {
								return "error: unexpected token '" + arg + "' in argument";
							}
							argument = fn;	
						}						
					}
					else if (op == OpCode::SWITCH) {
						argument = -1;
						for(size_t i = 0; auto& table : jumpTables) {
							if(table.first == arg) {
								argument = i;
								break;
							}
							++i;
						}
						if(argument == -1) {
							return "error: undefined reference to jump table '" + arg + "'";
						}
					}
					else if ((op >= OpCode::B && op <= OpCode::BGT)) {
						int label = -1;
						for (int label_index = 0; label_index < labels_addrs.size(); ++label_index) {
							if (labels_addrs[label_index].first == arg) {
								label = label_index;
							}
						}
						if (label == -1) {
							labels_jumps.push_back(std::pair<std::string, uint32_t>(arg, output.tellp()));
							argument = 0;
						}
						else {
							argument = labels_addrs[label].second;
						}
					}
					else {

						int arg_max = 0;
						int arg_min = 0;
						int arg_max_digits = 0;
						int arg_size;

						bool auto_size = false;

						//push has 32, 16 and 8-bits signed arguments
						if (operation == "push") {
							arg_size = 4;
							auto_size = true;
						}
						else {
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

						if (arg.size() > arg_max_digits) {
							return "error: argument overflow";
						}

						auto temp_argument = std::stoll(arg);						

						if (temp_argument > arg_max || temp_argument < arg_min) {
							return "error: argument overflow";
						}

						if (auto_size) {
							if (temp_argument >= INT8_MIN && temp_argument <= INT8_MAX) {
								arg_size = 1;
							}
							else if (temp_argument >= INT16_MIN && temp_argument <= INT16_MAX) {
								arg_size = 2;
							}
							//else arg_size = 32 by default
						}

						if (s_operations[op].name == "push") {
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

				output.write(char_pointer(op_code), 1);

				if (s_operations[op_code].size) {
					switch (s_operations[op_code].size)
					{
					case 1:
					{
						uint8_t arg = argument;
						output.write(char_pointer(arg), 1);
					}
					break;
					case 2:
					{
						uint16_t arg = argument;
						output.write(char_pointer(arg), 2);
					}
					break;
					case 4:
					{
						output.write(char_pointer(argument), 4);
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
		
		if (written) {
			continue;
		}

		return "error: undefined instruction '" + operation + "'";
	}

	size_t src_end = output.tellp();

	for (auto& labels_jump : labels_jumps) {
		output.seekp(labels_jump.second + 1);

		bool written = false;

		for (int label_index = 0; label_index < labels_addrs.size(); ++label_index) {
			if (labels_addrs[label_index].first == labels_jump.first) {
				output.write(char_pointer(labels_addrs[label_index].second), 4);
				written = true;
				break;
			}
		}

		if (!written) {
			return "error: undefined reference to label '" + labels_jump.first + "'";
		}
	}	

	output.seekp(src_end);

	while (src_end % 4 != 0) {
		++src_end;
		output.write("\0", 1);
	}	

	uint32_t code_lenght = src_end - src_start;
	uint32_t src_lenght = code_lenght + 4;

	output.seekp(0x10);
	output.write(char_pointer(src_lenght), 4);
	output.write(char_pointer(code_lenght), 4);

	output.seekp(src_end);

	if(jumpTables.size()) {
		//jump compilation
		uint32_t jumpStart = output.tellp();
		output << "JUMP";
		output.write("\0\0\0\0", 4);
		uint32_t jumpTablesCount = jumpTables.size();
		output.write(char_pointer(jumpTablesCount), 4);

		//jump pointers
		//one word per table
		uint32_t lastAddr = (uint32_t)(jumpTables.size()*4);

		for(auto& table : jumpTables) {
			output.write(char_pointer(lastAddr), 4);
			lastAddr += 4; //case count
			lastAddr += 4; //default label
			lastAddr += table.second.size()*8;
		}	

		for(size_t i = 0; auto& table : jumpTables) {		
			uint32_t caseCount = table.second.size();
			output.write(char_pointer(caseCount), 4);

			int32_t defaultCase = -1;

			if(!defaultLabels[i].empty()) {
				for(auto& pair : labels_addrs) {
					if(pair.first == defaultLabels[i]) {
						defaultCase = pair.second;
						break;
					}
				}

				if(defaultCase == -1) {
					return "error: undefined reference to label '" + defaultLabels[i] + "'";
				}
			}

			output.write(char_pointer(defaultCase), 4);
			for(auto& icase : table.second) {
				output.write(char_pointer(icase.first), 4);

				for(auto& pair : labels_addrs) {
					if(pair.first == icase.second) {
						output.write(char_pointer(pair.second), 4);
						break;
					}
				}
			}

			++i;
		}
		size_t jumpEnd = output.tellp();
		output.seekp(jumpStart+4);

		uint32_t jumpLenght = (jumpEnd-jumpStart)-8; //-JUMP & -JUMP LENGHT
		output.write(char_pointer(jumpLenght), 4);
		output.seekp(jumpEnd);
	}

	//String compilation	

	output << "STR ";

	auto str_it = std::find(lines.begin(), lines.end(), ".section str");

	if (str_it != lines.end()) {
		std::vector<uint32_t> pointers;
		std::string strings;

		for (int i = (str_it - lines.begin()) + 1; i < lines.size(); ++i) {
			std::string& line = lines[i];

			size_t start = line.find("\"");

			if (start == std::string::npos) {
				continue;
			}

			++start;

			std::string str;
			bool escape = false;
			while(start < line.size()) {
				switch (line[start])
				{
				case '\\':
					escape = true;
					break;
				case '\"':
					if(escape) {
						escape = false;
						str.push_back('"');
					} else {
						break;
					}
					break;
				case 'n':
					if(escape) {
						escape = false;
						str.push_back('\n');
					} else {
						str.push_back('n');
					}
					break;
				case 'r':
					if(escape) {
						escape = false;
						str.push_back('\r');
					} else {
						str.push_back('r');
					}
					break;
				default:
					if(escape) {
						return "error: undefined escape sequence '" + std::string(line[start], 1) + "'";
					} else {
						escape = false;
						str.push_back(line[start]);
					}
					break;
				}
				++start;
			}

			pointers.push_back(strings.size());

			Moon::String::Replace(str, "\\n", "\n");
			Moon::String::Replace(str, "\\r", "\r");

			strings.append(str);
			strings.push_back('\0');
		}

		if (pointers.size()) {
			uint32_t str_len = 0;
			uint32_t str_count = pointers.size();

			str_len += str_count * 4;
			str_len += 4;
			str_len += strings.size();

			output.write(char_pointer(str_len), 4);
			output.write(char_pointer(str_count), 4);

			for (const uint32_t& pointer : pointers) {
				output.write(char_pointer(pointer), 4);
			}

			output.write(strings.c_str(), strings.size());					
		} else {
			uint32_t len = 4;
			output.write(char_pointer(len), 4);
			output.write("\0\0\0", 4);
		}		
	} else {
		uint32_t len = 4;
		output.write(char_pointer(len), 4);
		output.write("\0\0\0", 4);
	}	

	uint32_t riff_size = output.tellp();

	output.seekp(4);
	output.write(char_pointer(riff_size), 4);

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