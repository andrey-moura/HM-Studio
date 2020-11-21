#include "frame_value_editor.hpp"

static int s_ValueSize[6]{ 4, 2, 1, -1, 1, 1 };
const wchar_t* s_ValueTypeName[6]{ L"Word", L"Half", L"Byte", L"String", L"Char", L"Bool" };
wxArrayString s_ValueTypeArray(6, s_ValueTypeName);
static size_t s_LastTypeSelection = 0;

uint32_t Value::GetValueSize() const
{
	if (type == ValueType::StringValue)
		return str_value.size();

	return s_ValueSize[(int)type];
}

const wxString& Value::GetValueName() const
{
	return s_ValueTypeArray[(int)type];
}

ValueType Value::GetValueFromName(const wxString& name)
{
	if (name == L"Word")
	{
		return ValueType::WordValue;
	}
	else if (name == L"Half")
	{
		return ValueType::HalfValue;
	}
	else if (name == L"Byte")
	{
		return ValueType::ByteValue;
	}
	else if (name == L"String")
	{
		return ValueType::StringValue;
	}
	else if (name == L"Char")
	{
		return ValueType::CharValue;
	}
	else if (name == L"Bool")
	{
		return ValueType::BoolValue;
	}

	return ValueType::StringValue;	
}

void Value::ValueFromString(const wxString& value)
{
	if (type == ValueType::WordValue || type == ValueType::HalfValue || type == ValueType::ByteValue)
	{
		this->value = std::stoi(value.ToStdWstring(), nullptr, showValueAsHex ? 16 : 10);
	}
	else if (type == ValueType::StringValue)
	{		
		this->str_value = value;
	}
	else if (type == ValueType::CharValue)
	{
		if (value.size() > 1)
		{
			if (value[0] == L'0' && value[1] == L'x')
			{
				wxString str = value.substr(2);

				if (str.size() == 2)
				{
					this->value = Moon::BitConverter::FromHexChar((unsigned char*)str.ToStdString().c_str());
				}
				else
				{
					//Exception
				}
			} 			
			else
			{
				//Exception
			}
		}
		else
		{
			this->value = (char)value[0];
		}		
	}
}

wxString Value::ValueToString() const
{
	if (type == ValueType::WordValue)
	{
		if (showValueAsHex)
			return L"0x" + Moon::BitConverter::ToHexString(value);
		else
			return std::to_wstring(value);
	}
	else if (type == ValueType::HalfValue)
	{
		uint16_t v = value;

		if (showValueAsHex)
			return L"0x" + Moon::BitConverter::ToHexString(v);
		else
			return std::to_wstring(v);
	}
	else if (type == ValueType::ByteValue)
	{
		uint8_t v = value;

		if (showValueAsHex)
			return L"0x" + Moon::BitConverter::ToHexString(v);
		else
			return std::to_wstring(v);
	}
	else if (type == ValueType::StringValue)
	{
		return str_value;
	}
	else if (type == ValueType::CharValue)
	{	
		return (char)value;
	}
	else if (type == ValueType::BoolValue)
	{
		return Moon::BitConverter::ToBooleanString(value);
	}

	return wxEmptyString;
}

wxString Value::OffsetToString() const
{
	if (showOffsetAsHex)
		return L"0x" + Moon::BitConverter::ToHexString(offset);

	return std::to_wstring(offset);
}

class NewValueDialog : public wxDialog
{
public:
	NewValueDialog() : wxDialog(nullptr, wxID_ANY, L"Create a new value")
	{
		SetFont(wxFontInfo(10).FaceName("Courier New"));

		wxBoxSizer* name_sizer = new wxBoxSizer(wxHORIZONTAL);
		name_sizer->Add(new wxStaticText(this, wxID_ANY, L"Name:   "));
		m_pNameInput = new wxTextCtrl(this, wxID_ANY, L"Value");
		name_sizer->Add(m_pNameInput);

		wxBoxSizer* type_sizer = new wxBoxSizer(wxHORIZONTAL);
		type_sizer->Add(new wxStaticText(this, wxID_ANY, L"Type:   "));
		m_pTypeBox = new wxComboBox(this, wxID_ANY, s_ValueTypeArray[s_LastTypeSelection], wxDefaultPosition, wxDefaultSize, s_ValueTypeArray, wxCB_READONLY);
		type_sizer->Add(m_pTypeBox);

		wxBoxSizer* offset_sizer = new wxBoxSizer(wxHORIZONTAL);
		offset_sizer->Add(new wxStaticText(this, wxID_ANY, L"Offset: "));

		m_pOffsetInput = new wxTextCtrl(this, wxID_ANY, L"00000000");
		m_pOffsetInput->SetValidator(wxTextValidator(wxTextValidatorStyle::wxFILTER_XDIGITS));
		offset_sizer->Add(m_pOffsetInput);
		offset_sizer->Add(new wxStaticText(this, wxID_ANY, L"hex"))->GetWindow()->Bind(wxEVT_LEFT_DOWN, &NewValueDialog::OnHexClick, this);

		m_pCountInput = new wxTextCtrl(this, wxID_ANY, L"1");
		m_pCountInput->SetValidator(wxTextValidator(wxTextValidatorStyle::wxFILTER_NUMERIC));

		wxBoxSizer* count_sizer = new wxBoxSizer(wxHORIZONTAL);
		count_sizer->Add(new wxStaticText(this, wxID_ANY, L"Count:  "));
		count_sizer->Add(m_pCountInput);

		wxBoxSizer* root_sizer = new wxBoxSizer(wxVERTICAL);
		root_sizer->Add(name_sizer);
		root_sizer->Add(type_sizer);
		root_sizer->Add(offset_sizer);
		root_sizer->Add(count_sizer);
		root_sizer->Add(CreateButtonSizer(wxOK | wxCANCEL));

		SetSizerAndFit(root_sizer);
	}
private:
	wxTextCtrl* m_pOffsetInput = nullptr;
	wxTextCtrl* m_pNameInput = nullptr;
	wxComboBox* m_pTypeBox = nullptr;
	wxTextCtrl* m_pCountInput = nullptr;
	bool m_OffsetHex = true;	
public:
	wxString GetName() { return m_pNameInput->GetValue(); }
	uint32_t GetOffset() { return std::stoi(m_pOffsetInput->GetValue().ToStdWstring(), nullptr, m_OffsetHex ? 16 : 10); }
	size_t GetCount() { return std::stoi(m_pCountInput->GetValue().ToStdWstring()); }
	Value GetValue()
	{
		Value v;
		v.value = 0;
		v.name = GetName();
		v.offset = GetOffset();
		v.type = Value::GetValueFromName(m_pTypeBox->GetStringSelection());
		s_LastTypeSelection = m_pTypeBox->GetSelection();		

		return v;
	}

private:
	void OnHexClick(wxMouseEvent& event)
	{
		wxStaticText* text = (wxStaticText*)event.GetEventObject();

		if (text->GetLabel() == L"hex")
		{
			text->SetLabel(L"decimal");

			uint32_t offset = std::stoi(m_pOffsetInput->GetValue().ToStdWstring(), nullptr, 16);

			m_pOffsetInput->SetValidator(wxTextValidator(wxTextValidatorStyle::wxFILTER_NUMERIC));
			m_pOffsetInput->SetValue(std::to_string(offset));
		}
		else
		{
			text->SetLabel(L"hex");

			uint32_t offset = std::stoi(m_pOffsetInput->GetValue().ToStdWstring());

			m_pOffsetInput->Clear();
			m_pOffsetInput->SetValidator(wxTextValidator(wxTextValidatorStyle::wxFILTER_XDIGITS));
			m_pOffsetInput->SetValue(Moon::BitConverter::ToHexString(offset));
		}

		m_OffsetHex = !m_OffsetHex;

		event.Skip();
	}
};

ValueEditor::ValueEditor(const id& id) : Editor(id, L"Values")
{
	wxFileName fn;
	fn.SetPath(m_EditorDir);
	fn.SetFullName("%s.xml");

	m_OriginalFormat = fn.GetFullPath();	

	wxDir::GetAllFiles(m_EditorDir, &m_Files, L"*.xml");	

	for (wxString& str : m_Files)
	{
		wxFileName fn(str);
		str = fn.GetName();
	}

	m_Info.Count = m_Files.Count();
}

void ValueEditor::SaveFile()
{
	wxXmlDocument doc;

	wxXmlNode* root_node = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, L"Values");
	doc.SetRoot(root_node);

	for (size_t i = 0; i < m_Values.size(); ++i)
	{
		wxXmlNode* node = new wxXmlNode(wxXmlNodeType::wxXML_ELEMENT_NODE, L"Value");
		node->AddAttribute(L"name", m_Values[i].name);
		node->AddAttribute(L"offset", "0x" + Moon::BitConverter::ToHexString(m_Values[i].offset));
		node->AddAttribute(L"type", m_Values[i].GetValueName());

		wxString str = m_Values[i].ValueToString();

		bool hex_char = false;

		if (m_Values[i].type == ValueType::CharValue)
		{
			if (str == L" " || !iswprint(str[0]))			
			{
				str = L"0x" + Moon::BitConverter::ToHexString<char>(str[0]);				
			}
		}
		
		node->AddChild(new wxXmlNode(wxXmlNodeType::wxXML_TEXT_NODE, L"", str));		

		root_node->AddChild(node);
	}

	doc.Save(GetPath(m_File));
}

void ValueEditor::InsertFile()
{
	for (const Value& value : m_Values)
	{
		m_RomTranslated.Seek(value.offset);

		switch (value.type)
		{
		case ValueType::WordValue:
			m_RomTranslated.WriteT(value.value);
			break;
		case ValueType::HalfValue:
			m_RomTranslated.WriteT((uint16_t)value.value);
			break;
		case ValueType::StringValue:
		{
			std::string str = value.str_value.ToStdString();
			m_RomTranslated.WriteBytes(str.c_str(), str.size()+1);
			break;
		}
		case ValueType::BoolValue: case ValueType::ByteValue: case ValueType::CharValue:
			m_RomTranslated.WriteT((uint8_t)value.value);
			break;
		}
	}

	m_RomTranslated.Flush();
}

bool ValueEditor::Open(uint32_t index)
{
	if (index >= m_Info.Count)
		return false;

	return Open(m_Files[index]);
}

bool ValueEditor::Open(const wxString& name)
{
	wxXmlDocument doc(GetPath(name));	
	wxXmlNode* root_node = doc.GetRoot();

	if (!doc.IsOk())
		return false;

	if (!root_node)
		return false;

	wxXmlNode* node = root_node->GetChildren();

	if (!node)
		return false;

	m_Values.clear();
	m_Number = m_Files.Index(name);

	while (node)
	{		
		Value value;
		value.name = node->GetAttribute(L"name", L"Value");
		value.type = Value::GetValueFromName(node->GetAttribute(L"type", "Word"));
		value.offset = std::stoi(node->GetAttribute(L"offset").ToStdWstring(), nullptr, 16);
		value.type = Value::GetValueFromName(node->GetAttribute(L"type", "Word"));
		value.ValueFromString(node->GetChildren()->GetContent());		
		m_Values.push_back(value);
		node = node->GetNext();
	}

	m_File = name;
	m_Count = m_Values.size();

	return true;
}

wxString ValueEditor::GetPath(const wxString& name)
{
	return wxString::Format(m_OriginalFormat, name);
}

void ValueEditor::AddValue()
{
	uint32_t offset = 0;

	if (m_Count > 0)
	{
		const Value& back = m_Values.back();
		offset = back.offset + back.GetValueSize();

		if (back.type == ValueType::StringValue)
		{
			++offset;
		}
	}

	m_Values.push_back(Value(L"Value", ValueType::WordValue, offset));	
	std::sort(m_Values.begin(), m_Values.end());
	++m_Count;
}

void ValueEditor::AddValue(const Value& value, size_t count)
{
	uint32_t cur_offset = value.offset;

	for (size_t i = 0; i < count; ++i)
	{
		m_Values.push_back(value);
		Value& v = m_Values.back();		
		v.offset = cur_offset;

		m_RomTranslated.Seek(v.offset);

		switch (v.type)
		{
		case ValueType::WordValue:
			v.value = m_RomTranslated.ReadUInt32();
			break;
		case ValueType::HalfValue:
			v.value = m_RomTranslated.ReadUInt16();
			break;
		case ValueType::ByteValue: case ValueType::CharValue: case ValueType::BoolValue:
			v.value = m_RomTranslated.ReadUInt8();
			break;
		case ValueType::StringValue:
		{						
			v.str_value = m_RomTranslated.ReadString();
			break;
		}
		default:
			break;
		}

		++m_Count;
		cur_offset += v.GetValueSize();
	}	

	std::sort(m_Values.begin(), m_Values.end());
}

void ValueEditor::Delete(size_t index)
{
	m_Values.erase(m_Values.begin() + index);
	m_Count--;
}

void ValueEditor::AddFile(const wxString& filename)
{
	m_Files.push_back(filename);
	m_Info.Count++;

	m_Number = m_Info.Count - 1;
	m_File = filename;
	UpdateText();
}

ValueEditorFrame::ValueEditorFrame(const id& id) : EditorFrame(new ValueEditor(id))
{
	m_pValueEditor = (ValueEditor*)m_pEditor;

	CreateGUIControls();
}

void ValueEditorFrame::UpdateValue(size_t index)
{
	const Value& value = m_pValueEditor->GetValue(index);
	
	m_pValueList2->SetCellValue(index, COL_NAME, value.name);
	m_pValueList2->SetCellValue(index, COL_TYPE, value.GetValueName());
	m_pValueList2->SetCellValue(index, COL_ADRESS, value.OffsetToString());
	m_pValueList2->SetCellValue(index, COL_SIZE, std::to_wstring(value.GetValueSize()));
	m_pValueList2->SetCellValue(index, COL_VALUE, value.ValueToString());

	m_pValueList2->SetCellEditor(index, COL_TYPE, new wxGridCellChoiceEditor(s_ValueTypeArray));
	m_pValueList2->SetCellEditor(index, COL_ADRESS, value.showOffsetAsHex ? new wxGridCellTextEditor() : new wxGridCellNumberEditor());
	m_pValueList2->SetCellEditor(index, COL_VALUE, value.showValueAsHex ? new wxGridCellTextEditor() : new wxGridCellNumberEditor());

	m_pValueList2->SetCellAlignment(index, COL_TYPE, wxALIGN_CENTER, wxALIGN_CENTER);

	if (m_FirstAutoSize)
	{
		m_pValueList2->AutoSizeColumn(COL_ADRESS);
		m_pValueList2->SetColSize(COL_VALUE, m_pValueList2->GetColSize(COL_ADRESS));

		m_FirstAutoSize = false;
	}
}

void ValueEditorFrame::UpdateFile()
{
	SetTitle(m_pValueEditor->GetName());
	
	//I don't know how to get the grid count... ;-;
	//Also how to create a new grid and replace the old one ;-;

	int diference = m_pEditor->GetCount() - m_OldCount;

	if (diference < 0)
	{
		m_pValueList2->DeleteRows(0, diference * -1);
	}
	else if (diference > 0)
	{
		m_pValueList2->InsertRows(0, diference);
	}

	m_OldCount = m_pEditor->GetCount();

	for (size_t i = 0; i < m_pEditor->GetCount(); ++i)
		UpdateValue(i);
}

void ValueEditorFrame::OnNewFileClick(wxCommandEvent& event)
{
	wxTextEntryDialog dialog(nullptr, L"Select a file name", L"File name");

	if (dialog.ShowModal() != wxID_CANCEL)
	{
		m_pValueEditor->AddFile(dialog.GetValue());
		m_pValueEditor->AddValue();

		UpdateFile();
	}

	event.Skip();
}

void ValueEditorFrame::OnNewValueClick(wxCommandEvent& event)
{
	event.Skip();

	if (!m_pEditor->IsOpened())
	{
		wxTextEntryDialog dialog(nullptr, L"Select a file name", L"File name");

		if (dialog.ShowModal() == wxID_CANCEL)
			return;

		m_pValueEditor->AddFile(dialog.GetValue());
	}
	
	m_pValueEditor->AddValue();

	UpdateFile();

	event.Skip();
}

void ValueEditorFrame::OnNewValue2Click(wxCommandEvent& event)
{
	event.Skip();

	if (!m_pEditor->IsOpened())
	{
		wxTextEntryDialog dialog(nullptr, L"Select a file name", L"File name");

		if (dialog.ShowModal() == wxID_CANCEL)
			return;

		m_pValueEditor->AddFile(dialog.GetValue());
	}
	
	NewValueDialog dialog;
	if(dialog.ShowModal() == wxID_OK)
	{
		m_pValueEditor->AddValue(dialog.GetValue(), dialog.GetCount());
	}

	UpdateFile();
}

void ValueEditorFrame::OnDeleteValue(wxCommandEvent& event)
{
	m_pValueEditor->Delete(m_pValueList2->GetGridCursorRow());
	UpdateFile();

	event.Skip();
}

void ValueEditorFrame::OnCellSelected(wxGridEvent& event)
{
	m_pEditor->SetIndex(event.GetRow());
	m_ColIndex = event.GetCol();

	UpdateIndex();

	event.Skip();
}

void ValueEditorFrame::OnCellChangeStart(wxGridEditorCreatedEvent& event)
{
	const Value& value = m_pValueEditor->GetValue(m_pEditor->GetIndex());

	if (event.GetCol() == COL_ADRESS)
	{
		if (value.showOffsetAsHex)
		{
			m_pValueList2->SetCellValue(event.GetRow(), COL_ADRESS, Moon::BitConverter::ToHexString(value.offset));
		}
	}
	else if (event.GetCol() == COL_VALUE)
	{
		if (value.type != ValueType::CharValue && value.type != ValueType::StringValue)
		{
			if (value.showValueAsHex)
			{
				m_pValueList2->SetCellValue(event.GetRow(), COL_VALUE, Moon::BitConverter::ToHexString(value.value));
			}
		}
	}

	event.Skip();
}

void ValueEditorFrame::OnCellChanging(wxGridEvent& event)
{
	event.Skip();

	Value& value = m_pValueEditor->GetValue(event.GetRow());
	
	if (event.GetCol() == COL_NAME)
	{
		value.name = event.GetString();
		UpdateValue(event.GetRow());
	}
	else if (event.GetCol() == COL_TYPE)
	{
		ValueType type = Value::GetValueFromName(event.GetString());

		if (value.type == ValueType::StringValue)
		{
			value.str_value.clear();
		}

		if (type == ValueType::CharValue)
		{
			if (!isprint(value.value))
			{
				value.value = ' ';
			}
		}
		else if (type == ValueType::StringValue)
		{			
			value.str_value = " ";
		}

		value.type = type;

		UpdateValue(event.GetRow());
	}
	else if (m_ColIndex == COL_ADRESS)
	{
		if (!value.showOffsetAsHex)
			return;

		wxString str = event.GetString();

		if (str.empty())
		{
			event.Veto();
			return;
		}

		if (!Moon::BitConverter::IsHexString(str.ToStdString()))
			event.Veto();
	}
	else if (m_ColIndex == COL_VALUE)
	{
		if (value.type == ValueType::CharValue)
		{
			wxString string = event.GetString();

			if (string.size() > 1)
			{
				event.Veto();
				return;
			}			

			char c = (char)string[0];

			if (!isprint(c))
			{
				event.Veto();
				return;
			}

			value.value = c;

			return;
		}
		else if (value.type == ValueType::StringValue)
		{
			value.str_value = event.GetString();
			UpdateValue(event.GetRow());
		}
	}	
}

void ValueEditorFrame::OnCellChanged(wxGridEvent& event)
{	
	event.Skip();

	Value& value = m_pValueEditor->GetValue(event.GetRow());	

	if (value.type == ValueType::CharValue)
		return;

	wxString string = m_pValueList2->GetCellValue(event.GetRow(), event.GetCol());

	if (event.GetCol() == COL_ADRESS)
	{
		value.offset = std::stoi(string.ToStdWstring(), nullptr, value.showOffsetAsHex ? 16 : 10);
	}
	else if (event.GetCol() == COL_VALUE)
	{
		if (value.type == ValueType::StringValue)
			return;

		value.ValueFromString(string.Upper());
	}

	UpdateValue(event.GetRow());	
}

void ValueEditorFrame::OnCellRightClick(wxGridEvent& event)
{
	event.Skip();

	m_pValueList2->SetGridCursor(event.GetRow(), event.GetCol());

	bool show = false;

	const Value& value = m_pValueEditor->GetValue(event.GetRow());	

	if (m_ColIndex == COL_ADRESS)
	{
		show = value.showOffsetAsHex;
	}
	else if (m_ColIndex == COL_VALUE)
	{
		if (value.type == ValueType::CharValue)
		{
			return;
		}

		show = value.showValueAsHex;
	}
	else
	{
		return;
	}	

	m_pToggleHexMenu->FindItemByPosition(0)->Check(show);
	m_pValueList2->PopupMenu(m_pToggleHexMenu, event.GetPosition());
}

void ValueEditorFrame::OnToggleHexMenuClick(wxCommandEvent& event)
{
	event.Skip();

	Value& value = m_pValueEditor->GetValue(m_pValueEditor->GetIndex());
	bool show = m_pToggleHexMenu->FindItemByPosition(0)->IsChecked();
	
	if (m_ColIndex == COL_ADRESS)
	{
		value.showOffsetAsHex = show;
	}
	else if (m_ColIndex == COL_VALUE)
	{
		value.showValueAsHex = show;
	}
	else
	{
		wxLogError(L"Invalid col index");
		return;
	}

	UpdateValue(m_pEditor->GetIndex());
}

void ValueEditorFrame::CreateGUIControls()
{
	m_pMenuFile = new wxMenu();
	Bind(wxEVT_MENU, &ValueEditorFrame::OnNewFileClick, this, m_pMenuFile->Append(wxID_NEW, L"New")->GetId());
	m_pMenuFile->Append(wxID_SAVE, L"Save");

	wxMenu* menuValue = new wxMenu();
	Bind(wxEVT_MENU, &ValueEditorFrame::OnNewValueClick, this, menuValue->Append(wxID_ANY, L"Add Value")->GetId());
	Bind(wxEVT_MENU, &ValueEditorFrame::OnNewValue2Click, this, menuValue->Append(wxID_ANY, L"Add Value...")->GetId());
	Bind(wxEVT_MENU, &ValueEditorFrame::OnDeleteValue, this, menuValue->Append(wxID_ANY, L"Delete Current")->GetId());

	CreateMyMenuBar();
	m_frameMenuBar->Append(m_pMenuFile, L"File");
	m_frameMenuBar->Append(menuValue, L"Value");
	CreateViewMenu();

	CreateMyToolBar();

	m_pValueList2 = new wxGrid(this, wxID_ANY);

	m_pValueList2->CreateGrid(0, 5);
	m_pValueList2->HideRowLabels();
	m_pValueList2->SetColLabelValue(COL_NAME, L"Name");
	m_pValueList2->SetColLabelValue(COL_TYPE, L"Type");
	m_pValueList2->SetColLabelValue(COL_ADRESS, L"Adress");
	m_pValueList2->SetColLabelValue(COL_SIZE, L"Size");
	m_pValueList2->SetColLabelValue(COL_VALUE, L"Value");
	m_pValueList2->UseNativeColHeader();
	m_pValueList2->SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

	m_pValueList2->Bind(wxEVT_GRID_SELECT_CELL, &ValueEditorFrame::OnCellSelected, this);
	m_pValueList2->Bind(wxEVT_GRID_EDITOR_CREATED, &ValueEditorFrame::OnCellChangeStart, this);
	m_pValueList2->Bind(wxEVT_GRID_CELL_CHANGING, &ValueEditorFrame::OnCellChanging, this);
	m_pValueList2->Bind(wxEVT_GRID_CELL_CHANGED, &ValueEditorFrame::OnCellChanged, this);
	m_pValueList2->Bind(wxEVT_GRID_CELL_RIGHT_CLICK, &ValueEditorFrame::OnCellRightClick, this);

	m_pToggleHexMenu = new wxMenu();
	m_pToggleHexMenu->Bind(wxEVT_MENU, &ValueEditorFrame::OnToggleHexMenuClick, this, m_pToggleHexMenu->AppendCheckItem(wxID_ANY, L"Show as hex")->GetId());

	wxBoxSizer* root_sizer = new wxBoxSizer(wxVERTICAL);
	root_sizer->Add(m_pValueList2, 1, wxEXPAND, 0);

	CreateMyStatusBar();

	if (m_RestoredSize)
	{
		SetSizer(root_sizer);
		SetMinSize(root_sizer->GetMinSize());
	}
	else
	{
		SetSizerAndFit(root_sizer);
		root_sizer->SetSizeHints(this);
	}
}