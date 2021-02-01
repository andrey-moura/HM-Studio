#include "frame_font_editor.hpp"

#include <wx/graphics.h>

#include <moon/wx/bitmap.hpp>

//Todo:
//Move to moon/wx to use in other tools

//Struct to hold undo operation info.
struct wxPixelEditorUndo
{
    uint16_t x;
    uint16_t y;
    uint8_t r;
    uint8_t g;
    uint8_t b;

    bool operator==(const wxPixelEditorUndo& other)
    {
        return memcmp(this, &other, sizeof(wxPixelEditorUndo)) == 0;
    }
};

//Maximum of 1024 bytes of undo.
static constexpr size_t wxPixelEditorMaxUndo = 1024/sizeof(wxPixelEditorUndo);

//A controls that displays a pixel bitmap and allows drawing on it.
class wxPixelEditor : public wxBitmapView
{
public:
    wxPixelEditor(wxWindow* parent, wxWindowID id)
        : wxBitmapView(parent, id)
        {
            m_UndoList.reserve(wxPixelEditorMaxUndo);

            Bind(wxEVT_LEFT_DOWN, &wxPixelEditor::OnMouseDown, this);
            Bind(wxEVT_RIGHT_DOWN, &wxPixelEditor::OnMouseDown, this);
            Bind(wxEVT_LEFT_UP, &wxPixelEditor::OnMouseUp, this);
            Bind(wxEVT_RIGHT_UP, &wxPixelEditor::OnMouseUp, this);
            Bind(wxEVT_MOTION, &wxPixelEditor::OnMotion, this);
            Bind(wxEVT_KEY_DOWN, &wxPixelEditor::OnKeyDown, this);
        }
private:
    bool m_PixelGrid = false;
    wxColour m_Palette[2] { *wxBLACK, *wxWHITE };
    wxMouseButton m_LastKey = wxMOUSE_BTN_NONE;
    std::vector<wxPixelEditorUndo> m_UndoList;
    wxPoint m_LastPoint { -1, -1 };
    wxColour m_LastColour;
public:
    void SetPixelGrid(bool grid)
    {
        m_PixelGrid = grid;
        Refresh();
    }

    bool GetPixelGrid() const
    {
        return m_PixelGrid;
    }    

    void SetPixel(int color, int x, int y)
    {
        SetPixel(m_Palette[color], x, y);
    }

    void SetPixel(const wxColour& color, int x, int y) 
    {
        wxNativePixelData data(*GetBitmap());
        wxNativePixelData::Iterator it = data.GetPixels();

        it.Offset(data, x, y);

        wxColour old_color;
        old_color.Set(it.Red(), it.Green(), it.Blue());

        it.Red() = color.Red();
        it.Green() = color.Green();
        it.Blue() = color.Blue();

        AddUndo(old_color, x, y);

        Refresh();        
    }

    void SetPixel(int color, wxPoint point)
    {
        point.x += GetVisibleColumnsBegin()*GetScale();
        point.y += GetVisibleRowsBegin()*GetScale();

        point.x /= GetScale();
        point.y /= GetScale();

        if(point.x > GetColumnCount() || point.y > GetRowCount() || point.x < 0 || point.y < 0)
        {
            return;
        }

        if(m_LastPoint == point)
        {
            if(m_LastColour == m_Palette[color])
            {
                //Don't draw several times on the same pixel. This is very useful for motion
                return;
            }
        }

        SetPixel(color, point.x, point.y);

        m_LastPoint = point;
        m_LastColour = m_Palette[color];
    }

    void Undo()
    {
        if(!m_UndoList.size())
            return;

        wxPixelEditorUndo undo = m_UndoList.back();
        m_UndoList.pop_back();

        wxColour color(undo.r, undo.g, undo.b);

        SetPixel(color, undo.x, undo.y);

        //Clear the last operation from undo
        m_UndoList.pop_back();
    }
private:
    void AddUndo(const wxColour& color, int x, int y)
    {
        wxPixelEditorUndo undo = {(uint16_t)x, (uint16_t)y, color.Red(), color.Green(), color.Blue()};

        if(m_UndoList.size() && undo == m_UndoList.back())
        {
            return;
        }

        m_UndoList.push_back(undo);

        if(m_UndoList.size() > wxPixelEditorMaxUndo)
        {
            m_UndoList.erase(m_UndoList.begin());
        }
    }
    void OnDraw(wxDC& dc) override
    {
        wxBitmapView::OnDraw(dc);

        if(!m_PixelGrid)
            return;

        dc.SetUserScale(1, 1);

        int row_start = GetVisibleRowsBegin();
        int col_start = GetVisibleColumnsBegin();

        int row_end = GetVisibleRowsEnd();
        int col_end = GetVisibleColumnsEnd();

        int scale = GetScale();

        dc.SetPen(*wxBLACK_PEN);

        for(int y = row_start+1; y < row_end; ++y)
        {
            dc.DrawLine(col_start*scale, y*scale, col_end*scale, y*scale);
        }

        for(int x = col_start+1; x < col_end; ++x)
        {
            dc.DrawLine(x*scale, row_start*scale, x*scale, row_end*scale);
        }
    }

    void OnMouseDown(wxMouseEvent& event)
    {
        int color = event.GetButton() == wxMouseButton::wxMOUSE_BTN_RIGHT;
        m_LastKey = (wxMouseButton)event.GetButton();
        wxPoint point = event.GetPosition();

        SetPixel(color, point);

        event.Skip();
    }

    void OnMouseUp(wxMouseEvent& event)
    {
        if(m_LastKey == event.GetButton())
        {
            m_LastKey = wxMOUSE_BTN_NONE;
        }
    }

    void OnMotion(wxMouseEvent& event)
    {
        if(m_LastKey == wxMouseButton::wxMOUSE_BTN_NONE)
        {
            event.Skip();
            return;
        }

        int color = m_LastKey == wxMouseButton::wxMOUSE_BTN_RIGHT;
        wxPoint point = event.GetPosition();        

        SetPixel(color, point);
    }

    void OnKeyDown(wxKeyEvent& event)
    {
        wxChar key = event.GetUnicodeKey();

        if(key == L'Z')
        {
            if(event.ControlDown())
            {
                Undo();
            }            
        }        
    }
};

//-------------------------------------------------------------------------------------/

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

void draw_grid(wxMemoryDC& dc, int w, int h)
{
    wxSize size = dc.GetSize();

    for(int x = w-1; x < size.GetWidth(); x += w)
    {
        dc.DrawLine(x, 0, x, size.GetHeight());
    }

    for(int y = h-1; y <size.GetHeight(); y +=h)
    {
        dc.DrawLine(0, y, size.GetWidth(), y);
    }
}

//-------------------------------------------------------------------------------------/

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

    if(!m_pCurGlyph)
    {
        m_pCurGlyph = new wxBitmap(8, 16);        
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
        draw_grid(dc, 8, 16);
    }    
    
    Color* colors = Graphics::ToImage24(glyphs[m_Selection], palette);
    *m_pCurGlyph = wxImage(8, 16, (uint8_t*)colors);

    m_pGlyphEditor->SetBitmap(m_pCurGlyph);

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

    UpdateScale();
}

void FontEditorFrame::UpdateScale()
{
    m_pFontViewer->SetMaxSize(m_pBitmap->GetSize()*m_pFontViewer->GetScale());    

    Layout();
}

void FontEditorFrame::OnShowGridClick(wxCommandEvent& event)
{
    m_ShowGrid = !m_ShowGrid;
    UpdateFontViewer();

    event.Skip();
}

void FontEditorFrame::OnShowPixelGridClick(wxCommandEvent& event)
{
    m_pGlyphEditor->SetPixelGrid(!m_pGlyphEditor->GetPixelGrid());
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

    UpdateScale();
}

void FontEditorFrame::OnFontKeyDown(wxKeyEvent& event)
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
        event.Skip();
        return;
    }

    UpdateFontViewer();
    event.Skip(false);
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
    Bind(wxEVT_MENU, &FontEditorFrame::OnShowPixelGridClick, this, 
    menuView->AppendCheckItem(wxID_ANY, L"Pixel Grid")->GetId());
    menuView->AppendSubMenu(menuZoom, L"Zoom");    

    m_frameMenuBar->Append(menuView, L"View");

    m_pFontViewer = new wxBitmapView(this, wxID_ANY);
    m_pFontViewer->Bind(wxEVT_LEFT_DOWN, &FontEditorFrame::OnFontViewerClick, this);
    m_pFontViewer->Bind(wxEVT_KEY_DOWN, &FontEditorFrame::OnFontKeyDown, this);

    m_pGlyphEditor = new wxPixelEditor(this, wxID_ANY);
    m_pGlyphEditor->SetMinSize(wxSize(8, 16)*8); //always on size to fit scale
    m_pGlyphEditor->SetScale(8);

    wxBoxSizer* root_sizer = new wxBoxSizer(wxHORIZONTAL);
    root_sizer->Add(m_pFontViewer, 1, wxEXPAND);
    root_sizer->Add(m_pGlyphEditor);

    SetSizer(root_sizer);
}

