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
// GeometryToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "GeometryToolDlg.h"
#include "GeometryTool.h"
#include "GeometryTool/Primitives.h"
#include "G3DCoreFiles/Layers.h"
#include "LinkPointsTool.h"
#include "G3DMainFrame.h"

bool Update_Selection_Status_Info();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GeometryToolDlg dialog

GeometryToolDlg *geometryDlg = 0;

GeometryToolDlg::GeometryToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GeometryToolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(GeometryToolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void GeometryToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GeometryToolDlg)
	DDX_Control(pDX, IDC_LINK_POINTS, m_LinkPoints);
	DDX_Control(pDX, IDC_PROJECT_TO_MODEL, m_ProjectToModel);
	DDX_Control(pDX, IDC_BROWSE_MODEL, m_BrowseModel);
	DDX_Control(pDX, IDC_LAYERS_LIST, m_LayersList);
	//}}AFX_DATA_MAP
	CheckDlgButton(IDC_SHOW_GEOMETRY_WIREFRAME, show_geometry_wireframe);


	GetDlgItem(IDC_LINK_POINTS)->SetFont(skin_font);
	GetDlgItem(IDC_LAYERS_LIST)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_FLAT_PLANE)->SetFont(skin_font);
	GetDlgItem(IDC_SPHERE)->SetFont(skin_font);
	GetDlgItem(IDC_CYLINDER)->SetFont(skin_font);
	GetDlgItem(IDC_BOX)->SetFont(skin_font);
	GetDlgItem(IDC_CONE)->SetFont(skin_font);
	GetDlgItem(IDC_MODEL)->SetFont(skin_font);
	GetDlgItem(IDC_BROWSE_MODEL)->SetFont(skin_font);
	GetDlgItem(IDC_SHOW_GEOMETRY)->SetFont(skin_font);
	GetDlgItem(IDC_INVERT_GEOMETRY)->SetFont(skin_font);
	GetDlgItem(IDC_SHOW_GEOMETRY_WIREFRAME)->SetFont(skin_font);
	GetDlgItem(IDC_PROJECT_TO_MODEL)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(GeometryToolDlg, CDialog)
	//{{AFX_MSG_MAP(GeometryToolDlg)
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_LAYERS_LIST, OnSelchangeLayersList)
	ON_BN_CLICKED(IDC_FLAT_PLANE, OnFlatPlane)
	ON_BN_CLICKED(IDC_SPHERE, OnSphere)
	ON_BN_CLICKED(IDC_CYLINDER, OnCylinder)
	ON_BN_CLICKED(IDC_BOX, OnBox)
	ON_BN_CLICKED(IDC_CONE, OnCone)
	ON_BN_CLICKED(IDC_MODEL, OnModel)
	ON_BN_CLICKED(IDC_BROWSE_MODEL, OnBrowseModel)
	ON_BN_CLICKED(IDC_SHOW_GEOMETRY, OnShowGeometry)
	ON_BN_CLICKED(IDC_INVERT_GEOMETRY, OnInvertGeometry)
	ON_BN_CLICKED(IDC_SHOW_GEOMETRY_WIREFRAME, OnShowGeometryWireframe)
	ON_BN_CLICKED(IDC_PROJECT_TO_MODEL, OnProjectToModel)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_LINK_POINTS, OnLinkPoints)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GeometryToolDlg message handlers

bool Open_Geometry_Dialog()
{
	if(!geometryDlg)
	{
		geometryDlg = new GeometryToolDlg;
		geometryDlg->Create(IDD_GEOMETRY_TOOL_DLG);
		geometryDlg->SetWindowText("Geometry Tool");
		Create_ToolSkin_Frame(geometryDlg, "Geometry Tool", IDC_APPLY_GEOMETRY);
	}
	geometryDlg->GetParent()->ShowWindow(SW_SHOW);
	geometryDlg->ListLayers();
	geometryDlg->is_visible = true;
	return true;
}

bool Close_Geometry_Dialog()
{
	if(geometryDlg)
	{
		geometryDlg->ShowWindow(SW_HIDE);
		geometryDlg->is_visible = false;
	}
	return true;
}

void Set_Geometry_Dialog_To_Selection()
{
	if(geometryDlg)
	{
		if(geometryDlg->is_visible)
		{
			int mrs_index = -1;
			int mrs = Get_Most_Recently_Selected_Layer(&mrs_index);
			geometryDlg->m_LayersList.SetCurSel(mrs_index);
			geometryDlg->UpdateTypeCheckbox(mrs_index);
		}
	}
}

void GeometryToolDlg::EnableCheckboxOptions(bool b)
{
	CheckDlgButton(IDC_FLAT_PLANE, false);
	CheckDlgButton(IDC_BOX, false);
	CheckDlgButton(IDC_SPHERE, false);
	CheckDlgButton(IDC_CYLINDER, false);
	CheckDlgButton(IDC_MODEL, false);
	CheckDlgButton(IDC_INVERT_GEOMETRY, false);
	CheckDlgButton(IDC_SHOW_GEOMETRY, false);

	GetDlgItem(IDC_FLAT_PLANE)->EnableWindow(b);
	GetDlgItem(IDC_BOX)->EnableWindow(b);
	GetDlgItem(IDC_SPHERE)->EnableWindow(b);
	GetDlgItem(IDC_CYLINDER)->EnableWindow(b);
	GetDlgItem(IDC_MODEL)->EnableWindow(b);
	GetDlgItem(IDC_INVERT_GEOMETRY)->EnableWindow(b);
	GetDlgItem(IDC_SHOW_GEOMETRY)->EnableWindow(b);
}

//called if the user changes their mond given last change to save link points info
bool Force_TriMesh_Checkbox()
{
	if(geometryDlg)
	{
		geometryDlg->CheckDlgButton(IDC_FLAT_PLANE, false);
		geometryDlg->CheckDlgButton(IDC_BOX, false);
		geometryDlg->CheckDlgButton(IDC_SPHERE, false);
		geometryDlg->CheckDlgButton(IDC_CYLINDER, false);
		geometryDlg->CheckDlgButton(IDC_MODEL, true);
		geometryDlg->GetDlgItem(IDC_BROWSE_MODEL)->EnableWindow(true);
		geometryDlg->GetDlgItem(IDC_PROJECT_TO_MODEL)->EnableWindow(true);
		geometryDlg->GetDlgItem(IDC_LINK_POINTS)->EnableWindow(true);
		return true;
	}
	return false;
}

void GeometryToolDlg::UpdateTypeCheckbox(int sel_index)
{
	int type = -1;
	if(!Get_Layer_Geometry_Type(sel_index, &type))
	{
		EnableCheckboxOptions(false);
		return;
	}
	EnableCheckboxOptions(true);
	CheckDlgButton(IDC_FLAT_PLANE, type==pt_PLANE);
	CheckDlgButton(IDC_BOX, type==pt_BOX);
	CheckDlgButton(IDC_SPHERE, type==pt_SPHERE);
	CheckDlgButton(IDC_CYLINDER, type==pt_CYLINDER);
	CheckDlgButton(IDC_MODEL, type==pt_TRIMESH);

	GetDlgItem(IDC_BROWSE_MODEL)->EnableWindow(type==pt_TRIMESH);
	GetDlgItem(IDC_PROJECT_TO_MODEL)->EnableWindow(type==pt_TRIMESH);
	GetDlgItem(IDC_LINK_POINTS)->EnableWindow(type==pt_TRIMESH);
	
	CheckDlgButton(IDC_INVERT_GEOMETRY, Layer_Geometry_Inverted(sel_index));
	CheckDlgButton(IDC_SHOW_GEOMETRY, Layer_Geometry_Visible(sel_index));
}


void GeometryToolDlg::ListLayers()
{
	m_LayersList.ResetContent();
	int n = Num_Layers();
	char name[512];
	for(int i = 0;i<n;i++)
	{
		Layer_Name(i, name);
		m_LayersList.AddString(name);
	}
	int index;
	Get_Most_Recently_Selected_Layer(&index);
	if(index!=-1)
	{
		m_LayersList.SetCurSel(index);
		UpdateTypeCheckbox(index);
	}
}

void GeometryToolDlg::OnClose() 
{
	Close_Geometry_Tool();
}

void GeometryToolDlg::OnSelchangeLayersList() 
{
	Select_All_Layers(false);
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		Select_Layer(sel, true);
		UpdateTypeCheckbox(sel);
		redraw_frame = true;
	}
}

void GeometryToolDlg::OnFlatPlane() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		Set_Layer_Geometry_Type(sel, pt_PLANE);
		ReProject_Layer(sel);
		redraw_frame = true;
		Update_Selection_Status_Info();
	}
	GetDlgItem(IDC_BROWSE_MODEL)->EnableWindow(false);
	GetDlgItem(IDC_PROJECT_TO_MODEL)->EnableWindow(false);
	GetDlgItem(IDC_LINK_POINTS)->EnableWindow(false);
}

void GeometryToolDlg::OnSphere() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		Set_Layer_Geometry_Type(sel, pt_SPHERE);
		ReProject_Layer(sel);
		redraw_frame = true;
		Update_Selection_Status_Info();
	}
	GetDlgItem(IDC_BROWSE_MODEL)->EnableWindow(false);
	GetDlgItem(IDC_PROJECT_TO_MODEL)->EnableWindow(false);
	GetDlgItem(IDC_LINK_POINTS)->EnableWindow(false);
}

void GeometryToolDlg::OnCylinder() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		Set_Layer_Geometry_Type(sel, pt_CYLINDER);
		ReProject_Layer(sel);
		redraw_frame = true;
		Update_Selection_Status_Info();
	}
	GetDlgItem(IDC_BROWSE_MODEL)->EnableWindow(false);
	GetDlgItem(IDC_PROJECT_TO_MODEL)->EnableWindow(false);
	GetDlgItem(IDC_LINK_POINTS)->EnableWindow(false);
}

void GeometryToolDlg::OnBox() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		Set_Layer_Geometry_Type(sel, pt_BOX);
		ReProject_Layer(sel);
		redraw_frame = true;
		Update_Selection_Status_Info();
	}
	GetDlgItem(IDC_BROWSE_MODEL)->EnableWindow(false);
	GetDlgItem(IDC_PROJECT_TO_MODEL)->EnableWindow(false);
	GetDlgItem(IDC_LINK_POINTS)->EnableWindow(false);
}

void GeometryToolDlg::OnCone() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		Update_Selection_Status_Info();
		redraw_frame = true;
	}
	GetDlgItem(IDC_BROWSE_MODEL)->EnableWindow(false);
	GetDlgItem(IDC_PROJECT_TO_MODEL)->EnableWindow(false);
	GetDlgItem(IDC_LINK_POINTS)->EnableWindow(false);
}

void GeometryToolDlg::OnModel() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		Set_Layer_Geometry_Type(sel, pt_TRIMESH);
		redraw_frame = true;
		GetDlgItem(IDC_BROWSE_MODEL)->EnableWindow(true);
		GetDlgItem(IDC_PROJECT_TO_MODEL)->EnableWindow(true);
		GetDlgItem(IDC_LINK_POINTS)->EnableWindow(true);
		Update_Selection_Status_Info();
	}
}

void GeometryToolDlg::OnBrowseModel() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		char file[512];
		if(Browse(file, "obj", false))
		{
			Set_Layer_Geometry_Model(sel, file);
			ReProject_Layer(sel);
			redraw_frame = true;
		}
	}
	else
	{
		SkinMsgBox("No layer selected!", 0, MB_OK);
	}
}

void GeometryToolDlg::OnShowGeometry() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		bool b = IsDlgButtonChecked(IDC_SHOW_GEOMETRY)!=0;
		Show_Layer_Geometry(sel, b);
		redraw_frame = true;
	}
}

void GeometryToolDlg::OnInvertGeometry() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		bool b = IsDlgButtonChecked(IDC_INVERT_GEOMETRY)!=0;
		Invert_Layer_Geometry(sel, b);
		ReProject_Layer(sel);
		redraw_frame = true;
	}
}

void GeometryToolDlg::OnShowGeometryWireframe() 
{
	show_geometry_wireframe = IsDlgButtonChecked(IDC_SHOW_GEOMETRY_WIREFRAME)!=0;
	redraw_frame = true;
}

void GeometryToolDlg::OnProjectToModel() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		Project_Layer_To_Model_Geometry(sel);
		redraw_frame = true;
	}
}

HBRUSH GeometryToolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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

void GeometryToolDlg::OnLinkPoints() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel>-1)
	{
		if(Layer_Geometry_Model_Loaded(sel))
		{
			mainframe->SetFocus();
			int layer_id = Get_Layer_ID(sel);
			Open_Link_Points_Tool(layer_id);
		}
		else
		{
			SkinMsgBox("Can't link points, no model specified yet.");
		}
	}
}
