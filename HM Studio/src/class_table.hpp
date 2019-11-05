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
	static void InputTable(const std::string &table, std::vector<std::string> &text);
	static void OutPutTable(const std::string &table, std::vector<std::string> &text);
private:
	static void Split(std::vector<char>& right, std::vector<char>& left, const std::string &table);
	static void Replace(const std::vector<char>& right, const std::vector<char>& left, std::vector<std::string>& text);
};

