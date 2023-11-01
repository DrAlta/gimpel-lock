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
#include "EditOps.h"
#include <GL/gl.h>
#include <vector>
#include "GLWnd.h"
#include "Frame.h"
#include "Console.h"
#include "Camera.h"

//FIXTHIS eliminate this file altogether

using namespace std;
void Layer_Popup_Menu(HWND hWnd, int xpos, int ypos, int clicked_layer);

bool Blend_Depth_Within_Brush();
bool Flatten_Depth_Within_Brush();
bool Push_Depth_Within_Brush();
bool Pull_Depth_Within_Brush();

bool Tracking_Outline_Is_Active();

extern int alt_key;

extern bool view_edges;

void Center_View();

extern int center_view_key;
extern int switch_view_key;

extern bool pick_concave_color;
extern bool pick_convex_color;

extern bool redraw_edit_window;
extern int select_all_key;
extern int unselect_all_key;

void Update_Concave_Color(unsigned char *rgb);
void Update_Convex_Color(unsigned char *rgb);

void Select_Concave_Color(unsigned char *rgb);
void Select_Convex_Color(unsigned char *rgb);

bool Expand_Tracking_Outline_Within_Brush();
bool Contract_Tracking_Outline_Within_Brush();

bool Smooth_Tracking_Outline_Within_Brush();

bool Get_Edit_Pixel(int x, int y, float *px, float *py);
bool Set_Edit_Window_Cursor(int x, int y);

bool Toggle_Edge_Detection()
{
	edge_detection_enabled = !edge_detection_enabled;
	if(edge_detection_enabled)
	{
		if(update_edge_texture)
		{
			Update_Edge_Texture();
		}
	}
	return true;
}

bool Toggle_Freeze_Selection();

bool Focus_Edit_View();
bool Focus_3D_View();

bool Reset_Edit_View();

extern bool edit_keys_pressed[256];

extern int move_forward_key;
extern int move_backward_key;
extern int move_up_key;
extern int move_down_key;
extern int turn_left_key;
extern int turn_right_key;
extern int turn_up_key;
extern int turn_down_key;

extern float fly_speed;
extern float turn_speed;

extern int speed_up_key;
extern int slow_down_key;

void Speed_Up();
void Slow_Down();
void Reset_Acceleration();
void Update_Acceleration();

extern float acceleration;

extern int move_left_key;
extern int move_right_key;



bool Drag_Edit_Image(int x, int y);
bool Scale_Edit_Image(int n);


int scroll_amount = 20;

bool Update_Selected_Layers();

bool Merge_Selection()
{
	vector<int> sel_ids;//really indices
	int n = Num_Layers();
	int i;
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			sel_ids.push_back(i);
		}
	}
	n = sel_ids.size();
	if(n==0)
	{
		return false;
	}
	RELIEF_INFO dri;
	Set_Default_Relief_Info(&dri);
	for(i = 1;i<n;i++)
	{
		//fixthis optimize get index beforehand
		Set_Layer_Relief_Info(sel_ids[i], &dri, true);
		Absorb_Layer(Get_Layer_ID(sel_ids[i]-(i-1)), Get_Layer_ID(sel_ids[0]));
	}
	Set_Layer_Relief_Info(sel_ids[0], &dri, true);
	Update_Modified_Layers();
	Clear_Modified_Layers();
	sel_ids.clear();
	Update_Layer_List();
	redraw_frame = true;
	redraw_edit_window = true;
	Update_Selected_Layers();
	selection_changed = true;
	return true;
}

class EDIT_OP
{
public:
	EDIT_OP()
	{
		edit_cb = 0;
		strcpy(name, "NONAME");
	}
	~EDIT_OP()
	{
	}
	char name[32];
	bool lock_brush;
	_Edit_Callback edit_cb;
	_StartStop_Edit_Callback ss_cb;
};

vector<EDIT_OP*> edit_ops;

int current_edit_op = 0;

int Find_Edit_Op(char *name)
{
	int n = edit_ops.size();
	for(int i= 0;i<n;i++)
	{
		if(!strcmp(name, edit_ops[i]->name))
		{
			return i;
		}
	}
	return -1;
}

bool Register_Edit_Op(int *id, char *name, bool lock_brush, _Edit_Callback edit_cb, _StartStop_Edit_Callback ss_cb)
{
	if(Find_Edit_Op(name)!=-1)
	{
		return false;
	}
	EDIT_OP *e = new EDIT_OP;
	strcpy(e->name, name);
	e->edit_cb = edit_cb;
	e->ss_cb = ss_cb;
	e->lock_brush = lock_brush;
	edit_ops.push_back(e);
	if(id)*id = edit_ops.size()-1;
	return true;
}

bool Free_Edit_Ops()
{
	int n = edit_ops.size();
	for(int i = 0;i<n;i++)
	{
		delete edit_ops[i];
	}
	edit_ops.clear();
	return true;
}

int Num_Edit_Ops()
{
	return edit_ops.size();
}

bool Edit_Op_Name(int id, char *res)
{
	if(id<0||id>=Num_Edit_Ops())
	{
		return false;
	}
	strcpy(res, edit_ops[id]->name);
	return true;
}

bool Set_Active_Op(int id)
{
	if(id<0||id>=Num_Edit_Ops())
	{
		return false;
	}
	current_edit_op = id;
	return true;
}

int Get_Active_Op()
{
	return current_edit_op;
}

void ReSizeGLScene(int width, int height);
void Get_Matrices();

bool StartStop_Edit_Op(bool b)
{
	edit_ops[current_edit_op]->ss_cb(b);
	return true;
}


extern int last_mouse_x;
extern int last_mouse_y;
extern int last_brush_x;
extern int last_brush_y;
extern float last_edit_px;
extern float last_edit_py;

extern int brush_size;

bool Execute_Edit_Op(int edit_op, float amount)
{
	float ow = Screenwidth();
	float oh = Screenheight();
	ReSizeGLScene(Frame_Width(), Frame_Height());
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	Get_Matrices();
	Get_Edit_Pixel(last_brush_x, last_brush_y, &last_edit_px, &last_edit_py);
	edit_ops[edit_op]->edit_cb(last_edit_px, last_edit_py, brush_size, amount);
	glPopMatrix();
	ReSizeGLScene(ow, oh);
	return true;
}

void Get_Current_Mouse_Edit_Pixel(float *px, float *py)
{
	Get_Edit_Pixel(last_mouse_x, last_mouse_y, px, py);
}

bool Get_Pixel_Color(int x, int y, unsigned char *rgb)
{
	if(!frame)return false;
	if(x<0||x>=frame->width||y<0||y>=frame->height)
	{
		rgb[0] = 0;
		rgb[1] = 0;
		rgb[2] = 0;
		return true;//false;
	}
	float *prgb = frame->Get_RGB(x, y);
	rgb[0] = (unsigned char)(prgb[0]*255);
	rgb[1] = (unsigned char)(prgb[1]*255);
	rgb[2] = (unsigned char)(prgb[2]*255);
	return true;
}

bool Toggle_Edge_View()
{
	View_Edges(!view_edges);
	return true;
}

bool Get_Edit_World_Pos(int x, int y, float *px, float *py);

#include <math.h>

bool Get_Vertex_Ring(int n, float *p, float size)
{
	float angle;
	for(int i = 0;i<n;i++)
	{
		angle = (((float)i/n)*360)*RAD;
		p[i*3] = cos(angle)*size;
		p[(i*3)+1] = 0;
		p[(i*3)+2] = sin(angle)*size;
	}
	return true;
}

//FIXTHIS make dedicated brush src file

bool Render_Edit_Brush()
{
	float px, py;
	Get_Edit_World_Pos(last_brush_x, last_brush_y, &px, &py);
	glColor3f(0,1,0);
	glLineWidth(3);
	float angle;
	float size = brush_size;
	int n = 50;
	glBegin(GL_LINE_STRIP);
	for(int i = 0;i<n;i++)
	{
		angle = (((float)i/n)*360)*RAD;
		glVertex2f(px+(cos(angle)*size), py+(sin(angle)*size));
	}
	angle = 0;
	glVertex2f(px+(cos(angle)*size), py+(sin(angle)*size));
	glEnd();
	glColor3f(1,1,1);
	glLineWidth(1);
	return true;
}


