#include "class_util_string.hpp"

void StringUtil::FindAllOccurances(std::string str, std::string toSearch, std::vector<size_t> &output)
{
	size_t pos = str.find(toSearch);

	while (pos != std::string::npos)
	{
		output.push_back(pos);
		pos = str.find(toSearch, pos + toSearch.size());
	}
}

void StringUtil::FindAllOccurances(std::vector<std::string> str, std::string toSearch, std::vector<size_t> &output)
{

	for (int i = 0; i < str.size(); ++i)
	{		
		size_t pos = str[i].find(toSearch);

		if (pos != std::string::npos)		
			output.push_back(pos);
	}
}

void StringUtil::SplitLines(std::string s, std::vector<std::string>& output)
{
	std::string cur_line;

	for (int i = 0; i < s.length(); ++i)
	{		
		if (i == 227)
			std::string();

		if (s[i] == 0x0a)
		{
			output.push_back(cur_line);
			cur_line.clear();
		}
		else if (s[i] == 0x0d)
		{
			continue;
		}
		else {
			cur_line += s[i];
		}
	}

	//This is because... um... well, the for loop was terminated in the last char, and now we need to add the last line...
	output.push_back(cur_line);
}

void StringUtil::Replace(std::string find, std::string replace, std::string& str)
{
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
		if (find == std::string("|²"))
			std::string();

		//int add = (int)replace.size() - (int)find.size();

		size_t pos = str.find(find, 0);

		while (pos != std::string::npos)
		{
			std::string newString;
			newString.resize(pos);

			memcpy((void*)newString.data(), str.data(), pos);
			newString.append(replace);
			newString.append(str.substr(pos + find.size()));

			str = newString;

			pos = str.find(find, pos + find.size());
		}

		//for (int i = 0; i < vec.size(); ++i)
		//{
		//	if (i != 0)
		//		vec[i] += add;

		//	std::string newString;
		//	newString.resize(vec[i]);

		//	memcpy((void*)newString.data(), str.data(), vec[i]);
		//	newString.append(replace);
		//	newString.append(str.substr(vec[i] + find.size()));

		//	str = newString;
		//}
	}
}
