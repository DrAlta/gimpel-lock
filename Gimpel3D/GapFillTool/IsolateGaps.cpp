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
#include "../GapFillTool.h"
#include "../G3DCoreFiles/Frame.h"
#include "../G3DCoreFiles/Image.h"
#include "../G3DCoreFiles/ProjectFile.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "../Skin.h"

bool Set_GapFill_Fullscreen_View(int w, int h);
bool Set_GapFill_Image_View();
bool Set_GapFill_Blend_Image_View();
bool Restore_GapFill_View();

int gapfill_file_version = 2;

extern unsigned char *gap_fill_left_image;
extern unsigned char *gap_fill_right_image;
extern bool *gap_fill_left_mask;
extern bool *gap_fill_right_mask;
extern int gapfill_images_width;
extern int gapfill_images_height;
extern float left_gap_threshold;
extern float right_gap_threshold;
extern int left_gap_slider_pos;
extern int right_gap_slider_pos;

extern float *blend_image_pixels;
extern int blend_image_width;
extern int blend_image_height;

char gapfill_data_file[512];

float GapFill_Pixel_Size();

bool highlight_gaps = true;
bool show_gaps = true;

bool Set_Gapfill_Blend_Texture(int frame);
bool Set_Gapfill_Blend_Texture(char *file);

bool Replace_Left_GapFill_Texture(unsigned char *new_image);
bool Replace_Right_GapFill_Texture(unsigned char *new_image);

bool Set_GapFill_Blend_Image_Size(float zoom, float xpos, float ypos);
bool Get_GapFill_Blend_Image_Size(float *zoom, float *xpos, float *ypos);

bool Set_GapFill_Image_Size(float zoom, float xpos, float ypos);
bool Get_GapFill_Image_Size(float *zoom, float *xpos, float *ypos);


extern float gapfill_background_color[4];

class GAP_PIXEL
{
public:
	GAP_PIXEL()
	{
		color[0] = gapfill_background_color[0];
		color[1] = gapfill_background_color[1];
		color[2] = gapfill_background_color[2];
	}
	~GAP_PIXEL()
	{
	}
	float color[3];
	int x, y;
};

class GAP
{
public:
	GAP()
	{
		selected = false;
		fill_type = GAPFILL_NO_FILL;
		blend_frame = -1;
		finalized = false;
		blend_frame_zoom = 5;//setting for the blend image
		blend_frame_xpos = 0;
		blend_frame_ypos = 0;
		frame_zoom = 5;//setting for the frame
		frame_xpos = 0;
		frame_ypos = 0;
		blend_image[0] = 0;
	}
	~GAP()
	{
		pixels.clear();
		outline_pixels.clear();
	}
	vector<GAP_PIXEL> pixels;
	vector<int> outline_pixels;
	bool selected;
	bool finalized;
	int fill_type;
	int blend_frame;
	char blend_image[512];
	float blend_frame_zoom;
	float blend_frame_xpos;
	float blend_frame_ypos;
	float frame_zoom;
	float frame_xpos;
	float frame_ypos;
};

vector<GAP*> left_gaps;
vector<GAP*> right_gaps;

bool Free_Left_Gaps()
{
	int n = left_gaps.size();
	for(int i = 0;i<n;i++){delete left_gaps[i];}
	left_gaps.clear();
	return true;
}

bool Free_Right_Gaps()
{
	int n = right_gaps.size();
	for(int i = 0;i<n;i++){delete right_gaps[i];}
	right_gaps.clear();
	return true;
}

bool Find_First_Empty_Pixel(bool *mask, int *x, int *y, int w, int h, int startx)
{
	int i, j, k;
	for(i = startx;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			k = (j*w)+i;
			if(!mask[k])
			{
				*x = i;
				*y = j;
				return true;
			}
		}
	}
	return false;
}

bool Get_Gap_Mask(bool *mask, unsigned char *image, float threshold)
{
	int n = gapfill_images_width*gapfill_images_height;
	for(int i = 0;i<n;i++)
	{
		mask[i] = (((float)(image[(i*4)+3]))/255>=threshold);
	}
	return true;
}

__forceinline bool Gap_Mask_Pixel_Is_Open(bool *mask, int x, int y)
{
	if(x<0||y<0||x>=gapfill_images_width||y>=gapfill_images_height)
	{
		return false;
	}
	return !mask[(y*gapfill_images_width)+x];
}

//"recursively" fill the gap pixels until no more touching empty ones can be found
bool Create_New_Gap(vector<GAP*> *gaps, bool *mask, int x, int y)
{
	GAP *gap = new GAP;
	gaps->push_back(gap);
	Print_GapFill_Status("Isolating gap #%i from seed pixel %i %i", gaps->size(), x, y);
	GAP_PIXEL gp;
	gp.x = x;//start with the initial seed pixel
	gp.y = y;
	vector<GAP_PIXEL> search_pixels;
	search_pixels.push_back(gp);//push it on the stack
	while(search_pixels.size()>0)
	{
		//get the last one on the stack
		GAP_PIXEL *pgp = &search_pixels[search_pixels.size()-1];
		x = pgp->x;
		y = pgp->y;
		//pop that bitch off the stack
		search_pixels.pop_back();
		//make sure it wasn't already processed by some other neighbor
		if(Gap_Mask_Pixel_Is_Open(mask, x, y))
		{
			//use it as a base to find others
			//if any immediate neighbors are open then push them onto the stack for later
			gp.x = x-1;//upper left
			gp.y = y-1;if(Gap_Mask_Pixel_Is_Open(mask, gp.x, gp.y)){search_pixels.push_back(gp);}
			gp.x = x;//top
			gp.y = y-1;if(Gap_Mask_Pixel_Is_Open(mask, gp.x, gp.y)){search_pixels.push_back(gp);}
			gp.x = x+1;//upper right
			gp.y = y-1;if(Gap_Mask_Pixel_Is_Open(mask, gp.x, gp.y)){search_pixels.push_back(gp);}
			gp.x = x-1;//left
			gp.y = y;if(Gap_Mask_Pixel_Is_Open(mask, gp.x, gp.y)){search_pixels.push_back(gp);}
			gp.x = x+1;//right
			gp.y = y;if(Gap_Mask_Pixel_Is_Open(mask, gp.x, gp.y)){search_pixels.push_back(gp);}
			gp.x = x-1;//lower left
			gp.y = y+1;if(Gap_Mask_Pixel_Is_Open(mask, gp.x, gp.y)){search_pixels.push_back(gp);}
			gp.x = x;//bottom
			gp.y = y+1;if(Gap_Mask_Pixel_Is_Open(mask, gp.x, gp.y)){search_pixels.push_back(gp);}
			gp.x = x+1;//lower right
			gp.y = y+1;if(Gap_Mask_Pixel_Is_Open(mask, gp.x, gp.y)){search_pixels.push_back(gp);}
			//set the flag for this pixel so it doesn't get re-used by any neighbors
			mask[(y*gapfill_images_width)+x] = true;
			//and add it to the pixel list for this gap
			gp.x = x;gp.y = y;gap->pixels.push_back(gp);
		}
	}
	return true;
}

__forceinline bool Is_Outline_Gap_Pixel(int x, int y, bool *mask)
{
	if(!Gap_Mask_Pixel_Is_Open(mask, x-1, y-1))return true;
	if(!Gap_Mask_Pixel_Is_Open(mask, x, y-1))return true;
	if(!Gap_Mask_Pixel_Is_Open(mask, x+1, y-1))return true;
	if(!Gap_Mask_Pixel_Is_Open(mask, x-1, y))return true;
	if(!Gap_Mask_Pixel_Is_Open(mask, x+1, y))return true;
	if(!Gap_Mask_Pixel_Is_Open(mask, x-1, y+1))return true;
	if(!Gap_Mask_Pixel_Is_Open(mask, x, y+1))return true;
	if(!Gap_Mask_Pixel_Is_Open(mask, x+1, y+1))return true;
	return false;
}

bool Get_Outline_Gap_Pixels(GAP *gap, bool *mask)
{
	int n = gap->pixels.size();
	for(int i = 0;i<n;i++)
	{
		if(Is_Outline_Gap_Pixel(gap->pixels[i].x, gap->pixels[i].y, mask))
		{
			gap->outline_pixels.push_back(i);
		}
	}
	return true;
}

bool Identify_Gaps()
{
	unsigned char *image = 0;
	bool *mask = 0;
	vector<GAP*> *gaps;
	float gap_threshold = 0;
	if(view_gapfill_left)
	{
		image = gap_fill_left_image;
		mask = gap_fill_left_mask;
		gaps = &left_gaps;
		Free_Left_Gaps();
		gap_threshold = left_gap_threshold;
	}
	else if(view_gapfill_right)
	{
		image = gap_fill_right_image;
		mask = gap_fill_right_mask;
		gaps = &right_gaps;
		Free_Right_Gaps();
		gap_threshold = right_gap_threshold;
	}
	else
	{
		return false;
	}
	//get all gaps and fill mask as we go
	Get_Gap_Mask(mask, image, gap_threshold);
	int x, y;
	int startx = 0;
	while(Find_First_Empty_Pixel(mask, &x, &y, gapfill_images_width, gapfill_images_height, startx))
	{
		Create_New_Gap(gaps, mask, x, y);
		startx = x;
	}

	//get fresh mask with blank spots to find outline pixels
	Get_Gap_Mask(mask, image, gap_threshold);
	
	int n = gaps->size();
	for(int i = 0;i<n;i++)
	{
		Print_GapFill_Status("Finding outline for gap #%i of %i..", i+1, gaps->size());
		Get_Outline_Gap_Pixels((*gaps)[i], mask);
	}

	Print_GapFill_Status("Done.");
	return true;
}

__forceinline bool Render_Gap_Outline(GAP *gap)
{
	if(gap->selected)
	{
		glColor3f(1, 0, 0);
	}
	else
	{
		glColor3f(0.5f, 0, 0);
	}
	int n = gap->outline_pixels.size();
	for(int i = 0;i<n;i++)
	{
		glVertex2f(0.5f+gap->pixels[gap->outline_pixels[i]].x, 0.5f+gap->pixels[gap->outline_pixels[i]].y);
	}
	return true;
}

__forceinline bool Render_Gap_Outlines()
{
	vector<GAP*> *gaps;
	if(view_gapfill_left)
	{
		gaps = &left_gaps;
	}
	else if(view_gapfill_right)
	{
		gaps = &right_gaps;
	}
	else
	{
		return false;
	}
	int n = gaps->size();
	for(int i = 0;i<n;i++)
	{
		Render_Gap_Outline((*gaps)[i]);
	}
	return true;
}

__forceinline bool Render_Gap_Pixels(GAP *gap)
{
	int n = gap->pixels.size();
	for(int i = 0;i<n;i++)
	{
		glColor3fv(gap->pixels[i].color);
		glVertex2f(0.5f+gap->pixels[i].x, 0.5f+gap->pixels[i].y);
	}
	return true;
}

bool Render_Gap_Pixels()
{
	vector<GAP*> *gaps;
	if(view_gapfill_left)
	{
		gaps = &left_gaps;
	}
	else if(view_gapfill_right)
	{
		gaps = &right_gaps;
	}
	else
	{
		return false;
	}
	glPointSize(GapFill_Pixel_Size()+0.5f);
	int n = gaps->size();
	GAP *gap;
	glBegin(GL_POINTS);
	if(show_gaps)
	{
		for(int i = 0;i<n;i++)
		{
			gap = (*gaps)[i];
			if(gap->fill_type==GAPFILL_INTERPOLATE_FILL||gap->fill_type==GAPFILL_DUPLICATE_FILL||(gap->fill_type==GAPFILL_BLEND_FILL&&gap->finalized))
			{
				Render_Gap_Pixels(gap);
			}
		}
	}
	glColor3f(1, 1, 1);
	if(highlight_gaps){Render_Gap_Outlines();}
	glEnd();
	glPointSize(1);
	glColor3f(1, 1, 1);
	return true;
}


int Num_Left_Gaps()
{
	return left_gaps.size();
}

int Num_Right_Gaps()
{
	return right_gaps.size();
}

bool Clear_Left_Gap_Selection()
{
	int n = left_gaps.size();
	for(int i = 0;i<n;i++)
	{
		if(left_gaps[i]->selected)
		{
			Get_GapFill_Blend_Image_Size(&left_gaps[i]->blend_frame_zoom, &left_gaps[i]->blend_frame_xpos, &left_gaps[i]->blend_frame_ypos);
			Get_GapFill_Image_Size(&left_gaps[i]->frame_zoom, &left_gaps[i]->frame_xpos, &left_gaps[i]->frame_ypos);
		}
		left_gaps[i]->selected = false;
	}
	return true;
}

bool Clear_Right_Gap_Selection()
{
	int n = right_gaps.size();
	for(int i = 0;i<n;i++)
	{
		if(right_gaps[i]->selected)
		{
			Get_GapFill_Blend_Image_Size(&right_gaps[i]->blend_frame_zoom, &right_gaps[i]->blend_frame_xpos, &right_gaps[i]->blend_frame_ypos);
			Get_GapFill_Image_Size(&right_gaps[i]->frame_zoom, &right_gaps[i]->frame_xpos, &right_gaps[i]->frame_ypos);
		}
		right_gaps[i]->selected = false;
	}
	return true;
}

__forceinline bool Gap_Has_Pixel(GAP *gap, int x, int y)
{
	int n = gap->pixels.size();
	for(int i = 0;i<n;i++)
	{
		if(gap->pixels[i].x==x&&gap->pixels[i].y==y)
		{
			return true;
		}
	}
	return false;
}

int Find_Left_Gap_At_Pixel(int x, int y)
{
	int n = left_gaps.size();
	for(int i = 0;i<n;i++)
	{
		if(Gap_Has_Pixel(left_gaps[i], x, y))
		{
			return i;
		}
	}
	return -1;
}

int Find_Right_Gap_At_Pixel(int x, int y)
{
	int n = right_gaps.size();
	for(int i = 0;i<n;i++)
	{
		if(Gap_Has_Pixel(right_gaps[i], x, y))
		{
			return i;
		}
	}
	return -1;
}


bool Select_Left_Gap(int index)
{
	int n = left_gaps.size();
	if(index<0||index>=n)return false;
	if(left_gaps[index]->selected)
	{
		return true;
	}
	Clear_Left_Gap_Selection();
	left_gaps[index]->selected = true;
	if(left_gaps[index]->fill_type==GAPFILL_BLEND_FILL)
	{
		if(left_gaps[index]->blend_frame!=-1)
		{
			Set_Gapfill_Blend_Texture(left_gaps[index]->blend_frame);
			Set_GapFill_Blend_Image_Size(left_gaps[index]->blend_frame_zoom, left_gaps[index]->blend_frame_xpos, left_gaps[index]->blend_frame_ypos);
			Set_GapFill_Image_Size(left_gaps[index]->frame_zoom, left_gaps[index]->frame_xpos, left_gaps[index]->frame_ypos);
		}
		else if(left_gaps[index]->blend_image[0]!=0)
		{
			Set_Gapfill_Blend_Texture(left_gaps[index]->blend_image);
			Set_GapFill_Blend_Image_Size(left_gaps[index]->blend_frame_zoom, left_gaps[index]->blend_frame_xpos, left_gaps[index]->blend_frame_ypos);
			Set_GapFill_Image_Size(left_gaps[index]->frame_zoom, left_gaps[index]->frame_xpos, left_gaps[index]->frame_ypos);
		}
	}
	else
	{
		Set_Gapfill_Blend_Texture(-1);
	}
	return true;
}

bool Select_Right_Gap(int index)
{
	int n = right_gaps.size();
	if(index<0||index>=n)return false;
	if(right_gaps[index]->selected)
	{
		return true;
	}
	Clear_Right_Gap_Selection();
	right_gaps[index]->selected = true;
	if(right_gaps[index]->fill_type==GAPFILL_BLEND_FILL)
	{
		if(right_gaps[index]->blend_frame!=-1)
		{
			Set_Gapfill_Blend_Texture(right_gaps[index]->blend_frame);
			Set_GapFill_Blend_Image_Size(right_gaps[index]->blend_frame_zoom, right_gaps[index]->blend_frame_xpos, right_gaps[index]->blend_frame_ypos);
			Set_GapFill_Image_Size(right_gaps[index]->frame_zoom, right_gaps[index]->frame_xpos, right_gaps[index]->frame_ypos);
		}
		else if(right_gaps[index]->blend_image[0]!=0)
		{
			Set_Gapfill_Blend_Texture(right_gaps[index]->blend_image);
			Set_GapFill_Blend_Image_Size(right_gaps[index]->blend_frame_zoom, right_gaps[index]->blend_frame_xpos, right_gaps[index]->blend_frame_ypos);
			Set_GapFill_Image_Size(right_gaps[index]->frame_zoom, right_gaps[index]->frame_xpos, right_gaps[index]->frame_ypos);
		}
	}
	else
	{
		Set_Gapfill_Blend_Texture(-1);
	}
	return true;
}

int Left_Gap_Selection()
{
	int n = left_gaps.size();
	for(int i = 0;i<n;i++)
	{
		if(left_gaps[i]->selected)
		{
			return i;
		}
	}
	return -1;
}

int Right_Gap_Selection()
{
	int n = right_gaps.size();
	for(int i = 0;i<n;i++)
	{
		if(right_gaps[i]->selected)
		{
			return i;
		}
	}
	return -1;
}

int Get_Left_Gap_Fill_Type(int index)
{
	int n = left_gaps.size();
	if(index<0||index>=n)return -1;
	return left_gaps[index]->fill_type;
}

int Get_Right_Gap_Fill_Type(int index)
{
	int n = right_gaps.size();
	if(index<0||index>=n)return -1;
	return right_gaps[index]->fill_type;
}

bool Interpolate_Gap_Pixels(GAP *gap, unsigned char *image)
{
	int n = gap->pixels.size();
	int k;
	unsigned char *p;
	for(int i = 0;i<n;i++)
	{
		k = (gap->pixels[i].y*gapfill_images_width)+gap->pixels[i].x;
		p = &image[k*4];
		gap->pixels[i].color[0] = ((float)p[0])/255;
		gap->pixels[i].color[1] = ((float)p[1])/255;
		gap->pixels[i].color[2] = ((float)p[2])/255;
	}
	return true;
}

unsigned char* Get_Leftmost_Valid_Pixel(unsigned char *image, bool *mask, int x, int y)
{
	bool *strip = &mask[y*gapfill_images_width];
	for(int i = x-1;i>-1;i--)
	{
		if(strip[i])
		{
			//go one more for the hell of it
			if(i>0)i--;
			return &image[((y*gapfill_images_width)+i)*4];
		}
	}
	return &image[((y*gapfill_images_width))*4];//return the leftmost pixel
}

unsigned char* Get_Rightmost_Valid_Pixel(unsigned char *image, bool *mask, int x, int y)
{
	bool *strip = &mask[y*gapfill_images_width];
	for(int i = x+1;i<gapfill_images_width;i++)
	{
		if(strip[i])
		{
			//go one more for the hell of it
			if(i<frame->width-1)i++;
			return &image[((y*gapfill_images_width)+i)*4];
		}
	}
	return &image[((y*gapfill_images_width)+(gapfill_images_width-1))*4];//return the rightmost pixel
}

bool Duplicate_Leftmost_Gap_Pixels(GAP *gap)
{
	int n = gap->pixels.size();
	unsigned char *p;
	for(int i = 0;i<n;i++)
	{
		p = Get_Rightmost_Valid_Pixel(gap_fill_left_image, gap_fill_left_mask, gap->pixels[i].x, gap->pixels[i].y);
		gap->pixels[i].color[0] = ((float)p[0])/255;
		gap->pixels[i].color[1] = ((float)p[1])/255;
		gap->pixels[i].color[2] = ((float)p[2])/255;
	}
	return true;
}

bool Duplicate_Rightmost_Gap_Pixels(GAP *gap)
{
	int n = gap->pixels.size();
	unsigned char *p;
	for(int i = 0;i<n;i++)
	{
		p = Get_Leftmost_Valid_Pixel(gap_fill_right_image, gap_fill_right_mask, gap->pixels[i].x, gap->pixels[i].y);
		gap->pixels[i].color[0] = ((float)p[0])/255;
		gap->pixels[i].color[1] = ((float)p[1])/255;
		gap->pixels[i].color[2] = ((float)p[2])/255;
	}
	return true;
}

bool Set_Left_Gap_Fill_Type(int index, int fill_type)
{
	int n = left_gaps.size();
	if(index<0||index>=n)return false;
	left_gaps[index]->fill_type = fill_type;
	redraw_gapfill_window = true;
	if(fill_type==GAPFILL_INTERPOLATE_FILL)
	{
		Interpolate_Gap_Pixels(left_gaps[index], gap_fill_left_image);
	}
	else if(fill_type==GAPFILL_DUPLICATE_FILL)
	{
		Duplicate_Leftmost_Gap_Pixels(left_gaps[index]);
	}
	else if(fill_type==GAPFILL_BLEND_FILL)
	{
		if(left_gaps[index]->selected)
		{
		}
	}
	return true;
}

bool Set_Right_Gap_Fill_Type(int index, int fill_type)
{
	int n = right_gaps.size();
	if(index<0||index>=n)return false;
	right_gaps[index]->fill_type = fill_type;
	redraw_gapfill_window = true;
	if(fill_type==GAPFILL_INTERPOLATE_FILL)
	{
		Interpolate_Gap_Pixels(right_gaps[index], gap_fill_right_image);
	}
	else if(fill_type==GAPFILL_DUPLICATE_FILL)
	{
		Duplicate_Rightmost_Gap_Pixels(right_gaps[index]);
	}
	else if(fill_type==GAPFILL_BLEND_FILL)
	{
		if(right_gaps[index]->selected)
		{
		}
	}
	return true;
}


bool Set_Left_Gap_Blend_Frame(int index, int frame)
{
	int n = left_gaps.size();
	if(index<0||index>=n)return false;
	left_gaps[index]->blend_frame = frame;
	left_gaps[index]->blend_image[0] = 0;
	redraw_gapfill_window = true;
	if(view_gapfill_left&&left_gaps[index]->selected)
	{
		Set_Gapfill_Blend_Texture(frame);
		Center_BlendImage_View();
		Get_GapFill_Blend_Image_Size(&left_gaps[index]->blend_frame_zoom, &left_gaps[index]->blend_frame_xpos, &left_gaps[index]->blend_frame_ypos);
		Get_GapFill_Image_Size(&left_gaps[index]->frame_zoom, &left_gaps[index]->frame_xpos, &left_gaps[index]->frame_ypos);
	}
	return true;
}

bool Set_Right_Gap_Blend_Frame(int index, int frame)
{
	int n = right_gaps.size();
	if(index<0||index>=n)return false;
	right_gaps[index]->blend_frame = frame;
	right_gaps[index]->blend_image[0] = 0;
	redraw_gapfill_window = true;
	if(view_gapfill_right&&right_gaps[index]->selected)
	{
		Set_Gapfill_Blend_Texture(frame);
		Center_BlendImage_View();
		Get_GapFill_Blend_Image_Size(&right_gaps[index]->blend_frame_zoom, &right_gaps[index]->blend_frame_xpos, &right_gaps[index]->blend_frame_ypos);
		Get_GapFill_Image_Size(&right_gaps[index]->frame_zoom, &right_gaps[index]->frame_xpos, &right_gaps[index]->frame_ypos);
	}
	return true;
}

bool Set_Left_Gap_Blend_Image(int index, char *file)
{
	int n = left_gaps.size();
	if(index<0||index>=n)return false;
	left_gaps[index]->blend_frame = -1;
	strcpy(left_gaps[index]->blend_image, file);
	redraw_gapfill_window = true;
	if(view_gapfill_left&&left_gaps[index]->selected)
	{
		Set_Gapfill_Blend_Texture(file);
		Center_BlendImage_View();
		Get_GapFill_Blend_Image_Size(&left_gaps[index]->blend_frame_zoom, &left_gaps[index]->blend_frame_xpos, &left_gaps[index]->blend_frame_ypos);
		Get_GapFill_Image_Size(&left_gaps[index]->frame_zoom, &left_gaps[index]->frame_xpos, &left_gaps[index]->frame_ypos);
	}
	return true;
}

bool Set_Right_Gap_Blend_Image(int index, char *file)
{
	int n = right_gaps.size();
	if(index<0||index>=n)return false;
	right_gaps[index]->blend_frame = -1;
	strcpy(right_gaps[index]->blend_image, file);
	redraw_gapfill_window = true;
	if(view_gapfill_right&&right_gaps[index]->selected)
	{
		Set_Gapfill_Blend_Texture(file);
		Center_BlendImage_View();
		Get_GapFill_Blend_Image_Size(&right_gaps[index]->blend_frame_zoom, &right_gaps[index]->blend_frame_xpos, &right_gaps[index]->blend_frame_ypos);
		Get_GapFill_Image_Size(&right_gaps[index]->frame_zoom, &right_gaps[index]->frame_xpos, &right_gaps[index]->frame_ypos);
	}
	return true;
}


int Get_Left_Gap_Blend_Frame(int index)
{
	int n = left_gaps.size();
	if(index<0||index>=n)return -1;
	return left_gaps[index]->blend_frame;
}

int Get_Right_Gap_Blend_Frame(int index)
{
	int n = right_gaps.size();
	if(index<0||index>=n)return -1;
	return right_gaps[index]->blend_frame;
}

bool Finalize_GapFill_Images()
{
	return false;
}

__forceinline void Blend_Pixel_Value(int x, int y, float *rgb, float s)
{
	if(x<0||y<0||x>=blend_image_width||y>=blend_image_height)
	{
		rgb[0] = 0;
		rgb[1] = 0;
		rgb[2] = 0;
		return;
	}
	float *p = &blend_image_pixels[((y*blend_image_width)+x)*3];
	rgb[0] = p[0]*s;
	rgb[1] = p[1]*s;
	rgb[2] = p[2]*s;
}

__forceinline bool Get_Interpolated_Blend_Pixel(float px, float py, float *rgb)
{
	px-=0.5f;
	py-=0.5f;
	if(px>=blend_image_width)px-=blend_image_width;
	if(py>=blend_image_height)py-=blend_image_height;
	int lx = (int)px;
	int ly = (int)py;
	int hx = lx+1;
	int hy = ly+1;
	if(hx>=blend_image_width)hx = blend_image_width-1;
	if(hy>=blend_image_height)hy = blend_image_height-1;
	float dx = px-lx;
	float dy = py-ly;
	float ipx = 1.0f-dx;
	float ipy = 1.0f-dy;
	float top1[3];Blend_Pixel_Value(lx, ly, top1, ipx);
	float top2[3];Blend_Pixel_Value(hx, ly, top2, dx);
	float top[3] = {top1[0]+top2[0],top1[1]+top2[1],top1[2]+top2[2]};
	float bottom1[3];Blend_Pixel_Value(lx, hy, bottom1, ipx);
	float bottom2[3];Blend_Pixel_Value(hx, hy, bottom2, dx);
	float bottom[3] = {bottom1[0]+bottom2[0],bottom1[1]+bottom2[1],bottom1[2]+bottom2[2]};
	rgb[0] = (top[0]*ipy)+(bottom[0]*dy);
	rgb[1] = (top[1]*ipy)+(bottom[1]*dy);
	rgb[2] = (top[2]*ipy)+(bottom[2]*dy);
	return true;
}


bool Apply_Gapfill_Blend_Pixels(GAP *gap)
{
	//get view matrices for the current offsets and zoom
	GLdouble modelview1[16];
	GLdouble projection1[16];
	GLint vport1[8];

	GLdouble modelview2[16];
	GLdouble projection2[16];
	GLint vport2[8];

	Set_GapFill_Context();
	//set a fullscreen projection at the original image resolution
	Set_GapFill_Fullscreen_View(gapfill_images_width, gapfill_images_height);
	//screen locations will match original pixel coordinates

	//setup the view for the blend image
	Set_GapFill_Image_View();

    glGetDoublev(GL_PROJECTION_MATRIX, projection1);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview1);
    glGetIntegerv(GL_VIEWPORT, vport1);


	//setup the view for the blend image
	Set_GapFill_Blend_Image_View();

    glGetDoublev(GL_PROJECTION_MATRIX, projection2);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview2);
    glGetIntegerv(GL_VIEWPORT, vport2);
	Set_GLContext();
	
	double px, py, pz;
	double wx, wy, wz;
	int n = gap->pixels.size();
	for(int i = 0;i<n;i++)
	{
		//get the screen position of this pixel
		gluProject(0.5f+gap->pixels[i].x, 0.5f+gap->pixels[i].y, 0, modelview1, projection1, vport1, &px, &py, &pz);
		//get the world position of the pixel
		gluUnProject(px, py, pz, modelview2, projection2, vport2, &wx, &wy, &wz);
		//this is the pixel from the blend image
		Get_Interpolated_Blend_Pixel((float)wx, (float)wy, gap->pixels[i].color);
	}

	//restore the original view
	Restore_GapFill_View();

	gap->finalized = true;
	redraw_gapfill_window = true;
	return false;
}

bool Clear_Gapfill_Blend_Pixels(GAP *gap)
{
	gap->finalized = false;
	redraw_gapfill_window = true;
	return false;
}

bool Apply_GapFill_Blend_Pixels()
{
	int sel = -1;
	if(view_gapfill_left)
	{
		sel = Left_Gap_Selection();
		if(Get_Left_Gap_Fill_Type(sel)!=GAPFILL_BLEND_FILL)
		{
			return false;
		}
		return Apply_Gapfill_Blend_Pixels(left_gaps[sel]);
	}
	else if(view_gapfill_right)
	{
		sel = Left_Gap_Selection();
		if(Get_Right_Gap_Fill_Type(sel)!=GAPFILL_BLEND_FILL)
		{
			return false;
		}
		return Apply_Gapfill_Blend_Pixels(right_gaps[sel]);
	}
	else
	{
		return false;
	}
	if(sel==-1){return false;}
	return false;
}

bool Clear_GapFill_Blend_Pixels()
{
	int sel = -1;
	if(view_gapfill_left)
	{
		sel = Left_Gap_Selection();
		if(sel<0)
		{
			return false;
		}
		return Clear_Gapfill_Blend_Pixels(left_gaps[sel]);
	}
	else if(view_gapfill_right)
	{
		sel = Left_Gap_Selection();
		if(sel<0)
		{
			return false;
		}
		return Clear_Gapfill_Blend_Pixels(right_gaps[sel]);
	}
	else
	{
		return false;
	}
	if(sel==-1){return false;}
	return false;
}


__forceinline unsigned char* Get_GapFill_Pixel_24Bit(unsigned char *image, int x, int y)
{
	if(x<0)x = 0;
	if(y<0)y = 0;
	if(x>=gapfill_images_width)x = gapfill_images_width-1;
	if(y>=gapfill_images_height)y = gapfill_images_height-1;
	return &image[((y*gapfill_images_width)+x)*3];
}

__forceinline bool Get_AntiAliased_Outline_Pixel(unsigned char *image, bool *mask, int x, int y, GAP_PIXEL *gp)
{
	float res[3] = {0,0,0};
	unsigned char *p;
	p = Get_GapFill_Pixel_24Bit(image, x-1, y-1);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_GapFill_Pixel_24Bit(image, x,   y-1);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_GapFill_Pixel_24Bit(image, x+1, y-1);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_GapFill_Pixel_24Bit(image, x-1, y);  res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_GapFill_Pixel_24Bit(image, x, y);    res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_GapFill_Pixel_24Bit(image, x+1, y);  res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_GapFill_Pixel_24Bit(image, x-1, y+1);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_GapFill_Pixel_24Bit(image, x,   y+1);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_GapFill_Pixel_24Bit(image, x+1, y+1);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	res[0] = (res[0]/9);
	res[1] = (res[1]/9);
	res[2] = (res[2]/9);
	gp->color[0] = (unsigned char)res[0];
	gp->color[1] = (unsigned char)res[1];
	gp->color[2] = (unsigned char)res[2];
	gp->x = x;
	gp->y = y;
	return true;
}


bool Replace_Gap_Pixels(unsigned char *image, bool *mask, GAP *gap, bool anti_alias)
{
	int n = gap->pixels.size();
	int k = 0;
	int i;
	//replace pixels
	for(i = 0;i<n;i++)
	{
		k = ((gap->pixels[i].y*gapfill_images_width)+gap->pixels[i].x)*3;
		image[k] = (unsigned char)(gap->pixels[i].color[0]*255);
		image[k+1] = (unsigned char)(gap->pixels[i].color[1]*255);
		image[k+2] = (unsigned char)(gap->pixels[i].color[2]*255);
	}
	if(!anti_alias)
	{
		return true;
	}
	//get outline pixels that need anti-aliasing
	n = gap->pixels.size();
	GAP_PIXEL aagp;
	vector<GAP_PIXEL> aa_pixels;
	for(i = 0;i<n;i++)
	{
		if(Get_AntiAliased_Outline_Pixel(image, mask, gap->pixels[i].x, gap->pixels[i].y, &aagp))
		{
			aa_pixels.push_back(aagp);
		}
	}
	//get the adjacent pixels to the sides of the outline pixels

	bool anti_alias_outside_edges = true;

	if(anti_alias_outside_edges)
	{
		n = gap->outline_pixels.size();
		for(i = 0;i<n;i++)
		{
			if(gap->pixels[gap->outline_pixels[i]].x>0)
			{
				if(Get_AntiAliased_Outline_Pixel(image, mask, gap->pixels[gap->outline_pixels[i]].x-1, gap->pixels[gap->outline_pixels[i]].y, &aagp))
				{
					aa_pixels.push_back(aagp);
				}
			}
			if(gap->pixels[gap->outline_pixels[i]].x<frame->width-1)
			{
				if(Get_AntiAliased_Outline_Pixel(image, mask, gap->pixels[gap->outline_pixels[i]].x+1, gap->pixels[gap->outline_pixels[i]].y, &aagp))
				{
					aa_pixels.push_back(aagp);
				}
			}
		}
	}

	//fill in the image pixels with anti-aliased pixels
	n = aa_pixels.size();
	for(i = 0;i<n;i++)
	{
		k = ((aa_pixels[i].y*gapfill_images_width)+aa_pixels[i].x)*3;
		image[k] = (unsigned char)(aa_pixels[i].color[0]);
		image[k+1] = (unsigned char)(aa_pixels[i].color[1]);
		image[k+2] = (unsigned char)(aa_pixels[i].color[2]);
	}
	aa_pixels.clear();;
	return true;
}

bool Replace_Gap_Pixels(unsigned char *image, bool *mask, vector<GAP*> *gaps, bool anti_alias)
{
	int n = gaps->size();
	for(int i = 0;i<n;i++)
	{
		Replace_Gap_Pixels(image, mask, (*gaps)[i], anti_alias);
	}
	return true;
}

bool Get_Left_Blended_Gap_Fill(unsigned char *new_image, bool anti_alias)
{
	Replace_Gap_Pixels(new_image, gap_fill_left_mask, &left_gaps, anti_alias);
	return true;
}

bool Get_Right_Blended_Gap_Fill(unsigned char *new_image, bool anti_alias)
{
	Replace_Gap_Pixels(new_image, gap_fill_right_mask, &right_gaps, anti_alias);
	return true;
}

bool Preview_Blended_Gap_Fill()
{
	unsigned char *old_image = 0;
	if(view_gapfill_left)
	{
		old_image = gap_fill_left_image;
	}
	else if(view_gapfill_right)
	{
		old_image = gap_fill_right_image;
	}
	else
	{
		return false;
	}
	int n = gapfill_images_width*gapfill_images_height;
	unsigned char *new_image = new unsigned char[n*3];
	for(int i = 0;i<n;i++)
	{
		memcpy(&new_image[i*3], &old_image[i*4], sizeof(unsigned char)*3);
	}
	if(view_gapfill_left)
	{
		Replace_Gap_Pixels(new_image, gap_fill_left_mask, &left_gaps, true);
		Replace_Left_GapFill_Texture(new_image);
	}
	if(view_gapfill_right)
	{
		Replace_Gap_Pixels(new_image, gap_fill_right_mask, &right_gaps, true);
		Replace_Right_GapFill_Texture(new_image);
	}
	delete[] new_image;
	return true;
}


bool Get_GapFill_Filename(char *res)
{
	if(!frame){return false;}
	char file[512];
	if(!Get_Session_Filename(Get_Current_Project_Frame(), file))
	{
		strcpy(file, frame->filename);
	}
	char *c = strrchr(file, '.');
	if(!c)
	{
		//this never happens
		return false;
	}
	strcpy(c, ".gpf");
	strcpy(res, file);
	return true;
}

struct GAP_HEADER
{
	bool finalized;
	int fill_type;
	int blend_frame;
	int num_pixels;
	int num_outline_pixels;
	float bi_zoom;
	float bi_xpos;
	float bi_ypos;
	float i_zoom;
	float i_xpos;
	float i_ypos;
	char blend_image[512];
};

__forceinline bool Save_Gap(GAP *gap, FILE *f)
{
	int no = gap->outline_pixels.size();
	int np = gap->pixels.size();
	GAP_PIXEL *gp = new GAP_PIXEL[np];
	int *op = new int[no];
	int i;
	for(i = 0;i<np;i++)
	{
		memcpy(gp[i].color, gap->pixels[i].color, sizeof(float)*3);
		gp[i].x = gap->pixels[i].x;
		gp[i].y = gap->pixels[i].y;
	}
	for(i = 0;i<no;i++)
	{
		op[i] = gap->outline_pixels[i];
	}
	GAP_HEADER gh;
	gh.blend_frame = gap->blend_frame;
	gh.fill_type = gap->fill_type;
	gh.finalized = gap->finalized;
	gh.num_pixels = np;
	gh.num_outline_pixels = no;
	strcpy(gh.blend_image, gap->blend_image);
	gh.bi_zoom = gap->blend_frame_zoom;
	gh.bi_xpos = gap->blend_frame_xpos;
	gh.bi_ypos = gap->blend_frame_ypos;
	gh.i_zoom = gap->frame_zoom;
	gh.i_xpos = gap->frame_xpos;
	gh.i_ypos = gap->frame_ypos;
	fwrite(&gh, sizeof(GAP_HEADER), 1, f);
	fwrite(gp, sizeof(GAP_PIXEL), np, f);
	fwrite(op, sizeof(int), no, f);
	delete[] gp;
	delete[] op;
	return true;
}

__forceinline bool Load_Gap(GAP *gap, FILE *f)
{
	GAP_HEADER gh;
	fread(&gh, sizeof(GAP_HEADER), 1, f);
	GAP_PIXEL *gp = new GAP_PIXEL[gh.num_pixels];
	int *op = new int[gh.num_outline_pixels];
	fread(gp, sizeof(GAP_PIXEL), gh.num_pixels, f);
	fread(op, sizeof(int), gh.num_outline_pixels, f);
	int i;
	for(i = 0;i<gh.num_pixels;i++)
	{
		gap->pixels.push_back(gp[i]);
	}
	for(i = 0;i<gh.num_outline_pixels;i++)
	{
		gap->outline_pixels.push_back(op[i]);
	}
	gap->blend_frame = gh.blend_frame;
	gap->fill_type = gh.fill_type;
	gap->finalized = gh.finalized;
	gap->blend_frame_zoom = gh.bi_zoom;
	gap->blend_frame_xpos = gh.bi_xpos;
	gap->blend_frame_ypos = gh.bi_ypos;
	gap->frame_zoom = gh.i_zoom;
	gap->frame_xpos = gh.i_xpos;
	gap->frame_ypos = gh.i_ypos;
	strcpy(gap->blend_image, gh.blend_image);
	delete[] gp;
	delete[] op;
	return true;
}

struct GAPFILL_HEADER
{
	int version;
	int num_left;
	int num_right;
	float left_gap_threshold;
	float right_gap_threshold;
	int left_gap_slider_pos;
	int right_gap_slider_pos;
};

bool Save_GapFill_Data()
{
	char file[512];
	if(!Get_GapFill_Filename(file)){return false;}
	FILE *f = fopen(file, "wb");
	if(!f)
	{
		SkinMsgBox(0, "ERROR! Can't open file for writing.", file, MB_OK);
		return false;
	}
	GAPFILL_HEADER gh;
	gh.version = gapfill_file_version;
	gh.num_left = left_gaps.size();
	gh.num_right = right_gaps.size();
	gh.left_gap_threshold = left_gap_threshold;
	gh.right_gap_threshold = right_gap_threshold;
	gh.left_gap_slider_pos = left_gap_slider_pos;
	gh.right_gap_slider_pos = right_gap_slider_pos;
	fwrite(&gh, sizeof(GAPFILL_HEADER), 1, f);
	int i;
	for(i = 0;i<gh.num_left;i++)
	{
		if(left_gaps[i]->selected)
		{
			Get_GapFill_Blend_Image_Size(&left_gaps[i]->blend_frame_zoom, &left_gaps[i]->blend_frame_xpos, &left_gaps[i]->blend_frame_ypos);
			Get_GapFill_Image_Size(&left_gaps[i]->frame_zoom, &left_gaps[i]->frame_xpos, &left_gaps[i]->frame_ypos);
		}
		Save_Gap(left_gaps[i], f);
	}
	for(i = 0;i<gh.num_right;i++)
	{
		if(right_gaps[i]->selected)
		{
			Get_GapFill_Blend_Image_Size(&right_gaps[i]->blend_frame_zoom, &right_gaps[i]->blend_frame_xpos, &right_gaps[i]->blend_frame_ypos);
			Get_GapFill_Image_Size(&right_gaps[i]->frame_zoom, &right_gaps[i]->frame_xpos, &right_gaps[i]->frame_ypos);
		}
		Save_Gap(right_gaps[i], f);
	}
	fclose(f);
	return true;
}

bool Load_GapFill_Data()
{
	strcpy(gapfill_data_file, "NOFILE");
	char file[512];
	if(!Get_GapFill_Filename(file)){return false;}
	FILE *f = fopen(file, "rb");
	if(!f){return false;}
	GAPFILL_HEADER gh;
	fread(&gh, sizeof(GAPFILL_HEADER), 1, f);
	if(gh.version!=gapfill_file_version)
	{
		SkinMsgBox(0, "Can't load gapfill data, older version!", "This should never happen", MB_OK);
		fclose(f);
		return false;
	}
	left_gap_threshold = gh.left_gap_threshold;
	right_gap_threshold = gh.right_gap_threshold;
	left_gap_slider_pos = gh.left_gap_slider_pos;
	right_gap_slider_pos = gh.right_gap_slider_pos;
	GAP *gap = 0;
	int i;
	for(i = 0;i<gh.num_left;i++)
	{
		gap = new GAP;
		Load_Gap(gap, f);
		left_gaps.push_back(gap);
	}
	for(i = 0;i<gh.num_right;i++)
	{
		gap = new GAP;
		Load_Gap(gap, f);
		right_gaps.push_back(gap);
	}
	fclose(f);
	Get_Gap_Mask(gap_fill_left_mask, gap_fill_left_image, left_gap_threshold);
	Get_Gap_Mask(gap_fill_right_mask, gap_fill_right_image, right_gap_threshold);
	return true;
}

