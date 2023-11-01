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
#include "Controls.h"
#include "Clock.h"
#include "Camera.h"
#include "Console.h"


//FIXTHIS eliminate this file altogether, all controls moved to KBMouseControls.cpp

bool enable_window_controls = true;

void Enable_Window_Controls(bool b)
{
	enable_window_controls = b;
}

int Get_3D_Clicked_Layer(int x, int y);
void Center_View();


bool Save_Camera_View(int n);
bool Set_Saved_Camera_View(int n);

extern bool redraw_frame;

void Speed_Up();
void Slow_Down();

void Tab_Selection();
void Toggle_Borders();

extern float fly_speed_scale;

extern bool keys_pressed[256];

float fly_speed = 0.065f/2;
float turn_speed = 80.5f/2;

float acceleration = 1.0f;
float max_acceleration = 25;//3;
float acceleration_increase = 1.02f;

void Select_All_Layers(bool b);

void Move_Selection_Depth(float v);

extern float kb_depth_move;
extern float kb_rotate_move;
extern float kb_geometry_move;

void Rotate_Horizontal_Selection(float v);//{}
void Rotate_Vertical_Selection(float v);//{}
void Reset_Rotate_Selection();//{}

extern int control_key;

bool Focus_Edit_View();
bool Focus_3D_View();


bool Update_Selected_Layer(int x, int y);
bool UnSelectLayer();

void Layer_Popup_Menu(HWND hWnd, int xpos, int ypos, int clicked_layer);


void Set_Fly_Speed(float s)
{
	fly_speed = s;
}

void Update_Acceleration()
{
	acceleration *= acceleration_increase;
	if(acceleration>max_acceleration)acceleration=max_acceleration;
}

void Reset_Acceleration()
{
	acceleration = 1.0f;
}

#include "GLWnd.h"

extern HWND _hWnd;

void Toggle_Clicked_Layer_Select(int mouse_x, int mouse_y)
{
	int last_clicked_layer = Get_3D_Clicked_Layer(mouse_x, mouse_y);
	if(last_clicked_layer==-1)
	{
		return;
	}
	int index = Get_Layer_Index(last_clicked_layer);
	Select_Layer(index, !Layer_Is_Selected(index));
	List_Selected_Layers();
	redraw_frame = true;
}


void Mouse_Hover()
{
	Print_To_Console("Mouse hover %f", Delta_Time());
}
