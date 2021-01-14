#include "frame_font_editor.hpp"

#include <wx/graphics.h>

#include <moon/wx/bitmap.hpp>

FontEditor::FontEditor(id i)
    : Editor(i, L"Font")
{
    m_Palette.resize(2);
    m_Palette[0] = { 255, 255, 255 };
    m_Palette[1] = { 0, 0, 0 };
}

Palette& FontEditor::GetPalette()
{
    return m_Palette;
}

void FontEditor::LoadFromRom()
{
    uint32_t glyphs_address;
    uint32_t table_address;
    int glyph_count;

    switch(m_RomTranslated.Id)
    {
        case id::MFoMT:
            glyphs_address = 0x9AB014;
            glyph_count = 0x171;
            table_address = 0x9AC6E8;
        break;
    }

    m_Glyphs.resize(glyph_count);

    m_RomTranslated.Seek(glyphs_address);

    for(int i = 0; i < glyph_count; ++i)
    {
        //1 tile width and 2 tiles height
        m_Glyphs[i].Create(8, 16, 1, false);

        //Fills first 2 lines with transparent (palette[0])
        memset(m_Glyphs[i].GetData(), 0x00, (2*8)/8);

        //Fills 12 lines with data from rom
        m_RomTranslated.Read(m_Glyphs[i].GetData()+((2*8)/8), (12*8)/8);

        //Fills last 2 lines with transparent (palette[0])
        memset(m_Glyphs[i].GetData()+((14*8)/8), 0, (2*8)/8);
    }

    m_RomTranslated.Seek(table_address);
    
    m_Table.resize(256);

    m_RomTranslated.Read(m_Table.data(), 512);    
}

Graphics& FontEditor::GetGlyph(const int& c)
{
    return m_Glyphs.at(c);
}

std::vector<Graphics>& FontEditor::GetGlyphs()
{
    return m_Glyphs;
}

int FontEditor::GetGlyphIndex(const char& c)
{
    return m_Table[(unsigned char)c];
}

//------------------------------------------------------------------------------------/-/

FontEditorFrame::FontEditorFrame(id i)
    : EditorFrame(new FontEditor(i))
{
    m_pFontEditor = (FontEditor*)m_pEditor;
    m_pFontEditor->LoadFromRom();

    CreateGUIControls();
    UpdateFontViewer();
}

void FontEditorFrame::UpdateFontViewer()
{
    auto& glyphs = m_pFontEditor->GetGlyphs();

    int size = glyphs.size();

    int dimension = ceil(sqrt(size));

    if(!m_pBitmap)
    {
        m_pBitmap = new wxBitmap(dimension*8, dimension*16, 24);
    }

    Palette& palette = m_pFontEditor->GetPalette();

    wxMemoryDC dc(*m_pBitmap);

    wxPoint point { 0, 0 };

    for(const Graphics& glyph : glyphs)
    {
        Color* colors = Graphics::ToImage24(glyph, palette);
        dc.DrawBitmap(wxBitmap(wxImage(8, 16, (uint8_t*)colors)), point);

        point.x += 8;

        if(point.x == dimension*8)
        {
            point.x = 0;
            point.y += 16;
        }
    }

    if(m_ShowGrid)
    {
        for(int x = 7; x < m_pBitmap->GetWidth(); x += 8)
        {
            dc.DrawLine(x, 0, x, m_pBitmap->GetHeight());
        }

        for(int y = 15; y < m_pBitmap->GetHeight(); y +=16)
        {
            dc.DrawLine(0, y, m_pBitmap->GetWidth(), y);
        }
    }    

    wxGraphicsContext* pContext = wxGraphicsContext::Create(*m_pBitmap);

    if(!pContext)
        return;

    pContext->SetPen(*wxTRANSPARENT_PEN);
    pContext->SetBrush(wxBrush(wxColour(0, 128, 0, 128), wxBRUSHSTYLE_SOLID));

    int w = dimension;

    int x = m_Selection % w;
    int y = m_Selection / w;

    pContext->DrawRectangle(x*8, y*16, 8, 16);

    delete pContext;

    m_pFontViewer->SetBitmap(m_pBitmap);
}

void FontEditorFrame::OnShowGridClick(wxCommandEvent& event)
{
    m_ShowGrid = !m_ShowGrid;
    UpdateFontViewer();

    event.Skip();
}

void FontEditorFrame::OnFontViewerClick(wxMouseEvent& event)
{
    event.Skip();

    if( !m_pBitmap )
        return;

    if( !m_pBitmap->IsOk() )
        return;

    wxPoint point = event.GetPosition();

    int scale = m_pFontViewer->GetScale();

    point.x /= scale;
    point.y /= scale;

    int x = point.x / 8;
    int y = point.y / 16;

    int w = m_pBitmap->GetWidth() / 8;

    int position = x + (y*w);

    if(point.x > m_pBitmap->GetWidth())
    {
        return;
    }

    if(point.y > m_pBitmap->GetHeight())
    {
        return;
    }

    if(m_Selection == position)
    {
        //No need to refresh
        return;
    } else if(position > m_pFontEditor->GetGlyphs().size()-1)
    {
        //Click outside limits
        return;
    }
    
    m_Selection = position;
    UpdateFontViewer();    
}

void FontEditorFrame::OnZoomClick(wxCommandEvent& event)
{
    wxMenuItem* item = m_frameMenuBar->FindItem(event.GetId());
    event.Skip();

    if(item == nullptr)
    {
        event.Skip(false);
        return;
    }
    
    wxString label = item->GetItemLabel();

    //Remove 'x'
    label.RemoveLast();

    if(!label.IsNumber())
    {
        event.Skip(false);
        return;
    }

    int zoom = std::stoi(label.ToStdWstring());
    m_pFontViewer->SetScale(zoom);
}

void FontEditorFrame::OnKeyDown(wxKeyEvent& event)
{
    auto& glyphs = m_pFontEditor->GetGlyphs();
    int width = m_pBitmap->GetWidth()/8;

    switch(event.GetKeyCode())
    {
        case wxKeyCode::WXK_LEFT:
        if(m_Selection > 0)
        {
            m_Selection--;
        }
        break;
        case wxKeyCode::WXK_RIGHT:
        if(m_Selection < glyphs.size()-1)
        {
            m_Selection++;
        }
        break;
        case wxKeyCode::WXK_UP:
        if(m_Selection >= width)
        {
            m_Selection-=width;
        }
        break;
        case wxKeyCode::WXK_DOWN:
        if(m_Selection + width <= glyphs.size()-1)
        {
            m_Selection+=width;
        }
        break;
        default:
        event.Skip(false);
        return;
    }

    UpdateFontViewer();
    event.Skip(true);
}

void FontEditorFrame::CreateGUIControls()
{
    CreateMyMenuBar();

    wxMenu* menuZoom = new wxMenu();
    Bind(wxEVT_MENU, &FontEditorFrame::OnZoomClick, this, menuZoom->AppendRadioItem(wxID_ANY, L"1x")->GetId());
    Bind(wxEVT_MENU, &FontEditorFrame::OnZoomClick, this, menuZoom->AppendRadioItem(wxID_ANY, L"2x")->GetId());
    Bind(wxEVT_MENU, &FontEditorFrame::OnZoomClick, this, menuZoom->AppendRadioItem(wxID_ANY, L"4x")->GetId());

    wxMenu* menuView = new wxMenu();
    Bind(wxEVT_MENU, &FontEditorFrame::OnShowGridClick, this, 
    menuView->AppendCheckItem(wxID_ANY, L"Glyph Grid")->GetId());
    menuView->AppendSubMenu(menuZoom, L"Zoom");

    m_frameMenuBar->Append(menuView, L"View");

    m_pFontViewer = new wxBitmapView(this, wxID_ANY);
    m_pFontViewer->Bind(wxEVT_LEFT_DOWN, &FontEditorFrame::OnFontViewerClick, this);
    m_pFontViewer->Bind(wxEVT_KEY_DOWN, &FontEditorFrame::OnKeyDown, this);
}

