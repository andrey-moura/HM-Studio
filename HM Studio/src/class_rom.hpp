#pragma once

#include <string>

#include <wx/filename.h>

#include "class_table.hpp"
#include "class_rom_file.hpp"

enum class RomConsole
{
	GBAROM,
	DSROM
};

class Rom : public RomFile
{
public:
	Rom(const std::string& name, const RomConsole& console, bool translated);
private:	
	std::string m_Name;
	Table m_Table;
	RomConsole m_Console;
	std::string m_RootDir;	
	std::string m_State;
public:	
	void PrepareTable();
	void SetRootPath(const std::string& rootFolder);
};

