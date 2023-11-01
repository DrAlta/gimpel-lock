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
// PerspectiveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "PerspectiveDlg.h"
#include "PerspectiveTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// PerspectiveDlg dialog

PerspectiveDlg *perspectiveDlg = 0;

PerspectiveDlg::PerspectiveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PerspectiveDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PerspectiveDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void PerspectiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PerspectiveDlg)
	DDX_Control(pDX, IDC_SET_DEFAULT_ALIGNMENT, m_SetDefaultAlignment);
	DDX_Control(pDX, IDC_MODIFY_GRID_TO_WALL, m_ModifyGridToWall);
	DDX_Control(pDX, IDC_MODIFY_GRID, m_ModifyGrid);
	DDX_Control(pDX, IDC_APPLY_PERSPECTIVE, m_ApplyPerspective);
	DDX_Control(pDX, IDC_ALIGN_PERSPECTIVE_RIGHT, m_AlignRight);
	DDX_Control(pDX, IDC_ALIGN_PERSPECTIVE_LEFT, m_AlignLeft);
	DDX_Control(pDX, IDC_ALIGN_PERSPECTIVE_FORWARD, m_AlignForward);
	DDX_Control(pDX, IDC_ALIGN_PERSPECTIVE_BACK, m_AlignBack);
	DDX_Control(pDX, IDC_ALIGN_PERSPECTIVE_ALL, m_AlignAll);
	//}}AFX_DATA_MAP
	CheckDlgButton(IDC_PARALLEL_LINES, perspective_projection_type==pps_PARALLEL);
	CheckDlgButton(IDC_RECTANGLE, perspective_projection_type==pps_RECTANGLE);
	CheckDlgButton(IDC_ANGLE_AT_CORNER, perspective_projection_type==pps_ANGLE);
	char text[512];
	sprintf(text, "%f", perspective_alignment_angle);
	SetDlgItemText(IDC_PERSPECTIVE_ANGLE, text);
	CheckDlgButton(IDC_VANISHING_POINT, use_single_vanishing_point);

	CheckDlgButton(IDC_ADJUSTABLE_FOV, adjustable_fov);
	GetDlgItem(IDC_RECTANGLE)->SetFont(skin_font);

	m_SetDefaultAlignment.SetFont(skin_font);
	GetDlgItem(IDC_ADJUSTABLE_FOV)->SetFont(skin_font);
	GetDlgItem(IDC_PARALLEL_LINES)->SetFont(skin_font);
	GetDlgItem(IDC_TRAPEZOID)->SetFont(skin_font);
	GetDlgItem(IDC_ANGLE_AT_CORNER)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_FOV_EDIT)->SetFont(skin_font);
	GetDlgItem(IDC_SET_PERSPECTIVE_FOV)->SetFont(skin_font);
	GetDlgItem(IDC_UPDATE_GRID)->SetFont(skin_font);
	GetDlgItem(IDC_APPLY_PERSPECTIVE)->SetFont(skin_font);
	GetDlgItem(IDC_PERSPECTIVE_ANGLE)->SetFont(skin_font);
	GetDlgItem(IDC_VANISHING_POINT)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_PERSPECTIVE_FORWARD)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC3)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_PERSPECTIVE_BACK)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC4)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_PERSPECTIVE_LEFT)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC5)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_PERSPECTIVE_RIGHT)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC6)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_PERSPECTIVE_ALL)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC7)->SetFont(skin_font);
	GetDlgItem(IDC_MODIFY_GRID)->SetFont(skin_font);
	GetDlgItem(IDC_MODIFY_GRID_TO_WALL)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(PerspectiveDlg, CDialog)
	//{{AFX_MSG_MAP(PerspectiveDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_PARALLEL_LINES, OnParallelLines)
	ON_BN_CLICKED(IDC_TRAPEZOID, OnTrapezoid)
	ON_BN_CLICKED(IDC_ANGLE_AT_CORNER, OnAngleAtCorner)
	ON_BN_CLICKED(IDC_SET_PERSPECTIVE_FOV, OnSetPerspectiveFov)
	ON_BN_CLICKED(IDC_UPDATE_GRID, OnUpdateGrid)
	ON_BN_CLICKED(IDC_APPLY_PERSPECTIVE, OnApplyPerspective)
	ON_BN_CLICKED(IDC_VANISHING_POINT, OnVanishingPoint)
	ON_BN_CLICKED(IDC_ALIGN_PERSPECTIVE_FORWARD, OnAlignPerspectiveForward)
	ON_BN_CLICKED(IDC_ALIGN_PERSPECTIVE_BACK, OnAlignPerspectiveBack)
	ON_BN_CLICKED(IDC_ALIGN_PERSPECTIVE_LEFT, OnAlignPerspectiveLeft)
	ON_BN_CLICKED(IDC_ALIGN_PERSPECTIVE_RIGHT, OnAlignPerspectiveRight)
	ON_BN_CLICKED(IDC_ALIGN_PERSPECTIVE_ALL, OnAlignPerspectiveAll)
	ON_BN_CLICKED(IDC_MODIFY_GRID, OnModifyGrid)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_MODIFY_GRID_TO_WALL, OnModifyGridToWall)
	ON_BN_CLICKED(IDC_RECTANGLE, OnRectangle)
	ON_BN_CLICKED(IDC_SET_DEFAULT_ALIGNMENT, OnSetDefaultAlignment)
	ON_BN_CLICKED(IDC_ADJUSTABLE_FOV, OnAdjustableFov)
	ON_BN_CLICKED(IDC_HORIZONTAL_LINES, OnHorizontalLines)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PerspectiveDlg message handlers

bool Open_Perspective_Dlg()
{
	if(!perspectiveDlg)
	{
		perspectiveDlg = new PerspectiveDlg;
		perspectiveDlg->Create(IDD_PERSPECTIVE_DLG);
		perspectiveDlg->SetWindowText("Perspective Projection");
		Create_ToolSkin_Frame(perspectiveDlg, "Perspective Projection", IDC_GET_PERSPECTIVE_PROJECTION);
	}
	perspectiveDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_Perspective_Dlg()
{
	perspectiveDlg->ShowWindow(SW_HIDE);
	return true;
}

void PerspectiveDlg::OnClose() 
{
	Close_Perspective_Tool();
}

void PerspectiveDlg::OnSetPerspectiveFov() 
{
}

void PerspectiveDlg::OnUpdateGrid() 
{
}

void PerspectiveDlg::OnApplyPerspective() 
{
}

void PerspectiveDlg::OnVanishingPoint() 
{
	use_single_vanishing_point = IsDlgButtonChecked(IDC_VANISHING_POINT)!=0;
	redraw_edit_window = true;
}

void PerspectiveDlg::OnAlignPerspectiveForward() 
{
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("No selection, nothing to align.");
		return;
	}
	char text[512];
	GetDlgItemText(IDC_PERSPECTIVE_ANGLE, text, 512);
	sscanf(text, "%f", &perspective_alignment_angle);
	Align_Perspective_Forward();
}

void PerspectiveDlg::OnAlignPerspectiveBack() 
{
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("No selection, nothing to align.");
		return;
	}
	char text[512];
	GetDlgItemText(IDC_PERSPECTIVE_ANGLE, text, 512);
	sscanf(text, "%f", &perspective_alignment_angle);
	Align_Perspective_Back();
}

void PerspectiveDlg::OnAlignPerspectiveLeft() 
{
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("No selection, nothing to align.");
		return;
	}
	char text[512];
	GetDlgItemText(IDC_PERSPECTIVE_ANGLE, text, 512);
	sscanf(text, "%f", &perspective_alignment_angle);
	Align_Perspective_Left();
}

void PerspectiveDlg::OnAlignPerspectiveRight() 
{
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("No selection, nothing to align.");
		return;
	}
	char text[512];
	GetDlgItemText(IDC_PERSPECTIVE_ANGLE, text, 512);
	sscanf(text, "%f", &perspective_alignment_angle);
	Align_Perspective_Right();
}

void PerspectiveDlg::OnAlignPerspectiveAll() 
{
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("No selection, nothing to align.");
		return;
	}
	char text[512];
	GetDlgItemText(IDC_PERSPECTIVE_ANGLE, text, 512);
	sscanf(text, "%f", &perspective_alignment_angle);
	Align_Perspective_All();
}

HBRUSH PerspectiveDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2||id==IDC_STATIC3||id==IDC_STATIC4||id==IDC_STATIC5||id==IDC_STATIC6||id==IDC_STATIC7)
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
	return hbr;
}


void PerspectiveDlg::OnRectangle() 
{
	perspective_projection_type = pps_RECTANGLE;
	redraw_edit_window = true;
}

void PerspectiveDlg::OnParallelLines() 
{
	perspective_projection_type = pps_PARALLEL;
	redraw_edit_window = true;
}

void PerspectiveDlg::OnTrapezoid() 
{
}

void PerspectiveDlg::OnAngleAtCorner() 
{
	perspective_projection_type = pps_ANGLE;
	redraw_edit_window = true;
}

void PerspectiveDlg::OnModifyGrid() 
{
	Set_Grid_To_Perspective_Floor();
}

void PerspectiveDlg::OnModifyGridToWall() 
{
	Set_Grid_To_Perspective_Wall();
}

void PerspectiveDlg::OnSetDefaultAlignment() 
{
	Set_Grid_To_Perspective_Origin();
}

void PerspectiveDlg::OnAdjustableFov() 
{
	adjustable_fov = IsDlgButtonChecked(IDC_ADJUSTABLE_FOV)!=0;
}

void PerspectiveDlg::OnHorizontalLines() 
{
	horizontal_parallel_lines = IsDlgButtonChecked(IDC_HORIZONTAL_LINES)!=0;
}
