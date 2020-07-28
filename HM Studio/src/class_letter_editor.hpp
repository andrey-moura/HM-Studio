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
	void SetPointer(size_t number, uint32_t offset);
public:
	bool Open(uint32_t number);
	std::string& GetText(bool translated);
	void SetText(const std::string& text);
	void SaveFile();
	bool SaveText(const std::string& text);
public:
	virtual std::string GetPath(const size_t& number, const bool& translated) const override;
	virtual std::string GetPath(const bool& translated) const override { return GetPath(m_Number, translated); }
	virtual void GetRomInfo() override;	
public:
	void DumpAll(bool translated);
	void InsertAll();
	void InsertFile();	
};
