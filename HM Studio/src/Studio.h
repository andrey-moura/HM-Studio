#pragma once

#include <vector>
#include <wx/font.h>

class Studio
{
private:
	Studio();
	~Studio();

public:
	static wxFont GetDefaultFont() { return wxFontInfo(10).FaceName("Courier New"); }
};

