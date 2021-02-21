#include "frame_font_editor.hpp"

#include <wx/graphics.h>

#include <moon/wx/bitmap.hpp>
#include <moon/wx/pixeleditor.hpp>

FontEditor::FontEditor(id i)
    : Editor(i, L"Font")
{
    m_Palette.resize(2);
    m_Palette[0] = { 255, 255, 255 };
    m_Palette[1] = { 0, 0, 0 };

    wxFileName fn;
    fn.SetPath(m_EditorDir);
    fn.SetFullName("font.png");

    wxString font_path = fn.GetFullPath();

    fn.SetFullName(L"table.txt");

    wxString table_path = fn.GetFullPath();

    if(!wxFileExists(font_path) || !wxFileExists(table_path))
    {
        LoadFromRom();
        return;
    }

    wxBitmap bitmap(font_path, wxBitmapType::wxBITMAP_TYPE_PNG);

    if(!bitmap.IsOk())
    {
        LoadFromRom();
        return;
    }

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

    if((bitmap.GetWidth() / 8) * (bitmap.GetHeight() / 16) < glyph_count)
    {
        LoadFromRom();
        return;
    }

    std::vector<std::string> lines = Moon::File::ReadAllLines(table_path.ToStdString(), true);

    if(lines.size() < 256)
    {
        LoadFromRom();
        return;
    }

    m_Table.reserve(256);

    for(int i = 0; i < 256; ++i)
    {
        std::string& line = lines[i];

        bool good = true;
        bool negative = line[0] == '-';

        if(negative)
        {
            line.erase(line.begin());
        }

        if(line.empty())
        {
            good = false;
        } else 
        {
            for(const char& c : line)
            {
                if(!isdigit(c))
                {
                    good = false;
                    break;
                }
            }
        }        

        int number = 0;

        if(good)
        {
            number = stoi(line);

            if(negative)
            {
                number *= -1;
            }
        } else 
        {
            number = -1;
        }

        m_Table.push_back((int16_t)number);
    }

    wxRect rect { 0, 0, 8, 16 };
    m_Glyphs.reserve(glyph_count);

    for(int i = 0; i < glyph_count; ++i)
    {
        wxBitmap sub = bitmap.GetSubBitmap(rect);
        rect.x += rect.width;

        if(rect.x == bitmap.GetWidth())
        {
            rect.x = 0;
            rect.y += rect.height;
        }

        Graphics graphics;
        graphics.Create(8, 16, 1, false);

        int index = 0;

        Moon::wxWidgets::Bitmap::for_each_pixel_24(sub, [&index, &graphics](
            uint8_t& r,
            uint8_t& g,
            uint8_t& b,
            uint8_t a
        )
        {
            graphics.SetPixel(index, r == 0);
            ++index;
        });

        m_Glyphs.push_back(graphics);
    }
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

void FontEditor::InsertFile()
{
    uint32_t glyphs_address;
    uint32_t table_address;    

    switch(m_RomTranslated.Id)
    {
        case id::MFoMT:
            glyphs_address = 0x9AB014;            
            table_address = 0x9AC6E8;
        break;
    }

    m_RomTranslated.Seek(glyphs_address);

    for(const Graphics& graphics : m_Glyphs)
    {
        m_RomTranslated.Write(graphics.GetData()+2, 12);
    }

    m_RomTranslated.Seek(table_address);
    m_RomTranslated.Write(m_Table.data(), m_Table.size()*sizeof(uint16_t));
}

void FontEditor::SaveFile()
{
    if(!wxDirExists(m_EditorDir))
    {
        wxMkdir(m_EditorDir, wxPATH_MKDIR_FULL);
    }

    int dimension = ceil(sqrt(m_Glyphs.size()));

    wxBitmap bitmap(dimension*8, dimension*16, 24);

    wxMemoryDC dc(bitmap);

    wxPoint point;

    Palette pal;
    pal.push_back({0xff, 0xff, 0xff});
    pal.push_back({0, 0, 0});

    for(const auto& glyph : m_Glyphs)
    {
        Color* colors = Graphics::ToImage24(glyph, pal);
        dc.DrawBitmap(wxBitmap(wxImage({8, 16}, (unsigned char*)colors)), point);

        point.x += 8;

        if(point.x == bitmap.GetWidth())
        {
            point.x = 0;
            point.y += 16;
        }
    }

    wxFileName fn;
    fn.SetPath(m_EditorDir);
    fn.SetName(L"font");
    fn.SetExt(L"png");

    bitmap.SaveFile(fn.GetFullPath(), wxBitmapType::wxBITMAP_TYPE_PNG);

    std::string str;

    for(const int16_t& entry : m_Table)
    {
        str += std::to_string(entry) + "\n";
    }

    fn.SetName(L"table");
    fn.SetExt(L"txt");

    wxFile file;
    file.Create(fn.GetFullPath(), true);
    file.Open(fn.GetFullPath(), wxFile::OpenMode::write);

    file.Write(str.data(), str.size());
}

Graphics& FontEditor::GetGlyph(const int& c)
{
    return m_Glyphs.at(c);
}

std::vector<Graphics>& FontEditor::GetGlyphs()
{
    return m_Glyphs;
}

std::vector<int16_t>& FontEditor::GetTable()
{
    return m_Table;
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

    CreateGUIControls();
    UpdateFontViewer();

    wxString converted("á", wxCSConv(wxFONTENCODING_CP1252), 1);
    int v = converted[0];
    
    std::string();
}

void FontEditorFrame::UpdateGlyph()
{
    auto& glyphs = m_pFontEditor->GetGlyphs();

    if(m_pCurGlyph)
    {
        Graphics& cur_glyph = glyphs[m_Selection];
        size_t index = 0;

        //Update the glyph with the curent glyph
        //To do: Only update if the glyph has changed
        Moon::wxWidgets::Bitmap::for_each_pixel_24(*m_pCurGlyph, [&cur_glyph, &index](
            uint8_t& r,
            uint8_t& g,
            uint8_t& b,
            uint8_t a)
        {        
            //It can only be 000000 or ffffff, so checking for one of the values do the job.
            cur_glyph.SetPixel(index, r == 0);
            ++index;
        });
    }

    wxString value = m_pLetters->GetValue();
    std::string letters = value.ToStdString(wxCSConv(wxFONTENCODING_CP1252));

    auto& table = m_pFontEditor->GetTable();

    for(int c = 0; c < 256; c++)
    {
        size_t pos = letters.find((char)c);
        if(table[c] == m_Selection && pos == std::string::npos)
        {
            table[c] = -1;
        } else if(table[c] != m_Selection && pos != std::string::npos)
        {
            table[c] = (int16_t)m_Selection;
        }
    }
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

    m_pFontViewer->SetBitmap(m_pBitmap);    

    std::string str;
    auto& table = m_pFontEditor->GetTable();

    pContext->SetBrush(wxBrush(wxColour(128, 0, 0, 128), wxBRUSHSTYLE_SOLID));

    if(m_ShowUnmapped)
    {
        for(int i = 0; i < glyphs.size(); ++i)
        {
            if(std::find(table.begin(), table.end(), (int16_t)i) == table.end())
            {
                x = i % w;
                y = i / w;
                pContext->DrawRectangle(x*8, y*16, 8, 16);
            }
        }
    }    
    
    delete pContext;

    for(int i = 0; i < table.size(); ++i)
    {
        if(table[i] == m_Selection)
        {
            str.push_back((char)i);
        }
    }

    m_pLetters->SetValue(wxString(str.c_str(), wxCSConv(wxFONTENCODING_CP1252), str.size()));

    UpdateScale();
}

void FontEditorFrame::UpdateScale()
{
    m_pFontViewer->SetMaxSize(m_pBitmap->GetSize()*m_pFontViewer->GetScale());    

    Layout();
}

void FontEditorFrame::OnCopyFrom(wxCommandEvent& event)
{
    wxTextEntryDialog dialog(this, L"Please, type a char to get glyph from...", "Type...");

    if(dialog.ShowModal() == wxID_OK)
    {
        wxString str = dialog.GetValue();

        if( str.size() > 1)
        {
            wxMessageBox(L"Please, type one char only.", "Huh?", wxICON_ERROR);
            return;
        }

        std::string std_str = str.ToStdString(wxCSConv(wxFONTENCODING_CP1252));

        if(std_str.size() != 1)
        {
            wxMessageBox(L"Failed to convert the character.", "Huh?", wxICON_ERROR);
            return;
        }

        auto& glyphs = m_pFontEditor->GetGlyphs();
        auto& table = m_pFontEditor->GetTable();

        int glyph = table[(unsigned char)std_str[0]];

        if(glyph == -1)
        {
            wxMessageBox(L"This character is unmaped!", L"Huh?", wxICON_ERROR);
            return;
        }

        glyphs[m_Selection] = glyphs[glyph];
        UpdateFontViewer();
    }
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

void FontEditorFrame::OnShowUnmappedClick(wxCommandEvent& event)
{
    m_ShowUnmapped = !m_ShowUnmapped;
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
    
    UpdateGlyph();
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

    int new_selection = m_Selection;

    switch(event.GetKeyCode())
    {
        case wxKeyCode::WXK_LEFT:
        if(m_Selection > 0)
        {
            new_selection--;
        }
        break;
        case wxKeyCode::WXK_RIGHT:
        if(m_Selection < glyphs.size()-1)
        {
            new_selection++;
        }
        break;
        case wxKeyCode::WXK_UP:
        if(m_Selection >= width)
        {
            new_selection-=width;
        }
        break;
        case wxKeyCode::WXK_DOWN:
        if(m_Selection + width <= glyphs.size()-1)
        {
            new_selection+=width;
        }
        break;          
        default:
        event.Skip();
        return;
    }
    
    UpdateGlyph();
    m_Selection = new_selection;
    UpdateFontViewer();
    event.Skip(false);
}

void FontEditorFrame::CreateGUIControls()
{
    CreateMyToolBar(false, false, false, true, true);

    CreateMyMenuBar();

    wxMenu* menuGlyph = new wxMenu();
    Bind(wxEVT_MENU, &FontEditorFrame::OnCopyFrom, this, 
    menuGlyph->Append(wxID_ANY, L"Copy Glyph")->GetId());

    wxMenu* menuZoom = new wxMenu();
    Bind(wxEVT_MENU, &FontEditorFrame::OnZoomClick, this, menuZoom->AppendRadioItem(wxID_ANY, L"1x")->GetId());
    Bind(wxEVT_MENU, &FontEditorFrame::OnZoomClick, this, menuZoom->AppendRadioItem(wxID_ANY, L"2x")->GetId());
    Bind(wxEVT_MENU, &FontEditorFrame::OnZoomClick, this, menuZoom->AppendRadioItem(wxID_ANY, L"4x")->GetId());

    wxMenu* menuView = new wxMenu();
    Bind(wxEVT_MENU, &FontEditorFrame::OnShowGridClick, this, 
    menuView->AppendCheckItem(wxID_ANY, L"Glyph Grid")->GetId());    
    Bind(wxEVT_MENU, &FontEditorFrame::OnShowPixelGridClick, this, 
    menuView->AppendCheckItem(wxID_ANY, L"Pixel Grid")->GetId());
    Bind(wxEVT_MENU, &FontEditorFrame::OnShowUnmappedClick, this, 
    menuView->AppendCheckItem(wxID_ANY, L"Show Unmapped")->GetId());
    menuView->AppendSubMenu(menuZoom, L"Zoom");    

    m_frameMenuBar->Append(menuGlyph, L"Glyph");
    m_frameMenuBar->Append(menuView, L"View");

    m_pFontViewer = new wxBitmapView(this, wxID_ANY);
    m_pFontViewer->Bind(wxEVT_LEFT_DOWN, &FontEditorFrame::OnFontViewerClick, this);
    m_pFontViewer->Bind(wxEVT_KEY_DOWN, &FontEditorFrame::OnFontKeyDown, this);

    m_pGlyphEditor = new wxPixelEditor(this, wxID_ANY);
    m_pGlyphEditor->SetMinSize(wxSize(8, 16)*8); //always on size to fit scale
    m_pGlyphEditor->SetScale(8);

    wxPanel* infoPanel = new wxPanel(this, wxID_ANY);
    m_pLetters = new wxTextCtrl(infoPanel, wxID_ANY);

    wxBoxSizer* info_sizer = new wxBoxSizer(wxVERTICAL);
    info_sizer->Add(m_pLetters);

    infoPanel->SetSizer(info_sizer);

    wxBoxSizer* root_sizer = new wxBoxSizer(wxHORIZONTAL);
    root_sizer->Add(m_pFontViewer, 1, wxEXPAND);

    root_sizer->Add(infoPanel);
    root_sizer->AddStretchSpacer(1);
    root_sizer->Add(m_pGlyphEditor);

    SetSizer(root_sizer);
}

