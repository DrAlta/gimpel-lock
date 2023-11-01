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
#include "GLText.h"
#include "GLWnd.h"
#include <GL/gl.h>
#include <vector>

using namespace std;

class GL_FONT
{
public:
	GL_FONT()
	{
	}
	~GL_FONT()
	{
	}
	char m_name[64];
	int m_fontbase;
	int m_size;
};

vector<GL_FONT*> gl_fonts;

class TEXT_ITEM
{
public:
	TEXT_ITEM()
	{
	}
	~TEXT_ITEM()
	{
	}
	GL_FONT *m_font;
	char m_text[256];
	float m_color[4];
	float m_x, m_y;
};

vector<TEXT_ITEM*> temp_text_items;
vector<TEXT_ITEM*> static_text_items;


GL_FONT *current_font = 0;

float print_x = 0;
float print_y = 0;
float print_color[4] ={1,1,1,1};

HDC Edit_HDC();

int Load_GLFont(char *name, int size)
{
	GL_FONT *f = new GL_FONT;
	HFONT font;
	HFONT oldfont;
	f->m_fontbase = glGenLists(96);
	font = CreateFont(	-size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE|DEFAULT_PITCH, name);
	oldfont = (HFONT)SelectObject(Edit_HDC(), font);
	SelectObject(Edit_HDC(), oldfont);
	DeleteObject(font);
	gl_fonts.push_back(f);
	strcpy(f->m_name, name);
	f->m_size = size;
	return gl_fonts.size()-1;
}

bool Set_GLFont(int id)
{
	if(id>=(int)gl_fonts.size()||id<0)
	{
		return false;
	}
	current_font = gl_fonts[id];
	wglUseFontBitmaps(Edit_HDC(), 32, 96, current_font->m_fontbase);
	glListBase(current_font->m_fontbase - 32);
	return true;
}

bool Set_GLPrint_Pos(float x, float y)
{
	print_x = x;
	print_y = y;
	return true;
}

bool Set_GLText_Color(float *rgba)
{
	print_color[0] = rgba[0];
	print_color[1] = rgba[1];
	print_color[2] = rgba[2];
	print_color[3] = rgba[3];
	return true;
}

bool Print_GLText(const char *fmt, ...)
{
	if(!current_font)
	{
		return false;
	}
	char text[256];
	va_list		ap;
	if (fmt == NULL)return false;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);
	TEXT_ITEM *ti = new TEXT_ITEM;
	strcpy(ti->m_text, text);
	ti->m_x = print_x;
	ti->m_y = print_y;
	ti->m_color[0] = print_color[0];
	ti->m_color[1] = print_color[1];
	ti->m_color[2] = print_color[2];
	ti->m_color[3] = print_color[3];
	ti->m_font = current_font;
	temp_text_items.push_back(ti);
	return true;
}

bool Clear_Static_Text_Items()
{
	int n = static_text_items.size();
	for(int i = 0;i<n;i++)
	{
		delete static_text_items[i];
	}
	static_text_items.clear();
	return true;
}

bool Add_Static_Text_Item(int x, int y, float *rgba, char *text)
{
	if(!current_font)
	{
		return false;
	}
	TEXT_ITEM *ti = new TEXT_ITEM;
	strcpy(ti->m_text, text);
	ti->m_x = (float)x;
	ti->m_y = (float)y;
	ti->m_color[0] = rgba[0];
	ti->m_color[1] = rgba[1];
	ti->m_color[2] = rgba[2];
	ti->m_color[3] = rgba[3];
	ti->m_font = current_font;
	static_text_items.push_back(ti);
	return true;
}

bool Init_GL_Text()
{
	Load_GLFont("Arial", 18);
	Set_GLFont(0);
	return true;
}

bool Print_Text_Item(TEXT_ITEM *ti)
{
	if(current_font!=ti->m_font)
	{
		current_font = ti->m_font;
		wglUseFontBitmaps(Edit_HDC(), 32, 96, ti->m_font->m_fontbase);
		glListBase(ti->m_font->m_fontbase - 32);
	}
	glColor3fv(ti->m_color);
	glRasterPos2i((int)ti->m_x, (int)ti->m_y);
	glCallLists(strlen(ti->m_text), GL_UNSIGNED_BYTE, ti->m_text);
	return true;
}


void Print_GLText_Items()
{
	int n = temp_text_items.size();
	glPushAttrib(GL_LIST_BIT);
	wglUseFontBitmaps(Edit_HDC(), 32, 96, current_font->m_fontbase);
	glListBase(current_font->m_fontbase - 32);
	int i;
	for(i = 0;i<n;i++)
	{
		Print_Text_Item(temp_text_items[i]);
		delete temp_text_items[i];
	}
	temp_text_items.clear();
	n = static_text_items.size();
	for(i = 0;i<n;i++)
	{
		Print_Text_Item(static_text_items[i]);
	}
	glColor4f(1,1,1,1);
	glPopAttrib();
}

bool Free_GLText()
{
	unsigned int n = gl_fonts.size();
	unsigned int i;
	for(i = 0;i<n;i++)
	{
		delete gl_fonts[i];
	}
	gl_fonts.clear();
	n = temp_text_items.size();
	for(i = 0;i<n;i++)
	{
		delete temp_text_items[i];
	}
	temp_text_items.clear();
	n = static_text_items.size();
	for(i = 0;i<n;i++)
	{
		delete static_text_items[i];
	}
	static_text_items.clear();
	return true;
}


