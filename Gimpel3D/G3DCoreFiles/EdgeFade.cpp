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
#include "stdafx.h"
#include "GLWnd.h"
#include <GL\gl.h>

extern bool cull_backfaces;

float default_edge_fade = 0.2f;

float edge_fade = default_edge_fade;


void Render_Edge_Fade()
{
	return;
	Set_2D_View();
	glColor3f(1,0,0);
	float w = (float)Screenwidth();
	float h = (float)Screenheight();
	float x1 = (w/2)*edge_fade;
	float x2 = w-x1;

	float c1[4] = {0,0,0,1};
	float c2[4] = {0,0,0,0.5f};
	glEnable(GL_BLEND);

	if(cull_backfaces)glDisable(GL_CULL_FACE);
	glBegin(GL_QUADS);
	glColor4fv(c1);
	glVertex2f(0,0);
	glVertex2f(0,h);
	glColor4fv(c2);
	glVertex2f(x1,h);
	glVertex2f(x1,0);

	glVertex2f(x2,0);
	glVertex2f(x2,h);
	glColor4fv(c1);
	glVertex2f(w,h);
	glVertex2f(w,0);
	glEnd();

	glDisable(GL_BLEND);
	if(cull_backfaces)glEnable(GL_CULL_FACE);
	Set_3D_View();
}

