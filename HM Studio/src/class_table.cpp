#include "class_table.hpp"

Table::Table(const std::string& path)
{
	OpenFile(path);
}

void Table::OpenFile(const std::string& path)
{
	std::string file = File::ReadAllText(path);	
	std::vector<std::string_view> lines = StringUtil::SplitLines(file);

	uint8_t count = lines.size();

	if (count != m_Left.size())
	{
		m_Left.resize(count);
		m_Right.resize(count);
	}	

	for (uint8_t i = 0; i < count; ++i)
	{
		m_Left[i] = BitConverter::FromHexString(lines[i].data());
		m_Right[i] = lines[i][3];
	}
}

void Table::Input(std::string& text) const
{
	if (m_Left.size() == 0)
		return;

	size_t pos = text.find_first_of(m_Left.c_str());

	while (pos != std::string::npos)
	{
		size_t leftPos = m_Left.find(text[pos]);

		if (leftPos == std::string::npos)
			continue;

		text[pos] = m_Right[leftPos];

		pos = text.find_first_of(m_Left.c_str(), pos + 1);
	}
}

void Table::Input(std::vector<std::string>& texts) const
{
	if (m_Left.size() == 0)
		return;

	for (std::string& text : texts)
	{
		size_t pos = text.find_first_of(m_Left.c_str());

		while (pos != std::string::npos)
		{
			size_t leftPos = m_Left.find(text[pos]);

			if (leftPos == std::string::npos)
				continue;

			text[pos] = m_Right[leftPos];

			pos = text.find_first_of(m_Left.c_str(), pos + 1);
		}
	}
}

void Table::Input(const std::string_view& text) const
{
	if (m_Left.size() == 0)
		return;

	size_t pos = text.find_first_of(m_Left.c_str());

	char* data = (char*)text.data();

	while (pos != std::string::npos)
	{
		size_t leftPos = m_Left.find(text[pos]);

		if (leftPos == std::string::npos)
			continue;

		data[pos] = m_Right[leftPos];

		pos = text.find_first_of(m_Left.c_str(), pos + 1);
	}
}

void Table::Output(std::vector<std::string>& texts) const
{
	if (m_Left.size() == 0)
		return;

	for (std::string& text : texts)
	{
		size_t pos = text.find_first_of(m_Right.c_str());

		while (pos != std::string::npos)
		{
			size_t rightPos = m_Right.find(text[pos]);

			if (rightPos == std::string::npos)
				continue;

			text[pos] = m_Left[rightPos];

			pos = text.find_first_of(m_Right.c_str(), pos + 1);
		}
	}
}

void Table::Output(std::string& text) const
{
	if (m_Left.size() == 0)
		return;

	size_t pos = text.find_first_of(m_Right.c_str());

	while (pos != std::string::npos)
	{
		size_t rightPos = m_Right.find(text[pos]);

		if (rightPos == std::string::npos)
			continue;

		text[pos] = m_Left[rightPos];

		pos = text.find_first_of(m_Right.c_str(), pos + 1);
	}
}