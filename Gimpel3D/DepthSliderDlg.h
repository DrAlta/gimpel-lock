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
#if !defined(AFX_DEPTHSLIDERDLG_H__01C1EE19_B5ED_4842_809D_CE22B1E790E4__INCLUDED_)
#define AFX_DEPTHSLIDERDLG_H__01C1EE19_B5ED_4842_809D_CE22B1E790E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DepthSliderDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DepthSliderDlg dialog

class DepthSliderDlg : public CDialog
{
// Construction
public:
	DepthSliderDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdateSliderPosition();

// Dialog Data
	//{{AFX_DATA(DepthSliderDlg)
	enum { IDD = IDD_DEPTH_SLIDER };
	CSkinButton	m_SetSliderRange;
	CSkinButton	m_SetDepth;
	CSkinButton	m_IncreaseDepth;
	CSkinButton	m_DecreaseDepth;
	SkinSlider	m_DepthSlider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DepthSliderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DepthSliderDlg)
	afx_msg void OnClose();
	afx_msg void OnSetDepth();
	afx_msg void OnIncreaseDepth();
	afx_msg void OnDecreaseDepth();
	afx_msg void OnSetSliderRange();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEPTHSLIDERDLG_H__01C1EE19_B5ED_4842_809D_CE22B1E790E4__INCLUDED_)
