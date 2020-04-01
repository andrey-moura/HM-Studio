#pragma once

#include <string_view>
#include <vector>

class Finder
{
private:
	Finder() = default;
	~Finder() = default;

public:
	static size_t Find(void* data, size_t dataSize, void* find, size_t findSize, uint32_t start = 0);
	static void FindAll(void* data, size_t dataSize, void* find, size_t findSize, std::vector<size_t>& output, uint32_t start = 0);
	static size_t FindBack(void* data, size_t dataSize, void* find, size_t findSize, uint32_t start = 0);
	static size_t FindFilledBlock(char fill, size_t count, void* data, size_t size, uint32_t start = 0);
	static size_t FindFilledAlingBlock(char fill, size_t count, void* data, size_t size, uint32_t dtart = 0, size_t align = 0);
	static bool VerifyFilled(char fill, void* data, size_t size);	
};

