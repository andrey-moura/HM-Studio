#pragma once

#include <string>

#include <wx/filename.h>
#include <wx/confbase.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>

#include <moon/file.hpp>

#define HUNSPELL_STATIC

#ifdef ENABLE_SPELL
#include <hunspell/hunspell.hxx>
#else
//ToDo: change this class to a wrapper to allow disabled, Hunspell or Aspell
class Hunspell
{
private:
	std::string word_chars;
public:
	Hunspell(const char* aff, const char* dic) {}
	std::string& get_wordchars_cpp() { return word_chars; } 
	void add(const std::string& word) {}
	void remove(const std::string& word) {}
	bool spell(const std::string& word) { return true; }
	std::vector<std::string> suggest(const std::string& word) { return std::vector<std::string>(); }
};
#endif

#define USER_DIR 0

class SpellChecker
{
private:
	SpellChecker() = delete;
	~SpellChecker() = delete;
private:
	static std::vector<std::string> m_UserDics;	

	static Hunspell* m_Hunspell;
public:
	static void AddToTemp(const std::string& string);	
	static size_t AddUserDic(const std::string& path);
	static void   RemoveUserDic(size_t index);
	static void AddToUser(const std::string& string, size_t index);
	static bool Spell(const std::string& string);
	static std::vector<std::string> Suggest(const std::string& string);
	static const std::string& GetWordChars();
	static const wxString& GetDictionariesPath();
	static const wxString& GetDictionary();
	static void SetDictionary(const wxString& dictionary);
	static const wxArrayString& GetDictionaries();
	static void Initialize();

	static bool m_sInitialized;
};