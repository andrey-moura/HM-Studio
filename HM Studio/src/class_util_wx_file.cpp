#include "class_util_wx_file.hpp"

std::vector<uint8_t> FileUtil::ReadAllBytes(const std::string &path)
{
	std::vector<uint8_t> bytes;

	if (!wxFile::Exists(path))
		return std::vector<uint8_t>();

	wxFile file;
	file.Open(path);	
	bytes.resize(file.Length());
	file.Read(bytes.data(), file.Length());
	file.Close();	
	return bytes;
}

void FileUtil::WriteAllBytes(const std::string& path, const std::vector<uint8_t>& bytes)
{
	wxFile file;
	file.Create(path, true);
	file.Open(path, wxFile::read_write);
	file.Write(bytes.data(), bytes.size());	
}

void FileUtil::WriteAllBytes(const std::string& path, uint8_t* bytes, size_t size)
{
	wxFile file;
	file.Create(path, true);
	file.Open(path, wxFile::read_write);
	file.Write(bytes, size);
}

std::string FileUtil::ReadAllText(const std::string& path)
{
	std::string outPut;

	wxFile file;
	file.Open(path);
	outPut.resize(file.Length());
	file.Read((void*)outPut.data(), file.Length());	

	return outPut;
}

void FileUtil::WriteAllText(const std::string& path, const std::string& text)
{
	wxFile file;
	file.Create(path, true);
	file.Open(path, wxFile::read_write);
	file.Write(text.data(), text.size());	
}
