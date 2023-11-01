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
#if !defined(AFX_SCALESLIDERDLG_H__69584480_762D_44A7_96CA_FA4D3F078442__INCLUDED_)
#define AFX_SCALESLIDERDLG_H__69584480_762D_44A7_96CA_FA4D3F078442__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScaleSliderDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ScaleSliderDlg dialog

class ScaleSliderDlg : public CDialog
{
// Construction
public:
	ScaleSliderDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdateSliderPosition();

// Dialog Data
	//{{AFX_DATA(ScaleSliderDlg)
	enum { IDD = IDD_SCALE_SLIDER };
	CSkinButton	m_SetSliderRange;
	CSkinButton	m_SetScale;
	CSkinButton	m_IncreaseScale;
	CSkinButton	m_DecreaseScale;
	SkinSlider	m_ScaleSlider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScaleSliderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ScaleSliderDlg)
	afx_msg void OnClose();
	afx_msg void OnSetScale();
	afx_msg void OnIncreaseScale();
	afx_msg void OnDecreaseScale();
	afx_msg void OnSetSliderRange();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCALESLIDERDLG_H__69584480_762D_44A7_96CA_FA4D3F078442__INCLUDED_)
