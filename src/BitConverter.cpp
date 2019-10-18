#include "BitConverter.h"

uint32_t BitConverter::ToInt32(unsigned char* b, int startIndex)
{
	int outuput = 0;
	memcpy(&outuput, b + startIndex, 4);
	return outuput;
}