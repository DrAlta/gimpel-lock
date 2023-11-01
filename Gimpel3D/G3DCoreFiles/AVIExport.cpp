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
#include <vfw.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include "../Skin.h"

#pragma comment(lib, "vfw32.lib")

bool Display_ILError();


IAVIStream *ps = 0;
IAVIStream *psCompressed = 0;
int frame_period = 0;
int current_frame = 0;
IAVIFile *pfile = 0;
void *avibits;
HBITMAP avihbm = 0;
HDC avihdc = 0;

int avi_width = 0;
int avi_height = 0;

bool Start_AVI_File(const char *fn, int width, int height)
{
	avi_width = width;
	avi_height = height;
	AVIFileInit();
	HRESULT hr = AVIFileOpen(&pfile, fn, OF_WRITE|OF_CREATE, NULL);
	if (hr!=AVIERR_OK)
	{
		AVIFileExit();
		SkinMsgBox(0, "error exporting avi", 0, MB_OK);
		return NULL;
	}
	current_frame = 0;
	HDC hdcscreen = GetDC(0);
	avihdc = CreateCompatibleDC(hdcscreen);
	ReleaseDC(0, hdcscreen);
	BITMAPINFO bi;
	ZeroMemory(&bi,sizeof(bi));
	BITMAPINFOHEADER &bih = bi.bmiHeader;
	bih.biSize = sizeof(bih);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = ((bih.biWidth*bih.biBitCount/8+3)&0xFFFFFFFC)*bih.biHeight;
	bih.biXPelsPerMeter = 10000;
	bih.biYPelsPerMeter = 10000;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	avihbm = CreateDIBSection(avihdc, (BITMAPINFO*)&bih, DIB_RGB_COLORS, &avibits, 0, 0);
	return true;
}


bool Add_Avi_Frame(HBITMAP hbm)
{
	DIBSECTION dibs;
	int sbm = GetObject(hbm, sizeof(dibs), &dibs);
	if(sbm!=sizeof(DIBSECTION))
	{
		return false;
	}
	if (ps==0)
	{
		AVISTREAMINFO strhdr; ZeroMemory(&strhdr,sizeof(strhdr));
	    strhdr.fccType = streamtypeVIDEO;
		strhdr.fccHandler = 0; 
		strhdr.dwScale = 1;
		strhdr.dwRate = 24;;
		strhdr.dwSuggestedBufferSize  = dibs.dsBmih.biSizeImage;
		SetRect(&strhdr.rcFrame, 0, 0, dibs.dsBmih.biWidth, dibs.dsBmih.biHeight);
		HRESULT hr = AVIFileCreateStream(pfile, &ps, &strhdr);
		if (hr!=AVIERR_OK)
		{
			return false;
		}
	}
	if (psCompressed==0)
	{
		AVICOMPRESSOPTIONS opts; ZeroMemory(&opts,sizeof(opts));
		opts.fccHandler = 0;
		AVICOMPRESSOPTIONS *po = &opts;
		AVISaveOptions(0, ICMF_CHOOSE_PREVIEW, 1, &ps, &po);
		HRESULT hr = AVIMakeCompressedStream(&psCompressed, ps, &opts, NULL);
		if (hr != AVIERR_OK)
		{
			return false;
		}
		hr = AVIStreamSetFormat(psCompressed, 0, &dibs.dsBmih, dibs.dsBmih.biSize+dibs.dsBmih.biClrUsed*sizeof(RGBQUAD));
		if (hr!=AVIERR_OK)
		{
			return false;
		}
		return true;
	}
	HRESULT hr = AVIStreamWrite(psCompressed, current_frame, 1, dibs.dsBm.bmBits, dibs.dsBmih.biSizeImage, AVIIF_KEYFRAME, NULL, NULL);
	if (hr!=AVIERR_OK)
	{
		return false;
	}
	current_frame++;
	return true;
}

bool Add_AVI_Frame(unsigned char *data)
{
	int n = avi_width*avi_height;
	char r;
	for(int i = 0;i<n;i++)
	{
		r = data[i*3];
		data[(i*3)] = data[(i*3)+2];
		data[(i*3)+2] = r;
	}
	memcpy(avibits, data, sizeof(unsigned char)*avi_width*avi_height*3);
	Add_Avi_Frame(avihbm);
	return true;
}

bool Close_AVI()
{
	if(psCompressed){AVIStreamRelease(psCompressed);}
	if(ps){AVIStreamRelease(ps);}
	if(pfile){AVIFileRelease(pfile);}
	AVIFileExit();
	ps = 0;
	pfile = 0;
	psCompressed = 0;
	DeleteDC(avihdc);
	DeleteObject(avihbm);
	return true;
}

bool Add_Image_To_AVI(char *file)
{
	unsigned int image;
	ilGenImages(1, &image);
	ilBindImage(image);
	if(!ilLoadImage(file))
	{
		Display_ILError();
		ilDeleteImages(1, &image);
		return false;
	}
	if(avi_width!=ilGetInteger(IL_IMAGE_WIDTH)||avi_height!=ilGetInteger(IL_IMAGE_HEIGHT)||ilGetInteger(IL_IMAGE_BITS_PER_PIXEL)!=24)
	{
		ilDeleteImages(1, &image);
		return false;
	}
	memcpy(avibits, ilGetData(), avi_width*avi_height*3);
	ilDeleteImages(1, &image);
	Add_Avi_Frame(avihbm);
	return true;
}
