#include "class_util_wx_file.hpp"

std::vector<uint8_t> FileUtil::ReadAllBytes(std::string path)
{
	std::vector<uint8_t> bytes;

	if (!wxFile::Exists(path))
		return std::vector<uint8_t>();

	wxFile* file = new wxFile();
	file->Open(path);	
	bytes.resize(file->Length());
	file->Read(bytes.data(), file->Length());
	file->Close();
	delete file;
	return bytes;
}

void FileUtil::WriteAllBytes(std::string path, std::vector<uint8_t> bytes)
{
	wxFile* file = new wxFile();
	file->Create(path, true);
	file->Open(path, wxFile::read_write);
	file->Write(bytes.data(), bytes.size());
	delete file;
}

std::string FileUtil::ReadAllText(std::string path)
{
	std::string outPut;

	wxFile* file = new wxFile();
	file->Open(path);
	outPut.resize(file->Length());
	file->Read((void*)outPut.data(), file->Length());
	delete file;

	return outPut;
}

void FileUtil::WriteAllText(std::string path, std::string text)
{
	wxFile* file = new wxFile();
	file->Create(path, true);
	file->Open(path, wxFile::read_write);
	file->Write(text.data(), text.size());
	delete file;
}
