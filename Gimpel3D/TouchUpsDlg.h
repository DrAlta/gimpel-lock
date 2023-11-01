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
#if !defined(AFX_TOUCHUPSDLG_H__164AFCCD_3B21_4C9F_A988_611303B90B9F__INCLUDED_)
#define AFX_TOUCHUPSDLG_H__164AFCCD_3B21_4C9F_A988_611303B90B9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TouchUpsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TouchUpsDlg dialog

#include "SKin.h"

class TouchUpsDlg : public CDialog
{
// Construction
public:
	TouchUpsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TouchUpsDlg)
	enum { IDD = IDD_TOUCH_UPS_DLG };
	CSkinButton	m_ApplySelected;
	CSkinButton	m_RefreshAndDontApply;
	CSkinButton	m_RefreshAndReApply;
	CSkinButton	m_UndoEdit;
	CListBox	m_TouchUpList;
	CSkinButton	m_RememberAllEdits;
	CSkinButton	m_RedoEdit;
	CSkinButton	m_ClearSelectedEdit;
	CSkinButton	m_ClearAllEdits;
	CSkinButton	m_ApplyStoredEdits;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TouchUpsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TouchUpsDlg)
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClearAllEdits();
	afx_msg void OnClearSelectedEdit();
	afx_msg void OnUndoEdit();
	afx_msg void OnRedoEdit();
	afx_msg void OnRememberAllEdits();
	afx_msg void OnApplyStoredEdits();
	afx_msg void OnSelchangeTouchUpList();
	afx_msg void OnRefreshAndDontApply();
	afx_msg void OnRefreshAndReapply();
	afx_msg void OnApplySelected();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOUCHUPSDLG_H__164AFCCD_3B21_4C9F_A988_611303B90B9F__INCLUDED_)
