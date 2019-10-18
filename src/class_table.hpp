#pragma once

#include <string>
#include <vector>

#include "class_util_string.hpp"

class Table
{
private:
	Table();
	~Table();

public:
	static void InputTable(std::string table, std::vector<std::string> &text);
	static void OutPutTable(std::string table, std::vector<std::string>& text);
private:
	static void Split(std::vector<char>& right, std::vector<char>& left, std::string table);
	static void Replace(std::vector<char>& right, std::vector<char>& left, std::vector<std::string>& text);
};

