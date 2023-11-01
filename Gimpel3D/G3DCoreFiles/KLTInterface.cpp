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
#include "Image.h"
#include "KLTFiles\klt.h"
#include <vector>
#include <GL/gl.h>
#include "../Skin.h"

using namespace std;

bool tracking_selection = false;

unsigned char *klt_image1 = 0;
unsigned char *klt_image2 = 0;
unsigned char *klt_red_image1 = 0;
unsigned char *klt_green_image1 = 0;
unsigned char *klt_blue_image1 = 0;
unsigned char *klt_grayscale_image1 = 0;
unsigned char *klt_red_image2 = 0;
unsigned char *klt_green_image2 = 0;
unsigned char *klt_blue_image2 = 0;
unsigned char *klt_grayscale_image2 = 0;

int klt_image_width = 0;
int klt_image_height = 0;

int klt_border_size = 25;


bool track_single_channels = true;

bool Free_KLT_Single_Channels()
{
	if(klt_red_image1)delete[] klt_red_image1;
	if(klt_green_image1)delete[] klt_green_image1;
	if(klt_blue_image1)delete[] klt_blue_image1;
	if(klt_grayscale_image1)delete[] klt_grayscale_image1;
	if(klt_red_image2)delete[] klt_red_image2;
	if(klt_green_image2)delete[] klt_green_image2;
	if(klt_blue_image2)delete[] klt_blue_image2;
	if(klt_grayscale_image2)delete[] klt_grayscale_image2;
	klt_red_image1 = 0;
	klt_green_image1 = 0;
	klt_blue_image1 = 0;
	klt_grayscale_image1 = 0;
	klt_red_image2 = 0;
	klt_green_image2 = 0;
	klt_blue_image2 = 0;
	klt_grayscale_image2 = 0;
	return true;
}

bool Free_KLT_Images()
{
	if(klt_image1)delete[] klt_image1;
	if(klt_image2)delete[] klt_image2;
	klt_image1 = 0;
	klt_image2 = 0;
	klt_image_width = 0;
	klt_image_height = 0;
	return true;
}

//starts tracking, generally using the current frame
bool Reset_KLT_Tracking(char *first_image_file)
{
	int w, h, n;
	unsigned char *ndata = Get_RGB_Image_Data(first_image_file, &w, &h);
	if(!ndata)
	{
		SkinMsgBox(0, "Error initializing KLT tracker, primary image can't be loaded!", first_image_file, MB_OK);
		return false;
	}
	if(w!=klt_image_width||h!=klt_image_height)
	{
		//mismatched or first image loaded
		Free_KLT_Images();
		Free_KLT_Single_Channels();
		klt_image_width = w;
		klt_image_height = h;
		klt_image1 = ndata;
		n = (w+klt_border_size+klt_border_size)*(h+klt_border_size+klt_border_size);
		klt_red_image1 = new unsigned char[n];
		klt_green_image1 = new unsigned char[n];
		klt_blue_image1 = new unsigned char[n];
		klt_grayscale_image1 = new unsigned char[n];
	}
	else
	{
		//memory already exists, just reuse
		n = w*h;
		memcpy(klt_image1, ndata, n*3);
		delete[] ndata;
	}
	if(track_single_channels)
	{
		RGB_To_Bordered_Single_Channel(klt_border_size, klt_red_image1, klt_image1, w, h, 0);
		RGB_To_Bordered_Single_Channel(klt_border_size, klt_green_image1, klt_image1, w, h, 1);
		RGB_To_Bordered_Single_Channel(klt_border_size, klt_blue_image1, klt_image1, w, h, 2);
	}
	RGB_To_Bordered_Grayscale(klt_border_size, klt_grayscale_image1, klt_image1, w, h);
	tracking_selection = true;
	return true;
}

bool Stop_KLT_Tracking()
{
	tracking_selection = false;
	return false;
}

//called when the frame changes
bool Set_Current_Tracking_Frame(unsigned char *data, int w, int h)
{
	if(!tracking_selection)
	{
		return false;
	}
	if(w!=klt_image_width||h!=klt_image_height)
	{
		SkinMsgBox(0, "Error setting next frame for trackimg, dimensions don't match!", 0, MB_OK);
		return false;
	}
	int n = (w+klt_border_size+klt_border_size)*(h+klt_border_size+klt_border_size);
	if(!klt_image2)
	{
		klt_image2 = new unsigned char[n*3];
		klt_red_image2 = new unsigned char[n];
		klt_green_image2 = new unsigned char[n];
		klt_blue_image2 = new unsigned char[n];
		klt_grayscale_image2 = new unsigned char[n];
	}
	memcpy(klt_image2, data, w*h*3);
	if(track_single_channels)
	{
		RGB_To_Bordered_Single_Channel(klt_border_size, klt_red_image2, klt_image2, w, h, 0);
		RGB_To_Bordered_Single_Channel(klt_border_size, klt_green_image2, klt_image2, w, h, 1);
		RGB_To_Bordered_Single_Channel(klt_border_size, klt_blue_image2, klt_image2, w, h, 2);
	}
	RGB_To_Bordered_Grayscale(klt_border_size, klt_grayscale_image2, klt_image2, w, h);
	return true;
}

bool Set_KLT_Feature_List(KLT_FeatureList flist, vector<float> *pixels)
{
	int n = pixels->size()/2;
	for(int i = 0;i<n;i++)
	{
		KLT_Feature f = flist->feature[i];
		f->x = ((*pixels)[i*2])+klt_border_size;
		f->y = ((*pixels)[(i*2)+1])+klt_border_size;
		f->aff_x = -1;
		f->aff_y = -1;
		f->aff_Axx = 1;
		f->aff_Ayx = 0;
		f->aff_Axy = 0;
		f->aff_Ayy = 1;
		f->val = 0;
	}
	return true;
}

bool Track_KLT_Points(vector<float> *old_pixels, vector<float> *new_pixels)
{
	int n = old_pixels->size()/2;
	float *temp_pixels = new float[n*2];
	int *hits = new int[n];
	KLT_FeatureList flist = KLTCreateFeatureList(n);
	int i;
	for(i = 0;i<n;i++)
	{
		temp_pixels[i*2] = 0;
		temp_pixels[(i*2)+1] = 0;
		hits[i] = 0;
	}

	KLT_TrackingContext tracking_context = KLTCreateTrackingContext();

	int bordered_width = klt_image_width+klt_border_size+klt_border_size;
	int bordered_height = klt_image_height+klt_border_size+klt_border_size;

	if(track_single_channels)
	{
		Set_KLT_Feature_List(flist, old_pixels);
		KLTTrackFeatures(tracking_context, klt_red_image1, klt_red_image2, bordered_width, bordered_height, flist);
		for(i = 0;i<n;i++){KLT_Feature f = flist->feature[i];if(f->x>=0&&f->y>=0){hits[i]++;temp_pixels[i*2] += f->x;temp_pixels[(i*2)+1] += f->y;}}
		Set_KLT_Feature_List(flist, old_pixels);
		KLTTrackFeatures(tracking_context, klt_green_image1, klt_green_image2, bordered_width, bordered_height, flist);
		for(i = 0;i<n;i++){KLT_Feature f = flist->feature[i];if(f->x>=0&&f->y>=0){hits[i]++;temp_pixels[i*2] += f->x;temp_pixels[(i*2)+1] += f->y;}}
		Set_KLT_Feature_List(flist, old_pixels);
		KLTTrackFeatures(tracking_context, klt_blue_image1, klt_blue_image2, bordered_width, bordered_height, flist);
		for(i = 0;i<n;i++){KLT_Feature f = flist->feature[i];if(f->x>=0&&f->y>=0){hits[i]++;temp_pixels[i*2] += f->x;temp_pixels[(i*2)+1] += f->y;}}
	}
	Set_KLT_Feature_List(flist, old_pixels);
	KLTTrackFeatures(tracking_context, klt_grayscale_image1, klt_grayscale_image2, bordered_width, bordered_height, flist);
	for(i = 0;i<n;i++){KLT_Feature f = flist->feature[i];if(f->x>=0&&f->y>=0){hits[i]++;temp_pixels[i*2] += f->x;temp_pixels[(i*2)+1] += f->y;}}

	float px, py;
	for(i = 0;i<n;i++)
	{
		if(hits[i]>0)
		{
			px = ((temp_pixels[i*2]/hits[i])+0.5f)-klt_border_size;
			py = ((temp_pixels[(i*2)+1]/hits[i])+0.5f)-klt_border_size;
			if(px<0)px = 0;
			if(py<0)py = 0;
			if(px>=klt_image_width)px = (float)(klt_image_width-1);
			if(py>=klt_image_height)py = (float)(klt_image_height-1);
			new_pixels->push_back(px);
			new_pixels->push_back(py);
		}
		else
		{
			new_pixels->push_back(-1);
			new_pixels->push_back(-1);
		}
	}

	delete[] temp_pixels;
	delete[] hits;
	KLTFreeFeatureList(flist);
	KLTFreeTrackingContext(tracking_context);
	return true;
}

bool Free_KLT_Data()
{
	Free_KLT_Images();
	Free_KLT_Single_Channels();
	return false;
}





