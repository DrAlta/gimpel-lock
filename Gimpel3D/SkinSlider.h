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
#if !defined(AFX_SKINSLIDER_H__5EB2F24D_942A_427E_92C5_A2611CD5BB98__INCLUDED_)
#define AFX_SKINSLIDER_H__5EB2F24D_942A_427E_92C5_A2611CD5BB98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinSlider.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SkinSlider window

class SkinSlider : public CSliderCtrl
{
// Construction
public:
	SkinSlider();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SkinSlider)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetColor( COLORREF cr );
	COLORREF m_crThumb;
	CBrush m_Brush;
	CPen m_Pen;
	virtual ~SkinSlider();

	// Generated message map functions
protected:
	//{{AFX_MSG(SkinSlider)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINSLIDER_H__5EB2F24D_942A_427E_92C5_A2611CD5BB98__INCLUDED_)
