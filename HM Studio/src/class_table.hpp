#pragma once

#include <string>
#include <vector>

#include "class_util_string.hpp"
#include "class_file.hpp"
#include "class_bit_converter.hpp"

class Table
{
public:
	Table(const std::string& path);
	Table() = default;
	~Table() = default;
private:
	std::string m_Left;
	std::string m_Right;
public:
	void OpenFile(const std::string& path);

	__forceinline void Input(const std::string_view& text) const
	{
		if (m_Left.size() == 0)
			return;

		size_t pos = text.find_first_of(m_Left.c_str());

		char* data = (char*)text.data();

		while (pos != std::string::npos)
		{
			size_t leftPos = m_Left.find(text[pos]);

			if (leftPos == std::string::npos)
				continue;

			data[pos] = m_Right[leftPos];

			pos = text.find_first_of(m_Left.c_str(), pos + 1);
		}
	}
	__forceinline void Input(std::string& text) const
	{
		Input(std::string_view(text));
	}
	void Input(std::vector<std::string>& texts) const;
	__forceinline void Output(const std::string_view& text) const
	{
		if (m_Left.size() == 0)
			return;

		size_t pos = text.find_first_of(m_Right.c_str());

		char* data = (char*)text.data();

		while (pos != std::string::npos)
		{
			size_t rightPos = m_Right.find(text[pos]);

			if (rightPos == std::string::npos)
				continue;

			data[pos] = m_Left[rightPos];

			pos = text.find_first_of(m_Right.c_str(), pos + 1);
		}
	}
	__forceinline void Output(std::string& text) const
	{
		Output(std::string_view(text));
	}
	void Output(std::vector<std::string>& texts) const;
};