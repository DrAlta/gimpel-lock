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
#if !defined(AFX_PERSPECTIVEDLG_H__B6EC4A99_9254_4DCD_ABC8_F5073B5E7F14__INCLUDED_)
#define AFX_PERSPECTIVEDLG_H__B6EC4A99_9254_4DCD_ABC8_F5073B5E7F14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PerspectiveDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PerspectiveDlg dialog

class PerspectiveDlg : public CDialog
{
// Construction
public:
	PerspectiveDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PerspectiveDlg)
	enum { IDD = IDD_PERSPECTIVE_DLG };
	CSkinButton	m_SetDefaultAlignment;
	CSkinButton	m_ModifyGridToWall;
	CSkinButton	m_ModifyGrid;
	CSkinButton	m_ApplyPerspective;
	CSkinButton	m_AlignRight;
	CSkinButton	m_AlignLeft;
	CSkinButton	m_AlignForward;
	CSkinButton	m_AlignBack;
	CSkinButton	m_AlignAll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PerspectiveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PerspectiveDlg)
	afx_msg void OnClose();
	afx_msg void OnParallelLines();
	afx_msg void OnTrapezoid();
	afx_msg void OnAngleAtCorner();
	afx_msg void OnSetPerspectiveFov();
	afx_msg void OnUpdateGrid();
	afx_msg void OnApplyPerspective();
	afx_msg void OnVanishingPoint();
	afx_msg void OnAlignPerspectiveForward();
	afx_msg void OnAlignPerspectiveBack();
	afx_msg void OnAlignPerspectiveLeft();
	afx_msg void OnAlignPerspectiveRight();
	afx_msg void OnAlignPerspectiveAll();
	afx_msg void OnModifyGrid();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnModifyGridToWall();
	afx_msg void OnRectangle();
	afx_msg void OnSetDefaultAlignment();
	afx_msg void OnAdjustableFov();
	afx_msg void OnHorizontalLines();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERSPECTIVEDLG_H__B6EC4A99_9254_4DCD_ABC8_F5073B5E7F14__INCLUDED_)
