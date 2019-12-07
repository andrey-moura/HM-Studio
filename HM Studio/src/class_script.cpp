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

	GetPointers();


	if (*m_pRiffLenght > size)
		*m_pRiffLenght = size;


	m_data[size - 1] = 0x00;
}

Script::~Script()
{
	delete[] m_data;
}

std::vector<std::string> Script::GetText()
{
	std::vector<std::string> text;	

	int this_size = 0;

	for (int i = 0; i < *m_pStrCount; ++i)
	{
		this_size = 0;
		uint8_t* pointer = m_pStrPointers[i] + m_pStartText;
		
		text.push_back((const char*)pointer);
	}

	return text;
}

int Script::size()
{
	return *m_pStrCount;
}

void Script::GetStrPosition()
{
	uint8_t* thisChar = m_data;
	uint8_t* lastChar = m_data + *m_pRiffLenght;

	while (thisChar < lastChar)
	{
		thisChar = (uint8_t*)memchr(thisChar, 0x53, (m_data + *m_pRiffLenght - thisChar));

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

void Script::GetPointers()
{
	m_pRiffLenght = (uint32_t*)(m_data + 4);

	GetStrPosition();

	m_pStrCount = (uint32_t*)(m_pStr + 8);
	m_pStrLenght = (uint32_t*)(m_pStr + 4);
	m_pStrPointers = (uint32_t*)(m_pStr + 12);
	m_pStartText = (uint8_t*)(m_pStr + (*m_pStrCount * 4) + 12);
}

void Script::UpdateText(const std::vector<std::string>& text)
{
	uint32_t totalTextSize = 0;

	//First, I will get the total lenght of strings. This will help us to not allocating memory in every loop in the future
	for (unsigned int i = 0; i < *m_pStrCount; ++i)
	{
		m_pStrPointers[i] = totalTextSize;
		totalTextSize += text[i].size() + 1; //The std::string is not null terminated
	}

	uint32_t newSize = totalTextSize + (*m_pStrCount * 4) + 4;

	if(newSize != (*m_pStrLenght)) //If is the same, we don't need to re-allocate memory.
	{
		int size = (m_pStr - m_data) + (*m_pStrCount * 4) + 12 + totalTextSize;
		*m_pRiffLenght = size;
		uint8_t* newData = new uint8_t[size];
		memcpy(newData, m_data, (m_pStr - m_data) + ((*m_pStrCount * 4) + 12));
		delete[] m_data;
		m_data = newData;

		GetPointers(); //Updates pointers
	}

	*m_pStrLenght = newSize;

	for (unsigned int i = 0; i < *m_pStrCount; ++i)
	{			
		memcpy(m_pStartText + m_pStrPointers[i], text[i].c_str(), text[i].size() + 1);
	}		
}