#include "Studio.h"

wxColour Studio::m_controlBackground = wxColour(30, 30, 30);
wxColour Studio::m_defaultFontColor = wxColour(220, 220, 220);
wxColour Studio::m_frameBackground = wxColour(45, 45, 48);


wxFont Studio::m_defaultFont = wxFontInfo(10).FaceName("Courier New");

wxColour& Studio::GetControlBackgroundColor()
{
	return m_controlBackground;
}

wxFont& Studio::GetDefaultFont()
{
	return m_defaultFont;
}

void Studio::SetupProgramStyle()
{
	
}

#ifdef USESPELL
Hunspell* Studio::m_sHunspell = new Hunspell("Spellchecker\\Dics\\pt_BR.aff", "Spellchecker\\Dics\\pt_BR.dic");

Hunspell* Studio::GetHunspell()
{
	return m_sHunspell;
}
#endif // USESPELL
