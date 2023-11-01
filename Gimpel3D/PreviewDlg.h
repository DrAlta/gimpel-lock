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
#if !defined(AFX_PREVIEWDLG_H__21C18D0C_189D_4D65_B068_89A7D4A65FB0__INCLUDED_)
#define AFX_PREVIEWDLG_H__21C18D0C_189D_4D65_B068_89A7D4A65FB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreviewDlg.h : header file
//

#include "Skin.h"

/////////////////////////////////////////////////////////////////////////////
// PreviewDlg dialog

class PreviewDlg : public CDialog
{
// Construction
public:
	PreviewDlg(CWnd* pParent = NULL);   // standard constructor
	void RecalcSpace();

// Dialog Data
	//{{AFX_DATA(PreviewDlg)
	enum { IDD = IDD_PREVIEW_DLG };
	CSkinButton	m_RecalcSpace;
	CSkinButton	m_StepForward3;
	CSkinButton	m_StepForward2;
	CSkinButton	m_StepBack3;
	CSkinButton	m_StepBack2;
	CButton	m_DeleteFrameData;
	CEdit	m_CurrentFrame;
	CStatic	m_Static4;
	CStatic	m_Static3;
	CStatic	m_Static2;
	CStatic	m_Static1;
	CSkinButton	m_StepForward;
	CSkinButton	m_StepBack;
	CEdit	m_StartFrame;
	CSkinButton	m_RestrictRange;
	CSkinButton	m_PlayStop;
	CSkinButton	m_Loop;
	CEdit	m_EndFrame;
	CEdit	m_DiskSpace;
	CSkinButton	m_PreCacheCurrentFrame;
	CSkinButton	m_PreCacheAllFrames;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PreviewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PreviewDlg)
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPrecacheAllFrames();
	afx_msg void OnPrecacheCurrentFrames();
	afx_msg void OnPlayStop();
	afx_msg void OnPause();
	afx_msg void OnStepBack();
	afx_msg void OnStepForward();
	afx_msg void OnLoop();
	afx_msg void OnDeleteFrameData();
	afx_msg void OnStepBack2();
	afx_msg void OnStepForward2();
	afx_msg void OnStepBack3();
	afx_msg void OnStepForward3();
	afx_msg void OnRestrictRange();
	afx_msg void OnRecalcSpace();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWDLG_H__21C18D0C_189D_4D65_B068_89A7D4A65FB0__INCLUDED_)
