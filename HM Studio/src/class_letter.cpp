#include "class_letter.hpp"

Letter::Letter(RomFile& rom, uint32_t startPointers, uint32_t count)
{
	uint32_t* pointers = new uint32_t[count];

	rom.Seek(startPointers);
	rom.Read(pointers, count * 4);

	for (int i = 0; i < count; ++i)
	{
		pointers[i] &= ROM_BUS_NOT;
	}

	m_Start = pointers[0];

	delete[] pointers;
}