#pragma once
#include <vector>
#include <string>

#include <wx/file.h>

#include "class_util_string.hpp"

class File
{
private:
	File();
	~File();

public:
	static std::vector<uint8_t> ReadAllBytes(const std::string &path);
	static uint8_t* ReadAllBytes(const std::string &path, int& lenght);
	static void WriteAllBytes(const std::string &path, const std::vector<uint8_t> &bytes);
	static void WriteAllBytes(const std::string& path, const uint8_t* bytes, size_t size);
	static std::string ReadAllText(const std::string &path);
	static void WriteAllText(const std::string &path, const std::string &text);
	static void AppendLine(const std::string& path, const std::string& line);
};

