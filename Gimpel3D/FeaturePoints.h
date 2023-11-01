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
#ifndef FEATURE_POINTS_H
#define FEATURE_POINTS_H





bool Open_Feature_Points_Dlg();
bool Close_Feature_Points_Dlg();

bool Open_Feature_Points_Tool();
bool Close_Feature_Points_Tool();

extern bool click_to_add_feature_points;
extern bool click_to_delete_feature_points;
extern bool click_to_move_feature_points;

extern bool render_feature_points_tool;
extern bool render_feature_points_2d;
extern bool render_feature_points_3d;

extern bool track_feature_points;

bool Clear_Feature_Points();

bool Keyframe_Feature_Points_At_Current_Frame(bool b);
bool Set_All_Feature_Point_Keyframes(bool b);

bool Keyframe_Selected_Feature_Point(bool b);
bool Set_Feature_Point_Dlg_Selected_Point_Keyframed(bool enable, bool check);

bool Clear_All_Keyframes_For_Selected_Feature_Point();

int Get_Selected_Feature_Point_ID();
bool Get_Feature_Point_2D_Position(int index, float *pos);

bool Select_Feature_Point(int index);

int Num_Feature_Points();

bool Click_Feature_Points_Tool(float x, float y);
bool Drag_Feature_Points_Tool(float x, float y);

bool Render_Feature_Points_2D();
bool Render_Feature_Points_3D();
bool Update_Feature_Points_Tracking();

bool Load_Feature_Points_Single_Frame(char *file);
bool Load_Feature_Points_Sequence(char *file);
bool Save_Feature_Points_Single_Frame(char *file);
bool Save_Feature_Points_Sequence(char *file);

int Get_Next_Feature_Point_Keyframe(int fpi, int frame);
int Get_Previous_Feature_Point_Keyframe(int fpi, int frame);
bool Feature_Point_Is_Keyframed(int fpi, int frame);

int Find_Feature_Point_Index(int unique_id);
int Get_Feature_Point_Unique_ID(int index);


#endif