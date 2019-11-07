#pragma once

#include <sstream>
#include <iomanip>

#include <wx/file.h>
#include <wx/string.h>
#include <wx/stdpaths.h>

#include "class_util_string.hpp"

enum class LogLevel {

	LEVEL_WARNING,
	LEVEL_ERROR,
	LEVEL_MESSAGE,
	LEVEL_DEBUG,
};

class LogFile
{
private:
	LogFile();
	~LogFile();

private:
	static wxString logPath;
private:
	static wxString GetLogLevel(LogLevel level);
private:
	static void OpenFile(bool override);
	static void OverrideFile();;
private:			
	static wxFile* logFile;
private:
	static void UpdateLog(const wxString& s);
public:
	static void WriteLine(const wxString& s, LogLevel level);
	static void WriteLine(const std::vector<uint8_t>& vector, LogLevel level);
	static void Init(bool overide);
	static void End();
};