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
#include "Layers.h"
#include "../Skin.h"

void Project_All_Layers(float depth_id);
extern int NEW_FRAME_LAYER_DEPTH;

void Reset_Default_Layer_Depth();

vector<unsigned char> layer_colors;
int *masked_layer_ids = 0;

int layer_mask_width, layer_mask_height;

float RF(float max);

bool flip_layermask = false;

bool Get_Enumerated_Color(int layer_id, float *rgb)
{
	srand(layer_id*47);
	rgb[0] = RF(1);
	rgb[1] = RF(1);
	rgb[2] = RF(1);
	return true;
}

bool Get_Enumerated_Color(int layer_id, unsigned char *rgb)
{
	srand(layer_id*47);
	rgb[0] = rand()%256;
	rgb[1] = rand()%256;
	rgb[2] = rand()%256;
	return true;
}

bool Clear_Layer_Colors()
{
	layer_colors.clear();
	return true;
}

__forceinline int Find_Layer_Color(unsigned char *rgb)
{
	int n = layer_colors.size();
	for(int i = 0;i<n;i+=3)
	{
		if(layer_colors[i]==rgb[0]&&layer_colors[i+1]==rgb[1]&&layer_colors[i+2]==rgb[2])
		{
			return i/3;
		}
	}
	return -1;
}

__forceinline int Add_Layer_Color(unsigned char *rgb)
{
	layer_colors.push_back(rgb[0]);
	layer_colors.push_back(rgb[1]);
	layer_colors.push_back(rgb[2]);
	return (layer_colors.size()/3)-1;
}

__forceinline int Layer_Color(unsigned char *rgb)
{
	int res = Find_Layer_Color(rgb);
	if(res!=-1)
	{
		return res;
	}
	return Add_Layer_Color(rgb);
}

__forceinline int Preset_Layer_Color(unsigned char *rgb)
{
	return Find_Layer_Color(rgb);
}

__forceinline bool Colors_Match(unsigned char *rgb1, unsigned char *rgb2)
{
	return rgb1[0]==rgb2[0]&&rgb1[1]==rgb2[1]&&rgb1[2]==rgb2[2];
}

__forceinline unsigned char* Get_Color(int x, int y, int *id, unsigned char *layers)
{
	if(x<0||x>=layer_mask_width||y<0||y>=layer_mask_height)
	{
		return 0;
	}
	*id = masked_layer_ids[(y*layer_mask_width)+x];
	if(*id==-1)
	{
		return 0;
	}
	return &layers[((y*layer_mask_width)+x)*3];
}

void Add_Verbatim_Masked_Layer_Colors(unsigned char *layers, int width, int height)
{
	Print_Status("Identifying all colors in image..");
	layer_mask_width = width;
	layer_mask_height = height;
	int t = width*height;
	for(int i = 0;i<t;i++)
	{
		Layer_Color(&layers[i*3]);
	}
	Print_Status("Done.");
}

void Get_Verbatim_Preset_Masked_Layers(unsigned char *layers, int width, int height)
{
	Print_Status("Assigning layers to all pixels..");
	layer_mask_width = width;
	layer_mask_height = height;
	int t = width*height;
	for(int i = 0;i<t;i++)
	{
		masked_layer_ids[i] = Preset_Layer_Color(&layers[i*3]);
	}
	Print_Status("Done.");
}

void Assign_Verbatim_Masked_Layers(unsigned char *layers, int width, int height)
{
	Print_Status("Assigning layers to all pixels..");
	layer_mask_width = width;
	layer_mask_height = height;
	int t = width*height;
	for(int i = 0;i<t;i++)
	{
		masked_layer_ids[i] = Layer_Color(&layers[i*3]);
	}
	Print_Status("Done.");
}


bool Load_Layer_Mask(unsigned char *layers, int w, int h)
{
	if(w!=frame->width||h!=frame->height)
	{
		return false;
	}
	masked_layer_ids = new int[w*h];
	Assign_Verbatim_Masked_Layers(layers, w, h);
	int i, j, k;
	int startx, layer, rgb_layer;
	int layer_id;
	Free_Layers();
	int num_layers = (layer_colors.size()/3)-1;
	unsigned char layer_color[3];
	for(i = 0;i<num_layers+1;i++)
	{
		layer_color[0] = layer_colors[i*3];
		layer_color[1] = layer_colors[(i*3)+1];
		layer_color[2] = layer_colors[(i*3)+2];
		New_Layer(&layer_id, 0, layer_color);
	}

	for(j = 0;j<frame->height;j++)
	{
		layer = -1;
		rgb_layer = -1;
		startx = -1;
		for(i = 0;i<frame->width;i++)
		{
			k = (j*frame->width)+i;
			rgb_layer = masked_layer_ids[k];
			if(rgb_layer!=layer)
			{
				if(layer!=-1)
				{
					//flip these bitches
					if(flip_layermask)
					{
						Force_Layer_RLE_Strip(layer, (frame->height-1)-j, startx, i);
					}
					else
					{
						Force_Layer_RLE_Strip(layer, j, startx, i);
					}
				}
				//start new
				layer = rgb_layer;
				startx = i;
			}
		}
		//flip these bitches
		if(flip_layermask)
		{
			Force_Layer_RLE_Strip(layer, (frame->height-1)-j, startx, frame->width);
		}
		else
		{
			Force_Layer_RLE_Strip(layer, j, startx, frame->width);
		}
	}
	Clear_Layer_Colors();
	delete[] masked_layer_ids;
	Project_All_Layers(NEW_FRAME_LAYER_DEPTH);
	Reset_Default_Layer_Depth();
	Update_All_Layer_Data();
	Update_Initial_Layer_Relief();
	return true;
}



bool Load_Layer_Mask_Image(char *file)
{
	strcpy(frame->layers_file, file);
	int w, h;
	unsigned char *layers = Get_RGB_Image_Data(file, &w, &h);
	if(!layers)
	{
		SkinMsgBox(0, "Error loading layermask image!", file, MB_OK);
		return false;
	}
	if(w!=frame->width||h!=frame->height)
	{
		char msg[512];
		sprintf(msg, "Error loading layer image! Layer image must be same size as frame image.\nFrame dimensions:%i %i\n, Layer image dimensions:%i %i", frame->width, frame->height, w, h);
		SkinMsgBox(0, msg, 0, MB_OK);
		delete[] layers;
		return false;
	}
	masked_layer_ids = new int[w*h];
	Assign_Verbatim_Masked_Layers(layers, w, h);
	int i, j, k;
	int startx, layer, rgb_layer;
	int layer_id;
	Free_Layers();
	int num_layers = (layer_colors.size()/3)-1;
	unsigned char layer_color[3];
	for(i = 0;i<num_layers+1;i++)
	{
		layer_color[0] = layer_colors[i*3];
		layer_color[1] = layer_colors[(i*3)+1];
		layer_color[2] = layer_colors[(i*3)+2];
		New_Layer(&layer_id, 0, layer_color);
	}

	for(j = 0;j<frame->height;j++)
	{
		layer = -1;
		rgb_layer = -1;
		startx = -1;
		for(i = 0;i<frame->width;i++)
		{
			k = (j*frame->width)+i;
			rgb_layer = masked_layer_ids[k];
			if(rgb_layer!=layer)
			{
				if(layer!=-1)
				{
					//flip these bitches
					if(flip_layermask)
					{
						Force_Layer_RLE_Strip(layer, (frame->height-1)-j, startx, i);
					}
					else
					{
						Force_Layer_RLE_Strip(layer, j, startx, i);
					}
				}
				//start new
				layer = rgb_layer;
				startx = i;
			}
		}
		//flip these bitches
		if(flip_layermask)
		{
			Force_Layer_RLE_Strip(layer, (frame->height-1)-j, startx, frame->width);
		}
		else
		{
			Force_Layer_RLE_Strip(layer, j, startx, frame->width);
		}
	}
	Clear_Layer_Colors();
	delete[] masked_layer_ids;
	delete[] layers;
	Project_All_Layers(NEW_FRAME_LAYER_DEPTH);
	Reset_Default_Layer_Depth();
	Update_All_Layer_Data();
	Update_Initial_Layer_Relief();
	return true;
}





bool Add_Layer_Colors_From_Mask_Image(char *file)
{
	int w, h;//, b;
	unsigned char *layers = Get_RGB_Image_Data(file, &w, &h);//, &b);
	if(!layers)
	{
		SkinMsgBox(0, "Error loading layer image!", file, MB_OK);
		return false;
	}
	Add_Verbatim_Masked_Layer_Colors(layers, w, h);
	delete[] layers;
	delete[] masked_layer_ids;
	return true;
}



bool Load_Preset_Layer_Mask_Image(char *file)
{
	char *c = strrchr(file, '\\');
	if(c)
	{
		Print_Status("Generating layers from %s", &c[1]);
	}
	strcpy(frame->layers_file, file);
	int w, h;
	unsigned char *layers = Get_RGB_Image_Data(file, &w, &h);//, &b);
	if(w!=frame->width||h!=frame->height)
	{
		SkinMsgBox(0, "Error loading layer image! Layer image must be same size as frame image", 0, MB_OK);
		delete[] layers;
		return false;
	}
	masked_layer_ids = new int[w*h];
	Get_Verbatim_Preset_Masked_Layers(layers, w, h);
	int i, j, k;
	int startx, layer, rgb_layer;
	Free_Layers();
	int num_layers = (layer_colors.size()/3)-1;
	unsigned char layer_color[3];
	for(i = 0;i<num_layers+1;i++)
	{
		layer_color[0] = layer_colors[i*3];
		layer_color[1] = layer_colors[(i*3)+1];
		layer_color[2] = layer_colors[(i*3)+2];
		New_Layer_Force_ID(i+1, 0, layer_color);
	}

	for(j = 0;j<frame->height;j++)
	{
		layer = -1;
		rgb_layer = -1;
		startx = -1;
		for(i = 0;i<frame->width;i++)
		{
			k = (j*frame->width)+i;
			rgb_layer = masked_layer_ids[k];
			if(rgb_layer!=layer)
			{
				if(layer!=-1)
				{
					//flip these bitches
					if(flip_layermask)
					{
						Force_Layer_RLE_Strip(layer, (frame->height-1)-j, startx, i);
					}
					else
					{
						Force_Layer_RLE_Strip(layer, j, startx, i);
					}
				}
				layer = rgb_layer;
				startx = i;
			}
		}
		//flip these bitches
		if(flip_layermask)
		{
			Force_Layer_RLE_Strip(layer, (frame->height-1)-j, startx, frame->width);
		}
		else
		{
			Force_Layer_RLE_Strip(layer, j, startx, frame->width);
		}
	}
	delete[] masked_layer_ids;
	delete[] layers;
	Project_All_Layers(NEW_FRAME_LAYER_DEPTH);
	Reset_Default_Layer_Depth();
	Update_All_Layer_Data();
	Update_Initial_Layer_Relief();
	Print_Status("Done.");
	return true;
}

#include <process.h>

bool Get_File_From_Full_Path(char *path, char *file);
bool Remove_File_From_Full_Path(char *path);
bool Exists(char *file);


bool Get_New_Path(char *file, char *nfile)
{
	char sfile[512];
	char path[512];
	strcpy(path, file);
	Get_File_From_Full_Path(file, sfile);
	Remove_File_From_Full_Path(path);
	sprintf(nfile, "%s\\FILTERED\\%s", path, sfile);
	return true;
}

bool Get_ImageFilter_AppPath(char *res)
{
	GetModuleFileName(GetModuleHandle(NULL), res, 512);
	char *c = strrchr(res, '\\');
	if(!c)
	{
		strcpy(res, "ImageFilter.exe");
		return false;
	}
	strcpy(c, "\\ImageFilter.exe");
	return true;
}

bool PreFilter_Image(char *file)
{
	char app[512];
	Get_ImageFilter_AppPath(app);
	if(!Exists(app))
	{
		SkinMsgBox(0, "ImageFilter.exe not found! There may be an installation error. Using backup filtering mechanism, this will take longer.", app, MB_OK);
		return false;
	}
	_spawnl(P_WAIT, app, "-f", file, 0);
	char nfile[512];
	Get_New_Path(file, nfile);
	if(Exists(nfile))
	{
		strcpy(file, nfile);
	}
	return true;
}

bool PreFilter_Path(char *path)
{
	char app[512];
	Get_ImageFilter_AppPath(app);
	if(!Exists(app))
	{
		SkinMsgBox(0, "ImageFilter.exe not found! There may be an installation error. Using backup filtering mechanism, this will take longer.", app, MB_OK);
		return false;
	}
	_spawnl(P_WAIT, app, "-f", path, 0);
	strcat(path, "\\FILTERED");
	return true;
}

