#pragma once

#define STC_STYLE_TEXT   0
#define STC_STYLE_VAR    1
#define STC_STYLE_SIMBOL 2

#define STC_MARK_ERROR 0
#define STC_MARK_ERROR_MASK 0b00000001

#define STC_INDIC_FIND 0
#define STC_INDIC_SPELL 1

#include <wx/stc/stc.h>
#include <wx/timer.h>
#include <wx/menu.h>

#include "class_finder.hpp"
#include "namespace_math.hpp"

#include "class_spell_checker.hpp"

class STC : public wxStyledTextCtrl
{
public:
	STC(wxWindow* parent, wxWindowID id);
	~STC() = default;
public:
	void SetMaxLineLenght(size_t lenght) { m_MaxLineLenght = lenght; }
	void AddVar(const std::string& var) { m_Vars.push_back(var); }
	void ConvertSelToUpperLower(bool upper);
	bool UserCanAddLine() { return m_UserCanAddLine; }
	void SetUserCanAddLine(bool can);	
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

	wxString m_Eol;
//String short cut
private:
	std::vector <std::string> m_StringsOnKey;
	std::string m_Keys;
public:
	void InsertOnCtrlKey(const std::string& s, char key);

//override
public:
	void SetEOLMode(int eol);
	wxString GetEOL();
private:	
	bool m_NeedToSpell = false;	

	inline void SpellSTC(size_t start, size_t end);
public:
	inline void Highlight(size_t start, size_t lenght, int style);
	inline void HighlightAll(const std::vector<size_t>& indexes, size_t lenght, int style);
	inline void FindAll(size_t start, size_t end, const std::string& s, std::vector<size_t>& result);
private:	
	inline void VerifyLineLenght(size_t line);
	inline void VerifyLineLenghtFromPos(size_t from, size_t to);
	inline void VerifyCurLineLenght();			
	inline void FindAndStyleAllVars(size_t start, size_t end);
//Menu	
private:
	wxMenu* m_pMenu = nullptr;
	size_t m_MenuSize;
	std::vector<std::pair<std::string, size_t>> m_ExtraDics;
	std::string m_ClickedWord;
	std::pair<size_t, size_t> m_ClickedWordPos;	
	__forceinline const void GetClickedWord(const size_t& start, const size_t& end)
	{
		m_ClickedWord = GetTextRange(start, end).ToStdString();
		m_ClickedWordPos.first = start;
		m_ClickedWordPos.second = start;		
	}
	__forceinline void DeleteClickedWord()
	{
		DeleteRange(m_ClickedWordPos.first, m_ClickedWordPos.second - m_ClickedWordPos.first);
		m_ClickedWord.clear();
	}

	size_t m_ID_UPPER;
	size_t m_ID_LOWER;
public:
	void ShowMenu(wxPoint point);
	void SuggestToMenu(wxPoint point);
	void ClearSuggestions();
	void AppendDicToMenu(const std::string& dicName, size_t index);
	void RemoveDicToMenu(size_t index);
//Events
private:
	void OnKeyPress(wxKeyEvent& event);
	void OnStyleNeeded(wxStyledTextEvent& event);
	void OnModified(wxStyledTextEvent& event);	
	void OnMouseRight(wxMouseEvent& event);
	void OnTimer(wxTimerEvent& event);
	void OnUpperLowerCaseClick(wxCommandEvent& event);	
	void OnSuggestionClick(wxCommandEvent& event);
	void OnAddToUserClick(wxCommandEvent& event);
	void OnAddToTempClick(wxCommandEvent& event);	
	void OnAddExtraClick(wxCommandEvent& event);
private:
	static wxBitmap m_sIconError;
private:
	void DoBinds();
	void CreateGUI();
};
