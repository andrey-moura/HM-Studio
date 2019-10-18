#pragma once

#include <string>
#include <vector>

class StringUtil
{
private:
	StringUtil();
	~StringUtil();

public:
	static void FindAllOccurances(std::string str, std::string toSearch, std::vector<size_t> &output);
	static void FindAllOccurances(std::vector<std::string> str, std::string toSearch, std::vector<size_t> &output);
	static void SplitLines(std::string s, std::vector<std::string>& output);
};

