#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "class_util_wx_file.hpp"

class Script
{
public:
	Script();
	~Script();	

	void SetData(std::vector<uint8_t>& bytes);
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
	//STR + (4 * STR count) + 12
	uint8_t* m_pStartText = nullptr;	
private:	
	//Calculates the pointers
	void GetPointers();	
public:
	//Gets the text of this script.
	std::vector<std::string> GetText();	
	void UpdateText(const std::vector<std::string>& text);
	//This script have text?
	inline bool HaveText() { return *m_pStrCount; }
	uint32_t GetRiffLenght() const { return *m_pRiffLenght; }
	//Take care
	uint8_t* GetData() const { return m_data; }
	int size();
};