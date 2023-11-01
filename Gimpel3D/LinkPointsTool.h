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
#ifndef LINK_POINTS_TOOL
#define LINK_POINTS_TOOL



extern bool render_link_points_vectors;
extern bool render_link_points_3d_tool;

void Render_Link_Points_Tool_3D_Model();

void Render_Link_Points_Vectors();

bool Update_Link_Points_Dlg_View_Checkboxes();

bool Try_Save_Current_Model_Link_Points();
bool Try_Load_Model_Link_Points();

bool Link_Selected_Points();
bool UnLink_Selected_Points();

bool Clear_Link_Points();

bool Clear_All_Link_Associations();


bool Open_Link_Points_Tool(int layer_id);
bool Close_Link_Points_Tool();

bool Open_Link_Points_Dlg();
bool Close_Link_Points_Dlg();

bool Start_Iterative_Link_Point_Alignment(bool position, bool rotation, bool depth);

bool Find_Initial_Link_Point_Alignment();

extern bool iterate_link_point_alignment;
extern bool iterate_link_point_alignment_position;
extern bool iterate_link_point_alignment_rotation;
extern bool iterate_link_point_alignment_depth;
bool Update_Link_Point_Alignment();

bool Update_Link_Point_Dlg_Align_Button();

bool Start_Iterative_Link_Point_Alignment();
bool Stop_Iterative_Link_Point_Alignment();

extern bool click_to_add_link_points;
extern bool click_to_delete_link_points;
extern bool click_to_move_link_points;

extern bool render_link_points_tool;

bool Click_Link_Points_Tool_3D(float x, float y);
bool Drag_Link_Points_Tool_3D(float x, float y);

bool Show_Link_Points_Vectors(bool b);

//used by feature points tool

extern bool link_points_tool_open;
bool Notify_Link_Point_Tool_Changed_Feature_Point();

bool View_Link_Points_Model();
bool View_Link_Points_Scene();

extern bool view_link_points_model;
extern bool view_link_points_scene;

bool Save_Layer_Links_Info();

#endif