#pragma once

#include <string>

#include <wx/filename.h>
#include <wx/stdpaths.h>

#include <moon/file.hpp>

#define HUNSPELL_STATIC

#include <hunspell.hxx>

#define USER_DIR 0

#define LANG "pt_BR"

class SpellChecker
{
private:
	SpellChecker() = delete;
	~SpellChecker() = delete;
private:
	static std::string m_Language;
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
	
	static void Initialize();

	static bool m_sInitialized;
};