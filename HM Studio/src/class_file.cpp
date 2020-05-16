#include "class_file.hpp"

std::vector<uint8_t> File::ReadAllBytes(const std::string &path)
{
	std::vector<uint8_t> vector;

	wxFile file;

	if (file.Exists(path))
	{
		file.Open(path);
		size_t size = file.Length();
		vector.resize(size);

		file.Read(vector.data(), size);
		file.Close();
	}

	return vector;
}

size_t File::ReadAllBytes(const std::string& path, void** bytes)
{
	wxFile file;

	size_t size = 0;

	if (file.Exists(path))
	{
		file.Open(path);
		size = file.Length();;
		*bytes = malloc(size);
		file.Read(*bytes, size);
		file.Close();		
	}

	return size;
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
	std::string s;

	wxFile file;

	if (file.Exists(path))
	{
		file.Open(path);
		size_t size = file.Length();
		s.resize(size);

		file.Read((void*)s.data(), size);
		file.Close();
	}	
	
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
	{
		file.Create(path, true);
	}

	file.Open(path, wxFile::OpenMode::read_write);	
	
	file.SeekEnd(0);

	if (file.Length() > 0)
		file.Write("\r\n");

	file.Write(line.data(), line.size());
}

void File::CreateDir(const std::string& path)
{
	wxFileName file(path);

	if (!file.DirExists())
		file.Mkdir(511, wxPATH_MKDIR_FULL);
}
