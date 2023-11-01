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
#if !defined(AFX_GAPFILLDLG_H__BB656177_DB17_45C2_B778_EA9C67468231__INCLUDED_)
#define AFX_GAPFILLDLG_H__BB656177_DB17_45C2_B778_EA9C67468231__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GapFillDlg.h : header file
//

#include "ColoredButton.h"

/////////////////////////////////////////////////////////////////////////////
// GapFillDlg dialog

class GapFillDlg : public CDialog
{
// Construction
public:
	GapFillDlg(CWnd* pParent = NULL);   // standard constructor
	int threshold_slider_resolution;
	void ListGaps();
	void ListFrames();
	void EnableFillOptionButtons();
	void EnableBlendImageControls(bool b);

// Dialog Data
	//{{AFX_DATA(GapFillDlg)
	enum { IDD = IDD_GAP_FILL_DLG };
	CSkinButton	m_ExportRightGapMask;
	CSkinButton	m_ExportRightFilledImage;
	CSkinButton	m_ExportLeftGapMask;
	CSkinButton	m_ExportLeftFilledImage;
	CSkinButton	m_ExportFilledStereo;
	CSkinButton	m_ExportFilledAnaglyph;
	CSkinButton	m_RestoreOriginalImage;
	CSkinButton	m_RefreshList;
	CSkinButton	m_PreviewFilledAnaglyph;
	CSkinButton	m_PreviewBlendedGapFill;
	CSkinButton	m_NoFillAll;
	CSkinButton	m_InterpolateAll;
	CSkinButton	m_FinalizeGapPixels;
	CSkinButton	m_FinalizeGapFill;
	CSkinButton	m_DuplicateAll;
	CSkinButton	m_ClearGapInfo;
	CSkinButton	m_ClearBlendPixels;
	CSkinButton	m_BrowseBlendImage;
	CSkinButton	m_BlendImageAll;
	CSkinButton	m_ApplyBlendPixels;
	CSkinButton	m_AlignBlendImage;
	CListBox	m_FramesList;
	CColoredButton	m_BackgroundColorButton;
	CListBox	m_GapList;
	SkinSlider	m_GapThresholdSlider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GapFillDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(GapFillDlg)
	afx_msg void OnInterpolateFill();
	afx_msg void OnDuplicateFill();
	afx_msg void OnBlendFill();
	afx_msg void OnClose();
	afx_msg void OnSelchangeBlendFrameList();
	afx_msg void OnSelchangeGapList();
	afx_msg void OnBrowseBlendImage();
	afx_msg void OnRefreshList();
	afx_msg void OnGapfillSave();
	afx_msg void OnGapfillLeft();
	afx_msg void OnGapfillRight();
	afx_msg void OnGapfillAnaglyph();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBackgroundColorButton();
	afx_msg void OnClearGapInfo();
	afx_msg void OnNoFill();
	afx_msg void OnNoFillAll();
	afx_msg void OnInterpolateAll();
	afx_msg void OnDuplicateAll();
	afx_msg void OnBlendImageAll();
	afx_msg void OnHighlightGaps();
	afx_msg void OnFinalizeGapFill();
	afx_msg void OnRenderGhostBlendImage();
	afx_msg void OnApplyBlendPixels();
	afx_msg void OnAlignBlendImage();
	afx_msg void OnFinalizeGapPixels();
	afx_msg void OnPreviewBlendedGapFill();
	afx_msg void OnRestoreOriginalImage();
	afx_msg void OnShowGaps();
	afx_msg void OnPreviewFilledAnaglyph();
	afx_msg void OnRenderBackgroundBlendImage();
	afx_msg void OnClearBlendPixels();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnExportLeftGapmask();
	afx_msg void OnExportRightGapmask();
	afx_msg void OnExportLeftFilledImage();
	afx_msg void OnExportRightFilledImage();
	afx_msg void OnExportFilledAnaglyph();
	afx_msg void OnExportFilledStereo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAPFILLDLG_H__BB656177_DB17_45C2_B778_EA9C67468231__INCLUDED_)
