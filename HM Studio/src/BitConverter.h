#pragma once
#include <cstdint>
#include <cstring>

class BitConverter
{
public:
	static uint32_t ToInt32(unsigned char* b, int startIndex);
};

