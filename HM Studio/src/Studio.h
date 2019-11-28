#pragma once

#include <vector>
#include <wx/font.h>
#include <wx/colour.h>

class Studio
{
private:
	Studio();
	~Studio();

public:
	static wxColour& GetControlBackgroundColor();
	static wxFont& GetDefaultFont();
	static wxColour& GetFontColour() { return m_defaultFontColor; }
	static wxColour& GetFrameColour() { return m_frameBackground; }

	static void SetupProgramStyle();
private:
	static wxFont m_defaultFont;
	static wxColour m_defaultFontColor;
	static wxColour m_controlBackground;
	static wxColour m_frameBackground;
};

