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
#ifndef LAYERS_H
#define LAYERS_H

#include "Frame.h"


bool New_Layer(int *id, char *name, unsigned char *color);
bool Add_Layer_RLE_Strip(int index, int y, int start_x, int end_x, bool fresh_layer);

bool Update_Layer_Data(int id);
bool Update_All_Layer_Data();

void Force_Layer_RLE_Strip(int index, int y, int start_x, int end_x);

bool Force_Add_Layer_RLE_Strip(int frame, int index, int y, int start_x, int end_x, bool fresh_layer);


bool Toggle_Select_Layer(int id);

bool Layer_Is_Selected(int index);
bool Select_Layer(int index, bool b);

bool Clear_Layer(int index);

bool Get_Layer_Depth_ID_Float(int index, float *res);

bool Get_Layer_Scale(int index, float *res);

int Find_Layer_Index(char *name);
int Find_Layer_Index(int id);

bool Freeze_Layer(int index, bool b);
bool Hide_Layer(int index, bool b);
bool Outline_Layer(int index, bool b);
bool Speckle_Layer(int index, bool b);
bool Layer_Is_Frozen(int index);
bool Layer_Is_Hidden(int index);
bool Layer_Is_Outlined(int index);
bool Layer_Is_Speckled(int index);

bool Keyframe_Layer(int index, bool b);
bool Layer_Is_Keyframed(int index);

bool Keyframe_All_Layers();

void Set_Selected_Layers_Scale(float scale);
void Adjust_Selected_Layers_Scale(float scale);


int Num_Selected_Layers();

int Get_Selected_Layer_ID(int index);


void Set_All_Layers_Depth_ID(int id);

bool Init_Default_Layer();

int Get_Layer_ID(int index);
int Get_Layer_Index(int layer_id);

int Get_Layer_Index(int frame, int layer_id);


bool Free_Layers();
bool Delete_Layer(int id);

int Num_Layers();
bool Layer_Name(int index, char *res);

bool Render_Layers_2D(float point_size, int lx, int hx, int ly, int hy);
bool Render_Layers_3D();
bool Render_Selection_Outline_3D();

void Set_Selection_Rotation(float *normal, float *rotation);//, int origin_type);
bool Set_Selection_Transform(float *pos, float *normal, float *rotation);
bool Get_Selection_Transform(float *pos, float *normal, float *rotation);

bool Get_Selection_Top(float *pos);
bool Get_Selection_Bottom(float *pos);
bool Get_Selection_Left(float *pos);
bool Get_Selection_Right(float *pos);
bool Get_Selection_Center(float *pos);

//used after auto-alignment for link points to model
bool Set_Layer_Transform(int layer_id, float *pos, float *rot);

void Project_Everything_To_Plane(float *pos, float *dir, float *rotation, float *offset, int origin_type);
void Get_Selection_Rotation(float *dir, float *rotation, int *origin_type);

bool Project_Layer_To_Plane(int layer_id, float *pos, float *dir, float *rotation, float *offset, int origin_type);

bool Project_Layer_To_Model_Geometry(int index);


bool Save_Layers(FILE *f);
bool Load_Layers(FILE *f, int n);

bool Import_Layer_Edits(FILE *f, int n);

bool Check_For_Contoured_Layers();

bool Project_Layers();

bool Update_Layers_Relief(bool reproject);

int Num_RLE_Strips_In_Layer(int index);
RLE_STRIP* Get_RLE_Strip(int layer_index, int strip_index);

bool Absorb_Layer(int layer_id, int new_layer_id);

bool Clear_Modified_Layers();
bool Update_Modified_Layers();

bool Set_Layer_Name(int index, char *name);

bool Find_Layer(char *name, int *id);

bool Get_Layer_Center(int layer_id, float *pos);
bool Get_Indexed_Layer_Center(int index, float *pos);
bool ReCalc_Layer_Center(int layer_id, float *pos);

bool ReCalc_And_Set_Layer_Center(int layer_id);

bool Get_Frame_Center(float *pos);
bool Set_Selection_Origin_Top();
bool Set_Selection_Origin_Left();
bool Set_Selection_Origin_Center();
bool Set_Selection_Origin_Right();
bool Set_Selection_Origin_Bottom();

bool Get_Selection_Origin_Center(float *pos);

bool Set_Selection_Plane_Origin(int type);

bool New_Layer_Force_ID(int id, char *name, unsigned char *color);

bool Set_Active_Frame_Layers(int frame, bool regen_data);
bool Free_Frame_Layers();
bool Allocate_Frame_Layers(int num_frames);
bool Set_Next_Highest_Layer_ID();
bool Interpolate_All_Layers(int frame);
void Enable_All_Borders(bool b);
void Update_Initial_Layer_Relief();
bool Reset_Next_Layer_ID();
bool Transfer_Layer_Selection_States(int old_frame, int new_frame);
bool Transfer_Layer_Names(int old_frame, int new_frame);

bool Copy_Layers_To_All_Frames(int frame);
bool Copy_Layers_To_Frame(int src, int dst);

bool Copy_Selected_Layer_Edits_To_All_Frames(int frame);
bool Copy_Selected_Layer_Edits_To_Frame(int src, int dst);


void Update_Pixels_Layer_IDs();
bool Dump_Disposable_Layer_Data();

bool Save_Layer_Map(char *file);
bool Set_Default_Layer_Relief_Image(char *file);
void Select_All_Layers(bool b);

bool Append_Single_Frame_Layers();

int Get_Oldest_Selected_Layer(int *index);
int Get_Most_Recently_Selected_Layer(int *index);

bool Project_Indexed_Layer_To_Plane(int index, float *pos, float *dir, float *rotation, float *offset, int origin_type, bool reproject);
bool Get_Indexed_Layer_Plane(int index, float *pos, float *dir, float *rotation, float *offset, int *origin_type);

bool Load_Contour_Pixels(char *file);
bool Save_Contour_Pixels(char *file);

bool Copy_Reference_Layer(int src_frame, int dst_frame, int layer_id);
bool Evaporate_Layer(int layer_id);
bool Delete_Layer(int frame, int id);


bool ReProject_Layer(int index);
bool Flag_Layer_For_ReProject(int index);


bool Shift_Selected_Layers_Vertically(int ydiff);

bool Refresh_All_Layer_Primitives(bool reproject);


bool Move_Layer_Geometry_Horizontally(int layer_index, float v, bool reproject);
bool Move_Layer_Geometry_Vertically(int layer_index, float v, bool reproject);
bool Move_Layer_Geometry_DepthWise(int layer_index, float v, bool reproject);
bool Scale_Layer_Geometry(int layer_index, float x, float y, float z, bool reproject);

bool Rotate_Layer_Geometry(int layer_index, float x, float y, float z, bool reproject);

bool Project_Pixel_To_Layer(int layer_index, int px, int py, float *res);

bool Pixel_Hits_Layer_Geometry(int layer_index, int px, int py, float *res);


#endif