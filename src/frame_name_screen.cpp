#include "frame_name_editor.hpp"

#include <wx/graphics.h>

NameScreenEditor::NameScreenEditor(id i)
    : Editor(i, L"NameScreen"), m_FontEditor(i)
{
    int start;

    switch(m_RomTranslated.Id)
    {
        case id::MFoMT:
            start = 0xF0EFE;
        break;
    }

    wxFileName fn;
    fn.SetPath(m_EditorDir);
    fn.SetName(Moon::BitConverter::ToHexString(start));
    fn.SetExt("txt");

    if(fn.FileExists())
    {
        wxFile file(fn.GetFullPath());

        std::string s(file.Length(), '\0');
        file.Read(s.data(), file.Length());

        m_Letters.reserve(file.Length());

        for(int i = 0; i < s.size(); ++i)
        {
            if(s[i] == '\r')
            {
                //just in case
                continue;
            }

            if(s[i] == '\n')
            {
                if(m_CharactersPerRow == 0)
                {
                    m_CharactersPerRow = i;
                }

                continue;
            }

            m_Letters.push_back(s[i]);
        }

        m_Info.StartBlock = start;
        return;
    }

    LoadFromRom();
}

void NameScreenEditor::LoadFromRom()
{
    int start;

    switch(m_RomTranslated.Id)
    {
        case id::MFoMT:
            start = 0xF0EFE;
        break;
    }

    m_RomTranslated.Seek(start);

    char c = m_RomTranslated.ReadInt8();
    bool lastNull = false;    

    while(1)
    {
        if(!c)
        {
            if(lastNull)
            {                
                break;
            }
            else 
            {
                lastNull = true;

                if(!m_CharactersPerRow)
                {
                    m_CharactersPerRow = m_Letters.size();
                }
            }
        } else
        {
            lastNull = false;
            m_Letters.push_back(c);
        }

        c = m_RomTranslated.ReadInt8();
    }

    m_Info.StartBlock = start;
}

void NameScreenEditor::SaveFile()
{
    if(!wxDirExists(m_EditorDir))
    {
        wxMkdir(m_EditorDir, wxPATH_MKDIR_FULL);
    }

    std::string s;
    s.reserve(m_Letters.size() + (m_Letters.size()/m_CharactersPerRow));

    for(int i = 0; i < m_Letters.size(); ++i)
    {
        if(i != 0 && i % m_CharactersPerRow == 0)
        {
            s.push_back('\n');
        }

        s.push_back(m_Letters[i]);        
    }

    wxFileName fn;
    fn.SetPath(m_EditorDir);
    fn.SetName(Moon::BitConverter::ToHexString(m_Info.StartBlock));
    fn.SetExt("txt");

    wxFile file;
    file.Create(fn.GetFullPath(), true);
    file.Open(fn.GetFullPath(), wxFile::OpenMode::write);
    file.Write(s.c_str(), s.size());
}

void NameScreenEditor::InsertFile()
{
    m_RomTranslated.Seek(m_Info.StartBlock);

    for(int i = 0; i < m_Letters.size(); ++i)    
    {
        if(i != 0 && i % m_CharactersPerRow == 0)
        {
            m_RomTranslated.WriteT<char>(0);
        }

        m_RomTranslated.WriteT<char>(m_Letters[i]);
    }
}

//--------------------------------------------------------------------------------------//

NameScreenEditorFrame::NameScreenEditorFrame(id i)
    : EditorFrame(new NameScreenEditor(i))
    {
        m_pNameScreenEditor = (NameScreenEditor*)m_pEditor;
        CreateGUIControls();

        UpdateBitmap();
    }

void NameScreenEditorFrame::UpdateBitmap()
{    
    std::string& letters = m_pNameScreenEditor->GetLetters();
    FontEditor& fontEditor = m_pNameScreenEditor->GetFontEditor();

    auto& glyphs = fontEditor.GetGlyphs();

    Palette palette;
    palette.push_back({255, 255, 255});
    palette.push_back({0, 0, 0});    

    wxPoint point { 0, 0 };    

    if(!m_Bitmap.IsOk())
    {
        m_Bitmap.Create(240, 160, 24);
    }

    wxMemoryDC dc(m_Bitmap);

    int i = 0;

    for(const char& c : letters)
    {
        if(i != 0 && i % m_pNameScreenEditor->GetCharsPerRow() == 0)
        {
            point.x = 0;
            point.y += 16;            
        }        

        int index = fontEditor.GetGlyphIndex(c);

        if(index != -1)
        {
            Color* colors = Graphics::ToImage24(glyphs[index], palette);        
            dc.DrawBitmap(wxBitmap(wxImage(8, 16, (unsigned char*)colors)), point);   
        }

        point.x += 8;
        ++i;
    }
        
    wxGraphicsContext* pContext = wxGraphicsContext::Create(m_Bitmap);

    if(!pContext)
        return;

    pContext->SetPen(*wxTRANSPARENT_PEN);
    pContext->SetBrush(wxBrush(wxColour(0, 128, 0, 128), wxBRUSHSTYLE_SOLID));

    int w = m_pNameScreenEditor->GetCharsPerRow();

    if(w != std::string::npos)
    {
        int x = m_Selection % w;
        int y = m_Selection / w;

        pContext->DrawRectangle(x*8, y*16, 8, 16);           
    }    

    delete pContext;

    m_pBitmapView->Refresh();
}

void NameScreenEditorFrame::OnLeftDown(wxMouseEvent& event)
{
    wxPoint point = event.GetPosition();

    int x, y;

    x = point.x / 8;
    y = point.y / 16;

    int new_selection = x + (y*m_pNameScreenEditor->GetCharsPerRow());

    if(new_selection % 2 == 0)
    {
        if(new_selection < m_pNameScreenEditor->GetLetters().size())
        {
            m_Selection = new_selection;
            UpdateBitmap();
        }        
    }

    event.Skip();
}

void NameScreenEditorFrame::OnDoubleLeft(wxMouseEvent& event)
{
    wxPoint point = event.GetPosition();

    int x, y;

    x = point.x / 8;
    y = point.y / 16;

    int new_selection = x + (y*m_pNameScreenEditor->GetCharsPerRow());

    if(new_selection % 2 == 0)
    {
        if(new_selection < m_pNameScreenEditor->GetLetters().size())
        {
            wxTextEntryDialog dialog(this, L"Type the new character.");

            if(dialog.ShowModal() == wxID_OK)
            {
                wxString str = dialog.GetValue();

                if(str.size() > 1)
                {
                    std::string std_str = str.ToStdString(wxCSConv(wxFONTENCODING_CP1252));

                    char c = std_str.back();

                    auto& letters = m_pNameScreenEditor->GetLetters();

                    letters[m_Selection] = c;
                    UpdateBitmap();
                }
            }
        }        
    }

    event.Skip();
}

void insert_range(char start, char end, int index, std::string& letters)
{
    int count = (end - start) * 2;
    char cur = start;

    if(count + index >= letters.size())    
    {
        wxMessageBox(L"No space to insert all letters", L"Huh?", wxICON_ERROR);
        return;
    }        

    ++count;

    for(int i = 0; i < count; i+=2)
    {
        letters[i+index] = cur;
        cur++;
    }
}

void NameScreenEditorFrame::OnInsertUpperClick(wxCommandEvent& event)
{
    insert_range('A', 'Z', m_Selection, m_pNameScreenEditor->GetLetters());
    UpdateBitmap();
    event.Skip();
}
void NameScreenEditorFrame::OnInsertLowerClick(wxCommandEvent& event)
{
    insert_range('a', 'z', m_Selection, m_pNameScreenEditor->GetLetters());
    UpdateBitmap();
    event.Skip();
}
void NameScreenEditorFrame::OnInsertNumbersClick(wxCommandEvent& event)
{
    insert_range('0', '9', m_Selection, m_pNameScreenEditor->GetLetters());
    UpdateBitmap();
    event.Skip();
}

void NameScreenEditorFrame::CreateGUIControls()
{
    CreateMyToolBar(false, false, false, true, true);
    CreateMyMenuBar();

    wxMenu* menuInsert = new wxMenu();
    Bind(wxEVT_MENU, &NameScreenEditorFrame::OnInsertUpperClick, this,
    menuInsert->Append(wxID_ANY, L"A to Z")->GetId());
    Bind(wxEVT_MENU, &NameScreenEditorFrame::OnInsertLowerClick, this,
    menuInsert->Append(wxID_ANY, L"a to z")->GetId());
    Bind(wxEVT_MENU, &NameScreenEditorFrame::OnInsertNumbersClick, this,
    menuInsert->Append(wxID_ANY, L"0 to 9")->GetId());

    m_frameMenuBar->Append(menuInsert, L"Insert");

    m_pBitmapView = new wxBitmapView(this, wxID_ANY);    
    m_pBitmapView->SetBitmap(&m_Bitmap);

    m_pBitmapView->Bind(wxEVT_LEFT_DOWN, &NameScreenEditorFrame::OnLeftDown, this);
}