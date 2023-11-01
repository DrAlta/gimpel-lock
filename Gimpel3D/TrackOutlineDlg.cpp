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
// TrackOutlineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "TrackOutlineDlg.h"


bool Close_Track_Outlines_Tool();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TrackOutlineDlg dialog

TrackOutlineDlg *trackOutlineDlg = 0;

TrackOutlineDlg::TrackOutlineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TrackOutlineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TrackOutlineDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void TrackOutlineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TrackOutlineDlg)
	DDX_Control(pDX, IDC_REPLACE_LAYERS_FROM_TRACKING_INFO, m_ReplaceLayers);
	DDX_Control(pDX, IDC_GENERATE_TRACKING_INFO_FOR_SELECTION, m_GenerateTrackingInfo);
	DDX_Control(pDX, IDC_EXTRA_SMOOTH_OUTLINES, m_ExtraSmoothOutlines);
	DDX_Control(pDX, IDC_EXPAND_OUTLINES, m_ExpandOutlines);
	DDX_Control(pDX, IDC_CREATE_LAYERS_FROM_TRACKING_INFO, m_CreateLayersFromTrackingInfo);
	DDX_Control(pDX, IDC_CREATE_LAYERS_AND_ADVANCE_FRAME, m_CreateLayersAndAdvanceFrame);
	DDX_Control(pDX, IDC_CONTRACT_OUTLINES, m_ContractOutlines);
	DDX_Control(pDX, IDC_CLEAR_TRACKING_INFO, m_ClearTrackingInfo);
	//}}AFX_DATA_MAP
	SetDlgItemText(IDC_EXPAND_CONTRACT_AMOUNT, "0.1");

	GetDlgItem(IDC_GENERATE_TRACKING_INFO_FOR_SELECTION)->SetFont(skin_font);
	GetDlgItem(IDC_CLEAR_TRACKING_INFO)->SetFont(skin_font);
	GetDlgItem(IDC_CREATE_LAYERS_FROM_TRACKING_INFO)->SetFont(skin_font);
	GetDlgItem(IDC_EXTRA_SMOOTH_OUTLINES)->SetFont(skin_font);
	GetDlgItem(IDC_CREATE_LAYERS_AND_ADVANCE_FRAME)->SetFont(skin_font);
	GetDlgItem(IDC_EXPAND_OUTLINES)->SetFont(skin_font);
	GetDlgItem(IDC_CONTRACT_OUTLINES)->SetFont(skin_font);
	GetDlgItem(IDC_EXPAND_CONTRACT_AMOUNT)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_REPLACE_LAYERS_FROM_TRACKING_INFO)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(TrackOutlineDlg, CDialog)
	//{{AFX_MSG_MAP(TrackOutlineDlg)
	ON_BN_CLICKED(IDC_GENERATE_TRACKING_INFO_FOR_SELECTION, OnGenerateTrackingInfoForSelection)
	ON_BN_CLICKED(IDC_CLEAR_TRACKING_INFO, OnClearTrackingInfo)
	ON_BN_CLICKED(IDC_CREATE_LAYERS_FROM_TRACKING_INFO, OnCreateLayersFromTrackingInfo)
	ON_BN_CLICKED(IDC_REPLACE_LAYERS_FROM_TRACKING_INFO, OnReplaceLayersFromTrackingInfo)
	ON_BN_CLICKED(IDC_CREATE_LAYERS_AND_ADVANCE_FRAME, OnCreateLayersAndAdvanceFrame)
	ON_BN_CLICKED(IDC_EXTRA_SMOOTH_OUTLINES, OnExtraSmoothOutlines)
	ON_BN_CLICKED(IDC_EXPAND_OUTLINES, OnExpandOutlines)
	ON_BN_CLICKED(IDC_CONTRACT_OUTLINES, OnContractOutlines)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TrackOutlineDlg message handlers

void TrackOutlineDlg::OnGenerateTrackingInfoForSelection() 
{
	Generate_Tracking_Info_For_Selection();
}

void TrackOutlineDlg::OnClearTrackingInfo() 
{
	Clear_Tracking_Info();
}

void TrackOutlineDlg::OnCreateLayersFromTrackingInfo() 
{
	Create_Layers_From_Tracking_Info();
	Generate_Tracking_Info_For_Selection();
	Update_Layer_List();
}

void TrackOutlineDlg::OnReplaceLayersFromTrackingInfo() 
{
	Replace_Layers_From_Tracking_Info();
	redraw_frame = true;
	redraw_edit_window = true;
	Generate_Tracking_Info_For_Selection();
}

void TrackOutlineDlg::OnCreateLayersAndAdvanceFrame() 
{
	Create_Layers_From_Tracking_Info();
	Generate_Tracking_Info_For_Selection();
	Advance_Project_Frame();
}

void TrackOutlineDlg::OnExtraSmoothOutlines() 
{
	Extra_Smooth_Tracking_Outlines();
}

void TrackOutlineDlg::OnExpandOutlines() 
{
	char text[32];
	float v = 0;
	GetDlgItemText(IDC_EXPAND_CONTRACT_AMOUNT, text, 32);
	sscanf(text, "%f", &v);
	ExpandContract_Outlines(v, false);
}

void TrackOutlineDlg::OnContractOutlines() 
{
	char text[32];
	float v = 0;
	GetDlgItemText(IDC_EXPAND_CONTRACT_AMOUNT, text, 32);
	sscanf(text, "%f", &v);
	ExpandContract_Outlines(-v, false);
}

bool Open_Tracking_Outline_Dlg()
{
	if(!trackOutlineDlg)
	{
		trackOutlineDlg = new TrackOutlineDlg;
		trackOutlineDlg->Create(IDD_TRACK_OUTLINE_DLG);
		trackOutlineDlg->SetWindowText("Track Outline");
		Create_ToolSkin_Frame(trackOutlineDlg, "Track Outline", IDC_TRACK_OUTLINE);
	}
	trackOutlineDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_Tracking_Outline_Dlg()
{
	if(!trackOutlineDlg)return false;
	trackOutlineDlg->GetParent()->ShowWindow(SW_HIDE);
	return true;
}

void TrackOutlineDlg::OnClose() 
{
	Close_Track_Outlines_Tool();
}

HBRUSH TrackOutlineDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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
	return hbr;
}
