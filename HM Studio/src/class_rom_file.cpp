#include "class_rom_file.hpp"

RomFile::RomFile(id i, bool translated) : wxFile()
{
	State = translated ? "Translated" : "Original";		
	Id = i;	

	switch (i)
	{
	case id::FoMT:			
		Name = "FoMT";
		Console = console::GBA;		
		break;
	case id::MFoMT:		
		Name = "MFoMT";
		Console = console::GBA;
		m_End = 0x9D98C8;
		m_FreeSpace = 0x626738;
		break;
	case id::DS:		
		Name = "DS";
		Console = console::DS;
		break;
	default:
		break;
	}		

	wxFileName path = wxFileName(wxStandardPaths::Get().GetExecutablePath());
	path.AppendDir("HM Studio");
	path.AppendDir(Name);

	m_HomeDir = path.GetPath().ToStdString();	

	path.AppendDir("Rom");
	path.SetName("Rom_" + State);

	switch (Console)
	{
	case console::GBA:
		path.SetExt("gba");
		m_EOL = 0x0d0a;
		m_EolString = "\r\n";
		m_MaxChars = 28;
		m_PointerIndex = 0x08000000;
		break;
	case console::DS:
		path.SetExt("nds");
		m_EOL = 0x0a;
		m_EolString = "\n";
		m_MaxChars = 30;
		m_PointerIndex = 0x02000000;
		break;
	default:
		break;
	}

	if (!path.DirExists())
		path.Mkdir(511, wxPATH_MKDIR_FULL);

	Path = path.GetFullPath();	
	this->Open(Path, wxFile::read_write);

	m_Table.Open(GetTablePath());
	SetupVars();
}

std::string RomFile::GetTablePath()
{
	wxFileName fileName = wxFileName(Path);
	fileName.RemoveLastDir();
	fileName.AppendDir("Table");
	fileName.SetName(State);
	fileName.SetExt("tbl");

	return fileName.GetFullPath().ToStdString();
}

void RomFile::SetupVars()
{
	if (Console == console::GBA)
	{
		m_VarTable.SetVarSize(12);

		m_VarTable.Reserve(6);
		m_VarTable.Append('!', "<PlayerName>");
		m_VarTable.Append('$', "<Ranch-Name>");
		m_VarTable.Append('%', "<AnimalName>");
		m_VarTable.Append(')', "<CustomName>");
		m_VarTable.Append('*', "<Child-Name>");
		m_VarTable.Append('"', "<Horse-Name>");
	}
	else if (Console == console::DS)
	{
		m_VarTable.SetVarSize(8);

		m_VarTable.Reserve(1);
		m_VarTable.Append('$', "<Player>");		
	}
}

void RomFile::InputTextWithVariables(std::vector<std::string>& text)
{
	m_VarTable.Input(text, m_Table);

	for (size_t i = 0; i < text.size(); ++i)
	{		
		Moon::String::Replace(text[i], "\x05\x0c", "\x05\x0c" + m_EolString);
	}	
}

void RomFile::OutputTextWithVariables(std::vector<std::string>& text)
{
	m_VarTable.Output(text, m_Table);

	for (size_t i = 0; i < text.size(); ++i)
	{
		Moon::String::Replace(text[i], "\x05\x0c" + m_EolString, "\x05\x0c");
	}
}

void RomFile::ReplaceWideChars(wxString& text)
{
	const wchar_t* txt = text.wc_str();

	if (Console == console::GBA)
	{
		std::vector<std::pair<wchar_t, char>> table;
		table.reserve(8);
		table.push_back(std::pair<wchar_t, char>(0x2190, 0xa9)); //←
		table.push_back(std::pair<wchar_t, char>(0x2191, 0xaa)); //↑
		table.push_back(std::pair<wchar_t, char>(0x2192, 0xa8)); //→
		table.push_back(std::pair<wchar_t, char>(0x2193, 0xab)); //↓
		table.push_back(std::pair<wchar_t, char>(0x2605, 0x9a)); //★
		table.push_back(std::pair<wchar_t, char>(0x2606, 0x99)); //☆
		table.push_back(std::pair<wchar_t, char>(0x2666, 0x9F)); //♦
		table.push_back(std::pair<wchar_t, char>(0x2022, 0x45)); //•
		
		wxString replace(1, '0');

		for (auto& tbl : table)
		{
			tbl.second = m_Table[tbl.second];

			replace = tbl.second; //The char need to be converted to wxString first

			size_t pos = text.find(tbl.first);

			while (pos != std::string::npos)
			{
				text[pos++] = 0x81;
				text.insert(pos++, 1, replace[0]);
				pos = text.find(tbl.first, pos);
			}
		}
	}	
}

wxString RomFile::ReplaceWideChars(std::string& text)
{
	wxString ret = text;

	const wchar_t* txt = ret.wc_str();

	if (Console == console::GBA)
	{
		std::vector<std::pair<wchar_t, char>> table;
		table.reserve(8);
		table.push_back(std::pair<wchar_t, char>(0x2190, 0xa9)); //←
		table.push_back(std::pair<wchar_t, char>(0x2191, 0xaa)); //↑
		table.push_back(std::pair<wchar_t, char>(0x2192, 0xa8)); //→
		table.push_back(std::pair<wchar_t, char>(0x2193, 0xab)); //↓
		table.push_back(std::pair<wchar_t, char>(0x2605, 0x9a)); //★
		table.push_back(std::pair<wchar_t, char>(0x2606, 0x99)); //☆
		table.push_back(std::pair<wchar_t, char>(0x2666, 0x9F)); //♦
		table.push_back(std::pair<wchar_t, char>(0x2022, 0x45)); //•		

		wxString search(L"\x81 ");
		const wchar_t* src = search.wx_str();		

		for (auto& tbl : table)
		{
			tbl.second = m_Table[tbl.second];

			search.RemoveLast();
			search.append(tbl.second);

			size_t pos = ret.find(search);

			while (pos != std::string::npos)
			{
				ret[pos++] = tbl.first;
				ret.erase(pos, 1);
				pos = ret.find(search, pos);
			}
		}
	}

	return ret;
}

int8_t RomFile::ReadInt8()
{
	int8_t value = 0;
	this->Read(&value, 1);
	return value;
}

int16_t RomFile::ReadInt16()
{
	int16_t value = 0;
	this->Read(&value, 2);
	return value;
}

int32_t RomFile::ReadInt32()
{		
	int32_t value = 0;
	this->Read(&value, 4);
	return value;
}

uint8_t RomFile::ReadUInt8()
{
	uint8_t value = 0;
	this->Read(&value, 1);
	return value;
}

uint16_t RomFile::ReadUInt16()
{
	uint16_t value = 0;
	this->Read(&value, 2);
	return value;
}

uint16_t RomFile::ReadUInt16(uint32_t off)
{
	this->Seek(off);
	return ReadUInt16();
}

uint32_t RomFile::ReadUInt32()
{
	uint32_t value = 0;
	this->Read(&value, 4);
	return value;
}

uint64_t RomFile::ReadUint64(uint32_t off)
{
	Seek(off);
	uint64_t value;
	Read(&value, 8);
	return value;
}

uint32_t RomFile::ReadPointer32(uint32_t offset)
{
	Seek(offset);
	return ReadUInt32() & ROM_BUS_NOT;
}

std::string RomFile::ReadString()
{
	size_t size = 0;

	while (this->ReadInt8() != 0x00) //Less memory alocation, but slow
	{	
		size++;
	}

	if (size == 0)
		return std::string();

	std::string string;
	string.resize(size);

	this->Seek(this->Tell() - size - 1);
	this->Read((void*)string.data(), size);

	return string;
}

void RomFile::WriteUInt32(uint32_t number)
{
	Write(&number, 4);
}

void RomFile::WriteUInt32(uint32_t number, uint32_t offset)
{
	Seek(offset);
	WriteUInt32(number);
}

void RomFile::ReadBytes(std::vector<uint8_t>& bytes, size_t size)
{
	bytes.resize(size, 0x00);
	this->Read(bytes.data(), size);
}

void RomFile::WriteBytes(const void* bytes, const size_t size)
{
	this->Write(bytes, size);
	this->Flush();
}

uint32_t RomFile::ConvertPointers(uint32_t pointer)
{
	return pointer - m_PointerIndex;
}

void RomFile::ConvertPointers(uint32_t* pointers, uint32_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		pointers[i] -= m_PointerIndex;
	}
}

uint32_t RomFile::ConvertOffsets(uint32_t offset)
{
	return offset + m_PointerIndex;
}

void RomFile::ConvertOffsets(uint32_t* offsets, uint32_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		offsets[i] += m_PointerIndex;
	}
}

bool RomFile::IsPointer(const uint32_t& pointer)
{
	return (pointer & 0x0f000000) == m_PointerIndex;
}

void RomFile::BackupRom(const std::string& inform)
{
	wxFileName destination(Path);
	destination.AppendDir("Backup");
	destination.SetName(inform);
	
	if (!destination.DirExists())
		destination.Mkdir();

	wxCopyFile(Path, destination.GetFullPath(), true);	
}

uint32_t RomFile::FindFreeSpace(uint32_t size)
{
	std::string block;
	block.resize(m_FreeSpace);

	Seek(m_End);
	Read(block.data(), m_FreeSpace);

	std::string buffer;
	buffer.resize(size+8, 0xff);

	size_t pos = block.find(buffer);	

//I don't know why, but my ROM got filled with 0x00 in the end instead of 0xff
//ToDo::Remove this section
	if(pos == std::string::npos)
	{
		memset(buffer.data(), 0, size+8);		
		pos = block.find(buffer);		
	}

	if (pos == std::string::npos)
		return pos;

	if (pos % 4 == 0) pos += 4;	 else while (pos % 4 != 0) ++pos;

	pos += 4;

	return pos + m_End;
}