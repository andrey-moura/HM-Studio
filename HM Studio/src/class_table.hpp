#pragma once

#include <string>
#include <vector>

#include "class_util_string.hpp"
#include "class_file.hpp"

class Table
{
public:
	Table(const std::string& path);
	Table() = default;
	~Table() = default;

	void OpenFile(const std::string& path);
private:
	std::string m_Path;
	std::string m_Left;
	std::string m_Right;
public:
	std::string InputText(std::string& text);
	std::string OutputText(std::string& text);
	std::vector<std::string> InputText(const std::vector<std::string>& text);
public:
	static void InputTable(const std::string &table, std::vector<std::string> &text);
	static void InputTable(const std::string &table, std::string text);
	static void OutPutTable(const std::string &table, std::vector<std::string> &text);
private:
	static void Split(std::string& right, std::string& left, const std::string &table);
	static void Replace(const std::string& right, const std::string& left, std::string& text);
};

