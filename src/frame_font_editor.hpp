#include "frame_editor.hpp"

#include "class_graphics.hpp"

#include <moon/wx/bmpviewer.hpp>

class FontEditor : public Editor
{
public:
    FontEditor(id i);
private:
    std::vector<Graphics> m_Glyphs;
    std::vector<int16_t> m_Table;
    Palette m_Palette;
public:
    void LoadFromRom();
    
    Graphics& GetGlyph(const int& c);
    std::vector<Graphics>& GetGlyphs();

    int GetGlyphIndex(const char& c);
    
    Palette& GetPalette();
};

class FontEditorFrame : public EditorFrame
{
public:
    FontEditorFrame(id i);
private:
    FontEditor* m_pFontEditor;
    wxBitmap* m_pBitmap = nullptr;

    int m_Selection = 0;
    bool m_ShowGrid = false;
private:
    void OnShowGridClick(wxCommandEvent& event);
    void OnFontViewerClick(wxMouseEvent& event);

    void OnZoomClick(wxCommandEvent& event);
private:
    void CreateGUIControls();
    void UpdateFontViewer();
private:
    wxBitmapView* m_pFontViewer;
};