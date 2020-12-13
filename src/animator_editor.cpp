#include "animator_editor.hpp"

AnimatorEditor::AnimatorEditor(const id& id)
    : Editor(id, L"Animator")
{
    wxFileName fn;

    fn.SetPath(m_EditorDir);
    fn.SetName(L"%s");
    fn.SetExt(L"bin");

    m_PathFormat = fn.GetFullPath();

    wxArrayString files;
    wxDir::GetAllFiles(m_EditorDir, &files, L"*.bin");        

    for(const wxString& file : files)
    {
        wxFileName file_name(file);

        wxString name = file_name.GetName();
        bool x = true;

        for(const wxChar c : name)
        {
            if(!isxdigit(c))
            {
                x = false;
                break;
            }
        }

        uint32_t offset = std::stoi(name.ToStdWstring(), nullptr, 16);

        m_Animators.push_back(offset);
    }    
}

wxString AnimatorEditor::FormatPath(const uint32_t& offset)
{
    return wxString::Format(m_PathFormat, Moon::BitConverter::ToHexString(offset));
}

wxString AnimatorEditor::FormatPath()
{
    return wxEmptyString;
}

bool AnimatorEditor::Open(uint32_t number)
{
    if(number > m_Animators.size()-1)
    {
        return false;
    }

    wxString path = FormatPath(m_Animators[number]);

    if(!wxFileExists(path))
        return false;

    wxFile file(path);
    m_Animator.LoadFromFile(file);

    m_Number = number;

    m_OldLenght = m_Animator.GetLength();
}

void AnimatorEditor::OpenNew(uint32_t offset)
{
    m_RomTranslated.Seek(offset);
    m_Animator.LoadFromFile(m_RomTranslated);

    m_Animators.push_back(offset);
    m_Number = m_Animators.size()-1;

    m_OldLenght = m_Animator.GetLength();
}

void AnimatorEditor::SaveFile()
{
    wxString path = FormatPath(m_Animators[m_Number]);

    wxFile file;
    file.Create(path, true);
    file.Open(path, wxFile::read_write);

    m_Animator.WriteToFile(file);
}

void AnimatorEditor::InsertFile()
{
    uint32_t newLength = m_Animator.GetLength();
    bool move = newLength > m_OldLenght;

    uint32_t insert_offset = m_Offset;

    uint32_t fill = 0;

    if(move)
    {
        insert_offset = m_RomTranslated.FindFreeSpace(newLength);

        if(insert_offset == std::string::npos)
        {
            wxMessageBox(L"Failed to insert the animator. No space found.", L"Error", wxICON_ERROR);
            return;
        }
    } else 
    {
        fill = m_OldLenght - newLength;
    }

    m_RomTranslated.Seek(insert_offset);
    m_Animator.WriteToFile(m_RomTranslated);

    if(move)
    {
        wxMessageBox(L"The animator was moved to " + Moon::BitConverter::ToHexString(insert_offset), L"Warning", wxICON_WARNING);
        return;
    }

    while(fill)
    {
        m_RomTranslated.WriteT<uint8_t>(0);        
        --fill;
    }

    m_RomTranslated.Flush();
}