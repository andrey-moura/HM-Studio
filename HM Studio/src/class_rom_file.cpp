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
		Moon::String::Replace(text[i], "\5\f", "\5\f" + m_EOL);
	}	
}

void RomFile::OutputTextWithVariables(std::vector<std::string>& text)
{
	//m_VarTable.Output(text, m_Table);

	StopWatch watch;
	watch.Start();

	for (std::string& str : text)
	{
		m_Table.Output(str);
	}

	watch.Stop();
	uint64_t elapsed = watch.Elapsed();

	std::string();

	for (size_t i = 0; i < text.size(); ++i)
	{
		Moon::String::Replace(text[i], "\5\f" + m_EOL, "\5\f");
	}
}

void RomFile::ReplaceWideChars(wxString& text)
{
	const wchar_t* txt = text.wc_str();

	if (Console == console::GBA)
	{		
		size_t pos = text.find((wchar_t)0x2605);

		while (pos != std::string::npos)
		{
			text[pos++] = 0x81;
			text.insert(pos++, 1, wchar_t(0x0161));
			pos = text.find((wchar_t)0x2605, pos);
		}
	}	
}

wxString RomFile::ReplaceWideChars(std::string& text)
{
	wxString ret = text;	
	const char* textData = text.data();
	const wchar_t* retData = ret.wc_str();	
	const wchar_t* search = L"\x81\x0161";	

	if (Console == console::GBA)
	{		
		size_t pos = ret.find(search);

		while (pos != std::string::npos)
		{
			ret[pos++] = (wchar_t)0x2605;
			ret.erase(pos, 1);
			pos = ret.find(search, pos);
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

std::vector<uint8_t> RomFile::ReadBytes(size_t offset, size_t size)
{
	std::vector<uint8_t> ret;

	Seek(offset);

	ret.resize(size);

	Read(ret.data(), size);

	return ret;
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

void RomFile::ConvertPointers(uint32_t* pointers, uint32_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		pointers[i] -= m_PointerIndex;
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
