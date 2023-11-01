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
// ShapesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "ShapesDlg.h"
#include "Shapes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ShapesDlg dialog

ShapesDlg *shapesDlg = 0;

ShapesDlg::ShapesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ShapesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ShapesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ShapesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ShapesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ShapesDlg, CDialog)
	//{{AFX_MSG_MAP(ShapesDlg)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ShapesDlg message handlers


bool Open_Shapes_Dlg()
{
	if(!shapesDlg)
	{
		shapesDlg = new ShapesDlg;
		shapesDlg->Create(IDD_SHAPES_DLG);
	}
	shapesDlg->ShowWindow(SW_SHOW);
	return true;
}

bool Close_Shapes_Dlg()
{
	shapesDlg->ShowWindow(SW_HIDE);
	return true;
}

void ShapesDlg::OnClose() 
{
	Close_Shapes_Tool();
}

HBRUSH ShapesDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}
