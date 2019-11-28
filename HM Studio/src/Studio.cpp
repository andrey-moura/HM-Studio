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
