#pragma once

#include <algorithm>
#include <string>
#include <vector>

class Script
{
//Constructors / deconstructors
public:
	Script();

	void SetData(std::vector<uint8_t>& bytes);

	~Script();	

private:
	//Gets the pointer to the script size
	inline uint32_t* RIFFLenght() { return (uint32_t*)(m_data + 4); }
	//Gets the pointer to the str count.
	inline uint32_t* STRCount() { return (uint32_t*)(m_pStr + 8); }
	//Gets the pointer to the str lenght.
	inline uint32_t* STRLenght() { return (uint32_t*)(m_pStr + 4); }
	//Gets the pointer to the (pointers) of text.
	inline uint32_t* STRPointers() { return (uint32_t*)(m_pStr + 12); }
	//Return a pointer, using a position relative to the start of the text. (Use with STRPointers())
	inline uint8_t* RelativeToStartText(uint32_t position) { return (m_pStr + (*STRCount() * 4) + 12) + position; }

private:
	//Gets the offset of the str block.
	void GetStrPosition();	

	std::vector<uint8_t> data;

	uint8_t* m_data = nullptr;
	uint32_t* m_size = nullptr;

	uint8_t* m_pStr = nullptr;
	uint32_t* m_strSize = nullptr;	

	bool m_delete = true;
public:
	//Gets the text of this script.
	std::vector<std::string> GetText();	
	void UpdateText(std::vector<std::string>& text);
	//This script have text?
	inline bool HaveText() { return *STRCount(); }
	int size();
};