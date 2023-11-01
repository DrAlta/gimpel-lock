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
#ifndef CONTROL_MESH_H
#define CONTROL_MESH_H



bool Reset_Control_Mesh();
bool Update_Samples_From_Control_Mesh();

bool Render_Control_Mesh();
bool Render_Control_Mesh_Points_2D();
bool Render_Control_Mesh_Points_3D();

int Add_Control_Mesh_Point(float x, float y);
bool Remove_Control_Mesh_Point(int id);
int Get_Selected_Control_Mesh_Point(float x, float y, float range);

float Get_Control_Mesh_Point_Depth(int id);
bool Set_Control_Mesh_Point_Depth(int id, float depth);


bool Init_Control_Mesh_Data();
bool Free_Control_Mesh_Data();

#endif