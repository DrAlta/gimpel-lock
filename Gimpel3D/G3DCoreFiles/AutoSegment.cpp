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
#include "AutoSegment.h"
#include "LayerMask.h"

//NOTE!
/*

  A lot of the flood-fill code is similar to the gap-fill "GAP" code to identify blank areas
  and regions in a grid, maybe consolidate this into generic classes in the future..

  */

bool Get_Enumerated_Color(int layer_id, unsigned char *rgb);

bool Get_Contrast_Map(unsigned char *image, int w, int h, float *contrast_map);


class SEGMENT_PIXEL
{
public:
	SEGMENT_PIXEL()
	{
	}
	~SEGMENT_PIXEL()
	{
	}
	int x, y;
};

class SEGMENT
{
public:
	SEGMENT()
	{
	}
	~SEGMENT()
	{
		pixels.clear();
	}
	vector<SEGMENT_PIXEL> pixels;
	unsigned char color[3];
};

vector<SEGMENT*> segments;

bool Free_Segment_Data()
{
	int n = segments.size();
	for(int i = 0;i<n;i++)
	{
		delete segments[i];
	}
	segments.clear();
	return true;
}


float Pixel_Size();

//from gapfill stuff
bool Find_First_Empty_Pixel(bool *mask, int *x, int *y, int w, int h, int startx);


vector<int> auto_segment_outline_pixels;

bool *auto_segment_outline_hits = 0;

bool render_autosegment_state = false;
bool auto_segment_render_outlines_only = false;

float outline_darkness_threshold = 0.0f;
float outline_color_threshold = 0.5f;

int segment_preview_texture = 0;

int segment_contrast_map_texture = 0;

float *segment_contrast_map = 0;

__forceinline float Hue_Difference(float *rgb)
{
	float a = rgb[0]-rgb[1];
	float b = rgb[0]-rgb[2];
	float c = rgb[1]-rgb[2];
	if(a<0)a = -a;
	if(b<0)b = -b;
	if(c<0)c = -c;
	return (a+b+c)/3;
}

__forceinline bool Is_Outline_Pixel(int x, int y)
{
	if(segment_contrast_map)
	{
		return (segment_contrast_map[(y*frame->width)+x])<=outline_darkness_threshold;
	}
	float *rgb = frame->Get_RGB(x, y);
	if(Hue_Difference(rgb)>outline_color_threshold)
	{
		return false;
	}
	return ((rgb[0]+rgb[1]+rgb[2])/3)<=outline_darkness_threshold;
}

bool Update_AutoSegment_Outline_Pixels()
{
	auto_segment_outline_pixels.clear();
	int i, j;
	bool hit = false;
	for(i = 0;i<frame->width;i++)
	{
		for(j = 0;j<frame->height;j++)
		{
			hit = Is_Outline_Pixel(i, j);
			if(hit)
			{
				auto_segment_outline_pixels.push_back(i);
				auto_segment_outline_pixels.push_back(j);
			}
			auto_segment_outline_hits[(j*frame->width)+i] = hit;
		}
	}
	return true;
}

bool Init_AutoSegment_Data()
{
	int n = frame->width*frame->height;
	auto_segment_outline_hits = new bool[n];
	memset(auto_segment_outline_hits, 0, n);
	return true;
}

bool Free_AutoSegment_Data()
{
	if(auto_segment_outline_hits)delete[] auto_segment_outline_hits;
	auto_segment_outline_hits = 0;
	auto_segment_outline_pixels.clear();
	Free_Segment_Data();
	if(segment_preview_texture!=0)
	{
		Set_Edit_Context();
		Free_GL_Texture(segment_preview_texture);
		segment_preview_texture = 0;
		if(segment_contrast_map_texture!=0)
		{
			Free_GL_Texture(segment_contrast_map_texture);
			segment_contrast_map_texture = 0;
		}
		Set_GLContext();
	}
	if(segment_contrast_map_texture!=0)
	{
		Set_Edit_Context();
		Free_GL_Texture(segment_contrast_map_texture);
		segment_contrast_map_texture = 0;
		Set_GLContext();
	}
	if(segment_contrast_map){delete[] segment_contrast_map;segment_contrast_map = 0;}
	return true;
}

bool Open_AutoSegment_Tool()
{
	Free_AutoSegment_Data();
	Init_AutoSegment_Data();
	Update_AutoSegment_Outline_Pixels();
	Open_AutoSegment_Dialog();
	render_autosegment_state = true;
	redraw_edit_window = true;
	return true;
}

bool Close_AutoSegment_Tool()
{
	Free_AutoSegment_Data();
	Close_AutoSegment_Dialog();
	render_autosegment_state = false;
	redraw_edit_window = true;
	return true;
}



bool Render_AutoSegment_Outlines()
{
	if(segment_preview_texture)
	{
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, segment_preview_texture);
		glBegin(GL_QUADS);
		float xs = frame->width;
		float ys = frame->height;
		float xd = 1.0f/frame->width;
		float yd = 1.0f/frame->height;
		glTexCoord2f(0,0);glVertex2f(0, 0);
		glTexCoord2f(frame->width,0);glVertex2f(xs, 0);
		glTexCoord2f(frame->width,frame->height);glVertex2f(xs, ys);
		glTexCoord2f(0,frame->height);glVertex2f(0, ys);
		glEnd();
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
		return true;
	}
	else if(segment_contrast_map_texture)
	{
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, segment_contrast_map_texture);
		glBegin(GL_QUADS);
		float xs = frame->width;
		float ys = frame->height;
		float xd = 1.0f/frame->width;
		float yd = 1.0f/frame->height;
		glTexCoord2f(0,0);glVertex2f(0, 0);
		glTexCoord2f(frame->width,0);glVertex2f(xs, 0);
		glTexCoord2f(frame->width,frame->height);glVertex2f(xs, ys);
		glTexCoord2f(0,frame->height);glVertex2f(0, ys);
		glEnd();
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
	}

	float psize = Pixel_Size();

	glVertex2f(0,0);
	glVertex2f(frame->width, frame->height);
	int n = auto_segment_outline_pixels.size()/2;
	glPointSize(psize+1);
	if(auto_segment_render_outlines_only)
	{
		glColor3f(1,1,1);
		glBegin(GL_QUADS);
		glVertex2f(0,0);
		glVertex2f(frame->width,0);
		glVertex2f(frame->width,frame->height);
		glVertex2f(0,frame->height);
		glEnd();
		glColor3f(0,0,0);
	}
	else
	{
		glColor3f(1,0,0);
	}
	glBegin(GL_POINTS);
	for(int i = 0;i<n;i++)
	{
		glVertex2f(0.5f+auto_segment_outline_pixels[i*2], 0.5f+auto_segment_outline_pixels[(i*2)+1]);
	}
	glEnd();
	glLineWidth(psize+1);
	glBegin(GL_LINE_STRIP);
	glVertex2f(0,0);
	glVertex2f(frame->width,0);
	glVertex2f(frame->width,frame->height);
	glVertex2f(0,frame->height);
	glVertex2f(0,0);
	glEnd();
	glPointSize(1);
	glLineWidth(1);
	glColor3f(1,1,1);
	return true;
}

__forceinline bool Segment_Mask_Pixel_Is_Open(bool *mask, int x, int y)
{
	if(x<0||y<0||x>=frame->width||y>=frame->height)
	{
		return false;
	}
	return !mask[(y*frame->width)+x];
}
/*
__forceinline bool Is_Outline_Segment_Pixel(int x, int y, bool *mask)
{
	if(!Segment_Mask_Pixel_Is_Open(mask, x-1, y-1))return true;
	if(!Segment_Mask_Pixel_Is_Open(mask, x, y-1))return true;
	if(!Segment_Mask_Pixel_Is_Open(mask, x+1, y-1))return true;
	if(!Segment_Mask_Pixel_Is_Open(mask, x-1, y))return true;
	if(!Segment_Mask_Pixel_Is_Open(mask, x+1, y))return true;
	if(!Segment_Mask_Pixel_Is_Open(mask, x-1, y+1))return true;
	if(!Segment_Mask_Pixel_Is_Open(mask, x, y+1))return true;
	if(!Segment_Mask_Pixel_Is_Open(mask, x+1, y+1))return true;
	return false;
}

bool Get_Outline_Segment_Pixels(SEGMENT *segment, bool *mask)
{
	int n = segment->pixels.size();
	for(int i = 0;i<n;i++)
	{
		if(Is_Outline_Segment_Pixel(segment->pixels[i].x, segment->pixels[i].y, mask))
		{
			segment->outline_pixels.push_back(i);
		}
	}
	return true;
}
*/

bool Create_New_Segment(bool *mask, int x, int y)
{
	SEGMENT *segment = new SEGMENT;
	Get_Enumerated_Color(segments.size(), segment->color);
	segments.push_back(segment);
	SEGMENT_PIXEL sp;
	sp.x = x;//start with the initial seed pixel
	sp.y = y;
	vector<SEGMENT_PIXEL> search_pixels;
	search_pixels.push_back(sp);//push it on the stack
	while(search_pixels.size()>0)
	{
		//get the last one on the stack
		SEGMENT_PIXEL *psp = &search_pixels[search_pixels.size()-1];
		x = psp->x;
		y = psp->y;
		//pop that bitch off the stack
		search_pixels.pop_back();
		//make sure it wasn't already processed by some other neighbor
		if(Segment_Mask_Pixel_Is_Open(mask, x, y))
		{
			//use it as a base to find others
			//if any immediate neighbors are open then push them onto the stack for later
			sp.x = x-1;//upper left
			sp.y = y-1;if(Segment_Mask_Pixel_Is_Open(mask, sp.x, sp.y)){search_pixels.push_back(sp);}
			sp.x = x;//top
			sp.y = y-1;if(Segment_Mask_Pixel_Is_Open(mask, sp.x, sp.y)){search_pixels.push_back(sp);}
			sp.x = x+1;//upper right
			sp.y = y-1;if(Segment_Mask_Pixel_Is_Open(mask, sp.x, sp.y)){search_pixels.push_back(sp);}
			sp.x = x-1;//left
			sp.y = y;if(Segment_Mask_Pixel_Is_Open(mask, sp.x, sp.y)){search_pixels.push_back(sp);}
			sp.x = x+1;//right
			sp.y = y;if(Segment_Mask_Pixel_Is_Open(mask, sp.x, sp.y)){search_pixels.push_back(sp);}
			sp.x = x-1;//lower left
			sp.y = y+1;if(Segment_Mask_Pixel_Is_Open(mask, sp.x, sp.y)){search_pixels.push_back(sp);}
			sp.x = x;//bottom
			sp.y = y+1;if(Segment_Mask_Pixel_Is_Open(mask, sp.x, sp.y)){search_pixels.push_back(sp);}
			sp.x = x+1;//lower right
			sp.y = y+1;if(Segment_Mask_Pixel_Is_Open(mask, sp.x, sp.y)){search_pixels.push_back(sp);}
			//set the flag for this pixel so it doesn't get re-used by any neighbors
			mask[(y*frame->width)+x] = true;
			//and add it to the pixel list for this gap
			sp.x = x;sp.y = y;segment->pixels.push_back(sp);
		}
	}
	return true;
}

bool Find_Segments()
{
	Free_Segment_Data();
	int x, y;
	int startx = 0;
	bool *mask = new bool[frame->width*frame->height];
	memcpy(mask, auto_segment_outline_hits, frame->width*frame->height);
	while(Find_First_Empty_Pixel(mask, &x, &y, frame->width, frame->height, startx))
	{
		Create_New_Segment(mask, x, y);
		startx = x;
	}
	delete[] mask;
	return true;
}

__forceinline bool Generate_Segment_Pixels(SEGMENT *segment, unsigned char *image, int id, int *map)
{
	int n = segment->pixels.size();
	unsigned char *p;
	for(int i = 0;i<n;i++)
	{
		p = &image[((segment->pixels[i].y*frame->width)+segment->pixels[i].x)*3];
		p[0] = segment->color[0];
		p[1] = segment->color[1];
		p[2] = segment->color[2];
		map[((segment->pixels[i].y*frame->width)+segment->pixels[i].x)] = id;
	}
	return true;
}

bool Generate_Segment_Pixels(unsigned char *image, int *map)
{
	int n = segments.size();
	for(int i = 0;i<n;i++)
	{
		Generate_Segment_Pixels(segments[i], image, i, map);
	}
	return true;
}

__forceinline int Get_Map_ID(int *map, int x, int y)
{
	if(x<0)x = 0;
	if(x>=frame->width)x = frame->width-1;
	if(y<0)y = 0;
	if(y>=frame->height)y = frame->height-1;
	return map[(y*frame->width)+x];
}

__forceinline void Get_Neighboring_IDs(int *map, int x, int y, int *res)
{
	res[0] = Get_Map_ID(map, x-1, y-1);
	res[1] = Get_Map_ID(map, x,   y-1);
	res[2] = Get_Map_ID(map, x+1, y-1);
	res[3] = Get_Map_ID(map, x-1, y);
	res[4] = Get_Map_ID(map, x+1, y);
	res[5] = Get_Map_ID(map, x-1, y+1);
	res[6] = Get_Map_ID(map, x,   y+1);
	res[7] = Get_Map_ID(map, x+1, y+1);
}

__forceinline int Get_Best_Neighboring_ID(int *map, int *counts, int x, int y)
{
	int nids[8];
	Get_Neighboring_IDs(map, x, y, nids);
	int i;
	for(i = 0;i<8;i++)
	{
		if(nids[i]!=-1)
		{
			counts[nids[i]]++;//inc num hits for this id
		}
	}
	//find the one with the most hits
	int best_id = -1;
	int highest = 0;
	for(i = 0;i<8;i++)
	{
		if(nids[i]!=-1)
		{
			if(highest<counts[nids[i]])
			{
				highest = counts[nids[i]];
				best_id = nids[i];
				counts[nids[i]] = 0;//reset the count for next time
			}
		}
	}
	return best_id;
}

bool Fill_Outline_Pixels(unsigned char *image, int *map)
{
	int i, j;
	int ns = segments.size();
	int *counts = new int[ns];
	for(i = 0;i<ns;i++){counts[i] = 0;}
	vector<int> outline_pixels;//store x/y and best id in triplets
	bool done = false;
	bool keep_checking = false;//assume single pass will get them all
	//multi-pass fill missing pixels with best id from surrounding pixels
	while(!done)
	{
		for(i = 0;i<frame->width;i++)
		{
			for(j = 0;j<frame->height;j++)
			{
				if(map[(j*frame->width)+i]==-1)
				{
					outline_pixels.push_back(i);
					outline_pixels.push_back(j);
					outline_pixels.push_back(Get_Best_Neighboring_ID(map, counts, i, j));
				}
			}
		}
		keep_checking = false;
		if(outline_pixels.size()==0)//until there are no more
		{
			done = true;
		}
		else
		{
			int n = outline_pixels.size()/3;
			int x, y, id;
			for(i = 0;i<n;i++)
			{
				x = outline_pixels[i*3];
				y = outline_pixels[(i*3)+1];
				id = outline_pixels[(i*3)+2];
				if(id==-1)
				{
					keep_checking = true;//got a bad one, need to do another pass
					done = false;
				}
				else
				{
					//good one, fill in the map and the image
					map[(y*frame->width)+x] = id;
					Get_Enumerated_Color(id, &image[((y*frame->width)+x)*3]);
				}
			}
			outline_pixels.clear();
		}
	}
	delete[] counts;
	return true;
}

bool Get_AutoSegment_Layermask(unsigned char *image, int *map)
{
	Find_Segments();
	int n = frame->width*frame->height;
	memset(image, 0, n*3);
	for(int i = 0;i<n;i++)
	{
		map[i] = -1;
	}
	Generate_Segment_Pixels(image, map);
	Fill_Outline_Pixels(image, map);
	Free_Segment_Data();
	return true;
}

bool Preview_AutoSegments()
{
	unsigned char *image = new unsigned char[frame->width*frame->height*3];
	int *map = new int[frame->width*frame->height];
	Get_AutoSegment_Layermask(image, map);
	Set_Edit_Context();
	if(segment_preview_texture!=0){Free_GL_Texture(segment_preview_texture);}
	segment_preview_texture = Create_Frame_Texture(frame->width, frame->height, image);
	Set_GLContext();
	delete[] image;
	delete[] map;
	return true;
}

bool Reset_Preview_AutoSegments()
{
	if(segment_preview_texture!=0)
	{
		Set_Edit_Context();
		Free_GL_Texture(segment_preview_texture);
		segment_preview_texture = 0;
		Set_GLContext();
	}
	return true;
}

bool Finalize_AutoSegments()
{
	unsigned char *image = new unsigned char[frame->width*frame->height*3];
	int *map = new int[frame->width*frame->height];
	Get_AutoSegment_Layermask(image, map);
	Load_Layer_Mask(image, frame->width, frame->height);
	Update_Layer_List();
	delete[] image;
	delete[] map;
	Close_AutoSegment_Tool();
	return true;
}


bool Generate_AutoSegment_Contrast_Map()
{
	unsigned char *image = new unsigned char[frame->width*frame->height*3];
	if(segment_contrast_map){delete[] segment_contrast_map;segment_contrast_map = 0;}

	segment_contrast_map = new float[frame->width*frame->height];
	
	Get_Contrast_Map(frame->original_rgb, frame->width, frame->height, segment_contrast_map);
	int n = frame->width*frame->height;
	unsigned char c;
	for(int i = 0;i<n;i++)
	{
		segment_contrast_map[i] = 1.0f-segment_contrast_map[i];
		c = (unsigned char)(segment_contrast_map[i]*255);
		image[(i*3)] = c;
		image[(i*3)+1] = c;
		image[(i*3)+2] = c;
	}
	Set_Edit_Context();
	if(segment_contrast_map_texture!=0){Free_GL_Texture(segment_contrast_map_texture);}
	segment_contrast_map_texture = Create_Frame_Texture(frame->width, frame->height, image);
	Set_GLContext();
	delete[] image;
	return true;
}


