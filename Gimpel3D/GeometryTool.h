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
#ifndef GEOMETRY_TOOL_H
#define GEOMETRY_TOOL_H



bool Open_Geometry_Tool();
bool Close_Geometry_Tool();

bool Open_Geometry_Dialog();
bool Close_Geometry_Dialog();

bool Get_Layer_Geometry_Type(int index, int *type);
bool Set_Layer_Geometry_Type(int index, int type);

bool Set_Layer_Geometry_Model(int index, char *file);

bool Invert_Layer_Geometry(int index, bool b);
bool Layer_Geometry_Inverted(int index);

bool Show_Layer_Geometry(int index, bool b);
bool Layer_Geometry_Visible(int index);


bool Free_Scene_Primitives();
bool Save_Scene_Primitives();
bool Load_Scene_Primitives(bool reproject);

extern bool show_geometry;
extern bool show_geometry_wireframe;

bool Render_Geometry();

bool Init_Geometry_System();
bool Free_Geometry_System();

bool Get_Layer_Primitive_Transform(int index, float *pos, float *rot);


//used for link points tool
int Find_Scene_Model_Primitive_Index(int layer_id);
bool Get_Scene_Primitive_Model_Filename(int index, char *file);
bool Get_Scene_Primitive_Model_Info(int index, float *size, float *pos, float *rot, float *scale);
bool Set_Scene_Primitive_Model_Info(int index, float *pos, float *rot, float scale);
bool Render_Scene_Primitive_Model(int index);//, float *pos, float *rot);

//used for link points tool
bool Get_Scene_Primitive_Model_Link_Info(int index, vector<int> *links);
bool Set_Scene_Primitive_Model_Link_Info(int index, vector<int> *links);


bool Layer_Geometry_Model_Loaded(int index);


#endif