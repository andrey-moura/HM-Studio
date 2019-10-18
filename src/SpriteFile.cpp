#include "SpriteFile.hpp"

SpriteFile::SpriteFile(std::vector<uint8_t> bytes)
{
	data = bytes;
	GetStrPosition();
	GetStrCount();
	GetTextOffset();
}

std::vector<uint8_t> SpriteFile::GetEmptyScript(int l_count)
{	 
	uint8_t script_template[56] = { 0x52, 0x49, 0x46, 0x46, 0x4F, 0x00, 0x00, 0x00, 0x53, 0x43,
									0x52, 0x20, 0x43, 0x4F, 0x44, 0x45, 0x18, 0x00, 0x00, 0x00,
									0x14, 0x00, 0x00, 0x00, 0x21, 0x1F, 0x00, 0x00, 0x00, 0x23,
									0x00, 0x21, 0x22, 0x00, 0x00, 0x00, 0x21, 0x21, 0x00, 0x00,
					 				0x00, 0x20, 0x00, 0x00, 0x53, 0x54, 0x52, 0x20, 0x1B, 0x00,
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

	std::string default_text = "This script does not exist.\x5";
	default_text.append(1, (char)0x00);

	int text_size = default_text.size();

	int riff_size = text_size + 56 + (l_count * 4);

	int str_size = (l_count * 4) + 4 + text_size;

	memcpy(script_template + 4, &riff_size, 4);
	memcpy(script_template + 0x30, &str_size, 4);
	memcpy(script_template + 0x34, &l_count, 4);	

	std::vector<uint8_t> bytes;
	bytes.resize(riff_size);
	memcpy(bytes.data(), script_template, 56);
	memcpy(bytes.data() + 0x38 + (l_count * 4), default_text.data(), text_size);

	return bytes;
}

std::vector<std::string> SpriteFile::GetText()
{
	std::vector<std::string> text;	

	std::vector<uint32_t> pointers = GetPointers();

	int this_size = 0;

	for (int i = 0; i < str_count; ++i)
	{
		this_size = 0;
		pointers[i] += text_offset;

		while (data[pointers[i] + this_size] != 0x00)
		{
			++this_size;
		}
		
		text.push_back(std::string());
		text[i].resize(this_size);
		memcpy((void*)text[i].data(), data.data() + pointers[i], this_size);
	}

	return text;
}

void SpriteFile::GetStrPosition()
{
	int index = 0;
	for (int i = 0; i < data.size(); ++i)
	{
		if (index == 0)
		{
			if (data[i] == 0x53)
			{
				++index;
			}
			else { i += 3; }
		}
		else if (index == 1)
		{
			if (data[i] == 0x54)
			{
				++index;
			}
			else { index = 0; i += 2; }
		}
		else if (index == 2)
		{
			if (data[i] == 0x52)
			{
				++index;
			}
			else { index = 0; i += 1; }
		}
		else if (index == 3)
		{
			if (data[i] == 0x20)
			{
				index = i - 3;
				break;
			}
			else { index = 0; }
		}
	}

	if (index > 0) str_offset = index;
	else str_offset = -1;
}

bool SpriteFile::HaveText()
{
	if (str_count > 0x00)
		return true;
	else
		return false;
}

uint32_t SpriteFile::GetStrLenght()
{
	uint32_t str_size = 0;

	memcpy(&str_size, data.data() + str_offset + 4, 4);

	return str_size;
}

void SpriteFile::SetStrLenght(uint32_t str_size)
{
	memcpy(data.data() + str_offset + 4, &str_size, 4);
}

void SpriteFile::GetStrCount()
{
	memcpy(&str_count, data.data() + str_offset + 8, 4);
}

std::vector<uint32_t> SpriteFile::GetPointers()
{
	std::vector<uint32_t> output;

	for (int i = 0; i < str_count * 4; i+=4)
	{
		int pointer = 0;
		memcpy(&pointer, data.data() + str_offset + 12 + i, 4);
		output.push_back(pointer);
	}

	return output;
}

void SpriteFile::UpdateText(std::vector<std::string> text)
{
	int text_size = 0;
	std::string buffer;

	for (int i = 0; i < str_count; ++i)
	{
		memcpy(data.data() + str_offset + (i * 4) + 12, &text_size, 4);		
		text_size += text[i].length() + 1;
		
		buffer.append(text[i]);
		buffer.append(1, (char)0x00);
	}

	data.resize(str_offset + (str_count * 4) + 12 + text_size);
	memcpy(data.data() + text_offset, buffer.data(), text_size);

	int str_size = (str_count * 4) + 4 + text_size;
	memcpy(data.data() + str_offset + 4, &str_size, 4);

	int riff_size = data.size();
	memcpy(data.data() + 4, &riff_size, 4);
}

void SpriteFile::GetTextOffset()
{
	text_offset = str_offset + (str_count * 4) + 12;
}
