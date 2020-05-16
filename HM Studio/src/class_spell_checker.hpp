#pragma once

#include <string>

#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "class_file.hpp"
#include "class_util_string.hpp"

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
	static std::string m_UserDics;	

	static Hunspell* m_Hunspell;
public:
	static void AddToTemp(const std::string& string);
	static void AddUserDic(const std::string& path);
	static void AddToUser(const std::string& string);
	static bool Spell(const std::string& string);
	static std::vector<std::string> Suggest(const std::string& string);
	static const std::string& GetWordChars();
	
	static void Initialize();

	static bool m_sInitialized;
};