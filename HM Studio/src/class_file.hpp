#pragma once
#include <vector>
#include <string>
#include <fstream>

#include <wx/file.h>
#include <wx/filename.h>

#include "class_util_string.hpp"

class File
{
private:
	File() = default;
	~File() = default;

public:	
	static std::vector<uint8_t> ReadAllBytes(const std::string &path);
	static size_t ReadAllBytes(const std::string &path, void** bytes);
	static void WriteAllBytes(const std::string &path, const std::vector<uint8_t> &bytes);
	static void WriteAllBytes(const std::string& path, const uint8_t* bytes, size_t size);
	static std::string ReadAllText(const std::string &path);
	static void WriteAllText(const std::string &path, const std::string &text);
	static void AppendLine(const std::string& path, const std::string& line);
	static void CreateDir(const std::string& path);
};

