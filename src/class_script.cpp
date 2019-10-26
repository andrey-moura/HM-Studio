#include "class_script.hpp"

Script::Script(std::vector<uint8_t>& bytes)
{
	data = bytes;
	GetStrPosition();
	GetStrCount();
	GetTextOffset();
}

Script::Script()
{
	data = { 0x52, 0x49, 0x46, 0x46, 0x28, 0x00, 0x00, 0x00, 0x53, 0x43,
			 0x52, 0x20, 0x43, 0x4F, 0x44, 0x45, 0x08, 0x00, 0x00, 0x00,
			 0x04, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x53, 0x54,
			 0x52, 0x20, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	GetStrPosition();
	GetStrCount();
	GetTextOffset();
}

std::vector<std::string> Script::GetText()
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

void Script::GetStrPosition()
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

bool Script::HaveText()
{
	if (str_count > 0x00)
		return true;
	else
		return false;
}

uint32_t Script::GetStrLenght()
{
	uint32_t str_size = 0;

	memcpy(&str_size, data.data() + str_offset + 4, 4);

	return str_size;
}

void Script::SetStrLenght(uint32_t str_size)
{
	memcpy(data.data() + str_offset + 4, &str_size, 4);
}

void Script::GetStrCount()
{
	memcpy(&str_count, data.data() + str_offset + 8, 4);
}

std::vector<uint32_t> Script::GetPointers()
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

void Script::UpdateText(std::vector<std::string> text)
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

void Script::GetTextOffset()
{
	text_offset = str_offset + (str_count * 4) + 12;
}
