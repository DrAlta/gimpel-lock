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
#if !defined(AFX_HTMLVIEWERDLG_H__2816198C_7655_4E63_922B_D70D843EDE81__INCLUDED_)
#define AFX_HTMLVIEWERDLG_H__2816198C_7655_4E63_922B_D70D843EDE81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HtmlViewerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// HtmlViewerDlg dialog

class HtmlViewerDlg : public CDialog
{
// Construction
public:
	HtmlViewerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(HtmlViewerDlg)
	enum { IDD = IDD_HTML_VIEWER_DLG };
	CWebBrowser2	m_HtmlViewControl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HtmlViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(HtmlViewerDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLVIEWERDLG_H__2816198C_7655_4E63_922B_D70D843EDE81__INCLUDED_)
