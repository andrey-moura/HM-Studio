#include "class_letter_editor.hpp"

LetterEditor::LetterEditor(const id& id) : Editor(id)
{
	SetupRom();	
}

bool LetterEditor::Open(uint32_t number)
{
	if (number > m_Count)
		return false;

	m_Original = Moon::File::ReadAllText(GetPath(number, false));
	m_Translated = Moon::File::ReadAllText(GetPath(number, true));

	m_Number = number;
	m_Index = number;

	return true;
}

std::string& LetterEditor::GetText(bool translated)
{
	return translated ? m_Translated : m_Original;
}

void LetterEditor::SetText(const std::string& text)
{
	m_Translated = text;
}

void LetterEditor::SaveFile()
{		
	Moon::File::WriteAllText(GetPath(true), m_Translated);
}

uint32_t* LetterEditor::GetPointers(bool translated)
{
	uint32_t* pointers = new uint32_t[m_Count];

	RomFile& rom = GetRom(translated);
	rom.Seek(m_StartPointers);
	rom.Read(pointers, m_Count*4);

	rom.ConvertPointers(pointers, m_Count);

	return pointers;
}

void LetterEditor::Dump(bool translated) 
{
	RomFile& rom = GetRom(translated);
	uint32_t* letterPointers = GetPointers(translated);	

	std::string eol = rom.GetEOlString();

	for (size_t letterIndex = 0; letterIndex < m_Count; ++letterIndex)
	{
		rom.Seek(letterPointers[letterIndex]);

		uint32_t pointer = rom.ReadUInt32();
		std::vector<uint32_t> textPointers;

		while (rom.IsPointer(pointer))
		{
			textPointers.push_back(pointer);
			pointer = rom.ReadUInt32();
		}

		if (textPointers.size() == 0)
			continue;

		rom.ConvertPointers(textPointers.data(), textPointers.size());

		std::string letter;

		rom.Seek(textPointers[0]);
		letter.append(rom.ReadString());

		for (size_t i = 1; i < textPointers.size(); ++i)
		{
			rom.Seek(textPointers[i]);
			letter.append(eol);
			letter.append(rom.ReadString());
		}

		const Moon::Hacking::Table& table = rom.GetTable();
		table.Input(letter);		

		Moon::File::WriteAllText(GetPath(letterIndex, translated), letter);
	}

	delete[] letterPointers;
}

void LetterEditor::SetupRom()
{	
	switch (m_RomOriginal.Id)
	{
	case id::DS:
		break;
	case id::FoMT:
		break;
	case id::MFoMT:
		m_StartPointers = 0x1110EC;
		m_Count = 190;
		break;
	default:
		break;
	}

	m_LetterDir = Moon::File::AppenPath(m_RomOriginal.m_HomeDir, "Letters");
	Moon::File::MakeDir(m_LetterDir);
	Moon::File::MakeDir(Moon::File::AppenPath(m_LetterDir, m_RomOriginal.State));
	Moon::File::MakeDir(Moon::File::AppenPath(m_LetterDir, m_RomTranslated.State));
}

std::string LetterEditor::GetPath(const uint32_t& number, const bool& translated)
{	
	const std::string& state = GetRom(translated).State;

	wxFileName fn;
	fn.SetPath(m_LetterDir);
	fn.AppendDir(state);

	fn.SetFullName("Letter_" + state + "_" + std::to_string(number) + ".letter");
	return fn.GetFullPath().ToStdString();
}