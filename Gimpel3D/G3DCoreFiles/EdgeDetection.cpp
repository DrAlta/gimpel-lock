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
#include "Image.h"
#include "Frame.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <vector>


extern int edit_texture;
extern int frame_texture;

bool use_cartoon_edge_detection = false;
int edge_texture = -1;

int edge_detection_range = 4;
float edge_detection_threshold = 0;
bool edge_detection_enabled = false;
bool view_edges = false;
bool update_edge_texture = true;


unsigned char gaussian_mask[75];//5x5x3
unsigned char* gaussian_mask_pixels[5][5];
unsigned char gaussian_result[3];


__forceinline unsigned char* Pixel(unsigned char *image, int w, int h, int px, int py)
{
	return &image[((py*w)+px)*3];
}

__forceinline float Pixel_Contrast(unsigned char *p1, unsigned char *p2)
{
	float rd = (float)p1[0]-p2[0];
	float gd = (float)p1[1]-p2[1];
	float bd = (float)p1[2]-p2[2];
	if(rd<0)rd = -rd;
	if(gd<0)gd = -gd;
	if(bd<0)bd = -bd;
	return rd+gd+bd;
}

__forceinline float Pixel_Contrast(unsigned char *image, int w, int h, int px, int py)
{
	unsigned char *ul = Pixel(image, w, h, px-1, py-1);
	unsigned char *ur = Pixel(image, w, h, px+1, py-1);
	unsigned char *ll = Pixel(image, w, h, px, py+1);
	unsigned char *lr = Pixel(image, w, h, px+1, py+1);
	unsigned char *t = Pixel(image, w, h, px, py-1);
	unsigned char *b = Pixel(image, w, h, px, py+1);
	unsigned char *l = Pixel(image, w, h, px-1, py);
	unsigned char *r = Pixel(image, w, h, px+1, py);
	unsigned char *c = Pixel(image, w, h, px, py);
	float res = 0;
	res += Pixel_Contrast(c, ul);
	res += Pixel_Contrast(c, ur);
	res += Pixel_Contrast(c, ll);
	res += Pixel_Contrast(c, lr);
	res += Pixel_Contrast(c, t);
	res += Pixel_Contrast(c, b);
	res += Pixel_Contrast(c, l);
	res += Pixel_Contrast(c, r);
	return res;
}


bool Init_Gaussian_Mask()
{
	int i, j;
	for(i = 0;i<5;i++)
	{
		for(j = 0;j<5;j++)
		{
			gaussian_mask_pixels[i][j] = &gaussian_mask[((j*5)+i)*3];
		}
	}
	return true;
}

__forceinline unsigned char* Safe_Pixel(unsigned char *image, int w, int h, int x, int y)
{
	if(x<0)x = 0;
	if(y<0)y = 0;
	if(x>=w)h = h-1;
	if(y>=h)y = h-1;
	return &image[((y*w)+x)*3];
}


__forceinline bool Get_Gaussian_Mask_Pixel(unsigned char *pixel, unsigned char *image, int w, int h, int x, int y)
{
	unsigned char *c = Safe_Pixel(image, w, h, x, y);
	pixel[0] = c[0];
	pixel[1] = c[1];
	pixel[2] = c[2];
	return true;
}

__forceinline bool Fill_Gaussian_Mask(unsigned char *src, int w, int h, int x, int y)
{
	int lx = x-2;
	int ly = y-2;
	int i, j;
	for(i = 0;i<5;i++)
	{
		for(j = 0;j<5;j++)
		{
			Get_Gaussian_Mask_Pixel(gaussian_mask_pixels[i][j], src, w, h, lx+i, ly+j);
		}
	}
	return true;
}

__forceinline bool Get_Gaussian_Result()
{
	float temp[3] = {0,0,0};
	unsigned char *p;
	for(int i = 0;i<25;i++)
	{
		p = &gaussian_mask[i*3];
		temp[0] += p[0];
		temp[1] += p[1];
		temp[2] += p[2];
	}
	gaussian_result[0] = (unsigned char)((int)(temp[0]/25));
	gaussian_result[1] = (unsigned char)((int)(temp[1]/25));
	gaussian_result[2] = (unsigned char)((int)(temp[2]/25));
	return true;
}

__forceinline bool Gaussian_Filter_Pixel(unsigned char *src, unsigned char *dst, int w, int h, int x, int y)
{
	Fill_Gaussian_Mask(src, w, h, x, y);
	Get_Gaussian_Result();
	dst[(((y*w)+x)*3)] = gaussian_result[0];
	dst[(((y*w)+x)*3)+1] = gaussian_result[1];
	dst[(((y*w)+x)*3)+2] = gaussian_result[2];
	return true;
}

bool Gaussian_Filter(unsigned char *src, unsigned char *dst, int w, int h)
{
	Init_Gaussian_Mask();
	int i, j;
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			Gaussian_Filter_Pixel(src, dst, w, h, i, j);
		}
	}
	return true;
}

#include <math.h>

__forceinline float Pixel_Difference(unsigned char *p1, unsigned char *p2)
{
	float rd = (float)p1[0]-p2[0];
	float gd = (float)p1[1]-p2[1];
	float bd = (float)p1[2]-p2[2];
	if(rd<0)rd = -rd;
	if(gd<0)gd = -gd;
	if(bd<0)bd = -bd;
	return rd+gd+bd;
}

__forceinline bool Get_Pixel_Direction(unsigned char *image, int w, int h, int x, int y, float *strength, unsigned char *dir)
{
	unsigned char *ul = Safe_Pixel(image, w, h, x-1, y-1);
	unsigned char *ur = Safe_Pixel(image, w, h, x, y-1);
	unsigned char *ll = Safe_Pixel(image, w, h, x, y+1);
	unsigned char *lr = Safe_Pixel(image, w, h, x+1, y+1);
	unsigned char *l = Safe_Pixel(image, w, h, x-1, y);
	unsigned char *r = Safe_Pixel(image, w, h, x+1, y);
	unsigned char *t = Safe_Pixel(image, w, h, x, y-1);
	unsigned char *b = Safe_Pixel(image, w, h, x, y+1);

	float xd = Pixel_Difference(l, r);
	float yd = Pixel_Difference(t, b);
	float ullr = Pixel_Difference(ul, lr);
	float urll = Pixel_Difference(ur, ll);

	int highest = 0;
	if(xd>highest)highest = (int)xd;
	if(yd>highest)highest = (int)yd;
	if(ullr>highest)highest = (int)ullr;
	if(urll>highest)highest = (int)urll;

	*dir = 0;
	if(highest==xd)
	{
		*dir = 0;//along y
	}
	if(highest==yd)
	{
		*dir = 1;//along x
	}
	if(highest==ullr)
	{
		*dir = 2;//along diagonal upper left to lower right
	}
	if(highest==urll)
	{
		*dir = 3;//along diagonal upper right to lower left
	}
	*strength = (float)highest;
	return true;
}

__forceinline bool Get_Pixel_Direction(unsigned char *image, int w, int h, int x, int y, float *strength)
{
	unsigned char *ul = Safe_Pixel(image, w, h, x-1, y-1);
	unsigned char *ur = Safe_Pixel(image, w, h, x, y-1);
	unsigned char *ll = Safe_Pixel(image, w, h, x, y+1);
	unsigned char *lr = Safe_Pixel(image, w, h, x+1, y+1);
	unsigned char *l = Safe_Pixel(image, w, h, x-1, y);
	unsigned char *r = Safe_Pixel(image, w, h, x+1, y);
	unsigned char *t = Safe_Pixel(image, w, h, x, y-1);
	unsigned char *b = Safe_Pixel(image, w, h, x, y+1);

	float xd = Pixel_Difference(l, r);
	float yd = Pixel_Difference(t, b);
	float ullr = Pixel_Difference(ul, lr);
	float urll = Pixel_Difference(ur, ll);

	int highest = 0;
	if(xd>highest)highest = (int)xd;
	if(yd>highest)highest = (int)yd;
	if(ullr>highest)highest = (int)ullr;
	if(urll>highest)highest = (int)urll;

	*strength = (float)highest;
	return true;
}


bool Get_Contrast_Map(unsigned char *image, int w, int h, float *contrast_map, unsigned char *pixel_dir)
{
	int i, j;
	float max = 0;
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			Get_Pixel_Direction(image, w, h, i, j, &contrast_map[(j*w)+i], &pixel_dir[(j*w)+i]);
			if(max<contrast_map[(j*w)+i])
			{
				max = contrast_map[(j*w)+i];
			}
		}
	}
	int total = w*h;
	for(i = 0;i<total;i++)
	{
		contrast_map[i] = contrast_map[i]/max;
	}
	return true;
}

bool Get_Contrast_Map(unsigned char *image, int w, int h, float *contrast_map)
{
	int i, j;
	float max = 0;
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			Get_Pixel_Direction(image, w, h, i, j, &contrast_map[(j*w)+i]);
			if(max<contrast_map[(j*w)+i])
			{
				max = contrast_map[(j*w)+i];
			}
		}
	}
	int total = w*h;
	for(i = 0;i<total;i++)
	{
		contrast_map[i] = contrast_map[i]/max;
	}
	return true;
}


bool Get_Outlines_Map(unsigned char *image, int w, int h, float *contrast_map, unsigned char *pixel_dir)
{
	int i, j, k;
	float max = 0;
	unsigned char *p;
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			k = (j*w)+i;
			p = &image[k*3];//get the total sum of all pixel brightness
			contrast_map[k] = p[0]+p[1]+p[2];
			if(max<contrast_map[k])
			{
				max = contrast_map[k];
			}
		}
	}
	int total = w*h;
	for(i = 0;i<total;i++)
	{
		//normalize the result
		contrast_map[i] = contrast_map[i]/max;
		//and invert it so the lowest values show up as the brightest
		contrast_map[i] = 1.0f-contrast_map[i];
	}
	return true;
}


bool Prune_Edge_Pixels(unsigned char *image, int w, int h, float *contrast_map, unsigned char *pixel_dir)
{
	return false;
}

bool Convert_Edge_Pixels(unsigned char *image, int w, int h, float *contrast_map, unsigned char *pixel_dir)
{
	int total = w*h;
	for(int i = 0;i<total;i++)
	{
		float v = contrast_map[i]*255;
		image[i*3] = (unsigned char)v;
		image[(i*3)+1] = (unsigned char)v;
		image[(i*3)+2] = (unsigned char)v;
	}
	return true;
}

bool Edge_Detection(unsigned char *image, unsigned char *edge_image, int w, int h, float *contrast_map)
{
	unsigned char *pixel_dir = new unsigned char[w*h];
	memcpy(edge_image, image, sizeof(unsigned char)*w*h*3);
	if(use_cartoon_edge_detection)
	{
		Get_Outlines_Map(edge_image, w, h, contrast_map, pixel_dir);
	}
	else
	{
		Get_Contrast_Map(edge_image, w, h, contrast_map, pixel_dir);
	}
	Convert_Edge_Pixels(edge_image, w, h, contrast_map, pixel_dir);
	delete[] pixel_dir;
	return true;
}

bool Update_Edge_Texture()
{
	if(edge_texture!=-1)
	{
		Free_GL_Texture(edge_texture);
		edge_texture = -1;
	}
	if(frame)
	{
		Edge_Detection(frame->original_rgb, frame->edge_image, frame->width, frame->height, frame->contrast_map);
		edge_texture = Create_Frame_Texture(frame->width, frame->height, frame->edge_image);
	}
	update_edge_texture = false;
	return true;
}

bool View_Edges(bool b)
{
	view_edges = b;
	if(b)
	{
		if(update_edge_texture)
		{
			Update_Edge_Texture();
		}
		edit_texture = edge_texture;
	}
	else
	{
		edit_texture = frame_texture;
	}
	Check_View_Edges_Checkbox(view_edges);
	return true;
}

__forceinline float Edge_Contrast_Pixel(int x, int y)
{
	return frame->contrast_map[(y*frame->width)+x];
}

bool Detect_Horizontal_Edge_Selection(int *x, int y)
{
	if(*x>=frame->width||*x<0||y>=frame->height||y<0)
	{
		return false;
	}
	int lx = *x-edge_detection_range;
	int hx = *x+edge_detection_range;
	if(lx<0)lx = 0;
	if(hx>=frame->width)hx = frame->width;
	int best = *x;
	float highest = Edge_Contrast_Pixel(*x, y);
	float v;
	for(int i = lx;i<hx;i++)
	{
		v = Edge_Contrast_Pixel(i, y);
		if(v>=edge_detection_threshold)
		{
			if(v>highest)
			{
				highest = v;
				best = i;
			}
		}
	}
	*x = best;
	return true;
}

bool Detect_Vertical_Edge_Selection(int x, int *y)
{
	if(x>=frame->width||x<0||*y>=frame->height||*y<0)
	{
		return false;
	}
	int ly = *y-edge_detection_range;
	int hy = *y+edge_detection_range;
	if(ly<0)ly = 0;
	if(hy>=frame->height)hy = frame->height;
	int best = *y;
	float highest = Edge_Contrast_Pixel(x, *y);
	float v;
	for(int i = ly;i<hy;i++)
	{
		v = Edge_Contrast_Pixel(x, i);
		if(v>=edge_detection_threshold)
		{
			if(v>highest)
			{
				highest = v;
				best = i;
			}
		}
	}
	*y = best;
	return true;
}

