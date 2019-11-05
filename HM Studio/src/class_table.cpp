#include "class_table.hpp"

void Table::InputTable(const std::string &table, std::vector<std::string> &text)
{	
	std::vector<char> right_char;
	std::vector<char> left_char;

	Split(right_char, left_char, table);
	Replace(right_char, left_char, text);	
}

void Table::OutPutTable(const std::string& table, std::vector<std::string>& text)
{
	std::vector<char> right_char;
	std::vector<char> left_char;

	Split(right_char, left_char, table);
	Replace(left_char, right_char, text);
}

void Table::Split(std::vector<char>& right, std::vector<char>& left, const std::string &table)
{
	std::vector<std::string> lines;
	StringUtil::SplitLines(table, lines);

	size_t* buf = nullptr;

	for (int i = 0; i < lines.size(); ++i)
	{
		std::string left_char_byte;

		left_char_byte = lines[i][0];
		left_char_byte += lines[i][1];

		right.push_back(lines[i][3]);

		left.push_back(std::stoi(left_char_byte, buf, 16));
	}

	delete buf;
}

void Table::Replace(const std::vector<char>& replace, const std::vector<char>& search, std::vector<std::string>& text)
{
	for (int z = 0; z < text.size(); ++z)
	{
		for (int i = 0; i < text[z].size(); ++i)
		{
			for (int y = 0; y < search.size(); ++y)
			{
				if (text[z][i] == search[y])
				{
					text[z][i] = replace[y];
					break;
				}
			}
		}
	}
}

//This is for 16 bits table. Need the replace function
/*
for (int i = 0; i < lines.size(); ++i)
	{
		std::string right_chars;
		std::string left_chars;
		std::string left_chars_bytes;

		bool addNew = false;

		for (int y = 0; y < lines[i].size(); ++y)
		{
			if (lines[i][y] != '=' && !addNew)
			{
				left_chars_bytes += lines[i][y];
				continue;
			}
			else if(lines[i][y] == '=')
			{
				addNew = true;
				continue;
			}

			if(addNew)
				right_chars += lines[i][y];
		}

		if (i == 2)
		{
			std::string();
		}

		for (int y = 0; y < left_chars_bytes.size(); y += 2)
		{
			std::string this_bytes;
			this_bytes += left_chars_bytes[y];
			this_bytes += left_chars_bytes[y + 1];

			char this_char = std::stoi(this_bytes, (size_t*)nullptr, 16);
			left_chars += this_char;
		}

		//text[0].replace()

		text[0] = left_chars + '=';
		text[1] = right_chars;
	}
*/