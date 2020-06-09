#include "class_letter_editor.hpp"

LetterEditor::LetterEditor(const id& id) : m_RomOriginal(id, false), m_RomTranslated(id, true)
{
	SetupRom();
}

uint32_t* LetterEditor::GetPointers(bool translated)
{
	uint32_t* pointers = new uint32_t[m_Info.m_Count];

	RomFile& rom = GetRom(translated);
	rom.Seek(m_Info.m_StartPointers);
	rom.Read(pointers, m_Info.m_Count);

	m_RomOriginal.ConvertPointers(pointers, m_Info.m_Count);

	return pointers;
}

void LetterEditor::Dump(bool translated)
{
	RomFile& rom = GetRom(translated);
	uint32_t* pointers = GetPointers(translated);	

	std::string eol = rom.GetEOlString();

	for (size_t i = 0; i < m_Info.m_Count; ++i)
	{
		rom.Seek(pointers[i]);

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

		Moon::File::WriteAllText(GetPath(i, translated), letter);
	}

	delete[] pointers;
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
		m_Info.m_StartPointers = 0x1110EC;
		m_Info.m_Count = 190;
		break;
	default:
		break;
	}

	m_LetterDir = Moon::File::AppenPath(m_RomOriginal.m_HomeDir, "Letters");
	Moon::File::MakeDir(m_LetterDir);
	Moon::File::MakeDir(Moon::File::AppenPath(m_LetterDir, m_RomOriginal.State));
	Moon::File::MakeDir(Moon::File::AppenPath(m_LetterDir, m_RomTranslated.State));
}

std::string LetterEditor::GetPath(uint32_t number, bool translated)
{	
	const std::string& state = GetRom(translated).State;

	wxFileName fn;
	fn.SetPath(m_LetterDir);
	fn.AppendDir(state);

	fn.SetFullName("Letter_" + state + "_" + std::to_string(number) + ".letter");
	return fn.GetFullPath().ToStdString();
}

RomFile& LetterEditor::GetRom(bool translated)
{
	return translated ? m_RomTranslated : m_RomOriginal;
}