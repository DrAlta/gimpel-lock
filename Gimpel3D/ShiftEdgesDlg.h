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
#if !defined(AFX_SHIFTEDGESDLG_H__C2CC2CBF_21F8_4E1C_857F_969D1CFAFCA5__INCLUDED_)
#define AFX_SHIFTEDGESDLG_H__C2CC2CBF_21F8_4E1C_857F_969D1CFAFCA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShiftEdgesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ShiftEdgesDlg dialog

class ShiftEdgesDlg : public CDialog
{
// Construction
public:
	ShiftEdgesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ShiftEdgesDlg)
	enum { IDD = IDD_SHIFT_EDGES };
	CSkinButton	m_TopUp;
	CSkinButton	m_TopDown;
	CSkinButton	m_RightOut;
	CSkinButton	m_RightIn;
	CSkinButton	m_LeftOut;
	CSkinButton	m_LeftIn;
	CSkinButton	m_BottomUp;
	CSkinButton	m_BottomDown;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ShiftEdgesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ShiftEdgesDlg)
	afx_msg void OnShiftLeftOut();
	afx_msg void OnShiftLeftIn();
	afx_msg void OnShiftRightIn();
	afx_msg void OnShiftRightOut();
	afx_msg void OnClose();
	afx_msg void OnShiftTopDown();
	afx_msg void OnShiftTopUp();
	afx_msg void OnShiftBottomDown();
	afx_msg void OnShiftBottomUp();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHIFTEDGESDLG_H__C2CC2CBF_21F8_4E1C_857F_969D1CFAFCA5__INCLUDED_)
