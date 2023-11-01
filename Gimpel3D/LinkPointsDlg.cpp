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
// LinkPointsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "LinkPointsDlg.h"
#include "LinkPointsTool.h"

bool Keyframe_Linked_Model_Layer();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LinkPointsDlg dialog

LinkPointsDlg *linkPointsDlg = 0;

LinkPointsDlg::LinkPointsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LinkPointsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(LinkPointsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void LinkPointsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LinkPointsDlg)
	DDX_Control(pDX, IDC_SHOW_VECTORS, m_ShowVectors);
	DDX_Control(pDX, IDC_FIND_START_ALIGNMENT, m_FindStartAlignment);
	DDX_Control(pDX, IDC_SAVE_LINKS, m_SaveLinks);
	DDX_Control(pDX, IDC_ALIGN_MODEL4, m_AlignModel4);
	DDX_Control(pDX, IDC_ALIGN_MODEL3, m_AlignModel3);
	DDX_Control(pDX, IDC_ALIGN_MODEL2, m_AlignModel2);
	DDX_Control(pDX, IDC_UNLINK_SELECTED_POINTS, m_UnLinkSelectedPoints);
	DDX_Control(pDX, IDC_CLEAR_LINKS, m_ClearLinks);
	DDX_Control(pDX, IDC_LINK_SELECTED_POINTS, m_LinkSelectedPoints);
	DDX_Control(pDX, IDC_SAVE_LINK_POINTS, m_SaveLinkPoints);
	DDX_Control(pDX, IDC_RELOAD_LINK_POINTS, m_ReLoadLinkPoints);
	DDX_Control(pDX, IDC_CLEAR_LINK_POINTS, m_ClearLinkPoints);
	DDX_Control(pDX, IDC_VIEW_SCENE, m_ViewScene);
	DDX_Control(pDX, IDC_VIEW_MODEL, m_ViewModel);
	DDX_Control(pDX, IDC_CLICK_TO_MOVE_POINTS, m_ClickToMovePoints);
	DDX_Control(pDX, IDC_CLICK_TO_DELETE_POINTS, m_ClickToDeletePoints);
	DDX_Control(pDX, IDC_CLICK_TO_ADD_POINTS, m_ClickToAddPoints);
	DDX_Control(pDX, IDC_ALIGN_MODEL, m_AlignModel);
	//}}AFX_DATA_MAP
	CheckDlgButton(IDC_CLICK_TO_ADD_POINTS, click_to_add_link_points);
	CheckDlgButton(IDC_CLICK_TO_DELETE_POINTS, click_to_delete_link_points);
	CheckDlgButton(IDC_CLICK_TO_MOVE_POINTS, click_to_move_link_points);

	CheckDlgButton(IDC_VIEW_SCENE, view_link_points_scene);
	CheckDlgButton(IDC_VIEW_MODEL, view_link_points_model);

	this->m_FindStartAlignment.SetFont(skin_font);

	m_ShowVectors.SetFont(skin_font);
	
	m_SaveLinks.SetFont(skin_font);
	m_ClearLinks.SetFont(skin_font);

	m_UnLinkSelectedPoints.SetFont(skin_font);
	
	m_LinkSelectedPoints.SetFont(skin_font);
	
	m_SaveLinkPoints.SetFont(skin_font);
	m_ReLoadLinkPoints.SetFont(skin_font);
	m_ClearLinkPoints.SetFont(skin_font);

	m_ViewScene.SetFont(skin_font);
	m_ViewModel.SetFont(skin_font);
	m_ClickToMovePoints.SetFont(skin_font);
	m_ClickToDeletePoints.SetFont(skin_font);
	m_ClickToAddPoints.SetFont(skin_font);
	m_AlignModel.SetFont(skin_font);
	m_AlignModel2.SetFont(skin_font);
	m_AlignModel3.SetFont(skin_font);
	m_AlignModel4.SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(LinkPointsDlg, CDialog)
	//{{AFX_MSG_MAP(LinkPointsDlg)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_ALIGN_MODEL, OnAlignModel)
	ON_BN_CLICKED(IDC_CLICK_TO_ADD_POINTS, OnClickToAddPoints)
	ON_BN_CLICKED(IDC_CLICK_TO_DELETE_POINTS, OnClickToDeletePoints)
	ON_BN_CLICKED(IDC_CLICK_TO_MOVE_POINTS, OnClickToMovePoints)
	ON_BN_CLICKED(IDC_VIEW_MODEL, OnViewModel)
	ON_BN_CLICKED(IDC_VIEW_SCENE, OnViewScene)
	ON_BN_CLICKED(IDC_CLEAR_LINK_POINTS, OnClearLinkPoints)
	ON_BN_CLICKED(IDC_SAVE_LINK_POINTS, OnSaveLinkPoints)
	ON_BN_CLICKED(IDC_RELOAD_LINK_POINTS, OnReloadLinkPoints)
	ON_BN_CLICKED(IDC_LINK_SELECTED_POINTS, OnLinkSelectedPoints)
	ON_BN_CLICKED(IDC_CLEAR_LINKS, OnClearLinks)
	ON_BN_CLICKED(IDC_UNLINK_SELECTED_POINTS, OnUnlinkSelectedPoints)
	ON_BN_CLICKED(IDC_SHOW_VECTORS, OnShowVectors)
	ON_BN_CLICKED(IDC_ALIGN_MODEL2, OnAlignModel2)
	ON_BN_CLICKED(IDC_ALIGN_MODEL3, OnAlignModel3)
	ON_BN_CLICKED(IDC_ALIGN_MODEL4, OnAlignModel4)
	ON_BN_CLICKED(IDC_SAVE_LINKS, OnSaveLinks)
	ON_BN_CLICKED(IDC_FIND_START_ALIGNMENT, OnFindStartAlignment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LinkPointsDlg message handlers

#include "GeometryToolDlg.h"

extern GeometryToolDlg *geometryDlg;


bool Open_Link_Points_Dlg()
{
	if(!linkPointsDlg)
	{
		linkPointsDlg = new LinkPointsDlg;
		linkPointsDlg->Create(IDD_LINK_POINTS_DLG);
		linkPointsDlg->SetWindowText("Link Points To Model");
		Create_Skin_Frame(linkPointsDlg);
	}
	linkPointsDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_Link_Points_Dlg()
{
	linkPointsDlg->GetParent()->ShowWindow(SW_HIDE);
	return true;
}

bool Update_Link_Point_Dlg_Align_Button()
{
	if(!linkPointsDlg)
	{
		return false;
	}
	linkPointsDlg->m_AlignModel.SetWindowText("Align Position");
	linkPointsDlg->m_AlignModel2.SetWindowText("Align Rotation");
	linkPointsDlg->m_AlignModel3.SetWindowText("Align Depth");
	linkPointsDlg->m_AlignModel4.SetWindowText("Align All");
	if(iterate_link_point_alignment)
	{
		if(iterate_link_point_alignment_position&&iterate_link_point_alignment_rotation&&iterate_link_point_alignment_depth)
		{
			linkPointsDlg->m_AlignModel4.SetWindowText("Stop");
			linkPointsDlg->m_AlignModel4.EnableWindow(true);
			linkPointsDlg->m_AlignModel.EnableWindow(false);
			linkPointsDlg->m_AlignModel2.EnableWindow(false);
			linkPointsDlg->m_AlignModel3.EnableWindow(false);
		}
		else if(iterate_link_point_alignment_position)
		{
			linkPointsDlg->m_AlignModel.SetWindowText("Stop");
			linkPointsDlg->m_AlignModel.EnableWindow(true);
			linkPointsDlg->m_AlignModel2.EnableWindow(false);
			linkPointsDlg->m_AlignModel3.EnableWindow(false);
			linkPointsDlg->m_AlignModel4.EnableWindow(false);
		}
		else if(iterate_link_point_alignment_rotation)
		{
			linkPointsDlg->m_AlignModel2.SetWindowText("Stop");
			linkPointsDlg->m_AlignModel.EnableWindow(false);
			linkPointsDlg->m_AlignModel2.EnableWindow(true);
			linkPointsDlg->m_AlignModel3.EnableWindow(false);
			linkPointsDlg->m_AlignModel4.EnableWindow(false);
		}
		else if(iterate_link_point_alignment_depth)
		{
			linkPointsDlg->m_AlignModel3.SetWindowText("Stop");
			linkPointsDlg->m_AlignModel.EnableWindow(false);
			linkPointsDlg->m_AlignModel2.EnableWindow(false);
			linkPointsDlg->m_AlignModel3.EnableWindow(true);
			linkPointsDlg->m_AlignModel4.EnableWindow(false);
		}
	}
	else
	{
		linkPointsDlg->m_AlignModel.EnableWindow(true);
		linkPointsDlg->m_AlignModel2.EnableWindow(true);
		linkPointsDlg->m_AlignModel3.EnableWindow(true);
		linkPointsDlg->m_AlignModel4.EnableWindow(true);
	}
	return true;
}

bool Update_Link_Points_Dlg_View_Checkboxes()
{
	if(!linkPointsDlg){return false;}
	linkPointsDlg->CheckDlgButton(IDC_VIEW_MODEL, view_link_points_model);
	linkPointsDlg->CheckDlgButton(IDC_VIEW_SCENE, view_link_points_scene);
	linkPointsDlg->CheckDlgButton(IDC_SHOW_VECTORS, render_link_points_vectors);
	return true;
}

void LinkPointsDlg::OnClose() 
{
	Close_Link_Points_Tool();
}

HBRUSH LinkPointsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
}

void LinkPointsDlg::OnClickToAddPoints() 
{
	click_to_add_link_points = true;
	click_to_delete_link_points = false;
	click_to_move_link_points = false;
	CheckDlgButton(IDC_CLICK_TO_ADD_POINTS, click_to_add_link_points);
	CheckDlgButton(IDC_CLICK_TO_DELETE_POINTS, click_to_delete_link_points);
	CheckDlgButton(IDC_CLICK_TO_MOVE_POINTS, click_to_move_link_points);
}

void LinkPointsDlg::OnClickToDeletePoints() 
{
	click_to_delete_link_points = true;
	click_to_add_link_points = false;
	click_to_move_link_points = false;
	CheckDlgButton(IDC_CLICK_TO_ADD_POINTS, click_to_add_link_points);
	CheckDlgButton(IDC_CLICK_TO_DELETE_POINTS, click_to_delete_link_points);
	CheckDlgButton(IDC_CLICK_TO_MOVE_POINTS, click_to_move_link_points);
}

void LinkPointsDlg::OnClickToMovePoints() 
{
	click_to_add_link_points = false;
	click_to_delete_link_points = false;
	click_to_move_link_points = true;
	CheckDlgButton(IDC_CLICK_TO_ADD_POINTS, click_to_add_link_points);
	CheckDlgButton(IDC_CLICK_TO_DELETE_POINTS, click_to_delete_link_points);
	CheckDlgButton(IDC_CLICK_TO_MOVE_POINTS, click_to_move_link_points);
}

/*
void LinkPointsDlg::OnFlipModelUp() 
{
	Flip_Link_Points_Model_Up();
}

void LinkPointsDlg::OnFlipModelDown() 
{
	Flip_Link_Points_Model_Down();
}

void LinkPointsDlg::OnFlipModelLeft() 
{
	Flip_Link_Points_Model_Left();
}

void LinkPointsDlg::OnFlipModelRight() 
{
	Flip_Link_Points_Model_Right();
}
*/

void LinkPointsDlg::OnViewModel() 
{
	bool b = IsDlgButtonChecked(IDC_VIEW_MODEL)!=0;
	if(!b)
	{
		CheckDlgButton(IDC_VIEW_MODEL, true);//no unchecking!
		return;
	}
	View_Link_Points_Model();
	CheckDlgButton(IDC_VIEW_SCENE, false);
}

void LinkPointsDlg::OnViewScene() 
{
	bool b = IsDlgButtonChecked(IDC_VIEW_SCENE)!=0;
	if(!b)
	{
		CheckDlgButton(IDC_VIEW_SCENE, true);//no unchecking!
		return;
	}
	View_Link_Points_Scene();
	CheckDlgButton(IDC_VIEW_MODEL, false);
}

void LinkPointsDlg::OnClearLinkPoints() 
{
	Clear_Link_Points();
	redraw_frame = true;
}

void LinkPointsDlg::OnSaveLinkPoints() 
{
	Try_Save_Current_Model_Link_Points();
}

void LinkPointsDlg::OnReloadLinkPoints() 
{
	Try_Load_Model_Link_Points();
	redraw_frame = true;
}

void LinkPointsDlg::OnLinkSelectedPoints() 
{
	Link_Selected_Points();
}

void LinkPointsDlg::OnClearLinks() 
{
	Clear_All_Link_Associations();
}

void LinkPointsDlg::OnUnlinkSelectedPoints() 
{
	UnLink_Selected_Points();
}

void LinkPointsDlg::OnShowVectors() 
{
	bool b = IsDlgButtonChecked(IDC_SHOW_VECTORS)!=0;
	Show_Link_Points_Vectors(b);
}

void LinkPointsDlg::OnAlignModel() 
{
	Keyframe_Linked_Model_Layer();
	if(iterate_link_point_alignment)
	{
		Stop_Iterative_Link_Point_Alignment();
	}
	else
	{
		Start_Iterative_Link_Point_Alignment(true, false, false);
	}
}

void LinkPointsDlg::OnAlignModel2() 
{
	Keyframe_Linked_Model_Layer();
	if(iterate_link_point_alignment)
	{
		Stop_Iterative_Link_Point_Alignment();
	}
	else
	{
		Start_Iterative_Link_Point_Alignment(false, true, false);
	}
}

void LinkPointsDlg::OnAlignModel3() 
{
	Keyframe_Linked_Model_Layer();
	if(iterate_link_point_alignment)
	{
		Stop_Iterative_Link_Point_Alignment();
	}
	else
	{
		Start_Iterative_Link_Point_Alignment(false, false, true);
	}
}

void LinkPointsDlg::OnAlignModel4() 
{
	Keyframe_Linked_Model_Layer();
	if(iterate_link_point_alignment)
	{
		Stop_Iterative_Link_Point_Alignment();
	}
	else
	{
		Start_Iterative_Link_Point_Alignment(true, true, true);
	}
}

void LinkPointsDlg::OnSaveLinks() 
{
	Save_Layer_Links_Info();
}

void LinkPointsDlg::OnFindStartAlignment() 
{
	Keyframe_Linked_Model_Layer();
	Find_Initial_Link_Point_Alignment();
}
