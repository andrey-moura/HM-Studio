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
    FramePiecesEditor(AnimatorEditor* editor, size_t currentFrame)
        : wxFrame(nullptr, wxID_ANY, L"Frame Pieces Editor"),
            m_pEditor(editor), m_CurrentFrame(currentFrame), m_Animator(editor->GetAnimator())
    {        
        CreateGUIControls();
        UpdatePieces();
        UpdatePieceInfo();

        m_pBitmapViewer->Bind(wxEVT_LEFT_DOWN, &FramePiecesEditor::OnPieceLeftDown, this);
    }
private:
    Animator& m_Animator;
    AnimatorEditor* m_pEditor;

    size_t m_CurrentFrame = 0;
    size_t m_CurrentPiece = 0;
    wxBitmap m_Frame;

    std::vector<wxRect> m_Pieces;

    wxArrayString m_Sizes;
private:
    wxBitmapView* m_pBitmapViewer;
    wxChoice* m_pSizeChoice;

    wxTextCtrl* m_pXInput;
    wxTextCtrl* m_pYInput;
private:
    void UpdatePieces()
    {
        m_Pieces.clear();        
        m_Frame = m_pEditor->MountFrame(m_CurrentFrame);

        Frame& frame = m_Animator.GetFrame(m_CurrentFrame);

        for(FramePiece& piece : frame.pieces)
        {
            m_Pieces.push_back(wxRect(piece.x, piece.y, piece.graphics.GetWidth(), piece.graphics.GetHeight()));
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
        Frame& frame = m_Animator.GetFrame(m_CurrentFrame);
        FramePiece& piece = frame.pieces[m_CurrentPiece];

        auto sprite_sizes = Animator::GetSizeList();

        for (size_t i = 0; i < m_Sizes.size(); ++i)
        {
            if (piece.graphics.GetWidth() == sprite_sizes[i].first && piece.graphics.GetHeight() == sprite_sizes[i].second)
            {
                m_pSizeChoice->SetSelection(i);
            }
        }

        m_pXInput->SetValue(std::to_wstring(piece.x));
        m_pYInput->SetValue(std::to_wstring(piece.y));
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

    void OnSizeChange(wxCommandEvent& event)
    {        
        wxString dimensions = event.GetString();

        wxSize new_size = { std::stoi(dimensions.ToStdWstring()), std::stoi(dimensions.substr(dimensions.find(L'x')+1).ToStdWstring()) };

        auto sprite_list = Animator::GetSizeList();

        Frame& frame = m_Animator.GetFrame(m_CurrentFrame);
        FramePiece& piece = frame.pieces[m_CurrentPiece];

        Graphics new_piece;
        new_piece.Create(new_size.x, new_size.y);

        int blit_x = std::min(new_size.x, (int)piece.graphics.GetWidth());
        int blit_y = std::min(new_size.y, (int)piece.graphics.GetHeight());

        int blit_tile_x = blit_x / 8;
        int blit_tile_y = blit_y / 8;

        int x = 0, y = 0;

        while (y < blit_tile_y)
        {
            while (x < blit_tile_x)
            {
                new_piece.BlitTile(piece.graphics.GetTile(x, y), x, y);

                x++;
            }

            x = 0;
            ++y;
        }

        piece.graphics = new_piece;
        frame.update_size();

        UpdatePieces();

        event.Skip();
    }

    void OnPositionEnter(wxCommandEvent& event)
    {
        Frame& frame = m_Animator.GetFrame(m_CurrentFrame);
        FramePiece& piece = frame.pieces[m_CurrentPiece];

        wxString str = event.GetString();

        int value = std::stoi(str.ToStdWstring());

        if(event.GetEventObject() == m_pXInput)
        {
            piece.x =  value;
        } else 
        {
            piece.y =  value;
        }

        frame.update_size();

        UpdatePieces();

        event.Skip();
    }

    void OnAddPiece(wxCommandEvent& event)
    {
        Frame& frame = m_Animator.GetFrame(m_CurrentFrame);
        FramePiece new_piece;

        new_piece.x = frame.w;
        new_piece.y = 0;

        new_piece.graphics.Create(8, 8);
        new_piece.palette = frame.pieces[0].palette;

        frame.pieces.push_back(new_piece);

        frame.w += 8;
        frame.h += 8;

        UpdatePieces();

        event.Skip();
    }

    void CreateGUIControls()
    {
        wxMenu* menuPieces = new wxMenu();
        Bind(wxEVT_MENU, &FramePiecesEditor::OnAddPiece, this, menuPieces->Append(wxID_ANY, L"Add Piece")->GetId());

        wxMenuBar* menuBar = new wxMenuBar();
        menuBar->Append(menuPieces, L"Pieces");

        SetMenuBar(menuBar);

        m_pBitmapViewer = new wxBitmapView(this, wxID_ANY);
        m_pBitmapViewer->SetBitmap(&m_Frame);

        wxPanel* right_panel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* right_sizer = new wxBoxSizer(wxVERTICAL);
        
        auto sizes = m_Animator.GetSizeList();

        for (size_t i = 0; i < 12; ++i)
        {
            m_Sizes.push_back(std::to_wstring(sizes[i].first) + L"x" + std::to_wstring(sizes[i].second));
        }        

        m_pSizeChoice = new wxChoice(right_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_Sizes);
        m_pSizeChoice->Bind(wxEVT_CHOICE, &FramePiecesEditor::OnSizeChange, this);

        m_pXInput = new wxTextCtrl(right_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        m_pYInput = new wxTextCtrl(right_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

        m_pXInput->SetMinSize(wxSize(32, -1));
        m_pYInput->SetMinSize(wxSize(32, -1));

        m_pXInput->Bind(wxEVT_TEXT_ENTER, &FramePiecesEditor::OnPositionEnter, this);
        m_pYInput->Bind(wxEVT_TEXT_ENTER, &FramePiecesEditor::OnPositionEnter, this);

        wxBoxSizer* position_sizer = new wxBoxSizer(wxHORIZONTAL);
        position_sizer->Add(new wxStaticText(right_panel, wxID_ANY, L"X:"));
        position_sizer->Add(m_pXInput);
        position_sizer->Add(new wxStaticText(right_panel, wxID_ANY, L"Y:"));
        position_sizer->Add(m_pYInput);

        UpdatePieceInfo();

        right_sizer->Add(m_pSizeChoice);
        right_sizer->Add(position_sizer);
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
    AnimatorEditor* m_pEditor;
    uint16_t m_SelectedFrame = 0;
public:
    FramesEditorFrame(AnimatorEditor* editor) :
        wxFrame(nullptr, wxID_ANY, L"Edit Frames"), m_Animator(editor->GetAnimator()), m_pEditor(editor)
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
        wxBitmap frame = m_pEditor->MountFrame(i);

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

        Frame& frame = m_Animator.GetFrame(m_SelectedFrame);
        if(bitmap.GetWidth() != frame.w || bitmap.GetHeight() != frame.h)
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

        for(size_t i = 0; i < pieces.size(); ++i)        
        {
            size_t cur_pixel = 0;

            Moon::wxWidgets::Bitmap::for_each_pixel(pieces[i], [&i, &frame, &cur_pixel](
                const uint8_t& r,
                const uint8_t& g,
                const uint8_t& b,
                uint8_t a)
                {
                    size_t p = frame.pieces[i].palette.find_color({r, g, b});

                    if(p == std::string::npos)
                    {
                        //Todo
                        //nearest color
                    } else                     
                    {
                        frame.pieces[i].graphics.SetPixel(p, cur_pixel);
                    }                    
                });
        }
        
        UpdateFrame(m_SelectedFrame);
    }

    void OnEditPiecesClick(wxCommandEvent& event)
    {
        FramePiecesEditor* frame = new FramePiecesEditor(m_pEditor, m_SelectedFrame);
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

        wxBitmap bmp = m_pEditor->MountFrame(m_SelectedFrame);
        bmp.SaveFile(path, wxBitmapType::wxBITMAP_TYPE_PNG);
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

    m_Frame = m_pAnimatorEditor->MountFrame(instruction.frame);

    if(instruction.time != 0)
    {        
        m_Timer.Start(instruction.time*16, true);
    }

    m_CurrentInstruction++;  

    if(m_CurrentInstruction == range.length)
    {
        m_CurrentInstruction = 0;
    }    

    m_pAnimationViewer->SetBitmap(&m_Frame);
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
    FramesEditorFrame* frame = new FramesEditorFrame(m_pAnimatorEditor);
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

    GifWriter g;
    GifBegin(&g, path.ToStdString().c_str(), 0, 0, 0);

    for (size_t anim_i = 0; anim_i < animRange.length; ++anim_i)
    {
        AnimInstruction instr = animator.GetInstruction(animRange.start + anim_i);

        wxImage img = m_pAnimatorEditor->MountFrame(instr.frame).ConvertToImage();
        
        GifWriteFrame(&g, img.GetData(), img.GetWidth(), img.GetHeight(), (instr.time*16)/10);        
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