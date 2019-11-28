#pragma once
#include "wx/wx.h"
#include "wx/dataview.h"

#include "wxUtil/wxRefereceCheckBox.hpp"

enum class OptionType {

	OPTION_BOOL = 0

};

class OptionWindow : public wxFrame
{
public:
	OptionWindow();
	~OptionWindow();

public:
	uint8_t AddCategory(const std::string& title);
	uint8_t AddSubCategory();

	uint8_t AddOption(bool& option, std::string optionName);

public:
	uint8_t m_index;

public:
	void CreateGUIControls();

	wxDataViewTreeCtrl* dataView = nullptr;

	wxSizer* m_leftSizer = nullptr;
};

