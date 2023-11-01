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
#ifndef GLWND_H
#define GLWND_H


bool Init_GLWindow(HWND pWnd, int xpos, int ypos, int width, int height, int bits, bool fullscreen);

bool KillGLWindow();
void Set_GLWnd_Focus();


void Clear_Screen();
void Update_Screen();

void Set_3D_View();
void Set_2D_View();

void Set_Perspective(float fov, float aspect);

void Set_Viewport(int x, int y, int w, int h);

int Screenwidth();
int Screenheight();
void ReSizeGLScene(int width, int height);

extern float _far_plane;
extern float _near_plane;
extern float _fov;
extern float _aspect;


bool Save_Screenshot(char *file);

extern HWND _hWnd;

bool Set_Anaglyph_Mode(bool b);

bool Set_Window_Size(int w, int h);
bool Set_Window_Pos(int x, int y);
bool Set_GLContext();

extern int _screenheight;
extern int _screenwidth;

#endif
