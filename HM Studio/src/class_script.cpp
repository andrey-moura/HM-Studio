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

bool Script::CompareCode(const Script& other)
{		
	if (memcmp((uint8_t*)m_pSrcCodeLenght - 8, (uint8_t*)other.m_pSrcCodeLenght - 8, (m_pStr - m_data) - 4) != 0) return false;

	if (*m_pStrCount != *(other.m_pStrCount)) return false;

	return true;
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

void Script::GetPointers() //This is very close to the way it is done in GBA
{	
	m_pRiffLenght = (uint32_t*)(m_data + 0x04);
	m_pSrcCodeLenght = (uint32_t*)(m_data + 0x10);

	uint32_t* strOrJump = (uint32_t*)(m_data + (*m_pSrcCodeLenght) + 0x14);

	if (*strOrJump == 0x504d554a) //It is jump
	{
		uint32_t jumpLenght = *(strOrJump + 1);
		m_pStr = (uint8_t*)(jumpLenght + 8 + (uint8_t*)strOrJump);
	}	
	else {
		m_pStr = (uint8_t*)strOrJump;
	}	
	
	m_pStrCount = (uint32_t*)(m_pStr + 0x08);
	m_pStrLenght = (uint32_t*)(m_pStr + 0x04);
	m_pStrPointers = (uint32_t*)(m_pStr + 0x0c);
	m_pStartText = (uint8_t*)(m_pStr + (*m_pStrCount * 4) + 0x0c);
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