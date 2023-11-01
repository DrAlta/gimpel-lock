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
#if !defined(AFX_CONTOUREXTRUSIONDLG_H__B3D66188_1DCA_489F_8FEC_C4BF4F027F07__INCLUDED_)
#define AFX_CONTOUREXTRUSIONDLG_H__B3D66188_1DCA_489F_8FEC_C4BF4F027F07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ContourExtrusionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ContourExtrusionDlg dialog

class ContourExtrusionDlg : public CDialog
{
// Construction
public:
	ContourExtrusionDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdateSlider();

// Dialog Data
	//{{AFX_DATA(ContourExtrusionDlg)
	enum { IDD = IDD_CONTOUR_EXTRUSION_DIALOG };
	CSkinButton	m_SetSearchRange;
	CSkinButton	m_SetScale;
	CSkinButton	m_SetRange;
	CSkinButton	m_SetSkip;
	CSkinButton	m_SetAngle;
	CSkinButton	m_ResetToSelection;
	CSkinButton	m_ResetAngle;
	CSkinButton	m_IncreaseRange;
	CSkinButton	m_IncreaseSkip;
	CSkinButton	m_IncreaseAngle;
	CSkinButton	m_DecreaseRange;
	CSkinButton	m_DecreaseSkip;
	CSkinButton	m_DecreaseAngle;
	CSkinButton	m_CenterPuffRange;
	CSkinButton	m_ApplyContour;
	SkinSlider	m_DirectionalExtrusionSlider;
	SkinSlider	m_PuffRangeSlider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ContourExtrusionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ContourExtrusionDlg)
	afx_msg void OnClose();
	afx_msg void OnResetToSelection();
	afx_msg void OnApplyContour();
	afx_msg void OnSphericalExtrusion();
	afx_msg void OnLinearExtrusion();
	afx_msg void OnDirectionalExtrusion();
	afx_msg void OnSetExtrusionAngle();
	afx_msg void OnIncreaseExtrusionAngle();
	afx_msg void OnDecreaseExtrusionAngle();
	afx_msg void OnResetExtrusionAngle();
	afx_msg void OnSetPreviewSkip();
	afx_msg void OnIncreasePreviewSkip();
	afx_msg void OnDecreasePreviewSkip();
	afx_msg void OnSetSearchRange();
	afx_msg void OnIncreaseSearchRange();
	afx_msg void OnDecreaseSearchRange();
	afx_msg void OnSetPuffRange();
	afx_msg void OnSetPuffScale();
	afx_msg void OnCenterPuffRange();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTOUREXTRUSIONDLG_H__B3D66188_1DCA_489F_8FEC_C4BF4F027F07__INCLUDED_)
