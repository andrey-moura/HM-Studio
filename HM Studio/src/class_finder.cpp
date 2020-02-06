#include "class_finder.hpp"

size_t Finder::Find(void* data, size_t dataSize, void* find, size_t findSize, uint32_t start)
{
	std::string_view _data((char*)data, dataSize);
	std::string_view _find((char*)find, findSize);

	return _data.find(_find, start);
}

size_t Finder::FindBack(void* data, size_t dataSize, void* find, size_t findSize, uint32_t start)
{
	std::string_view _data((char*)data, dataSize);
	std::string_view _find((char*)find, findSize);

	return _data.rfind(_find, start);
}

size_t Finder::FindFilledBlock(char fill, size_t count, void* data, size_t size, uint32_t start)
{	
	std::string filledBlock;
	filledBlock.resize(count, fill);
	return Find(data, size, filledBlock.data(), filledBlock.size());
}

size_t Finder::FindFilledAlingBlock(char fill, size_t count, void* data, size_t size, uint32_t start, size_t align)
{	
	size_t offset = start;

	std::string filled;
	filled.resize(count);

	while (1)
	{		
		offset = Find(data, size, filled.data(), count, start);

		if(offset == std::string::npos)
			break;

		size_t aligned = offset;

		if(align != 0)
		while (aligned % align != 0)
		{
			aligned++;
		}

		if (aligned == offset)
			break;

		if ((aligned + count) > size)
		{
			return std::string::npos;
		}

		if (VerifyFilled(fill, (char*)data + offset + count, aligned - offset))
		{
			offset = aligned;
			break;
		}
	}

	return offset;
}

bool Finder::VerifyFilled(char fill, void* data, size_t size)
{
	std::string_view sv((char*)data, size);

	return (sv.find_first_not_of(fill) == std::string::npos);
}