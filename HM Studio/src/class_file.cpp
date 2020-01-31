#include "class_file.hpp"


std::vector<uint8_t> File::ReadAllBytes(const std::string &path)
{
	int size = 0;

	uint8_t* bytes = ReadAllBytes(path, size);

	std::vector<uint8_t> vector;

	if (bytes != nullptr)
	{
		vector.resize(size);

		memcpy(vector.data(), bytes, size);
		delete[] bytes;
	}

	return vector;
}

uint8_t* File::ReadAllBytes(const std::string& path, int& lenght)
{
	wxFile file;	

	if (!file.Exists(path))
		return nullptr;

	file.Open(path);
	lenght = file.Length();

	uint8_t* bytes = new uint8_t[lenght];

	file.Read((char*)bytes, lenght);

	file.Close();

	return bytes;
}

void File::WriteAllBytes(const std::string& path, const std::vector<uint8_t>& bytes)
{
	WriteAllBytes(path, bytes.data(), bytes.size());
}

void File::WriteAllBytes(const std::string& path, const uint8_t* bytes, size_t size)
{
	wxFile file;

	file.Create(path, true);

	file.Open(path, wxFile::read_write);

	file.Write(bytes, size);

	file.Close();
}

std::string File::ReadAllText(const std::string& path)
{
	int size = 0;
	uint8_t* bytes = ReadAllBytes(path, size);
	std::string s;
	s.resize(size);
	memcpy(s.data(), bytes, size);
	delete[] bytes;
	return s;
}

void File::WriteAllText(const std::string& path, const std::string& text)
{
	WriteAllBytes(path, (uint8_t*)text.data(), text.size());
}

void File::AppendLine(const std::string& path, const std::string& line)
{
	wxFile file;
	
	if (!file.Exists(path))
		return;

	file.Open(path, wxFile::read_write);

	file.SeekEnd(0);

	if (file.Length() > 0)
		file.Write(StringUtil::eol.data(), StringUtil::eol.size());

	file.Write(line.data(), line.size());
}