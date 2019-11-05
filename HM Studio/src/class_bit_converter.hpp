#pragma once

#include <stdint.h>

class BitConverter
{
private:
	BitConverter() {};
	~BitConverter() {};

public:
	static inline uint32_t ToInt32(const void* data, unsigned int index) { return *(int*)(((char*)data) + index); };
};
