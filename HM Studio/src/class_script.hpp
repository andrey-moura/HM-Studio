#pragma once

#include <string>
#include <vector>

#define SCRIPT_RIFF 0x46464952

class Script
{
public:
	Script() = default;
	//Script is now the owner of this data. You don't to care about deleting.
	Script(uint8_t* bytes, size_t size);
	Script(std::vector<uint8_t> bytes);
	~Script();	

	//Script is now the owner of this data. You don't to care about deleting.
	void SetData(uint8_t* bytes, size_t size);
	void SetData(std::vector<uint8_t> bytes);
	bool CompareCode(const Script& other);
private:
	//RIFF
	uint8_t* m_data = nullptr;	
	//RIFF + 4
	uint32_t* m_pRiffLenght = nullptr;
	//SCR CODE + 4
	uint32_t* m_pSrcCodeLenght = nullptr;
	//Jump ? RIFF + 0x14 + SCR CODE Lenght + JUMP lenght : RIFF + 0x14 + SCR CODE lenght + 0x08
	uint8_t* m_pStr = nullptr;
	//STR + 4
	uint32_t* m_pStrLenght = nullptr;
	//STR + 8
	uint32_t* m_pStrCount = nullptr;
	//STR + 12
	uint32_t* m_pStrPointers = nullptr;
	//STR + (4 * STRcount) + 12
	uint8_t* m_pStartText = nullptr;	
private:	
	//Calculates the pointers
	void GetPointers();
	//const char* CStringAt(size_t index) { return (const char*)(m_pStartText + m_pStrPointers[index]); }
	void ReleasePointers();
//Operators
public:
	//No checking!
	std::string operator[](int index) const;
	bool operator==(const Script& other) const;
	bool operator!=(const Script& other) const;
public:
	//Gets the text of this script.
	std::vector<std::string> GetText();
	bool m_IsGood = false;
	void UpdateText(const std::vector<std::string>& text);
	uint8_t* GetStartText() const { return m_pStartText; }
	bool IsGood();
	//This script have text?
	bool HaveText();
	uint32_t GetRiffLenght() const { return *m_pRiffLenght; }
	//Take care
	uint8_t* GetData() const { return m_data; }
	size_t Count() const;
};