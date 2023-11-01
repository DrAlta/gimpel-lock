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
#if !defined(AFX_2DVIEWOPTIONS_H__FE3C0DC1_5A31_4ADB_AAE6_E76B1DF1C9E8__INCLUDED_)
#define AFX_2DVIEWOPTIONS_H__FE3C0DC1_5A31_4ADB_AAE6_E76B1DF1C9E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 2DViewOptions.h : header file
//
#include "ColoredButton.h"

#include "SkinCheckBox.h"
/////////////////////////////////////////////////////////////////////////////
// C2DViewOptions dialog

class C2DViewOptions : public CDialog
{
// Construction
public:
	C2DViewOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(C2DViewOptions)
	enum { IDD = IDD_2D_VIEW_OPTIONS };
	CSkinCheckBox	m_RenderLayerOutlines;
	//CButton	m_RenderLayerOutlines;
	CSkinStatic	m_Static1;
	CColoredButton	m_BackgroundColorButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C2DViewOptions)
	public:
	virtual void OnSetFont(CFont* pFont);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(C2DViewOptions)
	afx_msg void OnClose();
	afx_msg void OnRenderLayerOutlines();
	afx_msg void OnRenderLayerMask();
	afx_msg void OnBackgroundColorButton();
	afx_msg void OnViewEdges();
	afx_msg void OnEditCrosshairs();
	afx_msg void OnUseCartoonEdgeDetection();
	afx_msg void OnShowLayerColors();
	afx_msg void OnShowMouseoverInfo();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DVIEWOPTIONS_H__FE3C0DC1_5A31_4ADB_AAE6_E76B1DF1C9E8__INCLUDED_)
