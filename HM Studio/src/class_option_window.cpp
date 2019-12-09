#include "class_option_window.hpp"

OptionWindow::OptionWindow() : wxFrame(nullptr, wxID_ANY, "Options")
{
	CreateGUIControls();
}

OptionWindow::~OptionWindow()
{
}

uint8_t OptionWindow::AddCategory(const std::string& title)
{
	//dataView->text

	return ++m_index;
}

void OptionWindow::CreateGUIControls()
{
	dataView = new wxDataViewTreeCtrl(this, wxID_ANY);

	m_leftSizer = new wxBoxSizer(wxVERTICAL);
	m_leftSizer->Add(dataView, 1, wxEXPAND | wxALL, 5);

	SetSizer(m_leftSizer);
}