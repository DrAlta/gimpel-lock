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
// AnchorPointsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "AnchorPointsDlg.h"
#include "AnchorPoints.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AnchorPointsDlg dialog


AnchorPointsDlg *anchorPointsDlg = 0;

AnchorPointsDlg::AnchorPointsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AnchorPointsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AnchorPointsDlg)
	//}}AFX_DATA_INIT
}


void AnchorPointsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AnchorPointsDlg)
	DDX_Control(pDX, IDC_REMOVE_ANCHOR_POINT_FROM_SELECTED_FEATURE_POINT, m_RemoveAnchorPointFromSelectedFeaturePoint);
	DDX_Control(pDX, IDC_REMOVE_ALL_ANCHOR_POINTS, m_RemoveAllAnchorPoints);
	DDX_Control(pDX, IDC_ATTACH_ANCHOR_POINT_TO_SELECTED_FEATURE_POINT, m_AttachAnchorPointToSelectedFeaturePoint);
	DDX_Control(pDX, IDC_STATIC1, m_Static1);
	DDX_Control(pDX, IDC_COPY_ANCHOR_POINTS, m_CopyAnchorPoints);
	DDX_Control(pDX, IDC_CLEAR_SAVED_ANCHOR_POINTS, m_ClearSavedAnchorPoints);
	DDX_Control(pDX, IDC_APPLY_SAVED_ANCHOR_POINTS, m_ApplySavedAnchorPoints);
	DDX_Control(pDX, IDC_UPDATE_ANCHOR_POINTS_IN_REALTIME, m_UpdateInRealtime);
	DDX_Control(pDX, IDC_CLICK_TO_MOVE_POINTS, m_ClickToMovePoints);
	DDX_Control(pDX, IDC_CLICK_TO_DELETE_POINTS, m_ClickToDeletePoints);
	DDX_Control(pDX, IDC_CLICK_TO_ADD_POINTS, m_ClickToAddPoints);
	DDX_Control(pDX, IDC_UPDATE_AFFECTED_LAYERS, m_UpdateAffectedLayers);
	DDX_Control(pDX, IDC_CLEAR_ANCHOR_POINTS, m_ClearAnchorPoints);
	DDX_Control(pDX, IDC_ANCHOR_RANGE, m_AnchorRange);
	DDX_Control(pDX, IDC_ANCHOR_RANGE_SLIDER, m_AnchorRangeSlider);
	DDX_Control(pDX, IDC_ANCHOR_POINTS_LIST, m_AnchorPointsList);
	DDX_Control(pDX, IDC_LAYERS_LIST, m_LayersList);
	//}}AFX_DATA_MAP

	m_RemoveAnchorPointFromSelectedFeaturePoint.SetFont(skin_font);
	m_RemoveAllAnchorPoints.SetFont(skin_font);
	m_AttachAnchorPointToSelectedFeaturePoint.SetFont(skin_font);


	CheckDlgButton(IDC_UPDATE_ANCHOR_POINTS_IN_REALTIME, update_anchor_points_in_realtime);
	m_AnchorRangeSlider.SetRange(0,200);
	Set_Displayed_Anchor_Range(default_anchor_range);
	Set_Displayed_Anchor_Type(default_anchor_point_falloff_type);

	m_Static1.SetFont(skin_font);
	m_CopyAnchorPoints.SetFont(skin_font);
	m_ClearSavedAnchorPoints.SetFont(skin_font);
	m_ApplySavedAnchorPoints.SetFont(skin_font);
	m_UpdateInRealtime.SetFont(skin_font);
	m_ClickToMovePoints.SetFont(skin_font);
	m_ClickToDeletePoints.SetFont(skin_font);
	m_ClickToAddPoints.SetFont(skin_font);
	m_UpdateAffectedLayers.SetFont(skin_font);
	m_ClearAnchorPoints.SetFont(skin_font);
	m_AnchorRange.SetFont(skin_font);
	m_AnchorRangeSlider.SetFont(skin_font);
	m_AnchorPointsList.SetFont(skin_font);
	m_LayersList.SetFont(skin_font);

	GetDlgItem(IDC_LINEAR_FALLOFF)->SetFont(skin_font);
	GetDlgItem(IDC_STRETCH_FALLOFF)->SetFont(skin_font);
	GetDlgItem(IDC_CURVE_FALLOFF)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(AnchorPointsDlg, CDialog)
	//{{AFX_MSG_MAP(AnchorPointsDlg)
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_LAYERS_LIST, OnSelchangeLayersList)
	ON_LBN_SELCHANGE(IDC_ANCHOR_POINTS_LIST, OnSelchangeAnchorPointsList)
	ON_BN_CLICKED(IDC_CLICK_TO_ADD_POINTS, OnClickToAddPoints)
	ON_BN_CLICKED(IDC_CLEAR_ANCHOR_POINTS, OnClearAnchorPoints)
	ON_BN_CLICKED(IDC_CLICK_TO_DELETE_POINTS, OnClickToDeletePoints)
	ON_BN_CLICKED(IDC_CLICK_TO_MOVE_POINTS, OnClickToMovePoints)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_UPDATE_AFFECTED_LAYERS, OnUpdateAffectedLayers)
	ON_BN_CLICKED(IDC_LINEAR_FALLOFF, OnLinearFalloff)
	ON_BN_CLICKED(IDC_STRETCH_FALLOFF, OnStretchFalloff)
	ON_BN_CLICKED(IDC_CURVE_FALLOFF, OnCurveFalloff)
	ON_BN_CLICKED(IDC_UPDATE_ANCHOR_POINTS_IN_REALTIME, OnUpdateAnchorPointsInRealtime)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_COPY_ANCHOR_POINTS, OnCopyAnchorPoints)
	ON_BN_CLICKED(IDC_APPLY_SAVED_ANCHOR_POINTS, OnApplySavedAnchorPoints)
	ON_BN_CLICKED(IDC_CLEAR_SAVED_ANCHOR_POINTS, OnClearSavedAnchorPoints)
	ON_BN_CLICKED(IDC_ATTACH_ANCHOR_POINT_TO_SELECTED_FEATURE_POINT, OnAttachAnchorPointToSelectedFeaturePoint)
	ON_BN_CLICKED(IDC_REMOVE_ANCHOR_POINT_FROM_SELECTED_FEATURE_POINT, OnRemoveAnchorPointFromSelectedFeaturePoint)
	ON_BN_CLICKED(IDC_REMOVE_ALL_ANCHOR_POINTS, OnRemoveAllAnchorPoints)
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_BUTTON1, &AnchorPointsDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AnchorPointsDlg message handlers

bool Open_Anchor_Points_Dlg()
{
	if(!anchorPointsDlg)
	{
		anchorPointsDlg = new AnchorPointsDlg;
		anchorPointsDlg->Create(IDD_ANCHOR_POINTS_DLG);
		anchorPointsDlg->SetWindowText("Anchor Points");
		Create_ToolSkin_Frame(anchorPointsDlg, "Anchor Points", IDC_OPEN_ANCHOR_POINTS_TOOL);
	}
	anchorPointsDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_Anchor_Points_Dlg()
{
	anchorPointsDlg->ShowWindow(SW_HIDE);
	return true;
}

bool Set_Displayed_Anchor_Range(float v)
{
	if(anchorPointsDlg)
	{
		char text[32];
		sprintf(text, "%i", (int)v);
		anchorPointsDlg->m_AnchorRangeSlider.SetPos((int)v);
		anchorPointsDlg->m_AnchorRange.SetWindowText(text);
		return true;
	}
	return false;
}

bool Set_Displayed_Anchor_Type(int type)
{
	if(anchorPointsDlg)
	{
		anchorPointsDlg->CheckDlgButton(IDC_LINEAR_FALLOFF, type==0);
		anchorPointsDlg->CheckDlgButton(IDC_STRETCH_FALLOFF, type==1);
		anchorPointsDlg->CheckDlgButton(IDC_CURVE_FALLOFF, type==2);
		return true;
	}
	return false;
}

/*
bool Update_Anchor_Tool_Selection()
{
	if(anchorPointsDlg)
	{
		bool enable = (Num_Selected_Layers()!=0);
		anchorPointsDlg->GetDlgItem(IDC_LINEAR_FALLOFF)->EnableWindow(enable);
		anchorPointsDlg->GetDlgItem(IDC_STRETCH_FALLOFF)->EnableWindow(enable);
		anchorPointsDlg->GetDlgItem(IDC_CURVE_FALLOFF)->EnableWindow(enable);
	}
	return true;
}
*/

bool Enable_Anchor_Type_Selection(bool b)
{
	if(anchorPointsDlg)
	{
		anchorPointsDlg->GetDlgItem(IDC_LINEAR_FALLOFF)->EnableWindow(b);
		anchorPointsDlg->GetDlgItem(IDC_STRETCH_FALLOFF)->EnableWindow(b);
		anchorPointsDlg->GetDlgItem(IDC_CURVE_FALLOFF)->EnableWindow(b);
	}
	return true;
}


void AnchorPointsDlg::OnClose() 
{
	Close_Anchor_Points_Tool();
}

void AnchorPointsDlg::OnSelchangeLayersList() 
{
}

void AnchorPointsDlg::OnSelchangeAnchorPointsList() 
{
}

//make these act like radio buttons

void AnchorPointsDlg::OnClickToAddPoints() 
{
}

void AnchorPointsDlg::OnClickToDeletePoints() 
{
}

void AnchorPointsDlg::OnClickToMovePoints() 
{
}

void AnchorPointsDlg::OnClearAnchorPoints() 
{
	Clear_Anchor_Points();
	redraw_edit_window = true;
	redraw_frame = true;
}


void AnchorPointsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	int pos = m_AnchorRangeSlider.GetPos();
	Set_Anchor_Influence_Range((float)pos);
	char text[32];
	sprintf(text, "%i", pos);
	m_AnchorRange.SetWindowText(text);
}

void AnchorPointsDlg::OnUpdateAffectedLayers() 
{
	Update_Affected_Layers_With_Anchor_Points();
}

void AnchorPointsDlg::OnLinearFalloff() 
{
	Set_Anchor_Point_Falloff(0);
	Update_Affected_Layers_With_Selected_Anchor_Point();
}

void AnchorPointsDlg::OnStretchFalloff() 
{
	Set_Anchor_Point_Falloff(1);
	Update_Affected_Layers_With_Selected_Anchor_Point();
}

void AnchorPointsDlg::OnCurveFalloff() 
{
	Set_Anchor_Point_Falloff(2);
	Update_Affected_Layers_With_Selected_Anchor_Point();
}

void AnchorPointsDlg::OnUpdateAnchorPointsInRealtime() 
{
	update_anchor_points_in_realtime = IsDlgButtonChecked(IDC_UPDATE_ANCHOR_POINTS_IN_REALTIME)!=0;
	if(update_anchor_points_in_realtime)
	{
		Update_Affected_Layers_With_Anchor_Points();
	}
}

HBRUSH AnchorPointsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1)
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
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void AnchorPointsDlg::OnCopyAnchorPoints() 
{
}

void AnchorPointsDlg::OnApplySavedAnchorPoints() 
{
}

void AnchorPointsDlg::OnClearSavedAnchorPoints() 
{
}

void AnchorPointsDlg::OnAttachAnchorPointToSelectedFeaturePoint() 
{
	int pos = m_AnchorRangeSlider.GetPos();
	Set_Default_Anchor_Influence_Range((float)pos);
	Attach_Anchor_Point_To_Selected_Feature_Point();
}

void AnchorPointsDlg::OnRemoveAnchorPointFromSelectedFeaturePoint() 
{
	Remove_Anchor_Point_From_Selected_Feature_Point();
}

void AnchorPointsDlg::OnRemoveAllAnchorPoints() 
{
	Remove_All_Anchor_Points();
}

bool Enable_Attach_Anchor_Point_Button(bool b)
{
	if(anchorPointsDlg)
	{
		anchorPointsDlg->m_AttachAnchorPointToSelectedFeaturePoint.EnableWindow(b);
		return true;
	}
	return false;
}

bool Enable_Remove_Anchor_Point_Button(bool b)
{
	if(anchorPointsDlg)
	{
		anchorPointsDlg->m_RemoveAnchorPointFromSelectedFeaturePoint.EnableWindow(b);
		return true;
	}
	return false;
}

bool Enable_Remove_All_Anchor_Points_Button(bool b)
{
	if(anchorPointsDlg)
	{
		anchorPointsDlg->m_RemoveAllAnchorPoints.EnableWindow(b);
		return true;
	}
	return false;
}


void AnchorPointsDlg::OnBnClickedButton1()
{
}
