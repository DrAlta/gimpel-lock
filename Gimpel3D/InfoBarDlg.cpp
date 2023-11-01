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
// InfoBarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "InfoBarDlg.h"

extern int sidebar_bottom_border;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// InfoBarDlg dialog

InfoBarDlg *infoBarDlg = 0;

InfoBarDlg::InfoBarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(InfoBarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(InfoBarDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	infoBarDlg = this;
}


void InfoBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(InfoBarDlg)
	DDX_Control(pDX, IDC_SELECTION_STATUS, m_SelectionStatus);
	DDX_Control(pDX, IDC_STATUS, m_Status);
	//}}AFX_DATA_MAP
	ShowWindow(SW_HIDE);
	SetParent(mainwindow);
	Resize_Info_Bar_Dlg();
	ShowWindow(SW_SHOW);

	GetDlgItem(IDC_STATUS)->SetFont(skin_font);
	GetDlgItem(IDC_SELECTION_STATUS)->SetFont(skin_font);
}

bool Resize_Info_Bar_Dlg()
{
	if(infoBarDlg->GetSafeHwnd()==0)return false;
	CRect rect;
	mainwindow->GetClientRect(&rect);
	float xpos = 5;
	float ypos = (float)(rect.Height()-sidebar_bottom_border);
	infoBarDlg->SetWindowPos(0, (int)xpos, (int)ypos, rect.Width(), 20, 0);
	int status_width = 300;
	infoBarDlg->m_Status.SetWindowPos(0, 0, 0, status_width, 20, 0);
	int border = 1;
	infoBarDlg->m_SelectionStatus.SetWindowPos(0, status_width+border, 0, (rect.Width()-status_width)-border, 20, 0);
	return true;
}


BEGIN_MESSAGE_MAP(InfoBarDlg, CDialog)
	//{{AFX_MSG_MAP(InfoBarDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// InfoBarDlg message handlers

void Print_Status(const char *fmt, ...)
{
	if(!infoBarDlg)return;
	char text[512];
	va_list		ap;
	if (fmt == NULL)return;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);
	infoBarDlg->SetDlgItemText(IDC_STATUS, text);
}

bool Get_Single_Selection_Info(char *name, char *geotype, float *depth, float *rotation);

char selection_status_line[512];
char selection_status_name[512];
char selection_status_geotype[512];
float selection_status_depth;
float selection_status_rotation[2];
bool update_selection_status = false;

bool Update_Selection_Status()
{
	sprintf(selection_status_line, "Layer: \"%s\"      -     Type: \"%s\"     -     Depth: %.4f     -     Orientation: %.4f , %.4f", selection_status_name, selection_status_geotype, selection_status_depth*1000, selection_status_rotation[0], selection_status_rotation[1]);
	infoBarDlg->m_SelectionStatus.SetWindowText(selection_status_line);
	return true;
}

bool Update_Selection_Status_Info()
{
	int n = Num_Selected_Layers();
	if(n>1)
	{
		strcpy(selection_status_line, "Multiple Selection");
		update_selection_status = false;
		infoBarDlg->m_SelectionStatus.SetWindowText(selection_status_line);
	}
	if(n==0)
	{
		strcpy(selection_status_line, "No Selection");
		update_selection_status = false;
		infoBarDlg->m_SelectionStatus.SetWindowText(selection_status_line);
	}
	if(n==1)
	{
		if(Get_Single_Selection_Info(selection_status_name, selection_status_geotype, &selection_status_depth, selection_status_rotation))
		{
			Update_Selection_Status();
			update_selection_status = true;
			return true;
		}
		else
		{
			strcpy(selection_status_line, "");
			update_selection_status = false;
		}
	}
	return true;
}

bool Update_Selection_Status_Depth(float v)
{
	if(update_selection_status)
	{
		selection_status_depth = v;
		Update_Selection_Status();
	}
	return true;
}

bool Update_Selection_Status_Orientation(float x, float y)
{
	if(update_selection_status)
	{
		selection_status_rotation[0] = x;
		selection_status_rotation[1] = y;
		Update_Selection_Status();
	}
	return true;
}

bool Update_Selection_Status_Geometry_Type(char *name)
{
	if(update_selection_status)
	{
		strcpy(selection_status_geotype, name);
		Update_Selection_Status();
	}
	return true;
}


HBRUSH InfoBarDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATUS||id==IDC_SELECTION_STATUS)
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
