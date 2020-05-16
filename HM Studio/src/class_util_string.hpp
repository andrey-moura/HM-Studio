#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <string_view>

class StringUtil
{
private:
	StringUtil();
	~StringUtil();

public:
	static std::vector<size_t> FindAll(const std::string& str, const std::string& toSearch);
	static std::vector<std::string_view> SplitLines(const std::string& s);
	static void Replace(const std::string& find, const std::string& replace, std::string& str);
	static void Replace(std::string& string, const char& find, const char& replace);	
};

