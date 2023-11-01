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
#include "GeometryTool/GLBasic.h"
#include "CircularMaps.h"
#include <math.h>
#include <vector>

using namespace std;

vector<CIRCULAR_PIXEL_MAP*> circular_pixel_maps;
vector<CIRCULAR_FALLOFF_MAP*> circular_falloff_maps;


bool Delete_Circular_Maps()
{
	int n = circular_pixel_maps.size();
	int i;
	for(i = 0;i<n;i++){delete circular_pixel_maps[i];}
	n = circular_falloff_maps.size();
	for(i = 0;i<n;i++){delete circular_falloff_maps[i];}
	circular_pixel_maps.clear();
	circular_falloff_maps.clear();
	return true;
}

__forceinline float Get_Linear_Falloff(float x, float y, float range)
{
	return fastsqrt((x*x)+(y*y))/range;
}

__forceinline float Get_Sine_Falloff(float x, float y, float range)
{
	return sin((Get_Linear_Falloff(x,y, range)*90)*RAD);
}

__forceinline float Get_Cosine_Falloff(float x, float y, float range)
{
	return 1.0f-cos((Get_Linear_Falloff(x,y, range)*-90)*RAD);
}

int* Create_Circular_Pixel_Map(int size, int *total)
{
	bool *grid = new bool[size*size];
	memset(grid, 0, size*size);
	float range = ((float)size)*0.5;
	int cx = size/2;
	int cy = size/2;
	int i, j;
	float dist;
	float dx, dy;
	int cnt = 0;
	for(i = 0;i<size;i++)
	{
		for(j = 0;j<size;j++)
		{
			dx = i-cx;
			dy = j-cy;
			dist = sqrt((dx*dx)+(dy*dy));
			if(dist<=range)
			{
				grid[(j*size)+i] = true;
				cnt++;
			}
		}
	}
	int *res = new int[cnt*2];
	cnt = 0;
	for(i = 0;i<size;i++)
	{
		for(j = 0;j<size;j++)
		{
			if(grid[(j*size)+i])
			{
				res[cnt*2] = i-cx;
				res[(cnt*2)+1] = j-cy;
				cnt++;
			}
		}
	}
	*total = cnt;
	delete[] grid;
	return res;
}


CIRCULAR_PIXEL_MAP* Find_Circular_Pixel_Map(int size)
{
	int n = circular_pixel_maps.size();
	for(int i = 0;i<n;i++)
	{
		if(circular_pixel_maps[i]->size==size)
		{
			return circular_pixel_maps[i];
		}
	}
	return 0;
}

CIRCULAR_FALLOFF_MAP* Find_Circular_Falloff_Map(int size, int type)
{
	int n = circular_falloff_maps.size();
	for(int i = 0;i<n;i++)
	{
		if(circular_falloff_maps[i]->size==size&&circular_falloff_maps[i]->type==type)
		{
			return circular_falloff_maps[i];
		}
	}
	return 0;
}

CIRCULAR_PIXEL_MAP* Create_Circular_Pixel_Map(int size)
{
	CIRCULAR_PIXEL_MAP *cpm = new CIRCULAR_PIXEL_MAP;
	cpm->size = size;
	cpm->pixel_map = Create_Circular_Pixel_Map(size, &cpm->total);
	circular_pixel_maps.push_back(cpm);
	return cpm;
}

CIRCULAR_FALLOFF_MAP* Create_Circular_Falloff_Map(int size, int type)
{
	CIRCULAR_PIXEL_MAP *cpm = Get_Circular_Pixel_Map(size);
	CIRCULAR_FALLOFF_MAP *cfm = new CIRCULAR_FALLOFF_MAP;
	float range = ((float)size)*0.5f;
	cfm->falloff_map = new float[cpm->total];
	cfm->size = size;
	cfm->type = type;
	cfm->total = cpm->total;
	int i;

	if(type==falloff_LINEAR)
	{
		for(i = 0;i<cfm->total;i++)
		{cfm->falloff_map[i] = Get_Linear_Falloff(cpm->pixel_map[i*2], cpm->pixel_map[(i*2)+1], range);}
	}
	if(type==falloff_STRETCH)
	{
		for(i = 0;i<cfm->total;i++)
		{cfm->falloff_map[i] = Get_Sine_Falloff(cpm->pixel_map[i*2], cpm->pixel_map[(i*2)+1], range);}
	}
	if(type==falloff_CURVE)
	{
		for(i = 0;i<cfm->total;i++)
		{cfm->falloff_map[i] = Get_Cosine_Falloff(cpm->pixel_map[i*2], cpm->pixel_map[(i*2)+1], range);}
	}
	circular_falloff_maps.push_back(cfm);
	return cfm;
}

CIRCULAR_PIXEL_MAP* Get_Circular_Pixel_Map(int size)
{
	CIRCULAR_PIXEL_MAP *res = Find_Circular_Pixel_Map(size);
	if(!res)res = Create_Circular_Pixel_Map(size);
	return res;
}

CIRCULAR_FALLOFF_MAP* Get_Circular_Falloff_Map(int size, int type)
{
	CIRCULAR_FALLOFF_MAP *res = Find_Circular_Falloff_Map(size, type);
	if(!res)res = Create_Circular_Falloff_Map(size, type);
	return res;
}

float Get_Circular_Falloff(int size, int type, float dx, float dy)
{
	float res = 0;
	float range = ((float)size)*0.5f;
	if(type==falloff_LINEAR)
	{
		res = Get_Linear_Falloff(dx, dy, range);
	}
	if(type==falloff_STRETCH)
	{
		res = Get_Sine_Falloff(dx, dy, range);
	}
	if(type==falloff_CURVE)
	{
		res = Get_Cosine_Falloff(dx, dy, range);
	}
	return res;
}
