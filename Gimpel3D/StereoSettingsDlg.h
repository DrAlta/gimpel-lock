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
#if !defined(AFX_STEREOSETTINGSDLG_H__2E30F2F2_35AC_4329_9392_4176BD6A28D1__INCLUDED_)
#define AFX_STEREOSETTINGSDLG_H__2E30F2F2_35AC_4329_9392_4176BD6A28D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StereoSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// StereoSettingsDlg dialog

class StereoSettingsDlg : public CDialog
{
// Construction
public:
	StereoSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdateSliders();
	void UpdateSliderPositions();
	SkinSlider	m_EyeSeparationSlider;
	SkinSlider	m_FocalLengthSlider;
	SkinSlider	m_DepthScaleSlider;

// Dialog Data
	//{{AFX_DATA(StereoSettingsDlg)
	enum { IDD = IDD_STEREO_SETTINGS_DLG };
	CSkinButton	m_ApplyToAll;
	CSkinButton	m_ResetStereo;
	CSkinButton	m_ResetDepthScale;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StereoSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(StereoSettingsDlg)
	afx_msg void OnClose();
	afx_msg void OnResetDepthScale();
	afx_msg void OnKeyframeStereo();
	afx_msg void OnApplyStereoSettingsToAll();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnResetStereo();
	afx_msg void OnRenderFocalPlane();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STEREOSETTINGSDLG_H__2E30F2F2_35AC_4329_9392_4176BD6A28D1__INCLUDED_)
