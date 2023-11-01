/*  Gimpel3D 2D/3D Stereo Converter
    Copyright (C) 2008-2011  Daniel René Dever (Gimpel)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
// PaintToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "PaintToolDlg.h"
#include "PaintTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PaintToolDlg dialog


PaintToolDlg *paintToolDlg = 0;

PaintToolDlg::PaintToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PaintToolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PaintToolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void PaintToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PaintToolDlg)
	DDX_Control(pDX, IDC_STATIC3, m_Static3);
	DDX_Control(pDX, IDC_STATIC2, m_Static2);
	DDX_Control(pDX, IDC_STATIC1, m_Static1);
	DDX_Control(pDX, IDC_RESET_GRAYSCALE_IMAGE, m_ResetGrayscaleImage);
	DDX_Control(pDX, IDC_GRAYSCALE_NEAR, m_GrayscaleNear);
	DDX_Control(pDX, IDC_GRAYSCALE_FAR, m_GrayscaleFar);
	DDX_Control(pDX, IDC_RENDER_LAYERMASK_IMAGE, m_RenderLayermaskImage);
	DDX_Control(pDX, IDC_RENDER_GRAYSCALE_IMAGE, m_RenderGrayscaleImage);
	DDX_Control(pDX, IDC_RENDER_FRAME_IMAGE, m_RenderFrameImage);
	DDX_Control(pDX, IDC_RENDER_PAINT_TOOL, m_RenderPaintTool);
	//}}AFX_DATA_MAP
	m_Static3.SetFont(skin_font);
	m_Static2.SetFont(skin_font);
	m_Static1.SetFont(skin_font);
	m_ResetGrayscaleImage.SetFont(skin_font);
	m_GrayscaleNear.SetFont(skin_font);
	m_GrayscaleFar.SetFont(skin_font);
	m_RenderPaintTool.SetFont(skin_font);
	m_RenderLayermaskImage.SetFont(skin_font);
	m_RenderGrayscaleImage.SetFont(skin_font);
	m_RenderFrameImage.SetFont(skin_font);
	CheckDlgButton(IDC_RENDER_PAINT_TOOL, render_paint_tool);
	CheckDlgButton(IDC_RENDER_FRAME_IMAGE, render_paint_frame_image);
	CheckDlgButton(IDC_RENDER_GRAYSCALE_IMAGE, render_paint_grayscale_image);
	CheckDlgButton(IDC_RENDER_LAYERMASK_IMAGE, render_paint_layermask_image);
}


BEGIN_MESSAGE_MAP(PaintToolDlg, CDialog)
	//{{AFX_MSG_MAP(PaintToolDlg)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RENDER_PAINT_TOOL, OnRenderPaintTool)
	ON_BN_CLICKED(IDC_RENDER_FRAME_IMAGE, OnRenderFrameImage)
	ON_BN_CLICKED(IDC_RENDER_GRAYSCALE_IMAGE, OnRenderGrayscaleImage)
	ON_BN_CLICKED(IDC_RENDER_LAYERMASK_IMAGE, OnRenderLayermaskImage)
	ON_BN_CLICKED(IDC_RESET_GRAYSCALE_IMAGE, OnResetGrayscaleImage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PaintToolDlg message handlers

bool Open_Paint_Tool_Dlg()
{
	if(!paintToolDlg)
	{
		paintToolDlg = new PaintToolDlg;
		paintToolDlg->Create(IDD_PAINT_TOOL_DLG);
		paintToolDlg->SetWindowText("Paint Tool");
		Create_ToolSkin_Frame(paintToolDlg, "Paint Tool", IDC_PAINT_TOOL);
	}
	paintToolDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_Paint_Tool_Dlg()
{
	if(!paintToolDlg)
	{
		return false;
	}
	paintToolDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}


void PaintToolDlg::OnClose() 
{
	Close_Paint_Tool();
}

HBRUSH PaintToolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
}

void PaintToolDlg::OnRenderPaintTool() 
{
	render_paint_tool = IsDlgButtonChecked(IDC_RENDER_PAINT_TOOL)!=0;
	redraw_edit_window = true;
}

void PaintToolDlg::OnRenderFrameImage() 
{
	render_paint_frame_image = IsDlgButtonChecked(IDC_RENDER_FRAME_IMAGE)!=0;
	redraw_edit_window = true;
}

void PaintToolDlg::OnRenderGrayscaleImage() 
{
	render_paint_grayscale_image = IsDlgButtonChecked(IDC_RENDER_GRAYSCALE_IMAGE)!=0;
	redraw_edit_window = true;
}

void PaintToolDlg::OnRenderLayermaskImage() 
{
	render_paint_layermask_image = IsDlgButtonChecked(IDC_RENDER_LAYERMASK_IMAGE)!=0;
	redraw_edit_window = true;
}

void PaintToolDlg::OnResetGrayscaleImage() 
{
}
