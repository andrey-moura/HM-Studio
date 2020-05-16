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

void Table::Input(std::vector<std::string>& texts) const
{	
	for (std::string& text : texts)
	{
		Input(std::string_view(text));
	}
}

void Table::Output(std::vector<std::string>& texts) const
{
	if (m_Left.size() == 0)
		return;

	for (std::string& text : texts)
	{
		Output(text);
	}
}