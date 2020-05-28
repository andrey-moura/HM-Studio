#include "class_util_string.hpp"

std::vector<size_t> StringUtil::FindAll(const std::string& str, const std::string& toSearch)
{
	std::vector<size_t> output;

	size_t pos = str.find(toSearch);

	int size = toSearch.size();

	while (pos != std::string::npos)
	{
		output.push_back(pos);
		pos = str.find(toSearch, pos + size);
	}

	return output;
}

std::vector<std::string_view> StringUtil::SplitLines(const std::string& s)
{
	const char endLine[] = "\r\n";
	size_t npos = std::string::npos;
	
	std::vector<std::string_view> output;

	for (size_t first = s.find_first_not_of(endLine, 0); first != npos; first = s.find_first_not_of(endLine, first))
	{		
		size_t last = s.find_first_of(endLine, first);		

		output.push_back(std::string_view(s.data() + first, last != npos ? last - first : s.size() - first));

		first = last;
	}

	return output;
}

//void StringUtil::Replace(const std::string& find, const std::string& replace, std::string& str)
//{
//	if (find.size() == 0 || replace.size() == 0 || str.size() == 0)
//		return;
//
//	if (find == replace)
//		return;
//
//	if (find.size() == replace.size())
//	{
//		std::vector<size_t> vec = FindAll(str, find);		
//
//		for (size_t pos : vec)
//		{
//			str.replace(pos, find.size(), replace);
//		}
//	}
//	else {			
//		size_t pos = str.find(find, 0);
//		size_t findSize = find.size();
//
//		while (pos != std::string::npos)
//		{
//			std::string newString;
//			newString.resize(pos);
//
//			memcpy((void*)newString.data(), str.data(), pos);
//			newString.append(replace);
//			newString.append(str.substr(pos + findSize));
//
//			str = newString;
//
//			pos = str.find(find, pos + replace.size());
//		}		
//	}
//}

//void StringUtil::Replace(std::string& string, const char& find, const char& replace)
//{	
//	size_t pos = string.find(find);
//
//	while (pos != std::string::npos)
//	{
//		string[pos] = replace;
//		pos = string.find(find, pos + 1);
//	}
//}
