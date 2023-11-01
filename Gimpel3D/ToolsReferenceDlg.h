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
#if !defined(AFX_TOOLSREFERENCEDLG_H__B76CB9FF_500D_4CAD_AFB9_4A377127FEE2__INCLUDED_)
#define AFX_TOOLSREFERENCEDLG_H__B76CB9FF_500D_4CAD_AFB9_4A377127FEE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolsReferenceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ToolsReferenceDlg dialog

class ToolsReferenceDlg : public CDialog
{
// Construction
public:
	ToolsReferenceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ToolsReferenceDlg)
	enum { IDD = IDD_TOOLS_REFERENCE };
	CWebBrowser2	m_HtmlWindow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ToolsReferenceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ToolsReferenceDlg)
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLSREFERENCEDLG_H__B76CB9FF_500D_4CAD_AFB9_4A377127FEE2__INCLUDED_)
