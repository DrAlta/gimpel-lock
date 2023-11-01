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
// SkinSlider.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "Skin.h"
#include "SkinSlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SkinSlider

SkinSlider::SkinSlider()
{
}

SkinSlider::~SkinSlider()
{
}


BEGIN_MESSAGE_MAP(SkinSlider, CSliderCtrl)
	//{{AFX_MSG_MAP(SkinSlider)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SkinSlider message handlers

void SkinSlider::SetColor(COLORREF cr)
{
	m_crThumb = cr;

	m_Brush.CreateSolidBrush( cr );
	m_Pen.CreatePen( PS_SOLID, 1, RGB(128,128,128) );	// dark gray
}

afx_msg void SkinSlider::OnCustomDraw ( NMHDR * pNotifyStruct, LRESULT* result )
{
	// for additional info, read beginning MSDN "Customizing a Control's Appearance Using Custom Draw" at
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/custdraw/custdraw.asp

	NMCUSTOMDRAW nmcd = *(LPNMCUSTOMDRAW)pNotifyStruct;

	if ( nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*result = CDRF_NOTIFYITEMDRAW ;		// we will now get subsequent CDDS_ITEMPREPAINT notifications
		return;
	}
	else if(nmcd.dwDrawStage==CDDS_ITEMPREPAINT)
	{
		if(nmcd.dwItemSpec==TBCD_THUMB)
		{
			CDC* pDC = CDC::FromHandle( nmcd.hdc );
			pDC->SelectObject(skin_slider_brush);
			pDC->SelectObject(skin_slider_pen);
			pDC->Ellipse( &(nmcd.rc) );
			pDC->Detach();
		}
		else if(nmcd.dwItemSpec==TBCD_CHANNEL)
		{
			CDC* pDC = CDC::FromHandle( nmcd.hdc );
			DrawSkinControlEdge(pDC, &nmcd.rc, false);
			pDC->Detach();
		}
		*result = CDRF_SKIPDEFAULT;
	} 



}

void DrawSkinnedSlider(NMHDR *pNotifyStruct, LRESULT* result)
{
	NMCUSTOMDRAW nmcd = *(LPNMCUSTOMDRAW)pNotifyStruct;

	if ( nmcd.dwDrawStage == CDDS_PREPAINT )
	{
		*result = CDRF_NOTIFYITEMDRAW ;		// we will now get subsequent CDDS_ITEMPREPAINT notifications
		return;
	}
	else if(nmcd.dwDrawStage==CDDS_ITEMPREPAINT)
	{
		if(nmcd.dwItemSpec==TBCD_THUMB)
		{
			CDC* pDC = CDC::FromHandle( nmcd.hdc );
			pDC->SelectObject(skin_slider_brush);
			pDC->SelectObject(skin_slider_pen);
			pDC->Ellipse( &(nmcd.rc) );
			pDC->Detach();
		}
		else if(nmcd.dwItemSpec==TBCD_CHANNEL)
		{
			CDC* pDC = CDC::FromHandle( nmcd.hdc );
			DrawSkinControlEdge(pDC, &nmcd.rc, false);
			pDC->Detach();
		}
		*result = CDRF_SKIPDEFAULT;
	} 

}
