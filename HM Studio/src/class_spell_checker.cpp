#include "class_spell_checker.hpp"

#ifdef USESPELL
SpellChecker::SpellChecker(const std::string& language, const std::string& dicsFolder) : m_Language(language), Hunspell("", "")
{
	m_FileName.SetPath(dicsFolder);
	m_FileName.SetName(m_Language);

	m_FileName.SetExt("aff");
	std::string affDir = m_FileName.GetFullPath().ToStdString();

	m_FileName.SetExt("dic");
	std::string dicDir = m_FileName.GetFullPath().ToStdString();

	m_FileName.SetExt("usr");

	Hunspell(affDir.c_str(), dicDir.c_str());
		
	add_dic(m_FileName.GetFullPath().ToStdString().c_str());
}

void SpellChecker::add_to_user(const std::string& word)
{
	File::AppendLine(m_FileName.GetFullPath().ToStdString(), word);
}
#endif // USESPELL