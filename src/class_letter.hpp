#pragma once

#include <vector>
#include <string>
#include "class_rom_file.hpp"

class Letter
{
public:
	Letter() = default;
	Letter(const std::string& letter);
private:
	std::string m_Lines;
public:
	std::vector<size_t> GetLineMatches(const Letter& other);
};

