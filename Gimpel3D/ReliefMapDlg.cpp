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
// ReliefMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "ReliefMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ReliefMapDlg dialog

extern ReliefMapDlg *_reliefMapDlg;

ReliefMapDlg::ReliefMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ReliefMapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ReliefMapDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ReliefMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReliefMapDlg)
	DDX_Control(pDX, IDC_SPLIT_LAYER, m_SplitLayer);
	DDX_Control(pDX, IDC_SET_TILING_SLIDER_RANGE, m_SetTilingSliderRange);
	DDX_Control(pDX, IDC_SET_TILING, m_SetTiling);
	DDX_Control(pDX, IDC_SET_SCALE_SLIDER_RANGE, m_SetScaleSliderRange);
	DDX_Control(pDX, IDC_SET_SCALE, m_SetScale);
	DDX_Control(pDX, IDC_SET_CONTRAST_SLIDER_RANGE, m_SetContrastSliderRange);
	DDX_Control(pDX, IDC_SET_CONTRAST, m_SetContrast);
	DDX_Control(pDX, IDC_SET_BIAS, m_SetBias);
	DDX_Control(pDX, IDC_RESET_BIAS, m_ResetBias);
	DDX_Control(pDX, IDC_MFC_SELECT_CONVEX_COLOR, m_MFCSelectConvexColor);
	DDX_Control(pDX, IDC_MFC_SELECT_CONCAVE_COLOR, m_MFCSelectConcaveColor);
	DDX_Control(pDX, IDC_FLIP_SCALE_SLIDER_RANGE, m_FlipScaleSlider);
	DDX_Control(pDX, IDC_CHANGE_IMAGE, m_ChangeImage);
	DDX_Control(pDX, IDC_BIAS_SLIDER, m_BiasSlider);
	DDX_Control(pDX, IDC_SCALE_SLIDER, m_ScaleSlider);
	DDX_Control(pDX, IDC_TILING_SLIDER, m_TilingSlider);
	DDX_Control(pDX, IDC_CONTRAST_SLIDER, m_ContrastSlider);
	DDX_Control(pDX, IDC_CONVEX_COLOR_BUTTON, m_ConvexColorButton);
	DDX_Control(pDX, IDC_CONCAVE_COLOR_BUTTON, m_ConcaveColorButton);
	//}}AFX_DATA_MAP
	m_ContrastSlider.SetRange(0, 1000);
	m_TilingSlider.SetRange(0, 1000);
	m_ScaleSlider.SetRange(0, 1000);
	m_BiasSlider.SetRange(0, 1000);
	Set_Default_Relief_Info(&relief_info);
	SetReliefInfo(&relief_info);

	GetDlgItem(IDC_RANDOM_NOISE)->SetFont(skin_font);
	GetDlgItem(IDC_RELIEF_IMAGE)->SetFont(skin_font);
	GetDlgItem(IDC_COLOR_DIFFERENCE)->SetFont(skin_font);
	GetDlgItem(IDC_SCALE_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_SCALE_LABEL)->SetFont(skin_font);
	GetDlgItem(IDC_SCALE)->SetFont(skin_font);
	GetDlgItem(IDC_SET_SCALE)->SetFont(skin_font);
	GetDlgItem(IDC_SLIDER_RANGE_LABEL)->SetFont(skin_font);
	GetDlgItem(IDC_SCALE_SLIDER_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_SET_SCALE_SLIDER_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_CONCAVE_COLOR_LABEL)->SetFont(skin_font);
	GetDlgItem(IDC_CONVEX_COLOR_LABEL)->SetFont(skin_font);
	GetDlgItem(IDC_SELECT_CONCAVE_COLOR)->SetFont(skin_font);
	GetDlgItem(IDC_SELECT_CONVEX_COLOR)->SetFont(skin_font);
	GetDlgItem(IDC_BIAS_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_DIFFERENCE_BIAS_LABEL)->SetFont(skin_font);
	GetDlgItem(IDC_BIAS)->SetFont(skin_font);
	GetDlgItem(IDC_SET_BIAS)->SetFont(skin_font);
	GetDlgItem(IDC_SPLIT_LAYER)->SetFont(skin_font);
	GetDlgItem(IDC_TILING_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_TILING_LABEL)->SetFont(skin_font);
	GetDlgItem(IDC_TILING)->SetFont(skin_font);
	GetDlgItem(IDC_SET_TILING)->SetFont(skin_font);
	GetDlgItem(IDC_TILING_SLIDER_RANGE_LABEL)->SetFont(skin_font);
	GetDlgItem(IDC_TILING_SLIDER_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_SET_TILING_SLIDER_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_CONTRAST_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_CONTRAST_LABEL)->SetFont(skin_font);
	GetDlgItem(IDC_CONTRAST)->SetFont(skin_font);
	GetDlgItem(IDC_SET_CONTRAST)->SetFont(skin_font);
	GetDlgItem(IDC_CONTRAST_SLIDER_RANGE_LABEL)->SetFont(skin_font);
	GetDlgItem(IDC_CONTRAST_SLIDER_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_SET_CONTRAST_SLIDER_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_IMAGE_LABEL)->SetFont(skin_font);
	GetDlgItem(IDC_IMAGE_FILE)->SetFont(skin_font);
	GetDlgItem(IDC_CHANGE_IMAGE)->SetFont(skin_font);
	GetDlgItem(IDC_CONCAVE_COLOR_BUTTON)->SetFont(skin_font);
	GetDlgItem(IDC_CONVEX_COLOR_BUTTON)->SetFont(skin_font);
	GetDlgItem(IDC_RESET_BIAS)->SetFont(skin_font);
	GetDlgItem(IDC_PICK_CONCAVE_COLOR)->SetFont(skin_font);
	GetDlgItem(IDC_PICK_CONVEX_COLOR)->SetFont(skin_font);
	GetDlgItem(IDC_BRIGHTNESS_ONLY)->SetFont(skin_font);
	GetDlgItem(IDC_SMOOTH_TRANSITION)->SetFont(skin_font);
	GetDlgItem(IDC_MFC_SELECT_CONCAVE_COLOR)->SetFont(skin_font);
	GetDlgItem(IDC_MFC_SELECT_CONVEX_COLOR)->SetFont(skin_font);
	GetDlgItem(IDC_FLIP_SCALE_SLIDER_RANGE)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(ReliefMapDlg, CDialog)
	//{{AFX_MSG_MAP(ReliefMapDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RANDOM_NOISE, OnRandomNoise)
	ON_BN_CLICKED(IDC_RELIEF_IMAGE, OnReliefImage)
	ON_BN_CLICKED(IDC_COLOR_DIFFERENCE, OnColorDifference)
	ON_BN_CLICKED(IDC_SET_SCALE, OnSetScale)
	ON_BN_CLICKED(IDC_FLIP_SCALE_SLIDER_RANGE, OnFlipScaleSliderRange)
	ON_BN_CLICKED(IDC_SET_SCALE_SLIDER_RANGE, OnSetScaleSliderRange)
	ON_BN_CLICKED(IDC_PICK_CONCAVE_COLOR, OnPickConcaveColor)
	ON_BN_CLICKED(IDC_PICK_CONVEX_COLOR, OnPickConvexColor)
	ON_BN_CLICKED(IDC_SPLIT_LAYER, OnSplitLayer)
	ON_BN_CLICKED(IDC_SET_BIAS, OnSetBias)
	ON_BN_CLICKED(IDC_RESET_BIAS, OnResetBias)
	ON_BN_CLICKED(IDC_BRIGHTNESS_ONLY, OnBrightnessOnly)
	ON_BN_CLICKED(IDC_SET_TILING_SLIDER_RANGE, OnSetTilingSliderRange)
	ON_BN_CLICKED(IDC_SET_CONTRAST_SLIDER_RANGE, OnSetContrastSliderRange)
	ON_BN_CLICKED(IDC_MFC_SELECT_CONCAVE_COLOR, OnMfcSelectConcaveColor)
	ON_BN_CLICKED(IDC_MFC_SELECT_CONVEX_COLOR, OnMfcSelectConvexColor)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_SMOOTH_TRANSITION, OnSmoothTransition)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CHANGE_IMAGE, OnChangeImage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ReliefMapDlg message handlers

void ReliefMapDlg::OnRandomNoise() 
{
	if(relief_info.type==RELIEF_TYPE_RANDOM_NOISE)return;
	relief_info.type = RELIEF_TYPE_RANDOM_NOISE;
	EnableControls();
	Update_Selection_Relief(true);
}

void ReliefMapDlg::OnReliefImage() 
{
	if(relief_info.type==RELIEF_TYPE_IMAGE)return;
	relief_info.type = RELIEF_TYPE_IMAGE;
	EnableControls();
	Update_Selection_Relief(true);
}

void ReliefMapDlg::OnColorDifference() 
{
	//bogus windows message sent to dialog?
	if(IsDlgButtonChecked(IDC_COLOR_DIFFERENCE)!=0)
	{
		OnBrightnessOnly();
		EnableControls();
	}
	else
	{
	}
}

void ReliefMapDlg::OnSetScale() 
{
	char text[32];
	GetDlgItemText(IDC_SCALE, text, 32);
	sscanf(text, "%f", &relief_info.scale);
	SetReliefInfo(&relief_info);
	Update_Selection_Relief(true);
}

void ReliefMapDlg::OnFlipScaleSliderRange() 
{
	char text[32];
	GetDlgItemText(IDC_SCALE_SLIDER_RANGE, text, 32);
	float r = relief_info.scale_slider_range;
	sscanf(text, "%f", &r);
	r = -r;
	sprintf(text, "%f", r);
	relief_info.scale = -relief_info.scale;
	SetDlgItemText(IDC_SCALE_SLIDER_RANGE, text);
	OnSetScaleSliderRange();
	Update_Selection_Relief(true);
}

void ReliefMapDlg::OnSetScaleSliderRange() 
{
	char text[32];
	GetDlgItemText(IDC_SCALE_SLIDER_RANGE, text, 32);
	sscanf(text, "%f", &relief_info.scale_slider_range);
	SetReliefInfo(&relief_info);
	Update_Selection_Relief(false);
}

void ReliefMapDlg::OnPickConcaveColor() 
{
	pick_concave_color = IsDlgButtonChecked(IDC_PICK_CONCAVE_COLOR)!=0;
	if(pick_concave_color)
	{
		::SetFocus(Edit_HWND());
	}
	else
	{
		CWnd *b = GetDlgItem(IDC_CONCAVE_COLOR_BUTTON);
		b->RedrawWindow();
	}
}

void ReliefMapDlg::OnPickConvexColor() 
{
	pick_convex_color = IsDlgButtonChecked(IDC_PICK_CONVEX_COLOR)!=0;
	if(pick_convex_color)
	{
		::SetFocus(Edit_HWND());
	}
	else
	{
		CWnd *b = GetDlgItem(IDC_CONVEX_COLOR_BUTTON);
		b->RedrawWindow();
	}
}

void ReliefMapDlg::OnSplitLayer() 
{
	if(Num_Selected_Layers()>0)
	{
		Split_Selection_By_Relief();
	}
}

void ReliefMapDlg::OnSetBias() 
{
	char text[32];
	GetDlgItemText(IDC_BIAS, text, 32);
	sscanf(text, "%f", &relief_info.bias);
	SetReliefInfo(&relief_info);
	Update_Selection_Relief(true);
}

void ReliefMapDlg::OnResetBias() 
{
	relief_info.bias = 0.5f;
	char text[32];
	sprintf(text, "%f", relief_info.bias);SetDlgItemText(IDC_BIAS, text);
	m_BiasSlider.SetPos(500);
	Update_Selection_Relief(true);
}

void ReliefMapDlg::OnBrightnessOnly() 
{
	bool b = IsDlgButtonChecked(IDC_BRIGHTNESS_ONLY)!=0;
	if(b)
	{
		if(relief_info.type==RELIEF_TYPE_BRIGHTNESS)return;
		relief_info.type = RELIEF_TYPE_BRIGHTNESS;
	}
	else
	{
		if(relief_info.type==RELIEF_TYPE_COLOR_DIFFERENCE)return;
		relief_info.type = RELIEF_TYPE_COLOR_DIFFERENCE;
	}
	Update_Selection_Relief(true);
	if(rotoscope_color_difference_mode)
	{
		Update_Relief_Layer_Split_Pixels();
		redraw_edit_window = true;
	}
}

void ReliefMapDlg::OnSetTilingSliderRange() 
{
	char text[32];
	GetDlgItemText(IDC_TILING_SLIDER_RANGE, text, 32);
	sscanf(text, "%f", &relief_info.tiling_slider_range);
	SetReliefInfo(&relief_info);
	Update_Selection_Relief(false);
}

void ReliefMapDlg::OnSetContrastSliderRange() 
{
	char text[32];
	GetDlgItemText(IDC_CONTRAST_SLIDER_RANGE, text, 32);
	sscanf(text, "%f", &relief_info.contrast_slider_range);
	SetReliefInfo(&relief_info);
	Update_Selection_Relief(false);
}

void ReliefMapDlg::OnMfcSelectConcaveColor() 
{
	CColorDialog dlg;
	dlg.SetCurrentColor(RGB(m_ConcaveColorButton.rgb[0],m_ConcaveColorButton.rgb[1],m_ConcaveColorButton.rgb[2]));
	if(dlg.DoModal()==IDOK)
	{
		unsigned char rgb[3];
		COLORREF color = dlg.GetColor();
		rgb[0] = GetRValue(color);
		rgb[1] = GetGValue(color);
		rgb[2] = GetBValue(color);
		m_ConcaveColorButton.Set_Color(rgb[0], rgb[1], rgb[2]);
		relief_info.concave_color[0] = rgb[0];
		relief_info.concave_color[1] = rgb[1];
		relief_info.concave_color[2] = rgb[2];
		CWnd *b = GetDlgItem(IDC_CONCAVE_COLOR_BUTTON);
		b->RedrawWindow();
		Update_Selection_Relief(true);
		if(rotoscope_color_difference_mode)
		{
			Update_Relief_Layer_Split_Pixels();
		}
		redraw_frame = true;
		redraw_edit_window = true;
	}
}

void ReliefMapDlg::OnMfcSelectConvexColor() 
{
	CColorDialog dlg;
	dlg.SetCurrentColor(RGB(m_ConvexColorButton.rgb[0],m_ConvexColorButton.rgb[1],m_ConvexColorButton.rgb[2]));
	if(dlg.DoModal()==IDOK)
	{
		unsigned char rgb[3];
		COLORREF color = dlg.GetColor();
		rgb[0] = GetRValue(color);
		rgb[1] = GetGValue(color);
		rgb[2] = GetBValue(color);
		m_ConvexColorButton.Set_Color(rgb[0], rgb[1], rgb[2]);
		relief_info.convex_color[0] = rgb[0];
		relief_info.convex_color[1] = rgb[1];
		relief_info.convex_color[2] = rgb[2];
		CWnd *b = GetDlgItem(IDC_CONVEX_COLOR_BUTTON);
		b->RedrawWindow();
		Update_Selection_Relief(true);
		if(rotoscope_color_difference_mode)
		{
			Update_Relief_Layer_Split_Pixels();
		}
		redraw_frame = true;
		redraw_edit_window = true;
	}
}

void ReliefMapDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	if(!rotoscope_color_difference_mode)
	{
		UpdateSliders();
	}
	else
	{
		char text[32];
		float b_pos = ((float)m_BiasSlider.GetPos())/1000;
		relief_info.bias = b_pos;
		sprintf(text, "%f", relief_info.bias);SetDlgItemText(IDC_BIAS, text);
		Update_Selection_Relief(true);
		if(rotoscope_color_difference_mode)
		{
			Update_Relief_Layer_Split_Pixels();
			redraw_edit_window = true;
		}
	}
}

void ReliefMapDlg::GetReliefInfo(RELIEF_INFO *ri)
{
	char text[256];
	if(IsDlgButtonChecked(IDC_RANDOM_NOISE)!=0)ri->type = RELIEF_TYPE_RANDOM_NOISE;
	if(IsDlgButtonChecked(IDC_RELIEF_IMAGE)!=0)ri->type = RELIEF_TYPE_IMAGE;
	if(IsDlgButtonChecked(IDC_COLOR_DIFFERENCE)!=0)
	{
		if(IsDlgButtonChecked(IDC_BRIGHTNESS_ONLY)!=0)
		{
			ri->type = RELIEF_TYPE_BRIGHTNESS;
		}
		else
		{
			ri->type = RELIEF_TYPE_COLOR_DIFFERENCE;
		}
	}
	GetDlgItemText(IDC_CONTRAST, text, 32);sscanf(text, "%f", &ri->contrast);
	GetDlgItemText(IDC_CONTRAST_SLIDER_RANGE, text, 32);sscanf(text, "%f", &ri->contrast_slider_range);
	GetDlgItemText(IDC_TILING, text, 32);sscanf(text, "%f", &ri->tiling);
	GetDlgItemText(IDC_TILING_SLIDER_RANGE, text, 32);sscanf(text, "%f", &ri->tiling_slider_range);
	GetDlgItemText(IDC_BIAS, text, 32);sscanf(text, "%f", &ri->bias);
	GetDlgItemText(IDC_SCALE, text, 32);sscanf(text, "%f", &ri->scale);
	GetDlgItemText(IDC_SCALE_SLIDER_RANGE, text, 32);sscanf(text, "%f", &ri->scale_slider_range);
	GetDlgItemText(IDC_IMAGE_FILE, text, 32);strcpy(ri->image, text);
	m_ConcaveColorButton.Get_Color(&ri->concave_color[0], &ri->concave_color[1], &ri->concave_color[2]);
	m_ConvexColorButton.Get_Color(&ri->convex_color[0], &ri->convex_color[1], &ri->convex_color[2]);
}

void ReliefMapDlg::SetReliefInfo(RELIEF_INFO *ri)
{
	char text[256];
	CheckDlgButton(IDC_RANDOM_NOISE, ri->type == RELIEF_TYPE_RANDOM_NOISE);
	CheckDlgButton(IDC_RELIEF_IMAGE, ri->type == RELIEF_TYPE_IMAGE);
	CheckDlgButton(IDC_COLOR_DIFFERENCE, ri->type == RELIEF_TYPE_COLOR_DIFFERENCE||ri->type == RELIEF_TYPE_BRIGHTNESS);
	CheckDlgButton(IDC_BRIGHTNESS_ONLY, ri->type == RELIEF_TYPE_BRIGHTNESS);
	CheckDlgButton(IDC_SMOOTH_TRANSITION, ri->smooth);

	sprintf(text, "%.2f", ri->contrast);SetDlgItemText(IDC_CONTRAST, text);
	sprintf(text, "%.2f", ri->contrast_slider_range);SetDlgItemText(IDC_CONTRAST_SLIDER_RANGE, text);
	sprintf(text, "%.2f", ri->tiling);SetDlgItemText(IDC_TILING, text);
	sprintf(text, "%.2f", ri->tiling_slider_range);SetDlgItemText(IDC_TILING_SLIDER_RANGE, text);
	sprintf(text, "%.2f", ri->bias);SetDlgItemText(IDC_BIAS, text);
	sprintf(text, "%.2f", ri->scale);SetDlgItemText(IDC_SCALE, text);
	sprintf(text, "%.2f", ri->scale_slider_range);SetDlgItemText(IDC_SCALE_SLIDER_RANGE, text);
	SetDlgItemText(IDC_IMAGE_FILE, ri->image);
	m_ConcaveColorButton.Set_Color(ri->concave_color[0], ri->concave_color[1], ri->concave_color[2]);
	m_ConvexColorButton.Set_Color(ri->convex_color[0], ri->convex_color[1], ri->convex_color[2]);
	m_ConcaveColorButton.RedrawWindow();
	m_ConvexColorButton.RedrawWindow();
	float c_pos = (ri->contrast/ri->contrast_slider_range)*1000;
	float t_pos = (ri->tiling/ri->tiling_slider_range)*1000;
	float s_pos = (ri->scale/ri->scale_slider_range)*1000;
	float b_pos = (ri->bias/1.0f)*1000;
	m_ContrastSlider.SetPos((int)c_pos);
	m_TilingSlider.SetPos((int)t_pos);
	m_ScaleSlider.SetPos((int)s_pos);
	last_relief_scale_pos = (int)s_pos;
	m_BiasSlider.SetPos((int)b_pos);
	CheckDlgButton(IDC_PICK_CONCAVE_COLOR, pick_concave_color);
	CheckDlgButton(IDC_PICK_CONVEX_COLOR, pick_convex_color);
	EnableControls();
}

void ReliefMapDlg::EnableControls()
{
	if(rotoscope_color_difference_mode)
	{
		return;
	}
	bool noise_controls = false;
	bool image_controls = false;
	bool color_controls = false;

	if(relief_info.type==RELIEF_TYPE_RANDOM_NOISE)
	{
		noise_controls = true;
		image_controls = false;
		color_controls = false;
	}
	if(relief_info.type==RELIEF_TYPE_IMAGE)
	{
		noise_controls = false;
		image_controls = true;
		color_controls = false;
	}
	if(relief_info.type==RELIEF_TYPE_COLOR_DIFFERENCE||relief_info.type==RELIEF_TYPE_BRIGHTNESS)
	{
		noise_controls = false;
		image_controls = false;
		color_controls = true;
	}

	int show_noise_controls;
	int show_image_controls;
	int show_color_controls;
	if(noise_controls)
	{
		show_noise_controls = SW_SHOW;
		show_image_controls = SW_HIDE;
		show_color_controls = SW_HIDE;
	}
	if(image_controls)
	{
		show_noise_controls = SW_HIDE;
		show_image_controls = SW_SHOW;
		show_color_controls = SW_HIDE;
	}
	if(color_controls)
	{
		show_noise_controls = SW_HIDE;
		show_image_controls = SW_HIDE;
		show_color_controls = SW_SHOW;
	}

	//enable/disable

	//random noise
	GetDlgItem(IDC_CONTRAST_SLIDER)->EnableWindow(noise_controls);
	GetDlgItem(IDC_CONTRAST)->EnableWindow(noise_controls);
	GetDlgItem(IDC_SET_CONTRAST)->EnableWindow(noise_controls);
	GetDlgItem(IDC_CONTRAST_SLIDER_RANGE)->EnableWindow(noise_controls);
	GetDlgItem(IDC_SET_CONTRAST_SLIDER_RANGE)->EnableWindow(noise_controls);
	//image
	GetDlgItem(IDC_TILING_SLIDER)->EnableWindow(image_controls);
	GetDlgItem(IDC_TILING)->EnableWindow(image_controls);
	GetDlgItem(IDC_SET_TILING)->EnableWindow(image_controls);
	GetDlgItem(IDC_TILING_SLIDER_RANGE)->EnableWindow(image_controls);
	GetDlgItem(IDC_SET_TILING_SLIDER_RANGE)->EnableWindow(image_controls);
	GetDlgItem(IDC_CHANGE_IMAGE)->EnableWindow(image_controls);
	GetDlgItem(IDC_IMAGE_FILE)->EnableWindow(image_controls);
	//color difference
	GetDlgItem(IDC_BIAS_SLIDER)->EnableWindow(color_controls);
	GetDlgItem(IDC_BIAS)->EnableWindow(color_controls);
	GetDlgItem(IDC_SET_BIAS)->EnableWindow(color_controls);
	GetDlgItem(IDC_PICK_CONCAVE_COLOR)->EnableWindow(color_controls);
	GetDlgItem(IDC_PICK_CONVEX_COLOR)->EnableWindow(color_controls);
	GetDlgItem(IDC_SPLIT_LAYER)->EnableWindow(color_controls);
	GetDlgItem(IDC_CONCAVE_COLOR_BUTTON)->EnableWindow(color_controls);
	GetDlgItem(IDC_CONVEX_COLOR_BUTTON)->EnableWindow(color_controls);
	GetDlgItem(IDC_RESET_BIAS)->EnableWindow(color_controls);
	GetDlgItem(IDC_BRIGHTNESS_ONLY)->EnableWindow(color_controls);
	GetDlgItem(IDC_SMOOTH_TRANSITION)->EnableWindow(color_controls);
	GetDlgItem(IDC_MFC_SELECT_CONCAVE_COLOR)->EnableWindow(color_controls);
	GetDlgItem(IDC_MFC_SELECT_CONVEX_COLOR)->EnableWindow(color_controls);


	//hide/show

	//random noise
	GetDlgItem(IDC_CONTRAST_SLIDER)->ShowWindow(show_noise_controls);
	GetDlgItem(IDC_CONTRAST)->ShowWindow(show_noise_controls);
	GetDlgItem(IDC_SET_CONTRAST)->ShowWindow(show_noise_controls);
	GetDlgItem(IDC_CONTRAST_SLIDER_RANGE)->ShowWindow(show_noise_controls);
	GetDlgItem(IDC_SET_CONTRAST_SLIDER_RANGE)->ShowWindow(show_noise_controls);
	GetDlgItem(IDC_CONTRAST_LABEL)->ShowWindow(show_noise_controls);
	GetDlgItem(IDC_CONTRAST_SLIDER_RANGE_LABEL)->ShowWindow(show_noise_controls);
	//image
	GetDlgItem(IDC_TILING_SLIDER)->ShowWindow(show_image_controls);
	GetDlgItem(IDC_TILING)->ShowWindow(show_image_controls);
	GetDlgItem(IDC_SET_TILING)->ShowWindow(show_image_controls);
	GetDlgItem(IDC_TILING_SLIDER_RANGE)->ShowWindow(show_image_controls);
	GetDlgItem(IDC_SET_TILING_SLIDER_RANGE)->ShowWindow(show_image_controls);
	GetDlgItem(IDC_CHANGE_IMAGE)->ShowWindow(show_image_controls);
	GetDlgItem(IDC_IMAGE_FILE)->ShowWindow(show_image_controls);
	GetDlgItem(IDC_TILING_LABEL)->ShowWindow(show_image_controls);
	GetDlgItem(IDC_IMAGE_LABEL)->ShowWindow(show_image_controls);
	GetDlgItem(IDC_TILING_SLIDER_RANGE_LABEL)->ShowWindow(show_image_controls);
	//color difference
	GetDlgItem(IDC_BIAS_SLIDER)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_BIAS)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_SET_BIAS)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_PICK_CONCAVE_COLOR)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_PICK_CONVEX_COLOR)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_SPLIT_LAYER)->ShowWindow(false);
	GetDlgItem(IDC_CONCAVE_COLOR_BUTTON)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_CONVEX_COLOR_BUTTON)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_RESET_BIAS)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_BRIGHTNESS_ONLY)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_SMOOTH_TRANSITION)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_CONCAVE_COLOR_LABEL)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_CONVEX_COLOR_LABEL)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_DIFFERENCE_BIAS_LABEL)->ShowWindow(show_color_controls);
	GetDlgItem(IDC_MFC_SELECT_CONCAVE_COLOR)->ShowWindow(color_controls);
	GetDlgItem(IDC_MFC_SELECT_CONVEX_COLOR)->ShowWindow(color_controls);
}

void ReliefMapDlg::UpdateSliders()
{
	float c_pos = ((float)m_ContrastSlider.GetPos())/1000;
	float t_pos = ((float)m_TilingSlider.GetPos())/1000;
	int pos = m_ScaleSlider.GetPos();
	float s_pos = ((float)pos)/1000;
	char text[32];
	if(pos!=last_relief_scale_pos)
	{
		//scale slider moved
		last_relief_scale_pos = pos;
		relief_info.scale = s_pos*relief_info.scale_slider_range;
		Update_Selection_Relief(true);
		sprintf(text, "%f", relief_info.scale);SetDlgItemText(IDC_SCALE, text);
		return;
	}
	float b_pos = ((float)m_BiasSlider.GetPos())/1000;
	relief_info.contrast = c_pos*relief_info.contrast_slider_range;
	relief_info.tiling = t_pos*relief_info.tiling_slider_range;
	relief_info.bias = b_pos;
	sprintf(text, "%f", relief_info.contrast);SetDlgItemText(IDC_CONTRAST, text);
	sprintf(text, "%f", relief_info.tiling);SetDlgItemText(IDC_TILING, text);
	sprintf(text, "%f", relief_info.bias);SetDlgItemText(IDC_BIAS, text);
	sprintf(text, "%f", relief_info.scale);SetDlgItemText(IDC_SCALE, text);
	Update_Selection_Relief(true);
}

void ReliefMapDlg::OnClose() 
{
	ShowWindow(SW_HIDE);
	render_relief_preview = false;
	redraw_frame = true;
	pick_concave_color = false;
	pick_convex_color = false;
	if(rotoscope_color_difference_mode)
	{
		Close_Rotoscope_Color_Difference_Dlg();
	}
	redraw_edit_window = true;
}

void ReliefMapDlg::OnSmoothTransition() 
{
	relief_info.smooth = IsDlgButtonChecked(IDC_SMOOTH_TRANSITION)!=0;
	Update_Selection_Relief(true);
}

HBRUSH ReliefMapDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	unsigned int id = pWnd->GetDlgCtrlID();
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX||id==IDC_IMAGE_FILE)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	if(

id==IDC_SCALE_LABEL||
id==IDC_SLIDER_RANGE_LABEL||
id==IDC_CONCAVE_COLOR_LABEL||
id==IDC_CONVEX_COLOR_LABEL||
id==IDC_DIFFERENCE_BIAS_LABEL||
id==IDC_TILING_LABEL||
id==IDC_TILING_SLIDER_RANGE_LABEL||
id==IDC_CONTRAST_LABEL||
id==IDC_CONTRAST_SLIDER_RANGE_LABEL||
id==IDC_IMAGE_LABEL)
	
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

void ReliefMapDlg::OnChangeImage() 
{
	if(Browse(relief_info.image, "*", false))
	{
		SetDlgItemText(IDC_IMAGE_FILE, relief_info.image);
		Update_Selection_Relief(true);
	}
}
