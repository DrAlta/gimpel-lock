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
// TouchUpsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "TouchUpsDlg.h"
#include "TouchUpTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TouchUpsDlg dialog

TouchUpsDlg *touchUpsDlg = 0;

TouchUpsDlg::TouchUpsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TouchUpsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TouchUpsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void TouchUpsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TouchUpsDlg)
	DDX_Control(pDX, IDC_APPLY_SELECTED, m_ApplySelected);
	DDX_Control(pDX, IDC_REFRESH_AND_DONT_APPLY, m_RefreshAndDontApply);
	DDX_Control(pDX, IDC_REFRESH_AND_REAPPLY, m_RefreshAndReApply);
	DDX_Control(pDX, IDC_UNDO_EDIT, m_UndoEdit);
	DDX_Control(pDX, IDC_TOUCH_UP_LIST, m_TouchUpList);
	DDX_Control(pDX, IDC_REMEMBER_ALL_EDITS, m_RememberAllEdits);
	DDX_Control(pDX, IDC_REDO_EDIT, m_RedoEdit);
	DDX_Control(pDX, IDC_CLEAR_SELECTED_EDIT, m_ClearSelectedEdit);
	DDX_Control(pDX, IDC_CLEAR_ALL_EDITS, m_ClearAllEdits);
	DDX_Control(pDX, IDC_APPLY_STORED_EDITS, m_ApplyStoredEdits);
	//}}AFX_DATA_MAP

	m_ApplySelected.SetFont(skin_font);
	m_RefreshAndDontApply.SetFont(skin_font);
	m_RefreshAndReApply.SetFont(skin_font);
	m_UndoEdit.SetFont(skin_font);
	m_TouchUpList.SetFont(skin_font);
	m_RememberAllEdits.SetFont(skin_font);
	m_RedoEdit.SetFont(skin_font);
	m_ClearSelectedEdit.SetFont(skin_font);
	m_ClearAllEdits.SetFont(skin_font);
	m_ApplyStoredEdits.SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(TouchUpsDlg, CDialog)
	//{{AFX_MSG_MAP(TouchUpsDlg)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CLEAR_ALL_EDITS, OnClearAllEdits)
	ON_BN_CLICKED(IDC_CLEAR_SELECTED_EDIT, OnClearSelectedEdit)
	ON_BN_CLICKED(IDC_UNDO_EDIT, OnUndoEdit)
	ON_BN_CLICKED(IDC_REDO_EDIT, OnRedoEdit)
	ON_BN_CLICKED(IDC_REMEMBER_ALL_EDITS, OnRememberAllEdits)
	ON_BN_CLICKED(IDC_APPLY_STORED_EDITS, OnApplyStoredEdits)
	ON_LBN_SELCHANGE(IDC_TOUCH_UP_LIST, OnSelchangeTouchUpList)
	ON_BN_CLICKED(IDC_REFRESH_AND_DONT_APPLY, OnRefreshAndDontApply)
	ON_BN_CLICKED(IDC_REFRESH_AND_REAPPLY, OnRefreshAndReapply)
	ON_BN_CLICKED(IDC_APPLY_SELECTED, OnApplySelected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TouchUpsDlg message handlers

bool Open_TouchUp_Dlg()
{
	if(!touchUpsDlg)
	{
		touchUpsDlg = new TouchUpsDlg;
		touchUpsDlg->Create(IDD_TOUCH_UPS_DLG);
		touchUpsDlg->SetWindowText("Touch Ups");
		Create_ToolSkin_Frame(touchUpsDlg, "Touch Ups", IDC_TOUCHUPS_TOOL);
	}
	touchUpsDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_TouchUp_Dlg()
{
	touchUpsDlg->ShowWindow(SW_HIDE);
	return true;
}


void TouchUpsDlg::OnClose() 
{
	Close_TouchUp_Tool();
}

bool Append_Last_TouchUp_To_List()
{
	if(touchUpsDlg)
	{
		char text[512];
		if(Get_TouchUp_Edit_Group_Text(Num_TouchUp_Edit_Groups()-1, text))
		{
			touchUpsDlg->m_TouchUpList.AddString(text);
			return true;
		}
	}
	return false;
}

bool List_All_TouchUp_Edits()
{
	if(touchUpsDlg)
	{
		touchUpsDlg->m_TouchUpList.ResetContent();
		int n = Num_TouchUp_Edit_Groups();
		char text[512];
		for(int i = 0;i<n;i++)
		{
			//these must succeed because list is index based
			Get_TouchUp_Edit_Group_Text(i, text);
			touchUpsDlg->m_TouchUpList.AddString(text);
		}
		return true;
	}
	return false;
}


HBRUSH TouchUpsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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
}

void TouchUpsDlg::OnClearAllEdits() 
{
	Clear_All_TouchUps();
	List_All_TouchUp_Edits();
	Refresh_Layers_And_Dont_Apply_Touchups();
}

void TouchUpsDlg::OnClearSelectedEdit() 
{
	int sel = m_TouchUpList.GetCurSel();
	if(sel!=-1)
	{
		Remove_Single_TouchUp_Edit(sel);
		List_All_TouchUp_Edits();
	}
}

void TouchUpsDlg::OnUndoEdit() 
{
}

void TouchUpsDlg::OnRedoEdit() 
{
}

void TouchUpsDlg::OnRememberAllEdits() 
{
	Remember_All_TouchUp_Edits();
}

void TouchUpsDlg::OnApplyStoredEdits() 
{
	Add_All_Stored_TouchUp_Edits();
	List_All_TouchUp_Edits();
	Refresh_Layers_And_Apply_Touchups();
}

void TouchUpsDlg::OnSelchangeTouchUpList() 
{
}

void TouchUpsDlg::OnRefreshAndDontApply() 
{
	Refresh_Layers_And_Dont_Apply_Touchups();
}

void TouchUpsDlg::OnRefreshAndReapply() 
{
	Refresh_Layers_And_Apply_Touchups();
}


void TouchUpsDlg::OnApplySelected() 
{
	int sel = m_TouchUpList.GetCurSel();
	if(sel!=-1)
	{
		Apply_Single_TouchUp_Edit(sel);
	}
}
