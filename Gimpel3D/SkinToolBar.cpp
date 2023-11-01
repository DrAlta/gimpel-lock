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
// SkinToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "SkinToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinToolBar

CSkinToolBar::CSkinToolBar()
{
}

CSkinToolBar::~CSkinToolBar()
{
}


BEGIN_MESSAGE_MAP(CSkinToolBar, CToolBar)
	//{{AFX_MSG_MAP(CSkinToolBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinToolBar message handlers

HBRUSH CSkinToolBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CToolBar::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

BOOL CSkinToolBar::OnEraseBkgnd(CDC* pDC) 
{
	return CToolBar::OnEraseBkgnd(pDC);
}

void CSkinToolBar::OnNcPaint() 
{
}

void CSkinToolBar::OnPaint() 
{
}
