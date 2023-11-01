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
// GetTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "GetTextDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GetTextDlg dialog


GetTextDlg::GetTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GetTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(GetTextDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	strcpy(title, "Enter text");
	strcpy(text, "");
	save_text = false;
	skinframe = 0;
}


void GetTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GetTextDlg)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	//}}AFX_DATA_MAP
	SetWindowText(title);
	SetDlgItemText(IDC_EDIT_TEXT, text);

	GetDlgItem(IDOK)->SetFont(skin_font);
	GetDlgItem(IDCANCEL)->SetFont(skin_font);
	GetDlgItem(IDC_EDIT_TEXT)->SetFont(skin_font);
	if(!skinframe)
	{
		skinframe = Create_Skin_Frame(this);
	}
}


BEGIN_MESSAGE_MAP(GetTextDlg, CDialog)
	//{{AFX_MSG_MAP(GetTextDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GetTextDlg message handlers

void GetTextDlg::OnOK() 
{
	save_text = true;
	GetDlgItemText(IDC_EDIT_TEXT, text, 256);
	CDialog::OnOK();
}

HBRUSH GetTextDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void GetTextDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	Remove_Skinframe(skinframe);
}
