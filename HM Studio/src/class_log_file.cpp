#include "class_log_file.hpp"

wxFile* LogFile::logFile = new wxFile();
wxString LogFile::logPath = wxString("log.txt");

LogFile::LogFile()
{

}

LogFile::~LogFile()
{
}

wxString LogFile::GetLogLevel(LogLevel level)
{
	switch (level)
	{
	case LogLevel::LEVEL_WARNING:
		return "Warning: ";
		break;
	case LogLevel::LEVEL_ERROR:
		return "Error: ";
		break;
	case LogLevel::LEVEL_MESSAGE:
		return "Message: ";
		break;
	case LogLevel::LEVEL_DEBUG:
		return "Debug: ";
		break;
	default:
		return "Debug: ";
		break;
	}

	return "Debug: ";
}

void LogFile::OpenFile(bool override)
{
	if (override)
	{
		OverrideFile();
	}
	else {
		if (!wxFile::Exists(logPath))
			OverrideFile();		
	}	

	logFile->Open(logPath, wxFile::OpenMode::read_write);	
}

void LogFile::OverrideFile()
{
	logFile->Create(logPath, true);
}

void LogFile::UpdateLog(const wxString& s)
{
	wxString log = wxString();
	int size = logFile->Length();
	log.resize(size);
	logFile->Read((void*)(log.data().AsChar()), size);
	logFile->Write(s);
	logFile->Flush();
}

void LogFile::WriteLine(const wxString& s, LogLevel level)
{	
	UpdateLog(GetLogLevel(level) + s + "\n");
}

void LogFile::WriteLine(const std::vector<uint8_t>& vector, LogLevel level)
{
	std::stringstream stream;
	stream << std::hex << std::setfill('0');

	stream << "vector = { ";

	for (size_t i = 0; i < vector.size(); ++i)
	{
		stream << std::setw(2) << (unsigned)vector[i];
		stream << ", ";
	}

	std::string s = stream.str();

	{
		std::string buf = stream.str();
		s = buf.substr(0, buf.size() - 2);
	}

	s.append(1, ' ');
	s.append(1, '}');

	WriteLine(s, level);
}

void LogFile::Init(bool override)
{
	OpenFile(override);	
}

void LogFile::End()
{
	logFile->Close();
	
	delete logFile;	
}