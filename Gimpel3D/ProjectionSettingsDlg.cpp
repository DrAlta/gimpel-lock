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
// ProjectionSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "ProjectionSettingsDlg.h"

bool ReApply_Frame_Transform();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ProjectionSettingsDlg dialog
extern ProjectionSettingsDlg *_projectionSettingsDlg;

ProjectionSettingsDlg::ProjectionSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ProjectionSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ProjectionSettingsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	_projectionSettingsDlg = this;
}


void ProjectionSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ProjectionSettingsDlg)
	DDX_Control(pDX, IDC_SET_FOV, m_SetFov);
	//}}AFX_DATA_MAP
	CheckDlgButton(IDC_FLAT_PROJECTION, projection_type==FLAT_PROJECTION);
	CheckDlgButton(IDC_DOME_PROJECTION, projection_type==DOME_PROJECTION);
	CheckDlgButton(IDC_CYLINDER_PROJECTION, projection_type==CYLINDER_PROJECTION);
	char text[32];
	sprintf(text, "%.2f", _fov);
	SetDlgItemText(IDC_FOV, text);

	GetDlgItem(IDC_FLAT_PROJECTION)->SetFont(skin_font);
	GetDlgItem(IDC_DOME_PROJECTION)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_FOV)->SetFont(skin_font);
	GetDlgItem(IDC_SET_FOV)->SetFont(skin_font);
	GetDlgItem(IDC_CYLINDER_PROJECTION)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(ProjectionSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(ProjectionSettingsDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_FLAT_PROJECTION, OnFlatProjection)
	ON_BN_CLICKED(IDC_SET_FOV, OnSetFov)
	ON_BN_CLICKED(IDC_DOME_PROJECTION, OnDomeProjection)
	ON_BN_CLICKED(IDC_CYLINDER_PROJECTION, OnCylinderProjection)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ProjectionSettingsDlg message handlers

void ProjectionSettingsDlg::OnFlatProjection() 
{
	projection_type = FLAT_PROJECTION;
	Get_Flat_Projection_Vectors();
	ReApply_Frame_Transform();
	Project_Layers();
}

void ProjectionSettingsDlg::OnSetFov() 
{
	char text[32];
	GetDlgItemText(IDC_FOV, text, 32);
	float v;
	sscanf(text, "%f", &v);
	_fov = aperture = DEFAULT_FOV = v;
	Get_Flat_Projection_Vectors();
	ReApply_Frame_Transform();
	Project_Layers();
	redraw_frame = true;
}

void Set_Projection_Dlg_FOV(float fov)
{
	if(_projectionSettingsDlg)
	{
		char text[32];
		sprintf(text, "%f", fov);
		_projectionSettingsDlg->SetDlgItemText(IDC_FOV, text);
	}
}

void ProjectionSettingsDlg::OnDomeProjection() 
{
	projection_type = DOME_PROJECTION;
	Get_Dome_Projection_Vectors();
}

void ProjectionSettingsDlg::OnCylinderProjection() 
{
	projection_type = CYLINDER_PROJECTION;
	Get_Cylinder_Projection_Vectors();
}

void ProjectionSettingsDlg::OnClose() 
{
}

HBRUSH ProjectionSettingsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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
