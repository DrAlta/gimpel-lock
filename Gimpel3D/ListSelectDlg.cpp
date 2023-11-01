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
// ListSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "ListSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ListSelectDlg dialog

SkinFrame *ls_skinframe = 0;

ListSelectDlg::ListSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ListSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ListSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	got_selection = false;
	selection = -1;
	strcpy(title,"Select Item");
}


void ListSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ListSelectDlg)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_SELECT_LIST, m_List);
	//}}AFX_DATA_MAP
	int n = list_items.size();
	for(int i = 0;i<n;i++)
	{
		m_List.AddString(list_items[i].text);
	}
	list_items.clear();

   	GetDlgItem(IDOK)->SetFont(skin_font);
    GetDlgItem(IDCANCEL)->SetFont(skin_font);
	GetDlgItem(IDC_SELECT_LIST)->SetFont(skin_font);

	SetWindowText(title);

	if(!ls_skinframe)
	{
		ls_skinframe = (SkinFrame*)Create_Skin_Frame(this);
		ls_skinframe->m_CloseButton.ShowWindow(SW_HIDE);
		ls_skinframe->ShowWindow(SW_SHOW);
	}

}


BEGIN_MESSAGE_MAP(ListSelectDlg, CDialog)
	//{{AFX_MSG_MAP(ListSelectDlg)
	ON_LBN_SELCHANGE(IDC_SELECT_LIST, OnSelchangeSelectList)
	ON_LBN_DBLCLK(IDC_SELECT_LIST, OnDblclkSelectList)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ListSelectDlg message handlers

void ListSelectDlg::OnOK() 
{
	got_selection = true;
	selection = m_List.GetCurSel();
	CDialog::OnOK();
}

void ListSelectDlg::OnCancel() 
{
	CDialog::OnCancel();
}

void ListSelectDlg::OnSelchangeSelectList() 
{
}

void ListSelectDlg::Add_List_Item(char *text)
{
	LIST_ITEM li;
	strcpy(li.text, text);
	list_items.push_back(li);
}


void ListSelectDlg::OnDblclkSelectList() 
{
	OnOK();
}

HBRUSH ListSelectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void ListSelectDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	if(ls_skinframe)
	{
		ls_skinframe->child_window = 0;
		Remove_Skinframe(ls_skinframe);
		ls_skinframe = 0;
	}
}


int List_Select_Dialog(vector<char*> *items, char *title)
{
	ListSelectDlg dlg;
	int n = items->size();
	for(int i = 0;i<n;i++)
	{
		dlg.Add_List_Item((*items)[i]);
	}
	strcpy(dlg.title, title);
	dlg.DoModal();
	if(dlg.got_selection)
	{
		return dlg.selection;
	}
	return -1;
}
