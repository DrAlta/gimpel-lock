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
#ifndef CIRCULAR_MAPS_H
#define CIRCULAR_MAPS_H




class CIRCULAR_PIXEL_MAP
{
public:
	CIRCULAR_PIXEL_MAP()
	{
		pixel_map = 0;
		total = 0;
		size = 0;
	}
	~CIRCULAR_PIXEL_MAP()
	{
		if(pixel_map)delete[] pixel_map;
		pixel_map = 0;
	}
	int size;
	int total;
	int *pixel_map;
};

class CIRCULAR_FALLOFF_MAP
{
public:
	CIRCULAR_FALLOFF_MAP()
	{
		falloff_map = 0;
		size = 0;
		total = 0;
		type = -1;
	}
	~CIRCULAR_FALLOFF_MAP()
	{
		if(falloff_map)delete[] falloff_map;
		falloff_map = 0;
	}
	int size;
	int total;
	int type;
	float *falloff_map;
};

//falloff types
enum 
{
	falloff_LINEAR,//straight
	falloff_STRETCH,//pinched
	falloff_CURVE//curved
};

CIRCULAR_PIXEL_MAP* Get_Circular_Pixel_Map(int size);
CIRCULAR_FALLOFF_MAP* Get_Circular_Falloff_Map(int size, int type);
float Get_Circular_Falloff(int size, int type, float dx, float dy);



#endif