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


bool Drag_GapFill_Image(int x, int y);
bool Scale_GapFill_Image(int n);
bool Get_GapFill_Pixel(int x, int y, float *px, float *py);

bool Drag_GapFill_Blend_Image(int x, int y);
bool Scale_GapFill_Blend_Image(int n);

bool Reset_GapFill_View();

bool gapfill_keys_pressed[256];

bool gapfill_mouse_drag_left = false;
bool gapfill_mouse_drag_right = false;

int last_gapfill_mouse_x = 0;
int last_gapfill_mouse_y = 0;

bool Select_Gap_At_Pixel(int x, int y);

extern int alt_key;
extern int control_key;
extern int center_view_key;

bool Clear_All_GapFill_Keys()
{
	gapfill_mouse_drag_left = false;
	gapfill_mouse_drag_right = false;
	memset(gapfill_keys_pressed, 0, sizeof(bool)*256);
	return true;
}

bool Update_GapFill_Mouse(int x, int y)
{
	int xdiff = x-last_gapfill_mouse_x;
	int ydiff = y-last_gapfill_mouse_y;
	if(gapfill_mouse_drag_left&&gapfill_keys_pressed[alt_key])
	{
		Drag_GapFill_Image(xdiff, ydiff);
	}
	if(gapfill_mouse_drag_left&&gapfill_keys_pressed[control_key])
	{
		Drag_GapFill_Blend_Image(xdiff, ydiff);
	}
	if(gapfill_mouse_drag_right&&gapfill_keys_pressed[alt_key])
	{
		Scale_GapFill_Image(ydiff);
	}
	if(gapfill_mouse_drag_right&&gapfill_keys_pressed[control_key])
	{
		Scale_GapFill_Blend_Image(ydiff);
	}
	last_gapfill_mouse_x = x;
	last_gapfill_mouse_y = y;
	return true;
}

bool Update_GapFill_Mouse_Wheel(int n)
{
	return true;
}

bool Click_Gap()
{
	float px = 0;
	float py = 0;
	Get_GapFill_Pixel(last_gapfill_mouse_x, last_gapfill_mouse_y, &px, &py);
	Select_Gap_At_Pixel((int)px, (int)py);
	return true;
}

bool Update_GapFill_Key(int key, bool pressed)
{
	gapfill_keys_pressed[key] = pressed;
	if(key==VK_LBUTTON)
	{
		gapfill_mouse_drag_left = pressed;
	}
	if(key==VK_RBUTTON)
	{
		gapfill_mouse_drag_right = pressed;
	}
	if(key==center_view_key&&pressed)
	{
		Reset_GapFill_View();
	}
	if(pressed&&key==VK_LBUTTON)
	{
		Click_Gap();
	}
	return true;
}

