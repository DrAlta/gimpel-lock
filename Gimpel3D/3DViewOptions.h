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
#if !defined(AFX_3DVIEWOPTIONS_H__57A3FADF_8481_4407_B5BE_04535FE26506__INCLUDED_)
#define AFX_3DVIEWOPTIONS_H__57A3FADF_8481_4407_B5BE_04535FE26506__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3DViewOptions.h : header file
//
#include "ColoredButton.h"
/////////////////////////////////////////////////////////////////////////////
// C3DViewOptions dialog

class C3DViewOptions : public CDialog
{
// Construction
public:
	C3DViewOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(C3DViewOptions)
	enum { IDD = IDD_3D_VIEW_OPTIONS };
	CSkinButton	m_SetSpeckleSkip;
	CSkinButton	m_ReCalcCenter;
	CSkinButton	m_CenterView;
	CSkinButton	m_ProjectionSettings;
	CColoredButton	m_BackgroundColorButton;
	SkinSlider	m_FlySpeedSlider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DViewOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(C3DViewOptions)
	afx_msg void OnRenderBorders();
	afx_msg void OnCullBackfaces();
	afx_msg void OnRenderNormals();
	afx_msg void OnBackgroundColorButton();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnClose();
	afx_msg void OnRecalcCenter();
	afx_msg void OnCenterView();
	afx_msg void OnRenderGrid();
	afx_msg void OnProjectionSettings();
	afx_msg void OnSetSpeckleSkip();
	afx_msg void OnOrientViewToGrid();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DVIEWOPTIONS_H__57A3FADF_8481_4407_B5BE_04535FE26506__INCLUDED_)