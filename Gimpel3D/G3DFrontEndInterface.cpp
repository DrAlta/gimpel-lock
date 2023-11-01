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
#include "stdafx.h"
#include "G3DCoreInterface.h"


#include "AlignmentOptionsDlg.h"
#include "ContourExtrusionDlg.h"
#include "DepthSliderDlg.h"
#include "ExportOptionsDlg.h"
#include "ImportOptionsDlg.h"
#include "PlanarProjectionDlg.h"
#include "ProjectionSettingsDlg.h"
#include "ReliefMapDlg.h"
#include "ScaleSliderDlg.h"
#include "StereoSettingsDlg.h"

#include "G3DMainFrame.h"

extern bool gapfill_tool_open;


bool wide_screen = false;
bool dual_monitor = false;
bool split_screen = false;
bool fullscreen_2d = false;
bool fullscreen_3d = false;

bool swap_monitors = false;

AlignmentOptionsDlg *_alignmentOptionsDlg = 0;
ContourExtrusionDlg *_contourExtrusionDlg = 0;
DepthSliderDlg *_depthSliderDlg = 0;
ExportOptionsDlg *_exportOptionsDlg = 0;
ImportOptionsDlg *_importOptionsDlg = 0;
PlanarProjectionDlg *_planeDlg = 0;
PlanarProjectionDlg *_planarProjectionDlg = 0;
ProjectionSettingsDlg *_projectionSettingsDlg = 0;
ReliefMapDlg *_reliefMapDlg = 0;
ScaleSliderDlg *_scaleSliderDlg = 0;
StereoSettingsDlg *_stereoDlg = 0;

bool Resize_ToolTray_Dlg();


bool Open_Stereo_Settings_Dlg()
{
	if(!_stereoDlg)
	{
		_stereoDlg = new StereoSettingsDlg;
		_stereoDlg->Create(IDD_STEREO_SETTINGS_DLG);
		_stereoDlg->SetWindowText("Stereo Settings");
		Create_ToolSkin_Frame(_stereoDlg, "Stereo Settings", IDC_STEREO_SETTINGS);
	}
	_stereoDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

void Update_Stereo_Sliders()
{
	if(_stereoDlg)
	{
		_stereoDlg->UpdateSliderPositions();
	}
}

void Update_Stereo_Keyframe_Buttons(int frame_index)
{
	if(_stereoDlg)
	{
		if(frame_index==0||frame_index==Get_Num_Frames()-1)
		{
			_stereoDlg->GetDlgItem(IDC_KEYFRAME_STEREO)->EnableWindow(false);
		}
		else
		{
			_stereoDlg->GetDlgItem(IDC_KEYFRAME_STEREO)->EnableWindow(true);
		}
		_stereoDlg->CheckDlgButton(IDC_KEYFRAME_STEREO, Stereo_Settings_Keyframed(frame_index));
	}
}

bool Open_Scale_Slider_Dlg()
{
	if(!_scaleSliderDlg)
	{
		_scaleSliderDlg = new ScaleSliderDlg;
		_scaleSliderDlg->Create(IDD_SCALE_SLIDER);
		_scaleSliderDlg->SetWindowText("Scale");
		Create_ToolSkin_Frame(_scaleSliderDlg, "Scale", IDC_OPEN_SCALE_SLIDER);
	}
	Set_Scale_Slider_To_Selection();
	_scaleSliderDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

void Set_Scale_Slider_To_Selection()
{
	if(_scaleSliderDlg)
	{
		_scaleSliderDlg->UpdateSliderPosition();
	}
}

void Set_Relief_To_Selection()
{
	if(!_reliefMapDlg)return;
	int n = Num_Layers();
	int num_selected = 0;
	char layer_name[64];
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Get_Layer_Relief_Info(i, &relief_info);
			Layer_Name(i, layer_name);
			num_selected++;
		}
	}
	_reliefMapDlg->SetReliefInfo(&relief_info);
	char title[256];
	if(num_selected==0)
	{
		sprintf(title, "Relief Texture: No Selection");
	}
	else if(num_selected==1)
	{
		sprintf(title, "Relief Texture For Layer \"%s\"", layer_name);
	}
	else
	{
		sprintf(title, "Relief Texture For Multiple Selection.");
	}
	_reliefMapDlg->SetWindowText(title);
	_reliefMapDlg->GetParent()->Invalidate();
}


bool Close_Rotoscope_Color_Difference_Dlg()
{
	_reliefMapDlg->GetDlgItem(IDC_SMOOTH_TRANSITION)->ShowWindow(true);
	_reliefMapDlg->GetDlgItem(IDC_RANDOM_NOISE)->ShowWindow(true);
	_reliefMapDlg->GetDlgItem(IDC_RELIEF_IMAGE)->ShowWindow(true);
	_reliefMapDlg->GetDlgItem(IDC_COLOR_DIFFERENCE)->ShowWindow(true);
	_reliefMapDlg->SetDlgItemText(IDC_CONCAVE_COLOR_LABEL, "Concave");
	_reliefMapDlg->SetDlgItemText(IDC_CONVEX_COLOR_LABEL, "Convex");
	_reliefMapDlg->GetDlgItem(IDC_SCALE_LABEL)->ShowWindow(true);
	_reliefMapDlg->GetDlgItem(IDC_SCALE)->ShowWindow(true);
	_reliefMapDlg->GetDlgItem(IDC_SCALE_SLIDER)->ShowWindow(true);
	_reliefMapDlg->GetDlgItem(IDC_SET_SCALE)->ShowWindow(true);
	_reliefMapDlg->GetDlgItem(IDC_SLIDER_RANGE_LABEL)->ShowWindow(true);
	_reliefMapDlg->GetDlgItem(IDC_SCALE_SLIDER_RANGE)->ShowWindow(true);
	_reliefMapDlg->GetDlgItem(IDC_SET_SCALE_SLIDER_RANGE)->ShowWindow(true);
	_reliefMapDlg->GetDlgItem(IDC_FLIP_SCALE_SLIDER_RANGE)->ShowWindow(true);
	_reliefMapDlg->SetWindowText("Relief Map");
	_reliefMapDlg->GetParent()->Invalidate();
	render_2d_relief_layer_split = false;
	rotoscope_color_difference_mode = false;
	Clear_Layer_Split_Pixels();
	return true;
}

bool Open_Rotoscope_Color_Difference_Dlg()
{
	Open_Relief_Map_Dlg();
	rotoscope_color_difference_mode = true;
	render_2d_relief_layer_split = true;
	redraw_frame = true;
	render_relief_preview = true;
	_reliefMapDlg->SetFocus();
	relief_info.type = RELIEF_TYPE_BRIGHTNESS;
	_reliefMapDlg->EnableControls();
	_reliefMapDlg->SetReliefInfo(&relief_info);
	_reliefMapDlg->GetDlgItem(IDC_SMOOTH_TRANSITION)->ShowWindow(false);
	_reliefMapDlg->GetDlgItem(IDC_RANDOM_NOISE)->ShowWindow(false);
	_reliefMapDlg->GetDlgItem(IDC_RELIEF_IMAGE)->ShowWindow(false);
	_reliefMapDlg->GetDlgItem(IDC_COLOR_DIFFERENCE)->ShowWindow(false);
	_reliefMapDlg->SetDlgItemText(IDC_CONCAVE_COLOR_LABEL, "Layer 1");
	_reliefMapDlg->SetDlgItemText(IDC_CONVEX_COLOR_LABEL, "Layer 2");
	_reliefMapDlg->GetDlgItem(IDC_SCALE_LABEL)->ShowWindow(false);
	_reliefMapDlg->GetDlgItem(IDC_SCALE)->ShowWindow(false);
	_reliefMapDlg->GetDlgItem(IDC_SCALE_SLIDER)->ShowWindow(false);
	_reliefMapDlg->GetDlgItem(IDC_SET_SCALE)->ShowWindow(false);
	_reliefMapDlg->GetDlgItem(IDC_SLIDER_RANGE_LABEL)->ShowWindow(false);
	_reliefMapDlg->GetDlgItem(IDC_SCALE_SLIDER_RANGE)->ShowWindow(false);
	_reliefMapDlg->GetDlgItem(IDC_SET_SCALE_SLIDER_RANGE)->ShowWindow(false);
	_reliefMapDlg->GetDlgItem(IDC_FLIP_SCALE_SLIDER_RANGE)->ShowWindow(false);
	_reliefMapDlg->GetDlgItem(IDC_SPLIT_LAYER)->ShowWindow(true);
	_reliefMapDlg->SetWindowText("Split Layer");
	_reliefMapDlg->GetParent()->Invalidate();

	Update_Selection_Relief(true);
	
	Init_Relief_Layer_Split_Pixels();
	redraw_edit_window = true;
	return true;
}

bool Open_Relief_Map_Dlg()
{
	if(!_reliefMapDlg)
	{
		_reliefMapDlg = new ReliefMapDlg;
		_reliefMapDlg->Create(IDD_RELIEF_MAP_DLG);
		Create_ToolSkin_Frame(_reliefMapDlg, "Relief Map", IDC_RELIEF_MAP);
	}
	if(rotoscope_color_difference_mode)
	{
		Close_Rotoscope_Color_Difference_Dlg();
	}
	rotoscope_color_difference_mode = false;
	_reliefMapDlg->GetParent()->ShowWindow(SW_SHOW);
	redraw_frame = true;
	redraw_edit_window = true;
	render_relief_preview = true;
	Set_Relief_To_Selection();
	_reliefMapDlg->SetFocus();
	return true;
}


void Update_Concave_Color(unsigned char *rgb)
{
	if(!_reliefMapDlg)return;
	_reliefMapDlg->m_ConcaveColorButton.Set_Color(rgb[0], rgb[1], rgb[2]);
	CWnd *b = _reliefMapDlg->GetDlgItem(IDC_CONCAVE_COLOR_BUTTON);
	b->RedrawWindow();
}

void Update_Convex_Color(unsigned char *rgb)
{
	if(!_reliefMapDlg)return;
	_reliefMapDlg->m_ConvexColorButton.Set_Color(rgb[0], rgb[1], rgb[2]);
	CWnd *b = _reliefMapDlg->GetDlgItem(IDC_CONVEX_COLOR_BUTTON);
	b->RedrawWindow();
}

void Select_Concave_Color(unsigned char *rgb)
{
	if(!_reliefMapDlg)return;
	_reliefMapDlg->m_ConcaveColorButton.Set_Color(rgb[0], rgb[1], rgb[2]);
	relief_info.concave_color[0] = rgb[0];
	relief_info.concave_color[1] = rgb[1];
	relief_info.concave_color[2] = rgb[2];
	pick_concave_color = false;
	_reliefMapDlg->CheckDlgButton(IDC_PICK_CONCAVE_COLOR, false);
	CWnd *b = _reliefMapDlg->GetDlgItem(IDC_CONCAVE_COLOR_BUTTON);
	b->RedrawWindow();
	Update_Selection_Relief(true);
	if(rotoscope_color_difference_mode)
	{
		Update_Relief_Layer_Split_Pixels();
	}
}

void Select_Convex_Color(unsigned char *rgb)
{
	if(!_reliefMapDlg)return;
	_reliefMapDlg->m_ConvexColorButton.Set_Color(rgb[0], rgb[1], rgb[2]);
	relief_info.convex_color[0] = rgb[0];
	relief_info.convex_color[1] = rgb[1];
	relief_info.convex_color[2] = rgb[2];
	pick_convex_color = false;
	_reliefMapDlg->CheckDlgButton(IDC_PICK_CONVEX_COLOR, false);
	CWnd *b = _reliefMapDlg->GetDlgItem(IDC_CONVEX_COLOR_BUTTON);
	b->RedrawWindow();
	Update_Selection_Relief(true);
	if(rotoscope_color_difference_mode)
	{
		Update_Relief_Layer_Split_Pixels();
	}
}

void Display_FOV(float fov)
{
	if(_projectionSettingsDlg)
	{
		char text[32];
		sprintf(text, "%.2f", fov);
		_projectionSettingsDlg->SetDlgItemText(IDC_FOV, text);
	}
}

#include "3DViewOptions.h"
extern C3DViewOptions *_3D_View_Options_Dlg;

bool Open_Projection_Settings_Dlg()
{
	if(!_projectionSettingsDlg)
	{
		_projectionSettingsDlg = new ProjectionSettingsDlg;
		_projectionSettingsDlg->Create(IDD_PROJECTION_SETTINGS_DLG);
		_projectionSettingsDlg->SetWindowText("Projection Settings");
		Create_Skin_Frame(_projectionSettingsDlg);
	}
	Display_FOV(_fov);
	_projectionSettingsDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}


bool Open_Planar_Projection_Dlg()
{
	if(!_planarProjectionDlg)
	{
		_planarProjectionDlg = new PlanarProjectionDlg;
		_planarProjectionDlg->Create(IDD_PLANAR_PROJECTION_DLG);
		_planarProjectionDlg->SetWindowText("Object Orientation");
		Create_ToolSkin_Frame(_planarProjectionDlg, "Orientation", IDC_PLANAR_PROJECTION);
	}
	_planarProjectionDlg->is_visible = true;
	Set_Plane_Dialog_To_Selection();
	redraw_frame = true;
	_planarProjectionDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

void Reset_Plane_Sliders()
{
	horizontal_rotation = 0;
	vertical_rotation = 0;
	z_rotation = 0;
	horizontal_position = 0;
	vertical_position = 0;
	depth_position = 0;
	if(_planeDlg)
	{
		_planeDlg->UpdateSliderPositions();
		_planeDlg->UpdateSliders();
	}
}

void Update_Planar_GUI()
{
	if(_planeDlg)
	{
		if(_planeDlg->is_visible)
		{
			char text[32];
			sprintf(text, "%f", vertical_rotation);_planeDlg->SetDlgItemText(IDC_VERTICAL_ROTATION, text);
			sprintf(text, "%f", horizontal_rotation);_planeDlg->SetDlgItemText(IDC_HORIZONTAL_ROTATION, text);
			sprintf(text, "%f", z_rotation);_planeDlg->SetDlgItemText(IDC_Z_ROTATION, text);
			_planeDlg->UpdateSliderPositions();
			_planeDlg->CheckDlgButton(IDC_LOCK_TOP, plane_origin_type==PLANE_ORIGIN_SELECTION_TOP);
			_planeDlg->CheckDlgButton(IDC_LOCK_LEFT, plane_origin_type==PLANE_ORIGIN_SELECTION_LEFT);
			_planeDlg->CheckDlgButton(IDC_LOCK_CENTER, plane_origin_type==PLANE_ORIGIN_SELECTION_CENTER);
			_planeDlg->CheckDlgButton(IDC_LOCK_RIGHT, plane_origin_type==PLANE_ORIGIN_SELECTION_RIGHT);
			_planeDlg->CheckDlgButton(IDC_LOCK_BOTTOM, plane_origin_type==PLANE_ORIGIN_SELECTION_BOTTOM);
		}
	}
}

void Update_Plane_Title()
{
	if(_planeDlg)
	{
		char text[256];
		int n = Num_Selected_Layers();
		if(n==0)
		{
			_planeDlg->SetWindowText("Object Orientation: No Selection");
			_planeDlg->CenterSliders();
		}
		else if(n==1)
		{
			char name[64];
			int nl = Num_Layers();
			for(int i = 0;i<nl;i++)
			{
				if(Layer_Is_Selected(i))
				{
					Layer_Name(i, name);
				}
			}
			sprintf(text, "Object Orientation For Layer: \"%s\"", name);
			_planeDlg->SetWindowText(text);
		}
		else
		{
			_planeDlg->SetWindowText("Object Orientation For Multiple Selection");
		}
		_planeDlg->GetParent()->Invalidate();
	}
}

bool Open_Import_Options_Dialog()
{
	if(!_importOptionsDlg)
	{
		_importOptionsDlg = new ImportOptionsDlg;
		_importOptionsDlg->Create(IDD_IMPORT_OPTIONS);
		_importOptionsDlg->SetWindowText("Import Options");
		Create_ToolSkin_Frame(_importOptionsDlg, "Import Options", IDC_IMPORT);
	}
	_importOptionsDlg->GetParent()->ShowWindow(SW_SHOW);
	_importOptionsDlg->UpdateData();
	_importOptionsDlg->UpdatePaths();
	return true;
}

bool Open_Export_Options_Dialog()
{
	if(!_exportOptionsDlg)
	{
		_exportOptionsDlg = new ExportOptionsDlg;
		_exportOptionsDlg->Create(IDD_EXPORT_OPTIONS);
		_exportOptionsDlg->SetWindowText("Export Options");
		Create_ToolSkin_Frame(_exportOptionsDlg, "Export Options", IDC_EXPORT);
	}
	_exportOptionsDlg->UpdateData();
	_exportOptionsDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Open_Depth_Slider_Dlg()
{
	if(!_depthSliderDlg)
	{
		_depthSliderDlg = new DepthSliderDlg;
		_depthSliderDlg->Create(IDD_DEPTH_SLIDER);
		_depthSliderDlg->SetWindowText("Depth");
		Create_ToolSkin_Frame(_depthSliderDlg, "Depth", IDC_OPEN_DEPTH_SLIDER);
	}
	Set_Depth_Slider_To_Selection();
	_depthSliderDlg->SetFocus();
	_depthSliderDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

void Set_Depth_Slider_To_Selection()
{
	if(_depthSliderDlg)
	{
		_depthSliderDlg->UpdateSliderPosition();
	}
}

void Update_Depth_Slider_Position()
{
	if(_depthSliderDlg)
	{
		_depthSliderDlg->UpdateSliderPosition();
	}
}

void Update_Contour_Dialog()
{
	if(!_contourExtrusionDlg)return;
	char text[32];
	sprintf(text, "%i", puff_preview_spacing);_contourExtrusionDlg->SetDlgItemText(IDC_PREVIEW_SKIP, text);
	sprintf(text, "%i", puff_search_range);_contourExtrusionDlg->SetDlgItemText(IDC_SEARCH_RANGE, text);
	sprintf(text, "%f", puff_range);_contourExtrusionDlg->SetDlgItemText(IDC_PUFF_RANGE, text);
	sprintf(text, "%f", puff_scale);_contourExtrusionDlg->SetDlgItemText(IDC_PUFF_SCALE, text);
	sprintf(text, "%f", directional_puff_angle);_contourExtrusionDlg->SetDlgItemText(IDC_EXTRUSION_ANGLE, text);
	_contourExtrusionDlg->UpdateSlider();
	_contourExtrusionDlg->CheckDlgButton(IDC_SPHERICAL_EXTRUSION, current_puff_type==PUFF_SPHERICAL);
	_contourExtrusionDlg->CheckDlgButton(IDC_LINEAR_EXTRUSION, current_puff_type==PUFF_LINEAR);
	_contourExtrusionDlg->CheckDlgButton(IDC_DIRECTIONAL_EXTRUSION, directional_extrusion);
}

bool Open_Contour_Extrusion_Dlg()
{
	if(!_contourExtrusionDlg)
	{
		_contourExtrusionDlg = new ContourExtrusionDlg;
		_contourExtrusionDlg->Create(IDD_CONTOUR_EXTRUSION_DIALOG);
		_contourExtrusionDlg->SetWindowText("Contour Extrusion");
		Create_ToolSkin_Frame(_contourExtrusionDlg, "Contour Extrusion", IDC_CONTOUR_EXTRUSION);
	}
	render_puff_preview = true;
	redraw_frame = true;
	Get_Preview_Puff_Pixels();
	Set_Contour_To_Selection();
	_contourExtrusionDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

void Set_Contour_To_Selection()
{
	if(!_contourExtrusionDlg)return;
	int ptype = -1;;
	if(!Get_Selection_Puff_Info(&puff_search_range, &puff_scale, &puff_range, &ptype, &directional_extrusion, &directional_puff_angle))
	{
		puff_scale = 0;
		directional_puff_angle = 180;
	}
	if(ptype!=-1)
	{
		current_puff_type = ptype;
		_contourExtrusionDlg->CheckDlgButton(IDC_SPHERICAL_EXTRUSION, current_puff_type==PUFF_SPHERICAL);
		_contourExtrusionDlg->CheckDlgButton(IDC_LINEAR_EXTRUSION, current_puff_type==PUFF_LINEAR);
	}
	if(_contourExtrusionDlg)
	{
		char text[32];
		sprintf(text, "%i", puff_preview_spacing);_contourExtrusionDlg->SetDlgItemText(IDC_PREVIEW_SKIP, text);
		sprintf(text, "%i", puff_search_range);_contourExtrusionDlg->SetDlgItemText(IDC_SEARCH_RANGE, text);
		sprintf(text, "%f", puff_range);_contourExtrusionDlg->SetDlgItemText(IDC_PUFF_RANGE, text);
		sprintf(text, "%f", puff_scale);_contourExtrusionDlg->SetDlgItemText(IDC_PUFF_SCALE, text);
		sprintf(text, "%f", directional_puff_angle);_contourExtrusionDlg->SetDlgItemText(IDC_EXTRUSION_ANGLE, text);
		_contourExtrusionDlg->CheckDlgButton(IDC_DIRECTIONAL_EXTRUSION, directional_extrusion);
		last_selection_puff_angle = directional_puff_angle;
		_contourExtrusionDlg->UpdateSlider();
		Set_Puff_Search_Range(puff_search_range);
		Get_Preview_Puff_Pixels();
	}
	char text[256];
	int n = Num_Selected_Layers();
	if(n==0)
	{
		_contourExtrusionDlg->SetWindowText("Contour Extrusion: No Selection");
	}
	else if(n==1)
	{
		char name[64];
		int nl = Num_Layers();
		for(int i = 0;i<nl;i++)
		{
			if(Layer_Is_Selected(i))
			{
				Layer_Name(i, name);
			}
		}
		sprintf(text, "Contour Extrusion For Layer: \"%s\"", name);
		_contourExtrusionDlg->SetWindowText(text);
	}
	else
	{
		_contourExtrusionDlg->SetWindowText("Contour Extrusion For Multiple Selection");
	}
	_contourExtrusionDlg->GetParent()->Invalidate();
}

void Print_Contour_Progress(char *layer_name, int percent)
{
	char text[256];
	sprintf(text, "Contouring layer: \"%s\" - %i percent", layer_name, percent);
	Print_Status(text);
}

void Enable_Start_Auto_Alignment_Button(bool b)
{
	if(_alignmentOptionsDlg)
	{
		CWnd *w = _alignmentOptionsDlg->GetDlgItem(IDC_START_AUTO_ALIGNMENT);
		w->EnableWindow(b);
	}
}

void Get_Alignment_Error_Text(char *res)
{
	_alignmentOptionsDlg->GetDlgItemText(IDC_TOTAL_ERROR, res, 32);
}

void Set_Alignment_Error_Text(char *text)
{
	_alignmentOptionsDlg->SetDlgItemText(IDC_TOTAL_ERROR, text);
}

bool Open_Alignment_Options_Dialog()
{
	if(!_alignmentOptionsDlg)
	{
		_alignmentOptionsDlg = new AlignmentOptionsDlg;
		_alignmentOptionsDlg->Create(IDD_ALIGNMENT_OPTIONS);
		_alignmentOptionsDlg->SetWindowText("Alignment Options");
		Create_ToolSkin_Frame(_alignmentOptionsDlg, "Alignment Options", IDC_AUTO_ALIGNMENT);
	}
	_alignmentOptionsDlg->GetParent()->ShowWindow(SW_SHOW);
	_alignmentOptionsDlg->UpdateData();
	return true;
}

void Get_Alignment_Angle_Text(char *text)
{
	_alignmentOptionsDlg->GetDlgItemText(IDC_ALIGNMENT_ANGLE, text, 32);
}

#include "GetTextDlg.h"

bool Get_User_Text(char *text, char *title)
{
	GetTextDlg dlg;
	strcpy(dlg.text, text);
	strcpy(dlg.title, title);
	dlg.DoModal();
	if(dlg.save_text)
	{
		strcpy(text, dlg.text);
		return true;
	}
	return false;
}

bool Fit_2D_Window()
{
	CRect rect;
	mainwindow->GetClientRect(&rect);
	int width = rect.Width()-(window_left_border+window_right_border);
	int height = rect.Height()-(window_top_border+window_bottom_border);
	SetWindowPos(Get_2D_Window(), 0, window_left_border, window_top_border, width, height, 0);
	return true;
}

bool Fit_GapFill_Window()
{
	CRect rect;
	mainwindow->GetClientRect(&rect);
	int width = rect.Width()-(window_left_border+window_right_border);
	int height = rect.Height()-(window_top_border+window_bottom_border);
	SetWindowPos(Get_GapFill_Window(), 0, window_left_border, window_top_border, width, height, 0);
	return true;
}

bool Fit_Splitscreen_Window()
{
	CRect rect;
	mainwindow->GetClientRect(&rect);
	int width = rect.Width()-(window_left_border+window_right_border);
	int height = rect.Height()-(window_top_border+window_bottom_border);
	SetWindowPos(Get_2D_Window(), 0, (window_left_border+(width/2))+1, window_top_border, (width/2)-1, height, 0);
	return true;
}

bool Fit_3D_Window()
{
	if(dual_monitor)
	{
		RECT r;
		HWND h = Get_3D_Frame_Window();
		GetClientRect(h, &r);
		CRect rect(r);
		int width = rect.Width();
		int height = rect.Height();
		int top_border = 0;
		if(Frame_Height()!=0&&wide_screen)
		{
			float fh = width*(((float)Frame_Height())/Frame_Width());
			if(fh>height)
			{
				float fw =  width*(((float)height)/fh);
				fh = (float)height;
			}
			top_border += (int)((height-fh)/2);
			height = (int)fh;
		}
		::SetWindowPos(Get_3D_Window(), 0, 0, top_border, width, height, 0);
		return true;
	}
	else if(split_screen)
	{
		CRect rect;
		mainwindow->GetClientRect(&rect);
		int top_border = window_top_border;
		int width = rect.Width()-(window_left_border+window_right_border);
		int height = rect.Height()-(window_top_border+window_bottom_border);
		width = width/2;
		if(Frame_Height()!=0&&wide_screen)
		{
			float fh = width*(((float)Frame_Height())/Frame_Width());
			if(fh>height)
			{
				float fw =  width*(((float)height)/fh);
				fh = (float)height;
			}
			top_border += (int)((height-fh)/2);
			height = (int)fh;
		}
		::SetWindowPos(Get_3D_Window(), 0, window_left_border, top_border, width, height, 0);
	}
	else
	{
		CRect rect;
		mainwindow->GetClientRect(&rect);
		int top_border = window_top_border;
		int width = rect.Width()-(window_left_border+window_right_border);
		int height = rect.Height()-(window_top_border+window_bottom_border);
		if(Frame_Height()!=0&&wide_screen)
		{
			float fh = width*(((float)Frame_Height())/Frame_Width());
			if(fh>height)
			{
				float fw =  width*(((float)height)/fh);
				fh = (float)height;
			}
			top_border += (int)((height-fh)/2);
			height = (int)fh;
		}
		::SetWindowPos(Get_3D_Window(), 0, window_left_border, top_border, width, height, 0);
	}
	return true;
}

bool Get_Desktop_Dimensions(int *width, int *height)
{
	HDC mhDC = CreateDC("DISPLAY", 0, 0, 0); 
	*width = GetDeviceCaps(mhDC, HORZRES); 
	*height = GetDeviceCaps(mhDC, VERTRES); 
	DeleteDC(mhDC); 
	return true;
}

bool Resize_Layout()
{
	Resize_ToolTray_Dlg();
	Resize_Layers_Dlg();
	Resize_Frames_Dlg();
	Resize_Timeline();
	Resize_Info_Bar_Dlg();
	Resize_Toolbar_Dlg();
	if(split_screen)
	{
		Fit_Splitscreen_Window();
		Fit_3D_Window();
	}
	else
	{
		Fit_2D_Window();
		Fit_3D_Window();
		if(gapfill_tool_open)Fit_GapFill_Window();
	}

	if(split_screen)
	{
	}
	mainwindow->SetFocus();
	return true;
}
