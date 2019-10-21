#pragma once
#include <vector>
#include <string>

#include <wx/file.h>

class FileUtil
{
private:
	FileUtil();
	~FileUtil();

public:
	static std::vector<uint8_t> ReadAllBytes(std::string path);
	static void WriteAllBytes(std::string path, std::vector<uint8_t> bytes);
	static std::string ReadAllText(std::string path);
	static void WriteAllText(std::string path, std::string text);
};

