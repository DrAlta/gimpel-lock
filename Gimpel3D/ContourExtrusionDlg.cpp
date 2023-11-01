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
// ContourExtrusionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "ContourExtrusionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ContourExtrusionDlg dialog


ContourExtrusionDlg::ContourExtrusionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ContourExtrusionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ContourExtrusionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ContourExtrusionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ContourExtrusionDlg)
	DDX_Control(pDX, IDC_SET_SEARCH_RANGE, m_SetSearchRange);
	DDX_Control(pDX, IDC_SET_PUFF_SCALE, m_SetScale);
	DDX_Control(pDX, IDC_SET_PUFF_RANGE, m_SetRange);
	DDX_Control(pDX, IDC_SET_PREVIEW_SKIP, m_SetSkip);
	DDX_Control(pDX, IDC_SET_EXTRUSION_ANGLE, m_SetAngle);
	DDX_Control(pDX, IDC_RESET_TO_SELECTION, m_ResetToSelection);
	DDX_Control(pDX, IDC_RESET_EXTRUSION_ANGLE, m_ResetAngle);
	DDX_Control(pDX, IDC_INCREASE_SEARCH_RANGE, m_IncreaseRange);
	DDX_Control(pDX, IDC_INCREASE_PREVIEW_SKIP, m_IncreaseSkip);
	DDX_Control(pDX, IDC_INCREASE_EXTRUSION_ANGLE, m_IncreaseAngle);
	DDX_Control(pDX, IDC_DECREASE_SEARCH_RANGE, m_DecreaseRange);
	DDX_Control(pDX, IDC_DECREASE_PREVIEW_SKIP, m_DecreaseSkip);
	DDX_Control(pDX, IDC_DECREASE_EXTRUSION_ANGLE, m_DecreaseAngle);
	DDX_Control(pDX, IDC_CENTER_PUFF_RANGE, m_CenterPuffRange);
	DDX_Control(pDX, IDC_APPLY_CONTOUR, m_ApplyContour);
	DDX_Control(pDX, IDC_DIRECTIONAL_EXTRUSION_SLIDER, m_DirectionalExtrusionSlider);
	DDX_Control(pDX, IDC_PUFF_RANGE_SLIDER, m_PuffRangeSlider);
	//}}AFX_DATA_MAP
	char text[32];
	sprintf(text, "%i", puff_preview_spacing);SetDlgItemText(IDC_PREVIEW_SKIP, text);
	sprintf(text, "%i", puff_search_range);SetDlgItemText(IDC_SEARCH_RANGE, text);
	sprintf(text, "%f", puff_range);SetDlgItemText(IDC_PUFF_RANGE, text);
	sprintf(text, "%f", puff_scale);SetDlgItemText(IDC_PUFF_SCALE, text);
	sprintf(text, "%f", directional_puff_angle);SetDlgItemText(IDC_EXTRUSION_ANGLE, text);
	m_PuffRangeSlider.SetRange(1, 1001);
	m_DirectionalExtrusionSlider.SetRange(1, 1001);
	UpdateSlider();
	CheckDlgButton(IDC_SPHERICAL_EXTRUSION, current_puff_type==PUFF_SPHERICAL);
	CheckDlgButton(IDC_LINEAR_EXTRUSION, current_puff_type==PUFF_LINEAR);
	CheckDlgButton(IDC_DIRECTIONAL_EXTRUSION, directional_extrusion);

	GetDlgItem(IDC_PREVIEW_SKIP)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_SET_PREVIEW_SKIP)->SetFont(skin_font);
	GetDlgItem(IDC_INCREASE_PREVIEW_SKIP)->SetFont(skin_font);
	GetDlgItem(IDC_DECREASE_PREVIEW_SKIP)->SetFont(skin_font);
	GetDlgItem(IDC_SEARCH_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_SET_SEARCH_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_INCREASE_SEARCH_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_DECREASE_SEARCH_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC3)->SetFont(skin_font);
	GetDlgItem(IDC_PUFF_RANGE_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_PUFF_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_SET_PUFF_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_CENTER_PUFF_RANGE)->SetFont(skin_font);
	GetDlgItem(IDC_SPHERICAL_EXTRUSION)->SetFont(skin_font);
	GetDlgItem(IDC_LINEAR_EXTRUSION)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC4)->SetFont(skin_font);
	GetDlgItem(IDC_EXTRUSION_ANGLE)->SetFont(skin_font);
	GetDlgItem(IDC_SET_EXTRUSION_ANGLE)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC5)->SetFont(skin_font);
	GetDlgItem(IDC_PUFF_SCALE)->SetFont(skin_font);
	GetDlgItem(IDC_SET_PUFF_SCALE)->SetFont(skin_font);
	GetDlgItem(IDC_APPLY_CONTOUR)->SetFont(skin_font);
	GetDlgItem(IDC_DIRECTIONAL_EXTRUSION)->SetFont(skin_font);
	GetDlgItem(IDC_INCREASE_EXTRUSION_ANGLE)->SetFont(skin_font);
	GetDlgItem(IDC_DECREASE_EXTRUSION_ANGLE)->SetFont(skin_font);
	GetDlgItem(IDC_DIRECTIONAL_EXTRUSION_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_RESET_TO_SELECTION)->SetFont(skin_font);
	GetDlgItem(IDC_RESET_EXTRUSION_ANGLE)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(ContourExtrusionDlg, CDialog)
	//{{AFX_MSG_MAP(ContourExtrusionDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RESET_TO_SELECTION, OnResetToSelection)
	ON_BN_CLICKED(IDC_APPLY_CONTOUR, OnApplyContour)
	ON_BN_CLICKED(IDC_SPHERICAL_EXTRUSION, OnSphericalExtrusion)
	ON_BN_CLICKED(IDC_LINEAR_EXTRUSION, OnLinearExtrusion)
	ON_BN_CLICKED(IDC_DIRECTIONAL_EXTRUSION, OnDirectionalExtrusion)
	ON_BN_CLICKED(IDC_SET_EXTRUSION_ANGLE, OnSetExtrusionAngle)
	ON_BN_CLICKED(IDC_INCREASE_EXTRUSION_ANGLE, OnIncreaseExtrusionAngle)
	ON_BN_CLICKED(IDC_DECREASE_EXTRUSION_ANGLE, OnDecreaseExtrusionAngle)
	ON_BN_CLICKED(IDC_RESET_EXTRUSION_ANGLE, OnResetExtrusionAngle)
	ON_BN_CLICKED(IDC_SET_PREVIEW_SKIP, OnSetPreviewSkip)
	ON_BN_CLICKED(IDC_INCREASE_PREVIEW_SKIP, OnIncreasePreviewSkip)
	ON_BN_CLICKED(IDC_DECREASE_PREVIEW_SKIP, OnDecreasePreviewSkip)
	ON_BN_CLICKED(IDC_SET_SEARCH_RANGE, OnSetSearchRange)
	ON_BN_CLICKED(IDC_INCREASE_SEARCH_RANGE, OnIncreaseSearchRange)
	ON_BN_CLICKED(IDC_DECREASE_SEARCH_RANGE, OnDecreaseSearchRange)
	ON_BN_CLICKED(IDC_SET_PUFF_RANGE, OnSetPuffRange)
	ON_BN_CLICKED(IDC_SET_PUFF_SCALE, OnSetPuffScale)
	ON_BN_CLICKED(IDC_CENTER_PUFF_RANGE, OnCenterPuffRange)
	ON_WM_HSCROLL()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ContourExtrusionDlg message handlers

void ContourExtrusionDlg::OnResetToSelection() 
{
	Set_Contour_To_Selection();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnApplyContour() 
{
	Puff_Selection();
	Set_Contour_To_Selection();
}

void ContourExtrusionDlg::OnSphericalExtrusion() 
{
	Set_Spherical_Extrusion();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnLinearExtrusion() 
{
	Set_Linear_Extrusion();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnDirectionalExtrusion() 
{
	bool b = IsDlgButtonChecked(IDC_DIRECTIONAL_EXTRUSION)!=0;
	Set_Directional_Extrusion(b);
	redraw_frame = true;
}

void ContourExtrusionDlg::OnSetExtrusionAngle() 
{
	char text[32];
	float angle = 0;
	GetDlgItemText(IDC_EXTRUSION_ANGLE, text, 32);
	sscanf(text, "%f", &angle);
	Set_Directional_Extrusion_Angle(angle);
	UpdateSlider();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnIncreaseExtrusionAngle() 
{
	char text[32];
	float angle = 0;
	GetDlgItemText(IDC_EXTRUSION_ANGLE, text, 32);
	sscanf(text, "%f", &angle);
	angle += 1;
	if(angle>360)
	{
		angle -= 360;
	}
	sprintf(text, "%f", angle);
	SetDlgItemText(IDC_EXTRUSION_ANGLE, text);
	Set_Directional_Extrusion_Angle(angle);
	UpdateSlider();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnDecreaseExtrusionAngle() 
{
	char text[32];
	float angle = 0;
	GetDlgItemText(IDC_EXTRUSION_ANGLE, text, 32);
	sscanf(text, "%f", &angle);
	angle -= 1;
	if(angle<0)
	{
		angle += 360;
	}
	sprintf(text, "%f", angle);
	SetDlgItemText(IDC_EXTRUSION_ANGLE, text);
	Set_Directional_Extrusion_Angle(angle);
	redraw_frame = true;
}

void ContourExtrusionDlg::OnResetExtrusionAngle() 
{
	float angle = 180;
	SetDlgItemText(IDC_EXTRUSION_ANGLE, "180");
	Set_Directional_Extrusion_Angle(angle);
	UpdateSlider();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnSetPreviewSkip() 
{
	char text[256];
	GetDlgItemText(IDC_PREVIEW_SKIP, text, 256);
	sscanf(text, "%i", &puff_preview_spacing);
	Get_Preview_Puff_Pixels();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnIncreasePreviewSkip() 
{
	char text[256];
	int n = puff_preview_spacing;
	GetDlgItemText(IDC_PREVIEW_SKIP, text, 256);
	sscanf(text, "%i", &n);
	n++;
	sprintf(text, "%i", n);
	SetDlgItemText(IDC_PREVIEW_SKIP, text);
	puff_preview_spacing = n;
	Get_Preview_Puff_Pixels();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnDecreasePreviewSkip() 
{
	char text[256];
	int n = puff_preview_spacing;
	GetDlgItemText(IDC_PREVIEW_SKIP, text, 256);
	sscanf(text, "%i", &n);
	if(n<2)return;
	n--;
	sprintf(text, "%i", n);
	SetDlgItemText(IDC_PREVIEW_SKIP, text);
	puff_preview_spacing = n;
	Get_Preview_Puff_Pixels();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnSetSearchRange() 
{
	char text[256];
	GetDlgItemText(IDC_SEARCH_RANGE, text, 256);
	int r = 32;
	sscanf(text, "%i", &r);
	Set_Puff_Search_Range(r);
	Get_Preview_Puff_Pixels();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnIncreaseSearchRange() 
{
	char text[256];
	int n = puff_search_range;
	GetDlgItemText(IDC_SEARCH_RANGE, text, 256);
	sscanf(text, "%i", &n);
	n++;
	sprintf(text, "%i", n);
	SetDlgItemText(IDC_SEARCH_RANGE, text);
	Set_Puff_Search_Range(n);
	Get_Preview_Puff_Pixels();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnDecreaseSearchRange() 
{
	char text[256];
	int n = puff_search_range;
	GetDlgItemText(IDC_SEARCH_RANGE, text, 256);
	sscanf(text, "%i", &n);
	if(n<2)return;
	n--;
	sprintf(text, "%i", n);
	SetDlgItemText(IDC_SEARCH_RANGE, text);
	Set_Puff_Search_Range(n);
	Get_Preview_Puff_Pixels();
	redraw_frame = true;
}

void ContourExtrusionDlg::OnSetPuffRange() 
{
	char text[256];
	GetDlgItemText(IDC_PUFF_RANGE, text, 256);
	sscanf(text, "%f", &puff_range);
	UpdateSlider();
}

void ContourExtrusionDlg::OnSetPuffScale() 
{
	char text[256];
	GetDlgItemText(IDC_PUFF_SCALE, text, 256);
	sscanf(text, "%f", &puff_scale);
	UpdateSlider();
}

void ContourExtrusionDlg::OnCenterPuffRange() 
{
	puff_scale = 0;
	UpdateSlider();
	char text[32];
	sprintf(text, "%f", puff_scale);
	SetDlgItemText(IDC_PUFF_SCALE, text);
	redraw_frame = true;
}

void ContourExtrusionDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	float p = ((float)(m_PuffRangeSlider.GetPos()-1))/1000;
	p = (p-0.5f)*2;
	puff_scale = puff_range*p;
	char text[32];
	sprintf(text, "%f", puff_scale);
	SetDlgItemText(IDC_PUFF_SCALE, text);
	p = ((float)(m_DirectionalExtrusionSlider.GetPos()-1))/1000;
	float angle = p*360;
	sprintf(text, "%f", angle);
	SetDlgItemText(IDC_EXTRUSION_ANGLE, text);
	Set_Directional_Extrusion_Angle(angle);
	redraw_frame = true;
}

void ContourExtrusionDlg::UpdateSlider() 
{
	float slider_pos = 0;
	if(puff_scale>0)
	{
		slider_pos = 0.5f + ((puff_scale/puff_range)/2);
	}
	else
	{
		slider_pos = 0.5f - (((-puff_scale)/puff_range)/2);
	}
	m_PuffRangeSlider.SetPos((int)(slider_pos*1000)+1);
	
	slider_pos = directional_puff_angle/360;

	m_DirectionalExtrusionSlider.SetPos((int)(slider_pos*1000)+1);
}

void ContourExtrusionDlg::OnClose() 
{
	ShowWindow(SW_HIDE);
	render_puff_preview = false;
	redraw_frame = true;
	Free_Preview_Puff_Pixels();
}


HBRUSH ContourExtrusionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2||id==IDC_STATIC3||id==IDC_STATIC4||id==IDC_STATIC5)
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
