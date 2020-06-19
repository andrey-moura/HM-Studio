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

void LetterEditor::DumpAll(bool translated)
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

void LetterEditor::InsertAll()
{
	std::vector<std::string> letters_lines_all;
	std::vector<uint32_t> letters_counts_all;
	std::vector<uint32_t> lines_pointers_all;
	std::string letter_block;
	letter_block.reserve(m_BlockSize);
	letters_counts_all.reserve(m_Count);

	uint32_t end_line = std::string::npos;

	std::vector<uint32_t> letters_pointers_all;
	letters_pointers_all.reserve(m_Count);
	
	const Moon::Hacking::Table table = m_RomTranslated.GetTable();

	for (size_t letterIndex = 0; letterIndex < m_Count; ++letterIndex)
	{
		auto lines = Moon::File::ReadAllLines(GetPath(letterIndex, true), true);
		std::vector<uint32_t> letter_pointers;		
		//I don't know why, but this slows down in debug mode
#ifndef _DEBUG
		letter_pointers.reserve(lines.size());
		letters_lines_all.reserve(letters_lines_all.size() + lines.size());
#endif // !_DEBUG		
		letters_counts_all.push_back(lines.size());

		for (auto& line : lines)
		{
			bool add_line = true;
			table.Output(line);

			if (line.empty())
			{
				if (end_line == std::string::npos)
				{
					while (letter_block.size() % 4 != 0)
						letter_block.push_back('\0');

					end_line = ToBlockPointer(letter_block.size());

					letter_block.append(4, '\0');
				}

				letter_pointers.push_back(end_line);
				continue;
			}
			else
			{
				//Looks back for find a matching string				
				for (size_t compare_index = letters_lines_all.size()-1; compare_index != std::string::npos; --compare_index)
				{
					const std::string& compare_line = letters_lines_all[compare_index];

					if (compare_line == line)
					{
						add_line = false;
						lines_pointers_all.push_back(lines_pointers_all[compare_index]);
						letter_pointers.push_back(lines_pointers_all[compare_index]);
						break;
					}
				}
			}

			letters_lines_all.push_back(line);

			if (add_line)
			{
				lines_pointers_all.push_back(ToBlockPointer(letter_block.size()));
				letter_pointers.push_back(ToBlockPointer(letter_block.size()));
				letter_block.append(line);
				letter_block.push_back('\0');
			}
		}

		while (letter_block.size() % 4 != 0)
			letter_block.push_back('\0');

		letters_pointers_all.push_back(ToBlockPointer(letter_block.size()));
		letter_block.append((const char*)letter_pointers.data(), letters_counts_all[letterIndex] * 4);
		letter_block.append(4, '\0');
	}

	if (letter_block.size() > m_BlockSize)
	{
		return;
	}
	else
	{
		letter_block.resize(m_BlockSize);
	}

	m_RomTranslated.Seek(m_StartPointers);
	m_RomTranslated.WriteBytes(letters_pointers_all);

	m_RomTranslated.Seek(m_StartBlock);
	m_RomTranslated.WriteBytes(letter_block.data(), m_BlockSize);
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
		m_BlockSize = 0x107DC;
		m_StartBlock = 0x1113E4;
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