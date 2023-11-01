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
#ifndef PERSPECTIVE_TOOL_H
#define PERSPECTIVE_TOOL_H


bool Open_Perspective_Tool();
bool Close_Perspective_Tool();

bool Click_Perspective_Tool(float x, float y);
bool Drag_Perspective_Tool(float x, float y);


bool Open_Perspective_Dlg();
bool Close_Perspective_Dlg();

extern bool render_perspective_tool;
bool Render_Perspective_Tool();

enum
{
	pps_PARALLEL,
	pps_RECTANGLE,
	pps_ANGLE
};

extern int perspective_projection_type;
extern float perspective_alignment_angle;

extern bool use_single_vanishing_point;

extern bool horizontal_parallel_lines;

bool Align_Perspective_Forward();
bool Align_Perspective_Back();
bool Align_Perspective_Left();
bool Align_Perspective_Right();
bool Align_Perspective_All();
bool Set_Grid_To_Perspective_Floor();
bool Set_Grid_To_Perspective_Wall();
bool Set_Grid_To_Perspective_Origin();

extern bool adjustable_fov;

bool Update_Perspective_Alignment();
extern bool iterate_perspective_alignment;

#endif