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
#include "Relief.h"
#include "Image.h"
#include "Frame.h"
#include "Console.h"
#include "Layers.h"
#include "PlanarProjection.h"
#include <GL\gl.h>
#include <vector>
#include "../Skin.h"

bool flip_relief_depth = false;
bool flip_relief_vertical = false;
bool flip_relief_horizontal = false;

void Set_Layer_Relief_Info(int index, RELIEF_INFO *ri, bool update_values);
void Get_Layer_Relief_Info(int index, RELIEF_INFO *ri);

void Force_Layer_RLE_Strip(int index, int y, int start_x, int end_x);

using namespace std;

float Pixel_Size();

float RF(float max);

extern float depth_increment;

bool render_2d_relief_layer_split = false;

int Layer_ID(int index);

class RELIEF_IMAGE
{
public:
	RELIEF_IMAGE()
	{
		width = 0;
		height = 0;
		data = 0;
	}
	~RELIEF_IMAGE()
	{
		if(data)
		{
			delete[] data;
		}
	}
	__forceinline float Pixel_Value(int x, int y)
	{
		return data[(y*width)+x];
	}
	float Get_Interpolated_Pixel(float px, float py)
	{
		if(!flip_relief_vertical)py = height-py;
		if(flip_relief_horizontal)px = width-px;
		while(px>=width)px-=width;
		while(py>=height)py-=height;
		while(px<0)px+=width;
		while(py<0)py+=height;
		int lx = (int)px;
		int ly = (int)py;
		int hx = lx+1;
		int hy = ly+1;
		if(hx>=width)hx = width-1;
		if(hy>=height)hy = height-1;
		float dx = px-lx;
		float dy = py-ly;
		float ipx = 1.0f-dx;
		float ipy = 1.0f-dy;
		float top = (Pixel_Value(lx, ly)*ipx)+(Pixel_Value(hx, ly)*dx);
		float bottom = (Pixel_Value(lx, hy)*ipx)+(Pixel_Value(hx, hy)*dx);
		float center = (top*ipy)+(bottom*dy);
		if(flip_relief_depth)
		{
			return 1.0f-center;
		}
		return center;
	}
	bool Load_Data()
	{
		unsigned char *gsdata = Get_Grayscale_Image_Data(file, &width, &height, &bits);
		if(!gsdata)
		{
			char msg[512];
			sprintf(msg, "Error loading image %s", file);
			Print_Status(msg);
			Print_To_Console(msg);
			SkinMsgBox(0, "Error loading image file!", file, MB_OK);
			return false;
		}
		int n = width*height;
		data = new float[n];
		for(int i = 0;i<n;i++)
		{
			data[i] = (float)gsdata[i]/255.0;
		}
		delete[] gsdata;
		return true;
	}
	char file[256];
	float *data;
	int width, height, bits;
};

vector<RELIEF_IMAGE*> relief_images;

RELIEF_IMAGE *noise_relief_image = 0;

void Init_Noise_Texture()
{
	RELIEF_IMAGE *ri = new RELIEF_IMAGE;
	int s = 64;
	int total = s*s;
	ri->width = ri->height = s;
	ri->data = new float[total];
	for(int i = 0;i<total;i++)
	{
		ri->data[i] = (RF(2)-1);
	}
	strcpy(ri->file, "NOISE TEXTURE");
	noise_relief_image = ri;
}

void Free_Noise_Texture()
{
	if(noise_relief_image)delete noise_relief_image;
	noise_relief_image = 0;
}


void Free_Relief_Images()
{
	int n = relief_images.size();
	for(int i = 0;i<n;i++)
	{
		delete relief_images[i];
	}
	relief_images.clear();
}

void Save_Relief_Images(FILE *f)
{
	int n = relief_images.size();
	fwrite(&n, sizeof(int), 1, f);
	for(int i = 0;i<n;i++)
	{
		fwrite(relief_images[i]->file, sizeof(char), 256, f);
	}
}

__forceinline RELIEF_IMAGE* Find_Relief_Image(char *file)
{
	int n = relief_images.size();
	for(int i = 0;i<n;i++)
	{
		if(!strcmp(relief_images[i]->file, file))
		{
			return relief_images[i];
		}
	}
	return 0;
}

void Load_Relief_Images(FILE *f)
{
	int n = 0;
	fread(&n, sizeof(int), 1, f);
	char file[256];
	for(int i = 0;i<n;i++)
	{
		fread(file, sizeof(char), 256, f);
		if(Find_Relief_Image(file)==0)
		{
			RELIEF_IMAGE *ri = new RELIEF_IMAGE;
			strcpy(ri->file, file);
			if(!ri->Load_Data())
			{
				SkinMsgBox(0, "Error loading relief image!", ri->file, MB_OK);
			}
			else
			{
				relief_images.push_back(ri);
			}
		}
	}
}

void Skip_Relief_Images(FILE *f)
{
	int n = 0;
	fread(&n, sizeof(int), 1, f);
	char file[256];
	for(int i = 0;i<n;i++)
	{
		fread(file, sizeof(char), 256, f);
	}
}


__forceinline RELIEF_IMAGE* Load_Relief_Image(char *file)
{
	RELIEF_IMAGE *ri = new RELIEF_IMAGE;
	strcpy(ri->file, file);
	if(!ri->Load_Data())
	{
		SkinMsgBox(0, "Error loading relief image!", ri->file, MB_OK);
		delete ri;
	}
	relief_images.push_back(ri);
	return ri;
}

bool Load_Single_Relief_Image(char *file)
{
	RELIEF_IMAGE *ri = Load_Relief_Image(file);
	return ri!=0;
}


__forceinline RELIEF_IMAGE* Get_Relief_Image(RELIEF_INFO *ri)
{
	RELIEF_IMAGE *pri = Find_Relief_Image(ri->image);
	if(pri)return pri;
	return Load_Relief_Image(ri->image);
}

__forceinline float myfabs(float v)
{
	if(v<0)return -v;
	return v;
}

__forceinline void Normalize_Hue(float *h)
{
	if(h[0]>=h[1]&&h[0]>=h[2])
	{
		h[1] = h[1]/h[0];
		h[2] = h[2]/h[0];
		h[0] = 1;
	}
	else if(h[1]>=h[0]&&h[1]>=h[2])
	{
		h[0] = h[1]/h[1];
		h[2] = h[2]/h[1];
		h[1] = 1;
	}
	else
	{
		h[0] = h[1]/h[2];
		h[1] = h[2]/h[2];
		h[2] = 1;
	}
}

__forceinline float Hue_Diff(float *h1, float *h2)
{
	//normalize the hues first
	float nh1[3] = {h1[0], h1[1], h1[2]};
	float nh2[3] = {h2[0], h2[1], h2[2]};
	Normalize_Hue(nh1);
	Normalize_Hue(nh2);
	return myfabs(nh1[0]-nh2[0])+myfabs(nh1[1]-nh2[1])+myfabs(nh1[2]-nh2[2]);
}

__forceinline void Get_Hue(float *rgb, float *res)
{
	float t = rgb[0]+rgb[1]+rgb[2];
	if(t)
	{
		res[0] = rgb[0]/t;
		res[1] = rgb[1]/t;
		res[2] = rgb[2]/t;
	}
	else
	{
		res[0] = res[1] = res[2] = 0;
	}
}

__forceinline void Get_Hue(unsigned char *rgb, float *res)
{
	float v[3] = {(float)rgb[0]/255,(float)rgb[1]/255,(float)rgb[2]/255};
	Get_Hue(v, res);
}

__forceinline float Get_Color_Difference_Relief(RELIEF_INFO *ri, float *concave_hue, float *convex_hue, float *hue)
{
	float d1 = Hue_Diff(hue, concave_hue)/3;
	float d2 = Hue_Diff(hue, convex_hue)/3;
	d1 *= 1.0f-ri->bias;
	d2 *= ri->bias;
	float total = d1+d2;
	float p = d1/total;
	if(ri->smooth)
	{
		return ((p-0.5f)*2)*-depth_increment;
	}
	if(p>0.5f)
	{
		return -depth_increment;
	}
	else
	{
		return depth_increment;
	}
	return 0;
}

__forceinline float Get_Brightness_Difference_Relief(RELIEF_INFO *ri, float concave_brightness, float convex_brightness, float brightness)
{
	float d1 = myfabs(concave_brightness-brightness);
	float d2 = myfabs(convex_brightness-brightness);
	d1 *= 1.0f-ri->bias;
	d2 *= ri->bias;
	float total = d1+d2;
	float p = d1/total;
	if(ri->smooth)
	{
		return ((p-0.5f)*2)*depth_increment;
	}
	if(p>0.5f)
	{
		return depth_increment;
	}
	else
	{
		return -depth_increment;
	}
	return 0;
}

void Update_Random_Noise_Relief(RELIEF_INFO *ri, unsigned int *indices, int num_indices)
{
	int i, k;
	RELIEF_IMAGE *pri = noise_relief_image;
	for(i = 0;i<num_indices;i++)
	{
		k = indices[i];
		float p = pri->Get_Interpolated_Pixel(ri->contrast*frame->pixels[k].x, ri->contrast*frame->pixels[k].y);
		p *= depth_increment;
		frame->pixels[k].relief = p;
	}
}

void Update_Image_Relief(RELIEF_INFO *ri, unsigned int *indices, int num_indices)
{
	int i, k;
	if(!strcmp(ri->image, "NO IMAGE"))
	{
		for(i = 0;i<num_indices;i++)
		{
			k = indices[i];
			frame->pixels[k].relief = 0;
		}
		return;
	}
	RELIEF_IMAGE *pri = Get_Relief_Image(ri);
	if(!pri)
	{
		for(i = 0;i<num_indices;i++)
		{
			k = indices[i];
			frame->pixels[k].relief = 0;
		}
	}
	else
	{
		for(i = 0;i<num_indices;i++)
		{
			k = indices[i];
			float p = pri->Get_Interpolated_Pixel(ri->tiling*frame->pixels[k].x, ri->tiling*frame->pixels[k].y);
			p = (p*2)-1;
			p *= depth_increment;
			frame->pixels[k].relief = -p;
		}
	}
}

void Update_Color_Difference_Relief(RELIEF_INFO *ri, unsigned int *indices, int num_indices)
{
	int k;
	float concave_hue[3];
	float convex_hue[3];
	float hue[3];
	Get_Hue(ri->concave_color, concave_hue);
	Get_Hue(ri->convex_color, convex_hue);
	for(int i = 0;i<num_indices;i++)
	{
		k = indices[i];
		Get_Hue(frame->Get_RGB(k), hue);
		frame->pixels[k].relief = Get_Color_Difference_Relief(ri, concave_hue, convex_hue, hue);
	}
}

void Update_Brightness_Relief(RELIEF_INFO *ri, unsigned int *indices, int num_indices)
{
	int k;
	float concave = 3;
	float convex = 0;
	float bright = 0;

	for(int i = 0;i<num_indices;i++)
	{
		k = indices[i];
		float *rgb = frame->Get_RGB(k);
		bright = (rgb[0]+rgb[1]+rgb[2]);
		frame->pixels[k].relief = Get_Brightness_Difference_Relief(ri, concave, convex, bright);
	}
}



void Update_Relief(RELIEF_INFO *ri, unsigned int *indices, int num_indices)
{
	switch(ri->type)
	{
	case RELIEF_TYPE_RANDOM_NOISE:Update_Random_Noise_Relief(ri, indices, num_indices);return;
	case RELIEF_TYPE_IMAGE:Update_Image_Relief(ri, indices, num_indices);return;
	case RELIEF_TYPE_COLOR_DIFFERENCE:Update_Color_Difference_Relief(ri, indices, num_indices);return;
	case RELIEF_TYPE_BRIGHTNESS:Update_Brightness_Relief(ri, indices, num_indices);return;
	};
}

/////////////////////////////////////////////////////////////////

//rotoscoping layer split

//FIXTHIS move to dedicated src file

vector<int> relief_layer_split_pixels;

void Clear_Layer_Split_Pixels()
{
	relief_layer_split_pixels.clear();
}

void Update_Layer_Split_Pixels()
{
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		int ns = Num_RLE_Strips_In_Layer(i);
		for(int j = 0;j<ns;j++)
		{
			if(Layer_Is_Selected(i))
			{
				RLE_STRIP *rs = Get_RLE_Strip(i, j);
				int layer = Layer_ID(i);
				for(int k = rs->start_x;k<rs->end_x-1;k++)
				{
					//using the "relief value" as a measure of color difference
					bool current = frame->pixels[(rs->y*frame->width)+k].relief>0;
					bool next = frame->pixels[(rs->y*frame->width)+k+1].relief>0;
					bool lower = current;
					if(rs->y!=frame->height-1)
					{
						if(frame->Get_Pixel_Layer(k, rs->y)==layer)
						{
							lower = frame->pixels[((rs->y+1)*frame->width)+k].relief>0;
						}
					}
					bool hit = false;
					if(current!=next)
					{
						relief_layer_split_pixels.push_back(k);
						relief_layer_split_pixels.push_back(rs->y);
						hit = true;
					}
					if(current!=lower)
					{
						if(!hit)
						{
							relief_layer_split_pixels.push_back(k);
							relief_layer_split_pixels.push_back(rs->y);
						}
					}
				}
			}
		}
	}
}

void Update_Relief_Layer_Split_Pixels()
{
	Clear_Layer_Split_Pixels();
	Update_Layer_Split_Pixels();
}

void Init_Relief_Layer_Split_Pixels()
{
	Update_Relief_Layer_Split_Pixels();
}

void Render_2D_Relief_Layer_Split()
{
	glPointSize(Pixel_Size());
	glColor3f(1, 0, 0);
	glBegin(GL_POINTS);
	int n = relief_layer_split_pixels.size();
	float x, y;
	for(int i = 0;i<n;i+=2)
	{
		x = relief_layer_split_pixels[i] + 0.5f;
		y = relief_layer_split_pixels[i+1] + 0.5f;
		glVertex2f(x, y);
	}
	glEnd();
	glPointSize(1);
	glColor3f(1,1,1);
}

vector<RLE_STRIP> layer1_rle_strips;
vector<RLE_STRIP> layer2_rle_strips;

void Clear_Temp_RLE_Strips()
{
	layer1_rle_strips.clear();
	layer2_rle_strips.clear();
}

int start_layer1_rle_strip_x = 0;
int start_layer2_rle_strip_x = 0;

void Start_Layer1_RLE_Strip(int x, int y)
{
	start_layer1_rle_strip_x = x;
}

void Finish_Layer1_RLE_Strip(int x, int y)
{
	RLE_STRIP rs;
	rs.y = y;
	rs.start_x = start_layer1_rle_strip_x;
	rs.end_x = x;
	layer1_rle_strips.push_back(rs);
}

void Start_Layer2_RLE_Strip(int x, int y)
{
	start_layer2_rle_strip_x = x;
}

void Finish_Layer2_RLE_Strip(int x, int y)
{
	RLE_STRIP rs;
	rs.y = y;
	rs.start_x = start_layer2_rle_strip_x;
	rs.end_x = x;
	layer2_rle_strips.push_back(rs);
}

void Get_Temp_RLE_Strips()
{
	int n = Num_Layers();
	bool b = false;
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			int ns = Num_RLE_Strips_In_Layer(i);
			for(int j = 0;j<ns;j++)
			{
				//break the strip down into pieces
				RLE_STRIP *rs = Get_RLE_Strip(i, j);
				RLE_STRIP s;
				s.start_x = rs->start_x;
				s.end_x = rs->end_x;
				s.y = rs->y;
				bool layer1_open = false;//flags for which target layer a pixel goes to
				bool layer2_open = false;
				for(int k = rs->start_x;k<rs->end_x;k++)
				{
					bool current = frame->pixels[(rs->y*frame->width)+k].relief>0;
					bool next = !current;
					if(k!=rs->end_x-1)//if not at the last pixel in the strip
					{
						next = frame->pixels[(rs->y*frame->width)+k+1].relief>0;
					}
					if(k==rs->start_x)//first pixel in strip
					{
						if(current)//start flag is true
						{
							Start_Layer1_RLE_Strip(k, rs->y);
							layer1_open = true;
						}
						else//start flag is false
						{
							Start_Layer2_RLE_Strip(k, rs->y);
							layer2_open = true;
						}
					}
					if(current!=next)
					{
						if(current&&layer1_open)
						{
							if(k!=rs->end_x-1)
							{
								Finish_Layer1_RLE_Strip(k+1, rs->y);
								layer1_open = false;
								Start_Layer2_RLE_Strip(k+1, rs->y);
								layer2_open = true;
							}
						}
						else if(!current&&layer2_open)
						{
							if(k!=rs->end_x-1)
							{
								Finish_Layer2_RLE_Strip(k+1, rs->y);
								layer2_open = false;
								Start_Layer1_RLE_Strip(k+1, rs->y);
								layer1_open = true;
							}
						}
					}
				}
				if(layer1_open)
				{
					Finish_Layer1_RLE_Strip(rs->end_x, rs->y);
				}
				if(layer2_open)
				{
					Finish_Layer2_RLE_Strip(rs->end_x, rs->y);
				}
			}
		}
	}
}


void Create_Split_Layers()
{
	int n1 = layer1_rle_strips.size();
	int n2 = layer2_rle_strips.size();
	int layer2_id = -1;
	if(n1==0||n2==0)
	{
		SkinMsgBox(0, "All pixels are on the same side, no split possible.\nAdjust the bias control to separate the pixels.");
		return;
	}
	int nl = Num_Layers();
	int keeper = -1;
	int i;
	for(int i = 0;i<nl;i++)
	{
		if(Layer_Is_Selected(i))
		{
			if(keeper == -1)
			{
				keeper = i;
			}
			Clear_Layer(i);
		}
	}
	int layer1_id = Get_Layer_ID(keeper);
	New_Layer(&layer2_id, 0, 0);
	int layer1_index = keeper;
	int layer2_index = Get_Layer_Index(layer2_id);
	RLE_STRIP *rs;
	for(i = 0;i<n1;i++)
	{
		rs = &layer1_rle_strips[i];
		Force_Layer_RLE_Strip(layer1_index, rs->y, rs->start_x, rs->end_x);
	}
	for(i = 0;i<n2;i++)
	{
		rs = &layer2_rle_strips[i];
		Force_Layer_RLE_Strip(layer2_index, rs->y, rs->start_x, rs->end_x);
	}
	float pos[3];
	float dir[3] = {0, 0, 1};
	float rotation[3] = {0,0,0};
	float offset[3] = {0,0,0};
	Update_Layer_Data(layer1_id);
	ReCalc_Layer_Center(layer1_id, pos);
	if(pos[2]<0)
	{
		pos[2] = -pos[2];
	}
	Project_Layer_To_Plane(layer1_id, pos, dir, rotation, offset, PLANE_ORIGIN_SELECTION_CENTER);
	Freeze_Layer(Find_Layer_Index(layer1_id), true);

	Update_Layer_Data(layer2_id);
	ReCalc_Layer_Center(layer2_id, pos);
	if(pos[2]<0)
	{
		pos[2] = -pos[2];
	}
	Project_Layer_To_Plane(layer2_id, pos, dir, rotation, offset, PLANE_ORIGIN_SELECTION_CENTER);
	Freeze_Layer(Find_Layer_Index(layer2_id), true);

	RELIEF_INFO r;
	int index1 = Get_Layer_Index(layer1_id);
	int index2 = Get_Layer_Index(layer2_id);
	Get_Layer_Relief_Info(index1, &r);
	Set_Layer_Relief_Info(index1, &r, true);
	Get_Layer_Relief_Info(index2, &r);
	Set_Layer_Relief_Info(index2, &r, true);
	Update_Layer_List();
}

bool Split_Selection_By_Relief()
{
	Print_Status("Sorting pixels..");
	Get_Temp_RLE_Strips();
	Print_Status("Creating new layers..");
	Create_Split_Layers();
	Print_Status("Deleting temporary data..");
	Clear_Temp_RLE_Strips();
	Clear_Layer_Split_Pixels();
	Print_Status("Done.");
	return true;
}














