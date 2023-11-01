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
// HaloToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "HaloToolDlg.h"
#include "HaloTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HaloToolDlg dialog

HaloToolDlg *haloToolDlg = 0;

float max_halo_slider_range = 100;

HaloToolDlg::HaloToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(HaloToolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(HaloToolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void HaloToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(HaloToolDlg)
	DDX_Control(pDX, IDC_LOAD_HALOS_FOR_FRAME, m_LoadHalosForFrame);
	DDX_Control(pDX, IDC_AUTO_LOAD_HALOS, m_AutoLoadHalos);
	DDX_Control(pDX, IDC_CLEAR_SAVED_HALO_INFO, m_ClearSavedHaloInfo);
	DDX_Control(pDX, IDC_APPLY_SAVED_HALO_INFO, m_ApplySavedHaloInfo);
	DDX_Control(pDX, IDC_SAVE_HALO_INFO, m_SaveHaloInfo);
	DDX_Control(pDX, IDC_DELETE_ALL_HALO_MASKS, m_DeleteAllHaloMasks);
	DDX_Control(pDX, IDC_STATIC1, m_Static1);
	DDX_Control(pDX, IDC_UPDATE_ALL_HALO_GEOMETRY, m_UpdateAllHaloGeometry);
	DDX_Control(pDX, IDC_RENDER_HALO_ALPHAMASK, m_RenderHaloAlphaMask);
	DDX_Control(pDX, IDC_RENDER_3D_HALO, m_Render3DHalo);
	DDX_Control(pDX, IDC_RENDER_2D_HALO, m_Render2DHalo);
	DDX_Control(pDX, IDC_BLEND_HALO_WITH_BACKGROUND, m_BlendHaloWithBackground);
	DDX_Control(pDX, IDC_SAVE_HALO_MASK, m_SaveHaloMask);
	DDX_Control(pDX, IDC_DELETE_HALO_MASK, m_DeleteHaloMask);
	DDX_Control(pDX, IDC_SORT_HALOS_MODIFY_FRAME, m_SortHalosModifyFrame);
	DDX_Control(pDX, IDC_GENERATE_DEFAULT_HALOS, m_GenerateDefaultHalos);
	DDX_Control(pDX, IDC_LAYERS_LIST, m_LayersList);
	DDX_Control(pDX, IDC_UPDATE_HALO_GEOMETRY, m_UpdateHaloGeometry);
	DDX_Control(pDX, IDC_FIND_HALO_OUTLINE, m_FindHaloOutline);
	DDX_Control(pDX, IDC_HALO_RANGE_SLIDER, m_HaloRangeSlider);
	DDX_Control(pDX, IDC_HALO_PIXEL_RANGE, m_HaloPixelRange);
	DDX_Control(pDX, IDC_GENERATE_HALO_MASK, m_GenerateHaloMask);
	//}}AFX_DATA_MAP

	m_LoadHalosForFrame.SetFont(skin_font);
	m_AutoLoadHalos.SetFont(skin_font);
	m_ClearSavedHaloInfo.SetFont(skin_font);
	m_ApplySavedHaloInfo.SetFont(skin_font);
	m_SaveHaloInfo.SetFont(skin_font);
	m_DeleteAllHaloMasks.SetFont(skin_font);
	m_SaveHaloMask.SetFont(skin_font);
	m_DeleteHaloMask.SetFont(skin_font);
	m_SortHalosModifyFrame.SetFont(skin_font);
	m_HaloRangeSlider.SetRange(0, 1000);
	m_HaloRangeSlider.SetFont(skin_font);
	m_HaloPixelRange.SetFont(skin_font);
	m_GenerateHaloMask.SetFont(skin_font);
	m_FindHaloOutline.SetFont(skin_font);
	m_UpdateHaloGeometry.SetFont(skin_font);
	m_GenerateDefaultHalos.SetFont(skin_font);

	m_BlendHaloWithBackground.SetFont(skin_font);
	m_Render2DHalo.SetFont(skin_font);
	m_Render3DHalo.SetFont(skin_font);
	m_RenderHaloAlphaMask.SetFont(skin_font);
	m_UpdateAllHaloGeometry.SetFont(skin_font);
	m_Static1.SetFont(skin_font);
	
	CheckDlgButton(IDC_RENDER_2D_HALO, render_2d_halo);
	CheckDlgButton(IDC_RENDER_3D_HALO, render_3d_halo);
	CheckDlgButton(IDC_BLEND_HALO_WITH_BACKGROUND, blend_halo_with_background);
	CheckDlgButton(IDC_RENDER_HALO_ALPHAMASK, render_halo_alphamask);
	CheckDlgButton(IDC_AUTO_LOAD_HALOS, auto_load_halos);
}


BEGIN_MESSAGE_MAP(HaloToolDlg, CDialog)
	//{{AFX_MSG_MAP(HaloToolDlg)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_GENERATE_HALO_MASK, OnGenerateHaloMask)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_FIND_HALO_OUTLINE, OnFindHaloOutline)
	ON_BN_CLICKED(IDC_RENDER_2D_HALO, OnRender2dHalo)
	ON_BN_CLICKED(IDC_BLEND_HALO_WITH_BACKGROUND, OnBlendHaloWithBackground)
	ON_BN_CLICKED(IDC_UPDATE_HALO_GEOMETRY, OnUpdateHaloGeometry)
	ON_BN_CLICKED(IDC_RENDER_3D_HALO, OnRender3dHalo)
	ON_BN_CLICKED(IDC_RENDER_HALO_ALPHAMASK, OnRenderHaloAlphamask)
	ON_LBN_SELCHANGE(IDC_LAYERS_LIST, OnSelchangeLayersList)
	ON_BN_CLICKED(IDC_GENERATE_DEFAULT_HALOS, OnGenerateDefaultHalos)
	ON_BN_CLICKED(IDC_SORT_HALOS_MODIFY_FRAME, OnSortHalosModifyFrame)
	ON_BN_CLICKED(IDC_DELETE_HALO_MASK, OnDeleteHaloMask)
	ON_BN_CLICKED(IDC_SAVE_HALO_MASK, OnSaveHaloMask)
	ON_BN_CLICKED(IDC_UPDATE_ALL_HALO_GEOMETRY, OnUpdateAllHaloGeometry)
	ON_BN_CLICKED(IDC_DELETE_ALL_HALO_MASKS, OnDeleteAllHaloMasks)
	ON_BN_CLICKED(IDC_SAVE_HALO_INFO, OnSaveHaloInfo)
	ON_BN_CLICKED(IDC_APPLY_SAVED_HALO_INFO, OnApplySavedHaloInfo)
	ON_BN_CLICKED(IDC_CLEAR_SAVED_HALO_INFO, OnClearSavedHaloInfo)
	ON_BN_CLICKED(IDC_AUTO_LOAD_HALOS, OnAutoLoadHalos)
	ON_BN_CLICKED(IDC_LOAD_HALOS_FOR_FRAME, OnLoadHalosForFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HaloToolDlg message handlers

bool Open_Halo_Dlg()
{
	if(!haloToolDlg)
	{
		haloToolDlg = new HaloToolDlg;
		haloToolDlg->Create(IDD_HALO_TOOL_DLG);
		haloToolDlg->SetWindowText("Halo Tool");
		Create_ToolSkin_Frame(haloToolDlg, "Halo Tool", IDC_HALO_TOOL);
	}
	haloToolDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_Halo_Dlg()
{
	haloToolDlg->ShowWindow(SW_HIDE);
	return true;
}

bool Update_Halo_Range_Slider()
{
	float v = Halo_Pixel_Range();
	char text[512];
	sprintf(text, "%.2f", v);
	haloToolDlg->m_HaloPixelRange.SetWindowText(text);
	float p = v/max_halo_slider_range;
	float sp = p*1000;
	int spos = (int)sp;
	haloToolDlg->m_HaloRangeSlider.SetPos(spos);
	return true;
}

void HaloToolDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	Close_Halo_Tool();
}

HBRUSH HaloToolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_HALO_PIXEL_RANGE)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
}

void HaloToolDlg::OnGenerateHaloMask() 
{
	Generate_Halo_Mask();
	redraw_edit_window = true;
	redraw_frame = true;
}

bool Enable_Generate_Halo_Button(bool b)
{
	if(haloToolDlg)
	{
		if(b)
		{
			haloToolDlg->m_GenerateHaloMask.SetWindowText("Generate Halo Mask");
		}
		else
		{
			haloToolDlg->m_GenerateHaloMask.SetWindowText("Working...");
		}
		haloToolDlg->m_GenerateHaloMask.EnableWindow(b);
		haloToolDlg->m_GenerateHaloMask.render_pushed = !b;
	}
	return false;
}

void HaloToolDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	int spos = haloToolDlg->m_HaloRangeSlider.GetPos();
	float p = ((float)spos)/1000;
	if(p>1)p = 1;
	float v = p*max_halo_slider_range;
	char text[512];
	sprintf(text, "%.2f", v);
	m_HaloPixelRange.SetWindowText(text);
	Set_Halo_Pixel_Range(v);
	redraw_edit_window = true;
}

void HaloToolDlg::OnFindHaloOutline() 
{
}

void HaloToolDlg::OnRender2dHalo() 
{
	render_2d_halo = IsDlgButtonChecked(IDC_RENDER_2D_HALO)!=0;
	redraw_edit_window = true;
}

void HaloToolDlg::OnBlendHaloWithBackground() 
{
	blend_halo_with_background = IsDlgButtonChecked(IDC_BLEND_HALO_WITH_BACKGROUND)!=0;
	redraw_edit_window = true;
}

void HaloToolDlg::OnUpdateHaloGeometry() 
{
	Update_Halo_Geometry();
	redraw_frame = true;
}

void HaloToolDlg::OnRender3dHalo() 
{
	render_3d_halo = IsDlgButtonChecked(IDC_RENDER_3D_HALO)!=0;
	redraw_frame = true;
}

void HaloToolDlg::OnRenderHaloAlphamask() 
{
	render_halo_alphamask = IsDlgButtonChecked(IDC_RENDER_HALO_ALPHAMASK)!=0;
	redraw_edit_window = true;
}

void HaloToolDlg::ListLayers()
{
	m_LayersList.ResetContent();
	int n = Num_Layers();
	char name[512];
	for(int i = 0;i<n;i++)
	{
		Layer_Name(i, name);
		m_LayersList.AddString(name);
	}
}

void List_Halo_Tool_Layers()
{
	if(haloToolDlg)
	{
		haloToolDlg->ListLayers();
	}
}

bool Select_Halo_Tool_Layer(int layer_id)
{
	if(haloToolDlg)
	{
		int index = Get_Layer_Index(layer_id);
		if(index!=-1)
		{
			char name[512];
			Layer_Name(index, name);
			int li = haloToolDlg->m_LayersList.FindStringExact(0, name);
			if(li!=-1)
			{
				haloToolDlg->m_LayersList.SetCurSel(index);
				return true;
			}
		}
	}
	return false;
}

void HaloToolDlg::OnSelchangeLayersList() 
{
	int sel = m_LayersList.GetCurSel();
	if(sel!=-1)
	{
		Select_All_Layers(false);
		Select_Layer(sel, true);
		Change_Active_Halo_Layer(sel);
		redraw_edit_window = true;
	}
}

void HaloToolDlg::OnGenerateDefaultHalos() 
{
	Generate_Default_Halos();
}

void HaloToolDlg::OnSortHalosModifyFrame() 
{
	redraw_edit_window = true;
	redraw_frame = true;
}

void HaloToolDlg::OnDeleteHaloMask() 
{
	Delete_Halo_Mask();
	redraw_edit_window = true;
	redraw_frame = true;
}

void HaloToolDlg::OnSaveHaloMask() 
{
}

void HaloToolDlg::OnUpdateAllHaloGeometry() 
{
	Update_All_Halo_Geometry();
	redraw_frame = true;
}

void HaloToolDlg::OnDeleteAllHaloMasks() 
{
	Delete_All_Halo_Masks();
	redraw_edit_window = true;
	redraw_frame = true;
}

void HaloToolDlg::OnSaveHaloInfo() 
{
	Save_Halo_Info();
	redraw_edit_window = true;
	redraw_frame = true;
}

void HaloToolDlg::OnApplySavedHaloInfo() 
{
	Apply_Saved_Halo_Info();
	redraw_edit_window = true;
	redraw_frame = true;
}

void HaloToolDlg::OnClearSavedHaloInfo() 
{
	Clear_Saved_Halo_Info();
	redraw_edit_window = true;
	redraw_frame = true;
}

void HaloToolDlg::OnAutoLoadHalos() 
{
	auto_load_halos = IsDlgButtonChecked(IDC_AUTO_LOAD_HALOS)!=0;
}

void HaloToolDlg::OnLoadHalosForFrame() 
{
	Load_Halos_For_Frame();
	redraw_edit_window = true;
	redraw_frame = true;
}
