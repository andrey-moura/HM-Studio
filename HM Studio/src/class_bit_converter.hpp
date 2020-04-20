#pragma once

#include <stdint.h>

#include <string>

class BitConverter
{
private:
	BitConverter() = default;
	~BitConverter() = default;
public:
	static std::string ToHexString(uint32_t value);
	static std::string ToHexString(uint8_t value);
	static std::string ToBooleanString(bool value);
	static uint8_t ReverseBits(const uint8_t& value);
private:
	static char* s_Nibbles;
};