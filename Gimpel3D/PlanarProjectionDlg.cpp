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
// PlanarProjectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "PlanarProjectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PlanarProjectionDlg dialog

extern PlanarProjectionDlg *_planeDlg;

PlanarProjectionDlg::PlanarProjectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PlanarProjectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PlanarProjectionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	_planeDlg = this;
	saved_horizontal_rotation = 0;
	saved_vertical_rotation = 0;
	saved_z_rotation = 0;
}

const int degrees_slider_range = 3600;

void PlanarProjectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PlanarProjectionDlg)
	DDX_Control(pDX, IDC_CENTER_Z_ROTATION, m_CenterZRotation);
	DDX_Control(pDX, IDC_SET_Z_ROTATION, m_SetZRotation);
	DDX_Control(pDX, IDC_Z_ROTATION_SLIDER, m_ZRotationSlider);
	DDX_Control(pDX, IDC_SAVED_ORIENTATION, m_SavedOrientation);
	DDX_Control(pDX, IDC_APPLY_SAVED_ORIENTATION, m_ApplySavedOrientation);
	DDX_Control(pDX, IDC_SAVE_CURRENT_ORIENTATION, m_SaveCurrentOrientation);
	DDX_Control(pDX, IDC_CENTER_VERTICAL_ROTATION, m_CenterVerticalRotation);
	DDX_Control(pDX, IDC_CENTER_HORIZONTAL_ROTATION, m_CenterHorizontalRotation);
	DDX_Control(pDX, IDC_SET_VERTICAL_ROTATION, m_SetVerticalRotation);
	DDX_Control(pDX, IDC_SET_HORIZONTAL_ROTATION, m_SetHorizontalRotation);
	DDX_Control(pDX, IDC_VERTICAL_ROTATION_SLIDER, m_VerticalRotationSlider);
	DDX_Control(pDX, IDC_HORIZONTAL_ROTATION_SLIDER, m_HorizontalRotationSlider);
	//}}AFX_DATA_MAP
	char text[32];
	sprintf(text, "%f", horizontal_rotation);SetDlgItemText(IDC_HORIZONTAL_ROTATION, text);
	sprintf(text, "%f", vertical_rotation);SetDlgItemText(IDC_VERTICAL_ROTATION, text);
	sprintf(text, "%f", z_rotation);SetDlgItemText(IDC_Z_ROTATION, text);
	sprintf(text, "%f", horizontal_position);SetDlgItemText(IDC_HORIZONTAL_POSITION, text);
	sprintf(text, "%f", vertical_position);SetDlgItemText(IDC_VERTICAL_POSITION, text);
	sprintf(text, "%f", depth_position);SetDlgItemText(IDC_DEPTH_POSITION, text);


	m_VerticalRotationSlider.SetRange(0, degrees_slider_range);
	m_HorizontalRotationSlider.SetRange(0, degrees_slider_range);
	
	m_ZRotationSlider.SetRange(0, degrees_slider_range);

	int center = 500;
	m_VerticalRotationSlider.SetPos(center);
	m_HorizontalRotationSlider.SetPos(center);
	m_ZRotationSlider.SetPos(center);

	sprintf(text, "%f  %f  %f", saved_horizontal_rotation, saved_z_rotation, saved_vertical_rotation);
	m_SavedOrientation.SetWindowText(text);
	m_SavedOrientation.SetFont(skin_font);
	m_ApplySavedOrientation.SetFont(skin_font);
	m_SaveCurrentOrientation.SetFont(skin_font);

	m_CenterZRotation.SetFont(skin_font);
	m_SetZRotation.SetFont(skin_font);

	CheckDlgButton(IDC_LOCK_CENTER, true);

	GetDlgItem(IDC_LOCK_TOP)->SetFont(skin_font);
	GetDlgItem(IDC_LOCK_LEFT)->SetFont(skin_font);
	GetDlgItem(IDC_LOCK_CENTER)->SetFont(skin_font);
	GetDlgItem(IDC_LOCK_RIGHT)->SetFont(skin_font);
	GetDlgItem(IDC_LOCK_BOTTOM)->SetFont(skin_font);
	GetDlgItem(IDC_HORIZONTAL_ROTATION)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_VERTICAL_ROTATION)->SetFont(skin_font);
	GetDlgItem(IDC_Z_ROTATION)->SetFont(skin_font);
	GetDlgItem(IDC_SET_HORIZONTAL_ROTATION)->SetFont(skin_font);
	GetDlgItem(IDC_SET_VERTICAL_ROTATION)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC3)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC4)->SetFont(skin_font);
	GetDlgItem(IDC_CENTER_HORIZONTAL_ROTATION)->SetFont(skin_font);
	GetDlgItem(IDC_CENTER_VERTICAL_ROTATION)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(PlanarProjectionDlg, CDialog)
	//{{AFX_MSG_MAP(PlanarProjectionDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SET_HORIZONTAL_ROTATION, OnSetHorizontalRotation)
	ON_BN_CLICKED(IDC_CENTER_HORIZONTAL_ROTATION, OnCenterHorizontalRotation)
	ON_BN_CLICKED(IDC_SET_VERTICAL_ROTATION, OnSetVerticalRotation)
	ON_BN_CLICKED(IDC_CENTER_VERTICAL_ROTATION, OnCenterVerticalRotation)
	ON_BN_CLICKED(IDC_LOCK_TOP, OnLockTop)
	ON_BN_CLICKED(IDC_LOCK_BOTTOM, OnLockBottom)
	ON_BN_CLICKED(IDC_LOCK_LEFT, OnLockLeft)
	ON_BN_CLICKED(IDC_LOCK_RIGHT, OnLockRight)
	ON_BN_CLICKED(IDC_RESET_TO_SELECTION, OnResetToSelection)
	ON_BN_CLICKED(IDC_APPLY_TO_SELECTION, OnApplyToSelection)
	ON_BN_CLICKED(IDC_SET_HORIZONTAL_POSITION, OnSetHorizontalPosition)
	ON_BN_CLICKED(IDC_CENTER_HORIZONTAL_POSITION, OnCenterHorizontalPosition)
	ON_BN_CLICKED(IDC_SET_VERTICAL_POSITION, OnSetVerticalPosition)
	ON_BN_CLICKED(IDC_CENTER_VERTICAL_POSITION, OnCenterVerticalPosition)
	ON_BN_CLICKED(IDC_SET_DEPTH_POSITION, OnSetDepthPosition)
	ON_BN_CLICKED(IDC_CENTER_DEPTH_POSITION, OnCenterDepthPosition)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_LOCK_CENTER, OnLockCenter)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_SAVE_CURRENT_ORIENTATION, OnSaveCurrentOrientation)
	ON_BN_CLICKED(IDC_APPLY_SAVED_ORIENTATION, OnApplySavedOrientation)
	ON_BN_CLICKED(IDC_SET_Z_ROTATION, OnSetZRotation)
	ON_BN_CLICKED(IDC_CENTER_Z_ROTATION, OnCenterZRotation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PlanarProjectionDlg message handlers

void PlanarProjectionDlg::OnSetHorizontalRotation() 
{
	char text[32];
	GetDlgItemText(IDC_HORIZONTAL_ROTATION, text, 32);
	sscanf(text, "%f", &horizontal_rotation);
	UpdateSliderPositions();
	Finalize_Selection_Planar_Projection();
	redraw_frame = true;
}

void PlanarProjectionDlg::CenterSliders()
{
	horizontal_rotation = 0;
	vertical_rotation = 0;
	z_rotation = 0;
	UpdateSliderPositions();
	Finalize_Selection_Planar_Projection();
	char text[32];
	sprintf(text, "%f", horizontal_rotation);
	SetDlgItemText(IDC_HORIZONTAL_ROTATION, text);
	sprintf(text, "%f", vertical_rotation);
	SetDlgItemText(IDC_VERTICAL_ROTATION, text);
	sprintf(text, "%f", z_rotation);
	SetDlgItemText(IDC_Z_ROTATION, text);
	redraw_frame = true;
}


void PlanarProjectionDlg::OnCenterHorizontalRotation() 
{
	horizontal_rotation = 0;
	UpdateSliderPositions();
	Finalize_Selection_Planar_Projection();
	char text[32];
	sprintf(text, "%f", horizontal_rotation);
	SetDlgItemText(IDC_HORIZONTAL_ROTATION, text);
	redraw_frame = true;
}

void PlanarProjectionDlg::OnSetVerticalRotation() 
{
	char text[32];
	GetDlgItemText(IDC_VERTICAL_ROTATION, text, 32);
	sscanf(text, "%f", &vertical_rotation);
	UpdateSliderPositions();
	Finalize_Selection_Planar_Projection();
	redraw_frame = true;
}

void PlanarProjectionDlg::OnCenterVerticalRotation() 
{
	vertical_rotation = 0;
	UpdateSliderPositions();
	Finalize_Selection_Planar_Projection();
	char text[32];
	sprintf(text, "%f", vertical_rotation);
	SetDlgItemText(IDC_VERTICAL_ROTATION, text);
	redraw_frame = true;
}


void PlanarProjectionDlg::OnLockTop() 
{
	Set_Selection_Plane_Origin(PLANE_ORIGIN_SELECTION_TOP);
	redraw_frame = true;
}

void PlanarProjectionDlg::OnLockBottom() 
{
	Set_Selection_Plane_Origin(PLANE_ORIGIN_SELECTION_BOTTOM);
	redraw_frame = true;
}

void PlanarProjectionDlg::OnLockLeft() 
{
	Set_Selection_Plane_Origin(PLANE_ORIGIN_SELECTION_LEFT);
	redraw_frame = true;
}

void PlanarProjectionDlg::OnLockRight() 
{
	Set_Selection_Plane_Origin(PLANE_ORIGIN_SELECTION_RIGHT);
	redraw_frame = true;
}

void PlanarProjectionDlg::OnLockCenter() 
{
	Set_Selection_Plane_Origin(PLANE_ORIGIN_SELECTION_CENTER);
	redraw_frame = true;
}

void PlanarProjectionDlg::OnResetToSelection() 
{
}

void PlanarProjectionDlg::OnApplyToSelection() 
{
}

void PlanarProjectionDlg::OnSetHorizontalPosition() 
{
}

void PlanarProjectionDlg::OnCenterHorizontalPosition() 
{
}

void PlanarProjectionDlg::OnSetVerticalPosition() 
{
}

void PlanarProjectionDlg::OnCenterVerticalPosition() 
{
}

void PlanarProjectionDlg::OnSetDepthPosition() 
{
}

void PlanarProjectionDlg::OnCenterDepthPosition() 
{
}

void PlanarProjectionDlg::UpdateSliderPositions()
{
	float vrp = (((vertical_rotation-min_vr)/(max_vr-min_vr))*degrees_slider_range);
	float hrp = (((horizontal_rotation-min_hr)/(max_hr-min_hr))*degrees_slider_range);
	float zrp = (((z_rotation-min_zr)/(max_zr-min_zr))*degrees_slider_range);
	m_VerticalRotationSlider.SetPos((int)vrp);
	m_HorizontalRotationSlider.SetPos((int)hrp);
	m_ZRotationSlider.SetPos((int)zrp);
}

float Fint(float v)//rounds off floats to closest integer
{
	bool neg = v<0;
	if(neg)v = -v;
	float di = v;
	float r = v-di;
	if(r>=0.5f){di+=1;}
	if(neg){di=-di;}
	return di;
}

float Clamp_Degrees(float v)
{
	float nv = v*10;
	nv = Fint(nv);
	return nv/10;
}

void PlanarProjectionDlg::UpdateSliders()
{
	static float last_vr = 0;
	static float last_hr = 0;
	static float last_zr = 0;
	float vr = ((float)((m_VerticalRotationSlider.GetPos()))/degrees_slider_range);
	float hr = ((float)((m_HorizontalRotationSlider.GetPos()))/degrees_slider_range);
	float zr = ((float)((m_ZRotationSlider.GetPos()))/degrees_slider_range);
	char text[32];
	if(Num_Selected_Layers()==0)
	{
		horizontal_rotation = 0;
		vertical_rotation = 0;
		z_rotation = 0;
		SetDlgItemText(IDC_VERTICAL_ROTATION, "0.000000");
		SetDlgItemText(IDC_HORIZONTAL_ROTATION, "0.000000");
		SetDlgItemText(IDC_Z_ROTATION, "0.000000");
		m_VerticalRotationSlider.SetPos(degrees_slider_range/2);
		m_HorizontalRotationSlider.SetPos(degrees_slider_range/2);
		m_ZRotationSlider.SetPos(degrees_slider_range/2);
	}
	else
	{
		if(last_vr!=vr)
		{
			vertical_rotation = Clamp_Degrees(min_vr + ((max_vr-min_vr)*vr));
			last_vr = vr;
			sprintf(text, "%f", vertical_rotation);SetDlgItemText(IDC_VERTICAL_ROTATION, text);
		}
		if(last_hr!=hr)
		{
			horizontal_rotation = Clamp_Degrees(min_hr + ((max_hr-min_hr)*hr));
			last_hr = hr;
			sprintf(text, "%f", horizontal_rotation);SetDlgItemText(IDC_HORIZONTAL_ROTATION, text);
		}
		if(last_zr!=zr)
		{
			z_rotation = Clamp_Degrees(min_zr + ((max_zr-min_zr)*zr));
			last_zr = zr;
			sprintf(text, "%f", z_rotation);SetDlgItemText(IDC_Z_ROTATION, text);
		}
	}
	Finalize_Selection_Planar_Projection();
	redraw_frame = true;
}

void PlanarProjectionDlg::OnClose() 
{
	ShowWindow(SW_HIDE);
	is_visible = false;
	redraw_frame = true;
}

void PlanarProjectionDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	UpdateSliders();
}


HBRUSH PlanarProjectionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2||id==IDC_STATIC3||id==IDC_STATIC4||id==IDC_SAVED_ORIENTATION)
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

void PlanarProjectionDlg::OnSaveCurrentOrientation() 
{
	saved_horizontal_rotation = horizontal_rotation;
	saved_vertical_rotation = vertical_rotation;
	saved_z_rotation = z_rotation;
	char text[512];
	sprintf(text, "%f  %f  %f", saved_horizontal_rotation, saved_z_rotation, saved_vertical_rotation);
	m_SavedOrientation.SetWindowText(text);
}

void PlanarProjectionDlg::OnApplySavedOrientation() 
{
	horizontal_rotation = saved_horizontal_rotation;
	vertical_rotation = saved_vertical_rotation;
	z_rotation = saved_z_rotation;
	char text[32];
	sprintf(text, "%f", horizontal_rotation);
	SetDlgItemText(IDC_HORIZONTAL_ROTATION, text);
	sprintf(text, "%f", vertical_rotation);
	SetDlgItemText(IDC_VERTICAL_ROTATION, text);
	sprintf(text, "%f", z_rotation);
	SetDlgItemText(IDC_Z_ROTATION, text);
	UpdateSliderPositions();
	Finalize_Selection_Planar_Projection();
	redraw_frame = true;
}


void PlanarProjectionDlg::OnSetZRotation() 
{
	char text[32];
	GetDlgItemText(IDC_Z_ROTATION, text, 32);
	sscanf(text, "%f", &z_rotation);
	UpdateSliderPositions();
	Finalize_Selection_Planar_Projection();
	redraw_frame = true;
}

void PlanarProjectionDlg::OnCenterZRotation() 
{
	z_rotation = 0;
	UpdateSliderPositions();
	Finalize_Selection_Planar_Projection();
	char text[32];
	sprintf(text, "%f", z_rotation);
	SetDlgItemText(IDC_Z_ROTATION, text);
	redraw_frame = true;
}
