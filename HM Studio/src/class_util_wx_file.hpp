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
	static std::vector<uint8_t> ReadAllBytes(const std::string &path);
	static void ReadAllBytes(const std::string &path, uint8_t** output, int& lenght);
	static void WriteAllBytes(const std::string &path, const std::vector<uint8_t> &bytes);
	static void WriteAllBytes(const std::string& path, uint8_t* bytes, size_t size);
	static std::string ReadAllText(const std::string &path);
	static void WriteAllText(const std::string &path, const std::string &text);
};

