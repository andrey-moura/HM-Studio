#pragma once

#define STC_STYLE_TEXT   0
#define STC_STYLE_VAR    1
#define STC_STYLE_SIMBOL 2

#define STC_MARK_ERROR 0
#define STC_MARK_ERROR_MASK 0b00000001

#define STC_INDIC_FIND 0
#define STC_INDIC_SPELL 1

#define STC_EOL_CRLF 0
#define STC_EOL_CR   1
#define STC_EOL_LF   2

#include <wx/stc/stc.h>
#include <wx/timer.h>
#include <wx/menu.h>

#ifdef USESPELL
#define HUNSPELL_STATIC
#include <hunspell.hxx>
#endif

#include "Studio.h"
#include "class_finder.hpp"
#include "namespace_math.hpp"

class STC : public wxStyledTextCtrl
{
public:
	STC(wxWindow* parent, wxWindowID id);
	~STC();
public:
	void SetMaxLineLenght(size_t lenght) { m_MaxLineLenght = lenght; }
	void AddVar(const std::string& var) { m_Vars.push_back(var); }

	bool UserCanAddLine() { return m_UserCanAddLine; }
	void SetUserCanAddLine(bool can);
private:
	wxMenu* m_pMenu = nullptr;
	const size_t m_MenuSize = 5;
	std::vector<size_t> m_MenuIds;

	std::pair<size_t, size_t> m_ClickedWord;
public:
	void ShowMenu(wxPoint point);
	void SuggestToMenu(wxPoint point);
private:	
	void OnMenuClick(wxCommandEvent& event);
private:
	bool m_UserCanAddLine = true;

	size_t m_MaxLineLenght = UINT32_MAX;

	std::vector<std::string> m_Vars;

	size_t m_TypingStart = 0;
	size_t m_TypingEnd = 0;

	wxTimer m_Timer;
	size_t m_TimeToType = 700;
	bool m_Typing = false;

	bool m_NeedStyle = false;

//String short cut
private:
	std::vector <std::string> m_StringsOnKey;
	std::string m_Keys;
public:
	void InsertOnCtrlKey(const std::string& s, char key);
#ifdef USESPELL
private:	
	bool m_NeedToSpell = false;
	Hunspell* m_Hunspell = nullptr;
	bool m_DeleteHunspell;

	inline void SpellSTC();
public:
	void SetHunspell(Hunspell* hunspell, bool canDelete = true);
#endif
public:
	inline void Highlight(size_t start, size_t lenght, int style);
	inline void HighlightAll(const std::vector<size_t>& indexes, size_t lenght, int style);
	inline void FindAll(size_t start, size_t end, const std::string& s, std::vector<size_t>& result);
private:	
	inline void VerifyLineLenght(size_t line);
	inline void VerifyLineLenghtFromPos(size_t from, size_t to);
	inline void VerifyCurLineLenght();			
	inline void FindAndStyleAllVars(size_t start, size_t end);
//Events
private:
	void OnKeyPress(wxKeyEvent& event);
	void OnStyleNeeded(wxStyledTextEvent& event);
	void OnModified(wxStyledTextEvent& event);	
	void OnMouseRight(wxMouseEvent& event);
	void OnTimer(wxTimerEvent& event);
private:
	static wxBitmap m_sIconError;
private:
	void DoBinds();
	void CreateGUI();
};
