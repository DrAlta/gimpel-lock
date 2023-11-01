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
// StereoSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "StereoSettingsDlg.h"
#include "G3DCoreFiles/StereoView.h"

extern bool render_camera;
bool ReCalc_Focal_Plane_Preview();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// StereoSettingsDlg dialog


StereoSettingsDlg::StereoSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(StereoSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(StereoSettingsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void StereoSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StereoSettingsDlg)
	DDX_Control(pDX, IDC_APPLY_STEREO_SETTINGS_TO_ALL, m_ApplyToAll);
	DDX_Control(pDX, IDC_RESET_STEREO, m_ResetStereo);
	DDX_Control(pDX, IDC_RESET_DEPTH_SCALE, m_ResetDepthScale);
	DDX_Control(pDX, IDC_DEPTH_SCALE_SLIDER, m_DepthScaleSlider);
	DDX_Control(pDX, IDC_EYE_SEPARATION_SLIDER, m_EyeSeparationSlider);
	DDX_Control(pDX, IDC_FOCAL_LENGTH_SLIDER, m_FocalLengthSlider);
	//}}AFX_DATA_MAP
	m_EyeSeparationSlider.SetRange(1, 1000);
	m_FocalLengthSlider.SetRange(1, 1000);
	m_DepthScaleSlider.SetRange(1, 1000);

	char text[32];
	
	sprintf(text, "%.5f", min_es*1000);SetDlgItemText(IDC_MIN_EYE_SEPARATION, text);//"0.01");
	sprintf(text, "%.5f", max_es*1000);SetDlgItemText(IDC_MAX_EYE_SEPARATION, text);//"0");
	sprintf(text, "%.5f", min_fl*1000);SetDlgItemText(IDC_MIN_FOCAL_LENGTH, text);//"0.01");
	sprintf(text, "%.5f", max_fl*1000);SetDlgItemText(IDC_MAX_FOCAL_LENGTH, text);//"0.1");
	sprintf(text, "%.5f", min_ds);SetDlgItemText(IDC_MIN_DEPTH_SCALE, text);//"0");
	sprintf(text, "%.5f", max_ds);SetDlgItemText(IDC_MAX_DEPTH_SCALE, text);//"2");
	UpdateSliderPositions();
	UpdateSliders();
	int frame_index = Get_Current_Project_Frame();
	if(frame_index==0||frame_index==Get_Num_Frames()-1)
	{
		GetDlgItem(IDC_KEYFRAME_STEREO)->EnableWindow(false);
	}
	else
	{
		GetDlgItem(IDC_KEYFRAME_STEREO)->EnableWindow(true);
	}
	if((int)stereo_frame_settings.size()>frame_index)
	{
		CheckDlgButton(IDC_KEYFRAME_STEREO, stereo_frame_settings[frame_index].keyframe);
	}

	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_EYE_SEPARATION_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_FOCAL_LENGTH_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_EYE_SEPARATION)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC3)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC4)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC5)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC6)->SetFont(skin_font);
	GetDlgItem(IDC_FOCAL_LENGTH)->SetFont(skin_font);
	GetDlgItem(IDC_MIN_EYE_SEPARATION)->SetFont(skin_font);
	GetDlgItem(IDC_MAX_EYE_SEPARATION)->SetFont(skin_font);
	GetDlgItem(IDC_MIN_FOCAL_LENGTH)->SetFont(skin_font);
	GetDlgItem(IDC_MAX_FOCAL_LENGTH)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC7)->SetFont(skin_font);
	GetDlgItem(IDC_DEPTH_SCALE_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC8)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC9)->SetFont(skin_font);
	GetDlgItem(IDC_MIN_DEPTH_SCALE)->SetFont(skin_font);
	GetDlgItem(IDC_MAX_DEPTH_SCALE)->SetFont(skin_font);
	GetDlgItem(IDC_RESET_DEPTH_SCALE)->SetFont(skin_font);
	GetDlgItem(IDC_KEYFRAME_STEREO)->SetFont(skin_font);
	GetDlgItem(IDC_APPLY_STEREO_SETTINGS_TO_ALL)->SetFont(skin_font);
	GetDlgItem(IDC_RESET_STEREO)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(StereoSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(StereoSettingsDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RESET_DEPTH_SCALE, OnResetDepthScale)
	ON_BN_CLICKED(IDC_KEYFRAME_STEREO, OnKeyframeStereo)
	ON_BN_CLICKED(IDC_APPLY_STEREO_SETTINGS_TO_ALL, OnApplyStereoSettingsToAll)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_RESET_STEREO, OnResetStereo)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StereoSettingsDlg message handlers

void StereoSettingsDlg::OnResetDepthScale() 
{
	Reset_Depth_Scale();
	m_DepthScaleSlider.SetPos(500);
	redraw_frame = true;
}

void StereoSettingsDlg::OnKeyframeStereo() 
{
	if(Get_Num_Frames()==0)
	{
		return;
	}
	int index = Get_Current_Project_Frame();
	stereo_frame_settings[index].keyframe = IsDlgButtonChecked(IDC_KEYFRAME_STEREO)!=0;
}

void StereoSettingsDlg::OnApplyStereoSettingsToAll() 
{
	int n = stereo_frame_settings.size();
	int index = Get_Current_Project_Frame();
	if(index<0||index>=n)
	{
		return;
	}
	STEREO_SETTINGS *ss = &stereo_frame_settings[index];
	for(int i = 0;i<n;i++)
	{
		stereo_frame_settings[i].eye_separation = ss->eye_separation;
		stereo_frame_settings[i].focal_length = ss->focal_length;
		stereo_frame_settings[i].depth_scale = ss->depth_scale;
	}
}

void StereoSettingsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	UpdateSliders();
	if(frame_loaded)session_altered = true;
}

void StereoSettingsDlg::UpdateSliderPositions()
{
	float es = ((eye_separation-min_es)/(max_es-min_es))*1000;
	float fl = ((focal_length-min_fl)/(max_fl-min_fl))*1000;
	float ds = ((depth_scale-min_ds)/(max_ds-min_ds))*1000;
	m_EyeSeparationSlider.SetPos((int)es);
	m_FocalLengthSlider.SetPos((int)fl);
	m_DepthScaleSlider.SetPos((int)ds);
	char text[32];
	sprintf(text, "%.5f", eye_separation*1000);SetDlgItemText(IDC_EYE_SEPARATION, text);
	sprintf(text, "%.5f", focal_length*1000);SetDlgItemText(IDC_FOCAL_LENGTH, text);
}

void StereoSettingsDlg::UpdateSliders()
{
	float esp = (float)(m_EyeSeparationSlider.GetPos());
	float flp = (float)(m_FocalLengthSlider.GetPos());
	float dsp = (float)(m_DepthScaleSlider.GetPos());
	char text[32];
	GetDlgItemText(IDC_MIN_DEPTH_SCALE, text, 32);sscanf(text, "%f", &min_ds);
	GetDlgItemText(IDC_MAX_DEPTH_SCALE, text, 32);sscanf(text, "%f", &max_ds);
	GetDlgItemText(IDC_MIN_EYE_SEPARATION, text, 32);sscanf(text, "%f", &min_es);min_es = min_es/1000;
	GetDlgItemText(IDC_MIN_FOCAL_LENGTH, text, 32);sscanf(text, "%f", &min_fl);min_fl = min_fl/1000;
	GetDlgItemText(IDC_MAX_EYE_SEPARATION, text, 32);sscanf(text, "%f", &max_es);max_es = max_es/1000;
	GetDlgItemText(IDC_MAX_FOCAL_LENGTH, text, 32);sscanf(text, "%f", &max_fl);max_fl = max_fl/1000;
	float es = min_es+((esp/1000)*(max_es-min_es));
	float fl = min_fl+((flp/1000)*(max_fl-min_fl));
	float ds = min_ds+((dsp/1000)*(max_ds-min_ds));
	sprintf(text, "%.5f", es*1000);SetDlgItemText(IDC_EYE_SEPARATION, text);
	sprintf(text, "%.5f", fl*1000);SetDlgItemText(IDC_FOCAL_LENGTH, text);
	
	Set_Eye_Separation(es);
	Set_Focal_Length(fl);
	Set_Depth_Scale(ds);
	redraw_frame = true;
	if(Get_Num_Frames()==0)
	{
		return;
	}
	int index = Get_Current_Project_Frame();
	stereo_frame_settings[index].eye_separation = eye_separation;
	stereo_frame_settings[index].focal_length = focal_length;
	stereo_frame_settings[index].depth_scale = depth_scale;
}

void StereoSettingsDlg::OnClose() 
{
	ShowWindow(SW_HIDE);
}

void StereoSettingsDlg::OnResetStereo() 
{
	Reset_Stereo();
	Update_Stereo_Sliders();
}

void StereoSettingsDlg::OnRenderFocalPlane() 
{
}

HBRUSH StereoSettingsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2||id==IDC_STATIC3||id==IDC_STATIC4||id==IDC_STATIC5||id==IDC_STATIC6||id==IDC_STATIC7||id==IDC_STATIC8||id==IDC_STATIC9)
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
