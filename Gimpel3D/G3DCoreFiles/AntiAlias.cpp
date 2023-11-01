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
#include "Frame.h"
#include "Camera.h"
#include <gl/gl.h>
#include <gl/glu.h>

float Fint(float v);

__forceinline unsigned char* Get_Pixel(int x, int y, unsigned char *rgb)
{
	if(x<0)return 0;
	if(y<0)return 0;
	if(x>=frame->width)return 0;
	if(y>=frame->height)return 0;
	return &rgb[((y*frame->width)+x)*3];
}

__forceinline void AntiAlias_Pixel(int x, int y, unsigned char *rgb, unsigned char *dst)
{
	unsigned char *d = Get_Pixel(x, y, dst);
	if(!d)
	{
		return;
	}
	float r[3] = {0,0,0};
	int cnt = 0;
	unsigned char *p;
	p = Get_Pixel(x-1, y-1, rgb);if(p){r[0] += p[0];r[1] += p[1];r[2] += p[2];cnt++;}
	p = Get_Pixel(x,   y-1, rgb);if(p){r[0] += p[0];r[1] += p[1];r[2] += p[2];cnt++;}
	p = Get_Pixel(x+1, y-1, rgb);if(p){r[0] += p[0];r[1] += p[1];r[2] += p[2];cnt++;}
	p = Get_Pixel(x-1, y,   rgb);if(p){r[0] += p[0];r[1] += p[1];r[2] += p[2];cnt++;}
	p = Get_Pixel(x,   y,   rgb);if(p){r[0] += p[0];r[1] += p[1];r[2] += p[2];cnt++;}
	p = Get_Pixel(x+1, y,   rgb);if(p){r[0] += p[0];r[1] += p[1];r[2] += p[2];cnt++;}
	p = Get_Pixel(x-1, y+1, rgb);if(p){r[0] += p[0];r[1] += p[1];r[2] += p[2];cnt++;}
	p = Get_Pixel(x,   y+1, rgb);if(p){r[0] += p[0];r[1] += p[1];r[2] += p[2];cnt++;}
	p = Get_Pixel(x+1, y+1, rgb);if(p){r[0] += p[0];r[1] += p[1];r[2] += p[2];cnt++;}
	r[0] = r[0]/cnt;
	r[1] = r[1]/cnt;
	r[2] = r[2]/cnt;
	d[0] = (unsigned char)r[0];
	d[1] = (unsigned char)r[1];
	d[2] = (unsigned char)r[2];
}

__forceinline bool Screenhit(int x, int y, bool *p)
{
	if(frame->IsValidPixel(x, y))
	{
		return p[(y*frame->width)+x];
	}
	return false;
}

__forceinline int* Get_Old_Position(int x, int y, bool *hits, int *p)
{
	if(Screenhit(x, y, hits))
	{
		return &p[((y*frame->width)+x)*2];
	}
	return 0;
}

__forceinline int Diff(int a, int b)
{
	int c = (a-b);
	if(c<0)
	{
		return -c;
	}
	return c;
}

__forceinline bool Pixel_Overlaps(int x, int y, bool *screen_hits, int *old_positions)
{
	if(!Screenhit(x, y, screen_hits)){return true;}//false;}//true;}
	int *center  = Get_Old_Position(x, y, screen_hits, old_positions);

	int *p;

	p = Get_Old_Position(x-1, y-1, screen_hits, old_positions);if(p){if(Diff(p[0], center[0])>1||Diff(p[1], center[1])>1){return true;}}//else{return true;}
	p = Get_Old_Position(x,   y-1, screen_hits, old_positions);if(p){if(Diff(p[0], center[0])>1||Diff(p[1], center[1])>1){return true;}}//else{return true;}
	p = Get_Old_Position(x+1, y-1, screen_hits, old_positions);if(p){if(Diff(p[0], center[0])>1||Diff(p[1], center[1])>1){return true;}}//else{return true;}
	p = Get_Old_Position(x-1, y,   screen_hits, old_positions);if(p){if(Diff(p[0], center[0])>1||Diff(p[1], center[1])>1){return true;}}//else{return true;}
	p = Get_Old_Position(x+1, y,   screen_hits, old_positions);if(p){if(Diff(p[0], center[0])>1||Diff(p[1], center[1])>1){return true;}}//else{return true;}
	p = Get_Old_Position(x-1, y+1, screen_hits, old_positions);if(p){if(Diff(p[0], center[0])>1||Diff(p[1], center[1])>1){return true;}}//else{return true;}
	p = Get_Old_Position(x,   y+1, screen_hits, old_positions);if(p){if(Diff(p[0], center[0])>1||Diff(p[1], center[1])>1){return true;}}//else{return true;}
	p = Get_Old_Position(x+1, y+1, screen_hits, old_positions);if(p){if(Diff(p[0], center[0])>1||Diff(p[1], center[1])>1){return true;}}//else{return true;}

	return false;
}

bool AntiAlias_Edges(unsigned char *rgb, float *positions)
{
	unsigned char *dst = new unsigned char[frame->width*frame->height*3];
	memcpy(dst, rgb, sizeof(unsigned char)*frame->width*frame->height*3);

	//grid of original pixel coordinates for final image
	int *old_positions = new int[frame->width*frame->height*2];

	//which positions have been filled
	bool *screen_hits = new bool[frame->width*frame->height];
	memset(screen_hits, 0, sizeof(bool)*frame->width*frame->height);

	//which pixels need anti-aliasing
	bool *aa_hits = new bool[frame->width*frame->height];

	int i, j, k, tk;
	float *p;
	int px, py;
	for(i = 0;i<frame->width;i++)
	{
		for(j = 0;j<frame->height;j++)
		{
			k = (j*frame->width)+i;
			p = &positions[k*3];
			px = (int)p[0];//where did it end up
			py = (int)p[1];
			if(frame->IsValidPixel(px, py))
			{
				tk = (py*frame->width)+px;
				if(screen_hits[tk])//already been hit
				{
					//check depth
					int opx = old_positions[tk*2];//where the older one came from
					int opy = old_positions[(tk*2)+1];
					float od = positions[(((opy*frame->width)+opx)*3)+2];//the older depth
					float nd = p[2];//the current depth
					if(od>nd)//if its closer
					{
						old_positions[tk*2] = i;//replace where it came from
						old_positions[(tk*2)+1] = j;//with the current
					}
				}
				else
				{
					//first hit
					screen_hits[tk] = true;
					old_positions[tk*2] = i;//where it came from
					old_positions[(tk*2)+1] = j;
				}
			}
		}
	}

	for(j = 0;j<frame->height;j++)
	{
		for(i = 0;i<frame->width;i++)
		{
			k = (j*frame->width)+i;
			if(Pixel_Overlaps(i, j, screen_hits, old_positions))
			{
				aa_hits[k] = true;
			}
			else
			{
				aa_hits[k] = false;
			}
		}
	}

	int num_passes = 1;

	for(int z = 0;z<num_passes;z++)
	{
		for(j = 0;j<frame->height;j++)
		{
			for(i = 0;i<frame->width;i++)
			{
				k = (j*frame->width)+i;
				if(aa_hits[k])
				{
					AntiAlias_Pixel(i,   j, rgb, dst);
				}
			}
		}
		memcpy(rgb, dst, sizeof(unsigned char)*frame->width*frame->height*3);
	}
	

	delete[] old_positions;

	delete[] screen_hits;

	delete[] aa_hits;

	delete[] dst;
	return true;
}

