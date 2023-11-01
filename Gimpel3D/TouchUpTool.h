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
#ifndef TOUCHUP_TOOL_H
#define TOUCHUP_TOOL_H


bool Open_TouchUp_Dlg();
bool Close_TouchUp_Dlg();

bool Open_TouchUp_Tool();
bool Close_TouchUp_Tool();

int Num_TouchUp_Edit_Groups();
bool Get_TouchUp_Edit_Group_Text(int index, char *res);

bool Append_Last_TouchUp_To_List();
bool List_All_TouchUp_Edits();

bool Clear_All_TouchUps();

bool Refresh_Layers_And_Dont_Apply_Touchups();
bool Refresh_Layers_And_Apply_Touchups();

bool Apply_Single_TouchUp_Edit(int index);
bool Remove_Single_TouchUp_Edit(int index);

bool Remember_All_TouchUp_Edits();
bool Add_All_Stored_TouchUp_Edits();


#endif