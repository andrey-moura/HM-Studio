#include "class_table.hpp"

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