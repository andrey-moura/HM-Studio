#pragma once

#include <vector>

#include <wx/window.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/bmpbuttn.h>
#include <wx/stattext.h>
#include <wx/dcbuffer.h>

class wxTabControl;

wxDECLARE_EVENT(wxEVT_TAB_CLICK, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_TAB_CHANGE, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_TAB_CLOSE, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_TAB_CLOSED, wxCommandEvent);

class wxTab : public wxWindow
{
public:
	wxTab(const std::string& title, wxWindow* parent, wxWindowID id);
	~wxTab() = default;

//Members
	std::string m_Title;
	std::size_t m_Index;	
//Icons
private:
	static uint8_t* m_pIconrgb;
//Events
private:
	void DoBinds();
	void OnTabLeftClick(wxMouseEvent& event);
	void OnCloseClick(wxCommandEvent& event);		
//GUI
private:
	void CreateGUIControls();

	wxStaticText* m_TitleLabel = nullptr;
	wxBitmapButton* m_CloseButton = nullptr;
};

class wxTabControl : public wxWindow
{
public:
	wxTabControl(wxWindow* parent, wxWindowID id);
	~wxTabControl() = default;

	//Members functions
public:
	void AddTab(const std::string& title);
	void SetSelColor(const wxColour& color);
	void SetUnSelColor(const wxColour& color);
	wxColour GetSelColor();
	wxColour GetUnSelColor();
	void SelectTab(size_t index);
	void SetDefaultTitle(const std::string& title);
	void SetUserCanAddTab(bool canAdd) { m_UserCanAdd = canAdd; }
private:
//Internal members functions
	void ChangeCurTabColor();
	void ChangeToUnSelColor(size_t index);
	void ChangeAllUnSelColor();
	void SendChangeTabEvent(std::pair<size_t, size_t>* fromTo);
//Members
private:
	std::vector<wxTab*> m_Tabs;
	wxColour m_SelColor;
	wxColour m_UnSelColor;
	size_t m_CurTab = -1;
	std::string m_DefaultTitle = "Untitled";
	bool m_UserCanAdd = true;
	wxWindow* m_pParent = nullptr;
//Events
	void DoBinds();
	void OnControlDoubleClick(wxMouseEvent& event);
	void OnTabClick(wxCommandEvent& event);	
	void OnTabClose(wxCommandEvent& event);
	void CallTabClosing(size_t index);
//GUI
private:
	void CreateGUIControls();

	wxBoxSizer* m_RootSizer = nullptr;
};

