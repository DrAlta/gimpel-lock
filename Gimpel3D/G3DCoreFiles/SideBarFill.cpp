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


float *temp_float_strip1 = 0;
float *temp_float_strip2 = 0;

__forceinline unsigned char* Pixel(unsigned char *pixels, int x, int y, int w, int h)
{
	return &pixels[((y*w)+x)*3];
}

__forceinline int Num_Blank_Pixels_On_Left(unsigned char *pixels, int y, int w, int h)
{
	unsigned char *pixel;
	for(int i = 0;i<w;i++)
	{
		pixel = Pixel(pixels, i, y, w, h);
		if(pixel[0]!=0||pixel[1]!=0||pixel[2]!=0)
		{
			return i;
		}
	}
	return 0;
}

__forceinline int Num_Blank_Pixels_On_Right(unsigned char *pixels, int y, int w, int h)
{
	unsigned char *pixel;
	for(int i = 0;i<w;i++)
	{
		pixel = Pixel(pixels, (w-1)-i, y, w, h);
		if(pixel[0]!=0||pixel[1]!=0||pixel[2]!=0)
		{
			return i;
		}
	}
	return 0;
}

__forceinline bool Get_Stretched_Pixel(float *res, float *pixels, int n, float rp)
{
	float fi = rp*n;
	int i = (int)fi;
	if(i==n-1)
	{
		res[0] = pixels[(i*3)];
		res[1] = pixels[(i*3)+1];
		res[2] = pixels[(i*3)+2];
		return true;
	}
	float p = fi-i;
	float ip = 1.0f-p;
	float *px1 = &pixels[i*3];
	float *px2 = &pixels[(i+1)*3];
	float fade = rp + ((1.0f-rp)*0.5f);
	res[0] = ((px1[0]*ip)+(px2[0]*p));
	res[1] = ((px1[1]*ip)+(px2[1]*p));
	res[2] = ((px1[2]*ip)+(px2[2]*p));
	return true;
}

__forceinline bool Stretch_Pixels(float *pixels, int n)
{
	float *temp = temp_float_strip2;
	float p, rp;
	for(int i = 0;i<n;i++)
	{
		p = ((float)i)/(n);
		rp = 0.5f+(p*0.5f);
		Get_Stretched_Pixel(&temp[i*3], pixels, n, rp);
	}
	memcpy(pixels, temp, sizeof(float)*3*n);
	return true;
}

__forceinline bool Fill_Right_Of_Horizontal_Strip(unsigned char *pixels, int y, int w, int h)
{
	int n = Num_Blank_Pixels_On_Right(pixels, y, w, h);
	if(n==0)
	{
		return false;
	}
	n = n*2;
	float *temp = temp_float_strip1;
	unsigned char *pixel;
	int i;
	for(i = 0;i<n;i++)
	{
		pixel = Pixel(pixels, (w-1)-i, y, w, h);
		temp[i*3] = ((float)pixel[0])/255;
		temp[(i*3)+1] = ((float)pixel[1])/255;
		temp[(i*3)+2] = ((float)pixel[2])/255;
	}
	Stretch_Pixels(temp, n);
	float r, g, b;
	for(i = 0;i<n;i++)
	{
		pixel = Pixel(pixels, (w-1)-i, y, w, h);
		r = temp[(i*3)]*255;
		g = temp[(i*3)+1]*255;
		b = temp[(i*3)+2]*255;
		pixel[0] = (unsigned char)(r);
		pixel[1] = (unsigned char)(g);
		pixel[2] = (unsigned char)(b);
	}
	return true;
}

__forceinline bool Fill_Left_Of_Horizontal_Strip(unsigned char *pixels, int y, int w, int h)
{
	int n = Num_Blank_Pixels_On_Left(pixels, y, w, h);
	if(n==0)
	{
		return false;
	}
	n = n*2;
	float *temp = temp_float_strip1;
	unsigned char *pixel;
	int i;
	for(i = 0;i<n;i++)
	{
		pixel = Pixel(pixels, i, y, w, h);
		temp[i*3] = ((float)pixel[0])/255;
		temp[(i*3)+1] = ((float)pixel[1])/255;
		temp[(i*3)+2] = ((float)pixel[2])/255;
	}
	Stretch_Pixels(temp, n);
	float r, g, b;
	for(i = 0;i<n;i++)
	{
		pixel = Pixel(pixels, i, y, w, h);
		r = temp[(i*3)]*255;
		g = temp[(i*3)+1]*255;
		b = temp[(i*3)+2]*255;
		pixel[0] = (unsigned char)(r);
		pixel[1] = (unsigned char)(g);
		pixel[2] = (unsigned char)(b);
	}
	return true;
}

bool Fill_Left_Side(unsigned char *rgb, int w, int h)
{
	temp_float_strip1 = new float[w*3];
	temp_float_strip2 = new float[w*3];
	//border is on right
	for(int i = 0;i<h;i++)
	{
		Fill_Right_Of_Horizontal_Strip(rgb, i, w, h);
	}
	delete[] temp_float_strip1;
	delete[] temp_float_strip2;
	temp_float_strip1 = 0;
	temp_float_strip2 = 0;
	return false;
}

bool Fill_Right_Side(unsigned char *rgb, int w, int h)
{
	temp_float_strip1 = new float[w*3];
	temp_float_strip2 = new float[w*3];
	//border is on left
	for(int i = 0;i<h;i++)
	{
		Fill_Left_Of_Horizontal_Strip(rgb, i, w, h);
	}
	delete[] temp_float_strip1;
	delete[] temp_float_strip2;
	temp_float_strip1 = 0;
	temp_float_strip2 = 0;
	return false;
}

