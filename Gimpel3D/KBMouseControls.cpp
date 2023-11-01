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
#include "Gimpel3D.h"
#include "PerspectiveTool.h"
#include "AnchorPoints.h"
#include "FeaturePoints.h"
#include "LinkPointsTool.h"

extern int NEW_FRAME_LAYER_DEPTH;

#define NOKEY 0

bool Report_MouseMove2D(int oldx, int oldy, int newx, int newy, float pixel_x, float pixel_y);
bool Report_MouseMove3D(int oldx, int oldy, int newx, int newy);

bool Report_2D_Window_KeyState(int key, bool pressed);
bool Report_3D_Window_KeyState(int key, bool pressed);
bool Report_2D_Window_Keys_Cleared();
bool Report_3D_Window_Keys_Cleared();

extern bool activate_alignment_points;
extern bool deactivate_alignment_points;

bool Enable_AutoAlignment_Points_Within_Brush(bool enable);


float mouse_sensitivity = 1;

bool allow_layer_popup_menu = true;

int num_keys_pressed = 0;
int num_edit_keys_pressed = 0;

bool keys_pressed[256];
bool edit_keys_pressed[256];

bool Save_Camera_View(int n);
bool Set_Saved_Camera_View(int n);
void Toggle_Clicked_Layer_Select(int mouse_x, int mouse_y);
void Tab_Selection();
void Layer_Popup_Menu(HWND hWnd, int xpos, int ypos, int clicked_layer);
int Get_3D_Clicked_Layer(int x, int y);
void Toggle_Borders();
void Reset_Rotate_Selection();
void Rotate_Horizontal_Selection(float v);
void Rotate_Vertical_Selection(float v);
void Rotate_Z_Selection(float v);
void Reset_Rotate_Selection();
void Speed_Up();
void Slow_Down();
void Reset_Acceleration();
void Update_Acceleration();
void Move_Forward(float f);
void Move_Back(float f);
void Move_Up(float f);
void Move_Down(float f);
void Move_Left(float f);
void Move_Right(float f);
void Turn_Up(float f);
void Turn_Down(float f);
void Turn_Left(float f);
void Turn_Right(float f);
void Move_Selection_Depth(float v);
bool Toggle_Edge_View();

bool Move_Selected_Geometry_Horizontally(float v);
bool Move_Selected_Geometry_Vertically(float v);
bool Scale_Selected_Geometry(float x, float y, float z);


bool Toggle_Freeze_Selection();

bool Clear_All_Edit_Keys();


float kb_depth_move = 0.5;
float kb_rotate_move = 1;
float kb_geometry_move = 0.001f;


//add numpad offset 96 for these

int move_outline_left_key    = 4+96;
int move_outline_right_key   = 6+96;
int move_outline_up_key      = 8+96;
int move_outline_down_key    = 2+96;

int shift_left_edge_out_key  = 7+96;
int shift_left_edge_in_key   = 9+96;
int shift_right_edge_out_key = 3+96;
int shift_right_edge_in_key  = 1+96;

int control_select_all_key = 'A';
int unselect_all_key = VK_ESCAPE;//27;//esc
int move_forward_key = NOKEY;//38;//up arrow
int move_backward_key = NOKEY;//40;//down arrow
int move_up_key = NOKEY;//65;//A
int move_down_key = NOKEY;//90;//Z
int turn_left_key = NOKEY;//37;//left arrow
int turn_right_key = NOKEY;//39;//right arrow
int turn_up_key = NOKEY;//35;//end key
int turn_down_key = NOKEY;//46;//delete key

int move_left_key = NOKEY;//'Q';
int move_right_key = NOKEY;//'W';

int view_left_key = 186;//   ";"//'L';
int view_right_key = 222;//   "'"//'R';
int view_anaglyph_key = '2';//72;//H

int save_camera_view1_key = 116;//F5
int save_camera_view2_key = 117;//F6
int save_camera_view3_key = 118;//F7
int save_camera_view4_key = 119;//F8

int speed_up_key = NOKEY;//'1';
int slow_down_key = NOKEY;//'2';

int control_merge_selection_key = 'M';
int toggle_select_key = 'Q';//'S';

int toggle_borders_key = 'B';
int tab_selection_key = 9;//TAB
int center_view_key = 'V';
int switch_view_key = VK_SPACE;

int toggle_edge_view_key = NOKEY;//'E';

int open_depth_dialog_key = 'W';
int open_planar_dialog_key = 'E';
int open_scale_dialog_key = 'R';
int open_contour_dialog_key = NOKEY;
int open_relief_dialog_key = NOKEY;
int open_alignment_dialog_key = NOKEY;
int open_tracking_dialog_key = NOKEY;
int open_shift_edges_dialog_key = NOKEY;

int mouse_x = 0;
int mouse_y = 0;

int last_3d_mouse_x = 0;
int last_3d_mouse_y = 0;

int alt_key = 18;
int shift_key = 16;
int control_key = 17;

int previous_frame_key = 37;//left arrow
int next_frame_key = 39;//right arrow

int shift_previous_5_frames_key = 37;//left arrow
int shift_next_5_frames_key = 39;//right arrow

int toggle_freeze_selection_key = 'F';

int mouse_drag_depth_key = 188;
int mouse_drag_rotate_left_right_key = 190;//">"
int mouse_drag_rotate_up_down_key = 191;//"?"
//int mouse_drag_rotate_z_key = VK_SHIFT;//'Z';//16;

int mouse_drag_leftright_key = 'M';
int mouse_drag_updown_key = 'N';
int mouse_drag_xscale_key = 'J';
int mouse_drag_yscale_key = 'K';
int mouse_drag_zscale_key = 'L';


extern float fly_speed;
extern float turn_speed;
extern float acceleration;
extern float kb_depth_move;
extern float kb_rotate_move;
////the following is input for the 3d window
///

void Reset_Controls()
{
	memset(keys_pressed, 0, sizeof(bool)*256);
	num_keys_pressed = 0;
	last_3d_mouse_x = mouse_x;
	last_3d_mouse_y = mouse_y;
	Report_3D_Window_Keys_Cleared();
}

bool Init_Controls()
{
	Reset_Controls();
	Clear_All_Edit_Keys();
	return true;
}

bool Alt_Key_Pressed()
{
	return keys_pressed[alt_key];
}

void Rotate_3D_View(int dx, int dy)
{
	horizontal_view += dx*0.2f;
	vertical_view -= dy*0.2f;
	if(vertical_view>90)vertical_view=90;
	if(vertical_view<-90)vertical_view=-90;
	Set_View();
}

void Zoom_3D_View(int d)
{
	float s = zoom;
	if(s<0.1f){s = 0.1f;}
	zoom -= (d*0.01f)*s;
	if(zoom<0){zoom = 0;}
	Set_View();
}

bool Update_Mouse(int px, int py)
{
	Report_MouseMove3D(mouse_x, mouse_y, px, py);
	last_3d_mouse_x = mouse_x;
	last_3d_mouse_y = mouse_y;
	mouse_x = px;
	mouse_y = py;
	float dx = (last_3d_mouse_x-mouse_x)*mouse_sensitivity;
	float dy = (last_3d_mouse_y-mouse_y)*mouse_sensitivity;
	if(keys_pressed[VK_LBUTTON]&&render_link_points_tool)
	{
		Drag_Link_Points_Tool_3D((float)mouse_x, (float)mouse_y);
	}
	if(keys_pressed[VK_LBUTTON]&&keys_pressed[alt_key])
	{
		Rotate_3D_View((int)dx, (int)dy);
	}
	if(keys_pressed[VK_RBUTTON]&&keys_pressed[alt_key])
	{
		Zoom_3D_View((int)dy);
	}
	if(keys_pressed[VK_MBUTTON]&&keys_pressed[alt_key])
	{
		Pan_View(dx, dy);
		redraw_frame = true;
	}
	float selection_adjust_scale = 1;
	if(keys_pressed[shift_key])selection_adjust_scale *= 10;
	else if(keys_pressed[control_key])selection_adjust_scale *= 0.1f;
	if(keys_pressed[VK_LBUTTON]&&keys_pressed[mouse_drag_depth_key])
	{
		Move_Selection_Depth(kb_depth_move*selection_adjust_scale*dy);
		if(render_anchor_points_tool)
		{
			Move_Selected_Anchor_Point_Depth(kb_depth_move*selection_adjust_scale*dy);
		}
	}
	if(keys_pressed[VK_LBUTTON])
	{
		//check both rot keys, it means Z
		if(keys_pressed[mouse_drag_rotate_left_right_key]&&keys_pressed[mouse_drag_rotate_up_down_key])
		{
			Rotate_Z_Selection(kb_rotate_move*selection_adjust_scale*dy);
			redraw_frame = true;
		}
		else if(keys_pressed[mouse_drag_rotate_left_right_key])
		{
			Rotate_Horizontal_Selection(kb_rotate_move*selection_adjust_scale*dy);
			redraw_frame = true;
		}
		else if(keys_pressed[mouse_drag_rotate_up_down_key])
		{
			Rotate_Vertical_Selection(-kb_rotate_move*selection_adjust_scale*dy);
			redraw_frame = true;
		}
	}
	if(keys_pressed[VK_LBUTTON]&&keys_pressed[mouse_drag_leftright_key])
	{
		Move_Selected_Geometry_Horizontally(kb_geometry_move*selection_adjust_scale*dy);
		redraw_frame = true;
	}
	if(keys_pressed[VK_LBUTTON]&&keys_pressed[mouse_drag_updown_key])
	{
		Move_Selected_Geometry_Vertically(kb_geometry_move*selection_adjust_scale*dy);
		redraw_frame = true;
	}
	if(keys_pressed[VK_LBUTTON]&&keys_pressed[mouse_drag_xscale_key])
	{
		Scale_Selected_Geometry(kb_geometry_move*selection_adjust_scale*dy, 0, 0);
		redraw_frame = true;
	}
	if(keys_pressed[VK_LBUTTON]&&keys_pressed[mouse_drag_yscale_key])
	{
		Scale_Selected_Geometry(0, kb_geometry_move*selection_adjust_scale*dy, 0);
		redraw_frame = true;
	}
	if(keys_pressed[VK_LBUTTON]&&keys_pressed[mouse_drag_zscale_key])
	{
		Scale_Selected_Geometry(0, 0, kb_geometry_move*selection_adjust_scale*dy);
		redraw_frame = true;
	}
	return true;
}


void Update_Common_Controls(bool *keys)
{
	float dt = Delta_Time();
	float m_speed = (fly_speed*dt)*(fly_speed_scale/100);
	float t_speed = (turn_speed*dt);
	float selection_adjust_scale = 1;
	if(keys[shift_key])selection_adjust_scale *= 10;
	else if(keys[alt_key])selection_adjust_scale *= 0.01f;
	else if(keys[control_key])selection_adjust_scale *= 0.1f;
	if(keys[speed_up_key])
	{
		Speed_Up();
	}
	if(keys[slow_down_key])
	{
		Slow_Down();
	}
	if(!keys[control_key])
	{
		if(!keys[move_forward_key]&&!keys[move_backward_key]&&!keys[move_up_key]&&!keys[move_down_key])
		{
			Reset_Acceleration();
		}
		if(keys[move_forward_key])
		{
			Update_Acceleration();
			Move_Forward(m_speed*acceleration);
		}
		if(keys[move_backward_key])
		{
			Update_Acceleration();
			Move_Back(m_speed*acceleration);
		}
		if(keys[move_up_key])
		{
			Update_Acceleration();
			Move_Up(m_speed+m_speed*acceleration);
		}
		if(keys[move_down_key])
		{
			Update_Acceleration();
			Move_Down(m_speed+m_speed*acceleration);
		}
		if(keys[move_left_key])
		{
			Update_Acceleration();
			Move_Left(m_speed+m_speed*acceleration);
		}
		if(keys[move_right_key])
		{
			Update_Acceleration();
			Move_Right(m_speed+m_speed*acceleration);
		}
		if(keys[turn_up_key])
		{
			Turn_Up(t_speed);
		}
		if(keys[turn_down_key])
		{
			Turn_Down(t_speed);
		}
		if(keys[turn_left_key])
		{
			Turn_Left(t_speed);
		}
		if(keys[turn_right_key])
		{
			Turn_Right(t_speed);
		}
	}
	else
	{
	}
}

//consolidate "pressed" cases

bool Update_Common_Keys(int key, bool pressed, bool *keys)
{
	if(pressed&&key==VK_RBUTTON&&keys[shift_key]&&keys[control_key])
	{
		Reset_Rotate_Selection();
		redraw_frame = true;
	}
	if(pressed&&key==open_depth_dialog_key)
	{
		Open_Depth_Slider_Dlg();
	}
	if(pressed&&key==open_planar_dialog_key)
	{
		Open_Planar_Projection_Dlg();
	}
	if(pressed&&key==open_scale_dialog_key)
	{
		Open_Scale_Slider_Dlg();
	}
	if(pressed&&key==open_contour_dialog_key)
	{
		Open_Contour_Extrusion_Dlg();
	}
	if(pressed&&key==open_relief_dialog_key)
	{
		Open_Relief_Map_Dlg();
	}
	if(pressed&&key==open_alignment_dialog_key)
	{
		Open_Alignment_Options_Dialog();
	}
	if(pressed&&key==open_tracking_dialog_key)
	{
		Open_Track_Outlines_Tool();
	}
	if(pressed&&key==open_shift_edges_dialog_key)
	{
		Open_Shift_Edges_Dlg();
	}
	if(keys[shift_key]&&pressed&&key==shift_previous_5_frames_key)
	{
		Advance_Project_Frames(-5);
		return true;//don't set key flag flight will be messed up
	}
	if(keys[shift_key]&&pressed&&key==shift_next_5_frames_key)
	{
		Advance_Project_Frames(5);
		return true;//don't set key flag flight will be messed up
	}
	if(pressed&&key==previous_frame_key)
	{
		Advance_Project_Frames(-1);
		return true;//don't set key flag flight will be messed up
	}
	if(pressed&&key==next_frame_key)
	{
		Advance_Project_Frames(1);
		return true;//don't set key flag flight will be messed up
	}
	if(pressed&&key==tab_selection_key)
	{
		Tab_Selection();
	}
	if(pressed&&key==switch_view_key)
	{
		Switch_View_Mode();
	}
	if(key==view_anaglyph_key||key==view_left_key||key==view_right_key)
	{
		redraw_frame = true;
	}
	if(key==unselect_all_key&&pressed)
	{
		Select_All_Layers(false);
	}
	if(key==control_select_all_key&&pressed&&keys[control_key])
	{
		Select_All_Layers(true);
	}
	if(key==toggle_borders_key&&pressed)
	{
		Toggle_Borders();
	}
	if(pressed)
	{
		if(key==save_camera_view1_key){if(keys['0']){Save_Camera_View(0);}else{Set_Saved_Camera_View(0);}}
		if(key==save_camera_view2_key){if(keys['0']){Save_Camera_View(1);}else{Set_Saved_Camera_View(1);}}
		if(key==save_camera_view3_key){if(keys['0']){Save_Camera_View(2);}else{Set_Saved_Camera_View(2);}}
		if(key==save_camera_view4_key){if(keys['0']){Save_Camera_View(3);}else{Set_Saved_Camera_View(3);}}
	}
	if(key==control_merge_selection_key&&pressed&&keys[control_key])
	{
		Merge_Selection();
	}
	if(key==toggle_freeze_selection_key&&pressed)
	{
		Toggle_Freeze_Selection();
		redraw_edit_window = true;
	}
	if(key==toggle_edge_view_key&&pressed)
	{
		Toggle_Edge_View();
		redraw_edit_window = true;
	}
	if(pressed)
	{
		if(key==move_outline_left_key)
		{
			Shift_Left_Out();
			Shift_Right_In();
			redraw_edit_window = true;
			redraw_frame = true;
		}
		if(key==move_outline_right_key)
		{
			Shift_Right_Out();
			Shift_Left_In();
			redraw_edit_window = true;
			redraw_frame = true;
		}
		if(key==move_outline_up_key)
		{
			Shift_Selected_Layers_Vertically(1);
			redraw_edit_window = true;
			redraw_frame = true;
		}
		if(key==move_outline_down_key)
		{
			Shift_Selected_Layers_Vertically(-1);
			redraw_edit_window = true;
			redraw_frame = true;
		}
		if(key==shift_left_edge_out_key)
		{
			Shift_Left_Out();
			redraw_edit_window = true;
			redraw_frame = true;
		}
		if(key==shift_left_edge_in_key)
		{
			Shift_Left_In();
			redraw_edit_window = true;
			redraw_frame = true;
		}
		if(key==shift_right_edge_out_key)
		{
			Shift_Right_Out();
			redraw_edit_window = true;
			redraw_frame = true;
		}
		if(key==shift_right_edge_in_key)
		{
			Shift_Right_In();
			redraw_edit_window = true;
			redraw_frame = true;
		}
	}
	return true;
}


bool Update_Key(int key, bool pressed)
{
	Report_3D_Window_KeyState(key, pressed);
	Update_Common_Keys(key, pressed, keys_pressed);
	
	if(keys_pressed[key]&&!pressed)
	{
		num_keys_pressed--;
	}
	else if(!keys_pressed[key]&&pressed)
	{
		num_keys_pressed++;
	}

	keys_pressed[key] = pressed;

	if(pressed&&(key==VK_LBUTTON||key==VK_RBUTTON||key==alt_key))
	{
		last_3d_mouse_x = mouse_x;
		last_3d_mouse_y = mouse_y;
	}
	if(pressed&&key==VK_LBUTTON)
	{
		if(keys_pressed[toggle_select_key])
		{
			Toggle_Clicked_Layer_Select(mouse_x, mouse_y);
		}
		if(render_link_points_tool)
		{
			Click_Link_Points_Tool_3D((float)mouse_x, (float)mouse_y);
		}
	}
	if(pressed&&key==VK_RBUTTON
		&&!keys_pressed[mouse_drag_depth_key]
		&&!keys_pressed[mouse_drag_rotate_left_right_key]
		&&!keys_pressed[mouse_drag_rotate_up_down_key]
		&&!keys_pressed[alt_key]
		&&!keys_pressed[shift_key]
		)
	{
		if(allow_layer_popup_menu)
		{
			Layer_Popup_Menu(Get_3D_Window(), mouse_x, mouse_y, Get_3D_Clicked_Layer(mouse_x, mouse_y));
		}
	}
	if(pressed&&key==center_view_key)
	{
		Center_View();
	}
	return true;
}



void Update_Controls()
{
	Update_Common_Controls(keys_pressed);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
////the following is input for the 2d window
///

bool Push_Depth_Within_Brush();
bool Blend_Depth_Within_Brush();
bool Flatten_Depth_Within_Brush();
bool Pull_Depth_Within_Brush();


bool Set_Edit_Window_Cursor(int x, int y);
bool StartStop_Edit_Op(bool b);
bool Execute_Edit_Op(int edit_op, float amount);
int Find_Edit_Op(char *name);
bool Get_Pixel_Color(int x, int y, unsigned char *rgb);

bool Expand_Tracking_Outline_Within_Brush();
bool Contract_Tracking_Outline_Within_Brush();
bool Smooth_Tracking_Outline_Within_Brush();
bool Tracking_Outline_Is_Active();
bool Get_Edit_Pixel(int x, int y, float *px, float *py);


bool Undo_Point();
bool Redo_Point();
bool Close_Print_Help();
bool Cancel_Outline_Select();

bool Scale_Edit_Image(int n);

bool Reset_Edit_View();
bool Finish_Outline_Select();
bool Toggle_Print_Help();
bool Get_Frame_Pixel_Layer(int x, int y, int *layer);
bool Drag_Edit_Image(int x, int y);

bool Shift_Pixels_Within_Brush(bool left, bool right);


int brush_size = 30;

int last_mouse_x = 0;
int last_mouse_y = 0;
int last_brush_x = 0;
int last_brush_y = 0;
float last_edit_px = 0;
float last_edit_py = 0;

int ctrl_undo_key = 'Z';
int ctrl_redo_key = 'Y';
int click_select_key = 'C';
int expand_contract_outline_key = 'D';
int smooth_outline_key = 'S';

int push_pull_key = 'G';
int blend_flatten_depth_key = 'H';

bool Reset_TouchUp_Edit_Group();




int shift_edges_left_with_brush_key = 'O';
int shift_edges_right_with_brush_key = 'P';

int zoom_in_key = 189;
int zoom_out_key = 187;
int finish_outline_key = VK_RETURN;
int cancel_outline_key = VK_ESCAPE;

int trace_outlines_key = 'T';//formerly control key


bool render_brush = false;
bool render_crosshairs = false;

bool show_edit_crosshairs = true;

extern int current_edit_op;

int smooth_outline_under_brush_key = 'S';

int shift_smooth_all_outlines_key = 'S';


bool Update_Brush_State()
{
	render_brush = false;
	render_crosshairs = false;
	if(
		edit_keys_pressed[click_select_key]||
		edit_keys_pressed[shift_edges_left_with_brush_key]||
		edit_keys_pressed[shift_edges_right_with_brush_key]||
		edit_keys_pressed[expand_contract_outline_key]||
		(edit_keys_pressed[smooth_outline_key]&&!edit_keys_pressed[shift_key])||
		edit_keys_pressed[blend_flatten_depth_key]||
		edit_keys_pressed[push_pull_key]
		)
	{
		render_brush = true;
		//render_crosshairs = true;
	}
	if(
		edit_keys_pressed[trace_outlines_key]
		)
	{
		render_crosshairs = show_edit_crosshairs;
	}
	return true;
}

bool Clear_All_Edit_Keys()
{
	memset(edit_keys_pressed, 0, sizeof(bool)*256);
	num_edit_keys_pressed = 0;
	render_brush = false;
	render_crosshairs = false;
	Reset_TouchUp_Edit_Group();
	Report_2D_Window_Keys_Cleared();
	return true;
}

bool Update_Edit_Key(int key, bool pressed)
{
	Report_2D_Window_KeyState(key, pressed);
	Update_Common_Keys(key, pressed, edit_keys_pressed);
	//validate operations
	if(pressed)
	{
		if(key==expand_contract_outline_key)//requires tracked outline
		{
			if(!Tracking_Outline_Is_Active())
			{
				return false;
			}
		}
		if(key==smooth_outline_key)//requires tracked outline
		{
			if(!Tracking_Outline_Is_Active())
			{
				return false;
			}
		}
	}
	if(edit_keys_pressed[key]&&!pressed)
	{
		num_edit_keys_pressed--;
	}
	else if(!edit_keys_pressed[key]&&pressed)
	{
		num_edit_keys_pressed++;
	}

	edit_keys_pressed[key] = pressed;
	if(key==shift_smooth_all_outlines_key&&edit_keys_pressed[shift_key])
	{
		Extra_Smooth_Tracking_Outlines();
	}
	if(key==VK_LBUTTON)
	{
		if(activate_alignment_points)
		{
			Enable_AutoAlignment_Points_Within_Brush(true);
			redraw_edit_window = true;
		}
		if(deactivate_alignment_points)
		{
			Enable_AutoAlignment_Points_Within_Brush(false);
			redraw_edit_window = true;
		}
		if(edit_keys_pressed[trace_outlines_key]&&!pressed)
		{
			last_mouse_x = last_brush_x;
			last_mouse_y = last_brush_y;
			Set_Edit_Window_Cursor(last_brush_x, last_brush_y);
			StartStop_Edit_Op(false);
		}
		if(edit_keys_pressed[trace_outlines_key]&&pressed)
		{
			StartStop_Edit_Op(true);
			Execute_Edit_Op(current_edit_op, 0);
			redraw_edit_window = true;
		}
		if(edit_keys_pressed[toggle_select_key]&&pressed)
		{
			Execute_Edit_Op(Find_Edit_Op("Select Layer(s)"), 0);
			redraw_edit_window = true;
		}
		if(edit_keys_pressed[click_select_key]&&pressed)
		{
			Execute_Edit_Op(Find_Edit_Op("Click Create Layer"), 0);
		}
		if(edit_keys_pressed[expand_contract_outline_key]&&pressed)
		{
			Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
			Expand_Tracking_Outline_Within_Brush();
		}
		if(edit_keys_pressed[push_pull_key]&&pressed)
		{
			Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
			Push_Depth_Within_Brush();
		}
		if(edit_keys_pressed[blend_flatten_depth_key]&&pressed)
		{
			Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
			Blend_Depth_Within_Brush();
		}
		if(edit_keys_pressed[smooth_outline_key]&&pressed&&!edit_keys_pressed[shift_key])
		{
			Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
			Smooth_Tracking_Outline_Within_Brush();
		}
		if(edit_keys_pressed[shift_edges_left_with_brush_key]&&pressed)
		{
			Shift_Pixels_Within_Brush(true, false);
		}
		if(edit_keys_pressed[shift_edges_right_with_brush_key]&&pressed)
		{
			Shift_Pixels_Within_Brush(false, true);
		}
		if(pick_concave_color)
		{
			unsigned char rgb[3];
			if(Get_Pixel_Color((int)last_edit_px, (int)last_edit_py, rgb))
			{
				Select_Concave_Color(rgb);
			}
		}
		if(pick_convex_color)
		{
			unsigned char rgb[3];
			if(Get_Pixel_Color((int)last_edit_px, (int)last_edit_py, rgb))
			{
				Select_Convex_Color(rgb);
			}
		}
		if(pressed&&render_perspective_tool)
		{
			Click_Perspective_Tool(last_edit_px, last_edit_py);
		}
		if(pressed&&render_feature_points_tool)
		{
			Click_Feature_Points_Tool(last_edit_px, last_edit_py);
		}
	}
	if(key==VK_RBUTTON)
	{
		if(edit_keys_pressed[expand_contract_outline_key]&&pressed)
		{
			Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
			Contract_Tracking_Outline_Within_Brush();
		}
		if(edit_keys_pressed[push_pull_key]&&pressed)
		{
			Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
			Pull_Depth_Within_Brush();
		}
		if(edit_keys_pressed[blend_flatten_depth_key]&&pressed)
		{
			Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
			Flatten_Depth_Within_Brush();
		}
	}
	if(key==control_key)
	{
		if(!pressed)
		{
			last_mouse_x = last_brush_x;
			last_mouse_y = last_brush_y;
			Set_Edit_Window_Cursor(last_brush_x, last_brush_y);
		}
		redraw_edit_window = true;
	}
	if(pressed&&edit_keys_pressed[control_key])
	{
		if(key==ctrl_undo_key)
		{
			Undo_Point();
			redraw_edit_window = true;
		}
		if(key==ctrl_redo_key)
		{
			Redo_Point();
			redraw_edit_window = true;
		}
	}
	if(key==shift_edges_left_with_brush_key)
	{
		Update_Brush_State();
		redraw_edit_window = true;
	}
	if(key==shift_edges_right_with_brush_key)
	{
		Update_Brush_State();
		redraw_edit_window = true;
	}
	if(key==click_select_key)
	{
		Update_Brush_State();
		redraw_edit_window = true;
	}
	if(key==expand_contract_outline_key)
	{
		Update_Brush_State();
		redraw_edit_window = true;
	}
	if(key==push_pull_key)
	{
		Update_Brush_State();
		redraw_edit_window = true;
	}
	if(key==blend_flatten_depth_key)
	{
		Update_Brush_State();
		redraw_edit_window = true;
	}
	if(key==smooth_outline_key)
	{
		Update_Brush_State();
		redraw_edit_window = true;
	}
	if(key==finish_outline_key&&pressed)
	{
		Finish_Outline_Select();
		redraw_edit_window = true;
	}
	if(key==cancel_outline_key&&pressed)
	{
		Cancel_Outline_Select();
		redraw_edit_window = true;
	}
	if(key==VK_RBUTTON&&pressed&&edit_keys_pressed[trace_outlines_key])
	{
		Toggle_Edge_Detection();
		redraw_edit_window = true;
	}
	if(key==trace_outlines_key)
	{
		Update_Brush_State();
		redraw_edit_window = true;
	}
	if(key==center_view_key&&pressed)
	{
		Reset_Edit_View();
		redraw_edit_window = true;
	}
	if(pressed)
	{
		if(key==toggle_select_key)
		{
			Print_Status("Left-click layers to toggle select state.");
		}
		if(key==click_select_key)
		{
			Print_Status("Circular brush - Left-click image to select circular areas.");
		}
		if(key==expand_contract_outline_key)
		{
			Print_Status("Circular brush - Left-click image to expand the outline, right-click to contract it.");
		}
		if(key==push_pull_key)
		{
			Print_Status("Circular brush - Left-click image to push the depth, right click to pull the depth.");
		}
		if(key==blend_flatten_depth_key)
		{
			Print_Status("Circular brush - Left-click image to blend the depth, right click to flatten the depth..");
		}
		if(key==smooth_outline_key)
		{
			Print_Status("Circular brush - Left-click image to smooth the outline.");
		}
	}
	else
	{
		if(key==toggle_select_key||
		key==click_select_key||
		key==expand_contract_outline_key||
		key==push_pull_key||
		key==blend_flatten_depth_key||
		key==smooth_outline_key)
		Print_Status("Ready..");
	}
	if(pressed&&key==VK_RBUTTON
		&&!edit_keys_pressed[mouse_drag_depth_key]
		&&!edit_keys_pressed[mouse_drag_rotate_left_right_key]
		&&!edit_keys_pressed[mouse_drag_rotate_up_down_key]
		&&!edit_keys_pressed[shift_key]
		&&!edit_keys_pressed[control_key]
		&&!edit_keys_pressed[trace_outlines_key]
		&&!edit_keys_pressed[alt_key]
		&&!edit_keys_pressed[expand_contract_outline_key]
		&&!edit_keys_pressed[push_pull_key]
		&&!edit_keys_pressed[blend_flatten_depth_key]
		)
	{
		if(frame_loaded)
		{
			if(allow_layer_popup_menu)
			{
				int clicked_layer = -1;
				if(Get_Frame_Pixel_Layer((int)last_edit_px, (int)last_edit_py, &clicked_layer))
				{
					Layer_Popup_Menu(Edit_HWND(), last_mouse_x, last_mouse_y, clicked_layer);
				}
			}
		}
	}
	if(!pressed)
	{
		if(key==push_pull_key||key==blend_flatten_depth_key)
		{
			Reset_TouchUp_Edit_Group();
		}
	}

	return true;
}

int increase_brush_key = 219;// "["
int decrease_brush_key = 221;// "]"

void Update_Edit_Controls()
{
	Update_Common_Controls(edit_keys_pressed);
}


bool Update_Edit_Mouse(int x, int y)
{
	Get_Edit_Pixel(x, y, &last_edit_px, &last_edit_py);
	Report_MouseMove2D(last_mouse_x, last_mouse_y, x, y, last_edit_px, last_edit_py);
	int xdiff = x-last_mouse_x;
	int ydiff = y-last_mouse_y;
	if(edit_keys_pressed[VK_LBUTTON]&&edit_keys_pressed[alt_key])
	{
		Drag_Edit_Image(xdiff, ydiff);
	}
	if(edit_keys_pressed[VK_RBUTTON]&&edit_keys_pressed[alt_key])
	{
		Scale_Edit_Image(ydiff);
	}
	last_mouse_x = x;
	last_mouse_y = y;
	if(pick_concave_color)
	{
		unsigned char rgb[3];
		if(Get_Pixel_Color((int)last_edit_px, (int)last_edit_py, rgb))
		{
			Update_Concave_Color(rgb);
		}
	}
	if(pick_convex_color)
	{
		unsigned char rgb[3];
		if(Get_Pixel_Color((int)last_edit_px, (int)last_edit_py, rgb))
		{
			Update_Convex_Color(rgb);
		}
	}
	if(edit_keys_pressed[VK_LBUTTON]&&edit_keys_pressed[trace_outlines_key])
	{
		{
			last_brush_x = x;
			last_brush_y = y;
		}
		Execute_Edit_Op(current_edit_op, (float)ydiff);
	}
	else
	{
		last_brush_x = x;
		last_brush_y = y;
	}
	float dy = (float)-ydiff;
	dy*= mouse_sensitivity;
	float selection_adjust_scale = 1;
	if(edit_keys_pressed[shift_key])selection_adjust_scale *= 10;
	else if(edit_keys_pressed[control_key])selection_adjust_scale *= 0.1f;
	if(edit_keys_pressed[VK_LBUTTON]&&edit_keys_pressed[mouse_drag_depth_key])
	{
		Move_Selection_Depth(kb_depth_move*selection_adjust_scale*dy);
		if(render_anchor_points_tool)
		{
			Move_Selected_Anchor_Point_Depth(kb_depth_move*selection_adjust_scale*dy);
		}
		redraw_frame = true;
	}
	if(edit_keys_pressed[VK_LBUTTON])
	{
		//check both rot keys, it means Z
		if(edit_keys_pressed[mouse_drag_rotate_left_right_key]&&edit_keys_pressed[mouse_drag_rotate_up_down_key])
		{
			Rotate_Z_Selection(kb_rotate_move*selection_adjust_scale*dy);
			redraw_frame = true;
		}
		else if(edit_keys_pressed[mouse_drag_rotate_left_right_key])
		{
			Rotate_Horizontal_Selection(kb_rotate_move*selection_adjust_scale*dy);
			redraw_frame = true;
		}
		else if(edit_keys_pressed[mouse_drag_rotate_up_down_key])
		{
			Rotate_Vertical_Selection(-kb_rotate_move*selection_adjust_scale*dy);
			redraw_frame = true;
		}
	}
	if(edit_keys_pressed[VK_LBUTTON]&&edit_keys_pressed[mouse_drag_leftright_key])
	{
		Move_Selected_Geometry_Horizontally(kb_geometry_move*selection_adjust_scale*dy);
		redraw_frame = true;
	}
	if(edit_keys_pressed[VK_LBUTTON]&&edit_keys_pressed[mouse_drag_updown_key])
	{
		Move_Selected_Geometry_Vertically(kb_geometry_move*selection_adjust_scale*dy);
		redraw_frame = true;
	}
	if(edit_keys_pressed[VK_LBUTTON]&&edit_keys_pressed[mouse_drag_xscale_key])
	{
		Scale_Selected_Geometry(kb_geometry_move*selection_adjust_scale*dy, 0, 0);
		redraw_frame = true;
	}
	if(edit_keys_pressed[VK_LBUTTON]&&edit_keys_pressed[mouse_drag_yscale_key])
	{
		Scale_Selected_Geometry(0, kb_geometry_move*selection_adjust_scale*dy, 0);
		redraw_frame = true;
	}
	if(edit_keys_pressed[VK_LBUTTON]&&edit_keys_pressed[mouse_drag_zscale_key])
	{
		Scale_Selected_Geometry(0, 0, kb_geometry_move*selection_adjust_scale*dy);
		redraw_frame = true;
	}
	if(edit_keys_pressed[VK_LBUTTON]&&render_perspective_tool)
	{
		Drag_Perspective_Tool(last_edit_px, last_edit_py);
	}
	if(edit_keys_pressed[VK_LBUTTON]&&render_feature_points_tool)
	{
		Drag_Feature_Points_Tool(last_edit_px, last_edit_py);
	}
	if(edit_keys_pressed[VK_LBUTTON])
	{
		if(edit_keys_pressed[push_pull_key])
		{
			Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
			Push_Depth_Within_Brush();
		}
		if(edit_keys_pressed[blend_flatten_depth_key])
		{
			Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
			Blend_Depth_Within_Brush();
		}
	}
	if(edit_keys_pressed[VK_RBUTTON])
	{
		if(edit_keys_pressed[push_pull_key])
		{
			Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
			Pull_Depth_Within_Brush();
		}
		if(edit_keys_pressed[blend_flatten_depth_key])
		{
			Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
			Flatten_Depth_Within_Brush();
		}
	}
	return true;
}

bool Update_Edit_Mouse_Wheel(int n)
{
	if(n>0)
	{
		brush_size += brush_size/10;
	}
	else
	{
		brush_size -= brush_size/10;
	}
	brush_size += n;
	if(brush_size<2)
	{
		brush_size = 2;
	}
	return true;
}

