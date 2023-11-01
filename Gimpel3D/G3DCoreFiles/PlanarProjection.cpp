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
#include "PlanarProjection.h"
#include "bb_matrix.h"
#include "Layers.h"
#include "Console.h"
#include <GL\gl.h>


//FIXTHIS eliminate this file altogether

void Update_Plane_Title();

extern bool anaglyph_mode;

extern float scene_center[3];

float horizontal_rotation = 0;
float vertical_rotation = 0;
float z_rotation = 0;
float horizontal_position = 0;
float vertical_position = 0;
float depth_position = 0;

int plane_origin_type = PLANE_ORIGIN_SELECTION_CENTER;

float plane_dir[3] = {0,0,0};

void Normalize(float *v);

void Get_Plane_Dir(float *orot, float *dir);

void Get_Projection_Plane()
{
	float rot[3];
	rot[0] = horizontal_rotation;
	rot[1] = vertical_rotation;
	rot[2] = z_rotation;
	Get_Plane_Dir(rot, plane_dir);
}

void Set_Plane_Dialog_To_Selection()
{
	float rotation[3];
	plane_origin_type = PLANE_ORIGIN_SELECTION_CENTER;//assume default
	Get_Selection_Rotation(plane_dir, rotation, &plane_origin_type);
	horizontal_rotation = rotation[0];
	vertical_rotation = rotation[1];
	z_rotation = rotation[2];
	Update_Plane_Title();
	if(Num_Selected_Layers()>0)Update_Planar_GUI();
}

void Finalize_Selection_Planar_Projection()
{
	Get_Projection_Plane();
	float rotation[3];
	float offset[3] = {0,0,0};
	rotation[0] = horizontal_rotation;
	rotation[1] = vertical_rotation;
	rotation[2] = z_rotation;
	Set_Selection_Rotation(plane_dir, rotation);
}
