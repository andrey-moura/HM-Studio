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
private:
	//Gets the pointer to the script size	
	uint32_t* m_pRiffLenght = nullptr;
	//Gets the pointer to the str count.	
	uint32_t* m_pStrCount = nullptr;
	//Gets the pointer to the str lenght.	
	uint32_t* m_pStrLenght = nullptr;
	//Gets the pointer to the (pointers) of text.	
	uint32_t* m_pStrPointers = nullptr;
	//Position relative to the start of the text. (Use with StrPointers)	
	uint8_t* m_pStartText = nullptr;	
private:
	//Gets the offset of the str block.
	void GetStrPosition();	
	//Calculates the pointers
	void GetPointers();

	uint8_t* m_data = nullptr;	

	uint8_t* m_pStr = nullptr;
	uint32_t* m_strSize = nullptr;	
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