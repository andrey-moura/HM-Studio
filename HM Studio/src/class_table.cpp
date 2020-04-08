#include "class_table.hpp"

Table::Table(const std::string& path)
{
	OpenFile(path);
}

void Table::OpenFile(const std::string& path)
{
	m_Path = path;

	std::vector<std::string> lines;
	StringUtil::SplitLines(File::ReadAllText(path), lines);

	m_Left.reserve(lines.size());
	m_Right.reserve(lines.size());

	for (const std::string& line : lines)
	{
		m_Left.push_back(std::stoi(line, nullptr, 16));
		m_Right.push_back(line[3]);
	}
}

std::string Table::InputText(std::string& text)
{
	if (text.size() == 0)
		return;

	Replace(m_Right, m_Left, text);
}

std::string Table::OutputText(std::string& text)
{
	if (text.size() == 0)
		return;

	Replace(m_Left, m_Right, text);
}

void Table::InputTable(const std::string &table, std::vector<std::string> &text)
{	
	if (text.size() <= 0 && table.size() < 3)
		return;

	std::string right_char;
	std::string left_char;

	Split(right_char, left_char, table);

	for (std::string& thisText : text)
		Replace(right_char, left_char, thisText);
}

void Table::InputTable(const std::string& table, std::string text)
{
	if (text.size() <= 0 && table.size() < 3)
		return;

	std::string right_char;
	std::string left_char;

	Split(right_char, left_char, table);

	Replace(right_char, left_char, text);
}

void Table::OutPutTable(const std::string& table, std::vector<std::string>& text)
{
	if (text.size() <= 0 && table.size() < 3)
		return;

	std::string right_char;
	std::string left_char;

	Split(right_char, left_char, table);

	for (std::string& thisText : text)
		Replace(left_char, right_char, thisText);
}

void Table::Split(std::string& right, std::string& left, const std::string &table)
{
	std::vector<std::string> lines;
	StringUtil::SplitLines(table, lines);

	right.reserve(lines.size());
	left.reserve(lines.size());

	for (size_t i = 0; i < lines.size(); ++i)
	{
		std::string left_char_byte;
		left_char_byte.reserve(2);

		left_char_byte = lines[i][0];
		left_char_byte += lines[i][1];		

		right.push_back(lines[i][3]);
		left.push_back(std::stoi(left_char_byte, nullptr, 16));
	}	
}

void Table::Replace(const std::string& replace, const std::string& search, std::string& text)
{
	for (char& c : text)
	{
		size_t pos = search.find(c);

		if (pos != std::string::npos)
		{
			c = replace[pos];
		}
	}
}