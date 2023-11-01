/*  Gimpel3D 2D/3D Stereo Converter
    Copyright (C) 2008-2011  Daniel Ren� Dever (Gimpel)

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
#if !defined(AFX_SKINTOOLBAR_H__A5679D50_0DEE_47BD_A23B_72D4A45124B6__INCLUDED_)
#define AFX_SKINTOOLBAR_H__A5679D50_0DEE_47BD_A23B_72D4A45124B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinToolBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSkinToolBar window

class CSkinToolBar : public CToolBar
{
// Construction
public:
	CSkinToolBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkinToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinToolBar)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINTOOLBAR_H__A5679D50_0DEE_47BD_A23B_72D4A45124B6__INCLUDED_)
