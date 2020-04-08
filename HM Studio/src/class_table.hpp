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
	static void InputTable(const std::string &table, std::string text);
	static void OutPutTable(const std::string &table, std::vector<std::string> &text);
private:
	static void Split(std::string& right, std::string& left, const std::string &table);
	static void Replace(const std::string& right, const std::string& left, std::string& text);
};

