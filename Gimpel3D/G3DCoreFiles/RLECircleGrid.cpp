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
#include "RLECircleGrid.h"
#include <math.h>


int rle_circle_grid_range = 0;

vector<RLE_STRIP> circle_grid_rle_strips;

bool Free_Circle_Grid_RLE_Strips()
{
	circle_grid_rle_strips.clear();
	return true;
}

bool Set_RLE_Circle_Grid(int range)
{
	if(range==rle_circle_grid_range)return false;
	circle_grid_rle_strips.clear();
	rle_circle_grid_range = range;
	int s = (range*2)+1;
	int total = s*s;
	bool *grid = new bool[total];
	memset(grid, 0, sizeof(bool)*total);
	int i, j;
	for(i = 0;i<s;i++)
	{
		for(j = 0;j<s;j++)
		{
			float dx = range-i;
			float dy = range-j;
			float d = (float)sqrt((dx*dx)+(dy*dy));
			if(d<range)
			{
				grid[(j*s)+i] = true;
			}
		}
	}
	int startx = -1;
	int endx = -1;
	for(j = 0;j<s;j++)
	{
		startx = -1;
		endx = -1;
		for(i = 0;i<s;i++)
		{
			if(startx==-1)
			{
				if(grid[(j*s)+i])
				{
					startx = i;
				}
			}
			else
			{
				if(!grid[(j*s)+i])
				{
					endx = i;
					i = s;
					RLE_STRIP rs;
					rs.start_x = startx;
					rs.end_x = endx;
					rs.y = j;
					circle_grid_rle_strips.push_back(rs);
				}
			}
		}
	}
	return true;
}

int Num_Circle_Grid_RLE_Strips()
{
	return circle_grid_rle_strips.size();
}

RLE_STRIP* Get_Circle_Grid_RLE_Strip(int index)
{
	return &circle_grid_rle_strips[index];
}
