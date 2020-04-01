#include "class_rom.hpp"

Rom::Rom(id i, bool translated) : wxFile()
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
	path.AppendDir(Name);

	m_Dir = path.GetPath().ToStdString();

	path.AppendDir("Rom");
	path.SetName("Rom_" + State);

	switch (Console)
	{
	case console::GBA:
		path.SetExt("gba");
		break;
	case console::DS:
		path.SetExt("nds");
		break;
	default:
		break;
	}

	if (!path.DirExists())
		path.Mkdir(511, wxPATH_MKDIR_FULL);

	Path = path.GetFullPath();	
	this->Open(Path, wxFile::read_write);	
}

std::string Rom::GetTablePath()
{
	wxFileName fileName = wxFileName(Path);
	fileName.RemoveLastDir();
	fileName.AppendDir("Table");
	fileName.SetName(State);
	fileName.SetExt("tbl");

	return fileName.GetFullPath().ToStdString();
}

void Rom::InputTextWithVariables(std::vector<std::string>& text)
{
	std::vector<std::string> raw;
	std::vector<std::string> spaced;
	std::vector<std::string> var;

	std::string endLine;

	switch (Console)
	{
	case console::GBA:
		raw.push_back("ÿ!");
		raw.push_back("ÿ#");
		raw.push_back("ÿ%");
		raw.push_back("ÿ\'");
		raw.push_back("ÿ)");
		raw.push_back("ÿ*");
		raw.push_back("ÿ,");
		raw.push_back("ÿ-");
		raw.push_back("ÿ+");

		spaced.push_back(" PlayerName ");
		spaced.push_back(" FarmName   ");
		spaced.push_back(" AnimalName ");
		spaced.push_back(" Variable02 ");
		spaced.push_back(" CustomName ");
		spaced.push_back(" InfantName ");
		spaced.push_back(" ValleyName ");
		spaced.push_back(" ValleyBaby ");
		spaced.push_back(" ValleyFarm ");

		var.push_back("<PlayerName>");
		var.push_back("<FarmName  >");
		var.push_back("<AnimalName>");
		var.push_back("<Variable02>");
		var.push_back("<CustomName>");
		var.push_back("<InfantName>");
		var.push_back("<ValleyName>");
		var.push_back("<ValleyBaby>");
		var.push_back("<ValleyFarm>");

		endLine = "\r\n";
		break;
	case console::DS:
		raw.push_back("ÿ$");

		spaced.push_back("Player");

		var.push_back("<Player>");

		endLine = "\n";
		break;
	default:
		return;
	}

	for (size_t i = 0; i < text.size(); ++i)
	{
		for (int z = 0; z < var.size(); ++z)
		{
			StringUtil::Replace(raw[z], spaced[z], text[i]);
		}
	}

	std::string tablePath = GetTablePath();

	if (wxFile::Exists(tablePath))
		Table::InputTable(File::ReadAllText(tablePath), text);

	std::string hex050c;
	hex050c.append(1, 0x05);
	hex050c.append(1, 0x0c);

	for (size_t i = 0; i < text.size(); ++i)
	{
		for (int z = 0; z < var.size(); ++z)
		{
			StringUtil::Replace(spaced[z], var[z], text[i]);
		}		

		StringUtil::Replace(hex050c, hex050c + endLine, text[i]);
	}
}

void Rom::OutputTextWithVariables(std::vector<std::string>& text)
{
	std::vector<std::string> raw;
	std::vector<std::string> spaced;
	std::vector<std::string> var;

	std::string endLine;

	switch (Console)
	{
	case console::GBA:
		raw.push_back("ÿ!");
		raw.push_back("ÿ#");
		raw.push_back("ÿ%");
		raw.push_back("ÿ\'");
		raw.push_back("ÿ)");
		raw.push_back("ÿ*");
		raw.push_back("ÿ,");
		raw.push_back("ÿ-");
		raw.push_back("ÿ+");

		spaced.push_back(" PlayerName ");
		spaced.push_back(" FarmName   ");
		spaced.push_back(" AnimalName ");
		spaced.push_back(" Variable02 ");
		spaced.push_back(" CustomName ");
		spaced.push_back(" InfantName ");
		spaced.push_back(" ValleyName ");
		spaced.push_back(" ValleyBaby ");
		spaced.push_back(" ValleyFarm ");

		var.push_back("<PlayerName>");
		var.push_back("<FarmName  >");
		var.push_back("<AnimalName>");
		var.push_back("<Variable02>");
		var.push_back("<CustomName>");
		var.push_back("<InfantName>");
		var.push_back("<ValleyName>");
		var.push_back("<ValleyBaby>");
		var.push_back("<ValleyFarm>");

		endLine = "\r\n";
		break;
	case console::DS:
		raw.push_back("ÿ$");		

		spaced.push_back("Player");		

		var.push_back("<Player>");

		endLine = "\n";
		break;
	default:
		return;
	}

	std::string hex050c;
	hex050c.append(1, 0x05);
	hex050c.append(1, 0x0c);

	for (size_t i = 0; i < text.size(); ++i)
	{		
		for (size_t z = 0; z < var.size(); ++z)
		{
			StringUtil::Replace(var[z], spaced[z], text[i]);
		}		

		StringUtil::Replace(hex050c + endLine, hex050c, text[i]);		
	}	

	Table::OutPutTable(File::ReadAllText(GetTablePath()), text);

	for (size_t i = 0; i < text.size(); ++i)
	{
		if (Console == console::GBA)
		{
			StringUtil::ReplaceMatching('\"', 0xcf, text[i], false);
			StringUtil::ReplaceMatching('\'', 0xd0, text[i], true);
		}		

		for (int z = 0; z < var.size(); ++z)
		{
			StringUtil::Replace(spaced[z], raw[z], text[i]);
		}
	}	
}

int8_t Rom::ReadInt8()
{
	int8_t value = 0;
	this->Read(&value, 1);
	return value;
}

int16_t Rom::ReadInt16()
{
	int16_t value = 0;
	this->Read(&value, 2);
	return value;
}

int32_t Rom::ReadInt32()
{		
	int32_t value = 0;
	this->Read(&value, 4);
	return value;
}

uint8_t Rom::ReadUInt8()
{
	uint8_t value = 0;
	this->Read(&value, 1);
	return value;
}

uint16_t Rom::ReadUInt16()
{
	uint16_t value = 0;
	this->Read(&value, 2);
	return value;
}

uint16_t Rom::ReadUInt16(uint32_t off)
{
	this->Seek(off);
	return ReadUInt16();
}

uint32_t Rom::ReadUInt32()
{
	uint32_t value = 0;
	this->Read(&value, 4);
	return value;
}

uint64_t Rom::ReadUint64(uint32_t off)
{
	Seek(off);
	uint64_t value;
	Read(&value, 8);
	return value;
}

uint32_t Rom::ReadPointer32(uint32_t offset)
{
	Seek(offset);
	return ReadUInt32() & ROM_BUS_NOT;
}

std::string Rom::ReadString()
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
	this->Read(string.data(), size);

	return string;
}

void Rom::WriteUInt32(uint32_t number)
{
	Write(&number, 4);
}

void Rom::WriteUInt32(uint32_t number, uint32_t offset)
{
	Seek(offset);
	WriteUInt32(number);
}

void Rom::ReadBytes(std::vector<uint8_t>& bytes, size_t size)
{
	bytes.resize(size, 0x00);
	this->Read(bytes.data(), size);
}

void Rom::WriteBytes(std::vector<uint8_t> bytes)
{
	this->Write(bytes.data(), bytes.size());
	this->Flush();
}

void Rom::WriteBytes(const void* bytes, const size_t size)
{
	this->Write(bytes, size);
	this->Flush();
}

void Rom::BackupRom(const std::string& inform)
{
	wxFileName destination(Path);
	destination.AppendDir("Backup");
	destination.SetName(inform);
	
	if (!destination.DirExists())
		destination.Mkdir();

	wxCopyFile(Path, destination.GetFullPath(), true);	
}