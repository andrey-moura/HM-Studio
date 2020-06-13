#pragma once

#include <stdint.h>

#include "class_editor.hpp"

#include "class_rom_file.hpp"

struct LetterInfo
{
	uint32_t m_StartPointers;
	uint32_t m_Count;
};

class LetterEditor : public Editor
{
public:
	LetterEditor(const id& id);
	~LetterEditor() = default;
private:
	LetterInfo m_Info;
	std::string m_LetterDir;

	std::string m_Original;
	std::string m_Translated;

	uint32_t m_Number;
private:
	uint32_t* GetPointers(bool translated);
public:
	bool Open(uint32_t number);
	std::string& GetText(bool translated);
	void SetText(const std::string& text);
	void SaveToFile();
public:
	std::string GetPath(const uint32_t& number, const bool& translated);
	std::string GetPath(const bool& translated) { return GetPath(m_Number, translated); }
	void SetupRom();
public:
	virtual void Dump(bool translated);
};
