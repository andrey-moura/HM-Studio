#include <wx/wx.h>
#include <wx/animate.h>
#include <wx/spinctrl.h>
#include <wx/grid.h>

#include <moon/wx/bmpviewer.hpp>

#include "animator_editor.hpp"
#include "animator.hpp"
#include "control_graphics_view.hpp"
#include "palctrl.hpp"

#include "frame_editor.hpp"

class AnimationEditorFrame : public EditorFrame
{
public:
    AnimationEditorFrame(const id& id);
private:
    AnimatorEditor* m_pAnimatorEditor;
    wxBitmap m_TilePalette;
    wxBitmap m_Frame;    
    uint32_t m_CurrentAnimation;
    uint32_t m_CurrentInstruction;
    uint32_t m_Offset = 0;
    uint32_t m_OldSize = 0;
    wxTimer m_Timer;
private:
    void LoadFromRom(RomFile& file, uint32_t offset);
    void UpdateFrame();
    void UpdateAnimation();
    void FlushTilePalette();
private:
    virtual void UpdateFile() override;
    virtual void OnGoFile() override;
private:    
    void OnEditFramesClick(wxCommandEvent& event);
    void OnEditRemoveInstrClick(wxCommandEvent& event);    
    void OnEditAnimFrameClick(wxCommandEvent& event);    
    void OnPrevAnimClick(wxCommandEvent& event);
    void OnProxAnimClick(wxCommandEvent& event);
    void OnChangingCell(wxGridEvent& event);
    void OnCellChanged(wxGridEvent& event);
    void OnInsertInstruction(wxCommandEvent& event);
    void OnExportAnimationClick(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
private:
    GraphicsView* m_pAnimationViewer = nullptr;    
    wxSpinCtrl* m_pAnimationStart = nullptr;
    wxSpinCtrl* m_pAnimationLenght = nullptr;
    wxGrid* m_pInstructionList = nullptr;
private:
    void CreateGUIControls();
};