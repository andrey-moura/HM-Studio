#pragma once

#include <string>

#include <wx/filename.h>

#include "class_file.hpp"

#ifdef USESPELL

#define HUNSPELL_STATIC

#include <hunspell.hxx>

class SpellChecker : public Hunspell
{
public:
	SpellChecker(const std::string& language, const std::string& dicsFolder);
	SpellChecker() = default;
	~SpellChecker() = default;
private:
	std::string m_Language;
	wxFileName m_FileName;
public:
	void add_to_user(const std::string& word);
};
#else
class SpellChecker
{
public:
	SpellChecker(const std::string& language, const std::string& dicsFolder) {};
	SpellChecker() = default;
	~SpellChecker() = default;
private:
	std::string m_Language;
	wxFileName m_FileName;
public:
	void add_to_user(const std::string& word) {};
};
#endif