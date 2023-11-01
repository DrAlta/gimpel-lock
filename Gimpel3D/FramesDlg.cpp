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
// FramesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "FramesDlg.h"


bool switch_lists_side = false;
bool hide_lists = false;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FramesDlg dialog

extern int sidebar_top_border;
extern int sidebar_bottom_border;
extern int sidebar_width;
extern int sidebar_restore_width;

extern int tooltray_height;

FramesDlg *framesDlg = 0;

FramesDlg::FramesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FramesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FramesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	framesDlg = this;
}


void FramesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FramesDlg)
	DDX_Control(pDX, IDC_HIDE_LISTS, m_HideLists);
	DDX_Control(pDX, IDC_SWITCH_SIDES, m_SwitchSides);
	DDX_Control(pDX, IDC_FRAMES_LABEL, m_FramesLabel);
	DDX_Control(pDX, IDC_SHOT_LIST, m_ShotList);
	//}}AFX_DATA_MAP

	GetDlgItem(IDC_HIDE_LISTS)->SetFont(skin_font);
	GetDlgItem(IDC_SWITCH_SIDES)->SetFont(skin_font);
	GetDlgItem(IDC_SHOT_LIST)->SetFont(skin_font);
	GetDlgItem(IDC_FRAMES_LABEL)->SetFont(skin_font);
}

bool Resize_Frames_Dlg()
{
	if(framesDlg->GetSafeHwnd()==0)return false;
	CRect rect;
	mainwindow->GetClientRect(&rect);
	int xpos = rect.Width()-sidebar_width;
	int lx = sidebar_top_border+tooltray_height;
	int hx = rect.Height()-sidebar_bottom_border;
	int full_height = hx-lx;
	int height = full_height/2;
	int ypos = sidebar_top_border+tooltray_height;
	int button_width = 37;
	int interior_width = sidebar_width-1;
	framesDlg->SetParent(mainwindow);
	if(switch_lists_side)
	{
		if(hide_lists)
		{
			ypos-=tooltray_height;
			height = (height*2)+tooltray_height;
			framesDlg->SetWindowPos(0, 0, ypos, sidebar_restore_width, height, 0);
			framesDlg->m_HideLists.SetWindowPos(0, 0, 0, sidebar_restore_width-6, height, 0);
			framesDlg->m_HideLists.SetWindowText(">");
		}
		else
		{
			framesDlg->SetWindowPos(0, 0, ypos, sidebar_width, height, 0);
			framesDlg->m_FramesLabel.SetWindowPos(0, button_width, 0, interior_width-(button_width*2), 16, 0);
			framesDlg->m_SwitchSides.SetWindowPos(0, interior_width-button_width, 0, button_width, 16, 0);
			framesDlg->m_HideLists.SetWindowPos(0, 0, 0, button_width, 16, 0);
			framesDlg->m_HideLists.SetWindowText("<<<<<");
		}
	}
	else
	{
		if(hide_lists)
		{
			ypos-=tooltray_height;
			height = (height*2)+tooltray_height;
			xpos = (rect.Width()-sidebar_restore_width)+4;
			framesDlg->SetWindowPos(0, xpos, ypos, sidebar_restore_width, height, 0);
			framesDlg->m_HideLists.SetWindowPos(0, 0, 0, sidebar_restore_width-6, height, 0);
			framesDlg->m_HideLists.SetWindowText("<");
		}
		else
		{
			framesDlg->SetWindowPos(0, xpos, ypos, sidebar_width, height, 0);
			framesDlg->m_FramesLabel.SetWindowPos(0, button_width, 0, interior_width-(button_width*2), 16, 0);
			framesDlg->m_SwitchSides.SetWindowPos(0, 0, 0, button_width, 16, 0);
			framesDlg->m_HideLists.SetWindowPos(0, interior_width-button_width, 0, button_width, 16, 0);
			framesDlg->m_HideLists.SetWindowText(">>>>>");
		}
	}
	int list_xpos = 0;
	int list_ypos = 20;
	int list_height = (height-list_ypos)-6;
	if(hide_lists)
	{
		framesDlg->m_ShotList.SetWindowPos(0, 1000, 1000, 1, 1, 0);
		framesDlg->m_FramesLabel.SetWindowPos(0, 1000, 1000, 1, 1, 0);
		framesDlg->m_SwitchSides.SetWindowPos(0, 1000, 1000, 1, 1, 0);
	}
	else
	{
		framesDlg->m_ShotList.SetWindowPos(0, list_xpos, list_ypos, interior_width, list_height+4, 0);
	}
	return true;
}


BEGIN_MESSAGE_MAP(FramesDlg, CDialog)
	//{{AFX_MSG_MAP(FramesDlg)
	ON_LBN_SELCHANGE(IDC_SHOT_LIST, OnSelchangeShotList)
	ON_BN_CLICKED(IDC_SWITCH_SIDES, OnSwitchSides)
	ON_BN_CLICKED(IDC_HIDE_LISTS, OnHideLists)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FramesDlg message handlers

void FramesDlg::OnSelchangeShotList() 
{
	int id = m_ShotList.GetCurSel();
		if(id!=Get_Current_Project_Frame())
		{
			Set_Project_Frame(id);
			m_ShotList.SetCurSel(id);
		}
}

void Select_Frame_In_List(int frame)
{
	if(framesDlg)
	{
		int id = framesDlg->m_ShotList.GetCurSel();
		if(id!=frame)
		{
			framesDlg->m_ShotList.SetCurSel(frame);
		}
	}
}

bool List_Frames()
{
	if(framesDlg)
	{
		framesDlg->m_ShotList.ResetContent();
		int n = Get_Num_Frames();
		char name[512];
		for(int i = 0;i<n;i++)
		{
			Get_Frame_Name(i, name);
			Get_Display_Name(name);
			framesDlg->m_ShotList.AddString(name);
		}
		framesDlg->m_ShotList.SetCurSel(0);
		framesDlg->m_ShotList.SetSel(0);
	}
	return true;
}

void Resize_Borders()
{
	if(switch_lists_side)
	{
		if(hide_lists)
		{
			window_left_border = sidebar_restore_width-4;
			window_right_border = window_border;
		}
		else
		{
			window_left_border = sidebar_width;
			window_right_border = window_border;
		}
	}
	else
	{
		if(hide_lists)
		{
			window_left_border = window_border;
			window_right_border = sidebar_restore_width-4;
		}
		else
		{
			window_left_border = window_border;
			window_right_border = sidebar_width;
		}
	}
}


void FramesDlg::OnSwitchSides() 
{
	switch_lists_side = !switch_lists_side;
	Resize_Borders();
	Resize_Layout();
}



void FramesDlg::OnHideLists() 
{
	hide_lists = !hide_lists;
	Resize_Borders();
	Resize_Layout();
}


HBRUSH FramesDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_FRAMES_LABEL)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkColor(skin_text_bg_color);	
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
