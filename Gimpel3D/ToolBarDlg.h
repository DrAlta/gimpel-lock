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
#if !defined(AFX_TOOLBARDLG_H__839B3EA6_BE54_4322_8B21_F8A3C85E5B0E__INCLUDED_)
#define AFX_TOOLBARDLG_H__839B3EA6_BE54_4322_8B21_F8A3C85E5B0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolBarDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ToolBarDlg dialog

class ToolBarDlg : public CDialog
{
// Construction
public:
	ToolBarDlg(CWnd* pParent = NULL);   // standard constructor
	void SetButtonLayout(int width);

// Dialog Data
	//{{AFX_DATA(ToolBarDlg)
	enum { IDD = IDD_TOOLBAR_DLG };
	CSkinButton	m_WidescreenButton;
	CSkinButton	m_CameraButton;
	CSkinButton	m_GridButton;
	CSkinButton	m_FullscreenApp;
	CSkinButton	m_Anaglyph;
	CSkinButton	m_SplitScreen;
	CSkinButton	m_Fullscreen2D;
	CSkinButton	m_Fullscreen3D;
	CSkinButton	m_Save;
	CSkinButton	m_New;
	CSkinButton	m_Load;
	CSkinButton	m_Import;
	CSkinButton	m_Export;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ToolBarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ToolBarDlg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNew();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnImportData();
	afx_msg void OnExportData();
	afx_msg void OnFullscreen3d();
	afx_msg void OnSplitscreen();
	afx_msg void OnFullscreen2d();
	afx_msg void OnAnaglyph();
	afx_msg void OnFullScreenApp();
	afx_msg void OnGridButton();
	afx_msg void OnCameraButton();
	afx_msg void OnWideScreen3d();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLBARDLG_H__839B3EA6_BE54_4322_8B21_F8A3C85E5B0E__INCLUDED_)
