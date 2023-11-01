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
// FeaturePointsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "FeaturePointsDlg.h"
#include "FeaturePoints.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FeaturePointsDlg dialog

FeaturePointsDlg *featurePointsDlg = 0;

FeaturePointsDlg::FeaturePointsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FeaturePointsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FeaturePointsDlg)
	//}}AFX_DATA_INIT
}


void FeaturePointsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FeaturePointsDlg)
	DDX_Control(pDX, IDC_CLEAR_ALL_KEYFRAMES_FOR_THIS_POINT, m_ClearAllKeyframesForThisPoint);
	DDX_Control(pDX, IDC_KEYFRAME_ALL_POINTS_FOR_THIS_FRAME, m_KeyframeAllPointsForThisFrame);
	DDX_Control(pDX, IDC_KEYFRAME_SELECTED_FEATURE_POINT, m_KeyframeSelectedFeaturePoint);
	DDX_Control(pDX, IDC_START_FEATURE_POINT_OUTLINE, m_StartOutline);
	DDX_Control(pDX, IDC_FINISH_FEATURE_POINT_OUTLINE, m_FinishOutline);
	DDX_Control(pDX, IDC_CLICK_TO_MOVE_POINTS, m_ClickToMovePoints);
	DDX_Control(pDX, IDC_CLICK_TO_DELETE_POINTS, m_ClickToDeletePoints);
	DDX_Control(pDX, IDC_CLICK_TO_ADD_POINTS, m_ClickToAddPoints);
	DDX_Control(pDX, IDC_CLEAR_ALL_POINTS, m_ClearAllPoints);
	DDX_Control(pDX, IDC_CLEAR_ALL_FEATURE_POINT_KEYFRAMES, m_ClearAllKeyframes);
	DDX_Control(pDX, IDC_CANCEL_FEATURE_POINT_OUTLINE, m_CancelOutline);
	DDX_Control(pDX, IDC_FEATURE_POINT_OUTLINE_LIST, m_OutlineList);
	DDX_Control(pDX, IDC_FEATURE_POINT_LIST, m_FeaturePointList);
	//}}AFX_DATA_MAP
	CheckDlgButton(IDC_CLICK_TO_ADD_POINTS, click_to_add_feature_points);
	CheckDlgButton(IDC_CLICK_TO_DELETE_POINTS, click_to_delete_feature_points);
	CheckDlgButton(IDC_CLICK_TO_MOVE_POINTS, click_to_move_feature_points);
	CheckDlgButton(IDC_TRACK_FEATURE_POINTS, track_feature_points);

	m_ClearAllKeyframesForThisPoint.SetFont(skin_font);
	m_KeyframeSelectedFeaturePoint.SetFont(skin_font);
	m_KeyframeAllPointsForThisFrame.SetFont(skin_font);

	GetDlgItem(IDC_FEATURE_POINT_LIST)->SetFont(skin_font);
	GetDlgItem(IDC_CLEAR_ALL_POINTS)->SetFont(skin_font);
	GetDlgItem(IDC_CLICK_TO_ADD_POINTS)->SetFont(skin_font);
	GetDlgItem(IDC_CLICK_TO_DELETE_POINTS)->SetFont(skin_font);
	GetDlgItem(IDC_CLICK_TO_MOVE_POINTS)->SetFont(skin_font);
	GetDlgItem(IDC_TRACK_FEATURE_POINTS)->SetFont(skin_font);
	GetDlgItem(IDC_CLEAR_ALL_FEATURE_POINT_KEYFRAMES)->SetFont(skin_font);
	GetDlgItem(IDC_START_FEATURE_POINT_OUTLINE)->SetFont(skin_font);
	GetDlgItem(IDC_FEATURE_POINT_OUTLINE_LIST)->SetFont(skin_font);
	GetDlgItem(IDC_CANCEL_FEATURE_POINT_OUTLINE)->SetFont(skin_font);
	GetDlgItem(IDC_FINISH_FEATURE_POINT_OUTLINE)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(FeaturePointsDlg, CDialog)
	//{{AFX_MSG_MAP(FeaturePointsDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CLICK_TO_ADD_POINTS, OnClickToAddPoints)
	ON_BN_CLICKED(IDC_CLICK_TO_DELETE_POINTS, OnClickToDeletePoints)
	ON_BN_CLICKED(IDC_CLICK_TO_MOVE_POINTS, OnClickToMovePoints)
	ON_BN_CLICKED(IDC_CLEAR_ALL_POINTS, OnClearAllPoints)
	ON_BN_CLICKED(IDC_TRACK_TO_NEXT_FRAME, OnTrackToNextFrame)
	ON_BN_CLICKED(IDC_TRACK_FEATURE_POINTS, OnTrackFeaturePoints)
	ON_BN_CLICKED(IDC_CLEAR_ALL_FEATURE_POINT_KEYFRAMES, OnClearAllFeaturePointKeyframes)
	ON_LBN_SELCHANGE(IDC_FEATURE_POINT_OUTLINE_LIST, OnSelchangeFeaturePointOutlineList)
	ON_BN_CLICKED(IDC_START_FEATURE_POINT_OUTLINE, OnStartFeaturePointOutline)
	ON_BN_CLICKED(IDC_FINISH_FEATURE_POINT_OUTLINE, OnFinishFeaturePointOutline)
	ON_BN_CLICKED(IDC_CANCEL_FEATURE_POINT_OUTLINE, OnCancelFeaturePointOutline)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_KEYFRAME_SELECTED_FEATURE_POINT, OnKeyframeSelectedFeaturePoint)
	ON_BN_CLICKED(IDC_KEYFRAME_ALL_POINTS_FOR_THIS_FRAME, OnKeyframeAllPointsForThisFrame)
	ON_BN_CLICKED(IDC_CLEAR_ALL_KEYFRAMES_FOR_THIS_POINT, OnClearAllKeyframesForThisPoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FeaturePointsDlg message handlers


bool Open_Feature_Points_Dlg()
{
	if(!featurePointsDlg)
	{
		featurePointsDlg = new FeaturePointsDlg;
		featurePointsDlg->Create(IDD_FEATURE_POINTS_DLG);
		featurePointsDlg->SetWindowText("Feature Points");
		Create_ToolSkin_Frame(featurePointsDlg, "Feature Points", IDC_FEATURE_POINTS_TOOL);
	}
	featurePointsDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_Feature_Points_Dlg()
{
	featurePointsDlg->ShowWindow(SW_HIDE);
	return true;
}

void FeaturePointsDlg::OnClose() 
{
	Close_Feature_Points_Tool();
}

void FeaturePointsDlg::OnClickToAddPoints() 
{
	click_to_add_feature_points = true;
	click_to_delete_feature_points = false;
	click_to_move_feature_points = false;
	CheckDlgButton(IDC_CLICK_TO_ADD_POINTS, click_to_add_feature_points);
	CheckDlgButton(IDC_CLICK_TO_DELETE_POINTS, click_to_delete_feature_points);
	CheckDlgButton(IDC_CLICK_TO_MOVE_POINTS, click_to_move_feature_points);
}

void FeaturePointsDlg::OnClickToDeletePoints()
{
	click_to_delete_feature_points = true;
	click_to_add_feature_points = false;
	click_to_move_feature_points = false;
	CheckDlgButton(IDC_CLICK_TO_ADD_POINTS, click_to_add_feature_points);
	CheckDlgButton(IDC_CLICK_TO_DELETE_POINTS, click_to_delete_feature_points);
	CheckDlgButton(IDC_CLICK_TO_MOVE_POINTS, click_to_move_feature_points);
}

void FeaturePointsDlg::OnClickToMovePoints()
{
	click_to_add_feature_points = false;
	click_to_delete_feature_points = false;
	click_to_move_feature_points = true;
	CheckDlgButton(IDC_CLICK_TO_ADD_POINTS, click_to_add_feature_points);
	CheckDlgButton(IDC_CLICK_TO_DELETE_POINTS, click_to_delete_feature_points);
	CheckDlgButton(IDC_CLICK_TO_MOVE_POINTS, click_to_move_feature_points);
}

void FeaturePointsDlg::OnClearAllPoints() 
{
	Clear_Feature_Points();
	redraw_edit_window = true;
}

void FeaturePointsDlg::OnTrackToNextFrame() 
{
}

void FeaturePointsDlg::OnTrackFeaturePoints() 
{
	track_feature_points = IsDlgButtonChecked(IDC_TRACK_FEATURE_POINTS)!=0;
}

void FeaturePointsDlg::OnClearAllFeaturePointKeyframes() 
{
	Set_All_Feature_Point_Keyframes(false);
}


void FeaturePointsDlg::OnSelchangeFeaturePointOutlineList() 
{
}

void FeaturePointsDlg::OnStartFeaturePointOutline() 
{
}

void FeaturePointsDlg::OnFinishFeaturePointOutline() 
{
}

void FeaturePointsDlg::OnCancelFeaturePointOutline() 
{
}

HBRUSH FeaturePointsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

bool Set_Feature_Point_Dlg_Selected_Point_Keyframed(bool enable, bool check)
{
	if(featurePointsDlg)
	{
		featurePointsDlg->m_KeyframeSelectedFeaturePoint.EnableWindow(enable);
		featurePointsDlg->m_ClearAllKeyframesForThisPoint.EnableWindow(enable);
		featurePointsDlg->CheckDlgButton(IDC_KEYFRAME_SELECTED_FEATURE_POINT, check);
	}
	return true;
}

void FeaturePointsDlg::OnKeyframeSelectedFeaturePoint() 
{
	bool b = IsDlgButtonChecked(IDC_KEYFRAME_SELECTED_FEATURE_POINT)!=0;
	Keyframe_Selected_Feature_Point(b);
}


void FeaturePointsDlg::OnKeyframeAllPointsForThisFrame() 
{
	Set_All_Feature_Point_Keyframes(true);
}

void FeaturePointsDlg::OnClearAllKeyframesForThisPoint() 
{
	Clear_All_Keyframes_For_Selected_Feature_Point();
}
