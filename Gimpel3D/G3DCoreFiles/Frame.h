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
#ifndef FRAME_H
#define FRAME_H

#include <vector>

using namespace std;
#include "../GeometryTool/GLBasic.h"

bool Fill_Selection_Buffer();

struct FRAME_PIXEL
{
	int x, y;
	int layer_id;
	float contour_ratio;
	float relief;
};

struct RLE_STRIP
{
	int y;
	int start_x, end_x;
};

#define SFI_VERSION 3

class FRAME
{
public:
	FRAME()
	{
		vertices = 0;
		identity_vectors = 0;
		frame_vectors = 0;
		rgb_colors = 0;
		edge_image = 0;
		original_rgb = 0;
		selection_buffer = 0;
		pixels = 0;
		contrast_map = 0;
		width = height = 0;
		strcpy(image_file, "NO_FILE");
		strcpy(depth_file, "NO_FILE");
		strcpy(layers_file, "NO_FILE");
		version = SFI_VERSION;
		anaglyph_bump = 0;
		strcpy(filename, "NOFILE");
		view_origin[0] = 0;
		view_origin[1] = 0;
		view_origin[2] = 0;
		view_rotation[0] = 0;
		view_rotation[1] = 0;
		view_rotation[2] = 0;
		view_direction[0] = 0;
		view_direction[1] = 0;
		view_direction[2] = 1;
		selection_buffer_is_current = false;
	}
	~FRAME()
	{
		if(vertices)delete[] vertices;
	
		//verify we are using separate buffers)
		if(identity_vectors==frame_vectors)
		{
			//it's the same buffer, just delete it once
			if(identity_vectors)delete[] identity_vectors;
		}
		else
		{
			//we are using different buffers, delete them both
			if(identity_vectors)delete[] identity_vectors;
			if(frame_vectors)delete[] frame_vectors;
		}
		if(rgb_colors)delete[] rgb_colors;
		if(pixels)delete[] pixels;
		if(contrast_map)delete[] contrast_map;
		if(edge_image)delete[] edge_image;
		if(original_rgb)delete[] original_rgb;
		if(selection_buffer)delete[] selection_buffer;
	}
	__forceinline float* Get_Pos(int k)
	{
		return &vertices[k*3];
	}
	__forceinline float *Get_Pos(int x, int y)
	{
		return &vertices[((y*width)+x)*3];
	}
	__forceinline float* Get_RGB(int k)
	{
		return &rgb_colors[k*3];
	}
	__forceinline float *Get_RGB(int x, int y)
	{
		return &rgb_colors[((y*width)+x)*3];
	}
	__forceinline unsigned char* Get_Original_RGB(int k)
	{
		return &original_rgb[k*3];
	}
	__forceinline unsigned char *Get_Original_RGB(int x, int y)
	{
		return &original_rgb[((y*width)+x)*3];
	}
	__forceinline float* Get_Identity_Vector(int k)
	{
		return &identity_vectors[k*3];
	}
	__forceinline float* Get_Identity_Vector(int x, int y)
	{
		return &identity_vectors[((y*width)+x)*3];
	}
	__forceinline float* Get_Frame_Vector(int k)
	{
		return &frame_vectors[k*3];
	}
	__forceinline float* Get_Frame_Vector(int x, int y)
	{
		return &frame_vectors[((y*width)+x)*3];
	}
	__forceinline int Get_Pixel_Layer(int x, int y)
	{
		if(x<0||x>=width||y<0||y>=height)
		{
			return -1;
		}
		return pixels[(y*width)+x].layer_id;
	}
	__forceinline void Set_Pixel_Layer(int x, int y, int layer_id)
	{
		pixels[(y*width)+x].layer_id = layer_id;
	}
	__forceinline void Set_Contour_Ratio(int x, int y, float r)
	{
		pixels[(y*width)+x].contour_ratio = r;
	}
	void Reset_Contour()
	{
		int t = width*height;
		for(int i = 0;i<t;i++)
		{
			pixels[i].contour_ratio = 0;
		}
	}
	void Get_Average_Center(float *res)
	{
		res[0] = 0;
		res[1] = 0;
		res[2] = 0;
		int n = width*height;
		if(n==0)
		{
			return;
		}
		for(int i = 0;i<n;i++)
		{
			res[0] += vertices[i*3];
			res[1] += vertices[(i*3)+1];
			res[2] += vertices[(i*3)+2];
		}
		res[0] = res[0]/n;
		res[1] = res[1]/n;
		res[2] = res[2]/n;
	}
	__forceinline bool IsValidPixel(int px, int py)
	{
		return !(px<0||px>=width||py<0||py>=height);
	}
	__forceinline bool Set_Pixel_Depth(int x, int y, float d)
	{
		if(IsValidPixel(x, y))
		{
			float *fdir = Get_Frame_Vector(x, y);
			float ndir[3] = {fdir[0],fdir[1],fdir[2]};
			MATH_UTILS::SetLength(ndir, d);
			float *pos = Get_Pos(x, y);
			pos[0] = view_origin[0] + ndir[0];
			pos[1] = view_origin[1] + ndir[1];
			pos[2] = view_origin[2] + ndir[2];
			return true;
		}
		return false;
	}
	__forceinline float Get_Pixel_Depth(int x, int y)
	{
		if(IsValidPixel(x, y))
		{
			return MATH_UTILS::Distance3D(view_origin, Get_Pos(x, y));
		}
		return -1;
	}
	__forceinline bool Get_Pixel_RGB(int x, int y, float *res)
	{
		bool valid = true;
		if(x<0){x = 0;valid = false;}
		if(x>=width){x = width-1;valid = false;}
		if(y<0){y = 0;valid = false;}
		if(y>=height){y = height-1;valid = false;}
		float *rgb = Get_RGB(x, y);
		res[0] = rgb[0];
		res[1] = rgb[1];
		res[2] = rgb[2];
		return valid;
	}
	__forceinline bool Set_Pixel_RGB(int x, int y, float *p)
	{
		bool valid = true;
		if(x<0){x = 0;valid = false;}
		if(x>=width){x = width-1;valid = false;}
		if(y<0){y = 0;valid = false;}
		if(y>=height){y = height-1;valid = false;}
		float *rgb = Get_RGB(x, y);
		rgb[0] = p[0];
		rgb[1] = p[1];
		rgb[2] = p[2];
		return valid;
	}
	__forceinline bool Get_Pixel_XYZ(int x, int y, float *res)
	{
		bool valid = true;
		if(x<0){x = 0;valid = false;}
		if(x>=width){x = width-1;valid = false;}
		if(y<0){y = 0;valid = false;}
		if(y>=height){y = height-1;valid = false;}
		float *pos = Get_Pos(x, y);
		res[0] = pos[0];
		res[1] = pos[1];
		res[2] = pos[2];
		return valid;
	}
	__forceinline bool Set_Pixel_XYZ(int x, int y, float *p)
	{
		bool valid = true;
		if(x<0){x = 0;valid = false;}
		if(x>=width){x = width-1;valid = false;}
		if(y<0){y = 0;valid = false;}
		if(y>=height){y = height-1;valid = false;}
		float *pos = Get_Pos(x, y);
		pos[0] = p[0];
		pos[1] = p[1];
		pos[2] = p[2];
		return valid;
	}
	__forceinline bool Get_Original_Pixel_RGB(int x, int y, float *res)
	{
		bool valid = true;
		if(x<0){x = 0;valid = false;}
		if(x>=width){x = width-1;valid = false;}
		if(y<0){y = 0;valid = false;}
		if(y>=height){y = height-1;valid = false;}
		unsigned char *rgb = Get_Original_RGB(x, y);
		res[0] = ((float)rgb[0])/255;
		res[1] = ((float)rgb[1])/255;
		res[2] = ((float)rgb[2])/255;
		return valid;
	}
	__forceinline float Get_Clamped_Pixel_Depth(int x, int y)
	{
		if(x<0)x = 0;
		if(x>=width)x = width-1;
		if(y<0)y = 0;
		if(y>=height)y = height-1;
		return MATH_UTILS::Distance3D(view_origin, Get_Pos(x, y));
	}
	__forceinline bool IndexToPixel(int k, int *x, int *y)
	{
		if(k<0||k>=width*height)return false;
		float num_rows = ((float)k)/width;
		*y = (int)num_rows;
		*x = k-((*y)*width);
		return true;
	}
	__forceinline bool Pixel_Is_Selected(int x, int y)
	{
		if(IsValidPixel(x, y))
		{
			return selection_buffer[(y*width)+x];
		}
		return false;
	}
	__forceinline void Set_Pixel_Selected(int x, int y)
	{
		selection_buffer[(y*width)+x] = true;
	}
	bool Invalidate_Selection_Buffer()
	{
		selection_buffer_is_current = false;
		return false;
	}
	bool Validate_Selection_Buffer()
	{
		if(selection_buffer_is_current)return true;
		memset(selection_buffer, 0, sizeof(bool)*width*height);
		Fill_Selection_Buffer();
		selection_buffer_is_current = true;
		return false;
	}
	int version;
	float *vertices;
	float *identity_vectors;
	float *frame_vectors;
	float *rgb_colors;
	bool *selection_buffer;
	bool selection_buffer_is_current;
	float *contrast_map;
	unsigned char *edge_image;
	unsigned char *original_rgb;
	int width, height;
	float min_pixel_depth, max_pixel_depth;
	float min_world_depth, max_world_depth;
	float world_width;
	float world_depth;
	float world_height;
	char image_file[256];
	char depth_file[256];
	char layers_file[256];
	float anaglyph_bump;
	char filename[512];
	FRAME_PIXEL *pixels;
	float view_origin[3];
	float view_rotation[3];
	float view_direction[3];
};

extern FRAME *frame;

struct FRAME_INFO_HEADER
{
	int version;
	int width;
	int height;
	char image_file[256];
	float camera_pos[3];
	float camera_rot[3];
	float eye_separation;
	float focal_length;
	float aperture;
	float depth_scale;
	float exp_depth_scale;//unused, could be replaced with something else
	int num_layers;
	int preview_skip;
	int speckle_skip;
	bool anaglyph_mode;
	bool render_borders;
	bool render_normals;
	bool view_edges;
	bool edge_detection;
	int edge_detection_range;
	float edge_detection_threshold;
	bool cull_backfaces;
	float extra_data[24];
};



bool Render_3D_View();
bool Render_Selection_Outline();
bool Free_Frames();
void Get_Flat_Projection_Vectors();
void Get_Dome_Projection_Vectors();
void Get_Cylinder_Projection_Vectors();

bool Save_Stereo();

void Set_Eye_Separation(float v);
void Set_Focal_Length(float v);
void Set_Aperture(float v);
void Set_Depth_Scale(float v);
void Set_Depth_Scale_Offset(float v);

bool Load_Frame_Image(char *file, bool default_layer);
bool TestFind_Frame_Image(char *file);

bool Replace_Frame_Image(char *file);
bool Load_Depth_Image(char *file);
bool Load_Layer_Image(char *file);

bool Save_Stereo_Image(char *file);
bool Save_Left_Image(char *file);
bool Save_Right_Image(char *file);
bool Save_Anaglyph_Image(char *file);
bool Save_Stereogram_Image(char *file);

void Reset_Depth_Scale();
void Reset_Depth_Scale_Offset();

float Frame_World_Width();
float Frame_World_Depth();
float Frame_World_Height();

bool Get_Frame_Pixel_Depth(int x, int y, float *res);
bool Set_Frame_Pixel_Depth(int x, int y, float d);

bool Get_Frame_Pixel_RGB(int x, int y, float *rgb);
bool Set_Frame_Pixel_RGB(int x, int y, float *rgb);

bool Get_Frame_Pixel_Layer(int x, int y, int *layer);

void Scale_Frame_Pixels(float amount, int num, unsigned int *indices);

int Frame_Width();
int Frame_Height();

bool Update_Frame_Depth_Map();

bool Save_Frame_Info(char *file, bool save_camera_views);
bool Load_Frame_Info(char *file);
bool Load_Stereo_Info(char *file);

bool Import_Layer_Edits(char *file);

extern bool redraw_frame;
extern bool session_altered;
extern int edge_detection_range;
extern float edge_detection_threshold;
extern bool edge_detection_enabled;

extern float DEFAULT_FOV;
#define RAD 0.017453f

extern int projection_type;

bool Load_Layers_Only(char *file);
bool Load_Temp_Layers_Only(char *file);
bool Load_First_Project_Frame_Image(char *file);
bool Free_Frame();

bool Get_Anaglyph_Frame(unsigned char *rgb);
bool Get_Left_Frame(unsigned char *rgb);
bool Get_Right_Frame(unsigned char *rgb);
bool Get_Stereo_Frame(unsigned char *rgb);

#endif





