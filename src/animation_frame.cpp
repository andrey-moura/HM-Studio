#include "animation_frame.hpp"

#include <wx/textdlg.h>

#include <moon/wx/bitmap.hpp>

//-----------------------------------------------------------------------------------------------//
//                                          Frames Editor
//-----------------------------------------------------------------------------------------------//

class FramesEditorFrame : public wxFrame
{
private:
    AnimatorEditor& m_Animator;
    uint16_t m_SelectedFrame = 0;
public:
    FramesEditorFrame(AnimatorEditor& animator) :
        wxFrame(nullptr, wxID_ANY, L"Edit Frames"), m_Animator(animator)
    {
        wxMenu* menuFrame = new wxMenu();
        Bind(wxEVT_MENU, &FramesEditorFrame::OnSwapFrameClick, this, menuFrame->Append(wxID_ANY, L"Swap tiles...")->GetId());
        Bind(wxEVT_MENU, &FramesEditorFrame::OnReplaceTilesClick, this, menuFrame->Append(wxID_ANY, L"Replace tiles...")->GetId());
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
        wxBitmap frame = GraphicsView::ConvertToBitmap(m_Animator.GetFrame(i), m_Animator.GetPalette(0));

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

        std::vector<wxBitmap> tiles;
        tiles.reserve((frame.GetWidth()/8)*(frame.GetHeight()/8));

        wxRect rect { 0, 0, 8, 8 };

        while(rect.y < frame.GetHeight())
        {
            while(rect.x < frame.GetWidth())
            {
                tiles.push_back(bitmap.GetSubBitmap(rect));
                rect.x += 8;
            }

            rect.x = 0;
            rect.y += 8;
        }

        Palette& pal = m_Animator.GetPalette(0);

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

                size_t p = pal.FindColor(c);

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

        FrameInfo info =  m_Animator.GetFrameInfo(m_SelectedFrame);
        AnimRange tile_range = info.tile;

        auto& anim_tiles =  m_Animator.GetTiles();

        for(size_t i = 0; i < tile_range.length; ++i)
        {
            anim_tiles[i+tile_range.start] = gtiles[i];
        }

         m_Animator.GenerateFrames();
        
        UpdateFrame(m_SelectedFrame);
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

        Color* colors = Graphics::ToImage24(frame,  m_Animator.GetPalette(0));

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
    m_pAnimator = (AnimatorEditor*)m_pEditor;

    CreateGUIControls();    
    LoadFromRom(m_pEditor->GetRom(true), 0x6C2F38);

    m_Timer.Bind(wxEVT_TIMER, &AnimationEditorFrame::OnTimer, this);
}

void AnimationEditorFrame::LoadFromRom(RomFile& file, uint32_t offset)
{
    m_Offset = offset;
    m_CurrentAnimation = 0;
    m_CurrentInstruction = 0;    
    file.Seek(offset);
    m_pAnimator->LoadFromFile(file);

    m_OldSize = m_pAnimator->GetLength();

    UpdateAnimation();
}

void AnimationEditorFrame::UpdateAnimation()
{    
    AnimRange range = m_pAnimator->GetAnimRange(m_CurrentAnimation);
    m_pAnimationStart->SetValue(range.start);
    m_pAnimationLenght->SetValue(range.length);

    m_CurrentInstruction = 0;        

    auto& instrs = m_pAnimator->GetInstructions();

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
    AnimRange range = m_pAnimator->GetAnimRange(m_CurrentAnimation);
    
    uint32_t cur_instruction = range.start+m_CurrentInstruction;
    AnimInstruction instruction = m_pAnimator->GetInstruction(cur_instruction);        

    Graphics& frame = m_pAnimator->GetFrame(instruction.frame);

    if(instruction.time != 0)
    {        
        m_Timer.Start(instruction.time*16, true);
    }

    m_pAnimationViewer->SetPalette(&m_pAnimator->GetPalette(0));
    m_pAnimationViewer->SetGraphics(&frame);    

    m_CurrentInstruction++;  

    if(m_CurrentInstruction == range.length)
    {
        m_CurrentInstruction = 0;
    }    
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
    Palette& palette = m_pAnimator->GetPalette(0);    

    for(const Graphics& tile : m_pAnimator->GetTiles())
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
    if(m_CurrentAnimation < m_pAnimator->GetAnimCount()-1)
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

    if(i >= m_pAnimator->GetFrames().size())
    {
        event.Veto();
    }            
}

void AnimationEditorFrame::OnCellChanged(wxGridEvent& event)
{
    AnimRange range = m_pAnimator->GetAnimRange(m_CurrentAnimation);
    AnimInstruction instruction = m_pAnimator->GetInstruction(range.start+event.GetRow());

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

    m_pAnimator->SetInstruction(range.start+event.GetRow(), instruction);
    UpdateAnimation();

    event.Skip();
}

void AnimationEditorFrame::OnOpenAnimationClick(wxCommandEvent& event)
{
    wxTextEntryDialog dialog(nullptr, L"Type an offset", "Open Animation");
    dialog.SetTextValidator(wxTextValidatorStyle::wxFILTER_XDIGITS);

    if(dialog.ShowModal() == wxID_OK)
    {    
        LoadFromRom(m_pEditor->GetRom(true), std::stoi(dialog.GetValue().ToStdWstring(), nullptr, 16));
    }

    event.Skip();
}

void AnimationEditorFrame::OnInsertAnimator(wxCommandEvent& event)
{
    event.Skip();
    
    uint32_t newSize = m_pAnimator->GetLength();
    bool move = newSize > m_OldSize;

    uint32_t insert_offset = m_Offset;

    uint32_t fill = 0;

    if(move)
    {
        insert_offset = m_pEditor->GetRom(true).FindFreeSpace(newSize);

        if(insert_offset == std::string::npos)
        {
            wxMessageBox(L"Failed to insert the animator. No space found.", L"Error", wxICON_ERROR);
            return;
        }
    } else 
    {
        fill = m_OldSize - newSize;
    }

    m_pEditor->GetRom(true).Seek(insert_offset);
    m_pAnimator->WriteToFile(m_pEditor->GetRom(true));

    if(move)
    {
        wxMessageBox(L"The animator was moved to " + Moon::BitConverter::ToHexString(insert_offset), L"Warning", wxICON_WARNING);
        return;
    }
    
    while(fill)
    {
        m_pEditor->GetRom(true).WriteT<uint8_t>(0);
        --fill;
    }
}

void AnimationEditorFrame::OnEditAnimFrameClick(wxCommandEvent& event) 
{
    FramesEditorFrame* frame = new FramesEditorFrame(*m_pAnimator);
    frame->Show();

    event.Skip();
}

void AnimationEditorFrame::OnEditFramesClick(wxCommandEvent& event)
{
    event.Skip();
}

void AnimationEditorFrame::OnEditRemoveInstrClick(wxCommandEvent& event)
{
    if(m_pAnimator->GetAnimRange(m_CurrentAnimation).length == 1)
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

    auto& instructions = m_pAnimator->GetInstructions();        

    AnimRange range = m_pAnimator->GetAnimRange(m_CurrentAnimation);
    range.length--;    

    if(instr >= range.length)
    {
        wxMessageBox("Index out of range", "Error", wxICON_ERROR);
        return;
    }

    m_pAnimator->SetRange(m_CurrentAnimation, range);

    instructions.erase(instructions.begin()+(instr+range.start));

    for(size_t i = m_CurrentAnimation+1; i < m_pAnimator->GetAnimCount(); ++i)
    {
        AnimRange range = m_pAnimator->GetAnimRange(i);
        range.start--;

        m_pAnimator->SetRange(i, range);
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

    auto& instrs = m_pAnimator->GetInstructions();
    auto& ranges = m_pAnimator->GetAnimRanges();

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

void AnimationEditorFrame::OnTimer(wxTimerEvent& event)
{
    UpdateFrame();
    event.Skip();
}

void AnimationEditorFrame::CreateGUIControls()
{
    wxMenu* menuAnimator = new wxMenu();
    Bind(wxEVT_MENU, &AnimationEditorFrame::OnOpenAnimationClick, this, menuAnimator->Append(wxID_ANY, "Open Animator...")->GetId());
    Bind(wxEVT_MENU, &AnimationEditorFrame::OnInsertAnimator, this, menuAnimator->Append(wxID_ANY, "Insert Animator")->GetId());
    Bind(wxEVT_MENU, &AnimationEditorFrame::OnEditAnimFrameClick, this, menuAnimator->Append(wxID_ANY, "Edit Frames")->GetId());

    wxMenu* menuAnimation = new wxMenu();
    Bind(wxEVT_MENU, &AnimationEditorFrame::OnEditFramesClick, this, menuAnimation->Append(wxID_ANY, L"Edit Frames")->GetId());
    Bind(wxEVT_MENU, &AnimationEditorFrame::OnEditRemoveInstrClick, this, menuAnimation->Append(wxID_ANY, L"Remove Instruction...")->GetId());
    Bind(wxEVT_MENU, &AnimationEditorFrame::OnInsertInstruction, this, menuAnimation->Append(wxID_ANY, L"Insert instruction...")->GetId());    

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(menuAnimator, L"Animator");    
    menuBar->Append(menuAnimation, L"Animation");  

    SetMenuBar(menuBar);

    CreateMyToolBar(true, true, true, true, true);

    m_pAnimationViewer = new GraphicsView(this);
    m_pAnimationViewer->SetMinSize(wxSize(128, 128));

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_pAnimationViewer, 0, wxEXPAND);

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

    sizer->AddStretchSpacer(1);
    sizer->Add(m_RightPanel);

    SetSizer(sizer);        
}