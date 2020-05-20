#pragma once

#include <string>
#include <vector>
#include <string_view>

#include <moon/table.hpp>
#include "class_stop_watch.hpp"

struct VarPos
{
	VarPos(size_t varIndex, size_t varPos);

	size_t m_VarIndex = 0;
	size_t m_VarPos = 0;	
};

class VarTable
{
public:
	VarTable() = default;
	void Reserve(size_t size);
	void Append(const char& var, const std::string& name);
public:
	__forceinline void Input(std::string& str, const Moon::Hacking::Table& tbl)
	{
		size_t varPos = str.find('ÿ');		

		std::vector<VarPos> poss;

		while (varPos != std::string::npos)
		{
			const char& varType = str[varPos + 1];

			size_t varIndex = m_Vars.find(varType);

			if (varIndex != std::string::npos)
			{
				poss.push_back(VarPos(varIndex, varPos + (poss.size() * (m_VarSize - 2))));				
			}

			varPos = str.find('ÿ', varPos + 1);
		}

		tbl.Input(str);

		size_t newSize = str.size() + ((m_VarSize - 2) * poss.size());

		str.reserve(newSize);		

		for (const VarPos pos : poss)
		{
			std::string_view name(m_Names.c_str() + (pos.m_VarIndex * m_VarSize), m_VarSize);

			//Do not make copies of the string
			str[pos.m_VarPos + 1] = name[m_VarSize - 1];
			str[pos.m_VarPos] = name[m_VarSize - 2];

			str.insert(pos.m_VarPos, name.data(), m_VarSize - 2);
		}		
	}
	__forceinline void Output(std::string& str, const Moon::Hacking::Table& tbl)
	{
		std::vector<VarPos> poss;

		size_t namePos = str.find('<');

		while (namePos != std::string::npos)
		{
			std::string_view sv(str.c_str() + namePos, m_VarSize);
			size_t nameIndex = m_Names.find(sv);

#ifdef _DEBUG
			_STL_VERIFY(nameIndex % m_VarSize == 0, "Name var not align, you must use a proper separator");
#endif //_DEBUG

			poss.push_back(VarPos(nameIndex / m_VarSize, namePos - (poss.size() * 10)));
			namePos = str.find('<', namePos + m_VarSize);
		}

		tbl.Output(str);

		for (const VarPos pos : poss)
		{
			const char& var = m_Vars[pos.m_VarIndex];

			//Do not make copies of the string
			str[pos.m_VarPos + 1] = var;
			str[pos.m_VarPos] = 'ÿ';

			str.erase(pos.m_VarPos + 2, m_VarSize - 2);
		}
	}
	
	void Input(std::vector<std::string>& strs, const Moon::Hacking::Table& tbl);
	void Output(std::vector<std::string>& strs, const Moon::Hacking::Table& tbl);
//Acessors
public:
	size_t Size() const { return m_Vars.size(); }
	size_t GetVarSize() const { return m_VarSize; }

	void SetVarSize(size_t varSize) { m_VarSize = varSize; }

	std::string_view GetName(size_t index) const;
	inline const std::string& GetNames() { return m_Names; };
//Members
private:
	std::string m_Vars;
	std::string m_Names;
	
	size_t m_VarSize;	
};

