#include "class_editor.hpp"

Editor::Editor(const id& id) : m_RomOriginal(id, false), m_RomTranslated(id, true)
{
	
}

RomFile& Editor::GetRom(const bool& translated)
{
	return (translated ? m_RomTranslated : m_RomOriginal);
}

const RomFile& Editor::GetRom(const bool& translated) const
{
	return (translated ? m_RomTranslated : m_RomOriginal);
}

bool Editor::NextFile()
{	
	return Open(m_Number+1);
}

bool Editor::PreviousFile()
{
	return Open(m_Number - 1);
}

FilesResults Editor::FindInFiles(std::string& search, bool useTable, bool translated)
{
	FilesResults results;	

	const char* ignore = "\x5\xff\r\n";

	wxString state = translated ? "Translated" : "Original";

	wxString hit = " hit";
	wxString hits = " hits";

	size_t totalCount = 0;

	wxProgressDialog dialog("Find in scripts in progress...", "", m_Count);
	dialog.Show();

	const Moon::Hacking::Table& table = GetRom(translated).GetTable();

	if (useTable)
	{
		table.Output(search);
	}

	for (size_t cur_letter = 0; cur_letter < m_Count; ++cur_letter)
	{
		SearchResult match;

		wxString name = wxString(m_Type) << state << L"_" << cur_letter << L"." << m_FileExt;
		dialog.Update(cur_letter, name);

		//ToDo
		//Make work for all type of editors
		auto lines = Moon::File::ReadAllLines(GetPath(cur_letter, translated));

		for (size_t cur_line = 0; cur_line < lines.size(); ++cur_line)
		{
			size_t pos = lines[cur_line].find(search);

			if (pos == std::string::npos)
				continue;

			match.AppendResult(wxString(L"Line: ") << cur_line << ": " << lines[cur_line]);
		}

		size_t count = match.GetCount();

		if (count == 0)
			continue;

		totalCount += count;

		match.SetTitle(name << " (" << count << (count == 1 ? hit : hits) << ")");
		results.AppendMatch(match);
	}

	dialog.Close();

	results.SetSearchTitle(wxString("Search \"") << search << "\" (" << totalCount << " " << (totalCount == 1 ? hit : hits) << " in " << results.GetCount() << " script" << (results.GetCount() > 1 ? "s)" : ")"));

	return results;
}

bool Editor::SetIndex(size_t index)
{
	if (index < m_Count)
	{
		m_Index = index;
		return true;
	}

	return false;
}