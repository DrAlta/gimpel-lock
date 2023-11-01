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
#if !defined(AFX_SKINMESSAGEBOX_H__B5FEC627_9218_4735_B52D_6F56ACFF4758__INCLUDED_)
#define AFX_SKINMESSAGEBOX_H__B5FEC627_9218_4735_B52D_6F56ACFF4758__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinMessageBox.h : header file
//
#include "Skin.h"
#include "SkinFrame.h"

/////////////////////////////////////////////////////////////////////////////
// SkinMessageBox dialog

enum
{
	smb_0,
	smb_1,
	smb_2,
	smb_3,
	smb_4,
	smb_5,
	smb_YES,
	smb_NO,
	smb_CANCEL,
	smb_OK
};

class SkinMessageBox : public CDialog
{
// Construction
public:
	SkinMessageBox(CWnd* pParent = NULL);   // standard constructor
	char title[256];
	char text[256];
	//SkinFrame *skinframe;
	int return_type;
	int type;
	void Close();
	vector<char*> text_items;
	int dlg_width;
	int dlg_height;
	int text_area_width;
	int text_area_height;

// Dialog Data
	//{{AFX_DATA(SkinMessageBox)
	enum { IDD = IDD_SKIN_MESSAGEBOX };
	CSkinButton	m_Yes;
	CSkinButton	m_OK;
	CSkinButton	m_No;
	CSkinButton	m_Cancel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SkinMessageBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SkinMessageBox)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	afx_msg void OnOk();
	afx_msg void OnCancel();
	afx_msg void OnYes();
	afx_msg void OnNo();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINMESSAGEBOX_H__B5FEC627_9218_4735_B52D_6F56ACFF4758__INCLUDED_)
