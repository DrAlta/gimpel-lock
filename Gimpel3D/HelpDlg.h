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
//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES
#if !defined(AFX_HELPDLG_H__B0208813_7F1D_4588_8F1A_3DAD5A7C1CC0__INCLUDED_)
#define AFX_HELPDLG_H__B0208813_7F1D_4588_8F1A_3DAD5A7C1CC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HelpDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// HelpDlg dialog

class HelpDlg : public CDialog
{
// Construction
public:
	HelpDlg(CWnd* pParent = NULL);   // standard constructor

	void ResizeLayout();
	void SetHelpView(HTREEITEM ti);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Dialog Data
	//{{AFX_DATA(HelpDlg)
	enum { IDD = IDD_HELP_DLG };
	CSkinButton	m_Forward;
	CSkinButton	m_Back;
	CTreeCtrl	m_HelpToc;
	CWebBrowser2	m_HtmlWindow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HelpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(HelpDlg)
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangedHelpToc(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTitleChangeHtmlWindow(LPCTSTR Text);
	afx_msg void OnClickHelpToc(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBack();
	afx_msg void OnForward();
	afx_msg void OnCommandStateChangeHtmlWindow(long Command, BOOL Enable);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBeforeNavigate2HtmlWindow(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HELPDLG_H__B0208813_7F1D_4588_8F1A_3DAD5A7C1CC0__INCLUDED_)
