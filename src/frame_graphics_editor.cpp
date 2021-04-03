#include "frame_graphics_editor.hpp"

#include "gba_graphics.hpp"

#include <moon/wx/bitmap.hpp>

GraphicsEditor::GraphicsEditor(id i)
	: Editor(i, L"Graphics")
{
	wxArrayString files;
	wxDir::GetAllFiles(m_EditorDir, &files, L"*.png");

	for(wxString& file : files)
	{
		std::vector<wxString> strings;

		wxFileName fn(file);
		wxString name = fn.GetName();
		
		size_t pos = 0;

		while(pos != std::string::npos)
		{
			strings.push_back(name.substr(pos == 0 ? 0 : pos+1, name.find(L'.', pos)));
			pos = name.find(L'.', pos+1);
		}		

		if(strings.size() != 3)
		{
			continue;		
		}

		bool good = true;

		for(const wxString& string : strings)
		{
			for(const auto& c : string)
			{
				if(!isxdigit(c))
				{
					good = false;
				}
			}
		}

		if(!good)
		{
			continue;
		}

		wxBitmap bitmap(file, wxBitmapType::wxBITMAP_TYPE_PNG);

		if(!bitmap.IsOk())
		{
			continue;
		}

		uint32_t img_address = std::stoi(strings[0].ToStdWstring(), nullptr, 16);
		uint32_t pal_address = std::stoi(strings[1].ToStdWstring(), nullptr, 16);
		uint16_t bpp = std::stoi(strings[2].ToStdWstring());

		m_GraphicsList.push_back({ img_address, pal_address, bpp });		
	}
}

bool GraphicsEditor::Open(uint32_t index)
{
	std::string img_address = Moon::BitConverter::ToHexString(m_GraphicsList[index].img);
	std::string pal_address = Moon::BitConverter::ToHexString(m_GraphicsList[index].pal);

	m_Info.StartBlock = m_GraphicsList[index].img;
	m_PalAddress = m_GraphicsList[index].pal;

	wxString name = wxString::Format(L"%s.%s.%i", img_address, pal_address, m_GraphicsList[index].bpp);

	wxFileName fn;
	fn.SetPath(m_EditorDir);
	fn.SetName(name);
	fn.SetExt("png");

	wxBitmap bmp;
	bool loaded = bmp.LoadFile(fn.GetFullPath(), wxBitmapType::wxBITMAP_TYPE_PNG);	

	if(!bmp.IsOk())
	{
		m_GraphicsList.erase(m_GraphicsList.begin()+index);
		return false;
	}

	m_RomTranslated.Seek(m_GraphicsList[index].pal);

	m_Palette.resize(16);

	for(int i = 0; i < 16; ++i)
	{
		m_Palette[i] = gba_to_color(m_RomTranslated.ReadUInt16());
	}

	m_Graphics.Create(bmp.GetWidth(), bmp.GetHeight(), 4);

	int i = 0;

	Moon::wxWidgets::Bitmap::for_each_pixel(bmp, [this, &i](
		uint8_t& r,
		uint8_t& g,
		uint8_t& b,
		uint8_t a
	)
	{
		size_t p = m_Palette.find_color({r,g,b});

		if(p == std::string::npos)
		{
			p = m_Palette.find_nearest({r,g,b});			
		}

		m_Graphics.SetPixel(i, p);
		++i;
	});
}

bool GraphicsEditor::Open(uint32_t img, uint32_t pal, uint16_t w, uint16_t h)
{
	int index = -1;
	for(int i = 0; i < m_GraphicsList.size(); ++i)	
	{
		if(m_GraphicsList[i].img == img)
		{
			if(Open(index))
			{
				return true;
			}			
		}
	}

	if(index == -1)
	{
		index = m_GraphicsList.size();
		m_GraphicsList.push_back({img, pal, 4, w, h});

		m_Info.StartBlock = m_GraphicsList[index].img;
		m_PalAddress = m_GraphicsList[index].pal;

		if(!m_Graphics.GetData() || m_Graphics.GetWidth() != m_GraphicsList[index].w || m_Graphics.GetHeight() != m_GraphicsList[index].h)
		{
			m_Graphics.Create(m_GraphicsList[index].w, m_GraphicsList[index].h);
		}

		m_RomTranslated.Seek(m_GraphicsList[index].img);
		m_RomTranslated.Read(m_Graphics.GetData(), m_Graphics.GetLenght());

		m_RomTranslated.Seek(m_GraphicsList[index].pal);

		m_Palette.resize(16);

		for(int i = 0; i < 16; ++i)
		{
			m_Palette[i] = gba_to_color(m_RomTranslated.ReadUInt16());
		}
	}
	
	Open(index);

	return true;
}

void GraphicsEditor::SaveFile()
{
	std::string img_address = Moon::BitConverter::ToHexString(m_Info.StartBlock);
	std::string pal_address = Moon::BitConverter::ToHexString(m_PalAddress);

	wxString name = wxString::Format(L"%s.%s.%i", img_address, pal_address, (int)m_Graphics.GetBpp());

	wxFileName fn;
	fn.SetPath(m_EditorDir);
	fn.SetName(name);
	fn.SetExt(L"png");	

	wxBitmap bitmap = GetBitmap();	
	bitmap.SaveFile(fn.GetFullPath(), wxBitmapType::wxBITMAP_TYPE_PNG);
}

void GraphicsEditor::InsertFile()
{
	m_RomTranslated.Seek(m_Info.StartBlock);

	m_RomTranslated.Write(m_Graphics.GetData(), m_Graphics.GetLenght());
	m_RomTranslated.Flush();
}

//--------------------------------------------------------------------------------------//

class OpenGraphicsDialog : public wxDialog
{
public:
OpenGraphicsDialog(wxWindow* parent, wxWindowID id)
	: wxDialog(parent, id, L"Type graphics information")
	{
		m_pImgAddress = new wxTextCtrl(this, wxID_ANY, L"70017c");
		m_pPalAddress = new wxTextCtrl(this, wxID_ANY, L"6CDD04");

		wxBoxSizer* addr_sizer = new wxBoxSizer(wxHORIZONTAL);
		addr_sizer->Add(new wxStaticText(this, wxID_ANY, L"Img Address: "));
		addr_sizer->Add(m_pImgAddress);

		wxBoxSizer* pal_sizer = new wxBoxSizer(wxHORIZONTAL);
		pal_sizer->Add(new wxStaticText(this, wxID_ANY, L"Pal Address: "));
		pal_sizer->Add(m_pPalAddress);

		m_pWidth = new wxTextCtrl(this, wxID_ANY, L"256");
		m_pHeight = new wxTextCtrl(this, wxID_ANY, L"32");

		wxBoxSizer* size_sizer = new wxBoxSizer(wxHORIZONTAL);
		size_sizer->Add(m_pWidth);
		size_sizer->Add(new wxStaticText(this, wxID_ANY, L"x"));
		size_sizer->Add(m_pHeight);

		wxBoxSizer* root_sizer = new wxBoxSizer(wxVERTICAL);
		root_sizer->Add(addr_sizer);
		root_sizer->Add(pal_sizer);
		root_sizer->Add(size_sizer);
		root_sizer->Add(CreateButtonSizer(wxOK | wxCANCEL));

		SetSizerAndFit(root_sizer);		
	}
private:
	wxTextCtrl* m_pImgAddress;
	wxTextCtrl* m_pPalAddress;
	wxTextCtrl* m_pWidth;
	wxTextCtrl* m_pHeight;
public:
	int GetImgAddress()
	{
		return std::stoi(m_pImgAddress->GetValue().ToStdWstring(), nullptr, 16);
	}

	int GetPalAddress()
	{
		return std::stoi(m_pPalAddress->GetValue().ToStdWstring(), nullptr, 16);
	}

	wxSize GetImgSize()
	{
		int width = std::stoi(m_pWidth->GetValue().ToStdWstring());
		int height = std::stoi(m_pHeight->GetValue().ToStdWstring());

		return { width, height };
	}
};

//--------------------------------------------------------------------------------------//

GraphicsEditorFrame::GraphicsEditorFrame(id i)
	: EditorFrame(new GraphicsEditor(i))
{
	m_pGraphicsEditor = (GraphicsEditor*)m_pEditor;	
	CreateGUIControls();
}

void GraphicsEditorFrame::OnGoFile()
{
	OpenGraphicsDialog dialog(this, wxID_ANY);

	if(dialog.ShowModal() != wxID_OK)
		return;

	wxSize size = dialog.GetImgSize();
	int img = dialog.GetImgAddress();
	int pal = dialog.GetPalAddress();

	m_pGraphicsEditor->Open(img, pal, size.GetWidth(), size.GetHeight());

	UpdateFile();
}

void GraphicsEditorFrame::UpdateFile()
{
	Graphics& graphics = m_pGraphicsEditor->GetGraphics();	
	m_Bitmap = m_pGraphicsEditor->GetBitmap();

	m_pPixelEditor->Refresh();
}

void GraphicsEditorFrame::CreateGUIControls()
{	
	CreateMyToolBar();

	m_pPixelEditor = new wxPixelEditor(this, wxID_ANY);
	m_pPixelEditor->SetBitmap(&m_Bitmap);
}