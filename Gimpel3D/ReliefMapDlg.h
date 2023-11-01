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
#if !defined(AFX_RELIEFMAPDLG_H__692E933D_B081_4AAC_BD88_DB1AB9DA228D__INCLUDED_)
#define AFX_RELIEFMAPDLG_H__692E933D_B081_4AAC_BD88_DB1AB9DA228D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReliefMapDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ReliefMapDlg dialog

#include "ColoredButton.h"

class ReliefMapDlg : public CDialog
{
// Construction
public:
	ReliefMapDlg(CWnd* pParent = NULL);   // standard constructor
	void GetReliefInfo(RELIEF_INFO *ri);
	void SetReliefInfo(RELIEF_INFO *ri);
	void EnableControls();
	void UpdateSliders();

// Dialog Data
	//{{AFX_DATA(ReliefMapDlg)
	enum { IDD = IDD_RELIEF_MAP_DLG };
	CSkinButton	m_SplitLayer;
	CSkinButton	m_SetTilingSliderRange;
	CSkinButton	m_SetTiling;
	CSkinButton	m_SetScaleSliderRange;
	CSkinButton	m_SetScale;
	CSkinButton	m_SetContrastSliderRange;
	CSkinButton	m_SetContrast;
	CSkinButton	m_SetBias;
	CSkinButton	m_ResetBias;
	CSkinButton	m_MFCSelectConvexColor;
	CSkinButton	m_MFCSelectConcaveColor;
	CSkinButton	m_FlipScaleSlider;
	CSkinButton	m_ChangeImage;
	SkinSlider	m_BiasSlider;
	SkinSlider	m_ScaleSlider;
	SkinSlider	m_TilingSlider;
	SkinSlider	m_ContrastSlider;
	CColoredButton	m_ConvexColorButton;
	CColoredButton	m_ConcaveColorButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ReliefMapDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ReliefMapDlg)
	afx_msg void OnClose();
	afx_msg void OnRandomNoise();
	afx_msg void OnReliefImage();
	afx_msg void OnColorDifference();
	afx_msg void OnSetScale();
	afx_msg void OnFlipScaleSliderRange();
	afx_msg void OnSetScaleSliderRange();
	afx_msg void OnPickConcaveColor();
	afx_msg void OnPickConvexColor();
	afx_msg void OnSplitLayer();
	afx_msg void OnSetBias();
	afx_msg void OnResetBias();
	afx_msg void OnBrightnessOnly();
	afx_msg void OnSetTilingSliderRange();
	afx_msg void OnSetContrastSliderRange();
	afx_msg void OnMfcSelectConcaveColor();
	afx_msg void OnMfcSelectConvexColor();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSmoothTransition();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnChangeImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RELIEFMAPDLG_H__692E933D_B081_4AAC_BD88_DB1AB9DA228D__INCLUDED_)
