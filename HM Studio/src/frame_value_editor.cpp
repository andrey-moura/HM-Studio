#include "frame_value_editor.hpp"

static uint32_t s_ValueSize[4]{ 4, 2, 1, 1 };
const wchar_t* s_ValueTypeName[4]{ L"Word", L"Half", L"Byte", L"Bool" };
wxArrayString s_ValueTypeArray(4, s_ValueTypeName);

uint32_t Value::GetValueSize() const
{
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
	else if (name == L"Bool")
	{
		return ValueType::BoolValue;
	}
}

void Value::ValueFromString(const wxString& value)
{
	if (type == ValueType::WordValue || type == ValueType::HalfValue || type == ValueType::ByteValue)
	{
		this->value = std::stoi(value.ToStdWstring(), nullptr, showValueAsHex ? 16 : 10);
	}
}

wxString Value::ValueToString() const
{
	if (type == ValueType::WordValue)
	{
		if (showValueAsHex)
			return L"0x" + Moon::BitConverter::TToHexString(value);
		else
			return std::to_wstring(value);
	}
	else if (type == ValueType::HalfValue)
	{
		uint16_t v = value;

		if (showValueAsHex)
			return L"0x" + Moon::BitConverter::TToHexString(v);
		else
			return std::to_wstring(v);
	}
	else if (type == ValueType::ByteValue)
	{
		uint8_t v = value;

		if (showValueAsHex)
			return L"0x" + Moon::BitConverter::TToHexString(v);
		else
			return std::to_wstring(v);
	}
	else if (type == ValueType::BoolValue)
	{
		return Moon::BitConverter::ToBooleanString(value);
	}
}

wxString Value::OffsetToString() const
{
	if (showOffsetAsHex)
		return L"0x" + Moon::BitConverter::TToHexString(offset);

	return std::to_wstring(offset);
}

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
		node->AddAttribute(L"offset", "0x" + Moon::BitConverter::TToHexString(m_Values[i].offset));
		node->AddAttribute(L"type", m_Values[i].GetValueName());
		node->AddChild(new wxXmlNode(wxXmlNodeType::wxXML_TEXT_NODE, L"", m_Values[i].ValueToString()));

		root_node->AddChild(node);
	}

	doc.Save(GetPath(m_File));
}

void ValueEditor::InsertFile()
{
	//What is slower, doing several seeks on the disk or loading the ROM in
	//RAM and then writing everything back?

	//If more than 10 values, read the ROM, otherwise, seek. I guess this is valid

	if (m_Values.size() < 10)
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
			case ValueType::BoolValue: case ValueType::ByteValue:
				m_RomTranslated.WriteT((uint8_t)value.value);
				break;
			}
		}

		m_RomTranslated.Flush();
	}
	else
	{
		std::vector<uint8_t> rom = m_RomTranslated.ReadBytes<uint8_t>(0, m_RomTranslated.Length());

		for (const Value& value : m_Values)
		{
			switch (value.type)
			{
			case ValueType::WordValue:
				memcpy(rom.data(), &value.value, 4);
				break;
			case ValueType::HalfValue:
			{
				uint16_t v = value.value;
				memcpy(rom.data(), &v, 2);
				break;
			}
			case ValueType::BoolValue: case ValueType::ByteValue:
			{
				uint8_t v = value.value;
				memcpy(rom.data(), &v, 1);
				break;
			}
			}
		}

		Moon::File::WriteAllBytes(m_RomTranslated.Path, rom);
	}
}

bool ValueEditor::Open(size_t index)
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
	
	m_pValueList2->SetCellValue(index, 0, value.name);
	m_pValueList2->SetCellValue(index, 1, value.GetValueName());
	m_pValueList2->SetCellValue(index, 2, value.OffsetToString());
	m_pValueList2->SetCellValue(index, 3, value.ValueToString());

	m_pValueList2->SetCellEditor(index, 1, new wxGridCellChoiceEditor(s_ValueTypeArray));
	m_pValueList2->SetCellEditor(index, 2, value.showOffsetAsHex ? new wxGridCellTextEditor() : new wxGridCellNumberEditor());
	m_pValueList2->SetCellEditor(index, 3, value.showValueAsHex ? new wxGridCellTextEditor() : new wxGridCellNumberEditor());

	m_pValueList2->SetCellAlignment(index, 1, wxALIGN_CENTER, wxALIGN_CENTER);

	if (m_FirstAutoSize)
	{
		m_pValueList2->AutoSizeColumn(2);
		m_pValueList2->AutoSizeColumn(3);

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
	
	m_pValueEditor->AddValue(Value(L"Value"));

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

	if (event.GetCol() == 2)
	{
		if (value.showOffsetAsHex)
		{
			m_pValueList2->SetCellValue(event.GetRow(), 2, Moon::BitConverter::TToHexString(value.offset));
		}
	}
	else if (event.GetCol() == 3)
	{
		if (value.showValueAsHex)
		{
			m_pValueList2->SetCellValue(event.GetRow(), 3, Moon::BitConverter::TToHexString(value.value));
		}
	}

	event.Skip();
}

void ValueEditorFrame::OnCellChanging(wxGridEvent& event)
{
	event.Skip();

	Value& value = m_pValueEditor->GetValue(event.GetRow());
	
	if (event.GetCol() == 0)
	{
		value.name = event.GetString();
		UpdateValue(event.GetRow());
	}
	else if (event.GetCol() == 1)
	{
		value.type = Value::GetValueFromName(event.GetString());
		UpdateValue(event.GetRow());
	}
	else if (m_ColIndex == 2)
	{
		if (!value.showOffsetAsHex)
			return;

		if (!Moon::BitConverter::IsHexString(event.GetString().Upper().ToStdString()))
			event.Veto();
	}
	else if (m_ColIndex == 3)
	{
		if (!value.showValueAsHex)
			return;

		if (!Moon::BitConverter::IsHexString(event.GetString().Upper().ToStdString()))
			event.Veto();
	}	
}

void ValueEditorFrame::OnCellChanged(wxGridEvent& event)
{
	Value& value = m_pValueEditor->GetValue(event.GetRow());
	wxString string = m_pValueList2->GetCellValue(event.GetRow(), event.GetCol());

	if (event.GetCol() == 2)
	{
		value.offset = std::stoi(string.ToStdWstring(), nullptr, value.showOffsetAsHex ? 16 : 10);
	}
	else if (event.GetCol() == 3)
	{
		value.ValueFromString(string.Upper());
	}

	UpdateValue(event.GetRow());

	event.Skip();
}

void ValueEditorFrame::OnCellRightClick(wxGridEvent& event)
{
	event.Skip();

	m_pValueList2->SetGridCursor(event.GetRow(), event.GetCol());

	bool show = false;

	const Value& value = m_pValueEditor->GetValue(event.GetRow());

	if (m_ColIndex == 2)
	{
		show = value.showOffsetAsHex;
	}
	else if (m_ColIndex == 3)
	{
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
	
	if (m_ColIndex == 2)
	{
		value.showOffsetAsHex = show;
	}
	else if (m_ColIndex == 3)
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

	CreateMyMenuBar();
	m_pMenuBar->Append(m_pMenuFile, L"File");
	m_pMenuBar->Append(menuValue, L"Value");

	CreateMyToolBar();

	m_pValueList2 = new wxGrid(this, wxID_ANY);

	m_pValueList2->CreateGrid(0, 4);
	m_pValueList2->HideRowLabels();
	m_pValueList2->SetColLabelValue(0, L"Name");
	m_pValueList2->SetColLabelValue(1, L"Type");
	m_pValueList2->SetColLabelValue(2, L"Adress");
	m_pValueList2->SetColLabelValue(3, L"Value");
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

	SetSizer(root_sizer);
}