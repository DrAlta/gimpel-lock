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
#if !defined(AFX_COLORDLG_H__8F0FEDC6_05A4_41EB_9DCF_594E3349D2F9__INCLUDED_)
#define AFX_COLORDLG_H__8F0FEDC6_05A4_41EB_9DCF_594E3349D2F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorDlg.h : header file
//
#include "Skin.h"
/////////////////////////////////////////////////////////////////////////////
// CColorDlg dialog

class CColorDlg : public CDialog
{
// Construction
public:
	CColorDlg(CWnd* pParent = NULL);   // standard constructor
	CWnd *skinframe;
	void UpdateCheckboxes();
	bool save_colors;

// Dialog Data
	//{{AFX_DATA(CColorDlg)
	enum { IDD = IDD_COLOR_DLG };
	CSkinButton	m_ApplyDefaults;
	CSkinButton	m_ApplyColors;
	CListBox	m_ListBox;
	CEdit	m_EditText;
	CButton	m_CheckBox;
	CSkinButton	m_DisabledButton;
	CSkinButton	m_PushedButton;
	CSkinButton	m_NormalButton;
	SkinSlider	m_MixSlider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorDlg)
	afx_msg void OnRed1();
	afx_msg void OnGreen1();
	afx_msg void OnBlue1();
	afx_msg void OnRed2();
	afx_msg void OnGreen2();
	afx_msg void OnBlue2();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnApplyColors();
	afx_msg void OnApplyDefaultColors();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORDLG_H__8F0FEDC6_05A4_41EB_9DCF_594E3349D2F9__INCLUDED_)
