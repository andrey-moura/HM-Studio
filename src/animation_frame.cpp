#include "animation_frame.hpp"

#include <wx/textdlg.h>
#include <wx/graphics.h>
#include <wx/spinctrl.h>

#include <gif-h/gif.h>
#include <moon/wx/bitmap.hpp>

//-----------------------------------------------------------------------------------------------//
//                                      Frame Pieces Editor
//-----------------------------------------------------------------------------------------------//

class FramePiecesEditor : public wxFrame
{
public:
    FramePiecesEditor(Animator& animator, size_t currentFrame)
        : wxFrame(nullptr, wxID_ANY, L"Frame Pieces Editor"),
            m_Animator(animator), m_CurrentFrame(currentFrame)
    {
        CreateGUIControls();
        UpdatePieces();
        UpdatePieceInfo();

        m_pBitmapViewer->Bind(wxEVT_LEFT_DOWN, &FramePiecesEditor::OnPieceLeftDown, this);
    }
private:
    Animator& m_Animator;
    size_t m_CurrentFrame = 0;
    size_t m_CurrentPiece = 0;
    wxBitmap m_Frame;

    std::vector<wxRect> m_Pieces;
private:
    wxBitmapView* m_pBitmapViewer;

    wxSpinCtrl* m_pSizeSpin;
    wxSpinCtrl* m_pShapeSpin;

    wxStaticText* m_pSizeText;
private:
    void UpdateTileRange(std::pair<uint16_t, uint16_t> old_size)
    {
        FrameInfo& info = m_Animator.GetFrameInfo(m_CurrentFrame);        

        SpriteAttribute& oam = m_Animator.GetAttribute(info.oam.start+m_CurrentPiece);

        auto sprite_sizes = Animator::GetSizeList();

        auto size = sprite_sizes[oam.size + (oam.shape * 4)];
        auto tile_size = std::pair<uint16_t, uint16_t>(size.first / 8, size.second / 8);
        uint16_t tile_count = tile_size.first * tile_size.second;

        size_t old_count = (old_size.first/8)*(old_size.second/8);

        int diff = tile_count - old_count;

        if(diff == 0)
            return;

        for(size_t i = m_CurrentPiece+1; i < info.oam.length; ++i)
        {
            SpriteAttribute& _oam = m_Animator.GetAttribute(info.oam.start+i);
            _oam.tile += diff;
        }

        auto& tiles = m_Animator.GetTiles();

        if(diff > 0)
        {
            Graphics tile;
            tile.Create(8, 8);

            memset(tile.GetData(), 0, tile.GetLenght());

            for(size_t i = 0; i < diff; ++i)
            {
                tiles.insert(info.tile.start+tiles.begin()+oam.tile+old_count, tile);
            }
        } else 
        {
            for(size_t i = 0; i < diff * -1; ++i)
            {
                tiles.erase(info.tile.start+tiles.begin()+oam.tile+tile_count);
            }
        }

        auto& infos = m_Animator.GetFramesInfo();

        for(size_t i = 0; i < infos.size(); ++i)
        {
            if(infos[i].tile.start > info.tile.start+oam.tile+old_count-1)
            {
                infos[i].tile.start+=diff;
            }
        }

        info.tile.length += diff;

        m_Animator.GenerateFrames();
    }

    void UpdatePieces()
    {
        m_Pieces.clear();

        Graphics& frame = m_Animator.GetFrame(m_CurrentFrame);
        m_Frame = wxImage(frame.GetWidth(), frame.GetHeight(), (unsigned char*)Graphics::ToImage24(frame, m_Animator.GetFramePalette(m_CurrentFrame)));

        FrameInfo& info = m_Animator.GetFrameInfo(m_CurrentFrame);

        auto sprite_sizes = Animator::GetSizeList();

        auto positions = m_Animator.GetPositions(m_CurrentFrame);

        for (size_t i = 0; i < info.oam.length; ++i)
        {
            SpriteAttribute& attr = m_Animator.GetAttribute(i + info.oam.start);
            auto size = sprite_sizes[attr.size + (attr.shape * 4)];

            m_Pieces.push_back({ wxPoint(positions[i].first, positions[i].second), wxSize(size.first, size.second) });
        }

        wxMemoryDC dc(m_Frame);

        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        for (size_t i = 0; i < m_Pieces.size(); ++i)
        {
            if (m_CurrentPiece == i)
            {
                wxGraphicsContext* context = wxGraphicsContext::Create(dc);

                if (context)
                {
                    context->SetPen(*wxTRANSPARENT_PEN);
                    context->SetBrush(wxBrush(wxColour(0, 0, 128, 128), wxBRUSHSTYLE_SOLID));

                    context->DrawRectangle(m_Pieces[i].x, m_Pieces[i].y, m_Pieces[i].width, m_Pieces[i].height);

                    delete context;
                }
            }
            else
            {
                dc.SetPen(wxPen(wxColour(0, 0, 0, 255), 1, wxPENSTYLE_SOLID));
                dc.DrawRectangle(m_Pieces[i]);
            }

            m_pBitmapViewer->Refresh();
        }
    }

    void UpdatePieceInfo()
    {
        FrameInfo& info = m_Animator.GetFrameInfo(m_CurrentFrame);
        SpriteAttribute& attr = m_Animator.GetAttribute(info.oam.start + m_CurrentPiece);
        
        m_pSizeSpin->SetValue(attr.size);
        m_pShapeSpin->SetValue(attr.shape);

        auto sprite_sizes = Animator::GetSizeList();
        auto size = sprite_sizes[attr.size + (attr.shape * 4)];

        wxString size_text;
        size_text << "(" << size.first << "x" << size.second << ")";

        m_pSizeText->SetLabel(size_text);
    }

    void OnSizeChange(wxSpinEvent& event)
    {
        event.Skip();

        FrameInfo& info = m_Animator.GetFrameInfo(m_CurrentFrame);
        SpriteAttribute& oam = m_Animator.GetAttribute(info.oam.start + m_CurrentPiece);        

        int value = event.GetValue();

        if(oam.size == value)
            return;

        auto sprite_sizes = Animator::GetSizeList();
        auto size = sprite_sizes[oam.size + (oam.shape * 4)];

        oam.size = value;

        UpdateTileRange(size);
        UpdatePieces();
        UpdatePieceInfo();
    }

    void OnShapeChange(wxSpinEvent& event)
    {
        event.Skip();

        FrameInfo& info = m_Animator.GetFrameInfo(m_CurrentFrame);
        SpriteAttribute& oam = m_Animator.GetAttribute(info.oam.start + m_CurrentPiece);

        int value = event.GetValue();

        if(oam.shape == value)
            return;

        oam.shape = value;

        auto sprite_sizes = Animator::GetSizeList();
        auto size = sprite_sizes[oam.size + (oam.shape * 4)];

        UpdateTileRange(size);
        UpdatePieces();
        UpdatePieceInfo();
    }

    void OnPieceLeftDown(wxMouseEvent& event)
    {
        wxPoint mousePos = event.GetPosition();

        for (size_t i = 0; i < m_Pieces.size(); ++i)
        {
            if (m_Pieces[i].Contains(mousePos))
            {
                m_CurrentPiece = i;
                UpdatePieces();
                UpdatePieceInfo();
                break;
            }
        }

        event.Skip();
    }    

    void CreateGUIControls()
    {
        m_pBitmapViewer = new wxBitmapView(this, wxID_ANY);
        m_pBitmapViewer->SetBitmap(&m_Frame);

        wxPanel* right_panel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* right_sizer = new wxBoxSizer(wxVERTICAL);

        m_pSizeSpin = new wxSpinCtrl(right_panel, wxID_ANY);
        m_pSizeSpin->SetRange(0, 3);

        wxBoxSizer* width_sizer = new wxBoxSizer(wxHORIZONTAL);
        width_sizer->Add(new wxStaticText(right_panel, wxID_ANY, L"Size:"));        
        width_sizer->AddStretchSpacer(1); //align to right
        width_sizer->Add(m_pSizeSpin);
        
        m_pShapeSpin = new wxSpinCtrl(right_panel, wxID_ANY);
        m_pShapeSpin->SetRange(0, 2);

        wxBoxSizer* height_sizer = new wxBoxSizer(wxHORIZONTAL);
        height_sizer->Add(new wxStaticText(right_panel, wxID_ANY, L"Shape:"));
        height_sizer->AddStretchSpacer(1); //align to right
        height_sizer->Add(m_pShapeSpin);

        m_pSizeSpin->Bind(wxEVT_SPINCTRL, &FramePiecesEditor::OnSizeChange, this);
        m_pShapeSpin->Bind(wxEVT_SPINCTRL, &FramePiecesEditor::OnShapeChange, this);

        m_pSizeText = new wxStaticText(right_panel, wxID_ANY, wxEmptyString);

        right_sizer->Add(width_sizer, 0, wxEXPAND);
        right_sizer->Add(height_sizer, 0, wxEXPAND);
        right_sizer->Add(m_pSizeText);
        right_panel->SetSizer(right_sizer);

        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(m_pBitmapViewer, 1, wxEXPAND);
        sizer->Add(right_panel);

        SetSizer(sizer);
    }
};

//-----------------------------------------------------------------------------------------------//
//                                          Frames Editor
//-----------------------------------------------------------------------------------------------//

class FramesEditorFrame : public wxFrame
{
private:
    Animator& m_Animator;
    uint16_t m_SelectedFrame = 0;
public:
    FramesEditorFrame(Animator& animator) :
        wxFrame(nullptr, wxID_ANY, L"Edit Frames"), m_Animator(animator)
    {
        wxMenu* menuFrame = new wxMenu();
        Bind(wxEVT_MENU, &FramesEditorFrame::OnSwapFrameClick, this, menuFrame->Append(wxID_ANY, L"Swap tiles...")->GetId());
        Bind(wxEVT_MENU, &FramesEditorFrame::OnReplaceTilesClick, this, menuFrame->Append(wxID_ANY, L"Replace tiles...")->GetId());
        Bind(wxEVT_MENU, &FramesEditorFrame::OnEditPiecesClick, this, menuFrame->Append(wxID_ANY, L"Edit Pieces...")->GetId());
        menuFrame->AppendSeparator();
        Bind(wxEVT_MENU, &FramesEditorFrame::OnExportFrameClick, this, menuFrame->Append(wxID_ANY, L"Export")->GetId());

        wxMenuBar* menuBar = new wxMenuBar();
        menuBar->Append(menuFrame, L"Frame");

        SetMenuBar(menuBar);

        m_pFramesWindows = new wxScrolled<wxWindow>(this, wxID_ANY);        
        m_pFramesSizer = new wxBoxSizer(wxVERTICAL);

        FlushFrames();    

        m_pFramesWindows->SetMinSize(wxSize(64, 64));
        m_pFramesWindows->SetScrollRate(5, 5);
        m_pFramesWindows->SetSizer(m_pFramesSizer);

        wxPanel* right_panel = new wxPanel(this, wxID_ANY);
        right_panel->SetBackgroundColour(wxColour(255, 255, 255));

        wxBoxSizer* right_panel_sizer = new wxBoxSizer(wxVERTICAL);
        right_panel_sizer->Add(new wxStaticText(right_panel, wxID_ANY, L"Frame Properties", wxDefaultPosition, wxDefaultSize, wxTEXT_ALIGNMENT_CENTRE), 0, wxEXPAND);        

        m_pRangeStart = new wxSpinCtrl(right_panel, wxID_ANY, L"");
        m_pRangeLength = new wxSpinCtrl(right_panel, wxID_ANY, L"");

        wxBoxSizer* range_start = new wxBoxSizer(wxHORIZONTAL);
        range_start->Add(new wxStaticText(right_panel, wxID_ANY, L"Start:"));
        range_start->AddStretchSpacer(1);
        range_start->Add(m_pRangeStart);

        wxBoxSizer* range_length = new wxBoxSizer(wxHORIZONTAL);
        range_length->Add(new wxStaticText(right_panel, wxID_ANY, L"Length:"));
        range_length->AddStretchSpacer(1);
        range_length->Add(m_pRangeLength);

        right_panel_sizer->Add(range_start, 0, wxEXPAND);
        right_panel_sizer->Add(range_length, 0, wxEXPAND);

        right_panel->SetSizer(right_panel_sizer);

        wxBoxSizer* root_sizer = new wxBoxSizer(wxHORIZONTAL);        
        root_sizer->Add(m_pFramesWindows, 0, wxEXPAND);
        root_sizer->Add(right_panel);

        UpdateFrameRange();

        SetSizer(root_sizer);
    }

    void ConvertToSelected(wxBitmap& bitmap)
    {
        wxMemoryDC dc(bitmap);

        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(*wxYELLOW_PEN);
        dc.DrawRectangle(0, 0, bitmap.GetWidth(), bitmap.GetHeight());
    }

    void UpdateFrame(size_t i)
    {
        wxBitmap frame = GraphicsView::ConvertToBitmap(m_Animator.GetFrame(i), m_Animator.GetFramePalette(i));

        if(i == m_SelectedFrame)
        {
            ConvertToSelected(frame);
        }

        m_StaticBitmaps[i]->SetBitmap(frame.ConvertToImage().Scale(64, 64, wxImageResizeQuality::wxIMAGE_QUALITY_NORMAL));
    }

    void UpdateFrameRange()
    {
        FrameInfo info = m_Animator.GetFrameInfo(m_SelectedFrame);
        AnimRange range = info.tile;

        m_pRangeStart->SetValue(range.start);
        m_pRangeLength->SetValue(range.length);
    }

    void FlushFrames()
    {
        auto& frames = m_Animator.GetFrames();        

        while(m_StaticBitmaps.size() != frames.size())
        {
            if(m_StaticBitmaps.size() < frames.size())
            {
                wxStaticBitmap* staticBitmap = new wxStaticBitmap(m_pFramesWindows, wxID_ANY, wxNullBitmap);
                staticBitmap->SetMinSize(wxSize(64, 64));
                staticBitmap->SetScaleMode(wxStaticBitmapBase::ScaleMode::Scale_AspectFill);
                staticBitmap->Bind(wxEVT_LEFT_DOWN, &FramesEditorFrame::OnFrameClick, this);

                m_pFramesSizer->Add(staticBitmap);
                m_StaticBitmaps.push_back(staticBitmap);
            }
        }

        for(size_t i = 0; i < frames.size(); ++i)        
        {
            UpdateFrame(i);
        }        

        m_pFramesSizer->Layout();
    }
private:
    wxScrolled<wxWindow>* m_pFramesWindows;
    wxBoxSizer* m_pFramesSizer;
    std::vector<wxStaticBitmap*> m_StaticBitmaps;
    wxSpinCtrl* m_pRangeStart;
    wxSpinCtrl* m_pRangeLength;
private:
    void OnFrameClick(wxMouseEvent& event)
    {   
        int old_frame = m_SelectedFrame;

        for(size_t i = 0; i < m_StaticBitmaps.size(); ++i)
        {
            if(m_StaticBitmaps[i]->GetId() == event.GetId())
            {
                m_SelectedFrame = i;
                break;
            }
        }

        if(m_SelectedFrame != old_frame)
        {
            UpdateFrame(old_frame);
            UpdateFrame(m_SelectedFrame);
            UpdateFrameRange();
        }

        event.Skip();
    }
    
    void OnReplaceTilesClick(wxCommandEvent& event)
    {
        event.Skip();

        wxString path = wxLoadFileSelector(L"Image", L"png");

        if(path.empty())
            return;

        wxBitmap bitmap(path, wxBitmapType::wxBITMAP_TYPE_PNG);

        if(!bitmap.IsOk())
        {
            wxMessageBox(L"Failed to load image.", L"Error", wxICON_ERROR);
            return;
        }    

        Graphics& frame = m_Animator.GetFrame(m_SelectedFrame);
        if(bitmap.GetWidth() != frame.GetWidth() || bitmap.GetHeight() != frame.GetHeight())
        {
            wxMessageBox(L"The loaded image don't match the frame size", L"Error!", wxICON_ERROR);
            return;
        }

        std::vector<wxBitmap> pieces;
        auto positions = m_Animator.GetPositions(m_SelectedFrame);

        FrameInfo info =  m_Animator.GetFrameInfo(m_SelectedFrame);

        auto sprite_sizes = Animator::GetSizeList();

        for(size_t i = 0; i < info.oam.length; ++i)        
        {
            wxRect rect;
            rect.SetPosition({positions[i].first, positions[i].second});

            SpriteAttribute attr = m_Animator.GetAttribute(i+info.oam.start);

            auto size = sprite_sizes[attr.size + (attr.shape * 4)];            

            rect.SetSize({ size.first, size.second });

            pieces.push_back(bitmap.GetSubBitmap(rect));
        }

        std::vector<wxBitmap> tiles;
        tiles.reserve((frame.GetWidth()/8)*(frame.GetHeight()/8));

        wxRect rect { 0, 0, 8, 8 };

        for(size_t i = 0; i < pieces.size(); ++i)
        {
            while(rect.y < pieces[i].GetHeight())
            {
                while(rect.x < pieces[i].GetWidth())
                {
                    tiles.push_back(pieces[i].GetSubBitmap(rect));
                    rect.x += 8;
                }

                rect.x = 0;
                rect.y += 8;
            }

            rect.SetPosition({0,0});
        }        

        Palette& pal = m_Animator.GetFramePalette(m_SelectedFrame);

        bool wrong_color = false;

        std::vector<Graphics> gtiles;

        for(wxBitmap& tile : tiles)
        {
            Graphics gtile;
            gtile.Create(8, 8);

            size_t cur_pixel = 0;

            Moon::wxWidgets::Bitmap::for_each_pixel(tile, [&pal, &wrong_color, &gtile, &cur_pixel](
                const uint8_t& r,
                const uint8_t& g,
                const uint8_t& b,
                uint8_t a)
                {

                if(wrong_color)
                    return;
                
                Color c {r, g, b};

                size_t p = pal.find_color(c);

                if(p == std::string::npos)
                {
                    wrong_color = true;                
                    return;
                }

                gtile.SetPixel(cur_pixel, p);
                ++cur_pixel;
                });

            if(wrong_color)            
            {
                wxMessageBox("The loaded image has colors that are not in the palette.", L"Error", wxICON_ERROR);
                return;
            }

            gtiles.push_back(gtile);
        }

        AnimRange tile_range = info.tile;

        auto& anim_tiles =  m_Animator.GetTiles();

        for(size_t i = 0; i < tile_range.length; ++i)
        {
            anim_tiles[i+tile_range.start] = gtiles[i];
        }

         m_Animator.GenerateFrames();
        
        UpdateFrame(m_SelectedFrame);
    }

    void OnEditPiecesClick(wxCommandEvent& event)
    {
        FramePiecesEditor* frame = new FramePiecesEditor(m_Animator, m_SelectedFrame);
        frame->Show();

        event.Skip();
    }

    void OnSwapFrameClick(wxCommandEvent& event)
    {
        event.Skip();

        wxTextEntryDialog dialog(nullptr, L"Type a frame to swap tiles...");        
        dialog.SetTextValidator(wxTextValidatorStyle::wxFILTER_NUMERIC);

        if(dialog.ShowModal() == wxID_CANCEL)
        {
            return;
        }        

        size_t other_index = std::stoi(dialog.GetValue().ToStdWstring());

        FrameInfo other_info = m_Animator.GetFrameInfo(other_index);
        FrameInfo info =  m_Animator.GetFrameInfo(m_SelectedFrame);

        AnimRange other_range = other_info.tile;
        AnimRange range = info.tile;

        if(other_range.length != range.length)
        {
            wxMessageBox(L"Cannot swap tiles. The frames has not the same tile count", L"Error");
            return;
        }   

        auto& tiles =  m_Animator.GetTiles();

        for(size_t i = 0; i < range.length; ++i)
        {
            Graphics& tile = tiles[i+range.start];
            Graphics& other_tile = tiles[i+other_range.start];

            uint8_t* data = tile.GetData();
            
            tile.SetData(other_tile.GetData());            
            other_tile.SetData(data);
        }

        m_Animator.GenerateFrames();

        m_pFramesWindows->SetScrollPos(wxVERTICAL, 0);
        UpdateFrame(m_SelectedFrame);
        UpdateFrame(other_index);
    }

    void OnExportFrameClick(wxCommandEvent& event)
    {
        event.Skip();

        wxString path = wxSaveFileSelector(L"Image File", L"png");

        if(path.empty())
        {
            return;
        }        

        const Graphics& frame = m_Animator.GetFrame(m_SelectedFrame);

        Color* colors = Graphics::ToImage24(frame,  m_Animator.GetFramePalette(m_SelectedFrame));        

        //~wxImage takes care of deleting colors
        wxBitmap(wxImage(frame.GetWidth(), frame.GetHeight(), (unsigned char*)colors)).SaveFile(path, wxBitmapType::wxBITMAP_TYPE_PNG);
    }
};

//-----------------------------------------------------------------------------------------------//
//                                          Animation Editor
//-----------------------------------------------------------------------------------------------//

AnimationEditorFrame::AnimationEditorFrame(const id& id)
    : EditorFrame(new AnimatorEditor(id))
{
    m_pAnimatorEditor = (AnimatorEditor*)m_pEditor;

    CreateGUIControls();    
    //LoadFromRom(m_pEditor->GetRom(true), 0x6C2F38);

    m_Timer.Bind(wxEVT_TIMER, &AnimationEditorFrame::OnTimer, this);
}

void AnimationEditorFrame::UpdateFile()
{
    m_CurrentAnimation = 0;
    m_CurrentInstruction = 0;

    UpdateAnimation();
}

void AnimationEditorFrame::OnGoFile()
{
    wxTextEntryDialog dialog(nullptr, L"Please, type an offset. Prefix the offset with '-' when using offset of the first tile.", L"Open animator");

    if(dialog.ShowModal() == wxID_OK)
    {
        wxString value = dialog.GetValue();

        bool find = value[0] == '-';

        if(find)
        {
            value.erase(0, 1);
        }

        for(const wchar_t c : value)
        {
            if(!isxdigit(c))
            {
                wxMessageBox(L"Invalid offset. Please, type hex numbers only.", L"Error", wxICON_ERROR);
                return;
            }
        }

        uint32_t offset = std::stoi(value.ToStdWstring(), nullptr, 16);

        if(find)
        {            
            RomFile& rom = m_pAnimatorEditor->GetRom(true);
            rom.Seek(offset);

            offset = Animator::FindAnimatorOffset(rom);

            if(offset == 0)
            {
                wxMessageBox(L"Failed to find the animator.", L"Error,", wxICON_ERROR);
                return;
            } else 
            {
                if(
                    wxMessageBox(wxString::Format(L"Found animator at %s. Open?", Moon::BitConverter::ToHexString(offset)), wxString::FromAscii(wxMessageBoxCaptionStr), wxICON_QUESTION | wxYES_NO, this)
                    != wxYES)
                    {
                        return;
                    }
            }
        }

        m_pAnimatorEditor->OpenNew(offset);

        UpdateFile();
    }
}

// void AnimationEditorFrame::LoadFromRom(RomFile& file, uint32_t offset)
// {
//     m_Offset = offset;
//     m_CurrentAnimation = 0;
//     m_CurrentInstruction = 0;

//     file.Seek(offset);
//     m_pAnimatorEditor->GetAnimator().LoadFromFile(file);

//     m_OldSize = m_pAnimatorEditor->GetAnimator().GetLength();

//     UpdateAnimation();
// }

void AnimationEditorFrame::UpdateAnimation()
{    
    AnimRange range = m_pAnimatorEditor->GetAnimator().GetAnimRange(m_CurrentAnimation);
    m_pAnimationStart->SetValue(range.start);
    m_pAnimationLenght->SetValue(range.length);

    m_CurrentInstruction = 0;        

    auto& instrs = m_pAnimatorEditor->GetAnimator().GetInstructions();

    int rows = m_pInstructionList->GetNumberRows();
    int diff = range.length-rows;

    if(diff < 0)
    {
        m_pInstructionList->DeleteRows(0, diff*-1);
    } else if(diff > 0)
    {
        m_pInstructionList->AppendRows(diff);
    }

    for(size_t i = 0; i < range.length; ++i)
    {   
//        m_pInstructionList->InsertItem()     

        m_pInstructionList->SetReadOnly(i, 0);
        m_pInstructionList->SetCellValue(i, 0, std::to_wstring(i));
        m_pInstructionList->SetCellValue(i, 1, std::to_wstring(instrs[range.start+i].frame));
        m_pInstructionList->SetCellValue(i, 2, std::to_wstring(instrs[range.start+i].time));
    }

    Layout();
    UpdateFrame();
}

void AnimationEditorFrame::UpdateFrame()
{
    AnimRange range = m_pAnimatorEditor->GetAnimator().GetAnimRange(m_CurrentAnimation);
    
    uint32_t cur_instruction = range.start+m_CurrentInstruction;
    AnimInstruction instruction = m_pAnimatorEditor->GetAnimator().GetInstruction(cur_instruction);            

    Frame& frame = m_pAnimatorEditor->GetAnimator().m_Frames2[instruction.frame];

    if( !m_Bitmap.IsOk() || frame.w != m_Bitmap.GetWidth() || frame.h != m_Bitmap.GetHeight() )
    {
        m_Bitmap.Create(frame.w, frame.h);
    }

    wxMemoryDC dc(m_Bitmap);

    for(const FramePiece& piece : frame.pieces)
    {
        wxBitmap pieceBitmap(wxImage(piece.graphics.GetWidth(), piece.graphics.GetHeight(), (uint8_t*)Graphics::ToImage24(piece.graphics, piece.palette)));

        dc.DrawBitmap(pieceBitmap, piece.x, piece.y);
    }

    if(instruction.time != 0)
    {        
        m_Timer.Start(instruction.time*16, true);
    }

    m_CurrentInstruction++;  

    if(m_CurrentInstruction == range.length)
    {
        m_CurrentInstruction = 0;
    }    

    m_pAnimationViewer->Refresh();
}

void AnimationEditorFrame::FlushTilePalette()
{
    if(!m_TilePalette.IsOk())
    {
        m_TilePalette.Create(128, 128, 24);
        //m_pTileViewer->SetBitmap(&m_TilePalette);
    }

    wxMemoryDC dc(m_TilePalette);
    wxPoint p;

    wxBitmap tile_bmp(8, 8, 24);
    Palette& palette = m_pAnimatorEditor->GetAnimator().GetPalette(0);    

    for(const Graphics& tile : m_pAnimatorEditor->GetAnimator().GetTiles())
    {
        size_t n = 0;

         Moon::wxWidgets::Bitmap::for_each_pixel_24(tile_bmp, [&palette, &tile, &n](uint8_t& r, uint8_t& g, uint8_t& b, const uint8_t a)
         {           
             Color c = palette[tile.GetPixel(n)];
             r = c.red;
             g = c.green;
             b = c.blue;
             ++n;
         });

        dc.DrawBitmap(tile_bmp, p);

        p.x+= 8;

        if(p.x == 128)
        {
            p.x = 0;
            p.y += 8;
        }        
    }

    if(false)
    {
        for(size_t y = 8; y < 128; y += 8)
        {                
            dc.DrawLine(0, y, 128, y);
        }

        for(size_t x = 8; x < 128; x += 8)
        {
           dc.DrawLine(x, 0, x, 128);
        }
    }
   
    //m_pTileViewer->Refresh();    
}

void AnimationEditorFrame::OnPrevAnimClick(wxCommandEvent& event)
{
    if(m_CurrentAnimation > 0)
    {
        m_CurrentAnimation--;
        UpdateAnimation();
    }
    
    event.Skip();
}

void AnimationEditorFrame::OnProxAnimClick(wxCommandEvent& event)
{
    if(m_CurrentAnimation < m_pAnimatorEditor->GetAnimator().GetAnimCount()-1)
    {
        m_CurrentAnimation++;
        UpdateAnimation();
    }
    
    event.Skip();
}

void AnimationEditorFrame::OnChangingCell(wxGridEvent& event)
{
    event.Skip();

    if(event.GetCol() != 1)
        return;

    wxString str = event.GetString();

    if(!str.IsNumber())    
        return;

    size_t i = std::stoi(str.ToStdWstring());

    if(i >= m_pAnimatorEditor->GetAnimator().GetFrames().size())
    {
        event.Veto();
    }            
}

void AnimationEditorFrame::OnCellChanged(wxGridEvent& event)
{
    AnimRange range = m_pAnimatorEditor->GetAnimator().GetAnimRange(m_CurrentAnimation);
    AnimInstruction instruction = m_pAnimatorEditor->GetAnimator().GetInstruction(range.start+event.GetRow());

    wxString str = m_pInstructionList->GetCellValue(event.GetRow(), event.GetCol());

    if(!str.IsNumber())    
        return;

    size_t i = std::stoi(str.ToStdWstring());

    if(event.GetCol() == 1)
    {
        instruction.frame = i;
    } else if(event.GetCol() == 2)
    {
        instruction.time = i;
    }

    m_pAnimatorEditor->GetAnimator().SetInstruction(range.start+event.GetRow(), instruction);
    UpdateAnimation();

    event.Skip();
}

void AnimationEditorFrame::OnEditAnimFrameClick(wxCommandEvent& event) 
{
    FramesEditorFrame* frame = new FramesEditorFrame(m_pAnimatorEditor->GetAnimator());
    frame->Show();

    event.Skip();
}

void AnimationEditorFrame::OnEditFramesClick(wxCommandEvent& event)
{
    event.Skip();
}

void AnimationEditorFrame::OnEditRemoveInstrClick(wxCommandEvent& event)
{
    if(m_pAnimatorEditor->GetAnimator().GetAnimRange(m_CurrentAnimation).length == 1)
    {
        wxMessageBox(L"Can not delete frame of an animation with one frame.", L"Error", wxICON_ERROR);
        return;
    }

    wxTextEntryDialog dialog(nullptr, L"Please, select an instruction to delete");
    dialog.SetTextValidator(wxTextValidatorStyle::wxFILTER_NUMERIC);

    if(dialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    wxString str = dialog.GetValue();

    size_t instr = std::stoi(str.ToStdWstring());    

    auto& instructions = m_pAnimatorEditor->GetAnimator().GetInstructions();        

    AnimRange range = m_pAnimatorEditor->GetAnimator().GetAnimRange(m_CurrentAnimation);
    range.length--;    

    if(instr >= range.length)
    {
        wxMessageBox("Index out of range", "Error", wxICON_ERROR);
        return;
    }

    m_pAnimatorEditor->GetAnimator().SetRange(m_CurrentAnimation, range);

    instructions.erase(instructions.begin()+(instr+range.start));

    for(size_t i = m_CurrentAnimation+1; i < m_pAnimatorEditor->GetAnimator().GetAnimCount(); ++i)
    {
        AnimRange range = m_pAnimatorEditor->GetAnimator().GetAnimRange(i);
        range.start--;

        m_pAnimatorEditor->GetAnimator().SetRange(i, range);
    }

    UpdateAnimation();
    event.Skip();
}

void AnimationEditorFrame::OnInsertInstruction(wxCommandEvent& event)
{
    wxTextEntryDialog dialog(nullptr, L"Please, type the position, a frame to insert and a duration (in frames) separated by ','");

    if(dialog.ShowModal() != wxID_OK)
        return;

    std::string str = dialog.GetValue().ToStdString();

    size_t pos = 0;

    std::vector<size_t> inputs;

    for(size_t i = 0; i < 3; ++i)
    {
        inputs.push_back(std::stoi(str.substr(pos), &pos));
        pos++;
    }

    auto& instrs = m_pAnimatorEditor->GetAnimator().GetInstructions();
    auto& ranges = m_pAnimatorEditor->GetAnimator().GetAnimRanges();

    AnimInstruction instruction;
    instruction.frame = inputs[1];
    instruction.time = inputs[2];

    instrs.insert(instrs.begin() + inputs[0]+ranges[m_CurrentAnimation].start, instruction);
    ranges[m_CurrentAnimation].length++;

    for(size_t i = m_CurrentAnimation+1; i < ranges.size(); ++i)
    {
        ranges[i].start++;
    }

    UpdateAnimation();
    event.Skip();
}

void AnimationEditorFrame::OnExportAnimationClick(wxCommandEvent& event)
{
    event.Skip();    

    wxString path = wxSaveFileSelector(L"Export GIF Animation", L"gif");

    if (path.empty())
        return;

    Animator& animator = m_pAnimatorEditor->GetAnimator();
    auto animRange = animator.GetAnimRange(m_CurrentAnimation);

    auto& instrs = animator.GetInstructions();    
    
    size_t width = 0, height = 0;    

    //Find largest frame
    for (size_t i = 0; i < animRange.length; ++i)
    {
        Graphics& frame = animator.GetFrame(animRange.start + i);

        width = std::max(frame.GetWidth(), width);
        height = std::max(frame.GetHeight(), height);
    }

    GifWriter g;
    GifBegin(&g, path.ToStdString().c_str(), width, height, 0);

    for (size_t anim_i = 0; anim_i < animRange.length; ++anim_i)
    {
        AnimInstruction instr = animator.GetInstruction(animRange.start + anim_i);

        Graphics& frame = animator.GetFrame(instr.frame);
        Palette& pal = animator.GetFramePalette(instr.frame);

        uint8_t * image = Graphics::ToImage32(frame, pal);
        
        GifWriteFrame(&g, image, frame.GetWidth(), frame.GetHeight(), (instr.time*16)/10);

        delete[] image;
    }

    GifEnd(&g);
}

void AnimationEditorFrame::OnTimer(wxTimerEvent& event)
{
    UpdateFrame();
    event.Skip();
}

void AnimationEditorFrame::CreateGUIControls()
{
    wxMenu* menuAnimator = new wxMenu();    
    Bind(wxEVT_MENU, &AnimationEditorFrame::OnEditAnimFrameClick, this, menuAnimator->Append(wxID_ANY, "Edit Frames")->GetId());

    wxMenu* menuAnimation = new wxMenu();
    Bind(wxEVT_MENU, &AnimationEditorFrame::OnEditRemoveInstrClick, this, menuAnimation->Append(wxID_ANY, L"Remove Instruction...")->GetId());
    Bind(wxEVT_MENU, &AnimationEditorFrame::OnInsertInstruction, this, menuAnimation->Append(wxID_ANY, L"Insert instruction...")->GetId());   
    menuAnimation->AppendSeparator();
    Bind(wxEVT_MENU, &AnimationEditorFrame::OnExportAnimationClick, this, menuAnimation->Append(wxID_ANY, L"Export...")->GetId());    

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(menuAnimator, L"Animator");    
    menuBar->Append(menuAnimation, L"Animation");  

    SetMenuBar(menuBar);

    CreateMyToolBar(true, true, true, true, true);

    m_pAnimationViewer = new wxBitmapView(this, wxID_ANY);
    m_pAnimationViewer->SetMinSize(wxSize(128, 128));
    m_pAnimationViewer->SetBitmap(&m_Bitmap);

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_pAnimationViewer, 1, wxEXPAND);

    wxPanel* m_RightPanel = new wxPanel(this, wxID_ANY);
    m_RightPanel->SetBackgroundColour(wxColour(255, 255, 255));    

    wxBoxSizer* rightPanelSizer = new wxBoxSizer(wxVERTICAL);        

    wxStaticBoxSizer* animBoxSizer = new wxStaticBoxSizer(wxVERTICAL, m_RightPanel, L"Animation Properties");    

    wxBoxSizer* anim_nav_btn_sizer = new wxBoxSizer(wxHORIZONTAL);

    Bind(wxEVT_BUTTON, &AnimationEditorFrame::OnPrevAnimClick, this, 
    anim_nav_btn_sizer->Add(new wxButton(m_RightPanel, wxID_ANY, L"<<"))->GetWindow()->GetId());

    Bind(wxEVT_BUTTON, &AnimationEditorFrame::OnProxAnimClick, this, 
    anim_nav_btn_sizer->Add(new wxButton(m_RightPanel, wxID_ANY, L">>"))->GetWindow()->GetId());

    wxBoxSizer* range_sizer = new wxBoxSizer(wxHORIZONTAL);

    m_pAnimationStart = new wxSpinCtrl(m_RightPanel, wxID_ANY);
    m_pAnimationLenght = new wxSpinCtrl(m_RightPanel, wxID_ANY);

    range_sizer->Add(m_pAnimationStart);
    range_sizer->Add(m_pAnimationLenght);

    m_pInstructionList = new wxGrid(m_RightPanel, wxID_ANY);    
    m_pInstructionList->CreateGrid(0, 3);        
    m_pInstructionList->SetColLabelValue(0, L"Instruction");
    m_pInstructionList->SetColLabelValue(1, L"Frame");
    m_pInstructionList->SetColLabelValue(2, L"Duration");    
    m_pInstructionList->SetColFormatNumber(1);
    m_pInstructionList->SetColFormatNumber(2);
    m_pInstructionList->SetRowLabelSize(0);    
    m_pInstructionList->UseNativeColHeader();

    m_pInstructionList->Bind(wxEVT_GRID_CELL_CHANGING, &AnimationEditorFrame::OnChangingCell, this);
    m_pInstructionList->Bind(wxEVT_GRID_CELL_CHANGED, &AnimationEditorFrame::OnCellChanged, this);

    animBoxSizer->Add(anim_nav_btn_sizer);
    animBoxSizer->Add(new wxStaticText(m_RightPanel, wxID_ANY, L"Animation Range:"));        
    animBoxSizer->Add(range_sizer);
    animBoxSizer->Add(m_pInstructionList);    

    rightPanelSizer->Add(animBoxSizer);

    m_RightPanel->SetSizer(rightPanelSizer);

    //sizer->AddStretchSpacer(1);
    sizer->Add(m_RightPanel);

    SetSizer(sizer);        
}