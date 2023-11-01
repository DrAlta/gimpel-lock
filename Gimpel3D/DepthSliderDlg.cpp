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
// DepthSliderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "DepthSliderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DepthSliderDlg dialog


DepthSliderDlg::DepthSliderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DepthSliderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(DepthSliderDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void DepthSliderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DepthSliderDlg)
	DDX_Control(pDX, IDC_SET_SLIDER_RANGE, m_SetSliderRange);
	DDX_Control(pDX, IDC_SET_DEPTH, m_SetDepth);
	DDX_Control(pDX, IDC_INCREASE_DEPTH, m_IncreaseDepth);
	DDX_Control(pDX, IDC_DECREASE_DEPTH, m_DecreaseDepth);
	DDX_Control(pDX, IDC_DEPTH_SLIDER, m_DepthSlider);
	//}}AFX_DATA_MAP
	m_DepthSlider.SetRange(1, 1000);
	UpdateSliderPosition();

	GetDlgItem(IDC_DEPTH_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_DEPTH)->SetFont(skin_font);
	GetDlgItem(IDC_SET_DEPTH)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_SLIDER_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_SET_SLIDER_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_INCREASE_DEPTH)->SetFont(skin_font);
	GetDlgItem(IDC_DECREASE_DEPTH)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(DepthSliderDlg, CDialog)
	//{{AFX_MSG_MAP(DepthSliderDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SET_DEPTH, OnSetDepth)
	ON_BN_CLICKED(IDC_INCREASE_DEPTH, OnIncreaseDepth)
	ON_BN_CLICKED(IDC_DECREASE_DEPTH, OnDecreaseDepth)
	ON_BN_CLICKED(IDC_SET_SLIDER_RANGE, OnSetSliderRange)
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DepthSliderDlg message handlers

void DepthSliderDlg::OnSetDepth() 
{
	char text[32];
	GetDlgItemText(IDC_DEPTH, text, 32);
	float depth = 50;
	sscanf(text, "%f", &depth);
	Set_Selected_Layers_Depth_ID_Float(depth);
	Set_Depth_Slider_To_Selection();
	redraw_frame = true;
}

void DepthSliderDlg::OnIncreaseDepth() 
{
}

void DepthSliderDlg::OnDecreaseDepth() 
{
}

void DepthSliderDlg::OnSetSliderRange() 
{
	char text[32];
	GetDlgItemText(IDC_SLIDER_RANGE, text, 32);
	sscanf(text, "%i", &depth_slider_range);
	UpdateSliderPosition();
}

void DepthSliderDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	int slider_pos = m_DepthSlider.GetPos();
	float new_depth = (((float)slider_pos)/1000)*depth_slider_range;
	float last_depth = (((float)last_depth_slider_pos)/1000)*depth_slider_range;
	char text[32];
	if(num_selected_depths==1||!depth_mismatch)
	{
		Set_Selected_Layers_Depth_ID_Float(new_depth);
		sprintf(text, "%.4f", new_depth);SetDlgItemText(IDC_DEPTH, text);
	}
	else
	{
		Adjust_Selected_Layers_Depth_ID_Float(new_depth-last_depth);
		total_depth_adjustment += new_depth-last_depth;
		sprintf(text, "%.4f", total_depth_adjustment);
		SetDlgItemText(IDC_DEPTH, text);
	}
	last_depth_slider_pos = (float)slider_pos;
	redraw_frame = true;
}


void DepthSliderDlg::UpdateSliderPosition()
{
	int n = Num_Layers();
	int num_selected = 0;
	float depth = -1;
	float res = -1;
	bool mismatch = false;
	float slider_pos = 0;
	char title[256];
	char text[64];
	int max_depth = depth_slider_range;
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Get_Layer_Depth_ID_Float(i, &res);
			if(depth_slider_range<res)
			{
				depth_slider_range = (int)res;
			}
			Layer_Name(i, text);
			num_selected++;
			if(num_selected==1)
			{
				depth = res;
			}
			else
			{
				if(res!=depth)
				{
					mismatch = true;
				}
			}
		}
	}
	if(num_selected==0)
	{
		slider_pos = 0;
		sprintf(title, "Edit Depth: No Selection");
	}
	else if(num_selected==1)
	{
		slider_pos = (depth/depth_slider_range)*1000;
		sprintf(title, "Edit Depth For Layer \"%s\"", text);
	}
	else
	{
		if(mismatch)
		{
			slider_pos = 500;
			sprintf(title, "Edit Depth For Multiple Selection, various depths.");
		}
		else
		{
			slider_pos = (depth/depth_slider_range)*1000;
			sprintf(title, "Edit Depth For Multiple Selection, All The Same Depth.");
		}
	}
	sprintf(text, "%i", depth_slider_range);SetDlgItemText(IDC_SLIDER_RANGE, text);
	if(num_selected==1||!mismatch)
	{
		sprintf(text, "%.4f", depth);SetDlgItemText(IDC_DEPTH, text);
	}
	else
	{
		sprintf(text, "%.4f", 0);SetDlgItemText(IDC_DEPTH, text);
	}
	SetWindowText(title);
	GetParent()->Invalidate();
	m_DepthSlider.SetPos((int)slider_pos);
	num_selected_depths = num_selected;
	last_depth_slider_pos = slider_pos;
	depth_mismatch = mismatch;
	total_depth_adjustment = 0;
}

void DepthSliderDlg::OnClose() 
{
	ShowWindow(SW_HIDE);
}
HBRUSH DepthSliderDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
