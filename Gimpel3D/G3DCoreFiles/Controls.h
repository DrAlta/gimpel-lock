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
#ifndef CONTROLS_H
#define CONTROLS_H

bool Init_Controls();
void Update_Controls();
void Reset_Controls();
bool Update_Mouse(int x, int y);
bool Update_Key(int key, bool pressed);

void Set_Fly_Speed(float s);

extern int view_left_key;
extern int view_right_key;
extern int view_anaglyph_key;
bool Alt_Key_Pressed();


#endif
