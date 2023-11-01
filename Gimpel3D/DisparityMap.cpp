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
#include "G3DCoreFiles/Image.h"
#include "Skin.h"

int disparity_window_size = 50;

class DISPARITY_INFO
{
public:
	DISPARITY_INFO()
	{
	}
	~DISPARITY_INFO()
	{
	}
	int px, py;//pixel location
	float mx;//horizontal change in x
	float d;//confidence
};

__forceinline float Disparity(float *a, float *b)
{
	float d1 = a[0]-b[0];
	float d2 = a[1]-b[1];
	float d3 = a[2]-b[2];
	if(d1<0)d1 = -d1;
	if(d2<0)d2 = -d2;
	if(d3<0)d3 = -d3;
	return d1+d2+d3;
}

__forceinline float Get_Disparity(float *s1, float *s2)
{
	float res = 0;
	for(int i = 0;i<disparity_window_size;i++)
	{
		res += Disparity(&s1[i*3], &s2[i*3]);
	}
	return res;
}

__forceinline bool Get_Pixel_Disparity(float *strip1, float *strip2, int w, int x, DISPARITY_INFO *di)
{
	float *start1 = &strip1[((disparity_window_size/2)+x)*3];//half the search window before the x pixel
	float *start2 = &strip2[((disparity_window_size)+x)*3];//full search window before the x pixel
	float lowest = 999999;
	int best = -1;
	float d;
	for(int i = 0;i<disparity_window_size;i++)
	{
		d = Get_Disparity(start1, start2);
		if(d<lowest||best==-1)
		{
			best = i;
			lowest = d;
		}
		start2 = &start2[3];
	}
	di->mx = (float)((disparity_window_size/2)-best);
	di->d = lowest;
	return true;
}

float *dstrip_image = 0;

__forceinline bool Get_Disparity_Across_Strip(int w, int h, int y, float *strip1, float *strip2, float *i1, float *i2, DISPARITY_INFO *di)
{
	float *rs1 = &i1[(y*w)*3];
	float *rs2 = &i2[(y*w)*3];
	int i;
	for(i = 0;i<disparity_window_size;i++)
	{
		memcpy(&strip1[i*3], rs1, sizeof(float)*3);//duplicate first pixel
		memcpy(&strip1[(i+disparity_window_size+w)*3], &rs1[(w-1)*3], sizeof(float)*3);//and last
		memcpy(&strip2[i*3], rs2, sizeof(float)*3);
		memcpy(&strip2[(i+disparity_window_size+w)*3], &rs2[(w-1)*3], sizeof(float)*3);//and last
	}
	//fill in the strip
	memcpy(&strip1[disparity_window_size*3], rs1, sizeof(float)*w*3);
	memcpy(&strip2[disparity_window_size*3], rs2, sizeof(float)*w*3);
	for(i = 0;i<w;i++)
	{
		Get_Pixel_Disparity(strip1, strip2, w, i, &di[i]);
	}
	int nw = w+(disparity_window_size*2);
	memcpy(&dstrip_image[(y*nw)*3], strip1, sizeof(float)*nw*3);
	return true;
}

bool Generate_Disparity_Image(int w, int h, DISPARITY_INFO *dmap, char *file)
{
	int n = w*h;
	float *data = new float[w*h*3];
	float max = 0;
	int i;
	for(i = 0;i<n;i++)
	{
		if(dmap[i].mx<0)dmap[i].mx = -dmap[i].mx;
		if(max<dmap[i].mx)max = dmap[i].mx;
	}
	for(i = 0;i<n;i++)
	{
		dmap[i].mx = dmap[i].mx/max;
		data[i*3] = dmap[i].mx;
		data[(i*3)+1] = dmap[i].mx;
		data[(i*3)+2] = dmap[i].mx;
	}
	Save_Clean_Image(data, w, h, file);
	return true;
}


bool Test_Disparity_Map(char *file1, char *file2, char *output)
{
	int w1 = 0;
	int h1 = 0;
	int w2 = 0;
	int h2 = 0;
	unsigned char *i1 = Get_RGB_Image_Data(file1, &w1, &h1);
	unsigned char *i2 = Get_RGB_Image_Data(file2, &w2, &h2);
	if(!i1||!i2||w1!=w2||h1!=h2)
	{
		SkinMsgBox("Error loading left/right pairs for disparity map!", 0, MB_OK);
		if(i1)delete[] i1;
		if(i2)delete[] i2;
		return false;
	}
	float *fi1 = new float[w1*h1*3];
	float *fi2 = new float[w1*h1*3];
	Convert_Image_To_Float(fi1, i1, w1*h1);
	Convert_Image_To_Float(fi2, i2, w1*h1);
	DISPARITY_INFO *dmap = new DISPARITY_INFO[w1*h1];
	int j;
	float *strip1 = new float[(w1+(disparity_window_size*2))*3];
	float *strip2 = new float[(w1+(disparity_window_size*2))*3];
	
	dstrip_image = new float[(w1+(disparity_window_size*2))*h1*3];
	
	for(j = 0;j<h1;j++)
	{
		Get_Disparity_Across_Strip(w1, h1, j, strip1, strip2, fi1, fi2, &dmap[(j*w1)]);
	}
	Generate_Disparity_Image(w1, h1, dmap, output);
	
	Save_Clean_Image(dstrip_image, w1+(disparity_window_size*2), h1, "d:\\spd.bmp");

	delete dstrip_image;
	
	delete[] strip1;
	delete[] strip2;
	delete[] dmap;
	delete[] fi1;
	delete[] fi2;
	delete[] i1;
	delete[] i2;
	return true;
}



