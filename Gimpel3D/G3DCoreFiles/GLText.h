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
#ifndef GLTEXT_H
#define GLTEXT_H



bool Init_GL_Text();
bool Free_GLText();

void Print_GLText_Items();

bool Set_GLFont(int id);
bool Set_GLPrint_Pos(float x, float y);
bool Set_GLText_Color(float *rgba);
bool Print_GLText(const char *fmt, ...);

bool Clear_Static_Text_Items();
bool Add_Static_Text_Item(int x, int y, float *rgba, char *text);

bool Print_Demo_Text();




#endif