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
// ImportListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "ImportListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ImportListDlg dialog

SkinFrame *ils_skinframe = 0;

ImportListDlg::ImportListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ImportListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ImportListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	num_included_files = 0;
	first = last = -1;
}


void ImportListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ImportListDlg)
	DDX_Control(pDX, IDC_SET_LAST, m_SetLast);
	DDX_Control(pDX, IDC_SET_FIRST, m_SetFirst);
	DDX_Control(pDX, IDC_SELECT_NONE, m_SelectNone);
	DDX_Control(pDX, IDC_SELECT_ALL, m_SelectAll);
	DDX_Control(pDX, IDC_DONE, m_Done);
	DDX_Control(pDX, IDC_CANCEL, m_Cancel);
	DDX_Control(pDX, IDC_IMPORT_LIST, m_ImportList);
	//}}AFX_DATA_MAP
	ListImportFiles();

	GetDlgItem(IDC_IMPORT_LIST)->SetFont(skin_font);
	GetDlgItem(IDC_SELECT_ALL)->SetFont(skin_font);
	GetDlgItem(IDC_SET_FIRST)->SetFont(skin_font);
	GetDlgItem(IDC_SET_LAST)->SetFont(skin_font);
	GetDlgItem(IDC_DONE)->SetFont(skin_font);
	GetDlgItem(IDC_SELECT_NONE)->SetFont(skin_font);
	GetDlgItem(IDC_CANCEL)->SetFont(skin_font);
	SetWindowText("Select Import Files");

	if(!ils_skinframe)
	{
		ils_skinframe = (SkinFrame*)Create_Skin_Frame(this);
		ils_skinframe->m_CloseButton.ShowWindow(SW_HIDE);
		ils_skinframe->ShowWindow(SW_SHOW);
	}
}


BEGIN_MESSAGE_MAP(ImportListDlg, CDialog)
	//{{AFX_MSG_MAP(ImportListDlg)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_SELECT_NONE, OnSelectNone)
	ON_BN_CLICKED(IDC_SET_FIRST, OnSetFirst)
	ON_BN_CLICKED(IDC_SET_LAST, OnSetLast)
	ON_BN_CLICKED(IDC_DONE, OnDone)
	ON_LBN_SELCHANGE(IDC_IMPORT_LIST, OnSelchangeImportList)
	ON_LBN_DBLCLK(IDC_IMPORT_LIST, OnDblclkImportList)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ImportListDlg message handlers

void ImportListDlg::ListImportFiles()
{
	m_ImportList.ResetContent();
	int n = files->size();
	char text[512];
	for(int i = 0;i<n;i++)
	{
		if(include_file[i])
		{
			sprintf(text, "X - %s", (*files)[i]);
			if(first==-1)
			{
				first = i;
			}
			last = i;
		}
		else
		{
			sprintf(text, "  - %s", (*files)[i]);
		}
		m_ImportList.AddString(text);
	}
}

void ImportListDlg::UpdateListImportFiles()
{
	int sel = m_ImportList.GetCurSel();
	int ti = m_ImportList.GetTopIndex();
	m_ImportList.ResetContent();
	int n = files->size();
	char text[512];
	for(int i = 0;i<n;i++)
	{
		if(include_file[i])
		{
			sprintf(text, "X - %s", (*files)[i]);
		}
		else
		{
			sprintf(text, "  - %s", (*files)[i]);
		}
		m_ImportList.AddString(text);
	}
	m_ImportList.SetTopIndex(ti);
	m_ImportList.SetCurSel(sel);
}

void ImportListDlg::InitListInfo()
{
	int n = files->size();
	for(int i = 0;i<n;i++)
	{
		include_file.push_back(true);
	}
}


void ImportListDlg::OnSelectAll() 
{
	int n = files->size();
	for(int i = 0;i<n;i++)
	{
		include_file[i] = true;
	}
	first = 0;
	last = n-1;
	UpdateListImportFiles();
}

void ImportListDlg::OnSelectNone() 
{
	int n = files->size();
	for(int i = 0;i<n;i++)
	{
		include_file[i] = false;
	}
	first = -1;
	last = -1;
	UpdateListImportFiles();
}

void ImportListDlg::OnSetFirst() 
{
	int sel = m_ImportList.GetCurSel();
	if(sel>-1)
	{
		int i;
		for(i = 0;i<sel;i++)
		{
			include_file[i] = false;
		}
		first = sel;
		for(i = first;i<last+1;i++)
		{
			include_file[i] = true;
		}
		if(first>=last)
		{
			last = first;
			include_file[first] = true;
		}
	}
	UpdateListImportFiles();
}

void ImportListDlg::OnSetLast() 
{
	int sel = m_ImportList.GetCurSel();
	if(sel>-1)
	{
		int n = files->size();
		int i;
		for(i = sel+1;i<n;i++)
		{
			include_file[i] = false;
		}
		last = sel;
		for(i = first;i<last+1;i++)
		{
			include_file[i] = true;
		}
		if(last<=first)
		{
			first = last;
			include_file[last] = true;
		}
	}
	UpdateListImportFiles();
}

void ImportListDlg::OnSelchangeImportList() 
{
}

void ImportListDlg::OnDblclkImportList() 
{
	int sel = m_ImportList.GetCurSel();
	if(sel>-1)
	{
		include_file[sel] = !include_file[sel];
		UpdateListImportFiles();
	}
}

void ImportListDlg::OnDone() 
{
	num_included_files = 0;
	int n = files->size();
	vector<char*> tfiles;
	int i;
	for(i = 0;i<n;i++)
	{
		if(include_file[i])
		{
			tfiles.push_back((*files)[i]);
		}
		else
		{
			delete[] (*files)[i];
		}
	}
	files->clear();
	include_file.clear();
	num_included_files = tfiles.size();
	for(i = 0;i<num_included_files;i++)
	{
		files->push_back(tfiles[i]);
	}
	tfiles.clear();
	CDialog::OnOK();
}

void ImportListDlg::OnCancel() 
{
	num_included_files = 0;
	int n = files->size();
	for(int i = 0;i<n;i++)
	{
		delete[] (*files)[i];
	}
	files->clear();
	CDialog::OnOK();
}



HBRUSH ImportListDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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

void ImportListDlg::PostNcDestroy() 
{
	if(ils_skinframe)
	{
		ils_skinframe->child_window = 0;
		ils_skinframe->ShowWindow(SW_HIDE);
		ils_skinframe->SetParent(0);
		Remove_Skinframe(ils_skinframe);
		ils_skinframe = 0;
	}
	CDialog::PostNcDestroy();
}
