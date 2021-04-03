#pragma once

#include <thread>
#include <sstream>

#include <wx/frame.h>
#include <wx/clipbrd.h>
#include <wx/txtstrm.h>
#include <wx/textbuf.h>
#include <wx/string.h>

#include "window_find_results.hpp"
#include "frame_search_script.hpp"
#include "dialog_dump_insert.hpp"

#include "class_rom_file.hpp"
#include "class_script.hpp"
#include "class_stc.hpp"
#include "class_script_editor.hpp"
#include "class_spell_checker.hpp"
#include "frame_editor.hpp"

#define wxSTC_WRAP_STYLE_DEFAULT 0
#define wxSTC_WRAP_STYLE_STRING 1
#define wxSTC_WRAP_STYLE_COMMENT 2
#define wxSTC_WRAP_STYLE_NUMBER 3
#define wxSTC_WRAP_STYLE_KEYWORD 4

wxDECLARE_EVENT(wxEVT_STC_WRAP_WORD, wxStyledTextEvent);

//This class will replace STC 
class wxStyledTextWrapper : public wxStyledTextCtrl
{
public:
	wxStyledTextWrapper(wxWindow* parent, wxWindowID id)
		: wxStyledTextCtrl(parent, id) {

		StyleSetForeground(wxSTC_WRAP_STYLE_COMMENT, wxColour(0, 128, 0));
		StyleSetForeground(wxSTC_WRAP_STYLE_STRING, wxColour(128, 0, 0));
		StyleSetForeground(wxSTC_WRAP_STYLE_NUMBER, wxColour(255, 128, 0));		

		StyleSetForeground(wxSTC_WRAP_STYLE_KEYWORD, wxColour(0, 58, 255));
		Bind(wxEVT_STC_STYLENEEDED, &wxStyledTextWrapper::OnStyleNeeded, this);

		for(wchar_t c = L'a'; c <= 'z'; ++c) {
			m_wordChars.append(c);
		}
		for(wchar_t c = L'A'; c <= 'Z'; ++c) {
			m_wordChars.append(c);
		}
		for(wchar_t c = L'0'; c <= '9'; ++c) {
			m_wordChars.append(c);
		}

		m_wordChars.append(L'_');
	}
private:
	wxString m_lineComment;
	std::vector<std::vector<wxString>> m_keywords;
	wxString m_wordChars;
private:
	size_t FindKeyWord(const wxString& word)
	{
		for(int i = 0; i < m_keywords.size(); ++i) {
			if(std::find(m_keywords[i].begin(), m_keywords[i].end(), word) != m_keywords[i].end()) {
				return i;
			}
		}		

		return std::string::npos;
	}
public:
	void DisplayLineNumbers(int margin = 0) {
		SetMarginWidth(margin, 32);
		SetMarginType(margin, wxSTC_MARGIN_NUMBER);
	}
	void SetBaseLexer(int lexer = 0) {
		SetLexer(lexer);
	}
	void SetLineCommentKey(const wxString& key) {
		m_lineComment = key;
	}
	void AppendKeyWord(const wxString& key, int index) {
		if(m_keywords.size() <= index) {
			m_keywords.resize(index+1);
		}
			
		m_keywords[index].push_back(key);
	}
	void SetKeyWordForeground(int index, const wxColour& fore)
	{
		StyleSetForeground(index+wxSTC_WRAP_STYLE_KEYWORD, fore);
	}
	void CheckWordAndApplyStyle(const wxString& word, int pos) {
		int style = wxSTC_WRAP_STYLE_DEFAULT;
		size_t key = FindKeyWord(word);
		if(key != std::string::npos) {
			style = key+wxSTC_WRAP_STYLE_KEYWORD;
		}
		else if(word.starts_with("0x")) {
			bool is_hex = true;
			for(int i = 2; i < word.size(); ++i) {
				if(!isxdigit(word[i])) {
					is_hex = false;
					break;
				}
			}
			if(is_hex) {
				style = wxSTC_WRAP_STYLE_NUMBER;
			}
		} else if(word.IsNumber()) {
			style = wxSTC_WRAP_STYLE_NUMBER;
		} else  {
			wxStyledTextEvent event(wxEVT_STC_WRAP_WORD, GetId());
			event.SetString(word);
			event.SetPosition(pos);

			ProcessWindowEvent(event);

			if(!event.GetSkipped()) {
				return;
			}
		}

		int start = pos-word.size();
		StartStyling(start);
		SetStyling(word.size(), style);
	}
	void OnStyleNeeded(wxStyledTextEvent& event) {
		int start = 0;
		int end = GetLength();

		std::cout << "Style needed from " << start << " to " << end;

		int line_start = LineFromPosition(start);
		int line_end = LineFromPosition(end);

		int line_start_pos = PositionFromLine(line_start);
		int line_end_pos = GetLineEndPosition(line_end);

		if(line_start_pos > line_end_pos) {
			std::swap(line_start_pos, line_end_pos);
		}

		bool found_string = false;
		bool escape_character = false;
		bool last_zero = false;
		bool allow_hex = false;
		int string_start;

		wxString word;
		int start_word = line_start_pos;

		//clear all style
		StartStyling(line_start_pos);
		SetStyling(line_end_pos-line_start_pos, 0);

		for(int current_style = line_start_pos; current_style < line_end_pos; ++current_style) {
			wchar_t c = GetCharAt(current_style);
			if(found_string) {						
				if(c == L'\"' || current_style == line_end_pos-1) {					
					StartStyling(string_start);
					SetStyling((current_style-string_start)+1, wxSTC_WRAP_STYLE_STRING);
					if(c == L'\"' && escape_character) {
						escape_character = false;
						continue;
					}
					found_string = false;
					start_word = current_style+1;
				} else if(c == L'\\') {
					escape_character = true;
					continue;
				}
				escape_character = false;
			}
			else {
				if(c == L'\"') {
					found_string = true;
					escape_character = true;
					string_start = current_style;
					last_zero = false;
					CheckWordAndApplyStyle(word, current_style);
					word.clear();					
				} else if(m_lineComment == c) {
					int cur_line = LineFromPosition(current_style);
					int end_styling = GetLineEndPosition(cur_line);
					StartStyling(current_style);
					SetStyling(end_styling-current_style, wxSTC_WRAP_STYLE_COMMENT);					
					CheckWordAndApplyStyle(word, current_style);
					word.clear();
					current_style = end_styling;
					last_zero = false;
			    } else {
					// if(isdigit(c) || (allow_hex && isxdigit(c))) {
					// 	if(c == '0') {
					// 		last_zero = true;
					// 	}

					// 	StartStyling(current_style);
					// 	SetStyling(1, wxSTC_WRAP_STYLE_NUMBER);
					// 	continue;
					// }
					
					// allow_hex = false;

					if(m_wordChars.find(c) == std::string::npos) {
						word.clear();
						start_word = current_style+1;
						continue;
					}

					// if(c == 'x' && last_zero) {
					// 	allow_hex = true;
					// 	StartStyling(current_style);
					// 	SetStyling(1, wxSTC_WRAP_STYLE_NUMBER);
					// 	continue;				
					// }

					//last_zero = false;

					word.append(c);
					CheckWordAndApplyStyle(word, current_style+1);					
				}
			}
		}

		event.Skip();
	}
private:
};

class ScriptEditorFrame : public EditorFrame
{
public:
	ScriptEditorFrame(id i);
	~ScriptEditorFrame();	

	void SetupRom(); 
//Events
private:
	void OnResultClick(wxCommandEvent& event);
	void OnConvertEOLClick(wxCommandEvent& event);
	void OnCheckCodeClick(wxCommandEvent& event);
	void OnSetEventClick(wxCommandEvent& event);
	void SetTextRange();
	void OnHorizontalModeClick(wxCommandEvent& event);
	void OnCodeStyleNeeded(wxStyledTextEvent& event);	
	void OnDisassemblyWord(wxStyledTextEvent& event);
private:
//Text save
	void ConfigureSTC(STC* stc, const RomFile& rom);	
	size_t GetNumberFromResult(const SearchResult& result);
	size_t GetIndexFromResult(const SearchResult& result, size_t clicked);
private:
	void BackupText();
	void RestoreText();
//Script manipulation
private:
	void OpenScript(size_t index);	
	void UpdateScript();	
	void FindText();
public:
	void ScriptTextRange(size_t from, size_t to, size_t script);
public:
	virtual void OnSaveString() override;
	virtual void UpdateText() override;
	virtual void UpdateFile() override;
	virtual void OnSaveFile() override;	
	virtual void OnInsertFile() override;
//Forms

//Text manipulation
private:	
	std::vector<size_t> m_FindPos;
	size_t m_FindIndex;	
	wxFileName m_ScriptDic;
	size_t m_DicIndex = std::string::npos;
	std::string m_BackupFile;

	void UpdateScriptDic();
//GUI
private:
	void CreateGUIControls();
	wxToolBar* m_pToolBar = nullptr;
	wxImage m_ImgPrevScript;

	void SetEditorVertical();
	void SetEditorHorizontal();	
	bool m_Vertical = true;	

	//Editor	
	STC* tScriptTranslated = nullptr;
	wxButton* editor_save_text = nullptr;
	
	wxButton* editor_prev_text = nullptr;
	wxButton* editor_next_text = nullptr;
	wxBoxSizer* editor_sizer = nullptr;
	wxBoxSizer* editor_buttons_sizer = nullptr;
	STC* tScriptOriginal = nullptr;

	STC* m_pSrcCodeOutput = nullptr;
	wxStyledTextWrapper* m_pDisassemblyEditor = nullptr;

	wxBoxSizer* global_sizer = nullptr;	
	FindResultsWindow* m_pFindResultsWindow = nullptr;	
};

class DialogTextRange : public wxDialog
{
public:
	DialogTextRange(ScriptEditorFrame* parent);
	~DialogTextRange() = default;

	ScriptEditorFrame* m_pParent = nullptr;
//Events
private:
	void OnGoButton(wxCommandEvent& event);
private:
	void CreateGUIControls();
		
	wxTextCtrl* dialogInputFrom = nullptr;
	wxTextCtrl* dialogInputTo = nullptr;
	wxTextCtrl* dialogInputScript = nullptr;
	wxButton* 	dialogGoButton = nullptr;
};