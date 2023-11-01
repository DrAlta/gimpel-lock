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
// Gimpel3DDlg.h : header file
//

#if !defined(AFX_GIMPEL3DDLG_H__228E39A4_B701_4619_86DB_1E1BB9BC2EB2__INCLUDED_)
#define AFX_GIMPEL3DDLG_H__228E39A4_B701_4619_86DB_1E1BB9BC2EB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGimpel3DDlg dialog
#include "Skin.h"

class CGimpel3DDlg : public CDialog
{
// Construction
public:
	CGimpel3DDlg(CWnd* pParent = NULL);	// standard constructor
	void Close();

// Dialog Data
	//{{AFX_DATA(CGimpel3DDlg)
	enum { IDD = IDD_GIMPEL3D_DIALOG };
	CStatic	m_SplashImage;
	CSkinButton	m_StartEvaluation;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGimpel3DDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGimpel3DDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStartEvaluation();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedSplash();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GIMPEL3DDLG_H__228E39A4_B701_4619_86DB_1E1BB9BC2EB2__INCLUDED_)
