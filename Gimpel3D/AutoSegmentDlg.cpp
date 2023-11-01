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
// AutoSegmentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "AutoSegmentDlg.h"
#include "G3DCoreFiles/AutoSegment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AutoSegmentDlg dialog

AutoSegmentDlg *autoSegmentDlg = 0;

AutoSegmentDlg::AutoSegmentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AutoSegmentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AutoSegmentDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AutoSegmentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AutoSegmentDlg)
	DDX_Control(pDX, IDC_FINALIZE_SEGMENTS, m_FinalizeSegments);
	DDX_Control(pDX, IDC_RESET_PREVIEW, m_ResetPreview);
	DDX_Control(pDX, IDC_PREVIEW_SEGMENTS, m_PreviewSegments);
	DDX_Control(pDX, IDC_GENERATE_CONTRAST_MAP, m_GenerateContrastMap);
	DDX_Control(pDX, IDC_COLOR_THRESHOLD_SLIDER, m_ColorThresholdSlider);
	DDX_Control(pDX, IDC_DARKNESS_THRESHOLD_SLIDER, m_DarknessThresholdSlider);
	//}}AFX_DATA_MAP
	CheckDlgButton(IDC_OUTLINES_ONLY, auto_segment_render_outlines_only);
	m_DarknessThresholdSlider.SetRange(0, 200);
	m_ColorThresholdSlider.SetRange(0, 200);
	float pos = outline_darkness_threshold*200;
	m_DarknessThresholdSlider.SetPos((int)pos);
	pos = outline_color_threshold*200;
	m_ColorThresholdSlider.SetPos((int)pos);


	GetDlgItem(IDC_DARKNESS_THRESHOLD_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_OUTLINES_ONLY)->SetFont(skin_font);
	GetDlgItem(IDC_PREVIEW_SEGMENTS)->SetFont(skin_font);
	GetDlgItem(IDC_RESET_PREVIEW)->SetFont(skin_font);
	GetDlgItem(IDC_FINALIZE_SEGMENTS)->SetFont(skin_font);
	GetDlgItem(IDC_GENERATE_CONTRAST_MAP)->SetFont(skin_font);
	GetDlgItem(IDC_COLOR_THRESHOLD_SLIDER)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(AutoSegmentDlg, CDialog)
	//{{AFX_MSG_MAP(AutoSegmentDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_OUTLINES_ONLY, OnOutlinesOnly)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_PREVIEW_SEGMENTS, OnPreviewSegments)
	ON_BN_CLICKED(IDC_RESET_PREVIEW, OnResetPreview)
	ON_BN_CLICKED(IDC_FINALIZE_SEGMENTS, OnFinalizeSegments)
	ON_BN_CLICKED(IDC_GENERATE_CONTRAST_MAP, OnGenerateContrastMap)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AutoSegmentDlg message handlers

bool Open_AutoSegment_Dialog()
{
	if(!autoSegmentDlg)
	{
		autoSegmentDlg  = new AutoSegmentDlg;
		autoSegmentDlg->Create(IDD_AUTO_SEGMENT_DLG);
		autoSegmentDlg->SetWindowText("Auto Segment Image");
		Create_ToolSkin_Frame(autoSegmentDlg, "Auto Segment Image", IDC_AUTO_SEGMENT_IMAGE);
	}
	autoSegmentDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_AutoSegment_Dialog()
{
	if(autoSegmentDlg)
	{
		autoSegmentDlg->ShowWindow(SW_HIDE);
	}
	return true;
}


void AutoSegmentDlg::OnClose() 
{
	Close_AutoSegment_Tool();
}

void AutoSegmentDlg::OnOutlinesOnly() 
{
	auto_segment_render_outlines_only = IsDlgButtonChecked(IDC_OUTLINES_ONLY)!=0;
	redraw_edit_window = true;
}

void AutoSegmentDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	float pos = (float)m_DarknessThresholdSlider.GetPos();
	outline_darkness_threshold = pos/200;
	pos = (float)m_ColorThresholdSlider.GetPos();
	outline_color_threshold = pos/200;
	Update_AutoSegment_Outline_Pixels();
	redraw_edit_window = true;
}

void AutoSegmentDlg::OnPreviewSegments() 
{
	Preview_AutoSegments();
	redraw_edit_window = true;
}

void AutoSegmentDlg::OnResetPreview() 
{
	Reset_Preview_AutoSegments();
	redraw_edit_window = true;
}

void AutoSegmentDlg::OnFinalizeSegments() 
{
	Finalize_AutoSegments();
	redraw_edit_window = true;
}

void AutoSegmentDlg::OnGenerateContrastMap() 
{
	Generate_AutoSegment_Contrast_Map();
	redraw_edit_window = true;
}

HBRUSH AutoSegmentDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
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
