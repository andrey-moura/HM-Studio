#pragma once

#include <wx/frame.h>

#include <moon/wx/bmpviewer.hpp>

#include "frame_editor.hpp"
#include "class_editor.hpp"

//Help for drawing
#include "frame_font_editor.hpp"

class NameScreenEditor : public Editor
{
public:
    NameScreenEditor(id i);
private:
    std::string m_Letters;
    FontEditor m_FontEditor;
    int m_CharactersPerRow = 0;
private:
    void LoadFromRom();
public:
    std::string& GetLetters()  { return m_Letters; }
    FontEditor& GetFontEditor() { return m_FontEditor; }

    int GetCharsPerRow() { return m_CharactersPerRow; }

    virtual void SaveFile() override;
    virtual void InsertFile() override;
};

class NameScreenEditorFrame : public EditorFrame 
{
public:
    NameScreenEditorFrame(id i);
private:
    NameScreenEditor* m_pNameScreenEditor;
    wxBitmapView* m_pBitmapView;
    wxBitmap m_Bitmap;

    int m_Selection = 0;    
private:
    void UpdateBitmap();
private:
    void OnLeftDown(wxMouseEvent& event);
    void OnDoubleLeft(wxMouseEvent& event);
    void OnInsertUpperClick(wxCommandEvent& event);
    void OnInsertLowerClick(wxCommandEvent& event);
    void OnInsertNumbersClick(wxCommandEvent& event);    
private:
    void CreateGUIControls();
};