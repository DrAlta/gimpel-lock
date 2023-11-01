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
// ToolsReferenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "ToolsReferenceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ToolsReferenceDlg dialog

ToolsReferenceDlg *toolsReferenceDlg = 0;

ToolsReferenceDlg::ToolsReferenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ToolsReferenceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ToolsReferenceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ToolsReferenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ToolsReferenceDlg)
	DDX_Control(pDX, IDC_HTML_WINDOW, m_HtmlWindow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ToolsReferenceDlg, CDialog)
	//{{AFX_MSG_MAP(ToolsReferenceDlg)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ToolsReferenceDlg message handlers

bool Display_Tools_Reference_Text_File(char *file)
{
	return true;
}

bool Load_File()
{
	toolsReferenceDlg->m_HtmlWindow.Navigate("G3DReadmeFirst.html", NULL, NULL, NULL, NULL);
	return true;
}

bool Resize_Tools_Reference_Layout()
{
	CRect rect;
	toolsReferenceDlg->GetClientRect(rect);
	int side_border = 10;
	int top_area_height = 50;
	toolsReferenceDlg->m_HtmlWindow.SetWindowPos(0, side_border, top_area_height, rect.Width()-(side_border*2), (rect.Height()-top_area_height)-side_border, 0);
	return true;
}

bool Open_Tools_Reference_Dialog()
{
	if(!toolsReferenceDlg)
	{
		toolsReferenceDlg = new ToolsReferenceDlg;
		toolsReferenceDlg->Create(IDD_TOOLS_REFERENCE);
	}
	toolsReferenceDlg->ShowWindow(SW_SHOW);
	Resize_Tools_Reference_Layout();
	Load_File();
	return true;
}

void ToolsReferenceDlg::OnClose() 
{
	toolsReferenceDlg->ShowWindow(SW_HIDE);

}

void ToolsReferenceDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	Resize_Tools_Reference_Layout();
}

HBRUSH ToolsReferenceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}
