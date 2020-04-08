#pragma once
#include <vector>

#include <wx/file.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>

#include "class_file.hpp"
#include "class_util_string.hpp"
#include "class_table.hpp"
#include "class_script.hpp"

#define ROM_BUS 0x8000000
#define ROM_BUS_NOT 0xF7FFFFFF

class RomFile : public wxFile
{
public:
	RomFile(const std::string& path);
	RomFile() = default;
	~RomFile() = default;
public:
	//std::string GetTablePath();
	//void InputTextWithVariables(std::vector<std::string>& text);
	//void OutputTextWithVariables(std::vector<std::string>& text);
	void BackupRom(const std::string& inform);				
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
	uint64_t ReadUint64(uint32_t off);

	uint32_t ReadPointer32(uint32_t offset);

	std::string ReadString();

	void WriteUInt32(uint32_t number);
	void WriteUInt32(uint32_t number, uint32_t offset);

	void ReadBytes(std::vector<uint8_t> &bytes, size_t size);

	void WriteBytes(std::vector<uint8_t> bytes);
	void WriteBytes(const void* bytes, const size_t size);
};