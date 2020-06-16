#include "class_letter.hpp"

Letter::Letter(const std::string& letter) : m_Lines(letter)
{
	
}

std::vector<size_t> Letter::GetLineMatches(const Letter& other)
{
	auto otherLines = Moon::String::ViewLines(other.m_Lines, false);
	auto lines = Moon::String::ViewLines(m_Lines);

	std::vector<size_t> ret;	

	for (size_t lineIndex = 0; lineIndex < lines.size(); ++lineIndex)
	{
		for (size_t otherIndex = 0; otherIndex < otherLines.size(); ++otherIndex)
		{
			ret.push_back(lineIndex);
		}
	}

	return ret;
}