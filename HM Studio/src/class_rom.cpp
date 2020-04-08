#include "class_rom.hpp"

Rom::Rom(const std::string& name, const RomConsole& console, bool translated) : m_Name(name), m_Console(console)
{
	if (translated)
	{
		m_State = "Translated";
	}
	else
	{
		m_State = "Original";
	}

	m_Name = name;

	if (console == RomConsole::DSROM)
	{

	}
}

void Rom::SetRootPath(const std::string& programPath)
{
	wxFileName fileName(programPath);
	fileName.AppendDir("Rom");
	fileName.SetName(m_Name);

	m_RootDir =
}

void Rom::PrepareTable()
{
	wxFileName tablePath(m_FileName.GetPath());
	tablePath.AppendDir("Table");

	tablePath.SetName(m_State);
	tablePath.SetExt("tbl");

	m_Table.OpenFile(tablePath.GetFullPath().ToStdString());
}