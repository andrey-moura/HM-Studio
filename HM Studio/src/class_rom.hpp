#pragma once
#include <vector>

#include <wx/file.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>


#include "class_util_string.hpp"
#include "class_table.hpp"
#include "class_util_wx_file.hpp"
#include "class_script.hpp"

enum class console {
	GBA,
	DS
};

enum class id {
	FoMT,
	MFoMT,
	DS
};

struct offset {
	
	uint32_t Script_start_pointers = 0;
	uint32_t Script_count = 0;	
};

//enum scriptCount {
//	FOMT_SCRIPT_COUNT,
//	MFoMT_SCRIPT_COUNT,
//	HMDS_SCRIPT_COUNT
//};

class Rom : public wxFile
{
public:
	Rom(id i, bool translated);
	~Rom();

	id Id;
	console Console;
	std::string Path;
	std::string Name;
	std::string State;

public:
	std::string GetTablePath();
	void InputTextWithVariables(std::vector<std::string>& original, std::vector<std::string> &translated);
	void OutputTextWithVariables(std::vector<std::string>& translated);
	void BackupRom(const std::string& inform);
//script
public:
	char scriptName[30] = "Script_%s_%s.%s";

	char scriptExportedName[23] = "Script_Exported_%i.txt";

	std::string GetScriptFullName(int num);
	std::string GetScriptFullPath(int num);
	std::string GetScriptExportedFullPath(int num);	

	wxFileName scriptPath;
	wxFileName exportedScriptPath;
	
	void GetOffset(std::vector<uint32_t>& vector);
	void GetOffset(uint32_t& value, int number);
	void GetSize(std::vector<uint32_t>& offsets, std::vector<uint32_t>& output);
	void GetSize(uint32_t offset, uint32_t& output);
	void Dump();

	offset Offset;

//File
public:
	//void ReadInt32(uint32_t& value);
	int8_t  ReadInt8();
	int16_t ReadInt16();
	int32_t ReadInt32();

	uint8_t  ReadUInt8();
	uint16_t ReadUInt16();
	uint16_t ReadUInt16(uint32_t off);
	uint32_t ReadUInt32();

	std::string ReadString();

	void ReadPointer32(uint32_t& value);
	void ReadBytes(std::vector<uint8_t> &bytes, size_t size);

	void WriteBytes(std::vector<uint8_t> bytes);
	void WriteBytes(const void* bytes, const size_t size);
	void EraseBlock(size_t size);
	bool VerifyEmptyBlock(size_t size);
	int InsertScript(int number, const Script& script);
private:
	void SetOffsets();

};

