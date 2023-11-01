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
// 3DViewOptions.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "3DViewOptions.h"
#include <gl/gl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3DViewOptions dialog

C3DViewOptions *_3D_View_Options_Dlg = 0;

extern float mouse_sensitivity;

C3DViewOptions::C3DViewOptions(CWnd* pParent /*=NULL*/)
	: CDialog(C3DViewOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(C3DViewOptions)
	//}}AFX_DATA_INIT
}


void C3DViewOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C3DViewOptions)
	DDX_Control(pDX, IDC_SET_SPECKLE_SKIP, m_SetSpeckleSkip);
	DDX_Control(pDX, IDC_RECALC_CENTER, m_ReCalcCenter);
	DDX_Control(pDX, IDC_CENTER_VIEW, m_CenterView);
	DDX_Control(pDX, IDC_PROJECTION_SETTINGS, m_ProjectionSettings);
	DDX_Control(pDX, IDC_BACKGROUND_COLOR_BUTTON, m_BackgroundColorButton);
	DDX_Control(pDX, IDC_FLY_SPEED_SLIDER, m_FlySpeedSlider);
	//}}AFX_DATA_MAP
	m_BackgroundColorButton.Set_Color((unsigned char)(background_color[0]*255),(unsigned char)(background_color[1]*255),(unsigned char)(background_color[2]*255));
	m_FlySpeedSlider.SetRange(1, 200);
	m_FlySpeedSlider.SetPos(100);
	CheckDlgButton(IDC_CULL_BACKFACES, cull_backfaces);
	CheckDlgButton(IDC_RENDER_BORDERS, render_borders);
	CheckDlgButton(IDC_RENDER_NORMALS, render_normals);
	char text[64];
	sprintf(text, "%i", speckle_skip);
	SetDlgItemText(IDC_SPECKLE_SKIP, text);

	GetDlgItem(IDC_FLY_SPEED_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_BACKGROUND_COLOR_BUTTON)->SetFont(skin_font);
	GetDlgItem(IDC_CULL_BACKFACES)->SetFont(skin_font);
	GetDlgItem(IDC_RENDER_BORDERS)->SetFont(skin_font);
	GetDlgItem(IDC_RENDER_NORMALS)->SetFont(skin_font);
	GetDlgItem(IDC_RECALC_CENTER)->SetFont(skin_font);
	GetDlgItem(IDC_CENTER_VIEW)->SetFont(skin_font);
	GetDlgItem(IDC_PROJECTION_SETTINGS)->SetFont(skin_font);
	GetDlgItem(IDC_SPECKLE_SKIP)->SetFont(skin_font);
	GetDlgItem(IDC_SET_SPECKLE_SKIP)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC3)->SetFont(skin_font);
	GetDlgItem(IDC_ANTIALIAS_HARD_EDGES)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(C3DViewOptions, CDialog)
	//{{AFX_MSG_MAP(C3DViewOptions)
	ON_BN_CLICKED(IDC_RENDER_BORDERS, OnRenderBorders)
	ON_BN_CLICKED(IDC_CULL_BACKFACES, OnCullBackfaces)
	ON_BN_CLICKED(IDC_RENDER_NORMALS, OnRenderNormals)
	ON_BN_CLICKED(IDC_BACKGROUND_COLOR_BUTTON, OnBackgroundColorButton)
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RECALC_CENTER, OnRecalcCenter)
	ON_BN_CLICKED(IDC_CENTER_VIEW, OnCenterView)
	ON_BN_CLICKED(IDC_RENDER_GRID, OnRenderGrid)
	ON_BN_CLICKED(IDC_PROJECTION_SETTINGS, OnProjectionSettings)
	ON_BN_CLICKED(IDC_SET_SPECKLE_SKIP, OnSetSpeckleSkip)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DViewOptions message handlers

void C3DViewOptions::OnRenderBorders() 
{
	render_borders = IsDlgButtonChecked(IDC_RENDER_BORDERS)!=0;
	Enable_All_Borders(render_borders);
	redraw_frame = true;
}

void C3DViewOptions::OnCullBackfaces() 
{
	cull_backfaces = IsDlgButtonChecked(IDC_CULL_BACKFACES)!=0;
	redraw_frame = true;
}

void C3DViewOptions::OnRenderNormals() 
{
	render_normals = IsDlgButtonChecked(IDC_RENDER_NORMALS)!=0;
	redraw_frame = true;
}

void C3DViewOptions::OnBackgroundColorButton() 
{
	CColorDialog dlg;
	if(dlg.DoModal()==IDOK)
	{
		unsigned char rgb[3];
		COLORREF color = dlg.GetColor();
		rgb[0] = GetRValue(color);
		rgb[1] = GetGValue(color);
		rgb[2] = GetBValue(color);
		background_color[0] = (float)rgb[0]/255;
		background_color[1] = (float)rgb[1]/255;
		background_color[2] = (float)rgb[2]/255;
		glClearColor(background_color[0],background_color[1],background_color[2],1);
		m_BackgroundColorButton.Set_Color(rgb[0], rgb[1], rgb[2]);
		CWnd *b = GetDlgItem(IDC_BACKGROUND_COLOR_BUTTON);
		b->RedrawWindow();
		redraw_frame = true;
	}
}

void C3DViewOptions::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	mouse_sensitivity = ((float)m_FlySpeedSlider.GetPos())/100;
}

bool Open_3D_View_Options_Dlg()
{
	if(!_3D_View_Options_Dlg)
	{
		_3D_View_Options_Dlg = new C3DViewOptions;
		_3D_View_Options_Dlg->Create(IDD_3D_VIEW_OPTIONS);
		_3D_View_Options_Dlg->SetWindowText("3D View Options");
		Create_ToolSkin_Frame(_3D_View_Options_Dlg, "3D View Options", IDC_3D_VIEW_OPTIONS);
	}
	_3D_View_Options_Dlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}


void C3DViewOptions::OnClose() 
{
	ShowWindow(SW_HIDE);
}

void Set_Fly_Speed_Slider(float pos)
{
	if(_3D_View_Options_Dlg)
	{
		_3D_View_Options_Dlg->m_FlySpeedSlider.SetPos((int)pos);
	}
}

void Check_Render_Borders_Checkbox(bool b)
{
	if(_3D_View_Options_Dlg)
	{
		_3D_View_Options_Dlg->CheckDlgButton(IDC_RENDER_BORDERS, b);
	}
}

void C3DViewOptions::OnRecalcCenter() 
{
	ReCalc_Center();
	Set_View();
}

void C3DViewOptions::OnCenterView() 
{
	Center_View();
}

void C3DViewOptions::OnRenderGrid() 
{
	render_grid = IsDlgButtonChecked(IDC_RENDER_GRID)!=0;
	redraw_frame = true;
}

void C3DViewOptions::OnProjectionSettings() 
{
	Open_Projection_Settings_Dlg();
}

void C3DViewOptions::OnSetSpeckleSkip() 
{
	char text[32];
	GetDlgItemText(IDC_SPECKLE_SKIP, text, 32);
	sscanf(text, "%i", &speckle_skip);
	redraw_frame = true;
}

void C3DViewOptions::OnOrientViewToGrid() 
{
}

HBRUSH C3DViewOptions::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2||id==IDC_STATIC3)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}

	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

