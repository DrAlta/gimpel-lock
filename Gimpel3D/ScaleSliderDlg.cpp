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
// ScaleSliderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "ScaleSliderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScaleSliderDlg dialog


ScaleSliderDlg::ScaleSliderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ScaleSliderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ScaleSliderDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ScaleSliderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ScaleSliderDlg)
	DDX_Control(pDX, IDC_SET_SLIDER_RANGE, m_SetSliderRange);
	DDX_Control(pDX, IDC_SET_SCALE, m_SetScale);
	DDX_Control(pDX, IDC_INCREASE_SCALE, m_IncreaseScale);
	DDX_Control(pDX, IDC_DECREASE_SCALE, m_DecreaseScale);
	DDX_Control(pDX, IDC_SCALE_SLIDER, m_ScaleSlider);
	//}}AFX_DATA_MAP
	m_ScaleSlider.SetRange(1, 1000);
	UpdateSliderPosition();

	GetDlgItem(IDC_SCALE_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_SCALE)->SetFont(skin_font);
	GetDlgItem(IDC_SET_SCALE)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_SLIDER_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_SET_SLIDER_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_INCREASE_SCALE)->SetFont(skin_font);
	GetDlgItem(IDC_DECREASE_SCALE)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(ScaleSliderDlg, CDialog)
	//{{AFX_MSG_MAP(ScaleSliderDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SET_SCALE, OnSetScale)
	ON_BN_CLICKED(IDC_INCREASE_SCALE, OnIncreaseScale)
	ON_BN_CLICKED(IDC_DECREASE_SCALE, OnDecreaseScale)
	ON_BN_CLICKED(IDC_SET_SLIDER_RANGE, OnSetSliderRange)
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScaleSliderDlg message handlers

void ScaleSliderDlg::OnSetScale() 
{
	char text[32];
	GetDlgItemText(IDC_SCALE, text, 32);
	float scale = 0;
	sscanf(text, "%f", &scale);
	Set_Selected_Layers_Scale(scale);
	Set_Scale_Slider_To_Selection();
	redraw_frame = true;
}

void ScaleSliderDlg::OnIncreaseScale() 
{
}

void ScaleSliderDlg::OnDecreaseScale() 
{
}

void ScaleSliderDlg::OnSetSliderRange() 
{
	char text[32];
	GetDlgItemText(IDC_SLIDER_RANGE, text, 32);
	sscanf(text, "%f", &scale_slider_range);
	UpdateSliderPosition();
}

void ScaleSliderDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	int slider_pos = m_ScaleSlider.GetPos()-1;
	float new_scale = 1.0f+((((float)slider_pos)/999)*scale_slider_range);
	float last_scale = 1.0f+((((float)last_scale_slider_pos)/1000)*scale_slider_range);
	char text[32];
	if(num_selected_scales==1)
	{
		Set_Selected_Layers_Scale(new_scale);
		sprintf(text, "%.4f", new_scale);SetDlgItemText(IDC_SCALE, text);
	}
	else
	{
		Adjust_Selected_Layers_Scale(new_scale-last_scale);
		total_scale_adjustment += new_scale-last_scale;
		sprintf(text, "%.4f", total_scale_adjustment);
		SetDlgItemText(IDC_SCALE, text);
	}
	last_scale_slider_pos = (float)slider_pos;
	redraw_frame = true;
}

void ScaleSliderDlg::UpdateSliderPosition()
{
	int n = Num_Layers();
	int num_selected = 0;
	float scale = -1;
	float res = -1;
	bool mismatch = false;
	float slider_pos = 0;
	char title[256];
	char text[64];
	int max_scale = (int)scale_slider_range;
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Get_Layer_Scale(i, &res);
			if(scale_slider_range<(1.0f-res))
			{
				scale_slider_range = 1.0f-res;
			}
			Layer_Name(i, text);
			num_selected++;
			if(num_selected==1)
			{
				scale = res;
			}
			else
			{
				if(res!=scale)
				{
					mismatch = true;
				}
			}
		}
	}
	if(num_selected==0)
	{
		slider_pos = 0;
		sprintf(title, "Edit Scale: No Selection");
	}
	else if(num_selected==1)
	{
		slider_pos = ((scale-1)/scale_slider_range)*1000;
		sprintf(title, "Edit Scale For Layer \"%s\"", text);
	}
	else
	{
		if(mismatch)
		{
			slider_pos = 500;
			sprintf(title, "Edit Scale For Multiple Selection, Various Scales.");
		}
		else
		{
			slider_pos = ((scale-1)/scale_slider_range)*1000;
			sprintf(title, "Edit Scale For Multiple Selection, All The Same Scale.");
		}
	}
	sprintf(text, "%f", scale_slider_range);SetDlgItemText(IDC_SLIDER_RANGE, text);

	if(num_selected==1||!mismatch)
	{
		sprintf(text, "%.4f", scale);SetDlgItemText(IDC_SCALE, text);
	}
	else
	{
		sprintf(text, "%.4f", 0);SetDlgItemText(IDC_SCALE, text);
	}
	SetWindowText(title);
	m_ScaleSlider.SetPos((int)slider_pos);
	num_selected_scales = num_selected;
	last_scale_slider_pos = slider_pos;
	scale_mismatch = mismatch;
	total_scale_adjustment = 0;

}

void ScaleSliderDlg::OnClose() 
{
	ShowWindow(SW_HIDE);
}

HBRUSH ScaleSliderDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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
