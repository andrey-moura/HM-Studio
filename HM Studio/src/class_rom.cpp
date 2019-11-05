#include "class_rom.hpp"

Rom::Rom(id i, bool translated) : wxFile()
{
	State = translated ? "Translated" : "Original";	
	std::string scriptExt = "";
	Id = i;	

	switch (i)
	{
	case id::FoMT:	
		scriptExt = "fsf";
		Name = "FoMT";
		Console = console::GBA;
		break;
	case id::MFoMT:
		scriptExt = "msf";
		Name = "MFoMT";
		Console = console::GBA;
		break;
	case id::DS:
		scriptExt = "dsf";
		Name = "DS";
		Console = console::DS;
		break;
	default:
		break;
	}

	char buffer[30];
	sprintf(buffer, scriptName, State.c_str(), "%i", scriptExt.c_str());
	
	Offset = offset();
	SetOffsets();

	memcpy(scriptName, buffer, 30);

	wxFileName path = wxFileName(wxStandardPaths::Get().GetExecutablePath());
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

	path.AppendDir(Name);
	path.AppendDir("Rom");

	if (!path.DirExists())
		path.Mkdir(511, wxPATH_MKDIR_FULL);

	Path = path.GetFullPath();
	//if (wxFile::Exists(Path));
	this->Open(Path, wxFile::read_write);

	scriptPath = wxFileName(wxStandardPaths::Get().GetExecutablePath());
	scriptPath.AppendDir(Name);
	scriptPath.AppendDir("Script");
	scriptPath.AppendDir(State);

	exportedScriptPath = wxFileName(scriptPath.GetFullPath());
	exportedScriptPath.RemoveLastDir();
	exportedScriptPath.AppendDir("Exported");
	exportedScriptPath.SetExt("txt");

	if (!scriptPath.DirExists())
		scriptPath.Mkdir(511, wxPATH_MKDIR_FULL);	
}

Rom::~Rom()
{	
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

void Rom::InputTextWithVariables(std::vector<std::string> &original, std::vector<std::string> &translated)
{
	std::string rawPlayer;
	std::string rawFarm;

	std::string player;
	std::string farm;

	std::string playerVar;
	std::string farmVar;

	switch (Console)
	{
	case console::GBA:
		rawPlayer = "ÿ!";
		rawFarm = "ÿ#";

		player = " PlayerName ";
		farm =   " FarmName   ";

		playerVar = "<PlayerName>";
		farmVar = "<FarmName  >";
		break;
	case console::DS:
		rawPlayer = "ÿ$";
		//rawFarm = "ÿ#";

		player = "Player";

		playerVar = "Player";
		//farm =   "Farm  ";
		break;
	default:
		return;
	}

	for (int i = 0; i < translated.size(); ++i)
	{
		StringUtil::Replace(rawPlayer, player, translated[i]);
		StringUtil::Replace(rawFarm, farm, translated[i]);
	}

	std::string table_path = GetTablePath();

	if (wxFile::Exists(table_path))
		Table::InputTable(FileUtil::ReadAllText(table_path), translated);

	for (int i = 0; i < translated.size(); ++i)
	{
		StringUtil::Replace(player, playerVar, translated[i]);
		StringUtil::Replace(farm, farmVar, translated[i]);

		if (i == 8)
			std::string();
		StringUtil::Replace(std::string("|²"), std::string("|²\r\n"), translated[i]);
	}

	for (int i = 0; i < original.size(); ++i)
	{
		StringUtil::Replace(rawPlayer, playerVar, original[i]);
		StringUtil::Replace(rawFarm, farmVar, original[i]);

		StringUtil::Replace(std::string("|²"), std::string("|²\r\n"), original[i]);
	}
}

void Rom::OutputTextWithVariables(std::vector<std::string>& translated)
{
	std::string rawPlayer;
	std::string rawFarm;

	std::string player;
	std::string farm;

	std::string playerVar;
	std::string farmVar;

	switch (Console)
	{
	case console::GBA:
		rawPlayer = "ÿ!";
		rawFarm = "ÿ#";

		player = " PlayerName ";
		farm = " FarmName   ";

		playerVar = "<PlayerName>";
		farmVar = "<FarmName  >";
		break;
	case console::DS:
		rawPlayer = "ÿ$";
		//rawFarm = "ÿ#";

		player = "Player";

		playerVar = "Player";
		//farm =   "Farm  ";
		break;
	default:
		return;
	}

	for (int i = 0; i < translated.size(); ++i)
	{
		StringUtil::Replace(playerVar, player, translated[i]);
		StringUtil::Replace(farmVar, farm, translated[i]);

		StringUtil::Replace("|²\r\n", "|²", translated[i]);
	}	

	Table::OutPutTable(FileUtil::ReadAllText(GetTablePath()), translated);

	for (int i = 0; i < translated.size(); ++i)
	{
		StringUtil::Replace(player, rawPlayer, translated[i]);
		StringUtil::Replace(farm, rawFarm, translated[i]);
	}

	if (Console == console::DS)
	{
		for (int i = 0; i < translated.size(); ++i)
		{
			StringUtil::Replace("\r\n", "\n", translated[i]);
		}
	}
}

std::string Rom::GetScriptFullName(int num)
{
	char buffer[30];
	sprintf(buffer, scriptName, num);
	return buffer;
}

std::string Rom::GetScriptFullPath(int num)
{
	scriptPath.SetFullName(GetScriptFullName(num));
	return scriptPath.GetFullPath().ToStdString();
}

std::string Rom::GetScriptExportedFullPath(int num)
{
	std::string buffer;
	buffer.resize(25);

	sprintf((char*)buffer.data(), scriptExportedName, num);

	exportedScriptPath.SetFullName(buffer);

	return exportedScriptPath.GetFullPath().ToStdString();
}

void Rom::ReadInt32(uint32_t& value)
{	
	value = 0;
	this->Read(&value, 4);
}

void Rom::ReadPointer32(uint32_t& value)
{
	value = 0;
	this->Read(&value, 3);
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

void Rom::EraseBlock(size_t size)
{
	std::vector<uint8_t> bytes;
	bytes.resize(size, 0x00);
	this->WriteBytes(bytes);
}

bool Rom::VerifyEmptyBlock(size_t size)
{
	std::vector<uint8_t> bytes;
	std::vector<uint8_t> buffer;

	bytes.resize(size, 0x00);	

	this->Read(bytes.data(), size);

	if (bytes[0] == 0x00)
	{
		buffer.resize(size, 0x00);
		if (memcmp(bytes.data(), buffer.data(), size) == 0)
			return true;
		else return false;
	}	
	else if (bytes[0] == 0xff)
	{
		buffer.resize(size, 0xff);
		if (memcmp(bytes.data(), buffer.data(), size) == 0)
			return true;
		else return false;
	}
	else return false;
}

int Rom::InsertScript(int number, std::vector<uint8_t>& bytes)
{
	uint32_t offset = 0;
	uint32_t size = 0;
	GetOffset(offset, number);
	GetSize(offset, size);	

	if (bytes.size() <= size)
	{
		this->Seek(offset);
		this->EraseBlock(size);
		this->Seek(offset);
		this->WriteBytes(bytes);

		return 0;
	}
	else if(bytes.size() > size)
	{
		int end_offset = offset + size;
		this->Seek(end_offset);

		if (VerifyEmptyBlock(bytes.size() - size))
		{
			this->Seek(offset);
			this->WriteBytes(bytes);
			return 1;
		}
		else {
			return -1;
		}

		return end_offset;
	}

	return 0;
}

void Rom::SetOffsets()
{
	switch (Id)
	{
	case id::FoMT:
		Offset.Script_start_pointers = 0xF89D8;		
		Offset.Script_count = 1328;
		break;
	case id::MFoMT:
		Offset.Script_start_pointers = 0x1014C0;
		Offset.Script_count = 1415;
		break;
	case id::DS:
		Offset.Script_start_pointers = 0x24C2E04;
		Offset.Script_count = 1296;
		break;
	default:
		break;
	}
}

void Rom::GetOffset(std::vector<uint32_t>& vector)
{
	std::vector<uint8_t> pointers;
	pointers.resize(Offset.Script_count * 4, 0x00);

	this->Seek(Offset.Script_start_pointers);

	this->Read(pointers.data(), Offset.Script_count * 4);

	for (int i = 3; i < Offset.Script_count * 4; i += 4)
	{
		pointers[i] = 0x00;
	}

	vector.resize(Offset.Script_count, 0x00);

	for (int i = 0; i < Offset.Script_count; ++i)
	{
		vector[i] = *((int*)pointers.data()) + (i * 4);
	}
}

void Rom::GetOffset(uint32_t& value, int number)
{
	value = 0;
	this->Seek(Offset.Script_start_pointers + (number * 4));
	ReadPointer32(value);
}

void Rom::GetSize(std::vector<uint32_t>& offsets, std::vector<uint32_t>& output)
{
	output.resize(Offset.Script_count, 0x00);
	
	uint32_t riff = 0x46464952;
	uint32_t riff_in = 0;

	for (int i = 0; i < Offset.Script_count; ++i)
	{
		this->Seek(offsets[i]);

		this->Read(&riff_in, 4);

		if (riff_in == riff)
			this->Read(&output[i], 4);
		else
			output[i] = 0;
	}	
}
void Rom::GetSize(uint32_t offset, uint32_t& output)
{
	this->Seek(offset + 4);
	ReadInt32(output);
}


void Rom::Dump()
{
	std::vector<uint32_t> offsets;
	GetOffset(offsets);
	std::vector<uint32_t> sizes;
	GetSize(offsets, sizes);

	for (int i = 0; i < Offset.Script_count; ++i)
	{		
		if (offsets[i] > 0 && sizes[i] > 0)
		{
			wxFile* script = new wxFile();
			script->Create(GetScriptFullPath(i), true);
			script->Open(GetScriptFullPath(i), wxFile::read_write);

			std::vector<uint8_t> scriptBytes;
			scriptBytes.resize(sizes[i], 0x00);

			this->Seek(offsets[i]);
			this->Read(scriptBytes.data(), sizes[i]);
			script->Write(scriptBytes.data(), sizes[i]);
			script->Close();
		}
	}
}
