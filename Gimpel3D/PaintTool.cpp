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
#include "PaintTool.h"
#include "G3DCoreFiles/Frame.h"
#include "G3DCoreFiles/Image.h"

bool Get_Frame_Depth_Map(unsigned char *res);
bool Get_Frame_Layer_Map(unsigned char *res);

float paint_grayscale_near = 0;
float paint_grayscale_far = 0;

bool paint_tool_open = false;

bool render_paint_tool = false;
bool render_paint_frame_image = true;
bool render_paint_grayscale_image = true;
bool render_paint_layermask_image = true;


int paint_grayscale_texture = -1;
int paint_layermask_texture = -1;

bool Init_Paint_Grayscale_Texture()
{
	Set_Edit_Context();
	unsigned char *data = new unsigned char[frame->width*frame->height*3];
	Get_Frame_Depth_Map(data);
	paint_grayscale_texture = Create_Frame_Texture(frame->width, frame->height, data);
	delete[] data;
	Set_GLContext();
	return true;
}

bool Init_Paint_Layermask_Texture()
{
	Set_Edit_Context();
	unsigned char *data = new unsigned char[frame->width*frame->height*3];
	Get_Frame_Layer_Map(data);
	paint_layermask_texture = Create_Frame_Texture(frame->width, frame->height, data);
	delete[] data;
	Set_GLContext();
	return true;
}

bool Free_Paint_Grayscale_Texture()
{
	Set_Edit_Context();
	Free_GL_Texture(paint_grayscale_texture);
	Set_GLContext();
	return true;
}

bool Free_Paint_Layermask_Texture()
{
	Set_Edit_Context();
	Free_GL_Texture(paint_layermask_texture);
	Set_GLContext();
	return true;
}


bool Open_Paint_Tool()
{
	if(paint_tool_open)
	{
		return false;
	}
	render_paint_tool = true;
	redraw_edit_window = true;
	Init_Paint_Grayscale_Texture();
	Init_Paint_Layermask_Texture();
	Open_Paint_Tool_Dlg();
	paint_tool_open = true;
	return true;
}

bool Close_Paint_Tool()
{
	if(!paint_tool_open)
	{
		return false;
	}
	Close_Paint_Tool_Dlg();
	render_paint_tool = false;
	redraw_edit_window = true;
	Free_Paint_Grayscale_Texture();
	Free_Paint_Layermask_Texture();
	paint_tool_open = false;
	return true;
}



