#include "class_bit_converter.hpp"

char* BitConverter::s_Nibbles = new char[16]{ '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

std::string BitConverter::ToHexString(uint32_t value)
{
	std::string offsetText(8, '0');

	uint32_t mask = 0xF0000000;
	uint8_t index = 28;

	for (size_t i = 0; i < 8; ++i)
	{
		offsetText[i] = s_Nibbles[(value & mask) >> index];
		mask = mask >> 4;
		index -= 4;
	}

	return offsetText;
}

std::string BitConverter::ToHexString(uint8_t value)
{
	std::string byteText(2, '0');
	byteText[0] = s_Nibbles[(value & 0xF0) >> 4];
	byteText[1] = s_Nibbles[value & 0x0F];	

	return byteText;
}

std::string BitConverter::ToBooleanString(bool value)
{
	return std::string(value ? "True" : "False");
}

uint8_t BitConverter::ReverseBits(const uint8_t& value)
{	
	uint8_t output = value;

	output = (output & 0xF0) >> 4 | (output & 0x0F) << 4;
	output = (output & 0xCC) >> 2 | (output & 0x33) << 2;
	output = (output & 0xAA) >> 1 | (output & 0x55) << 1;

	return output;
}