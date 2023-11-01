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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <vector>
#include "../Skin.h"

using namespace std;
bool Get_File_From_Full_Path(char *path, char *file);
bool Exists(char *file);

int texture_target = GL_TEXTURE_RECTANGLE_ARB;

bool Display_ILError()
{
	MessageBox(0, iluErrorString(ilGetError()), "Image Library Error!", MB_OK);
	return true;
}

class ALTERNATE_IMAGE_PATH
{
public:
	ALTERNATE_IMAGE_PATH(){}
	~ALTERNATE_IMAGE_PATH(){}
	char path[512];
};

vector<ALTERNATE_IMAGE_PATH> alternate_image_paths;

bool Register_Alternate_Image_Path(char *apath);

bool Load_Alternate_Image_Paths()
{
	alternate_image_paths.clear();
	char pfile[512];
	Get_Project_Filename(pfile);
	char *c = strrchr(pfile, '.');
	if(!c)
	{
		return false;
	}
	strcpy(c, ".alt");
	FILE *f = fopen(pfile, "rb");
	if(!f)
	{
		return false;
	}
	char path[512];
	while(!feof(f))
	{
		path[0] = 0;
		fread(path, sizeof(unsigned char), 512, f);
		if(path[0]!=0)
		{
			ALTERNATE_IMAGE_PATH aip;
			alternate_image_paths.push_back(aip);
			strcpy(alternate_image_paths[alternate_image_paths.size()-1].path, path);
		}
	}
	fclose(f);
	return true;
}

bool Append_Alternate_Image_Path(char *path)
{
	char pfile[512];
	Get_Project_Filename(pfile);
	char *c = strrchr(pfile, '.');
	if(!c)
	{
		return false;
	}
	strcpy(c, ".alt");
	FILE *f = fopen(pfile, "ab");
	if(!f)
	{
		return false;
	}
	fwrite(path, sizeof(unsigned char), 512, f);
	fclose(f);
	return true;
}

bool Save_Alternate_Image_Paths()
{
	int nap = alternate_image_paths.size();
	char pfile[512];
	Get_Project_Filename(pfile);
	char *c = strrchr(pfile, '.');
	if(!c)
	{
		return false;
	}
	strcpy(c, ".alt");
	FILE *f = fopen(pfile, "wb");
	if(!f)
	{
		return false;
	}
	for(int i = 0;i<nap;i++)
	{
		fwrite(alternate_image_paths[i].path, sizeof(unsigned char), 512, f);
	}
	fclose(f);
	return true;
}

bool Register_Alternate_Image_Path(char *apath)
{
	ALTERNATE_IMAGE_PATH aip;
	alternate_image_paths.push_back(aip);
	strcpy(alternate_image_paths[alternate_image_paths.size()-1].path, apath);
	Append_Alternate_Image_Path(apath);
	return true;
}

bool Try_Alternate_Image_Paths(char *file, char *res)
{
	int n = alternate_image_paths.size();
	char sfile[512];
	char temp[512];
	Get_File_From_Full_Path(file, sfile);
	for(int i = 0;i<n;i++)
	{
		sprintf(temp, "%s\\%s", alternate_image_paths[i].path, sfile);
		if(Exists(temp))
		{
			strcpy(res, temp);
			return true;
		}
	}
	return false;
}

bool flip_loaded_images_vertical = false;
bool flip_loaded_images_horizontal = false;
bool flip_redblue = false;

void Apply_Watermark_To_Export(unsigned char *data, int w, int h);

void Flip_Image_Vertical(unsigned char *rgb, int w, int h)
{
	unsigned char *temp = new unsigned char[w*h*3];
	int i, j, k1, k2;
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			k1 = ((j*w)+i)*3;
			k2 = ((((h-1)-j)*w)+i)*3;
			temp[k1] = rgb[k2];
			temp[k1+1] = rgb[k2+1];
			temp[k1+2] = rgb[k2+2];
		}
	}
	memcpy(rgb, temp, sizeof(unsigned char)*w*h*3);
	delete[] temp;
}

void Flip_Image_Horizontal(unsigned char *rgb, int w, int h)
{
	unsigned char *temp = new unsigned char[w*h*3];
	int i, j, k1, k2;
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			k1 = ((j*w)+i)*3;
			k2 = ((j*w)+((w-1)-i))*3;
			temp[k1] = rgb[k2];
			temp[k1+1] = rgb[k2+1];
			temp[k1+2] = rgb[k2+2];
		}
	}
	memcpy(rgb, temp, sizeof(unsigned char)*w*h*3);
	delete[] temp;
}


void Flip_Image8Bit_Vertical(unsigned char *rgb, int w, int h)
{
	unsigned char *temp = new unsigned char[w*h];
	int i, j, k1, k2;
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			k1 = ((j*w)+i);
			k2 = ((((h-1)-j)*w)+i);
			temp[k1] = rgb[k2];
		}
	}
	memcpy(rgb, temp, sizeof(unsigned char)*w*h);
	delete[] temp;
}

void Flip_Image8Bit_Horizontal(unsigned char *rgb, int w, int h)
{
	unsigned char *temp = new unsigned char[w*h];
	int i, j, k1, k2;
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			k1 = ((j*w)+i);
			k2 = ((j*w)+((w-1)-i));
			temp[k1] = rgb[k2];
		}
	}
	memcpy(rgb, temp, sizeof(unsigned char)*w*h);
	delete[] temp;
}


void Flip_RedBlue(unsigned char *rgb, int w, int h)
{
	unsigned char temp;
	int i, j, k;
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			k = ((j*w)+i)*3;
			temp = rgb[k+2];
			rgb[k+2] = rgb[k];
			rgb[k] = temp;
		}
	}
}

//FIXTHIS move watermark stuff into separate file?
unsigned char watermark[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,0,1,1,0,0,0,0,1,1,1,0,0,
0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,
0,0,0,0,1,0,0,0,1,0,0,1,1,1,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,
0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,
0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,1,0,0,1,0,0,1,0,0,0,1,1,1,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

int wm_width = 32;
int wm_height = 8;

extern int random_seed;

int wm_offset = 0;

bool Add_Random_Lines_To_Image(unsigned char *rgb, int width, int height, int bounce);

bool Add_Lines_To_Image(unsigned char *rgb, int width, int height)
{
#ifdef ADD_WATERMARK
//	srand(Get_Current_Project_Frame()+random_seed);
//	Add_Random_Lines_To_Image(rgb, width, height, 128);
#endif
	return true;
}


__forceinline void Watermark_Pixel(unsigned char *rgb, int x, int y)
{
#ifdef ADD_WATERMARK
	x += wm_offset;
	y += wm_offset;
	x = x/4;
	y = y/4;
	while(y>=wm_height){y-=wm_height;x+=10;}
	while(x>=wm_width){x-=wm_width;}
	unsigned char b = watermark[(y*wm_width)+x];
	if(b==0)return;
	int diff = 12;
	if(rgb[0]>128){rgb[0]-=diff;}else{rgb[0]+=diff;}
	if(rgb[1]>128){rgb[1]-=diff;}else{rgb[1]+=diff;}
	if(rgb[2]>128){rgb[2]-=diff;}else{rgb[2]+=diff;}
#endif
}

__forceinline void Apply_Watermark(unsigned char *data, int w, int h)
{
	wm_offset += random_seed;
	int i, j;
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			Watermark_Pixel(&data[((j*w)+i)*3], i, h-j);
		}
	}
}

__forceinline void Apply_Stereo_Watermark(unsigned char *data, int w, int h)
{
	wm_offset += random_seed;
	int i, j;
	int hw = w/2;
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			if(i>=hw)
			{
				Watermark_Pixel(&data[((j*w)+i)*3], i-hw, h-j);
			}
			else
			{
				Watermark_Pixel(&data[((j*w)+i)*3], i, h-j);
			}
		}
	}
}


bool Save_Stereo_Watermarked_Image(unsigned char *rgb, int w, int h, char *file)
{
	remove(file);
	unsigned int image = 0;
	ilGenImages(1, &image);
	ilBindImage(image);
	Apply_Stereo_Watermark(rgb, w, h);
	ilTexImage(w, h, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, rgb);
    iluImageParameter(ILU_FILTER, ILU_LINEAR);
	ilSaveImage(file);
	ilDeleteImages(1, &image);
	return true;
}

bool Save_Watermarked_Image(unsigned char *rgb, int w, int h, char *file)
{
	remove(file);
	unsigned int image = 0;
	ilGenImages(1, &image);
	ilBindImage(image);
	Apply_Watermark(rgb, w, h);
	ilTexImage(w, h, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, rgb);
    iluImageParameter(ILU_FILTER, ILU_LINEAR);
	ilSaveImage(file);
	ilDeleteImages(1, &image);
	return true;
}

bool Convert_Image_To_Float(float *dst, unsigned char *src, int total)
{
	for(int i = 0;i<total;i++)
	{
		dst[(i*3)] = ((float)src[(i*3)])/255;
		dst[(i*3)+1] = ((float)src[(i*3)+1])/255;
		dst[(i*3)+2] = ((float)src[(i*3)+2])/255;
	}
	return true;
}

bool Convert_Image_To_Bytes(unsigned char *dst, float *src, int total)
{
	for(int i = 0;i<total;i++)
	{
		dst[(i*3)] = (unsigned char)((float)src[(i*3)]*255);
		dst[(i*3)+1] = (unsigned char)((float)src[(i*3)+1]*255);
		dst[(i*3)+2] = (unsigned char)((float)src[(i*3)+2]*255);
	}
	return true;
}


bool Save_Image(unsigned char *rgb, int w, int h, char *file)
{
	remove(file);
	unsigned int image = 0;
	ilGenImages(1, &image);
	ilBindImage(image);
	ilTexImage(w, h, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, rgb);
    iluImageParameter(ILU_FILTER, ILU_LINEAR);
	ilSaveImage(file);
	ilDeleteImages(1, &image);
	return true;
}

bool Save_Clean_Image(float *rgb, int w, int h, char *file)
{
	remove(file);
	unsigned int image = 0;
	ilGenImages(1, &image);
	ilBindImage(image);
	ilTexImage(w, h, 1, 3, IL_RGB, IL_FLOAT, rgb);
    iluImageParameter(ILU_FILTER, ILU_LINEAR);
	ilSaveImage(file);
	ilDeleteImages(1, &image);
	return true;
}

bool Save_Clean_Image(unsigned char *rgb, int w, int h, char *file)
{
	remove(file);
	unsigned int image = 0;
	ilGenImages(1, &image);
	ilBindImage(image);
	ilTexImage(w, h, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, rgb);
    iluImageParameter(ILU_FILTER, ILU_LINEAR);
	ilSaveImage(file);
	ilDeleteImages(1, &image);
	return true;
}

bool Grayscale_To_RGB(unsigned char *dst, unsigned char *src, int num)
{
	for(int i = 0;i<num;i++)
	{
		dst[(i*3)] = dst[(i*3)+1] = dst[(i*3)+2] = src[i];
	}
	return true;
}

bool RGB_To_Grayscale(unsigned char *dst, unsigned char *src, int num)
{
	for(int i = 0;i<num;i++)
	{
		dst[i] = (src[(i*3)] + src[(i*3)+1] + src[(i*3)+2])/3;
	}
	return true;
}

bool RGBA_To_Grayscale(unsigned char *dst, unsigned char *src, int num)
{
	for(int i = 0;i<num;i++)
	{
		dst[i] = (src[(i*4)] + src[(i*4)+1] + src[(i*4)+2])/3;
	}
	return true;
}

bool RGBA_To_RGB(unsigned char *dst, unsigned char *src, int num)
{
	for(int i = 0;i<num;i++)
	{
		dst[(i*3)] = src[(i*4)];
		dst[(i*3)+1] = src[(i*4)+1];
		dst[(i*3)+2] = src[(i*4)+2];
	}
	return true;
}

bool RGB_To_RGBA(unsigned char *dst, unsigned char *src, int num)
{
	for(int i = 0;i<num;i++)
	{
		dst[(i*4)] = src[(i*3)];
		dst[(i*4)+1] = src[(i*3)+1];
		dst[(i*4)+2] = src[(i*3)+2];
	}
	return true;
}

bool RGB_To_Single_Channel(unsigned char *dst, unsigned char *src, int num, int channel)
{
	for(int i = 0;i<num;i++)
	{
		dst[i] = src[(i*3)+channel];
	}
	return true;
}


bool RGB_To_Bordered_Single_Channel(int border_size, unsigned char *dst, unsigned char *src, int w, int h, int channel)
{
	int i, j, k1, k2;
	int nw = w+border_size+border_size;//dst width with border
	int nh = h+border_size+border_size;//dst height with border
	//get center pixels on first pass
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			k1 = ((j+border_size)*nw)+i+border_size;//dst index
			k2 = (j*w)+i;//src index
			dst[k1] = src[(k2*3)+channel];
		}
	}
	//get top and bottom strips
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<border_size;j++)
		{
			k1 = ((j)*nw)+i+border_size;//dst index
			k2 = i;//src index
			dst[k1] = src[(k2*3)+channel];
			
			k1 = (((nh-1)-j)*nw)+i+border_size;//dst index
			k2 = ((h-1)*w)+i;//src index
			dst[k1] = src[(k2*3)+channel];
		}
	}
	//get left and right strips
	for(i = 0;i<border_size;i++)
	{
		for(j = 0;j<h;j++)
		{
			k1 = ((j+border_size)*nw)+i;//dst index
			k2 = (j*w);//src index
			dst[k1] = src[(k2*3)+channel];
			
			k1 = ((j+border_size)*nw)+((nw-1)-i);//dst index
			k2 = (j*w)+(w-1);//src index
			dst[k1] = src[(k2*3)+channel];
		}
	}
	//get corners
	for(i = 0;i<border_size;i++)
	{
		for(j = 0;j<border_size;j++)
		{
			//upper left
			k1 = (j*nw)+i;//dst index
			k2 = 0;//src index
			dst[k1] = src[(k2*3)+channel];

			//lower left
			k1 = (((nh-1)-j)*nw)+i;//dst index
			k2 = ((h-1))*w;//src index
			dst[k1] = src[(k2*3)+channel];

			//upper right
			k1 = (j*nw)+((nw-1)-i);//dst index
			k2 = w-1;//src index
			dst[k1] = src[(k2*3)+channel];

			//lower right
			k1 = (((nh-1)-j)*nw)+((nw-1)-i);//dst index
			k2 = ((h-1)*w)+w-1;//src index
			dst[k1] = src[(k2*3)+channel];
		}
	}
	return true;
}

bool RGB_To_Bordered_Grayscale(int border_size, unsigned char *dst, unsigned char *src, int w, int h)
{
	int i, j, k1, k2;
	int nw = w+border_size+border_size;//dst width with border
	int nh = h+border_size+border_size;//dst height with border
	//get center pixels on first pass
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<h;j++)
		{
			k1 = ((j+border_size)*nw)+i+border_size;//dst index
			k2 = (j*w)+i;//src index
			dst[k1] = (src[(k2*3)] + src[(k2*3)+1] + src[(k2*3)+2])/3;
		}
	}
	//get top and bottom strips
	for(i = 0;i<w;i++)
	{
		for(j = 0;j<border_size;j++)
		{
			k1 = ((j)*nw)+i+border_size;//dst index
			k2 = i;//src index
			dst[k1] = (src[(k2*3)] + src[(k2*3)+1] + src[(k2*3)+2])/3;
			
			k1 = (((nh-1)-j)*nw)+i+border_size;//dst index
			k2 = ((h-1)*w)+i;//src index
			dst[k1] = (src[(k2*3)] + src[(k2*3)+1] + src[(k2*3)+2])/3;
		}
	}
	//get left and right strips
	for(i = 0;i<border_size;i++)
	{
		for(j = 0;j<h;j++)
		{
			k1 = ((j+border_size)*nw)+i;//dst index
			k2 = (j*w);//src index
			dst[k1] = (src[(k2*3)] + src[(k2*3)+1] + src[(k2*3)+2])/3;
			
			k1 = ((j+border_size)*nw)+((nw-1)-i);//dst index
			k2 = (j*w)+(w-1);//src index
			dst[k1] = (src[(k2*3)] + src[(k2*3)+1] + src[(k2*3)+2])/3;
		}
	}
	//get corners
	for(i = 0;i<border_size;i++)
	{
		for(j = 0;j<border_size;j++)
		{
			//upper left
			k1 = (j*nw)+i;//dst index
			k2 = 0;//src index
			dst[k1] = (src[(k2*3)] + src[(k2*3)+1] + src[(k2*3)+2])/3;

			//lower left
			k1 = (((nh-1)-j)*nw)+i;//dst index
			k2 = ((h-1))*w;//src index
			dst[k1] = (src[(k2*3)] + src[(k2*3)+1] + src[(k2*3)+2])/3;

			//upper right
			k1 = (j*nw)+((nw-1)-i);//dst index
			k2 = w-1;//src index
			dst[k1] = (src[(k2*3)] + src[(k2*3)+1] + src[(k2*3)+2])/3;

			//lower right
			k1 = (((nh-1)-j)*nw)+((nw-1)-i);//dst index
			k2 = ((h-1)*w)+w-1;//src index
			dst[k1] = (src[(k2*3)] + src[(k2*3)+1] + src[(k2*3)+2])/3;
		}
	}
	return true;
}


bool Save_Grayscale_Image(unsigned char *img, int w, int h, char *file)
{
	unsigned char *rgb = new unsigned char[w*h*3];
	Grayscale_To_RGB(rgb, img, w*h);
	Save_Image(rgb, w, h, file);
	delete[] rgb;
	return true;
}

void Convert_48_Bit(unsigned char *src, unsigned char *dst, int w, int h)
{
	int total = w*h*3;
	for(int i = 0;i<total;i++)
	{
		dst[i] = (unsigned char)((((float)(*((unsigned short*)(&src[(i*2)]))))/65535)*255);
	}
}

bool IsEven(int n)
{
	float f = (float)n;
	return (f/2)==((int)f/2);
}


//limit to 720x480
__forceinline void DownSizeILImage()
{
	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	int depth = ilGetInteger(IL_IMAGE_DEPTH);
	if(width>720)
	{
		float p = (720.0f/width);
		float nh = height*p;
		width = 720;
		height = (int)nh;
		iluScale(width, height, 24);
	}
	if(height>480)
	{
		float p = (480.0f/height);
		float nw = width*p;
		height = 480;
		width = (int)nw;
		iluScale(width, height, 24);
	}
}

unsigned char* Get_Image_Data(char *file, int *width, int *height, int *bits)
{
	char rfile[512];
	strcpy(rfile, file);
	if(!Exists(rfile))
	{
		Try_Alternate_Image_Paths(rfile, rfile);
	}
	unsigned int image;
	ilGenImages(1, &image);
	ilBindImage(image);
	if(!ilLoadImage(rfile))
	{
		Display_ILError();
		ilDeleteImages(1, &image);
		return 0;
	}
#ifdef RESIZE_IMAGES
	DownSizeILImage();
#endif
	*width = ilGetInteger(IL_IMAGE_WIDTH);
	*height = ilGetInteger(IL_IMAGE_HEIGHT);
	*bits = ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);
	bool we = IsEven(*width);
	bool he = IsEven(*height);
	if(!we||!he)
	{
		if(!we)(*width) = *width + 1;
		if(!he)(*height) = *height + 1;
		int depth = ilGetInteger(IL_IMAGE_DEPTH);
		iluScale(*width, *height, depth);
	}
	int bytes = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	unsigned int total = ((*width)*(*height))*bytes;
	unsigned char *data = 0;
	unsigned char *src = ilGetData();
	if(bytes==6)
	{
		data = new unsigned char[total/2];
		Convert_48_Bit(src, data, *width, *height);
	}
	else
	{
		data = new unsigned char[total];
		int strip_total = *width*bytes;
		memcpy(data, src, total);
	}
	ilDeleteImages(1, &image);
	return data;
}

bool Extension_Needs_Flipping(char *file)
{
	char *c = strrchr(file, '.');
	if(c)
	{
		if(!strcmp(c, ".bmp"))return true;
		if(!strcmp(c, ".BMP"))return true;
		if(!strcmp(c, ".jpg"))return true;
		if(!strcmp(c, ".JPG"))return true;
	}
	return false;
}

unsigned char* Get_RGB_Image_Data(char *file, int *width, int *height)
{
	int bits;
	unsigned char *data = Get_Image_Data(file, width, height, &bits);
	if(!data)
	{
		return 0;
	}
	if(bits==8)
	{
		unsigned char *ndata = new unsigned char[(*width)*(*height)*3];
		Grayscale_To_RGB(ndata, data, (*width)*(*height));
		delete[] data;
		data = ndata;
	}
	if(bits==32)
	{
		unsigned char *ndata = new unsigned char[(*width)*(*height)*3];
		RGBA_To_RGB(ndata, data, (*width)*(*height));
		delete[] data;
		data = ndata;
	}
	bool auto_flip = Extension_Needs_Flipping(file);
	if(flip_loaded_images_vertical)
	{
		if(!auto_flip)
		{
			Flip_Image_Vertical(data, *width, *height);
		}
	}
	else if(auto_flip)
	{
		Flip_Image_Vertical(data, *width, *height);
	}
	if(flip_loaded_images_horizontal)Flip_Image_Horizontal(data, *width, *height);
	if(flip_redblue)Flip_RedBlue(data, *width, *height);
	return data;
}

unsigned char* Replace_RGB_Image_Data(unsigned char *buffer, int old_width, int old_height, char *file, int *width, int *height)
{
	char rfile[512];
	strcpy(rfile, file);
	if(!Exists(rfile))
	{
		Try_Alternate_Image_Paths(rfile, rfile);
	}
	unsigned int image;
	ilGenImages(1, &image);
	ilBindImage(image);
	if(!ilLoadImage(rfile))
	{
		Display_ILError();
		ilDeleteImages(1, &image);
		return 0;
	}
#ifdef RESIZE_IMAGES
	DownSizeILImage();
#endif
	*width = ilGetInteger(IL_IMAGE_WIDTH);
	*height = ilGetInteger(IL_IMAGE_HEIGHT);
	bool we = IsEven(*width);
	bool he = IsEven(*height);
	if(!we||!he)
	{
		if(!we)(*width) = *width + 1;
		if(!he)(*height) = *height + 1;
		int depth = ilGetInteger(IL_IMAGE_DEPTH);
		iluScale(*width, *height, depth);
	}
	if(*width!=old_width||*height!=old_height)
	{
		SkinMsgBox(0, "New image dimensions don't match existing frame!", 0, MB_OK);
		ilDeleteImages(1, &image);
		return 0;
	}
	int bytes = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	unsigned int total = ((*width)*(*height))*bytes;
	unsigned char *src = ilGetData();
	if(bytes==6)
	{
		Convert_48_Bit(src, buffer, *width, *height);
	}
	else if(bytes==4)
	{
		RGBA_To_RGB(buffer, src, ((*width)*(*height)));
	}
	else if(bytes==3)
	{
		memcpy(buffer, src, total);
	}
	else if(bytes==1)
	{
		Grayscale_To_RGB(buffer, src, ((*width)*(*height)));
	}
	ilDeleteImages(1, &image);
	bool auto_flip = Extension_Needs_Flipping(file);
	if(flip_loaded_images_vertical)
	{
		if(!auto_flip)
		{
			Flip_Image_Vertical(buffer, *width, *height);
		}
	}
	else if(auto_flip)
	{
		Flip_Image_Vertical(buffer, *width, *height);
	}
	if(flip_loaded_images_horizontal)Flip_Image_Horizontal(buffer, *width, *height);
	if(flip_redblue)Flip_RedBlue(buffer, *width, *height);
	return buffer;
}


unsigned char* Get_Grayscale_Image_Data(char *file, int *width, int *height, int *bits)
{
	unsigned char *data = Get_Image_Data(file, width, height, bits);
	if(!data)
	{
		return 0;
	}
	if(*bits==24)
	{
		unsigned char *ndata = new unsigned char[(*width)*(*height)];
		RGB_To_Grayscale(ndata, data, (*width)*(*height));
		delete[] data;
		data = ndata;
	}
	if(*bits==32)
	{
		unsigned char *ndata = new unsigned char[(*width)*(*height)];
		RGBA_To_Grayscale(ndata, data, (*width)*(*height));
		delete[] data;
		data = ndata;
	}
	if(flip_loaded_images_vertical)Flip_Image8Bit_Vertical(data, *width, *height);
	if(flip_loaded_images_horizontal)Flip_Image8Bit_Horizontal(data, *width, *height);
	return data;
}

unsigned int Create_Frame_Texture(int width, int height, unsigned char *data)
{
	UINT textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	return textureID;
}



unsigned int Create_32Bit_Frame_Texture(int width, int height, unsigned char *data)
{
	UINT textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	return textureID;
}

unsigned int Create_32Bit_Frame_Texture(int width, int height, float *data)
{
	UINT textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);
	return textureID;
}

void Free_GL_Texture(unsigned int id)
{
	glDeleteTextures(1, &id);
}

