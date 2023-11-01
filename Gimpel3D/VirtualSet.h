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
#ifndef VIRTUAL_SET_H
#define VIRTUAL_SET_H


bool Open_Virtual_Set_Dlg();
bool Close_Virtual_Set_Dlg();

bool Open_Virtual_Set_Tool();
bool Close_Virtual_Set_Tool();

bool Layer_Is_Dynamic(int id);
bool Set_Layer_Dynamic(int id, bool b);


#endif