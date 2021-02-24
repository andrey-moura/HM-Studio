#include <wx/wx.h>
#include <wx/dir.h>

#include "animator.hpp"

class AnimatorEditor : public Editor
{
public:
    AnimatorEditor(const id& id);
private:
    Animator m_Animator;
    std::vector<uint32_t> m_References;

    std::vector<uint32_t> m_Animators;
    wxString m_PathFormat;
    uint32_t m_Offset;

    uint32_t m_OldLenght = 0;    
public:
    virtual bool Open(uint32_t number) override;
    void OpenNew(uint32_t offset);
    virtual void SaveFile() override;
    virtual void InsertFile() override;

    uint32_t GetOffset() { return m_Offset; }
public:
    wxBitmap MountFrame(size_t n);

    wxString FormatPath(const uint32_t& offset);
    wxString FormatPath();

    Animator& GetAnimator() { return m_Animator; }
};