#pragma once

#include <iostream>
#include <vector>

class SpriteFile
{
//Constructors / deconstructors
public:
	SpriteFile(std::vector<uint8_t> bytes);

//Satic, for helping...	
public:
	static std::vector<uint8_t> GetEmptyScript(int l_count = 0);
	//This script have text?
	bool HaveText();
	//Gets the offset of the str block.
	void GetStrPosition();
	//Gets the lenght, in bytes, of the str block.
	uint32_t GetStrLenght();
	//Sets the lenght, in bytes, of the str block.
	void SetStrLenght(uint32_t str_size);
	//Gets the count of text in this script.
	void GetStrCount();
	//Gets the pointers of this script.
	std::vector<uint32_t> GetPointers();
	//Updates the str lenght, riff lenght, the text, and the pointers.
	void UpdateText(std::vector<std::string> text);
	//Gets the offset of the start of the text.
	void GetTextOffset();
	//Gets the text of this script.
	std::vector<std::string> GetText();

public:
	std::vector<uint8_t> data;
	uint32_t str_count = 0;
//For internel use only.	
private:
	uint32_t str_offset = 0;	
	uint32_t text_offset = 0;
};

