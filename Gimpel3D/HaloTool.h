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
#ifndef HALO_TOOL_H
#define HALO_TOOL_H


bool Open_Halo_Dlg();
bool Close_Halo_Dlg();

bool Open_Halo_Tool();
bool Close_Halo_Tool();

bool Generate_Halo_Mask();

bool Enable_Generate_Halo_Button(bool b);

bool Generate_Default_Halos();

extern bool render_halo_tool;
extern bool render_2d_halo;
extern bool render_3d_halo;
extern bool blend_halo_with_background;
extern bool render_halo_alphamask;

bool Render_Halo_Tool_2D();
bool Render_Halo_Tool_3D();

float Halo_Pixel_Range();
bool Set_Halo_Pixel_Range(float v);

bool Update_Halo_Range_Slider();

bool Update_Halo_Geometry();
bool Update_All_Halo_Geometry();

bool Delete_Halo_Mask();
bool Delete_All_Halo_Masks();

void List_Halo_Tool_Layers();
bool Select_Halo_Tool_Layer(int layer_id);

bool Change_Active_Halo_Layer(int layer_index);

bool Set_Halo_Tool_To_Selection();

int Get_Active_Halo_Layer();

bool Depth_Sort_Halo_Pixels_And_Modify_Frame_Pixels();

bool Save_Halo_Info();
bool Apply_Saved_Halo_Info();
bool Clear_Saved_Halo_Info();

bool Load_Halos_For_Frame();

extern bool auto_load_halos;



#endif