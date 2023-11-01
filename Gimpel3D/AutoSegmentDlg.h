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
#if !defined(AFX_AUTOSEGMENTDLG_H__BE9BE035_0E52_47D0_9751_D403E040283E__INCLUDED_)
#define AFX_AUTOSEGMENTDLG_H__BE9BE035_0E52_47D0_9751_D403E040283E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoSegmentDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AutoSegmentDlg dialog

class AutoSegmentDlg : public CDialog
{
// Construction
public:
	AutoSegmentDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AutoSegmentDlg)
	enum { IDD = IDD_AUTO_SEGMENT_DLG };
	CSkinButton	m_FinalizeSegments;
	CSkinButton	m_ResetPreview;
	CSkinButton	m_PreviewSegments;
	CSkinButton	m_GenerateContrastMap;
	SkinSlider	m_ColorThresholdSlider;
	SkinSlider	m_DarknessThresholdSlider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AutoSegmentDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AutoSegmentDlg)
	afx_msg void OnClose();
	afx_msg void OnOutlinesOnly();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPreviewSegments();
	afx_msg void OnResetPreview();
	afx_msg void OnFinalizeSegments();
	afx_msg void OnGenerateContrastMap();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOSEGMENTDLG_H__BE9BE035_0E52_47D0_9751_D403E040283E__INCLUDED_)
