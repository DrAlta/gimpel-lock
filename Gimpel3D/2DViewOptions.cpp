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
// 2DViewOptions.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "2DViewOptions.h"
#include <gl/gl.h>


extern bool use_cartoon_edge_detection;
bool Open_AutoSegment_Tool();

extern bool show_layer_colors;
extern bool show_mouseover_info;

extern bool view_edges;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C2DViewOptions dialog

C2DViewOptions *_2D_View_Options_Dlg = 0;

C2DViewOptions::C2DViewOptions(CWnd* pParent /*=NULL*/)
	: CDialog(C2DViewOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(C2DViewOptions)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void C2DViewOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C2DViewOptions)
	DDX_Control(pDX, IDC_RENDER_LAYER_OUTLINES, m_RenderLayerOutlines);
	DDX_Control(pDX, IDC_STATIC1, m_Static1);
	DDX_Control(pDX, IDC_BACKGROUND_COLOR_BUTTON, m_BackgroundColorButton);
	//}}AFX_DATA_MAP
	m_BackgroundColorButton.Set_Color((unsigned char)(edit_background_color[0]*255),(unsigned char)(edit_background_color[1]*255),(unsigned char)(edit_background_color[2]*255));
	CheckDlgButton(IDC_RENDER_LAYER_OUTLINES, render_layer_outlines);
	CheckDlgButton(IDC_RENDER_LAYER_MASK, render_layer_mask);
	CheckDlgButton(IDC_EDIT_CROSSHAIRS, show_edit_crosshairs);
	CheckDlgButton(IDC_USE_CARTOON_EDGE_DETECTION, use_cartoon_edge_detection);
	CheckDlgButton(IDC_SHOW_LAYER_COLORS, show_layer_colors);
	CheckDlgButton(IDC_SHOW_MOUSEOVER_INFO, show_mouseover_info);
	CheckDlgButton(IDC_VIEW_EDGES, view_edges);

	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_RENDER_LAYER_OUTLINES)->SetFont(skin_font);
	GetDlgItem(IDC_RENDER_LAYER_MASK)->SetFont(skin_font);
	GetDlgItem(IDC_EDIT_CROSSHAIRS)->SetFont(skin_font);
	GetDlgItem(IDC_USE_CARTOON_EDGE_DETECTION)->SetFont(skin_font);
	GetDlgItem(IDC_SHOW_LAYER_COLORS)->SetFont(skin_font);
	GetDlgItem(IDC_SHOW_MOUSEOVER_INFO)->SetFont(skin_font);
	GetDlgItem(IDC_VIEW_EDGES)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(C2DViewOptions, CDialog)
	//{{AFX_MSG_MAP(C2DViewOptions)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RENDER_LAYER_OUTLINES, OnRenderLayerOutlines)
	ON_BN_CLICKED(IDC_RENDER_LAYER_MASK, OnRenderLayerMask)
	ON_BN_CLICKED(IDC_BACKGROUND_COLOR_BUTTON, OnBackgroundColorButton)
	ON_BN_CLICKED(IDC_VIEW_EDGES, OnViewEdges)
	ON_BN_CLICKED(IDC_EDIT_CROSSHAIRS, OnEditCrosshairs)
	ON_BN_CLICKED(IDC_USE_CARTOON_EDGE_DETECTION, OnUseCartoonEdgeDetection)
	ON_BN_CLICKED(IDC_SHOW_LAYER_COLORS, OnShowLayerColors)
	ON_BN_CLICKED(IDC_SHOW_MOUSEOVER_INFO, OnShowMouseoverInfo)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C2DViewOptions message handlers

bool Open_2D_View_Options_Dlg()
{
	if(!_2D_View_Options_Dlg)
	{
		_2D_View_Options_Dlg = new C2DViewOptions;
		_2D_View_Options_Dlg->Create(IDD_2D_VIEW_OPTIONS);
		_2D_View_Options_Dlg->SetWindowText("2D View Options");
		Create_ToolSkin_Frame(_2D_View_Options_Dlg, "2D View Options", IDC_2D_VIEW_OPTIONS);
	}
	_2D_View_Options_Dlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

void Check_View_Edges_Checkbox(bool b)
{
	if(_2D_View_Options_Dlg)
	{
		_2D_View_Options_Dlg->CheckDlgButton(IDC_VIEW_EDGES, b);
	}
}


void C2DViewOptions::OnClose() 
{
	ShowWindow(SW_HIDE);
}

void C2DViewOptions::OnRenderLayerOutlines() 
{
	render_layer_outlines = IsDlgButtonChecked(IDC_RENDER_LAYER_OUTLINES)!=0;
	redraw_edit_window = true;
	if(render_layer_outlines)
	{
		Print_Status("true");
	}
	else
	{
		Print_Status("false");
	}
}

void C2DViewOptions::OnRenderLayerMask() 
{
	render_layer_mask = IsDlgButtonChecked(IDC_RENDER_LAYER_MASK)!=0;
	redraw_edit_window = true;
}

void C2DViewOptions::OnBackgroundColorButton() 
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK)
	{
		unsigned char rgb[3];
		COLORREF color = dlg.GetColor();
		rgb[0] = GetRValue(color);
		rgb[1] = GetGValue(color);
		rgb[2] = GetBValue(color);
		edit_background_color[0] = (float)rgb[0]/255;
		edit_background_color[1] = (float)rgb[1]/255;
		edit_background_color[2] = (float)rgb[2]/255;
		Set_Edit_Context();
		glClearColor(edit_background_color[0],edit_background_color[1],edit_background_color[2],1);
		Set_GLContext();
		m_BackgroundColorButton.Set_Color(rgb[0], rgb[1], rgb[2]);
		CWnd *b = GetDlgItem(IDC_BACKGROUND_COLOR_BUTTON);
		b->RedrawWindow();
		redraw_edit_window = true;
	}
}

void C2DViewOptions::OnViewEdges() 
{
	View_Edges(IsDlgButtonChecked(IDC_VIEW_EDGES)!=0);
	redraw_edit_window = true;
}

void C2DViewOptions::OnEditCrosshairs() 
{
	show_edit_crosshairs = IsDlgButtonChecked(IDC_EDIT_CROSSHAIRS)!=0;
	redraw_edit_window = true;
}

void C2DViewOptions::OnUseCartoonEdgeDetection() 
{
	use_cartoon_edge_detection = IsDlgButtonChecked(IDC_USE_CARTOON_EDGE_DETECTION)!=0;
	update_edge_texture = true;
	View_Edges(IsDlgButtonChecked(IDC_VIEW_EDGES)!=0);
	redraw_edit_window = true;
}

void C2DViewOptions::OnShowLayerColors() 
{
	show_layer_colors = IsDlgButtonChecked(IDC_SHOW_LAYER_COLORS)!=0;
	redraw_edit_window = true;
}

void C2DViewOptions::OnShowMouseoverInfo() 
{
	show_mouseover_info = IsDlgButtonChecked(IDC_SHOW_MOUSEOVER_INFO)!=0;
	redraw_edit_window = true;
}

HBRUSH C2DViewOptions::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}

	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
}

void C2DViewOptions::OnSetFont(CFont* pFont) 
{
	CDialog::OnSetFont(skin_font);
}
