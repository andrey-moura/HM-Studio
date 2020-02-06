#include "class_script_editor.hpp"

ScriptEditor::ScriptEditor(Rom& original, Rom& translated) : m_RomOriginal(original), m_RomTranslated(translated)
{
	m_Info = GetRomInformation(original);
}

ScriptFlags ScriptEditor::OpenScript(size_t scriptNum)
{
	std::vector<uint8_t> dataOri = File::ReadAllBytes(FormatPath(scriptNum, PathFormat(m_RomOriginal)));

	Script script(dataOri);

	if (!script.HaveText())	
		return ScriptFlags::ERROR_NOTEXT;
		
	std::vector<uint8_t> data = File::ReadAllBytes(FormatPath(scriptNum, PathFormat(m_RomTranslated)));

	m_ScriptOriginal.SetData(dataOri);
	m_ScriptTranslated.SetData(data);

	m_Original = m_ScriptOriginal.GetText();
	m_Translated = m_ScriptTranslated.GetText();

	m_RomTranslated.InputTextWithVariables(m_Translated);
	m_RomOriginal.InputTextWithVariables(m_Original);

	m_Index = 0;
	m_Number = scriptNum;
	m_Size = m_Original.size();

	m_Opened = true;	

	return ScriptFlags::SUCCESS;
}

std::string ScriptEditor::GetCurOriginal()
{
	return m_Original[m_Index];
}

std::string ScriptEditor::GetCurTranslated()
{
	return m_Translated[m_Index];
}

bool ScriptEditor::ProxText()
{
	if (m_Index < m_Size - 1)
	{
		if (!m_Saved && m_Changed)
			BackupText(m_Translated[m_Index]);
		++m_Index;

		return true;
	}

	return false;
}

bool ScriptEditor::PrevText()
{
	if (m_Index > 0)
	{
		if (!m_Saved && m_Changed)
			BackupText(m_Translated[m_Index]);
		--m_Index;

		return true;
	}

	return false;
}

bool ScriptEditor::SaveText(const std::string& text)
{
	m_Saved = true;

	ReleaseBackup();

	m_Translated[m_Index] = text;
	return ProxText();
}

void ScriptEditor::BackupText(const std::string& text)
{
	m_TextBackup = text;
	m_IndexBackup = m_Index;
}

void ScriptEditor::ReleaseBackup()
{
	m_IndexBackup = 1;
	m_TextBackup.clear();
}

void ScriptEditor::UpdateScript()
{
	std::vector<std::string> text = m_Translated;
	m_RomTranslated.OutputTextWithVariables(text);
	m_ScriptTranslated.UpdateText(text);
}

void ScriptEditor::SetText(const std::vector<std::string>& text)
{
	m_Translated = text;
	m_RomTranslated.InputTextWithVariables(m_Translated);
}

void ScriptEditor::SaveScript()
{
	std::vector<std::string> buffer = m_Translated;

	m_RomTranslated.OutputTextWithVariables(buffer);

	m_ScriptTranslated.UpdateText(buffer);

	File::WriteAllBytes(GetPath(true), m_ScriptTranslated.GetData(), m_ScriptTranslated.GetRiffLenght());
}

void ScriptEditor::ReplaceInAllScripts(const std::string& find, const std::string& replace)
{
	wxFileName outPut(m_RomOriginal.Path);
	outPut.RemoveLastDir();
	outPut.AppendDir("Script");
	outPut.AppendDir("Backup");

	if (!outPut.DirExists())
		outPut.Mkdir(511, wxPATH_MKDIR_FULL);

	for (size_t i = 0; i < m_Info.ScriptCount; ++i)
	{
		std::string path = GetPath(true);	
		outPut.SetFullName(FormatPath(i, NameFormat(m_RomTranslated)));

		Script script(File::ReadAllBytes(path));

		if (!script.HaveText())
			continue;

		std::string_view view = (const char*)script.GetStartText();

		if (view.find(find) == std::string::npos)
			continue;

		wxCopyFile(path, outPut.GetFullPath());

		std::vector<std::string> text = script.GetText();

		m_RomTranslated.InputTextWithVariables(text);

		for (std::string& s : text)
		{
			StringUtil::Replace(find, replace, s);
		}

		m_RomTranslated.OutputTextWithVariables(text);
		script.UpdateText(text);

		File::WriteAllBytes(path, script.GetData(), script.GetRiffLenght());
	}
}

std::string ScriptEditor::NameFormat(const Rom& rom)
{
	std::string format = "Script_%s_%s.%s";

	std::string scriptExt = "";

	switch (rom.Id)
	{
	case id::FoMT:
		scriptExt = "fsf";
		break;
	case id::MFoMT:
		scriptExt = "msf";
		break;
	case id::DS:
		scriptExt = "dsf";
		break;
	default:
		break;
	}

	std::string name(25, '\0');

	sprintf((char*)name.c_str(), format.c_str(), rom.State.c_str(), "%i", scriptExt.c_str());

	return name;
}

std::string ScriptEditor::PathFormat(const Rom& rom)
{	
	wxFileName fileName;
	fileName.SetPath(rom.m_Dir);
	fileName.AppendDir("Script");
	fileName.AppendDir(rom.State);

	std::string name = NameFormat(rom);
	fileName.SetFullName(name);

	return fileName.GetFullPath().ToStdString();
}

std::string ScriptEditor::FormatPath(size_t number, const std::string& format)
{
	std::string buffer(format.size() + 2, '\0');
	sprintf((char*)buffer.c_str(), format.c_str(), number);

	return buffer;
}

std::string ScriptEditor::GetPath(size_t number, bool translated)
{
	std::string path = PathFormat(translated ? m_RomTranslated : m_RomOriginal);
	return FormatPath(number, path);
}

std::string ScriptEditor::GetPath(bool translated)
{
	return GetPath(m_Number, translated);
}

Rom& ScriptEditor::GetRom(bool translated)
{
	return (translated ? m_RomTranslated : m_RomOriginal);
}

RomInfo ScriptEditor::GetRomInformation(const Rom& rom)
{
	RomInfo information;

	switch (rom.Id)
	{
	case id::FoMT:

		information.StartPointers = 0xF89D8;
		information.ScriptCount = 1328;
		information.StartScript = 0x2AC0B0;
		information.BlockLenght = 0x24D01C;
		break;
	case id::MFoMT:
		information.StartPointers = 0x1014C0;
		information.ScriptCount = 1415;
		information.StartScript = 0x701694;
		information.BlockLenght = 0x2A9980;
		break;
	case id::DS:
		information.StartPointers = 0x24C2E04;
		information.ScriptCount = 1296 - 1;//I don't know what happened, but the ROM is missing the last script...
		information.StartScript = 0x24C4244;
		information.BlockLenght = 0x2E9C10;
		break;
	default:
		break;
	}

	return information;
}

uint32_t ScriptEditor::GetOffset(Rom& rom, size_t number)
{			
	uint32_t pointer = rom.ReadPointer32(m_Info.StartPointers + (number * 4));

	if (rom.Console == console::DS)
	{
		pointer += m_Info.StartPointers - 4;
	}

	return pointer;
}

uint32_t ScriptEditor::GetOffset(Rom& rom)
{
	return GetOffset(rom, m_Number);
}

uint32_t* ScriptEditor::GetOffsets(Rom& rom)
{	
	uint32_t* output = new uint32_t[m_Info.ScriptCount];

	rom.Seek(m_Info.StartPointers);
	rom.Read(output, m_Info.ScriptCount * 4);

	if (rom.Console == console::DS)
	{
		for(size_t i = 0; i < m_Info.ScriptCount; ++i)
		{
			output += m_Info.StartPointers - 4;
		}
	}
	else {		

		for (size_t i = 0; i < m_Info.ScriptCount; ++i)
		{
			output[i] &= ROM_BUS_NOT;
		}
	}

	return output;
}

void ScriptEditor::SetOffset(uint32_t offset)
{	
	if (m_RomTranslated.Console == console::DS)
	{
		offset += 0x1444;
	}
	else
	{
		offset |= ROM_BUS;
	}

	m_RomTranslated.WriteUInt32(offset, m_Info.StartPointers + (m_Number * 4));
}

void ScriptEditor::SetOffsets(uint32_t* offets)
{
	if (m_RomTranslated.Console == console::DS)
	{
		for (size_t i = 0; i < m_Info.ScriptCount; ++i)
		{
			offets[i] += 0x1444;
		}
	}
	else {

		for (size_t i = 0; i < m_Info.ScriptCount; ++i)
		{
			offets[i] |= ROM_BUS;

		}
	}
}

inline uint32_t ScriptEditor::ScriptSize(uint32_t* value)
{
	if (value[0] != SCRIPT_RIFF)
		return 0xffffffff;

	return value[1];
}

inline uint32_t ScriptEditor::ScriptSize(const uint32_t& offset, Rom& rom)
{
	uint64_t data = rom.ReadUint64(offset);
	return ScriptSize((uint32_t*)&data);
}

inline uint32_t ScriptEditor::ScriptSize(const uint32_t& offset, uint8_t* bytes)
{		
	return ScriptSize((uint32_t*)(&bytes[offset]));
}

inline bool ScriptEditor::IsInsideBlock(const uint32_t& offset)
{
	return ((offset - m_Info.StartScript) <= (m_Info.StartScript - (m_Info.StartScript + m_Info.BlockLenght)));
}

inline bool ScriptEditor::IsFreeSpace(const uint32_t& offset, const uint32_t& size)
{
	uint8_t* data = new uint8_t[size];

	m_RomTranslated.Seek(offset);
	m_RomTranslated.Read(data, size);

	bool inside = IsInsideBlock(offset);
	bool free = Finder::VerifyFilled(inside ? 0x00 : 0xff, data, size);

	delete[] data;

	return free;
}

void ScriptEditor::Dump(Rom& rom)
{
	uint32_t* offsets = GetOffsets(rom);

	//More memmory, less seeks
	uint8_t* scriptBlock = new uint8_t[m_Info.BlockLenght];

	rom.Seek(m_Info.StartScript);
	rom.Read(scriptBlock, m_Info.BlockLenght);

	std::string pathFormat = PathFormat(rom);

	uint32_t maxPosition = m_Info.StartScript + m_Info.BlockLenght;

	File::CreateDir(pathFormat);	

	for (uint32_t i = 0; i < m_Info.ScriptCount; ++i)
	{
		size_t size = 0;
		std::string path = FormatPath(i, pathFormat);

		if (IsInsideBlock(offsets[i]))
		{
			//The script has been moved to another location

			size = ScriptSize(offsets[i], rom);

			if (size != 0xffffffff)
			{
				std::vector<uint8_t> bytes;
				rom.Seek(offsets[i]);
				rom.ReadBytes(bytes, size);

				File::WriteAllBytes(path, bytes);
			}

			continue;
		}

		size = ScriptSize(offsets[i] - m_Info.StartScript, scriptBlock);

		if (size == 0xffffffff)
			return;

		File::WriteAllBytes(path, scriptBlock + (offsets[i] - m_Info.StartScript), size);
	}

	delete[] offsets;
	delete[] scriptBlock;
}

void ScriptEditor::EraseScript(uint32_t offset, uint32_t size)
{
	if (size != 0xffffffff)
	{
		bool inside = IsInsideBlock(offset);

		std::string zeroBlock;
		zeroBlock.resize(size, inside ? 0x00 : 0xff);

		m_RomTranslated.Seek(offset);
		m_RomTranslated.Write(zeroBlock.data(), size);
	}
}

void ScriptEditor::InsertOnly(const Script& script, uint32_t offset)
{
	m_RomTranslated.Seek(offset);
	m_RomTranslated.Write(script.GetData(), script.GetRiffLenght());
	m_RomTranslated.Flush();
}

void ScriptEditor::InsertSmaller(Script& script, uint32_t offset, uint32_t oldSize)
{	
	uint8_t* data = new uint8_t[oldSize];

	memset(data, 0x00, oldSize);
	memcpy(data, script.GetData(), oldSize);

	m_RomTranslated.Seek(offset);
	m_RomTranslated.Write(data, oldSize);
	m_RomTranslated.Flush();

	delete[] data;
}

bool ScriptEditor::InsertVerify(Script& script, uint32_t offset, uint32_t newSize)
{
	bool free = IsFreeSpace(offset, newSize);

	if (free)
		InsertOnly(script, offset);

	return free;
}

bool ScriptEditor::InsertLarger(const Script& script, uint32_t oldOffset, uint32_t oldSize, uint32_t newSize)
{
	size_t result = newSize - oldSize;
	size_t endOffset = oldOffset + oldSize;

	bool flag = IsFreeSpace(endOffset, result);

	if (flag)
	{
		m_RomTranslated.Seek(oldOffset);
		m_RomTranslated.Write(script.GetData(), newSize);
		m_RomTranslated.Flush();
	}

	return flag;
}

void ScriptEditor::InsertMove(const Script& script, uint32_t oldOffset, uint32_t oldSize, uint32_t newOffset)
{
	SetOffset(newOffset);
	EraseScript(oldOffset, oldSize);
	InsertOnly(script, newOffset);
}

bool ScriptEditor::InsertFind(Script& script, uint32_t oldOffset, uint32_t oldSize, uint32_t newSize)
{
	uint8_t* scriptBlock = new uint8_t[m_Info.BlockLenght];

	m_RomTranslated.Seek(m_Info.StartScript);
	m_RomTranslated.Read(scriptBlock, m_Info.BlockLenght);
	
	if (IsInsideBlock(oldOffset))
		memset(scriptBlock + (m_Info.StartScript - oldOffset), 0x00, oldSize);

	bool trying = true;

	uint32_t freeSpace = 0;

	uint32_t riff = SCRIPT_RIFF;

	bool flag = false;

	while (1)
	{
		freeSpace = Finder::FindFilledAlingBlock(0x00, newSize, scriptBlock, m_Info.BlockLenght, freeSpace, 4);

		if (freeSpace == std::string::npos)
		{
			break;
		}

		uint32_t lastScript = Finder::FindBack(scriptBlock, m_Info.BlockLenght, &riff, 4, freeSpace);
		uint32_t lastSize = ScriptSize((uint32_t*)&scriptBlock[lastScript]);

		if ((lastScript + lastSize) > freeSpace)
		{
			freeSpace = lastScript + lastSize;			
			continue;
		}
	
		flag = true;
		break;
	}

	delete[] scriptBlock;

	if (flag)
	{
		InsertMove(script, oldOffset, oldSize, freeSpace + m_Info.StartScript);
	}

	return flag;
}

ScriptFlags ScriptEditor::Insert(Rom& rom, Script& script, uint32_t number)
{
	RomInfo information = GetRomInformation(rom);
	
	uint32_t oldOffset = GetOffset(rom);
	uint32_t oldSize = ScriptSize(oldOffset, rom);
	uint32_t newSize = script.GetRiffLenght();

	if (newSize <= oldSize)
	{
		if (oldSize != 0xffffffff)
		{
			InsertSmaller(script, oldOffset, oldSize);
			return ScriptFlags::INSERT_LESS;
		}

		if (InsertVerify(script, oldOffset, newSize))
			return ScriptFlags::INSERT_VERIFY;
	}
	else if (newSize > oldSize)
	{
		if (InsertLarger(script, oldOffset, oldSize, newSize))
			return ScriptFlags::INSERT_LARGER;
	}	 

	if (InsertFind(script, oldOffset, oldSize, newSize))
		return ScriptFlags::INSERT_FIND;

	return ScriptFlags::INSERT_NONE;
}

void ScriptEditor::Insert()
{
	uint8_t* scriptBlock = new uint8_t[m_Info.BlockLenght];
	uint32_t* offset = GetOffsets(m_RomTranslated);

	uint32_t currentOffset = 0;

	memset(scriptBlock, 0x00, m_Info.BlockLenght);

	std::string format = PathFormat(m_RomTranslated);

	for (size_t i = 0; i < m_Info.ScriptCount; ++i)
	{
		std::string path = FormatPath(i, format);

		wxFile file;

		if (wxFile::Exists(path))
		{
			return;
		}

		file.Open(path);
		uint32_t size = file.Length();

		file.Read(scriptBlock + currentOffset, size);
		currentOffset += size;

		while (currentOffset % 4 != 0) currentOffset++;

		if (currentOffset >= m_Info.BlockLenght && i != (m_Info.ScriptCount - 1))
			return;

		uint32_t oldOffset = GetOffset(m_RomTranslated, i);

		if (!IsInsideBlock(oldOffset))
		{
			EraseScript(oldOffset, ScriptSize((uint32_t*)scriptBlock[currentOffset]));
		}

		offset[i] = currentOffset;
	}

	delete[] scriptBlock;

	if (m_RomTranslated.Console == console::GBA)
		for (size_t i = 0; i < m_Info.ScriptCount; ++i)
			offset[i] += m_Info.StartScript;

	SetOffsets(offset);

	delete[] offset;

	m_RomTranslated.Seek(m_Info.StartScript);
	m_RomTranslated.Write(scriptBlock, m_Info.BlockLenght);
	m_RomTranslated.Flush();
}

ScriptFlags ScriptEditor::CheckAndGoScript(size_t index)
{
 	if (index > m_Info.ScriptCount)
		return ScriptFlags::ERROR_OUTOFRANGE;

	return OpenScript(index);
}