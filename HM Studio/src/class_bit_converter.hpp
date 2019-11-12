#pragma once

#include <stdint.h>
#include <vector>

static inline uint32_t ToInt32(const void* data, unsigned int index) { return *(int*)(((char*)data) + index); };

static inline void Get2Bits(std::vector<uint8_t>& bits, const uint8_t byte)
{
	bits.reserve(4);
	uint8_t b = byte;

	for (int i = 0; i < 4; ++i)
	{
		bits.push_back(b & 0b00000011);
		b = b >> 2;
	}
}
	
static inline void Get4Bits(uint8_t& left, uint8_t& right, const uint8_t byte)
{
	left = byte >> 4;
	right = byte & 0x0f;
}

static inline void Get8Bits(std::vector<uint8_t>& bits, const uint8_t byte)
{
	bits.resize(8);

	uint8_t b = byte;

	for (int i = 0; i < 8; ++i)
	{
		bits[i] = b & 0b00000001;
		b = b >> 1;
	}		
}

static inline void ReverseNibble(uint8_t& byte)
{
	uint8_t left = 0;
	uint8_t right = 0;

	Get4Bits(left, right, byte);

	byte = 0x0f & left;
	byte = 0xf0 & right;
}