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
// AlignmentOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "AlignmentOptionsDlg.h"
#include <math.h>


bool Project_Indexed_Layer_To_Plane(int index, float *pos, float *dir, float *rotation, float *offset, int origin_type, bool reproject);

extern bool render_2d_autoalignment_points;

extern bool activate_alignment_points;
extern bool deactivate_alignment_points;

bool Clear_Alignment_Points();

extern bool auto_apply_alignment_operation_to_other_frames;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AlignmentOptionsDlg dialog


AlignmentOptionsDlg::AlignmentOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AlignmentOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AlignmentOptionsDlg)
	//}}AFX_DATA_INIT
}


void AlignmentOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AlignmentOptionsDlg)
	DDX_Control(pDX, IDC_AUTO_APPLY_TO_OTHER_FRAMES, m_AutoApplyToOtherFrames);
	DDX_Control(pDX, IDC_APPLY_ALIGNMENT_TO_ALL_FRAMES, m_ApplyAlignmentToAllFrames);
	DDX_Control(pDX, IDC_ALIGN_PRIMITIVE, m_AlignPrimitive);
	DDX_Control(pDX, IDC_IDENTIFY_EDGE_PIXELS, m_IdentifyEdgePixels);
	DDX_Control(pDX, IDC_START_AUTO_ALIGNMENT, m_StartAlignment);
	DDX_Control(pDX, IDC_PLUS_ANGLE_Y, m_PlusY);
	DDX_Control(pDX, IDC_PLUS_ANGLE_X, m_PlusX);
	DDX_Control(pDX, IDC_MINUS_ANGLE_Y, m_MinusY);
	DDX_Control(pDX, IDC_MINUS_ANGLE_X, m_MinusX);
	DDX_Control(pDX, IDC_FORM_FIT_TO_SURROUNDINGS, m_FormFit);
	DDX_Control(pDX, IDC_ALIGN_YROT, m_AlignYRot);
	DDX_Control(pDX, IDC_ALIGN_XROT, m_AlignXRot);
	DDX_Control(pDX, IDC_ALIGN_PARALLEL, m_AlignParallel);
	DDX_Control(pDX, IDC_ALIGN_DEPTH, m_AlignDepth);
	DDX_Control(pDX, IDC_ALIGN_ANGLE, m_AlignAngle);
	DDX_Control(pDX, IDC_ALIGN_ALL, m_AlignAll);
	//}}AFX_DATA_MAP
	char text[32];
	sprintf(text, "%f", alignment_angle);
	SetDlgItemText(IDC_ALIGNMENT_ANGLE, text);
	m_IdentifyEdgePixels.SetFont(skin_font);

	m_ApplyAlignmentToAllFrames.SetFont(skin_font);
	m_AlignPrimitive.SetFont(skin_font);

	GetDlgItem(IDC_ALLOW_SCALE_X)->SetFont(skin_font);
	GetDlgItem(IDC_ALLOW_SCALE_Y)->SetFont(skin_font);
	GetDlgItem(IDC_ALLOW_SCALE_Z)->SetFont(skin_font);
	GetDlgItem(IDC_ALLOW_ROTATE_X)->SetFont(skin_font);
	GetDlgItem(IDC_ALLOW_ROTATE_Y)->SetFont(skin_font);
	GetDlgItem(IDC_ALLOW_ROTATE_Z)->SetFont(skin_font);
	GetDlgItem(IDC_ALLOW_MOVE_X)->SetFont(skin_font);
	GetDlgItem(IDC_ALLOW_MOVE_Y)->SetFont(skin_font);
	GetDlgItem(IDC_ALLOW_MOVE_Z)->SetFont(skin_font);
	
	GetDlgItem(IDC_STATIC9)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC10)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC11)->SetFont(skin_font);

	GetDlgItem(IDC_STATIC7)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC8)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_TOTAL_ERROR)->SetFont(skin_font);
	GetDlgItem(IDC_START_AUTO_ALIGNMENT)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGNMENT_ANGLE)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_DEPTH)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_XROT)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_YROT)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_ANGLE)->SetFont(skin_font);
	GetDlgItem(IDC_PLUS_ANGLE_X)->SetFont(skin_font);
	GetDlgItem(IDC_MINUS_ANGLE_X)->SetFont(skin_font);
	GetDlgItem(IDC_PLUS_ANGLE_Y)->SetFont(skin_font);
	GetDlgItem(IDC_MINUS_ANGLE_Y)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_ALL)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC3)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC4)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC5)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC6)->SetFont(skin_font);
	GetDlgItem(IDC_ALIGN_PARALLEL)->SetFont(skin_font);
	GetDlgItem(IDC_FORM_FIT_TO_SURROUNDINGS)->SetFont(skin_font);

	CheckDlgButton(IDC_ACTIVATE, activate_alignment_points);
	CheckDlgButton(IDC_DEACTIVATE, deactivate_alignment_points);

	CheckDlgButton(IDC_AUTO_APPLY_TO_OTHER_FRAMES, auto_apply_alignment_operation_to_other_frames);

}


BEGIN_MESSAGE_MAP(AlignmentOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(AlignmentOptionsDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ALIGN_DEPTH, OnAlignDepth)
	ON_BN_CLICKED(IDC_ALIGN_ANGLE, OnAlignAngle)
	ON_BN_CLICKED(IDC_ALIGN_XROT, OnAlignXrot)
	ON_BN_CLICKED(IDC_ALIGN_PARALLEL, OnAlignParallel)
	ON_BN_CLICKED(IDC_ALIGN_YROT, OnAlignYrot)
	ON_BN_CLICKED(IDC_ALIGN_ALL, OnAlignAll)
	ON_BN_CLICKED(IDC_START_AUTO_ALIGNMENT, OnStartAutoAlignment)
	ON_BN_CLICKED(IDC_PLUS_ANGLE_Y, OnPlusAngleY)
	ON_BN_CLICKED(IDC_MINUS_ANGLE_Y, OnMinusAngleY)
	ON_BN_CLICKED(IDC_PLUS_ANGLE_X, OnPlusAngleX)
	ON_BN_CLICKED(IDC_MINUS_ANGLE_X, OnMinusAngleX)
	ON_BN_CLICKED(IDC_FORM_FIT_TO_SURROUNDINGS, OnFormFitToSurroundings)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_IDENTIFY_EDGE_PIXELS, OnIdentifyEdgePixels)
	ON_BN_CLICKED(IDC_ACTIVATE, OnActivate)
	ON_BN_CLICKED(IDC_DEACTIVATE, OnDeactivate)
	ON_BN_CLICKED(IDC_ALLOW_SCALE_X, OnAllowScaleX)
	ON_BN_CLICKED(IDC_ALLOW_SCALE_Y, OnAllowScaleY)
	ON_BN_CLICKED(IDC_ALLOW_SCALE_Z, OnAllowScaleZ)
	ON_BN_CLICKED(IDC_ALLOW_FREE_MOTION, OnAllowFreeMotion)
	ON_BN_CLICKED(IDC_ALLOW_ROTATE_X, OnAllowRotateX)
	ON_BN_CLICKED(IDC_ALLOW_ROTATE_Y, OnAllowRotateY)
	ON_BN_CLICKED(IDC_ALLOW_ROTATE_Z, OnAllowRotateZ)
	ON_BN_CLICKED(IDC_ALIGN_PRIMITIVE, OnAlignPrimitive)
	ON_BN_CLICKED(IDC_ALLOW_MOVE_X, OnAllowMoveX)
	ON_BN_CLICKED(IDC_ALLOW_MOVE_Y, OnAllowMoveY)
	ON_BN_CLICKED(IDC_ALLOW_MOVE_Z, OnAllowMoveZ)
	ON_BN_CLICKED(IDC_APPLY_ALIGNMENT_TO_ALL_FRAMES, OnApplyAlignmentToAllFrames)
	ON_BN_CLICKED(IDC_AUTO_APPLY_TO_OTHER_FRAMES, OnAutoApplyToOtherFrames)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AlignmentOptionsDlg message handlers

void AlignmentOptionsDlg::OnAlignDepth() 
{
	align_depth = true;
	Start_Alignment();
}

void AlignmentOptionsDlg::OnAlignAngle() 
{
	if(!Verify_Alignment_Data())
	{
		return;
	}
	Update_Alignment_Angle();
	float xrot = fabs(alignment_neighbor_rotation[0]);
	float yrot = fabs(alignment_neighbor_rotation[1]);
	if(xrot<yrot)
	{
		Set_Initial_Relative_Alignment_Rotation(alignment_angle, 0);
		align_xrot = true;
	}
	else
	{
		Set_Initial_Relative_Alignment_Rotation(0, alignment_angle);
		align_yrot = true;
	}
	Start_Alignment();
}

void AlignmentOptionsDlg::OnAlignXrot() 
{
	align_yrot = true;
	Start_Alignment();
}

void AlignmentOptionsDlg::OnAlignParallel() 
{
	int mrs_index, os_index;
	int mrs = Get_Most_Recently_Selected_Layer(&mrs_index);
	int os = Get_Oldest_Selected_Layer(&os_index);
	if(mrs==-1||os==-1)return;
	if(mrs_index==-1||os_index==-1)return;
	Get_Indexed_Layer_Plane(os_index, alignment_neighbor_pos, alignment_neighbor_dir, alignment_neighbor_rotation, alignment_neighbor_offset, &alignment_neighbor_origin_type);
	Project_Indexed_Layer_To_Plane(mrs_index, alignment_neighbor_pos, alignment_neighbor_dir, alignment_neighbor_rotation, alignment_neighbor_offset, alignment_neighbor_origin_type, true);
	alignment_layer_index = mrs_index;
	alignment_plane_rotation[0] = alignment_neighbor_rotation[0];
	alignment_plane_rotation[1] = alignment_neighbor_rotation[1];
	alignment_plane_rotation[2] = alignment_neighbor_rotation[2];
	redraw_frame = true;
}

void AlignmentOptionsDlg::OnAlignYrot() 
{
	align_xrot = true;
	Start_Alignment();
}

void AlignmentOptionsDlg::OnAlignAll() 
{
	align_xrot = true;
	align_yrot = true;
	Start_Alignment();
}

void AlignmentOptionsDlg::OnStartAutoAlignment() 
{
	Stop_Alignment();
}

void AlignmentOptionsDlg::OnPlusAngleY() 
{
	Start_Vertical_Convex_Alignment();
}

void AlignmentOptionsDlg::OnMinusAngleY() 
{
	Start_Vertical_Concave_Alignment();
}

void AlignmentOptionsDlg::OnPlusAngleX() 
{
	Start_Horizontal_Convex_Alignment();
}

void AlignmentOptionsDlg::OnMinusAngleX() 
{
	Start_Horizontal_Concave_Alignment();
}

void AlignmentOptionsDlg::OnClose()
{
	ShowWindow(SW_HIDE);
	render_2d_autoalignment_points = false;
	activate_alignment_points = false;
	deactivate_alignment_points = false;
	Clear_Alignment_Points();
}

void AlignmentOptionsDlg::OnFormFitToSurroundings() 
{
	Form_Fit_To_Surroundings();
}

HBRUSH AlignmentOptionsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2||id==IDC_STATIC3||id==IDC_STATIC4||id==IDC_STATIC5||id==IDC_STATIC6||id==IDC_STATIC7||id==IDC_STATIC8||id==IDC_STATIC9||id==IDC_STATIC10||id==IDC_STATIC11||id==IDC_TOTAL_ERROR)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}
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

void AlignmentOptionsDlg::OnIdentifyEdgePixels() 
{
	if(Get_Alignment_Data())
	{
		render_2d_autoalignment_points = true;
		redraw_edit_window = true;
	}
}

void AlignmentOptionsDlg::OnActivate() 
{
	activate_alignment_points = IsDlgButtonChecked(IDC_ACTIVATE)!=0;
	if(deactivate_alignment_points)
	{
		deactivate_alignment_points = false;
	}
	CheckDlgButton(IDC_ACTIVATE, activate_alignment_points);
	CheckDlgButton(IDC_DEACTIVATE, deactivate_alignment_points);
	if(!activate_alignment_points&&!deactivate_alignment_points)redraw_edit_window = true;
}

void AlignmentOptionsDlg::OnDeactivate() 
{
	deactivate_alignment_points = IsDlgButtonChecked(IDC_DEACTIVATE)!=0;
	if(activate_alignment_points)
	{
		activate_alignment_points = false;
	}
	CheckDlgButton(IDC_ACTIVATE, activate_alignment_points);
	CheckDlgButton(IDC_DEACTIVATE, deactivate_alignment_points);
	if(!activate_alignment_points&&!deactivate_alignment_points)redraw_edit_window = true;
}

void AlignmentOptionsDlg::OnAllowScaleX() 
{
}

void AlignmentOptionsDlg::OnAllowScaleY() 
{
}

void AlignmentOptionsDlg::OnAllowScaleZ() 
{
}

void AlignmentOptionsDlg::OnAllowFreeMotion() 
{
}

void AlignmentOptionsDlg::OnAllowRotateX() 
{
}

void AlignmentOptionsDlg::OnAllowRotateY() 
{
}

void AlignmentOptionsDlg::OnAllowRotateZ() 
{
}

void AlignmentOptionsDlg::OnAllowMoveX() 
{
}

void AlignmentOptionsDlg::OnAllowMoveY() 
{
}

void AlignmentOptionsDlg::OnAllowMoveZ() 
{
}

void AlignmentOptionsDlg::OnAlignPrimitive()
{
	align_primitive = true;
	align_primitive_scale_x = IsDlgButtonChecked(IDC_ALLOW_SCALE_X)!=0;
	align_primitive_scale_y = IsDlgButtonChecked(IDC_ALLOW_SCALE_Y)!=0;
	align_primitive_scale_z = IsDlgButtonChecked(IDC_ALLOW_SCALE_Z)!=0;
	align_primitive_rot_x = IsDlgButtonChecked(IDC_ALLOW_ROTATE_X)!=0;
	align_primitive_rot_y = IsDlgButtonChecked(IDC_ALLOW_ROTATE_Y)!=0;
	align_primitive_rot_z = IsDlgButtonChecked(IDC_ALLOW_ROTATE_Z)!=0;
	align_primitive_move_x = IsDlgButtonChecked(IDC_ALLOW_MOVE_X)!=0;
	align_primitive_move_y = IsDlgButtonChecked(IDC_ALLOW_MOVE_Y)!=0;
	align_primitive_move_z = IsDlgButtonChecked(IDC_ALLOW_MOVE_Z)!=0;
	Start_Alignment();
}

void AlignmentOptionsDlg::OnApplyAlignmentToAllFrames() 
{
}

void AlignmentOptionsDlg::OnAutoApplyToOtherFrames() 
{
	auto_apply_alignment_operation_to_other_frames = IsDlgButtonChecked(IDC_AUTO_APPLY_TO_OTHER_FRAMES)!=0;
}
