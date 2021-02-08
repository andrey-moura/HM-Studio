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

    std::vector<int16_t>& GetTable();

    int GetGlyphIndex(const char& c);
    
    Palette& GetPalette();
public:
    void InsertFile() override;
    void SaveFile() override;
};

class wxPixelEditor;

class FontEditorFrame : public EditorFrame
{
public:
    FontEditorFrame(id i);
private:
    FontEditor* m_pFontEditor;
    wxBitmap* m_pBitmap = nullptr;
    wxBitmap* m_pCurGlyph = nullptr;

    int m_Selection = 0;
    bool m_ShowGrid = false;
    bool m_ShowUnmapped = false;
private:
    void OnCopyFrom(wxCommandEvent& event);
    void OnShowGridClick(wxCommandEvent& event);
    void OnShowPixelGridClick(wxCommandEvent& event);
    void OnShowUnmappedClick(wxCommandEvent& event);    
    void OnZoomClick(wxCommandEvent& event);

    void OnFontViewerClick(wxMouseEvent& event);
    void OnFontKeyDown(wxKeyEvent& event);
private:
    void CreateGUIControls();
    void UpdateFontViewer();
    void UpdateGlyph();
    void UpdateScale();
private:
    wxBitmapView* m_pFontViewer;
    wxPixelEditor* m_pGlyphEditor;
    wxTextCtrl* m_pLetters;
};