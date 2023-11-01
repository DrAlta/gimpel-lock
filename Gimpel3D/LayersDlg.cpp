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
// LayersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "LayersDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool switch_lists_side;
extern bool hide_lists;

/////////////////////////////////////////////////////////////////////////////
// LayersDlg dialog
extern int sidebar_top_border;
extern int sidebar_bottom_border;
extern int sidebar_width;
extern int sidebar_restore_width;
extern int tooltray_height;

LayersDlg *layersDlg = 0;

LayersDlg::LayersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LayersDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(LayersDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	layersDlg = this;
}


void LayersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LayersDlg)
	DDX_Control(pDX, IDC_UPDATE_LAYER_DATA, m_UpdateLayers);
	DDX_Control(pDX, IDC_LAYERS_LABEL, m_LayersLabel);
	DDX_Control(pDX, IDC_LAYERS_LIST, m_LayersList);
	//}}AFX_DATA_MAP
	CheckDlgButton(IDC_UPDATE_LAYER_DATA, update_3d_data);

	GetDlgItem(IDC_LAYERS_LIST)->SetFont(skin_font);
	GetDlgItem(IDC_LAYERS_LABEL)->SetFont(skin_font);
	GetDlgItem(IDC_FREEZE_LAYER)->SetFont(skin_font);
	GetDlgItem(IDC_OUTLINE_LAYER)->SetFont(skin_font);
	GetDlgItem(IDC_SPECKLE_LAYER)->SetFont(skin_font);
	GetDlgItem(IDC_LAYER_VISIBLE)->SetFont(skin_font);
	GetDlgItem(IDC_LAYER_KEYFRAME)->SetFont(skin_font);
	GetDlgItem(IDC_UPDATE_LAYER_DATA)->SetFont(skin_font);
}

bool Resize_Layers_Dlg()
{
	if(layersDlg->GetSafeHwnd()==0)return false;
	if(hide_lists)
	{
		layersDlg->ShowWindow(SW_HIDE);
	}
	else
	{
		layersDlg->ShowWindow(SW_SHOW);
	}
	CRect rect;
	mainwindow->GetClientRect(&rect);
	int xpos = rect.Width()-sidebar_width;
	int lx = sidebar_top_border+tooltray_height;
	int hx = rect.Height()-sidebar_bottom_border;
	int full_height = hx-lx;
	int height = full_height/2;
	int ypos = sidebar_top_border+height+tooltray_height;
	int interior_width = sidebar_width-1;
	layersDlg->SetParent(mainwindow);
	if(switch_lists_side)
	{
		layersDlg->SetWindowPos(0, 0, ypos, sidebar_width, height, 0);
	}
	else
	{
		layersDlg->SetWindowPos(0, xpos, ypos, sidebar_width, height, 0);
	}
	int list_xpos = 0;
	int list_ypos = 20;
	int list_height = (height-list_ypos)-6;
	int checkboxes_height = 27;
	list_ypos += checkboxes_height;
	list_height -= checkboxes_height;
	layersDlg->m_LayersList.SetWindowPos(0, list_xpos, list_ypos, interior_width, list_height+4, 0);
	int update_layers_checkbox_width = 96;
	layersDlg->m_LayersLabel.SetWindowPos(0, 0, 0, (sidebar_width-update_layers_checkbox_width)-4, 16, 0);
	layersDlg->m_UpdateLayers.SetWindowPos(0, sidebar_width-update_layers_checkbox_width, 0, update_layers_checkbox_width, 16, 0);
	return true;
}

void Enable_Freeze_Checkbox(bool b)
{
	layersDlg->GetDlgItem(IDC_FREEZE_LAYER)->EnableWindow(b);
}
void Enable_Visible_Checkbox(bool b)
{
	layersDlg->GetDlgItem(IDC_LAYER_VISIBLE)->EnableWindow(b);
}
void Enable_Keyframe_Checkbox(bool b)
{
	layersDlg->GetDlgItem(IDC_LAYER_KEYFRAME)->EnableWindow(b);
}
void Enable_Outline_Checkbox(bool b)
{
	layersDlg->GetDlgItem(IDC_OUTLINE_LAYER)->EnableWindow(b);
}
void Enable_Speckle_Checkbox(bool b)
{
	layersDlg->GetDlgItem(IDC_SPECKLE_LAYER)->EnableWindow(b);
}
void Check_Freeze_Checkbox(bool b)
{
	layersDlg->CheckDlgButton(IDC_FREEZE_LAYER, b);
}
void Check_Visible_Checkbox(bool b)
{
	layersDlg->CheckDlgButton(IDC_LAYER_VISIBLE, b);
}
void Check_Keyframe_Checkbox(bool b)
{
	layersDlg->CheckDlgButton(IDC_LAYER_KEYFRAME, b);
}
void Check_Outline_Checkbox(bool b)
{
	layersDlg->CheckDlgButton(IDC_OUTLINE_LAYER, b);
}
void Check_Speckle_Checkbox(bool b)
{
	layersDlg->CheckDlgButton(IDC_SPECKLE_LAYER, b);
}

BEGIN_MESSAGE_MAP(LayersDlg, CDialog)
	//{{AFX_MSG_MAP(LayersDlg)
	ON_LBN_SELCHANGE(IDC_LAYERS_LIST, OnSelchangeLayersList)
	ON_BN_CLICKED(IDC_LAYER_KEYFRAME, OnLayerKeyframe)
	ON_BN_CLICKED(IDC_LAYER_VISIBLE, OnLayerVisible)
	ON_BN_CLICKED(IDC_OUTLINE_LAYER, OnOutlineLayer)
	ON_BN_CLICKED(IDC_SPECKLE_LAYER, OnSpeckleLayer)
	ON_BN_CLICKED(IDC_FREEZE_LAYER, OnFreezeLayer)
	ON_BN_CLICKED(IDC_UPDATE_LAYER_DATA, OnUpdateLayerData)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LayersDlg message handlers

void LayersDlg::OnSelchangeLayersList() 
{
	int n = Num_Layers();
	char text[256];
	int num_selected = 0;
	bool b;
	int sel_index;
	int index;
	for(int i = 0;i<n;i++)
	{
		m_LayersList.GetText(i, text);
		index = Find_Layer_Index(text);
		if(index!=-1)
		{
			b = m_LayersList.GetSel(i)!=0;
			if(b)
			{
				Select_Layer(index, b);
				num_selected++;
				sel_index = index;
			}
			else
			{
				Select_Layer(index, b);
			}
		}
	}
	Update_Selected_States();
}

void LayersDlg::UpdateSelectionInfo()
{
	OnSelchangeLayersList();
}

bool Update_Selection_Info()
{
	if(layersDlg)
	{
		layersDlg->UpdateSelectionInfo();
	}
	return true;
}

int FindExactString(CListBox *lb, char *text)
{
	int n = Num_Layers();
	char buf[512];
	for(int i = 0;i<n;i++)
	{
		lb->GetText(i, buf);
		if(!strcmp(buf, text))
		{
			return i;
		}
	}
	return -1;
}

void List_Selected_Layers()
{
	if(layersDlg)
	{
		int n = Num_Layers();
		char name[64];
		layersDlg->m_LayersList.SelItemRange(false, 0, n);
		int num_selected = 0;
		for(int i = 0;i<n;i++)
		{
			if(Layer_Is_Selected(i))
			{
				Layer_Name(i, name);
				int n = FindExactString(&layersDlg->m_LayersList, name);
				if(n!=-1)
				{
					layersDlg->m_LayersList.SetSel(n, true);
					num_selected++;
				}
			}
		}
		layersDlg->UpdateSelectionInfo();
	}
}

bool Update_Layer_List()
{
	if(layersDlg)
	{
		layersDlg->m_LayersList.ResetContent();
		int n = Num_Layers();
		char name[512];
		for(int i = 0;i<n;i++)
		{
			Layer_Name(i, name);
			layersDlg->m_LayersList.AddString(name);
		}
		List_Selected_Layers();
	}
	return true;
}


void LayersDlg::OnLayerKeyframe() 
{
	bool b = IsDlgButtonChecked(IDC_LAYER_KEYFRAME)!=0;
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Keyframe_Layer(i, b);
		}
	}
}

void LayersDlg::OnLayerVisible() 
{
	bool b = IsDlgButtonChecked(IDC_LAYER_VISIBLE)!=0;
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Hide_Layer(i, !b);
			redraw_edit_window = true;
			redraw_frame = true;
		}
	}
}

void LayersDlg::OnOutlineLayer() 
{
	bool b = IsDlgButtonChecked(IDC_OUTLINE_LAYER)!=0;
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Outline_Layer(i, b);
			redraw_edit_window = true;
			redraw_frame = true;
		}
	}
	redraw_frame = true;
}

void LayersDlg::OnSpeckleLayer() 
{
	bool b = IsDlgButtonChecked(IDC_SPECKLE_LAYER)!=0;
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Speckle_Layer(i, b);
			redraw_edit_window = true;
			redraw_frame = true;
		}
	}
}

void LayersDlg::OnFreezeLayer() 
{
	bool b = IsDlgButtonChecked(IDC_FREEZE_LAYER)!=0;
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Freeze_Layer(i, b);
			redraw_edit_window = true;
			redraw_frame = true;
		}
	}
}

void LayersDlg::OnUpdateLayerData() 
{
	update_3d_data = IsDlgButtonChecked(IDC_UPDATE_LAYER_DATA)!=0;
	if(update_3d_data)
	{
		Update_Project_Frame_3D_Data();
	}
}

HBRUSH LayersDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_LAYERS_LABEL)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkColor(skin_text_bg_color);	
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}
