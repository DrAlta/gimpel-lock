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
// VirtualCameraDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "VirtualCameraDlg.h"

extern bool select_tracking_feature_points;
extern bool unselect_tracking_feature_points;

extern bool render_camera_vectors_to_points;
extern bool render_camera_path;
extern bool render_camera_direction;

extern bool track_vc_yaw;
extern bool track_vc_pitch;

extern int num_camera_smooth_steps;

bool Smooth_Camera_Path();

bool Set_Current_Frame_As_Reference();
bool Toggle_Generate_Relative_Camera_Path();
bool Select_Camera_Path_Frame(int index);
bool Clear_Tracked_Feature_Points();
bool Reset_Virtual_Camera_Info();

bool Generate_Relative_Camera_Position();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VirtualCameraDlg dialog

bool Close_Virtual_Camera_Tool();

VirtualCameraDlg *virtualCameraDlg = 0;

VirtualCameraDlg::VirtualCameraDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VirtualCameraDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VirtualCameraDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void VirtualCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VirtualCameraDlg)
	DDX_Control(pDX, IDC_STATIC1, m_Static1);
	DDX_Control(pDX, IDC_SMOOTH_CAMERA_STEPS, m_SmoothCameraSteps);
	DDX_Control(pDX, IDC_SMOOTH_CAMERA_PATH, m_SmoothCameraPath);
	DDX_Control(pDX, IDC_RENDER_CAMERA_DIRECTION, m_RenderCameraDirection);
	DDX_Control(pDX, IDC_TRACK_YAW, m_TrackYaw);
	DDX_Control(pDX, IDC_TRACK_PITCH, m_TrackPitch);
	DDX_Control(pDX, IDC_GENERATE_RELATIVE_CAMERA_POSITION, m_GenerateRelativeCameraPosition);
	DDX_Control(pDX, IDC_RESET_VIRTUAL_CAMERA_INFO, m_ResetVirtualCameraInfo);
	DDX_Control(pDX, IDC_UNSELECT_TRACKING_FEATURE_POINTS, m_UnSelectTrackingFeaturePoints);
	DDX_Control(pDX, IDC_SELECT_TRACKING_FEATURE_POINTS, m_SelectTrackingFeaturePoints);
	DDX_Control(pDX, IDC_RENDER_CAMERA_VECTORS_TO_POINTS, m_RenderCameraVectorsToPoints);
	DDX_Control(pDX, IDC_RENDER_CAMERA_PATH, m_RenderCameraPath);
	DDX_Control(pDX, IDC_SET_CURRENT_FRAME_AS_REFERENCE, m_SetCurrentFrameAsReference);
	DDX_Control(pDX, IDC_GENERATE_RELATIVE_CAMERA_PATH, m_GenerateRelativeCameraPath);
	DDX_Control(pDX, IDC_CLEAR_TRACKED_FEATURE_POINTS, m_ClearTrackedFeaturePoints);
	DDX_Control(pDX, IDC_FRAMES_LIST, m_FramesList);
	//}}AFX_DATA_MAP

	m_Static1.SetFont(skin_font);
	m_SmoothCameraSteps.SetFont(skin_font);
	m_SmoothCameraPath.SetFont(skin_font);

	m_RenderCameraDirection.SetFont(skin_font);
	m_TrackPitch.SetFont(skin_font);
	m_TrackYaw.SetFont(skin_font);
	m_GenerateRelativeCameraPosition.SetFont(skin_font);
	m_ResetVirtualCameraInfo.SetFont(skin_font);
	m_UnSelectTrackingFeaturePoints.SetFont(skin_font);
	m_SelectTrackingFeaturePoints.SetFont(skin_font);
	m_RenderCameraVectorsToPoints.SetFont(skin_font);
	m_RenderCameraPath.SetFont(skin_font);
	m_SetCurrentFrameAsReference.SetFont(skin_font);
	m_GenerateRelativeCameraPath.SetFont(skin_font);
	m_ClearTrackedFeaturePoints.SetFont(skin_font);
	m_FramesList.SetFont(skin_font);
	CheckDlgButton(IDC_UNSELECT_TRACKING_FEATURE_POINTS, unselect_tracking_feature_points);
	CheckDlgButton(IDC_SELECT_TRACKING_FEATURE_POINTS, select_tracking_feature_points);
	CheckDlgButton(IDC_RENDER_CAMERA_VECTORS_TO_POINTS, render_camera_vectors_to_points);
	CheckDlgButton(IDC_RENDER_CAMERA_PATH, render_camera_path);
	CheckDlgButton(IDC_RENDER_CAMERA_DIRECTION, render_camera_direction);
	CheckDlgButton(IDC_TRACK_YAW, track_vc_yaw);
	CheckDlgButton(IDC_TRACK_PITCH, track_vc_pitch);
	ListFrames();

	char text[32];
	sprintf(text, "%i", num_camera_smooth_steps);
	m_SmoothCameraSteps.SetWindowText(text);
}


BEGIN_MESSAGE_MAP(VirtualCameraDlg, CDialog)
	//{{AFX_MSG_MAP(VirtualCameraDlg)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_SELECT_TRACKING_FEATURE_POINTS, OnSelectTrackingFeaturePoints)
	ON_BN_CLICKED(IDC_UNSELECT_TRACKING_FEATURE_POINTS, OnUnselectTrackingFeaturePoints)
	ON_BN_CLICKED(IDC_CLEAR_TRACKED_FEATURE_POINTS, OnClearTrackedFeaturePoints)
	ON_BN_CLICKED(IDC_RENDER_CAMERA_VECTORS_TO_POINTS, OnRenderCameraVectorsToPoints)
	ON_BN_CLICKED(IDC_RENDER_CAMERA_PATH, OnRenderCameraPath)
	ON_BN_CLICKED(IDC_SET_CURRENT_FRAME_AS_REFERENCE, OnSetCurrentFrameAsReference)
	ON_BN_CLICKED(IDC_GENERATE_RELATIVE_CAMERA_PATH, OnGenerateRelativeCameraPath)
	ON_LBN_SELCHANGE(IDC_FRAMES_LIST, OnSelchangeFramesList)
	ON_BN_CLICKED(IDC_RESET_VIRTUAL_CAMERA_INFO, OnResetVirtualCameraInfo)
	ON_BN_CLICKED(IDC_GENERATE_RELATIVE_CAMERA_POSITION, OnGenerateRelativeCameraPosition)
	ON_BN_CLICKED(IDC_TRACK_YAW, OnTrackYaw)
	ON_BN_CLICKED(IDC_TRACK_PITCH, OnTrackPitch)
	ON_BN_CLICKED(IDC_RENDER_CAMERA_DIRECTION, OnRenderCameraDirection)
	ON_BN_CLICKED(IDC_SMOOTH_CAMERA_PATH, OnSmoothCameraPath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VirtualCameraDlg message handlers


bool Open_Virtual_Camera_Dlg()
{
	if(!virtualCameraDlg)
	{
		virtualCameraDlg = new VirtualCameraDlg;
		virtualCameraDlg->Create(IDD_VIRTUAL_CAMERA_DLG);
		virtualCameraDlg->SetWindowText("Virtual Camera");
		Create_ToolSkin_Frame(virtualCameraDlg, "Virtual Camera", IDC_OPEN_VIRTUAL_CAMERA_TOOL);
	}
	virtualCameraDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_Virtual_Camera_Dlg()
{
	return true;
}

bool Select_VC_Dialog_Frame(int frame)
{
	if(!virtualCameraDlg)
	{
		return false;
	}
	virtualCameraDlg->m_FramesList.SetCurSel(frame);
	return true;
}


void VirtualCameraDlg::ListFrames()
{
	m_FramesList.ResetContent();
	int n = Get_Num_Frames();
	char name[512];
	for(int i = 0;i<n;i++)
	{
		Get_Frame_Name(i, name);
		Get_Display_Name(name);
		m_FramesList.AddString(name);
	}
}

void VirtualCameraDlg::OnClose() 
{
	Close_Virtual_Camera_Tool();
}

HBRUSH VirtualCameraDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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
}

void VirtualCameraDlg::OnSelectTrackingFeaturePoints() 
{
	bool b = IsDlgButtonChecked(IDC_SELECT_TRACKING_FEATURE_POINTS)!=0;
	if(b)
	{
		CheckDlgButton(IDC_UNSELECT_TRACKING_FEATURE_POINTS, false);
		unselect_tracking_feature_points = false;
	}
	select_tracking_feature_points = b;
}

void VirtualCameraDlg::OnUnselectTrackingFeaturePoints() 
{
	bool b = IsDlgButtonChecked(IDC_UNSELECT_TRACKING_FEATURE_POINTS)!=0;
	if(b)
	{
		CheckDlgButton(IDC_SELECT_TRACKING_FEATURE_POINTS, false);
		select_tracking_feature_points = false;
	}
	unselect_tracking_feature_points = b;
}

void VirtualCameraDlg::OnClearTrackedFeaturePoints() 
{
	Clear_Tracked_Feature_Points();
}

void VirtualCameraDlg::OnRenderCameraVectorsToPoints() 
{
	render_camera_vectors_to_points = IsDlgButtonChecked(IDC_RENDER_CAMERA_VECTORS_TO_POINTS)!=0;
	redraw_frame = true;
}

void VirtualCameraDlg::OnSetCurrentFrameAsReference() 
{
	Set_Current_Frame_As_Reference();
	redraw_frame = true;
}

void VirtualCameraDlg::OnGenerateRelativeCameraPath() 
{
	Toggle_Generate_Relative_Camera_Path();
	redraw_frame = true;
}

bool Set_Generate_Camera_Path_Button_Text(char *text)
{
	if(!virtualCameraDlg)return false;
	virtualCameraDlg->SetDlgItemText(IDC_GENERATE_RELATIVE_CAMERA_PATH, text);
	return true;
}


void VirtualCameraDlg::OnSelchangeFramesList() 
{
	int sel = m_FramesList.GetCurSel();
	Select_Camera_Path_Frame(sel);
	redraw_frame = true;
}

void VirtualCameraDlg::OnResetVirtualCameraInfo() 
{
	Reset_Virtual_Camera_Info();
	redraw_frame = true;
}

void VirtualCameraDlg::OnGenerateRelativeCameraPosition() 
{
	Generate_Relative_Camera_Position();
	redraw_frame = true;
}

void VirtualCameraDlg::OnTrackYaw() 
{
	track_vc_yaw = IsDlgButtonChecked(IDC_TRACK_YAW)!=0;
}

void VirtualCameraDlg::OnTrackPitch() 
{
	track_vc_pitch = IsDlgButtonChecked(IDC_TRACK_PITCH)!=0;
}

void VirtualCameraDlg::OnRenderCameraDirection() 
{
	render_camera_direction = IsDlgButtonChecked(IDC_RENDER_CAMERA_DIRECTION)!=0;
	redraw_frame = true;
}

void VirtualCameraDlg::OnRenderCameraPath() 
{
	render_camera_path = IsDlgButtonChecked(IDC_RENDER_CAMERA_PATH)!=0;
	redraw_frame = true;
}

void VirtualCameraDlg::OnSmoothCameraPath() 
{
	char text[32];
	m_SmoothCameraSteps.GetWindowText(text, 32);
	sscanf(text, "%i", &num_camera_smooth_steps);
	Smooth_Camera_Path();
}
