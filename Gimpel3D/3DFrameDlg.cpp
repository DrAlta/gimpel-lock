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
// 3DFrameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "3DFrameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3DFrameDlg dialog


C3DFrameDlg::C3DFrameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(C3DFrameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(C3DFrameDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_background_brush.CreateSolidBrush(RGB(0,0,0));
}


void C3DFrameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(C3DFrameDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void C3DFrameDlg::Close()
{
	CDialog::OnClose();
}


BEGIN_MESSAGE_MAP(C3DFrameDlg, CDialog)
	//{{AFX_MSG_MAP(C3DFrameDlg)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DFrameDlg message handlers

void C3DFrameDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
}


C3DFrameDlg *frameDlg = 0;

HWND Get_3D_Frame_Window()
{
	if(frameDlg)
	{
		return frameDlg->GetSafeHwnd();
	}
	return 0;
}

bool Init_3D_Frame_Dialog()
{
	frameDlg = new C3DFrameDlg;
	frameDlg->Create(IDD_3D_FRAME_DLG);
	frameDlg->ShowWindow(SW_SHOW);
	SetParent(Get_3D_Window(), frameDlg->GetSafeHwnd());
	int width, height;
	Get_Desktop_Dimensions(&width, &height);
	if(swap_monitors)
	{
		frameDlg->SetWindowPos(0, width, 0, width, height, 0);
	}
	else
	{
		frameDlg->SetWindowPos(0, 0, 0, width, height, 0);
	}
	SetWindowPos(Get_3D_Window(), 0, 0, 0, width, height, 0);
	return true;
}


bool Close_3D_Frame_Dialog()
{
	if(frameDlg)
	{
		SetParent(Get_3D_Window(), mainwindow->GetSafeHwnd());
		frameDlg->Close();
		delete frameDlg;
		frameDlg = 0;
		Fit_3D_Window();
	}
	return true;
}


HBRUSH C3DFrameDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkColor(skin_text_bg_color);	
	return skin_background_brush_color;
	return m_background_brush;
}
