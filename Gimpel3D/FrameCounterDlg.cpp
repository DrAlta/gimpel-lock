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
// FrameCounterDlg.cpp : implementation file
//

#include "stdafx.h"



/*
#include "Skin.h"
#include "Gimpel3D.h"
#include "FrameCounterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FrameCounterDlg dialog


FrameCounterDlg::FrameCounterDlg(CWnd* pParent)
	: CDialog(FrameCounterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FrameCounterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void FrameCounterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FrameCounterDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FrameCounterDlg, CDialog)
	//{{AFX_MSG_MAP(FrameCounterDlg)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FrameCounterDlg message handlers

void FrameCounterDlg::ClearTextItems()
{
	text_items.clear();
}

void FrameCounterDlg::RefreshWindow()
{
	CRect rect;
	GetClientRect(&rect);
	InvalidateRect(&rect);
}

void FrameCounterDlg::Add_Text_Item(char *text, int xpos, int ypos)
{
	FC_TEXT_ITEM ti;
	text_items.push_back(ti);
	FC_TEXT_ITEM *p = &text_items[text_items.size()-1];
	strcpy(p->text, text);
	p->len = strlen(text);
	p->xpos = xpos;
	p->ypos = ypos;
}



void FrameCounterDlg::OnPaint() 
{
	CPaintDC dc(this);
	dc.SelectObject(skin_font);
	int n = text_items.size();
	for(int i = 0;i<n;i++)
	{
		FC_TEXT_ITEM *ti = &text_items[i];
		dc.TextOut(ti->xpos,ti->ypos,ti->text,ti->len);
	}
}

HBRUSH FrameCounterDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkColor(skin_text_bg_color);	
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
*/
