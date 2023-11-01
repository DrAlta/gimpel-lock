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
#include "../G3DCoreFiles/Camera.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "../Skin.h"

void Push_3D_View();
void Pop_3D_View();

bool Get_Frame_Image(int index, char *res);

bool Set_GLContext();
void Set_3D_View();

void Get_Matrices();

void Set_Left_View(int w, int h);
void Set_Right_View(int w, int h);
bool Get_Clean_Left_Frame(unsigned char *rgb);
bool Get_Clean_Right_Frame(unsigned char *rgb);

bool Generate_Anaglyph_Image_From_32Bit(int n, unsigned char *left, unsigned char *right, unsigned char *res);
bool Generate_Anaglyph_Image_From_24Bit(int n, unsigned char *left, unsigned char *right, unsigned char *res);

int Find_Left_Gap_At_Pixel(int x, int y);
int Find_Right_Gap_At_Pixel(int x, int y);


bool view_gapfill_left = true;
bool view_gapfill_right = false;
bool view_gapfill_anaglyph = false;

extern int gapfill_texture;
extern int gapfill_blend_texture;

int gapfill_images_width = 0;
int gapfill_images_height = 0;

float left_gap_threshold = 0;
float right_gap_threshold = 0;

int left_gap_slider_pos = 0;
int right_gap_slider_pos = 0;

unsigned char *gap_fill_left_image = 0;
unsigned char *gap_fill_right_image = 0;
unsigned char *gap_fill_anaglyph_image = 0;

unsigned char *original_gap_fill_left_image = 0;
unsigned char *original_gap_fill_right_image = 0;


float *gap_fill_left_gapsizes = 0;
float *gap_fill_right_gapsizes = 0;

bool *gap_fill_left_mask = 0;
bool *gap_fill_right_mask = 0;

float *blend_image_pixels = 0;

int blend_image_width = 0;
int blend_image_height = 0;

int gapfill_left_texture = 0;
int gapfill_right_texture = 0;
int gapfill_anaglyph_texture = 0;

bool gapfill_view_left = true;
bool gapfill_view_right = true;
bool gapfill_view_anaglyph = true;

bool Clear_Left_Gap_Selection();
bool Clear_Right_Gap_Selection();

//don't round down, figure which side of 0.5
__forceinline float Round_Off_4Realz(double v)
{
	int base = (int)v;
	double rest =  v-base;
	if(rest>=0.5)base++;
	return base;
}

bool Get_Left_Raycast_Map(float *res);
bool Get_Right_Raycast_Map(float *res);

bool Get_Pixel_3D_Raycast_Hits(int width, int height, bool *hits, float *ray_hits)
{
	int i, j;
	memset(hits, 0, sizeof(bool)*width*height);
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			float *wp = &ray_hits[((j*width)+i)*3];
			int px = Round_Off_4Realz(wp[0]);
			int py = Round_Off_4Realz(wp[1]);
			if(!(px<0||px>=width||py<0||py>=height))
			{
				hits[(py*width)+px] = true;
			}
		}
	}
	//fill in single pixel holes, these are rounding errors, a single pixel isn't worth filling anyway
	for(j = 0;j<height;j++)
	{
		for(i = 1;i<width-1;i++)
		{
			if(!hits[(j*width)+i])//pixel is empty
			{
				if(hits[((j*width)+i)-1]&&hits[(j*width)+i+1])//if adjacent pixels are hit
				{
					hits[(j*width)+i] = true;//flag it as hit
				}
			}
		}
	}
	return true;
}

bool Expand_Hit_Gaps_Rightward(bool *hits, int width, int height)
{
	int i, j;
	for(j = 0;j<height;j++)
	{
		for(i = 1;i<width-1;i++)
		{
			if(hits[(j*width)+i])//pixel is hit
			{
				if(!hits[((j*width)+i)-1])//if previous pixel is empty
				{
					hits[(j*width)+i] = false;//flag it as empty
					//and increase the index so this doesn't drag the gap leftward forever
					i++;
				}
			}
		}
	}
	return true;
}

bool Expand_Hit_Gaps_Leftward(bool *hits, int width, int height)
{
	int i, j;
	for(j = 0;j<height;j++)
	{
		for(i = 1;i<width-1;i++)
		{
			if(!hits[(j*width)+i])//pixel is empty
			{
				if(hits[((j*width)+i)-1])//if previous pixel is hit
				{
					hits[((j*width)+i)-1] = false;//flag previous as empty
				}
			}
		}
	}
	return true;
}

bool Get_Pixel_Gap_Size_Map(int width, int height, bool *hits, float *buffer)
{
	int i, j, k;
	bool ingap = false;
	bool hit;
	int gap_size = 0;
	memset(buffer, 0, sizeof(float)*width*height);
	for(j = 0;j<height;j++)
	{
		ingap = false;
		for(i = 0;i<width;i++)
		{
			hit = hits[(j*width)+i];
			if(hit)
			{
				if(ingap)
				{
					ingap = false;
					//we are now out of the gap, fill in all the previous pixels
					for(k = i-gap_size;k<i;k++)
					{
						buffer[(j*width)+k] = gap_size;
					}
				}
				else
				{
					//still waiting for the next gap
				}
			}
			else
			{
				if(!ingap)
				{
					//starting a new gap, reset size to at least 1
					gap_size = 1;
					ingap = true;
					//now wait for more emtpy pixels
				}
				else
				{
					//we are in the gap, add to the size
					gap_size++;
				}
			}
		}
		//did we end with a gap?
		if(ingap)
		{
			ingap = false;
			for(k = i-gap_size;k<i;k++)//get the last string of empty pixels
			{
				buffer[(j*width)+k] = gap_size;
			}
		}//this happens naturally for one side
	}
	return true;
}

bool Get_Left_View_Gapmask(unsigned char *rgb)
{
	bool *hits = new bool[frame->width*frame->height];
	float *ray_hits = new float[frame->width*frame->height*3];
	Get_Left_Raycast_Map(ray_hits);
	Get_Pixel_3D_Raycast_Hits(frame->width, frame->height, hits, ray_hits);
	Expand_Hit_Gaps_Rightward(hits, frame->width, frame->height);
	Expand_Hit_Gaps_Rightward(hits, frame->width, frame->height);
	int n = frame->width*frame->height;
	unsigned char *dst;
	for(int i = 0;i<n;i++)
	{
		dst = &rgb[i*3];
		if(hits[i])
		{
			dst[0] = dst[1] = dst[2] = 0;
		}
		else
		{
			dst[0] = dst[1] = dst[2] = 255;
		}
	}
	delete[] ray_hits;
	delete[] hits;
	return true;
}

bool Get_Right_View_Gapmask(unsigned char *rgb)
{
	bool *hits = new bool[frame->width*frame->height];
	float *ray_hits = new float[frame->width*frame->height*3];
	Get_Right_Raycast_Map(ray_hits);
	Get_Pixel_3D_Raycast_Hits(frame->width, frame->height, hits, ray_hits);
	Expand_Hit_Gaps_Leftward(hits, frame->width, frame->height);
	Expand_Hit_Gaps_Leftward(hits, frame->width, frame->height);
	int n = frame->width*frame->height;
	unsigned char *dst;
	for(int i = 0;i<n;i++)
	{
		dst = &rgb[i*3];
		if(hits[i])
		{
			dst[0] = dst[1] = dst[2] = 0;
		}
		else
		{
			dst[0] = dst[1] = dst[2] = 255;
		}
	}
	delete[] ray_hits;
	delete[] hits;
	return true;
}


bool Get_Pixel_Gap_Map(float *buffer, bool left, bool right)
{
	bool *hits = new bool[frame->width*frame->height];
	float *ray_hits = new float[frame->width*frame->height*3];
	if(left)
	{
		Get_Left_Raycast_Map(ray_hits);
		Get_Pixel_3D_Raycast_Hits(frame->width, frame->height, hits, ray_hits);
	}
	if(right)
	{
		Get_Right_Raycast_Map(ray_hits);
		Get_Pixel_3D_Raycast_Hits(frame->width, frame->height, hits, ray_hits);
	}
	delete[] ray_hits;
	if(left)
	{
		Expand_Hit_Gaps_Rightward(hits, frame->width, frame->height);
		Expand_Hit_Gaps_Rightward(hits, frame->width, frame->height);
	}
	if(right)
	{
		Expand_Hit_Gaps_Leftward(hits, frame->width, frame->height);
		Expand_Hit_Gaps_Leftward(hits, frame->width, frame->height);
	}
	Get_Pixel_Gap_Size_Map(frame->width, frame->height, hits, buffer);
	delete[] hits;
	return true;
}


void View_GapFill_Left()
{
	view_gapfill_left = true;
	view_gapfill_right = false;
	view_gapfill_anaglyph = false;
	redraw_gapfill_window = true;
	gapfill_texture = gapfill_left_texture;
	Clear_Right_Gap_Selection();
}

void View_GapFill_Right()
{
	view_gapfill_left = false;
	view_gapfill_right = true;
	view_gapfill_anaglyph = false;
	redraw_gapfill_window = true;
	gapfill_texture = gapfill_right_texture;
	Clear_Left_Gap_Selection();
}

void View_GapFill_Anaglyph()
{
	view_gapfill_left = false;
	view_gapfill_right = false;
	view_gapfill_anaglyph = true;
	redraw_gapfill_window = true;
	gapfill_texture = gapfill_anaglyph_texture;
}

bool Get_Pixel_Gap_Maps()
{
	Get_Pixel_Gap_Map(gap_fill_left_gapsizes, true, false);

	Get_Pixel_Gap_Map(gap_fill_right_gapsizes, false, true);
	return true;
}

bool Get_Gap_Alphas(unsigned char *image, float *gsizes, int n)
{
	float max_gap = 0;
	int i;
	for(i = 0;i<n;i++)
	{
		if(max_gap<gsizes[i])max_gap = gsizes[i];
	}
	unsigned char *p;
	unsigned char c;
	float v;
	unsigned char max_alpha = 0;
	for(i = 0;i<n;i++)
	{
		if(gsizes[i]==0)
		{
			c = 255;
		}
		else
		{
			v = 1.0f-((gsizes[i])/(max_gap));
			v *= 255;
			c = (unsigned char)v;
		}
		p = &image[i*4];
		p[3] = c;
	}
	return true;
}



bool Create_GapFill_Images()
{
	int n = frame->width*frame->height;
	gap_fill_left_image = new unsigned char[(n+32)*4];
	gap_fill_right_image = new unsigned char[(n+32)*4];
	original_gap_fill_left_image = new unsigned char[(n+32)*4];
	original_gap_fill_right_image = new unsigned char[(n+32)*4];
	gap_fill_anaglyph_image = new unsigned char[(n+32)*3];
	gap_fill_left_gapsizes = new float[n];
	gap_fill_right_gapsizes = new float[n];
	gap_fill_left_mask = new bool[n];
	gap_fill_right_mask = new bool[n];
	bool orb = render_borders;
	if(!orb)
	{
		Enable_All_Borders(true);
		render_borders = true;
	}
	Get_Clean_Left_Frame(gap_fill_anaglyph_image);
	RGB_To_RGBA(gap_fill_left_image, gap_fill_anaglyph_image, n);
	Get_Clean_Right_Frame(gap_fill_anaglyph_image);
	RGB_To_RGBA(gap_fill_right_image, gap_fill_anaglyph_image, n);
	Generate_Anaglyph_Image_From_32Bit(n, gap_fill_left_image, gap_fill_right_image, gap_fill_anaglyph_image);
	if(!orb)
	{
		Enable_All_Borders(false);
		render_borders = false;
	}
	Get_Pixel_Gap_Maps();
	Get_Gap_Alphas(gap_fill_left_image, gap_fill_left_gapsizes, n);
	Get_Gap_Alphas(gap_fill_right_image, gap_fill_right_gapsizes, n);
	Set_GapFill_Context();
	gapfill_left_texture = Create_32Bit_Frame_Texture(frame->width, frame->height, gap_fill_left_image);
	gapfill_right_texture = Create_32Bit_Frame_Texture(frame->width, frame->height, gap_fill_right_image);
	gapfill_anaglyph_texture = Create_Frame_Texture(frame->width, frame->height, gap_fill_anaglyph_image);
	Set_GapFill_Frame_Size(frame->width, frame->height);
	Center_GapFill_View();
	Set_GLContext();
	if(view_gapfill_left)gapfill_texture = gapfill_left_texture;
	if(view_gapfill_right)gapfill_texture = gapfill_right_texture;
	if(view_gapfill_anaglyph)gapfill_texture = gapfill_anaglyph_texture;
	gapfill_images_width = frame->width;
	gapfill_images_height = frame->height;
	memcpy(original_gap_fill_left_image, gap_fill_left_image, sizeof(unsigned char)*(n+32)*4);
	memcpy(original_gap_fill_right_image, gap_fill_right_image, sizeof(unsigned char)*(n+32)*4);
	return true;
}

bool Free_GapFill_Images()
{
	if(gap_fill_left_image)delete[] gap_fill_left_image;
	if(gap_fill_right_image)delete[] gap_fill_right_image;
	if(original_gap_fill_left_image)delete[] original_gap_fill_left_image;
	if(original_gap_fill_right_image)delete[] original_gap_fill_right_image;
	if(gap_fill_anaglyph_image)delete[] gap_fill_anaglyph_image;
	if(gap_fill_left_gapsizes)delete[] gap_fill_left_gapsizes;
	if(gap_fill_right_gapsizes)delete[] gap_fill_right_gapsizes;
	if(gap_fill_left_mask)delete[] gap_fill_left_mask;
	if(gap_fill_right_mask)delete[] gap_fill_right_mask;
	if(blend_image_pixels)delete[] blend_image_pixels;
	blend_image_width = 0;
	blend_image_height = 0;
	gapfill_images_width = 0;
	gapfill_images_height = 0;
	blend_image_pixels = 0;
	gap_fill_left_image = 0;
	gap_fill_right_image = 0;
	original_gap_fill_left_image = 0;
	original_gap_fill_right_image = 0;
	gap_fill_anaglyph_image = 0;
	gap_fill_left_gapsizes = 0;
	gap_fill_right_gapsizes = 0;
	gap_fill_left_mask = 0;
	gap_fill_right_mask = 0;
	Set_GapFill_Context();
	Free_GL_Texture(gapfill_left_texture);
	Free_GL_Texture(gapfill_right_texture);
	Free_GL_Texture(gapfill_anaglyph_texture);
	Free_GL_Texture(gapfill_blend_texture);
	Set_GLContext();
	gapfill_left_texture = 0;
	gapfill_right_texture = 0;
	gapfill_anaglyph_texture = 0;
	gapfill_blend_texture = -1;
	return true;
}

bool gapfill_tool_open = false;

bool Open_GapFill_Tools()
{
	if(gapfill_tool_open)
	{
		return false;
	}
	gapfill_tool_open = true;
	//get the left/right images from origin
	Push_3D_View();
	Set_GLContext();
	Center_View();
	Set_Camera_To_Target();//snap to target camera
	Set_3D_View();
	Open_GapFill_Dialog();
	Free_GapFill_Images();
	Print_GapFill_Status("Generating images for gap fill preview..");
	Create_GapFill_Images();
	redraw_gapfill_window = true;
	//reset the 3d view
	Set_GLContext();
	Center_View();
	Set_Camera_To_Target();
	Set_3D_View();
	redraw_frame = true;
	Print_GapFill_Status("Done.");
	//check for existing data for this frame
	Load_GapFill_Data();
	Update_GapFill_Slider();
	Update_GapFill_List();
	return true;
}


bool Close_GapFill_Tools()
{
	if(!gapfill_tool_open)
	{
		return false;
	}
	gapfill_tool_open = false;
	//save any existing data for this frame
	Save_GapFill_Data();
	Close_GapFill_Dialog();
	Free_Left_Gaps();
	Free_Right_Gaps();
	Free_GapFill_Images();
	redraw_gapfill_window = false;
	Restore_Windows_From_GapFill_Tool();
	Pop_3D_View();
	return true;
}

void Set_Left_Gap_Threshold(float v, int slider_pos)
{
	left_gap_threshold = v;
	left_gap_slider_pos = slider_pos;
	redraw_gapfill_window = true;
}

void Set_Right_Gap_Threshold(float v, int slider_pos)
{
	right_gap_threshold = v;
	right_gap_slider_pos = slider_pos;
	redraw_gapfill_window = true;
}


bool Free_Gapfill_Blend_Texture()
{
	if(blend_image_pixels)delete[] blend_image_pixels;
	if(gapfill_blend_texture>-1)
	{
		Set_GapFill_Context();
		Free_GL_Texture(gapfill_blend_texture);
		gapfill_blend_texture = -1;
		Set_GLContext();
	}
	blend_image_pixels = 0;
	blend_image_width = 0;
	blend_image_height = 0;
	return true;
}


bool Set_Gapfill_Blend_Texture(char *file)
{
	char image[512];
	char msg[512];
	strcpy(image, file);
	if(blend_image_pixels)delete[] blend_image_pixels;
	blend_image_pixels = 0;
	int width = 0;
	int height = 0;
	unsigned char *data = Get_RGB_Image_Data(image, &width, &height);
	if(!data)
	{
		sprintf(msg, "Error loading image %s", image);
		Print_Status(msg);
		Print_To_Console(msg);
		SkinMsgBox(0, msg, 0, MB_OK);
		return false;
	}
	Set_GapFill_Context();
	if(gapfill_blend_texture>-1)
	{
		Free_GL_Texture(gapfill_blend_texture);
		gapfill_blend_texture = -1;
	}
	gapfill_blend_texture = Create_Frame_Texture(width, height, data);
	Set_GapFill_Blend_Image_Size(width, height);
	Set_GLContext();
	
	blend_image_pixels = new float[width*height*3];
	Convert_Image_To_Float(blend_image_pixels, data, width*height);
	delete[] data;

	blend_image_width = width;
	blend_image_height = height;
	return true;
}

bool Set_Gapfill_Blend_Texture(int frame)
{
	if(frame==-1)
	{
		return Free_Gapfill_Blend_Texture();
	}
	char image[512];
	char msg[512];
	if(!Get_Frame_Image(frame, image))
	{
		sprintf(msg, "Error getting image filename for frame index %i", frame);
		SkinMsgBox(0, msg, 0, MB_OK);
		return false;
	}
	if(blend_image_pixels)delete[] blend_image_pixels;
	blend_image_pixels = 0;
	int width = 0;
	int height = 0;
	unsigned char *data = Get_RGB_Image_Data(image, &width, &height);
	if(!data)
	{
		sprintf(msg, "Error loading image %s for frame index %i", image, frame);
		Print_Status(msg);
		Print_To_Console(msg);
		SkinMsgBox(0, msg, 0, MB_OK);
		return false;
	}
	Set_GapFill_Context();
	if(gapfill_blend_texture>-1)
	{
		Free_GL_Texture(gapfill_blend_texture);
		gapfill_blend_texture = -1;
	}
	gapfill_blend_texture = Create_Frame_Texture(width, height, data);
	Set_GapFill_Blend_Image_Size(width, height);
	Set_GLContext();

	blend_image_pixels = new float[width*height*3];
	Convert_Image_To_Float(blend_image_pixels, data, width*height);
	delete[] data;

	blend_image_width = width;
	blend_image_height = height;
	return true;
}

bool Replace_Left_GapFill_Texture(unsigned char *new_image)
{
	Set_GapFill_Context();
	Free_GL_Texture(gapfill_left_texture);
	gapfill_left_texture = Create_Frame_Texture(gapfill_images_width, gapfill_images_height, new_image);
	Set_GLContext();
	return true;
}

bool Replace_Right_GapFill_Texture(unsigned char *new_image)
{
	Set_GapFill_Context();
	Free_GL_Texture(gapfill_right_texture);
	gapfill_right_texture = Create_Frame_Texture(gapfill_images_width, gapfill_images_height, new_image);
	Set_GLContext();
	return true;
}

bool Restore_Left_GapFill_Texture()
{
	Set_GapFill_Context();
	Free_GL_Texture(gapfill_left_texture);
	gapfill_left_texture = Create_32Bit_Frame_Texture(gapfill_images_width, gapfill_images_height, original_gap_fill_left_image);
	Set_GLContext();
	return true;
}

bool Restore_Right_GapFill_Texture()
{
	Set_GapFill_Context();
	Free_GL_Texture(gapfill_right_texture);
	gapfill_right_texture = Create_32Bit_Frame_Texture(gapfill_images_width, gapfill_images_height, original_gap_fill_right_image);
	Set_GLContext();
	return true;
}

bool Restore_Original_GapFill_Texture()
{
	if(view_gapfill_left)
	{
		Restore_Left_GapFill_Texture();
	}
	else if(view_gapfill_right)
	{
		Restore_Right_GapFill_Texture();
	}
	return true;
}

bool Get_Left_Blended_Gap_Fill_Image(unsigned char *rgb)
{
	int n = gapfill_images_width*gapfill_images_height;
	for(int i = 0;i<n;i++)
	{
		memcpy(&rgb[i*3], &gap_fill_left_image[i*4], sizeof(unsigned char)*3);
	}
	Get_Left_Blended_Gap_Fill(rgb, true);
	return true;
}

bool Get_Right_Blended_Gap_Fill_Image(unsigned char *rgb)
{
	int n = gapfill_images_width*gapfill_images_height;
	for(int i = 0;i<n;i++)
	{
		memcpy(&rgb[i*3], &gap_fill_right_image[i*4], sizeof(unsigned char)*3);
	}
	Get_Right_Blended_Gap_Fill(rgb, true);
	return true;
}

bool Preview_GapFilled_Anaglyph()
{
	int n = gapfill_images_width*gapfill_images_height;
	unsigned char *left = new unsigned char[n*3];
	unsigned char *right = new unsigned char[n*3];
	unsigned char *anaglyph = new unsigned char[n*3];
	for(int i = 0;i<n;i++)
	{
		memcpy(&left[i*3], &gap_fill_left_image[i*4], sizeof(unsigned char)*3);
		memcpy(&right[i*3], &gap_fill_right_image[i*4], sizeof(unsigned char)*3);
	}

	Get_Left_Blended_Gap_Fill(left, true);
	Get_Right_Blended_Gap_Fill(right, true);

	Generate_Anaglyph_Image_From_24Bit(n, left, right, anaglyph);

	Set_GapFill_Context();
	Free_GL_Texture(gapfill_anaglyph_texture);
	gapfill_anaglyph_texture = Create_Frame_Texture(gapfill_images_width, gapfill_images_height, anaglyph);
	Set_GLContext();
	delete[] left;
	delete[] right;
	delete[] anaglyph;
	return true;
}


bool Select_Gap_At_Pixel(int x, int y)
{
	if(x<0||y<0||x>=gapfill_images_width||y>=gapfill_images_height)
	{
		return false;
	}
	bool *mask = 0;
	if(view_gapfill_left)
	{
		mask = gap_fill_left_mask;
	}
	else if(view_gapfill_right)
	{
		mask = gap_fill_right_mask;
	}
	else
	{
		return false;
	}
	if(mask[(y*gapfill_images_width)+x])
	{
		return false;//no gap at pixel
	}
	int gap_index = -1;
	if(view_gapfill_left)
	{
		gap_index = Find_Left_Gap_At_Pixel(x, y);
		if(gap_index==-1)
		{
			return false;
		}
		Select_Left_Gap(gap_index);
		Update_GapFill_List();
		redraw_gapfill_window = true;
		return true;
	}
	else if(view_gapfill_right)
	{
		gap_index = Find_Right_Gap_At_Pixel(x, y);
		if(gap_index==-1)
		{
			return false;
		}
		Select_Right_Gap(gap_index);
		Update_GapFill_List();
		redraw_gapfill_window = true;
		return true;
	}
	return false;
}


struct AA_PIXEL
{
	int x, y;
	unsigned char r, g, b;
};

__forceinline unsigned char* Get_AA_Pixel(unsigned char *rgb, int x, int y, int width, int height)
{
	if(x<0)x = 0;
	if(y<0)y = 0;
	if(x>=width)x = width-1;
	if(y>=height)y = height-1;
	return &rgb[((y*width)+x)*3];
}

__forceinline bool Get_AA_Pixel(unsigned char *image, int x, int y, AA_PIXEL *ap, int width, int height)
{
	float res[3] = {0,0,0};
	unsigned char *p;
	p = Get_AA_Pixel(image, x-1, y-1, width, height);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_AA_Pixel(image, x,   y-1, width, height);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_AA_Pixel(image, x+1, y-1, width, height);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_AA_Pixel(image, x-1, y, width, height);  res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_AA_Pixel(image, x, y, width, height);    res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_AA_Pixel(image, x+1, y, width, height);  res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_AA_Pixel(image, x-1, y+1, width, height);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_AA_Pixel(image, x,   y+1, width, height);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	p = Get_AA_Pixel(image, x+1, y+1, width, height);res[0] += (float)p[0];res[1] += (float)p[1];res[2] += (float)p[2];
	res[0] = (res[0]/9);
	res[1] = (res[1]/9);
	res[2] = (res[2]/9);
	ap->r = (unsigned char)res[0];
	ap->g = (unsigned char)res[1];
	ap->b = (unsigned char)res[2];
	if(x<0)x = 0;
	if(y<0)y = 0;
	if(x>=width)x = width-1;
	if(y>=height)y = height-1;
	ap->x = x;
	ap->y = y;
	return true;
}

bool Anti_Alias_Image(unsigned char *rgb, bool *mask, int width, int height)
{
	AA_PIXEL ap;
	vector<AA_PIXEL> aa_pixels;
	int i, j, k;
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			k = (j*width)+i;
			if(!mask[k])
			{
				Get_AA_Pixel(rgb, i, j, &ap, width, height);aa_pixels.push_back(ap);
				Get_AA_Pixel(rgb, i, j-1, &ap, width, height);aa_pixels.push_back(ap);//get above
				Get_AA_Pixel(rgb, i, j+1, &ap, width, height);aa_pixels.push_back(ap);//and below
				Get_AA_Pixel(rgb, i-1, j, &ap, width, height);aa_pixels.push_back(ap);//get left
				Get_AA_Pixel(rgb, i+1, j, &ap, width, height);aa_pixels.push_back(ap);//and right
			}
		}
	}
	int n = aa_pixels.size();
	for(i = 0;i<n;i++)
	{
		k = (aa_pixels[i].y*width)+aa_pixels[i].x;
		rgb[k*3] = aa_pixels[i].r;
		rgb[(k*3)+1] = aa_pixels[i].g;
		rgb[(k*3)+2] = aa_pixels[i].b;
	}
	aa_pixels.clear();
	return true;
}

struct FAA_PIXEL
{
	int x, y;
	float r, g, b;
};

//sample the frame image to duplicate patterns
__forceinline void Sample_Image_Pixels(float x, float y, unsigned char *rgb, unsigned char *res)
{
	int ix = (int)x;//the starting location of this pixel
	int iy = (int)y;

	//get nearest sample

	if(ix<0)ix = 0;
	if(iy<0)iy = 0;
	if(ix>=frame->width)ix = frame->width-1;
	if(iy>=frame->height)iy = frame->height-1;

	int k = (frame->width*iy)+x;

	res[0] = rgb[k*3];
	res[1] = rgb[(k*3)+1];
	res[2] = rgb[(k*3)+2];
}

bool Auto_Fill_Gaps_In_Left_Image(unsigned char *rgb, float *positions)
{
	bool *hits = new bool[frame->width*frame->height];
	float *ray_hits = positions;
	if(!positions)
	{
		ray_hits = new float[frame->width*frame->height*3];
		Get_Left_Raycast_Map(ray_hits);
	}
	Get_Pixel_3D_Raycast_Hits(frame->width, frame->height, hits, ray_hits);
	Expand_Hit_Gaps_Rightward(hits, frame->width, frame->height);
	Expand_Hit_Gaps_Rightward(hits, frame->width, frame->height);
	int i, j, k;
	//copy over background pixels

	//a vector for searching into image for mirrored pattern
	float pattern_vector[2] = {1,0};
	float pattern_sample_pos[2] = {0,0};
	float search_dir = 1;//direction to modulate searches
	int max_search_hits = 2;//the max number of pixels before it goes back
	int num_search_hits = 0;
	
	for(j = 0;j<frame->height;j++)
	{
		for(i = frame->width-1;i>-1;i--)
		{
			k = (j*frame->width)+i;
			if(hits[k])
			{
				pattern_sample_pos[0] = i;//save this is the last valid pixel location
				pattern_sample_pos[1] = j;
				num_search_hits = 0;
				search_dir = 1;
			}
			else
			{
				Sample_Image_Pixels(pattern_sample_pos[0], pattern_sample_pos[1], rgb, &rgb[k*3]);
				num_search_hits++;
				if(num_search_hits>=max_search_hits)
				{
					num_search_hits = 0;
					search_dir = -search_dir;
				}
			}
		}
	}
	Anti_Alias_Image(rgb, hits, frame->width, frame->height);
	delete[] hits;
	if(!positions)
	{
		delete[] ray_hits;
	}
	return true;
}


bool Auto_Fill_Gaps_In_Right_Image(unsigned char *rgb, float *positions)
{
	bool *hits = new bool[frame->width*frame->height];
	float *ray_hits = positions;
	if(!positions)
	{
		ray_hits = new float[frame->width*frame->height*3];
		Get_Right_Raycast_Map(ray_hits);
	}
	Get_Pixel_3D_Raycast_Hits(frame->width, frame->height, hits, ray_hits);
	Expand_Hit_Gaps_Leftward(hits, frame->width, frame->height);
	Expand_Hit_Gaps_Leftward(hits, frame->width, frame->height);
	int i, j, k;
	//copy over background pixels

	//a vector for searching into image for mirrored pattern
	float pattern_vector[2] = {-1,0};
	float pattern_sample_pos[2] = {0,0};
	float search_dir = 1;//direction to modulate searches
	int max_search_hits = 20;//the max number of pixels before it goes back
	int num_search_hits = 0;

	for(j = 0;j<frame->height;j++)
	{
		for(i = 0;i<frame->width;i++)
		{
			k = (j*frame->width)+i;
			if(hits[k])
			{
				pattern_sample_pos[0] = i;//save this is the last valid pixel location
				pattern_sample_pos[1] = j;
				num_search_hits = 0;
				search_dir = 1;
			}
			else
			{
				Sample_Image_Pixels(pattern_sample_pos[0], pattern_sample_pos[1], rgb, &rgb[k*3]);
				num_search_hits++;
				if(num_search_hits>=max_search_hits)
				{
					num_search_hits = 0;
					search_dir = -search_dir;
				}
			}
		}
	}
	Anti_Alias_Image(rgb, hits, frame->width, frame->height);
	delete[] hits;
	if(!positions)
	{
		delete[] ray_hits;
	}
	return true;
}



