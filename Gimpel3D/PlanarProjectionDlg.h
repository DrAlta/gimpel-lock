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
#if !defined(AFX_PLANARPROJECTIONDLG_H__3E50E5B3_EC82_4144_A8D2_F97B18690A69__INCLUDED_)
#define AFX_PLANARPROJECTIONDLG_H__3E50E5B3_EC82_4144_A8D2_F97B18690A69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanarProjectionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PlanarProjectionDlg dialog

class PlanarProjectionDlg : public CDialog
{
// Construction
public:
	PlanarProjectionDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdateSliderPositions();
	void UpdateSliders();
	void CenterSliders();
	bool is_visible;
	float saved_horizontal_rotation;
	float saved_vertical_rotation;
	float saved_z_rotation;

// Dialog Data
	//{{AFX_DATA(PlanarProjectionDlg)
	enum { IDD = IDD_PLANAR_PROJECTION_DLG };
	CSkinButton	m_CenterZRotation;
	CSkinButton	m_SetZRotation;
	SkinSlider	m_ZRotationSlider;
	CStatic	m_SavedOrientation;
	CSkinButton	m_ApplySavedOrientation;
	CSkinButton	m_SaveCurrentOrientation;
	CSkinButton	m_CenterVerticalRotation;
	CSkinButton	m_CenterHorizontalRotation;
	CSkinButton	m_SetVerticalRotation;
	CSkinButton	m_SetHorizontalRotation;
	SkinSlider	m_VerticalRotationSlider;
	SkinSlider	m_HorizontalRotationSlider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PlanarProjectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PlanarProjectionDlg)
	afx_msg void OnClose();
	afx_msg void OnSetHorizontalRotation();
	afx_msg void OnCenterHorizontalRotation();
	afx_msg void OnSetVerticalRotation();
	afx_msg void OnCenterVerticalRotation();
	afx_msg void OnLockTop();
	afx_msg void OnLockBottom();
	afx_msg void OnLockLeft();
	afx_msg void OnLockRight();
	afx_msg void OnResetToSelection();
	afx_msg void OnApplyToSelection();
	afx_msg void OnSetHorizontalPosition();
	afx_msg void OnCenterHorizontalPosition();
	afx_msg void OnSetVerticalPosition();
	afx_msg void OnCenterVerticalPosition();
	afx_msg void OnSetDepthPosition();
	afx_msg void OnCenterDepthPosition();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLockCenter();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSaveCurrentOrientation();
	afx_msg void OnApplySavedOrientation();
	afx_msg void OnSetZRotation();
	afx_msg void OnCenterZRotation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANARPROJECTIONDLG_H__3E50E5B3_EC82_4144_A8D2_F97B18690A69__INCLUDED_)
