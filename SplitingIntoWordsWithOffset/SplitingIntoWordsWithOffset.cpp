// SplitingIntoWordsWithOffset.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <chrono>

#ifdef _DEBUG

int alocatedCount = 0;

void* operator new(size_t size)
{
	++alocatedCount;
	return malloc(size);
}

#endif // Debug

const char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

void DoSpelling(std::string& s);
void BreakLines1(std::string& s);
void BreakLines2(std::string& s);

int main()
{
	std::string pharse = "This is a test string.\nEvery line needs to be a new string.\nThis works?\r\nOr not?";

	int times = 100000;
	

	std::cout << "---------------------------------------------" << "\n";
	std::cout << "Starting method 1 with " << times << " times." << "\n";

    double start = (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count() * 0.001);

	for(int i =0; i < times; ++i)
	BreakLines2(pharse);
	double end = (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count() * 0.001);

	double elapsed = (end - start);

	std::cout << "---------------------------------------------" << "\n";
	std::cout << "End method 1 with " << elapsed << "ms" << "\n";

	std::cout << "---------------------------------------------" << "\n";
	std::cout << "Starting method 2 with " << times << " times." << "\n";

	start = (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count() * 0.001);
	for (int i = 0; i < times; ++i)
	BreakLines2(pharse);
	end = (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count() * 0.001);

	elapsed = (end - start);

	std::cout << "---------------------------------------------" << "\n";
	std::cout << "End method 2 with " << elapsed << "ms" << "\n";

	std::cout << "The are " << alocatedCount << " alocations in this application" << "\n";

	std::getchar();
}

void DoSpelling(std::string& s)
{	
	size_t npos = std::string::npos;

	const char* data = s.c_str();

	for (size_t first = s.find_first_of(letters, 0); first != npos; first = s.find_first_of(letters, first))
	{
		size_t last = s.find_first_not_of(letters, first);
		
		if (last != npos)
		{
			s[last] = 0x00;
		}

		std::cout << (data + first) << "\n";		
		first = last;		
	}
}

void BreakLines2(std::string& s)
{
	const char endLine[2] = { 0x0a, 0x0d };

	size_t npos = std::string::npos;

	for (size_t first = s.find_first_not_of(endLine, 0); first != npos; first = s.find_first_not_of(endLine, first))
	{
		size_t last = s.find_first_of(endLine, first);

		std::string teste = s.substr(first, last != npos ? last - first : s.size() - first);

		//std::cout << s.substr(first, last != npos ? last - first : s.size() - first) << " (new line)" << "\n";

		first = last;
	}
}