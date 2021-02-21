#pragma once

#include <wx/wx.h>

#include <moon/wx/pixeleditor.hpp>

#include "class_editor.hpp"
#include "frame_editor.hpp"

#include "class_graphics.hpp"

struct GraphicsInfo
{
	uint32_t img;
	uint32_t pal;
	uint16_t bpp;
	uint16_t w;
	uint16_t h;
};

class GraphicsEditor : public Editor
{
public:
	GraphicsEditor(id i);
private:	
	uint32_t m_PalAddress;
	Graphics m_Graphics;
	Palette m_Palette;	

	std::vector<GraphicsInfo> m_GraphicsList;
public:
	bool Open(uint32_t img, uint32_t pal, int w, int h);
	virtual bool Open(uint32_t index) override;
	virtual void SaveFile() override;	
	virtual void InsertFile() override;

	Graphics& GetGraphics() { return m_Graphics; }
	wxBitmap GetBitmap() { return wxImage(m_Graphics.GetWidth(), m_Graphics.GetHeight(), (unsigned char*)Graphics::ToImage24(m_Graphics, m_Palette)); } 
};

class GraphicsEditorFrame : public EditorFrame
{
public:
	GraphicsEditorFrame(id i);
private:
	GraphicsEditor* m_pGraphicsEditor;
private:
	virtual void OnGoFile()	override;
	virtual void UpdateFile() override;
private:
	wxPixelEditor* m_pPixelEditor;
	wxBitmap m_Bitmap;
private:
	void CreateGUIControls();
};