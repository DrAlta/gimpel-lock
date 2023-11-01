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
#ifndef ANCHOR_POINTS_H
#define ANCHOR_POINTS_H



bool Open_Anchor_Points_Dlg();
bool Close_Anchor_Points_Dlg();


bool Open_Anchor_Points_Tool();
bool Close_Anchor_Points_Tool();

extern bool render_anchor_points_tool;
extern bool render_anchor_points_2d;

bool Render_Anchor_Points_2D();

bool Clear_Anchor_Points();
bool Set_Displayed_Anchor_Range(float v);
bool Set_Displayed_Anchor_Type(int type);

extern float default_anchor_range;
extern int default_anchor_point_falloff_type;

extern bool update_anchor_points_in_realtime;

bool Set_Anchor_Influence_Range(float v);
bool Set_Default_Anchor_Influence_Range(float v);

bool Move_Selected_Anchor_Point_Depth(float v);

bool Update_Affected_Layers_With_Anchor_Points();
bool Update_Affected_Layers_With_Selected_Anchor_Point();

bool Set_Anchor_Point_Falloff(int type);

bool Enable_Attach_Anchor_Point_Button(bool b);
bool Enable_Remove_Anchor_Point_Button(bool b);
bool Enable_Remove_All_Anchor_Points_Button(bool b);
bool Attach_Anchor_Point_To_Selected_Feature_Point();
bool Remove_Anchor_Point_From_Selected_Feature_Point();
bool Remove_All_Anchor_Points();

#endif