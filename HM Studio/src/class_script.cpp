#include "class_script.hpp"

Script::Script()
{
	
}

void Script::SetData(std::vector<uint8_t>& bytes)
{
	int size = bytes.size();

	if (m_data)
		delete[] m_data;

	m_data = new uint8_t[size];

	memcpy(m_data, bytes.data(), size);

	if (*RIFFLenght() > size)
		*RIFFLenght() = size;

	GetStrPosition();

	m_data[size - 1] = 0x00;
}

Script::~Script()
{
	delete[] m_data;
}

std::vector<std::string> Script::GetText()
{
	std::vector<std::string> text;

	uint32_t* pointers = STRPointers();

	int this_size = 0;

	for (int i = 0; i < *STRCount(); ++i)
	{
		this_size = 0;
		uint8_t* pointer = RelativeToStartText(pointers[i]);
		
		text.push_back((const char*)pointer);		
	}

	return text;
}

int Script::size()
{
	return *STRCount();
}

void Script::GetStrPosition()
{
	uint8_t* thisChar = m_data;
	uint8_t* lastChar = m_data + *RIFFLenght();
	while (thisChar < lastChar)
	{
		thisChar = (uint8_t*)memchr(thisChar, 0x53, (data.data() + data.size() - thisChar));

		if (thisChar + 3 > lastChar) //Can we use index++ and stay inside m_data?
			return;

		++thisChar;

		if (*thisChar != 0x54)
			continue;

		++thisChar;

		if (*thisChar != 0x52)
			continue;

		++thisChar;

		if (*thisChar != 0x20)
			continue;

		m_pStr = (uint8_t*)(thisChar - 3);
		return;
	}
}

void Script::UpdateText(std::vector<std::string>& text)
{
	//*STRLenght() = 0; //Resets the size of the STR

	int totalTextSize = 0;

	//First, I will get the lenght of total strings. This will help to not allocating memory in every for in the future
	for (unsigned int i = 0; i < *STRCount(); ++i)
	{
		STRPointers()[i] = totalTextSize;
		totalTextSize += text[i].size() + 1; //The std::string is not null terminated
	}

	std::vector<char> buffer;
	buffer.resize(totalTextSize);

	for (unsigned int i = 0; i < *STRCount(); ++i)
	{
		memcpy(buffer.data() + STRPointers()[i], text[i].data(), text[i].size());
	}

	if ((totalTextSize + (*STRCount() * 4) + 4) > * STRLenght())
	{

	}

	//int text_size = 0;
	//std::string buffer;

	//for (int i = 0; i < str_count; ++i)
	//{
	//	memcpy(data.data() + str_offset + (i * 4) + 12, &text_size, 4);		
	//	text_size += text[i].length() + 1;
	//	
	//	buffer.append(text[i]);
	//	buffer.append(1, (char)0x00);
	//}

	//data.resize(str_offset + (str_count * 4) + 12 + text_size);
	//memcpy(data.data() + text_offset, buffer.data(), text_size);

	//int str_size = (str_count * 4) + 4 + text_size;
	//memcpy(data.data() + str_offset + 4, &str_size, 4);

	//int riff_size = data.size();
	//memcpy(data.data() + 4, &riff_size, 4);
}