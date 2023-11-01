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
#include "G3D.h"
#include <gl\gl.h>

bool render_grid = true;

bool Render_Grid()
{
	int num_units = 100;
	float size = 1;
	float spacing = size/num_units;
	float start = -size/2;
	float gray = 0.5f;
	float grid_color[3] = {gray,gray,gray};
	glColor3fv(grid_color);
	glBegin(GL_LINES);
	int i;
	for(i = 0;i<num_units+1;i++)
	{
		glVertex3f(start, 0, start+(i*spacing));
		glVertex3f(start+size, 0, start+(i*spacing));
	}
	for(i = 0;i<num_units+1;i++)
	{
		glVertex3f(start+(i*spacing), 0, start);
		glVertex3f(start+(i*spacing), 0, start+size);
	}
	glEnd();
	return true;
}
