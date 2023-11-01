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
#if !defined(AFX_LISTSELECTDLG_H__EF446723_991B_4204_82BB_D8F4C32999C7__INCLUDED_)
#define AFX_LISTSELECTDLG_H__EF446723_991B_4204_82BB_D8F4C32999C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListSelectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ListSelectDlg dialog
#include "Skin.h"
#include "SkinFrame.h"

#include <vector>

using namespace std;

class LIST_ITEM
{
public:
	LIST_ITEM()
	{
	}
	~LIST_ITEM()
	{
	}
	char text[128];
};

class ListSelectDlg : public CDialog
{
// Construction
public:
	ListSelectDlg(CWnd* pParent = NULL);   // standard constructor
	bool got_selection;
	int selection;
	char title[512];
	vector<LIST_ITEM> list_items;
	void Add_List_Item(char *text);

// Dialog Data
	//{{AFX_DATA(ListSelectDlg)
	enum { IDD = IDD_LIST_SELECT_DLG };
	CSkinButton	m_OK;
	CSkinButton	m_Cancel;
	CListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ListSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ListSelectDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeSelectList();
	afx_msg void OnDblclkSelectList();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTSELECTDLG_H__EF446723_991B_4204_82BB_D8F4C32999C7__INCLUDED_)
