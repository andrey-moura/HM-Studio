#include "class_util_string.hpp"

void StringUtil::FindAllOccurances(const std::string& str, const std::string& toSearch, std::vector<size_t>& output)
{
	size_t pos = str.find(toSearch);

	int size = toSearch.size();

	while (pos != std::string::npos)
	{
		output.push_back(pos);
		pos = str.find(toSearch, pos + size);
	}
}

void StringUtil::FindAllOcurrances(std::string* start, std::size_t count, const std::string& toSearch, std::vector<size_t>& output)
{

}

void StringUtil::FindAllOccurances(const std::vector<std::string>& str, const std::string& toSearch, std::vector<size_t>& output)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		size_t pos = str[i].find(toSearch);

		if (pos != std::string::npos)
			output.push_back(i);
	}
}

size_t StringUtil::FindFirstOf(const char* find, const std::string& target, const size_t startIndex)
{
	size_t npos = std::string::npos;

	if (find == nullptr)
		return npos;
	if (target.size() == 0)
		return npos;

	size_t findSize = std::strlen(find);

	size_t index = 0;

	for (size_t i = 0; i < findSize; ++i)
	{
		size_t curIndex = target.find(find[i], startIndex);

		if (curIndex < index && curIndex != npos)	index = curIndex;
	}

	return index;
}

void StringUtil::SplitLines(const std::string& s, std::vector<std::string>& output)
{
	const char endLine[] = "\r\n";

	size_t npos = std::string::npos;

	for (size_t first = s.find_first_not_of(endLine, 0); first != npos; first = s.find_first_not_of(endLine, first))
	{		
		size_t last = s.find_first_of(endLine, first);		

		output.push_back(s.substr(first, last != npos ? last - first : s.size() - first));

		first = last;
	}
}

void StringUtil::Replace(const std::string& find, const std::string& replace, std::string& str)
{
	if (find.size() <= 0 || replace.size() <= 0 || str.size() <= 0)
		return;

	if (find.size() == replace.size())
	{
		std::vector<size_t> vec;
		FindAllOccurances(str, find, vec);

		for (size_t pos : vec)
		{
			str.replace(pos, find.size(), replace);
		}
	}
	else {

		size_t pos = str.find(find, 0);
		int size = find.size();

		while (pos != std::string::npos)
		{
			std::string newString;
			newString.resize(pos);

			memcpy((void*)newString.data(), str.data(), pos);
			newString.append(replace);
			newString.append(str.substr(pos + size));

			str = newString;

			pos = str.find(find, pos + find.size());
		}
	}
}
