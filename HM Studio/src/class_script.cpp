#include "class_script.hpp"

Script::Script(uint8_t* bytes, size_t size)
{
	SetData(bytes, size);
}

Script::Script(std::vector<uint8_t> bytes)
{
	SetData(bytes);
}

Script::~Script()
{
	delete[] m_data;
}

void Script::SetData(uint8_t* bytes, size_t size)
{	
	if (m_data)
		delete[] m_data;

	if (size == 0)
	{
		m_IsGood = false;
		ReleasePointers();
		return;
	}

	m_IsGood = true;

	m_data = bytes;

	GetPointers();

	if (*m_pRiffLenght > size)
		*m_pRiffLenght = size;

	m_data[size - 1] = 0x00; //This situation can generate an exception.
}

void Script::SetData(std::vector<uint8_t> bytes)
{
	size_t size = bytes.size();

	if (size == 0)
	{
		m_IsGood = false;
		ReleasePointers();
		return;
	}

	m_IsGood = true;

	uint8_t* b = new uint8_t[size];

	memcpy(b, bytes.data(), size);

	SetData(b, size);
}

bool Script::CompareCode(const Script& other)
{		
	if (memcmp((uint8_t*)m_pSrcCodeLenght - 8, (uint8_t*)other.m_pSrcCodeLenght - 8, (m_pStr - m_data) - 4) != 0) return false;

	if (*m_pStrCount != *(other.m_pStrCount)) return false;

	return true;
}

std::vector<std::string> Script::GetText()
{
	std::vector<std::string> text;	

	for (size_t i = 0; i < *m_pStrCount; ++i)
	{
		text.push_back((const char*)(m_pStrPointers[i] + m_pStartText));
	}

	return text;
}

bool Script::IsGood()
{
	return m_IsGood;
}

bool Script::HaveText()
{
	if (!IsGood())
		return false;

	return *m_pStrCount;
}

size_t Script::Count() const
{
	return *m_pStrCount;
}

void Script::GetPointers()
{	
	if (m_data == nullptr)
		return;

	//This is very close to the way it is done in GBA
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

void Script::ReleasePointers()
{
	m_data = nullptr;
	m_pRiffLenght = nullptr;
	m_pSrcCodeLenght = nullptr;
	m_pStr = nullptr;
	m_pStrCount = nullptr;
	m_pStrLenght = nullptr;
	m_pStrPointers = nullptr;
	m_pStartText = nullptr;
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

const char* Script::operator[](int index) const
{
	return (const char*)(m_pStartText + m_pStrPointers[index]);
}

bool Script::operator==(const Script& other) const
{
	if (*(other.m_pStrCount) != *m_pStrCount)
		return false;

	bool flag = true;

	for (size_t C = 0; C < *m_pStrCount; C++)
	{
		if (other[C] != this->operator[](C))
		{
			flag = false;
			break;
		}
	}

	return flag;
}

bool Script::operator!=(const Script& other) const
{
	return !(*this == other);
}