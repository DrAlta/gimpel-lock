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
// ShiftEdgesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "ShiftEdgesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ShiftEdgesDlg dialog

ShiftEdgesDlg *shiftEdgesDlg = 0;

ShiftEdgesDlg::ShiftEdgesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ShiftEdgesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ShiftEdgesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ShiftEdgesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ShiftEdgesDlg)
	DDX_Control(pDX, IDC_SHIFT_TOP_UP, m_TopUp);
	DDX_Control(pDX, IDC_SHIFT_TOP_DOWN, m_TopDown);
	DDX_Control(pDX, IDC_SHIFT_RIGHT_OUT, m_RightOut);
	DDX_Control(pDX, IDC_SHIFT_RIGHT_IN, m_RightIn);
	DDX_Control(pDX, IDC_SHIFT_LEFT_OUT, m_LeftOut);
	DDX_Control(pDX, IDC_SHIFT_LEFT_IN, m_LeftIn);
	DDX_Control(pDX, IDC_SHIFT_BOTTOM_UP, m_BottomUp);
	DDX_Control(pDX, IDC_SHIFT_BOTTOM_DOWN, m_BottomDown);
	//}}AFX_DATA_MAP

	GetDlgItem(IDC_SHIFT_LEFT_OUT)->SetFont(skin_font);
	GetDlgItem(IDC_SHIFT_LEFT_IN)->SetFont(skin_font);
	GetDlgItem(IDC_SHIFT_RIGHT_IN)->SetFont(skin_font);
	GetDlgItem(IDC_SHIFT_RIGHT_OUT)->SetFont(skin_font);
	GetDlgItem(IDC_SHIFT_TOP_DOWN)->SetFont(skin_font);
	GetDlgItem(IDC_SHIFT_TOP_UP)->SetFont(skin_font);
	GetDlgItem(IDC_SHIFT_BOTTOM_DOWN)->SetFont(skin_font);
	GetDlgItem(IDC_SHIFT_BOTTOM_UP)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC3)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC4)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(ShiftEdgesDlg, CDialog)
	//{{AFX_MSG_MAP(ShiftEdgesDlg)
	ON_BN_CLICKED(IDC_SHIFT_LEFT_OUT, OnShiftLeftOut)
	ON_BN_CLICKED(IDC_SHIFT_LEFT_IN, OnShiftLeftIn)
	ON_BN_CLICKED(IDC_SHIFT_RIGHT_IN, OnShiftRightIn)
	ON_BN_CLICKED(IDC_SHIFT_RIGHT_OUT, OnShiftRightOut)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SHIFT_TOP_DOWN, OnShiftTopDown)
	ON_BN_CLICKED(IDC_SHIFT_TOP_UP, OnShiftTopUp)
	ON_BN_CLICKED(IDC_SHIFT_BOTTOM_DOWN, OnShiftBottomDown)
	ON_BN_CLICKED(IDC_SHIFT_BOTTOM_UP, OnShiftBottomUp)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ShiftEdgesDlg message handlers

void ShiftEdgesDlg::OnShiftLeftOut() 
{
	Shift_Left_Out();
	redraw_frame = true;
	redraw_edit_window = true;
}

void ShiftEdgesDlg::OnShiftLeftIn() 
{
	Shift_Left_In();
	redraw_frame = true;
	redraw_edit_window = true;
}

void ShiftEdgesDlg::OnShiftRightIn() 
{
	Shift_Right_In();
	redraw_frame = true;
	redraw_edit_window = true;
}

void ShiftEdgesDlg::OnShiftRightOut() 
{
	Shift_Right_Out();
	redraw_frame = true;
	redraw_edit_window = true;
}

bool Open_Shift_Edges_Dlg()
{
	if(!shiftEdgesDlg)
	{
		shiftEdgesDlg = new ShiftEdgesDlg;
		shiftEdgesDlg->Create(IDD_SHIFT_EDGES);
		shiftEdgesDlg->SetWindowText("Shift Edges");
		Create_ToolSkin_Frame(shiftEdgesDlg, "Shift Edges", IDC_SHIFT_EDGES);
	}
	shiftEdgesDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

void ShiftEdgesDlg::OnClose() 
{
	ShowWindow(SW_HIDE);
}

void ShiftEdgesDlg::OnShiftTopDown() 
{
	Shift_Top_Down();
	redraw_frame = true;
	redraw_edit_window = true;
}

void ShiftEdgesDlg::OnShiftTopUp() 
{
	Shift_Top_Up();
	redraw_frame = true;
	redraw_edit_window = true;
}

void ShiftEdgesDlg::OnShiftBottomDown() 
{
	Shift_Bottom_Down();
	redraw_frame = true;
	redraw_edit_window = true;
}

void ShiftEdgesDlg::OnShiftBottomUp() 
{
	Shift_Bottom_Up();
	redraw_frame = true;
	redraw_edit_window = true;
}

HBRUSH ShiftEdgesDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2||id==IDC_STATIC3||id==IDC_STATIC4)
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
