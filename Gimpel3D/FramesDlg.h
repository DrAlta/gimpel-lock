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
#if !defined(AFX_FRAMESDLG_H__1284628F_99D9_4F1F_9CC4_F1ECA7B7003A__INCLUDED_)
#define AFX_FRAMESDLG_H__1284628F_99D9_4F1F_9CC4_F1ECA7B7003A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FramesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FramesDlg dialog

class FramesDlg : public CDialog
{
// Construction
public:
	FramesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FramesDlg)
	enum { IDD = IDD_FRAMES_DLG };
	CSkinButton	m_HideLists;
	CSkinButton	m_SwitchSides;
	CStatic	m_FramesLabel;
	CListBox	m_ShotList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FramesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FramesDlg)
	afx_msg void OnSelchangeShotList();
	afx_msg void OnSwitchSides();
	afx_msg void OnHideLists();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMESDLG_H__1284628F_99D9_4F1F_9CC4_F1ECA7B7003A__INCLUDED_)
