#pragma once

#include <string>
#include <vector>

#include "class_util_string.hpp"
#include "class_file.hpp"
#include "class_bit_converter.hpp"

class Table
{
public:
	Table(const std::string& path);
	Table() = default;
	~Table() = default;
private:	
	std::string m_Left;
	std::string m_Right;
public:
	void OpenFile(const std::string& path);

	void Input(std::string& text) const;
	void Input(std::vector<std::string>& texts) const;
	void Output(std::string& text) const;
	void Output(std::vector<std::string>& texts) const;
};

