#include "Rom.h"

Rom::Rom(id i, console c, bool translated)
{
	State = translated ? "Translated" : "Original";	
	std::string scriptExt = "";
	Id = i;

	switch (i)
	{
	case id::FoMT:	
		scriptExt = "fsf";
		Name = "FoMT";
		break;
	case id::MFoMT:
		scriptExt = "msf";
		Name = "MFoMT";
		break;
	case id::HMDS:
		scriptExt = "dsf";
		Name = "HMDS";
		break;
	default:
		break;
	}

	char buffer[30];
	sprintf(buffer, scriptName, State.c_str(), "%i", scriptExt.c_str());
	SetScriptInformation();

	memcpy(scriptName, buffer, 30);

	wxFileName* path = new wxFileName(wxStandardPaths::Get().GetExecutablePath());
	path->SetName("Rom_" + State);

	switch (c)
	{
	case console::GBA:
		path->SetExt("gba");
		break;
	case console::DS:
		path->SetExt("nds");
		break;
	default:
		break;
	}

	path->AppendDir(Name);
	path->AppendDir("Rom");

	if (!path->DirExists())
		path->Mkdir(511, wxPATH_MKDIR_FULL);

	Path = path->GetFullPath();
	//if (wxFile::Exists(Path));
	this->Open(Path, wxFile::read_write);

	scriptPath = new wxFileName(wxStandardPaths::Get().GetExecutablePath());
	scriptPath->AppendDir(Name);
	scriptPath->AppendDir("Script");
	scriptPath->AppendDir(State);

	if (!scriptPath->DirExists())
		scriptPath->Mkdir(511, wxPATH_MKDIR_FULL);

	delete path;
}

Rom::~Rom()
{
	delete scriptPath;
}

std::string Rom::GetTablePath()
{
	wxFileName* fileName = new wxFileName(Path);
	fileName->RemoveLastDir();
	fileName->AppendDir("Table");
	fileName->SetName(State);
	fileName->SetExt("tbl");

	std::string tablePath = fileName->GetFullPath().ToStdString();
	delete fileName;
	return tablePath;
}

std::string Rom::GetScriptFullName(int num)
{
	char buffer[30];
	sprintf(buffer, scriptName, num);
	return buffer;
}

std::string Rom::GetScriptFullPath(int num)
{
	scriptPath->SetFullName(GetScriptFullName(num));
	return scriptPath->GetFullPath().ToStdString();
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

void Rom::SetScriptInformation()
{
	switch (Id)
	{
	case id::FoMT:
		ScriptStartPointers = FOMT_SCRIPT_STARTPOINTERS;
		ScriptCount = FOMT_SCRIPT_COUNT;
		break;
	case id::MFoMT:
		ScriptStartPointers = MFOMT_SCRIPT_STARTPOINTERS;
		ScriptCount = MFOMT_SCRIPT_COUNT;
		break;
	case id::HMDS:
		ScriptStartPointers = HMDS_SCRIPT_STARTPOINTERS;
		ScriptCount = HMDS_SCRIPT_COUNT;
		break;
	default:
		break;
	}
}

void Rom::GetOffset(std::vector<uint32_t>& vector)
{
	std::vector<uint8_t> pointers;
	pointers.resize(ScriptCount * 4, 0x00);

	this->Seek(ScriptStartPointers);

	this->Read(pointers.data(), ScriptCount * 4);

	for (int i = 3; i < ScriptCount * 4; i += 4)
	{
		pointers[i] = 0x00;
	}

	vector.resize(ScriptCount, 0x00);	

	for (int i = 0; i < ScriptCount; ++i)
	{
		vector[i] = BitConverter::ToInt32(pointers.data(), i * 4);		
	}
}

void Rom::GetOffset(uint32_t& value, int number)
{
	value = 0;
	this->Seek(ScriptStartPointers + (number * 4));
	ReadPointer32(value);
}

void Rom::GetSizes(std::vector<uint32_t>& offsets, std::vector<uint32_t>& output)
{
	output.resize(ScriptCount, 0x00);
	
	uint32_t riff = 0x46464952;
	uint32_t riff_in = 0;

	for (int i = 0; i < ScriptCount; ++i)
	{
		this->Seek(offsets[i]);

		this->Read(&riff_in, 4);

		if (riff_in == riff)
			this->Read(&output[i], 4);
		else
			output[i] = 0;
	}	
}	

void Rom::Dump()
{
	std::vector<uint32_t> offsets;
	GetOffset(offsets);
	std::vector<uint32_t> sizes;
	GetSizes(offsets, sizes);

	for (int i = 0; i < ScriptCount; ++i)
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

			delete script;
		}
	}
}
