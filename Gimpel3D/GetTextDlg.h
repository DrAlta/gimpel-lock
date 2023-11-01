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
#if !defined(AFX_GETTEXTDLG_H__05AD0F82_9F44_4320_8290_C0CF57EA3A02__INCLUDED_)
#define AFX_GETTEXTDLG_H__05AD0F82_9F44_4320_8290_C0CF57EA3A02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetTextDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// GetTextDlg dialog

#include "Skin.h"

#include "resource.h"

class GetTextDlg : public CDialog
{
// Construction
public:
	GetTextDlg(CWnd* pParent = NULL);   // standard constructor
	char title[256];
	char text[256];
	bool save_text;
	CWnd *skinframe;

// Dialog Data
	//{{AFX_DATA(GetTextDlg)
	enum { IDD = IDD_GETTEXT_DLG };
	CSkinButton	m_OK;
	CSkinButton	m_Cancel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GetTextDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(GetTextDlg)
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETTEXTDLG_H__05AD0F82_9F44_4320_8290_C0CF57EA3A02__INCLUDED_)
