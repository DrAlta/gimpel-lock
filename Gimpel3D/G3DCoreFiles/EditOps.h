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
#ifndef EDIT_OPS_H
#define EDIT_OPS_H

//pixel position, brush size, amount moved
typedef void (*_Edit_Callback)(float x, float y, float range, float amount);
typedef void (*_StartStop_Edit_Callback)(bool b);

bool Register_Edit_Op(int *id, char *name, bool lock_brush,  _Edit_Callback edit_cb, _StartStop_Edit_Callback ss_cb);

int Num_Edit_Ops();
bool Edit_Op_Name(int id, char *res);

bool Set_Active_Op(int id);
int Get_Active_Op();

bool Update_Edit_Mouse(int x, int y);
bool Update_Edit_Mouse_Wheel(int n);
bool Update_Edit_Key(int key, bool pressed);

bool Free_Edit_Ops();

bool Start_Outline_Select();
bool Cancel_Outline_Select();
bool Finish_Outline_Select();

bool Undo_Point();
bool Redo_Point();

#endif