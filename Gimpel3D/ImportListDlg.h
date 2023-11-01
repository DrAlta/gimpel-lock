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
#if !defined(AFX_IMPORTLISTDLG_H__3793DCEB_34CD_494F_82F2_3304456125E5__INCLUDED_)
#define AFX_IMPORTLISTDLG_H__3793DCEB_34CD_494F_82F2_3304456125E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ImportListDlg dialog

#include <vector>
#include "SkinFrame.h"

using namespace std;

class ImportListDlg : public CDialog
{
// Construction
public:
	ImportListDlg(CWnd* pParent = NULL);   // standard constructor
	void ListImportFiles();
	void UpdateListImportFiles();
	vector<char*> *files;
	vector<bool> include_file;
	int num_included_files;
	int first, last;
	void InitListInfo();

// Dialog Data
	//{{AFX_DATA(ImportListDlg)
	enum { IDD = IDD_IMPORT_LIST_DLG };
	CSkinButton	m_SetLast;
	CSkinButton	m_SetFirst;
	CSkinButton	m_SelectNone;
	CSkinButton	m_SelectAll;
	CSkinButton	m_Done;
	CSkinButton	m_Cancel;
	CListBox	m_ImportList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ImportListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ImportListDlg)
	afx_msg void OnSelectAll();
	afx_msg void OnSelectNone();
	afx_msg void OnSetFirst();
	afx_msg void OnSetLast();
	afx_msg void OnDone();
	afx_msg void OnSelchangeImportList();
	afx_msg void OnDblclkImportList();
	afx_msg void OnCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTLISTDLG_H__3793DCEB_34CD_494F_82F2_3304456125E5__INCLUDED_)
