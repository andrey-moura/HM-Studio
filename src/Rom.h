#pragma once
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"

#include <vector>

#include "BitConverter.h"

#include "FoMT.h"
#include "MFoMT.h"
#include "HMDS.h"

enum class console {
	GBA,
	DS
};

enum class id {
	FoMT,
	MFoMT,
	HMDS
};

//enum scriptCount {
//	FOMT_SCRIPT_COUNT,
//	MFoMT_SCRIPT_COUNT,
//	HMDS_SCRIPT_COUNT
//};

class Rom : public wxFile
{
public:
	Rom(id i, console c, bool translated);
	~Rom();

	id Id;
	std::string Path;
	std::string Name;
	std::string State;

public:
	std::string GetTablePath();

//script
public:
	char scriptName[30] = "Script_%s_%s.%s";	
	std::string GetScriptFullName(int num);
	std::string GetScriptFullPath(int num);

	wxFileName* scriptPath = nullptr;
	
	void GetOffset(std::vector<uint32_t>& vector);
	void GetOffset(uint32_t& value, int number);
	void GetSizes(std::vector<uint32_t>& offsets, std::vector<uint32_t>& output);
	void Dump();

	uint32_t ScriptStartPointers;
	uint32_t ScriptCount;

//File
public:
	void ReadInt32(uint32_t& value);
	void ReadPointer32(uint32_t& value);

private:
	void SetScriptInformation();

};

