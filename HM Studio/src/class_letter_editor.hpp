#pragma once

#include <stdint.h>

#include "class_rom_file.hpp"

struct LetterInfo
{
	uint32_t m_StartPointers;
	uint32_t m_Count;
};

class LetterEditor
{
public:
	LetterEditor(const id& console);
	~LetterEditor() = default;
private:
	RomFile m_RomOriginal;
	RomFile m_RomTranslated;

	LetterInfo m_Info;

	std::string m_LetterDir;

	void SetupRom();

public:
	RomFile& GetRom(bool translated);
	void Dump(bool translated);

	std::string GetPath(uint32_t number, bool translated);
private:
	uint32_t* GetPointers(bool translated);
};
