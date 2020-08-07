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
#include <wx/statusbr.h>
#include <wx/../../src/stc/scintilla/include/Scintilla.h>

#include "class_finder.hpp"
#include "namespace_math.hpp"

#include "class_spell_checker.hpp"

enum class StcStatus
{
	SIZE,
	LINE,
	SELECTION,
	COLUMN
};

class STC : public wxStyledTextCtrl
{
public:
	STC(wxWindow* parent, wxWindowID id);
	~STC() = default;
public:
	void SetMaxLineLenght(size_t lenght) { m_MaxLineLenght = lenght; }
	void AddVar(const std::string& var) { m_Vars.push_back(var); }	
	bool UserCanAddLine() { return m_UserCanAddLine; }
	void SetUserCanAddLine(bool can);
	void SetStatusBar(wxStatusBar* statusBar);
	void SetStatusIndex(int index, StcStatus status);
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
	//Returns the start pos of the line
	size_t DeleteLine(size_t ln);
	//Returns the start pos of the line
	size_t CleanLine(size_t ln);
	void ReplaceLineText(const wxString& line, size_t ln);
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

	size_t m_ID_UPPER;
	size_t m_ID_LOWER;
//Status
private:
	int m_StatusIndex[4]{ -1, -1, -1, -1 };
	wxStatusBar* m_pStatusBar = nullptr;

	void UpdateStatusText();
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
	void OnUpdateUI(wxStyledTextEvent& event);
private:
	static wxBitmap m_sIconError;
private:
	void DoBinds();
	void CreateGUI();
};
