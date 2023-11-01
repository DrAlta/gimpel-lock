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
// VirtualSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "VirtualSetDlg.h"
#include "VirtualSet.h"
#include "G3DCoreFiles/Layers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VirtualSetDlg dialog

VirtualSetDlg *virtualSetDlg = 0;

VirtualSetDlg::VirtualSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VirtualSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VirtualSetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void VirtualSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VirtualSetDlg)
	DDX_Control(pDX, IDC_CHANGE_STATIC_TO_DYNAMIC, m_StaticToDynamic);
	DDX_Control(pDX, IDC_CHANGE_DYNAMIC_TO_STATIC, m_DynamicToStatic);
	DDX_Control(pDX, IDC_TRACK_TO_NEXT_FRAME, m_TrackToNextFrame);
	DDX_Control(pDX, IDC_LOAD_SCENE_FRAMEWORK, m_LoadFramework);
	DDX_Control(pDX, IDC_SAVE_SCENE_FRAMEWORK, m_SaveFramework);
	DDX_Control(pDX, IDC_GENERATE_SCENE_FRAMEWORK, m_GenerateFramework);
	DDX_Control(pDX, IDC_DYNAMIC_LAYERS_LIST, m_DynamicLayersList);
	DDX_Control(pDX, IDC_STATIC_LAYERS_LIST, m_StaticLayersList);
	//}}AFX_DATA_MAP

	GetDlgItem(IDC_STATIC_LAYERS_LIST)->SetFont(skin_font);
	GetDlgItem(IDC_DYNAMIC_LAYERS_LIST)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_SAVE_SCENE_FRAMEWORK)->SetFont(skin_font);
	GetDlgItem(IDC_LOAD_SCENE_FRAMEWORK)->SetFont(skin_font);
	GetDlgItem(IDC_TRACK_TO_NEXT_FRAME)->SetFont(skin_font);
	GetDlgItem(IDC_CHANGE_STATIC_TO_DYNAMIC)->SetFont(skin_font);
	GetDlgItem(IDC_CHANGE_DYNAMIC_TO_STATIC)->SetFont(skin_font);
	GetDlgItem(IDC_RENDER_SCENE_FRAMEWORK)->SetFont(skin_font);
	GetDlgItem(IDC_GENERATE_SCENE_FRAMEWORK)->SetFont(skin_font);

}


BEGIN_MESSAGE_MAP(VirtualSetDlg, CDialog)
	//{{AFX_MSG_MAP(VirtualSetDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_GENERATE_SCENE_FRAMEWORK, OnGenerateSceneFramework)
	ON_BN_CLICKED(IDC_SAVE_SCENE_FRAMEWORK, OnSaveSceneFramework)
	ON_BN_CLICKED(IDC_TRACK_TO_NEXT_FRAME, OnTrackToNextFrame)
	ON_BN_CLICKED(IDC_LOAD_SCENE_FRAMEWORK, OnLoadSceneFramework)
	ON_BN_CLICKED(IDC_RENDER_SCENE_FRAMEWORK, OnRenderSceneFramework)
	ON_LBN_SELCHANGE(IDC_STATIC_LAYERS_LIST, OnSelchangeStaticLayersList)
	ON_LBN_SELCHANGE(IDC_DYNAMIC_LAYERS_LIST, OnSelchangeDynamicLayersList)
	ON_BN_CLICKED(IDC_CHANGE_STATIC_TO_DYNAMIC, OnChangeStaticToDynamic)
	ON_BN_CLICKED(IDC_CHANGE_DYNAMIC_TO_STATIC, OnChangeDynamicToStatic)
	ON_LBN_DBLCLK(IDC_DYNAMIC_LAYERS_LIST, OnDblclkDynamicLayersList)
	ON_LBN_DBLCLK(IDC_STATIC_LAYERS_LIST, OnDblclkStaticLayersList)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VirtualSetDlg message handlers

bool Open_Virtual_Set_Dlg()
{
	if(!virtualSetDlg)
	{
		virtualSetDlg = new VirtualSetDlg;
		virtualSetDlg->Create(IDD_VIRTUAL_SET);
		virtualSetDlg->SetWindowText("Virtual Set");
		Create_ToolSkin_Frame(virtualSetDlg, "Virtual Set", IDC_OPEN_VIRTUAL_SET_TOOL);
	}
	virtualSetDlg->ListLayers();
	virtualSetDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_Virtual_Set_Dlg()
{
	virtualSetDlg->ShowWindow(SW_HIDE);
	return true;
}

void VirtualSetDlg::OnClose() 
{
	Close_Virtual_Set_Tool();
}

void VirtualSetDlg::ListLayers()
{
	m_DynamicLayersList.ResetContent();
	m_StaticLayersList.ResetContent();
	int n = Num_Layers();
	char name[512];
	for(int i = 0;i<n;i++)
	{
		Layer_Name(i, name);
		if(Layer_Is_Dynamic(Get_Layer_ID(i)))
		{
			m_DynamicLayersList.AddString(name);
		}
		else
		{
			m_StaticLayersList.AddString(name);
		}
	}
}

void VirtualSetDlg::OnGenerateSceneFramework() 
{
}

void VirtualSetDlg::OnSaveSceneFramework() 
{
}

void VirtualSetDlg::OnTrackToNextFrame() 
{
}

void VirtualSetDlg::OnLoadSceneFramework() 
{
}

void VirtualSetDlg::OnRenderSceneFramework() 
{
}

void VirtualSetDlg::OnSelchangeStaticLayersList() 
{
}

void VirtualSetDlg::OnSelchangeDynamicLayersList() 
{
}

void VirtualSetDlg::OnChangeStaticToDynamic() 
{
	OnDblclkStaticLayersList();
}

void VirtualSetDlg::OnChangeDynamicToStatic() 
{
	OnDblclkDynamicLayersList();
}

void VirtualSetDlg::OnDblclkDynamicLayersList() 
{
	int sel = this->m_DynamicLayersList.GetCurSel();
	if(sel!=-1)
	{
		Set_Layer_Dynamic(Get_Layer_ID(sel), false);
		ListLayers();
	}
}

void VirtualSetDlg::OnDblclkStaticLayersList() 
{
	int sel = this->m_StaticLayersList.GetCurSel();
	if(sel!=-1)
	{
		Set_Layer_Dynamic(Get_Layer_ID(sel), true);
		ListLayers();
	}
}

HBRUSH VirtualSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}
