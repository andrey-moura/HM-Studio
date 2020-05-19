#include "class_spell_checker.hpp"

std::string SpellChecker::m_Language;
std::vector<std::string> SpellChecker::m_UserDics;
bool SpellChecker::m_sInitialized = false;
Hunspell* SpellChecker::m_Hunspell = nullptr;

#ifdef _DEBUG
void SpellChecker::Initialize() {}

size_t SpellChecker::AddUserDic(const std::string& path) { return std::string::npos; }

void SpellChecker::AddToTemp(const std::string& string) {}

bool SpellChecker::Spell(const std::string& string) { return true; }

std::vector<std::string> SpellChecker::Suggest(const std::string& string) { return std::vector<std::string>(); }

void SpellChecker::AddToUser(const std::string& string, size_t index) {}

const std::string& SpellChecker::GetWordChars() { return m_Language; }

void SpellChecker::RemoveUserDic(size_t index) {}
#else
void SpellChecker::Initialize()
{
	m_Language = LANG;

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

	m_sInitialized = true;
}

const std::string& SpellChecker::GetWordChars()
{	
	return m_Hunspell->get_wordchars_cpp();
}

size_t SpellChecker::AddUserDic(const std::string& path)
{
	std::string file = File::ReadAllText(path);

	if (!file.empty())
	{
		auto lines = StringUtil::SplitLines(file);

		if (lines.empty())
			return std::string::npos;

		for (const std::string_view& line : lines)
		{
			if (line.empty())
				continue;

			m_Hunspell->add(std::string(line));
		}
	}
	m_UserDics.push_back(path);
	return m_UserDics.size() - 1;
}

void SpellChecker::RemoveUserDic(size_t index)
{
#ifdef _DEBUG
	_STL_Verify(index < m_UserDics.size(), "index out of range");
#endif	
	const std::string& path = m_UserDics[index];
	
	std::string text = File::ReadAllText(path);
	
	if(text.empty())
		return;
	
	auto lines = StringUtil::SplitLines(text);
	
	for(const std::string_view& sv : lines)
	{
		m_Hunspell->remove(std::string(sv));
	}
	
	m_UserDics.erase(m_UserDics.begin()+index);
}

void SpellChecker::AddToUser(const std::string& string, size_t index)
{
	if (string.empty())
		return;

	File::AppendLine(m_UserDics[index], string);

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