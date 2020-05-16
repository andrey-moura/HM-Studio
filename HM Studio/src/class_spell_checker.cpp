#include "class_spell_checker.hpp"

std::string SpellChecker::m_Language = LANG;
std::string SpellChecker::m_UserDics;
bool SpellChecker::m_sInitialized = false;
Hunspell* SpellChecker::m_Hunspell = nullptr;

#ifdef _DEBUG
void SpellChecker::Initialize() {}

void SpellChecker::AddUserDic(const std::string& path) {}

void SpellChecker::AddToTemp(const std::string& string) {}

bool SpellChecker::Spell(const std::string& string) { return true; }

std::vector<std::string> SpellChecker::Suggest(const std::string& string) { return std::vector<std::string>(); }

void SpellChecker::AddToUser(const std::string& string) {}

const std::string& SpellChecker::GetWordChars() { return m_UserDics; }
#else
void SpellChecker::Initialize()
{
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	fileName.AppendDir("SpellChecker");

	fileName.SetName(m_Language);
	fileName.SetExt("aff");

	std::string affDir = fileName.GetFullPath().ToStdString();

	fileName.SetExt("dic");
	std::string dicDir = fileName.GetFullPath().ToStdString();

	if (m_Hunspell != nullptr)
		delete m_Hunspell;

	m_Hunspell = new Hunspell(affDir.c_str(), dicDir.c_str());

	fileName.RemoveLastDir();
	fileName.AppendDir("HM Studio");
	fileName.AppendDir("Dics");
	fileName.SetExt("usr");

	AddUserDic(fileName.GetFullPath().ToStdString());
	AddToTemp("PlayerName");		

	m_Language = m_Hunspell->get_wordchars_cpp();

	m_sInitialized = true;
}

const std::string& SpellChecker::GetWordChars()
{	
	return m_Hunspell->get_wordchars_cpp();
}

void SpellChecker::AddUserDic(const std::string& path)
{
	m_UserDics = path;

	std::string file = File::ReadAllText(path);

	if (file.empty())
		return;

	auto lines = StringUtil::SplitLines(file);

	if (lines.empty())
		return;

	for (const std::string_view& line : lines)
	{
		if (line.empty())
			continue;

		m_Hunspell->add(std::string(line));
	}		
}

void SpellChecker::AddToUser(const std::string& string)
{
	if (string.empty())
		return;

	File::AppendLine(m_UserDics, string);

	m_Hunspell->add(string);
}

void SpellChecker::AddToTemp(const std::string& string)
{
	m_Hunspell->add(string);
}

bool SpellChecker::Spell(const std::string& string)
{
	return m_Hunspell->spell(string);
}

std::vector<std::string> SpellChecker::Suggest(const std::string& string)
{
	return m_Hunspell->suggest(string);
}

#endif