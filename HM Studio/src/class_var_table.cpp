#include "class_var_table.hpp"

VarPos::VarPos(size_t varIndex, size_t varPos) : m_VarIndex(varIndex), m_VarPos(varPos)
{

}

void VarTable::Reserve(size_t size)
{
	m_Vars.reserve(size);
	m_Names.reserve(size * m_VarSize);
}

void VarTable::Append(const char& var, const std::string& name)
{
	m_Vars.push_back(var);
	m_Names.append(name);
}

void VarTable::Input(std::vector<std::string>& strs, const Table& tbl)
{
	for (std::string& str : strs)
	{
		Input(str, tbl);
	}
}

void VarTable::Output(std::vector<std::string>& strs, const Table& tbl)
{
	for (std::string& str : strs)
	{
		Output(str, tbl);
	}
}