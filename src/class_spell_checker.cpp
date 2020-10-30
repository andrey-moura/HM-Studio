#include "class_spell_checker.hpp"

static wxString s_Dictionary;
std::vector<std::string> SpellChecker::m_UserDics;
bool SpellChecker::m_sInitialized = false;
Hunspell* SpellChecker::m_Hunspell = nullptr;
static wxString s_DictionariesPath;
static wxArrayString s_Dictionaries;
static bool s_HasDictionary = false;

void SpellChecker::Initialize()
{
	s_Dictionaries.clear();
	m_UserDics.clear();

	wxConfigBase* pConfig = wxConfigBase::Get();	

	s_DictionariesPath = pConfig->Read(L"/SpellChecker/DictionariesDir", L"");	
	wxFileName fileName;
	wxString exeDir = wxPathOnly(wxStandardPaths::Get().GetExecutablePath());

	//First SpellChecker Initialization.
	//Create default path
	if (s_DictionariesPath.empty())
	{		
		fileName.SetPath(exeDir);
		fileName.AppendDir("SpellChecker");

		s_DictionariesPath = fileName.GetPath();

		pConfig->Write("/SpellChecker/DictionariesDir", s_DictionariesPath);
	}

	wxArrayString dics;
	wxDir::GetAllFiles(s_DictionariesPath, &dics, L"*.dic");

	s_Dictionaries.Add(L"None");

	for (const wxString& dic : dics)
	{
		wxString fileName = wxFileNameFromPath(dic);		
		s_Dictionaries.Add(fileName.substr(0, fileName.find_last_of(L".")));
	}

	dics.clear();

	wxString dic = pConfig->Read(L"/SpellChecker/Dictionary", L"");

	//First SpellChecker Initialization.
	//Create default language	
	if (!dic.empty())
	{
		if (s_Dictionaries.Index(dic) == -1)
		{
			dic = L"None";
		}
	}

	SetDictionary(dic);	

	/*
		For the to do list:

		I have to create a nonstatic SpellChecker, so I can create a instance of the
		Spellchecker to check in the dictionary and another for each ROM and also
		another for checking the HM Studio dictionary.

		This would envolve multiple instances of Hunspell, but a better source code.
	*/

	if (s_Dictionary != L"None")
	{
		//ToDo: Remove this
		fileName.SetPath(exeDir);
		fileName.AppendDir("HM Studio");
		fileName.AppendDir("Dics");
		fileName.SetExt("usr");

		Moon::File::MakeDir(fileName.GetPath().ToStdString());

		AddUserDic(fileName.GetFullPath().ToStdString());		
	}

	AddToTemp("PlayerName");

	m_sInitialized = true;
}

void SpellChecker::SetDictionary(const wxString& dictionary)
{	
	delete m_Hunspell;

	s_Dictionary = dictionary;

	if (s_Dictionary == wxEmptyString)
		s_Dictionary = L"None";

	if (s_Dictionary == L"None")
	{
		m_Hunspell = new Hunspell("", "");
		s_HasDictionary = false;
	}
	else
	{
		wxFileName fn;
		fn.SetPath(s_DictionariesPath);
		fn.SetName(s_Dictionary);
		fn.SetExt(L"dic");

		if (!fn.FileExists())
		{
			m_Hunspell = new Hunspell("", "");
			s_HasDictionary = false;
			s_Dictionary = L"None";			
		}
		else
		{
			wxString dic = fn.GetFullPath();

			fn.SetExt(L"aff");

			if (!fn.FileExists())
			{
				m_Hunspell = new Hunspell("", "");
				s_HasDictionary = false;
				s_Dictionary = L"None";
			}
			else
			{
				wxString aff = fn.GetFullPath();

				m_Hunspell = new Hunspell(aff.ToStdString().c_str(), dic.ToStdString().c_str());
				s_HasDictionary = true;
			}
		}		
	}

	wxConfigBase* pConfig = wxConfigBase::Get();
	pConfig->Write("/SpellChecker/Dictionary", s_Dictionary);
	pConfig->Flush();
}

const wxString& SpellChecker::GetDictionary()
{
	return s_Dictionary;
}

const wxArrayString& SpellChecker::GetDictionaries()
{
	return s_Dictionaries;
}

const std::string& SpellChecker::GetWordChars()
{	
	return m_Hunspell->get_wordchars_cpp();
}

const wxString& SpellChecker::GetDictionariesPath()
{
	return s_DictionariesPath;
}

size_t SpellChecker::AddUserDic(const std::string& path)
{
	std::string file = Moon::File::ReadAllText(path);

	if (!file.empty())
	{
		auto lines = Moon::String::ViewLines(file, false);

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
	const std::string& path = m_UserDics[index];
	
	std::string text = Moon::File::ReadAllText(path);
	
	if(text.empty())
		return;
	
	auto lines = Moon::String::ViewLines(text);
	
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

	Moon::File::AppendLine(m_UserDics[index], string);

	m_Hunspell->add(string);
}

void SpellChecker::AddToTemp(const std::string& string)
{
	m_Hunspell->add(string);
}

bool SpellChecker::Spell(const std::string& string)
{
	return m_Hunspell->spell(string) || !s_HasDictionary;
}

std::vector<std::string> SpellChecker::Suggest(const std::string& string)
{
	return m_Hunspell->suggest(string);
}