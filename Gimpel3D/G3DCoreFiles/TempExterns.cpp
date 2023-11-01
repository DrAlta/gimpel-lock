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
#include "G3D.h"
#include "Frame.h"
#include "bb_matrix.h"
#include "Layers.h"
#include "Camera.h"
#include "../GapFillTool.h"
#include "../GeometryTool.h"
#include "../PerspectiveTool.h"
#include "../AnchorPoints.h"
#include "../LinkPointsTool.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <GL/gl.h>
#include "../Skin.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "devil.lib")
#pragma comment(lib, "ilu.lib")

HWND Edit_HWND();

bool Report_Selection_Changed();

extern bool play_preview;
bool Update_Preview_Playback();

extern bool update_2d_view_texture;
bool Restore_Original_Frame_Texture();

extern bool iterate_virtual_camera_alignment;
bool Update_Iterative_Virtual_Camera_Alignment();

float VecLength(float* v);

bool Update_Alignment();
extern bool iterate_alignment;

void Update_GUI_Selection_Info();
bool Delete_Circular_Maps();

float fastsqrt(float n);
void Normalize(float *v);
bool ReCalc_Layer_Center(int layer_id, float *pos);
void Set_Target_Camera(float *pos, float *rot);
bool Is_First_Or_Last_Frame();
float Delta_Time();
void Update_Camera();
bool Init_Basic_Edit_Ops();
bool Init_Edit_Window(int x, int y, int w, int h);
bool Init_GL_Text();
bool Init_GLWindow(HWND pWnd, int xpos, int ypos, int width, int height, int bits, bool fullscreen);
HGLRC Edit_HRC();
bool Init_FastMath();
bool Init_Clock();
bool Init_Controls();
void Init_Noise_Texture();
void Update_Clock();
void Update_Controls();
void Update_Edit_Controls();

extern int num_keys_pressed;
extern int num_edit_keys_pressed;

bool Render_Edit_Window();
bool Set_GLContext();
void Render_Puff_Preview();
void Render_3D_Layer_Outline(int layer_id);
void Render_2D_Layer_Outline(int layer_id);
bool Alt_Key_Pressed();
void Run_Layer_Popup_Menu();
void Free_Noise_Texture();
bool Free_Console();
bool Free_GLText();
bool Free_Frames();
bool Init_Control_Mesh_Data();
bool Free_Control_Mesh_Data();
bool KillGLWindow();
bool Free_Edit_Window();
bool Free_Edit_Ops();
bool Free_Puff_Search_Info();
bool Free_Circle_Grid_RLE_Strips();
bool Free_KLT_Data();

void Clear_Screen();
void Update_Screen();
extern HGLRC _hRC;

extern bool render_grid;

extern bool anaglyph_mode;

extern float scene_center[3];
extern bool run_layer_popup_menu;
extern int last_clicked_layer;

extern HWND _hWnd;

float fly_speed_scale = 100;
int depth_slider_range = 500;

bool pick_concave_color = false;
bool pick_convex_color = false;
bool rotoscope_color_difference_mode = false;
float scale_slider_range = 1;
float min_depth_offset = 0.0f;
bool render_relief_preview = false;
bool frame_loaded = false;

float horizontal_view = 0;
float vertical_view = 0;
float view_change = 15;
float total_depth_adjustment = 0;
bool depth_mismatch = false;
int num_selected_depths = 0;
float last_depth_slider_pos = 0;
bool exporting = false;
bool prefilter_layermasks = false;
float depth_increment = 0.001f;
float min_vr = -180;
float max_vr = 180;
float min_hr = -180;
float max_hr = 180;
float min_zr = -180;
float max_zr = 180;
int projection_type = FLAT_PROJECTION;
int last_relief_scale_pos = 0;
int num_selected_scales = 0;
float last_scale_slider_pos = 0;
bool update_contours = true;
bool use_backup_contours = true;
bool click_select = true;
int main_window_xpos = 0;
int main_window_ypos = 0;
int main_window_width = 853;
int main_window_height = 640;

int small_window_xpos = 853;
int small_window_ypos = 0;
int small_window_width = 1280-853;
int small_window_height = 1280-853;

bool selection_changed = false;

bool scale_mismatch = false;
float total_scale_adjustment = 0;
bool show_3d_view = true;
bool show_edit_view = true;


float zoom_change = 1.05f;
RELIEF_INFO relief_info;

float zoom = 0;

//FIXTHIS move view stuff into camera

class VIEW_INFO
{
public:
	VIEW_INFO(){}
	~VIEW_INFO(){}
	float rot[3];
	float center[3];
	float zoom;
	float camera_pos[3];
	float camera_rot[3];
};

vector<VIEW_INFO> view_stack;

void Push_3D_View()
{
	VIEW_INFO vi;
	view_stack.push_back(vi);
	int n = view_stack.size();
	VIEW_INFO *pvi = &view_stack[n-1];
	pvi->center[0] = scene_center[0];
	pvi->center[1] = scene_center[1];
	pvi->center[2] = scene_center[2];
	pvi->rot[0] = -vertical_view;
	pvi->rot[1] = horizontal_view;
	pvi->rot[2] = 0;
	pvi->zoom = zoom;
	pvi->camera_pos[0] = camera_pos[0];
	pvi->camera_pos[1] = camera_pos[1];
	pvi->camera_pos[2] = camera_pos[2];
	pvi->camera_rot[0] = camera_rot[0];
	pvi->camera_rot[1] = camera_rot[1];
	pvi->camera_rot[2] = camera_rot[2];
}

void Set_3D_View(float *rot, float *center, float zoom_out);

void Pop_3D_View()
{
	int n = view_stack.size();
	if(n>0)
	{
		VIEW_INFO *pvi = &view_stack[n-1];
		vertical_view = -pvi->rot[0];
		horizontal_view = pvi->rot[1];
		zoom = pvi->zoom;
		Set_Camera(pvi->camera_pos, pvi->camera_rot);
		scene_center[0] = pvi->center[0];
		scene_center[1] = pvi->center[1];
		scene_center[2] = pvi->center[2];
		view_stack.pop_back();
	}
}

void Set_View()
{
	float rot[3] = {-vertical_view, horizontal_view, 0};
	oBB_MATRIX m;
	m.Set(m.m, scene_center, rot);
	float *forward = &m.m[2][0];
	Normalize(forward);
	float distance = zoom;
	forward[0] *= distance;
	forward[1] *= distance;
	forward[2] *= distance;
	float pos[3];
	pos[0] = scene_center[0]+forward[0];
	pos[1] = scene_center[1]+forward[1];
	pos[2] = scene_center[2]+forward[2];
	rot[0] = -rot[0];
	rot[1] = -rot[1];
	rot[2] = -rot[2];
	Set_Target_Camera(pos, rot);
}

void Get_3D_View(float *rot, float *center, float *zoom_out)
{
	center[0] = scene_center[0];
	center[1] = scene_center[1];
	center[2] = scene_center[2];
	rot[0] = -vertical_view;
	rot[1] = horizontal_view;
	rot[2] = 0;
	*zoom_out = zoom;
}

void Set_3D_View(float *rot, float *center, float zoom_out)
{
	vertical_view = -rot[0];
	horizontal_view = rot[1];
	scene_center[0] = center[0];
	scene_center[1] = center[1];
	scene_center[2] = center[2];
	zoom = zoom_out;
	Set_View();
	Set_Camera_To_Target();
}

void Pan_View(float dx, float dy)
{
	float s = 0.001f*zoom;
	dx *= s;
	dy *= s;
	float rot[3] = {-vertical_view, horizontal_view, 0};
	oBB_MATRIX m;
	m.Set(m.m, scene_center, rot);
	float *side = &m.m[0][0];
	float *up = &m.m[1][0];
	scene_center[0] += ((side[0]*dx)-(up[0]*dy));
	scene_center[1] += ((side[1]*dx)-(up[1]*dy));
	scene_center[2] += ((side[2]*dx)-(up[2]*dy));
	Set_View();
}

void Get_View_Center(float *pos, float *rot)
{
	if(frame)
	{
		pos[0] = frame->view_origin[0];
		pos[1] = frame->view_origin[1];
		pos[2] = frame->view_origin[2];
		rot[0] = (frame->view_rotation[0]);//flip pitch because we're spinning around vertical axis
		rot[1] = (-frame->view_rotation[1])+180;
		rot[2] = 0;
	}
}

void Center_View()
{
	float pos[3] = {0,0,0};
	float rot[3] = {0,180,0};
	if(frame)
	{
		pos[0] = frame->view_origin[0];
		pos[1] = frame->view_origin[1];
		pos[2] = frame->view_origin[2];
		rot[0] = (frame->view_rotation[0]);//flip pitch because we're spinning around vertical axis
		rot[1] = (-frame->view_rotation[1])+180;
		rot[2] = 0;
	}
		Set_Target_Camera(pos, rot);
		horizontal_view = (frame->view_rotation[1])-180;
		vertical_view = rot[0];
		ReCalc_Center();
		zoom = VecLength(scene_center);
}

bool Browse(char *res, char *ext, bool save)
{
    char szFilter[64];
	char fext[5];
	sprintf(fext, "*.%s", ext);
	sprintf(szFilter, "(*.%s)|*.%s|All Files (*.*)|*.*||", ext, ext);
    CFileDialog dlg(!save, ext, fext, 0, szFilter, 0);
	if(dlg.DoModal()==IDOK)
	{
		strcpy(res, dlg.GetPathName().operator LPCTSTR());
		return true;
	}
	return false;
}

bool Exists(char *file)
{
	FILE *f = fopen(file, "r");
	if(!f)
	{
		return false;
	}
	fclose(f);
	return true;
}

bool app_running = false;

bool Check_Save_Session();

bool Quit()
{
	int res = SkinMsgBox(0, "Are you sure you want to exit?", "Gimpel3D", MB_YESNO);
	if(res==IDNO)
	{
		return false;
	}
	app_running = false;
	return true;
}

bool Export_Frame(int index, int type, char *output_path, char *extension, char *prefix)
{
	char file[512];
	sprintf(file, "%s\\%s_%.6i.%s", output_path, prefix, index, extension);
	if(!Set_Project_Frame(index))
	{
		return false;
	}
	Render_Frame_Update();
	if(type==ANAGLYPH_OUTPUT)
	{
		Save_Anaglyph_Image(file);
	}
	else if(type==STEREO_OUTPUT)
	{
		Save_Stereo_Image(file);
	}
	else if(type==LEFTRIGHT_OUTPUT)
	{
		sprintf(file, "%s\\LEFT\\%s_LEFT_%.6i.%s", output_path, prefix, index, extension);
		Save_Left_Image(file);
		sprintf(file, "%s\\RIGHT\\%s_RIGHT_%.6i.%s", output_path, prefix, index, extension);
		Save_Right_Image(file);
	}
	return true;
}

bool BrowseForFolder(char *path, char *title)
{
   bool retVal = false;

   BROWSEINFO bi;
   memset(&bi, 0, sizeof(bi));

   bi.ulFlags   = 0x00000040;//BIF_NEWDIALOGSTYLE;
   bi.hwndOwner = 0;
   bi.lpszTitle = title;

   // must call this if using BIF_USENEWUI
   ::OleInitialize(NULL);

   // Show the dialog and get the itemIDList for the 
   // selected folder.
   LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

   if(pIDL != NULL)
   {
      // Create a buffer to store the path, then 
      // get the path.
      char buffer[_MAX_PATH] = {'\0'};
      if(::SHGetPathFromIDList(pIDL, buffer) != 0)
      {
         // Set the string value.
		  strcpy(path, buffer);
		  retVal = true;
      }

      // free the item id list
      CoTaskMemFree(pIDL);
   }
   ::OleUninitialize();
   return retVal;
}

extern CWinApp *mainApp;

vector <CButton*> tooltip_buttons;

int num_tooltip_buttons = 0;

bool Register_Tooltip_Button(CButton *b)
{
	tooltip_buttons.push_back(b);
	num_tooltip_buttons++;
	return true;
}

bool UnRegister_Tooltip_Button(CButton *b)
{
	int n = tooltip_buttons.size();
	for(int i = 0;i<n;i++)
	{
		if(tooltip_buttons[i]==b)
		{
			tooltip_buttons.erase(tooltip_buttons.begin()+i);
			num_tooltip_buttons--;
			return true;
		}
	}
	return false;
}

void Update_Win32_Messages()
{
	MSG _msg;
	while(PeekMessage(&_msg,0,0,0,PM_REMOVE))
	{
		for(int i = 0;i<num_tooltip_buttons;i++)
		{
			tooltip_buttons[i]->PreTranslateMessage(&_msg);
		}
		TranslateMessage(&_msg);DispatchMessage(&_msg);
	}
}


void Render_Frame_Update()
{
	Clear_Screen();
	Render_3D_View();
	Update_Screen();
	Update_Win32_Messages();
}

//FIXTHIS move to relief stuff
bool Set_Default_Relief_Info(RELIEF_INFO *ri)
{
	ri->type = RELIEF_TYPE_BRIGHTNESS;
	ri->contrast = 1;
	ri->contrast_slider_range = 1;
	ri->tiling = 1;
	ri->tiling_slider_range = 10;
	ri->bias = 0.5f;
	ri->concave_color[0] = 0;
	ri->concave_color[1] = 0;
	ri->concave_color[2] = 0;
	ri->convex_color[0] = 255;
	ri->convex_color[1] = 255;
	ri->convex_color[2] = 255;
	ri->scale = 0;
	ri->smooth = true;
	ri->scale_slider_range = 100;
	strcpy(ri->image, "NO IMAGE");
	return true;
}
void Update_Selection_Relief(bool update_values)
{
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Set_Layer_Relief_Info(i, &relief_info, update_values);
			redraw_frame = true;
			session_altered = true;
		}
	}
}
void Set_Global_Relief_Image(char *file)
{
	strcpy(relief_info.image, file);
	relief_info.scale = 50;
	relief_info.type = RELIEF_TYPE_IMAGE;
	relief_info.tiling = 1;
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		Set_Layer_Relief_Info(i, &relief_info, true);
		redraw_frame = true;
		session_altered = true;
	}
}

void Update_Selected_States()
{
	int n = Num_Layers();
	int num_selected = 0;
	int i;
	for(i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			num_selected++;
		}
	}

	bool freeze_mismatch = false;
	bool visible_mismatch = false;
	bool outline_mismatch = false;
	bool speckle_mismatch = false;
	bool keyframed_mismatch = false;

	bool frozen = false;
	bool visible = false;
	bool outline = false;
	bool speckle = false;
	bool keyframed = false;

	int cnt = 0;

	int nl = Num_Layers();
	int first_sel_id = -1;
	for(i = 0;i<nl;i++)
	{
		if(Layer_Is_Selected(i))
		{
			if(first_sel_id==-1)
			{
				first_sel_id = i;
			}
			cnt++;
			if(cnt==1)
			{
				frozen = Layer_Is_Frozen(i);
				visible = !Layer_Is_Hidden(i);
				outline = Layer_Is_Outlined(i);
				speckle = Layer_Is_Speckled(i);
				keyframed = Layer_Is_Keyframed(i);
			}
			else
			{
				if(!freeze_mismatch)
				{
					if(frozen!=Layer_Is_Frozen(i))freeze_mismatch = true;
				}
				if(!visible_mismatch)
				{
					if(visible==Layer_Is_Hidden(i))visible_mismatch = true;
				}
				if(!outline_mismatch)
				{
					if(outline!=Layer_Is_Outlined(i))outline_mismatch = true;
				}
				if(!speckle_mismatch)
				{
					if(speckle!=Layer_Is_Speckled(i))speckle_mismatch = true;
				}
				if(!keyframed_mismatch)
				{
					if(keyframed!=Layer_Is_Keyframed(i))keyframed_mismatch = true;
				}
			}
		}
	}
	
	if(num_selected==0)
	{
		Enable_Freeze_Checkbox(false);
		Enable_Visible_Checkbox(false);
		Enable_Keyframe_Checkbox(false);
		Enable_Outline_Checkbox(false);
		Enable_Speckle_Checkbox(false);
		Check_Freeze_Checkbox(false);
		Check_Visible_Checkbox(false);
		Check_Keyframe_Checkbox(false);
		Check_Outline_Checkbox(false);
		Check_Speckle_Checkbox(false);
	}
	else
	{
		if(Is_First_Or_Last_Frame())
		{
			keyframed_mismatch = true;
		}
		Enable_Freeze_Checkbox(!freeze_mismatch);
		Enable_Visible_Checkbox(!visible_mismatch);
		Enable_Keyframe_Checkbox(!keyframed_mismatch);
		Enable_Outline_Checkbox(!outline_mismatch);
		Enable_Speckle_Checkbox(!speckle_mismatch);
		Check_Freeze_Checkbox(Layer_Is_Frozen(first_sel_id)||freeze_mismatch);
		Check_Visible_Checkbox(!Layer_Is_Hidden(first_sel_id)||visible_mismatch);
		Check_Keyframe_Checkbox(Layer_Is_Keyframed(first_sel_id)||keyframed_mismatch);
		Check_Outline_Checkbox(Layer_Is_Outlined(first_sel_id)||outline_mismatch);
		Check_Speckle_Checkbox(Layer_Is_Speckled(first_sel_id)||speckle_mismatch);
	}
}

void Reset_Contour()
{
	if(frame)frame->Reset_Contour();
}

void Get_Display_Name(char *image)
{
	char temp[512];
	char *c = strrchr(image, '\\');
	if(!c)c = image;
	else c = &c[1];
	sprintf(temp, "%s", c);
	c = strrchr(temp, '.');
	if(c)
	{
		*c = 0;
	}
	strcpy(image, temp);
}

HWND Get_3D_Window()
{
	return _hWnd;
}

HWND Get_2D_Window()
{
	return Edit_HWND();
}

void Speed_Up()
{
	if(fly_speed_scale>=199)return;
	fly_speed_scale += Delta_Time()*30;
	Set_Fly_Speed_Slider(fly_speed_scale);
}

void Slow_Down()
{
	if(fly_speed_scale<=1)return;
	fly_speed_scale -= Delta_Time()*30;
	Set_Fly_Speed_Slider(fly_speed_scale);
}
void Move_Selection_Depth(float v)
{
	Adjust_Selected_Layers_Depth_ID_Float(v);
	Update_Depth_Slider_Position();
}
void Toggle_Borders()
{
	render_borders = !render_borders;
	Enable_All_Borders(render_borders);
	Check_Render_Borders_Checkbox(render_borders);
	redraw_frame = true;
}
bool Focus_Edit_View()
{
	SetWindowPos(Edit_HWND(), 0, main_window_xpos, main_window_ypos, main_window_width, main_window_height, 0);
	SetWindowPos(_hWnd, 0, small_window_xpos, small_window_ypos, small_window_width, small_window_height, 0);
	Update_Camera();
	return true;
}

bool Focus_3D_View()
{
	SetWindowPos(_hWnd, 0, main_window_xpos, main_window_ypos, main_window_width, main_window_height, 0);
	SetWindowPos(Edit_HWND(), 0, small_window_xpos, small_window_ypos, small_window_width, small_window_height, 0);
	SetFocus(_hWnd);
	Update_Camera();
	return true;
}

//FIXTHIS move to external layer control?
void Project_All_Layers(float depth_id)
{
	float pos[3] = {0,0,(depth_increment*depth_id)+min_depth_offset};
	float dir[3] = {0, 0, 1};
	float rotation[3] = {0,0,0};
	float offset[3] = {0,0,0};
	Project_Everything_To_Plane(pos, dir, rotation, offset, PLANE_ORIGIN_SELECTION_CENTER);
	redraw_frame = true;
	Reset_Plane_Sliders();
}

//FIXTHIS move to relief stuff
void Set_Temp_Global_Relief_Image(char *file)
{
	strcpy(relief_info.image, file);
	relief_info.scale = 50;
	relief_info.type = RELIEF_TYPE_IMAGE;
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		Set_Layer_Relief_Info(i, &relief_info, false);
		redraw_frame = true;
		session_altered = true;
	}
}

//FIXTHIS WTF is this?
float Get_Depth_ID_Float(float v)
{
	float res = 0;
	float d = v;
	d -= min_depth_offset;
	d = d/depth_increment;
	res = d;
	if(res<0)
	{
		res = -res;
	}
	return res;
}

//FIXTHIS eliminate and make part of frame
bool Get_Frame_Filename(char *file)
{
	if(!frame)
	{
		return false;
	}
	strcpy(file, frame->filename);
	return true;
}

bool Show_3D_View(bool b)
{
	show_3d_view = b;
	if(b)
	{
		ShowWindow(_hWnd, SW_SHOW);
		redraw_frame = true;
	}
	else
	{
		ShowWindow(_hWnd, SW_HIDE);
	}
	return true;
}

bool Show_2D_View(bool b)
{
	show_edit_view = b;
	if(b)
	{
		ShowWindow(Edit_HWND(), SW_SHOW);
		redraw_frame = true;
		if(update_2d_view_texture)
		{
			Restore_Original_Frame_Texture();
		}
	}
	else
	{
		ShowWindow(Edit_HWND(), SW_HIDE);
	}
	return true;
}

bool Init_G3D_Core()
{
	Init_Basic_Edit_Ops();
	Init_Control_Mesh_Data();
	ilInit();
	iluInit();
	Init_Edit_Window(small_window_xpos, small_window_ypos, small_window_width, small_window_height);
	Init_GL_Text();
	Init_GLWindow(0, main_window_xpos, main_window_ypos, main_window_width, main_window_height, 24, false);
	Init_Geometry_System();
	show_3d_view = true;
	Init_GapFill_Window(0, 0, 100, 100);

	wglShareLists(Edit_HRC(), _hRC);
	Init_FastMath();
	Init_Clock();
	Init_Controls();
	Reset_Stereo();
	Update_Camera();
	Set_Puff_Search_Range(25);

	Init_Noise_Texture();
	return true;
}

bool Update_G3D_Core()
{
	static int demo_popup = 0;
	Update_Clock();
	Update_Win32_Messages();
	if(play_preview)
	{
		Update_Preview_Playback();
	}
	if(selection_changed)
	{
		if(frame)frame->Invalidate_Selection_Buffer();
		Update_GUI_Selection_Info();//update core GUIs
		Report_Selection_Changed();//update plugins
		selection_changed = false;
	}
	if(iterate_alignment)
	{
		Update_Alignment();
		redraw_frame = true;
	}
	if(iterate_perspective_alignment)
	{
		Update_Perspective_Alignment();
		redraw_frame = true;
	}
	if(iterate_link_point_alignment)
	{
		Update_Link_Point_Alignment();
		redraw_frame = true;
	}
	if(iterate_virtual_camera_alignment)
	{
		Update_Iterative_Virtual_Camera_Alignment();
	}
	bool render_2d = redraw_edit_window&&show_edit_view;
	bool render_3d = redraw_frame&&show_3d_view;
	if(num_keys_pressed>0)Update_Controls();
	if(num_edit_keys_pressed>0)Update_Edit_Controls();
	if(camera_moving)Update_Camera();
	if(redraw_gapfill_window)
	{
		Render_GapFill_Window();
		redraw_gapfill_window = false;
	}
	if(render_2d)
	{
		Set_Edit_Context();
		if(run_layer_popup_menu)//make sure layer is selected for highlight
		{
			int index = Get_Layer_Index(last_clicked_layer);
			if(index!=-1)
			{
				//layer might not be selected but needs to be rendered that way
				bool was_selected = Layer_Is_Selected(index);
				Select_Layer(index, true);
				Render_Edit_Window();
				redraw_edit_window = false;
				if(!was_selected)
				{
					//set it back
					Select_Layer(index, false);
				}
			}
			else
			{
				//no actual layer clicked
				//just render it normally
				Render_Edit_Window();
				redraw_edit_window = false;
			}
		}
		else
		{
			//just render it normally
			Render_Edit_Window();
			redraw_edit_window = false;
		}
		Set_GLContext();
	}
	if(render_3d)
	{
		Clear_Screen();
		Render_3D_View();
		if(demo_popup==20||Alt_Key_Pressed())
		{
			demo_popup = 0;
		}
		Update_Screen();
		redraw_frame = false;
	}
	if(run_layer_popup_menu)
	{
		Run_Layer_Popup_Menu();
	}
	return true;
}

bool Free_G3D_Core()
{
	Free_Frame();
	Free_Noise_Texture();
	Free_Console();
	Free_GLText();
	Free_Frames();
	Free_Control_Mesh_Data();
	Free_Geometry_System();
	KillGLWindow();
	Free_Edit_Window();
	Free_GapFill_Window();
	Free_Edit_Ops();
	Free_Puff_Search_Info();
	Free_Circle_Grid_RLE_Strips();
	Free_KLT_Data();
	Delete_Circular_Maps();
	return true;
}
