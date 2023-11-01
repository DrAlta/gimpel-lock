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
#include "Skin.h"
#include "Resource.h"


int Get_Desktop_Bits()
{
	HDC mhDC = CreateDC("DISPLAY", 0, 0, 0); 
	int bits = GetDeviceCaps(mhDC, BITSPIXEL); 
	DeleteDC(mhDC); 
	return bits;
}

int desktop_bits = 0;

COLORREF skin_button_color;
COLORREF skin_button_text_color;

COLORREF skin_button_disabled_color;
COLORREF skin_button_pushed_color;

COLORREF skin_text_color;
COLORREF skin_text_bg_color;

COLORREF skin_static_text_bg_color;
COLORREF skin_static_text_color;

CBrush skin_static_background_brush_color;
COLORREF skin_checkbox_color;


COLORREF skin_edit_text_color;
COLORREF skin_edit_text_bg_color;
CBrush skin_edit_background_brush_color;


CFont *skin_font;
CFont *skin_cross_out_font;

CBrush skin_background_brush_color;

CBrush skin_texture_brush_color;

CBrush dark_skin_background_brush_color;

COLORREF selected_menu_bg_color;
COLORREF unselected_menu_bg_color;
CBrush selected_menu_brush;
CBrush unselected_menu_brush;

COLORREF skin_titlebar_text_color;
COLORREF skin_titlebar_color;

COLORREF skin_slider_button_color;
COLORREF skin_slider_button_outline_color;

CPen skin_slider_pen;
CBrush skin_slider_brush;

bool Init_Skin_Font()
{
	skin_font = new CFont;
	bool res = skin_font->CreateFont(
   14,                        // nHeight
   0,                         // nWidth
   0,                         // nEscapement
   0,                         // nOrientation
   FW_THIN,                 // nWeight
   FALSE,                     // bItalic
   FALSE,                     // bUnderline
   0,                         // cStrikeOut
   ANSI_CHARSET,              // nCharSet
   OUT_DEFAULT_PRECIS,        // nOutPrecision
   CLIP_DEFAULT_PRECIS,       // nClipPrecision
   DEFAULT_QUALITY,           // nQuality
   DEFAULT_PITCH,  // nPitchAndFamily
   "Arial")!=0;                 // lpszFacename
	if(!res)
	{
		return false;
	}

	skin_cross_out_font = new CFont;
	res = skin_cross_out_font->CreateFont(
   14,                        // nHeight
   0,                         // nWidth
   0,                         // nEscapement
   0,                         // nOrientation
   FW_THIN,                 // nWeight
   FALSE,                     // bItalic
   FALSE,                     // bUnderline
   1,                         // cStrikeOut
   ANSI_CHARSET,              // nCharSet
   OUT_DEFAULT_PRECIS,        // nOutPrecision
   CLIP_DEFAULT_PRECIS,       // nClipPrecision
   DEFAULT_QUALITY,           // nQuality
   DEFAULT_PITCH,  // nPitchAndFamily
   "Arial")!=0;                 // lpszFacename

	if(!res)
	{
		return false;
	}
	return res;
}

CPen control_edge_lightest_pen;
CPen control_edge_lighter_pen;
CPen control_edge_darker_pen;
CPen control_edge_darkest_pen;

int primary_skin_color = 2;
int secondary_skin_color = 1;
float skin_hue_mix = 0.25f;

bool Set_Skin_Color(int color1, int color2, int mix)
{
	primary_skin_color = color1;
	secondary_skin_color = color2;
	skin_hue_mix = ((float)mix)/100;
	Init_Skin();
	return true;
}


COLORREF myRGB(int *rgb)
{
	COLORREF res;
	float p[3];
	float s[3];
	float r[3];
	if(primary_skin_color==2)
	{
		p[0] = (float)rgb[0];
		p[1] = (float)rgb[1];
		p[2] = (float)rgb[2];
	}
	else if(primary_skin_color==1)
	{
		p[0] = (float)rgb[1];
		p[1] = (float)rgb[2];
		p[2] = (float)rgb[0];
	}
	else if(primary_skin_color==0)
	{
		p[0] = (float)rgb[2];
		p[1] = (float)rgb[0];
		p[2] = (float)rgb[1];
	}
	if(secondary_skin_color==2)
	{
		s[0] = (float)rgb[0];
		s[1] = (float)rgb[1];
		s[2] = (float)rgb[2];
	}
	else if(secondary_skin_color==1)
	{
		s[0] = (float)rgb[1];
		s[1] = (float)rgb[2];
		s[2] = (float)rgb[0];
	}
	else if(secondary_skin_color==0)
	{
		s[0] = (float)rgb[2];
		s[1] = (float)rgb[0];
		s[2] = (float)rgb[1];
	}
	float ip = 1.0f-skin_hue_mix;
	r[0] = (p[0]*ip)+(s[0]*skin_hue_mix);
	r[1] = (p[1]*ip)+(s[1]*skin_hue_mix);
	r[2] = (p[2]*ip)+(s[2]*skin_hue_mix);
	if(r[0]<0)r[0] = 0;if(r[0]>255)r[0] = 255;
	if(r[1]<0)r[1] = 0;if(r[1]>255)r[1] = 255;
	if(r[2]<0)r[2] = 0;if(r[2]>255)r[2] = 255;
	res = RGB((int)r[0],(int)r[1],(int)r[2]);
	return res;
}

COLORREF myRGB(int r, int g, int b)
{
	int rgb[3] = {r,g,b};
	return myRGB(rgb);
}

bool Set_Control_Edge_Color(int r, int g, int b)
{
	float base[3] = {(float)r,(float)g,(float)b};
	if(base[0]>=base[1]&&base[0]>=base[2])
	{
		base[1] = (base[1]/base[0])*255;
		base[2] = (base[2]/base[0])*255;
		base[0] = 255;
	}
	else if(base[2]>=base[0]&&base[2]>=base[1])
	{
		base[0] = (base[0]/base[2])*255;
		base[1] = (base[1]/base[2])*255;
		base[2] = 255;
	}
	else
	{
		base[0] = (base[0]/base[1])*255;
		base[2] = (base[2]/base[1])*255;
		base[1] = 255;
	}
	float flightest[3] = {base[0],base[1],base[2]};
	float flighter[3] = {base[0]*0.75f,base[1]*0.75f,base[2]*0.75f};
	float fdarker[3] = {base[0]*0.5f,base[1]*0.5f,base[2]*0.5f};
	float fdarkest[3] = {0,0,0};
	COLORREF lightest = myRGB((int)flightest[0],(int)flightest[1],(int)flightest[2]);
	COLORREF lighter = myRGB((int)flighter[0],(int)flighter[1],(int)flighter[2]);
	COLORREF darker = myRGB((int)fdarker[0],(int)fdarker[1],(int)fdarker[2]);
	COLORREF darkest = myRGB((int)fdarkest[0],(int)fdarkest[1],(int)fdarkest[2]);
	control_edge_lightest_pen.CreatePen(PS_SOLID, 0, lightest);
	control_edge_lighter_pen.CreatePen(PS_SOLID, 0, lighter);
	control_edge_darker_pen.CreatePen(PS_SOLID, 0, darker);
	control_edge_darkest_pen.CreatePen(PS_SOLID, 0, darkest);
	return true;
}

//base color 196,196,255
//dark bg color 32,32,64
//text color - base color 196,196,255
//text bg color - 64,64,128 == dark bg * 2
//button color 40,40,80
//button disabled color 64,80,128
//button pushed color 80,80,128
//button text color - base color 196,196,255
//selected menu bg color 0,0,153
//static text bg 64,64,100
//static text 196,255,255
//edit text - base color 196,196,255
//edit text color 64,64,80
//titlebar text 196,200,255
//titlebar bg 64,64,100
//checkbox - base color 196,196,255


int SkinBase[3] = {196,196,255};
int SkinBaseDark[3] = {32,32,64};
int SkinText[3] = {196,196,255};
int SkinTextBG[3] = {64,64,128};
int SkinButton[3] = {40,40,80};
int SkinButtonDisabled[3] = {64,80,128};
int SkinButtonPushed[3] = {80,80,128};
int SkinButtonText[3] = {196,196,255};
int SkinMenuSelected[3] = {0,0,153};
int SkinStaticTextBG[3] = {64,64,100};
int SkinStaticText[3] = {196,255,255};
int SkinEditText[3] = {196,196,255};
int SkinEditTextBG[3] = {64,64,80};
int SkinTitleBarText[3] = {196,200,255};
int SkinTitleBarTextBG[3] = {64,64,100};
int SkinCheckBox[3] = {196,196,255};
int SkinSliderButtonOutline[3] = {40,40,80};
int SkinSliderButton[3] = {196,255,255};

unsigned char tint(unsigned char c, float s)
{
	float fc = c;
	fc*=s;
	return (unsigned char)fc;
}

unsigned char* Get_Skin_Texture_Bits(int *w, int *h)
{
	int width = 512;
	int height = 512;
	int total = width*height;
	unsigned char *data = new unsigned char[total*8];
	*w = width;
	*h = height;
	return data;
}

bool Update_Skin_Texture()
{
	CBitmap skin_bmp;
	skin_bmp.LoadBitmap(IDB_SKIN);
	BITMAP bmp;
	skin_bmp.GetBitmap(&bmp);
	int width = bmp.bmWidth;
	int height = bmp.bmHeight;
	int total = bmp.bmWidth*bmp.bmHeight;
	unsigned char *rgb = 0;
	float base[3];
	COLORREF c = myRGB(SkinBase);
	unsigned char r = GetRValue(c);
	unsigned char g = GetGValue(c);
	unsigned char b = GetBValue(c);
	base[0] = ((float)r)/255;
	base[1] = ((float)g)/255;
	base[2] = ((float)b)/255;
	if(bmp.bmBitsPixel==24)
	{
		rgb = new unsigned char[total*3];
		skin_bmp.GetBitmapBits(total*3, rgb);
		//bgr
		for(int i = 0;i<total;i++)
		{
			rgb[(i*3)] = tint(rgb[(i*3)], base[2]);
			rgb[(i*3)+1] = tint(rgb[(i*3)+1], base[1]);
			rgb[(i*3)+2] = tint(rgb[(i*3)+2], base[0]);
		}
		skin_bmp.SetBitmapBits(total*3, rgb);
	}
	if(bmp.bmBitsPixel==32)
	{
		rgb = new unsigned char[total*4];
		skin_bmp.GetBitmapBits(total*4, rgb);
		//bgr
		for(int i = 0;i<total;i++)
		{
			rgb[(i*4)] = tint(rgb[(i*4)], base[2]);
			rgb[(i*4)+1] = tint(rgb[(i*4)+1], base[1]);
			rgb[(i*4)+2] = tint(rgb[(i*4)+2], base[0]);
		}
		skin_bmp.SetBitmapBits(total*4, rgb);
	}
	skin_texture_brush_color.CreatePatternBrush(&skin_bmp);
	skin_bmp.DeleteObject();
	return true;
}

bool Init_Skin()
{

	desktop_bits = Get_Desktop_Bits();

	Set_Control_Edge_Color(196,196,255);

	static bool first = true;
	if(first)
	{
		Init_Skin_Font();
		first = false;
	}

	Update_Skin_Texture();

	COLORREF dark_bg_color = myRGB(SkinBaseDark);//32,32,64);
	skin_text_color = myRGB(SkinText);//196,196,255);
	skin_text_bg_color = myRGB(SkinTextBG);//64,64,128);

	skin_button_color = myRGB(SkinButton);//40,40,80);
	skin_button_disabled_color = myRGB(SkinButtonDisabled);//64,80,128);
	skin_button_pushed_color = myRGB(SkinButtonPushed);//80,80,128);
	skin_button_text_color = myRGB(SkinButtonText);//196,196,255);
	
	skin_background_brush_color.CreateSolidBrush(skin_text_bg_color);
	
	dark_skin_background_brush_color.CreateSolidBrush(dark_bg_color);

	selected_menu_bg_color = myRGB(SkinMenuSelected);//0,0,153);
	unselected_menu_bg_color = skin_text_bg_color;
	selected_menu_brush.CreateSolidBrush(selected_menu_bg_color);
	unselected_menu_brush.CreateSolidBrush(unselected_menu_bg_color);

	
	skin_static_text_bg_color = myRGB(SkinStaticTextBG);//64,64,100);
	skin_static_text_color = myRGB(SkinStaticText);//196,255,255);
	
	skin_static_background_brush_color.CreateSolidBrush(skin_static_text_bg_color);

	skin_edit_text_color = myRGB(SkinEditText);//196,196,255);
	skin_edit_text_bg_color = myRGB(SkinEditTextBG);//64,64,80);
	skin_edit_background_brush_color.CreateSolidBrush(skin_edit_text_bg_color);

	skin_titlebar_text_color = myRGB(SkinTitleBarText);//196,200,255);
	skin_titlebar_color = myRGB(SkinTitleBarTextBG);//64,64,100);

	skin_checkbox_color = myRGB(SkinCheckBox);//196,196,255);

	skin_slider_button_color = myRGB(SkinSliderButton);
	skin_slider_button_outline_color = myRGB(SkinSliderButtonOutline);

	skin_slider_pen.CreatePen( PS_SOLID, 1, skin_slider_button_outline_color);
	skin_slider_brush.CreateSolidBrush(skin_slider_button_color);
	
	return false;
}

bool Free_Skin()
{
	return false;
}


void CSkinStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}

struct COLOR_SCHEME
{
	int primary_skin_color;
	int secondary_skin_color;
	float skin_hue_mix;
};

bool Save_Color_Scheme(char *file)
{
	COLOR_SCHEME cs;
	cs.primary_skin_color = primary_skin_color;
	cs.secondary_skin_color = secondary_skin_color;
	cs.skin_hue_mix = skin_hue_mix;
	FILE *f = fopen(file, "wb");
	if(!f){return false;}
	fwrite(&cs, sizeof(COLOR_SCHEME), 1, f);
	fclose(f);
	return true;
}

bool Load_Color_Scheme(char *file)
{
	COLOR_SCHEME cs;
	cs.primary_skin_color = primary_skin_color;
	cs.secondary_skin_color = secondary_skin_color;
	cs.skin_hue_mix = skin_hue_mix;
	FILE *f = fopen(file, "rb");
	if(!f){return false;}
	fread(&cs, sizeof(COLOR_SCHEME), 1, f);
	fclose(f);
	primary_skin_color = cs.primary_skin_color;
	secondary_skin_color = cs.secondary_skin_color;
	skin_hue_mix = cs.skin_hue_mix;
	return true;
}

