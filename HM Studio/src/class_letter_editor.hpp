#pragma once

#include <stdint.h>

#include "class_editor.hpp"

#include "class_rom_file.hpp"
#include "class_letter.hpp"

#define ToBlockPointer(x) x+m_StartBlock+0x08000000
#define ToPointerBlockPointer(x) x+m_StartPointers

class LetterEditor : public Editor
{
public:
	LetterEditor(const id& id);
	~LetterEditor() = default;
private:	
	std::string m_LetterDir;

	std::string m_Original;
	std::string m_Translated;	

	uint32_t m_StartPointers;	
	uint32_t m_StartBlock;
	uint32_t m_BlockSize;
private:
	uint32_t* GetPointers(bool translated);	

	void GetAllLines(std::vector<std::string>& letters_lines_all, std::vector<uint32_t>& letters_counts_all);
public:
	bool Open(uint32_t number);
	std::string& GetText(bool translated);
	void SetText(const std::string& text);
	void SaveFile();
public:
	std::string GetPath(const uint32_t& number, const bool& translated);
	std::string GetPath(const bool& translated) { return GetPath(m_Number, translated); }
	void SetupRom();
public:
	virtual void Dump(bool translated);
	virtual void Insert();
};
