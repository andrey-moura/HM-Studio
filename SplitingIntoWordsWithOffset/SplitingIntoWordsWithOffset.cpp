// SplitingIntoWordsWithOffset.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include <string_view>

#ifdef _DEBUG

int alocatedCount = 0;

void* operator new(size_t size)
{
	++alocatedCount;
	std::cout << alocatedCount << "\n";
	return malloc(size);
}

#endif // Debug

const char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

void DoSpelling(std::string& s);

int main()
{
	std::string pharse = "This is a test string. Note that dot are not word! Th1s is so perfect. Largerword";
	DoSpelling(pharse);

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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
