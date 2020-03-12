#pragma once

#include <vector>
#include <string>
#include "class_rom.hpp"

class Letter
{
public:
	Letter() = default;
	Letter(Rom& rom, uint32_t startPointers, uint32_t count);
private:
	std::vector<std::string> m_Lines;
	uint32_t m_Start;
	uint32_t m_End;
	uint32_t m_StartPointers;
public:
	std::vector<std::string> GetLines() { return m_Lines; }
	uint32_t GetStart() { return m_Start; }
	uint32_t GetEnd() { return m_End; }
	uint32_t GetStartPointers() { return m_StartPointers; }
};

