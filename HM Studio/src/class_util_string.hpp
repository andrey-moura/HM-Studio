#pragma once

#include <string>
#include <vector>
#include <string_view>

class StringUtil
{
private:
	StringUtil();
	~StringUtil();

public:
	static void FindAllOccurances(const std::string& str, const std::string& toSearch, std::vector<size_t>& output);
	static void FindAllOcurrances(std::string* start, std::size_t count, const std::string& toSearch, std::vector<size_t>& output);
	static void FindAllOccurances(const std::vector<std::string>& str, const std::string& toSearch, std::vector<size_t>& output);	
	static void SplitLines(const std::string& s, std::vector<std::string>& output);
	static void Replace(const std::string& find, const std::string& replace, std::string& str);
};

