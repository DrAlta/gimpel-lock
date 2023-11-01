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
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>
#include "../GapFillTool.h"
#include "../Skin.h"

extern HGLRC _hRC;

void Print_GLText_Items();

bool Clear_All_GapFill_Keys();
bool Update_GapFill_Mouse(int x, int y);
bool Update_GapFill_Mouse_Wheel(int n);
bool Update_GapFill_Key(int key, bool pressed);

bool render_ghost_blend_image = false;
bool render_background_blend_image = false;

int gapfill_texture = -1;
int gapfill_blend_texture = -1;


bool Render_Gap_Pixels();

float gapfill_background_color[4] = {0,0,0,1};

extern float left_gap_threshold;
extern float right_gap_threshold;

bool redraw_gapfill_window = false;

float gapfill_width = 0;
float gapfill_height = 0;

//used for blend image
float bi_gapfill_width = 0;
float bi_gapfill_height = 0;


LRESULT CALLBACK _GapFillWndProc(HWND hWnd,UINT	uMsg,WPARAM	wParam,LPARAM lParam);

bool resize_gapfill_window = true;


class GLGAPFILLWND
{
public:
	GLGAPFILLWND()
	{
		_hDC = 0;
		_hRC = 0;
		_hWnd = 0;
		_pWnd = 0;
		_screenheight = 0;
		_screenwidth = 0;
		_screenbits = 0;
		_far_plane = 100;
		_near_plane = 0.01f;
		_aspect = 1;
		xscale = 0;
		yscale = 0;
		m_zoom = 5;
		m_xpos = 0;
		m_ypos = 0;
		zoom_speed = 0.01f;
		//for blend image view
		bi_xscale = 0;
		bi_yscale = 0;
		m_bi_zoom = 5;
		m_bi_xpos = 0;
		m_bi_ypos = 0;
	}
	~GLGAPFILLWND()
	{
	}
	void ReSizeGLScene(int width, int height)
	{
		Set_Context();
		if (height==0){height=1;}
		_screenwidth = width;
		_screenheight = height;
		glViewport(0,0,width,height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(_fov,(GLfloat)width/(GLfloat)height,_near_plane,_far_plane);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Set_Perspective(_fov, (GLfloat)width/(GLfloat)height);
		vp_xpos = 0;
		vp_ypos = 0;
		vp_width = width;
		vp_height = height;
	}
	void Clear_Screen()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void Update_Screen()
	{
		SwapBuffers(_hDC);
	}
	bool InitGL(GLvoid)
	{
		glClearColor(gapfill_background_color[0],gapfill_background_color[1],gapfill_background_color[2],1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		return true;
	}
	bool KillGLWindow()
	{
		Set_Context();
		glFinish();
		if (_hRC)
		{
			if (!wglMakeCurrent(0,0)){SkinMsgBox(0, "Release Of DC And RC Failed.", 0, MB_OK);}
			if (!wglDeleteContext(_hRC)){SkinMsgBox(0, "Release Rendering Context Failed.", 0, MB_OK);}
			_hRC=0;
		}
		if (_hDC && !ReleaseDC(_hWnd,_hDC)){SkinMsgBox(0, "Release Device Context Failed.", 0, MB_OK);_hDC=0;}
		if (_hWnd && !DestroyWindow(_hWnd)){SkinMsgBox(0, "Could Not Release hWnd.", 0, MB_OK);_hWnd=0;}
		_hWnd = 0;
		_hDC = 0;
		return true;
	}
	bool Extended_GL_Window(HWND pWnd, char* title, int xpos, int ypos, int width, int height, int bits, bool fullscreen_flag)
	{
		_screenwidth = width;
		_screenheight = height;
		_screenbits = bits;
		GLuint		PixelFormat;
		WNDCLASS	wc;
		DWORD		dwExStyle;
		DWORD		dwStyle;
		_hInstance			= GetModuleHandle(0);
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc		= (WNDPROC) _GapFillWndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= _hInstance;
		wc.hIcon			= LoadIcon(0, IDI_WINLOGO);
		wc.hCursor			= LoadCursor(0, IDC_ARROW);
		wc.hbrBackground	= 0;
		wc.lpszMenuName		= 0;
		wc.lpszClassName	= "GapFillDANGL";
		if (!RegisterClass(&wc)){SkinMsgBox(0, "Failed To Register The Window Class.", 0, MB_OK);return FALSE;}
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle=WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	dwExStyle=WS_EX_APPWINDOW;
	dwStyle=WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		if (!(_hWnd=CreateWindow("GapFillDANGL","Gimpel 3D - Gap Fill Editor - Unregistered Evaluation Version",dwStyle,xpos,ypos,width,height,pWnd,0,_hInstance,0))){KillGLWindow();SkinMsgBox(0, "Window Creation Error.", 0, MB_OK);return FALSE;}
		static	PIXELFORMATDESCRIPTOR pfd={sizeof(PIXELFORMATDESCRIPTOR),1,PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,bits,0,0,0,0,0,0,0,0,0,0,0,0,0,32,1,0,PFD_MAIN_PLANE,0,0,0,0};
		if (!(_hDC=GetDC(_hWnd))){KillGLWindow();SkinMsgBox(0, "Can't Create A GL Device Context.", 0, MB_OK);return FALSE;}
		if (!(PixelFormat=ChoosePixelFormat(_hDC,&pfd)))
		{
			KillGLWindow();
			SkinMsgBox(0, "Can't Find A Suitable PixelFormat.", 0, MB_OK);
			return FALSE;
		}
		if(!SetPixelFormat(_hDC,PixelFormat,&pfd)){KillGLWindow();SkinMsgBox(0, "Can't Set The PixelFormat.", 0, MB_OK);return FALSE;}
		if (!(_hRC=wglCreateContext(_hDC))){KillGLWindow();SkinMsgBox(0, "Can't Create A GL Rendering Context.", 0, MB_OK);return FALSE;}
		glFinish();
		if(!wglMakeCurrent(_hDC,_hRC)){KillGLWindow();SkinMsgBox(0, "Can't Activate The GL Rendering Context.", 0, MB_OK);return FALSE;}
		ShowWindow(_hWnd,SW_HIDE);
		if (!InitGL()){KillGLWindow();SkinMsgBox(0, "Opengl window initialization failed.", 0, MB_OK);return FALSE;}
		Clear_Screen();
		Update_Screen();
		return true;
	}
	bool Set_Context()
	{
		//glFinish();
		if(_hRC!=wglGetCurrentContext())
		{
			wglMakeCurrent(_hDC, _hRC);
		}
		return true;
	}
	bool Init_GLWindow(HWND pWnd, int xpos, int ypos, int width, int height, int bits, bool fullscreen)
	{
		return Extended_GL_Window(pWnd, 0, xpos, ypos, width, height, bits, fullscreen);
	}
	void Set_Perspective(float fov, float aspect)
	{
		_aspect = aspect;
	}
	void Set_Viewport(int x, int y, int w, int h)
	{
		vp_xpos = x;
		vp_ypos = y;
		vp_width = w;
		vp_height = h;
	}
	int Screenwidth()
	{
		return _screenwidth;
	}
	int Screenheight()
	{
		return _screenheight;
	}
	void _Set_3D_View()
	{
		glViewport(vp_xpos,vp_ypos,vp_width,vp_height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(_fov,_aspect, _near_plane, _far_plane);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	void Set_2D_Edit_View()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float w = (float)w_width;
		float h = (float)w_height;
		glOrtho( (float)(w)/(float)(h)*-m_zoom-m_xpos, (float)(w)/(float)(h)*m_zoom-m_xpos, -m_zoom+m_ypos, m_zoom+m_ypos, -200.0f, 200.0f );
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	void Set_2D_Blend_Image_View()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float w = (float)w_width;
		float h = (float)w_height;
		glOrtho( (float)(w)/(float)(h)*-m_bi_zoom-m_bi_xpos, (float)(w)/(float)(h)*m_bi_zoom-m_bi_xpos, -m_bi_zoom+m_bi_ypos, m_bi_zoom+m_bi_ypos, -200.0f, 200.0f );
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	void Update_Scale()
	{
		RECT cr;
		GetClientRect(_hWnd, &cr );
		float cw = (float)(cr.right-cr.left);
		float ch = (float)(cr.bottom-cr.top);
		float width = cw;
		float height = ch;
		left = ((width/height)*-m_zoom)-m_xpos;
		right = ((width/height)*m_zoom)-m_xpos;
		top = -m_zoom+m_ypos;
		bottom = m_zoom+m_ypos;
		xscale = ((right-left)/width);
		yscale = ((bottom-top)/height);
		w_width = (int)width;
		w_height = (int)height;
		pixel_size = width/(right-left);

		bi_left = ((width/height)*-m_bi_zoom)-m_bi_xpos;
		bi_right = ((width/height)*m_bi_zoom)-m_bi_xpos;
		bi_top = -m_bi_zoom+m_bi_ypos;
		bi_bottom = m_bi_zoom+m_bi_ypos;
		bi_xscale = ((bi_right-bi_left)/width);
		bi_yscale = ((bi_bottom-bi_top)/height);
		bi_pixel_size = width/(bi_right-bi_left);
	}
	void Set_2D_View()
	{
		glViewport(vp_xpos,vp_ypos,vp_width,vp_height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, _screenwidth, 0, _screenheight);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
	}
	void _Set_GLWnd_Focus()
	{
		SetFocus(_hWnd);
	}
	bool _Set_Window_Size(int w, int h)
	{
		_screenwidth = w;
		_screenheight = h;
		ReSizeGLScene(w, h);
		::SetWindowPos(_hWnd, 0, 0, 0, w, h, 0);
		return true;
	}
	HDC _hDC;
	HGLRC _hRC;
	HWND _hWnd;
	HWND _pWnd;
	HINSTANCE _hInstance;
	int _screenheight;
	int _screenwidth;
	int _screenbits;
	float _far_plane;
	float _near_plane;
	float _fov;
	float _aspect;
	float pixel_size;
	int vp_xpos, vp_ypos, vp_width, vp_height;
	float xscale;
	float yscale;
	float left;
	float right;
	float top;
	float bottom;
	int w_width;
	int	w_height;
	float m_zoom;
	float m_xpos;
	float m_ypos;
	float zoom_speed;

	//for blend image view
	float bi_pixel_size;
	float bi_xscale;
	float bi_yscale;
	float bi_left;
	float bi_right;
	float bi_top;
	float bi_bottom;
	float m_bi_zoom;
	float m_bi_xpos;
	float m_bi_ypos;

};


GLGAPFILLWND *gapfillWnd = 0;

LRESULT CALLBACK _GapFillWndProc(HWND hWnd,UINT	uMsg,WPARAM	wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		{
			return 0;
		}
	case WM_KILLFOCUS:
		{
			Clear_All_GapFill_Keys();
			return 0;
		}
		
	case WM_SYSCOMMAND:
		{
			if(wParam==SC_RESTORE)
			{
				redraw_gapfill_window = true;
			}
			switch (wParam)
			{
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
				return 0;
			}
			break;
		}
		
	case WM_MOUSEMOVE:
		{
            Update_GapFill_Mouse(LOWORD(lParam), HIWORD(lParam));
			return 0;
		}
	case WM_MOUSEWHEEL:
		{
			int x = wParam;
			if(x>=0)
			{
				Update_GapFill_Mouse_Wheel(1);
			}
			else
			{
				Update_GapFill_Mouse_Wheel(-1);
			}
			redraw_gapfill_window = true;
			return 0;
		}
	case WM_LBUTTONDOWN:
		{
			::SetCapture(gapfillWnd->_hWnd);
			Update_GapFill_Key(VK_LBUTTON, true);
			return 0;
		}
		
	case WM_LBUTTONUP:
		{
			::ReleaseCapture();
			Update_GapFill_Key(VK_LBUTTON, false);
			return 0;
		}
		
	case WM_RBUTTONDOWN:
		{
			Update_GapFill_Key(VK_RBUTTON, true);
			return 0;
		}
		
	case WM_RBUTTONUP:
		{
			Update_GapFill_Key(VK_RBUTTON, false);
			return 0;
		}
		
	case WM_CLOSE:
		{
			Close_GapFill_Tools();
			return 0;
		}

	case WM_PAINT:
		{
			redraw_gapfill_window = true;
		}break;

		
	case WM_KEYDOWN:
		{
			Update_GapFill_Key(wParam, true);
			return 0;
		}
		
	case WM_KEYUP:
		{
			Update_GapFill_Key(wParam, false);
			return 0;
		}
	case WM_SYSKEYDOWN:
		{
			Update_GapFill_Key(wParam, true);
			return 0;
		}
		
	case WM_SYSKEYUP:
		{
			Update_GapFill_Key(wParam, false);
			return 0;
		}
	case WM_MOVE:
		{
			return 0;
		}
		case WM_SETFOCUS:
		{
			redraw_gapfill_window = true;
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
		}
	case WM_SIZE:
		{
			redraw_gapfill_window = true;
			resize_gapfill_window = true;
			gapfillWnd->w_width = LOWORD(lParam);
			gapfillWnd->w_height = HIWORD(lParam);
			return 0;
		}
	}
	
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

bool Get_GapFill_Pixel(int x, int y, float *px, float *py)
{
	RECT cr;
	GetClientRect(gapfillWnd->_hWnd, &cr);

	float w = (float)(cr.right-cr.left);
	float h = (float)(cr.bottom-cr.top);

	float xp = (((float)x)/((float)w));
    float yp = (((float)y)/((float)h));
	*px = (gapfillWnd->left*(1.0f-xp))+(gapfillWnd->right*xp);
	*py = ((gapfillWnd->bottom*(1.0f-yp))+(gapfillWnd->top*yp));
	return true;
}

bool Center_GapFill_View()
{
	gapfillWnd->m_zoom = gapfill_width/2;
	gapfillWnd->m_xpos = -gapfill_width/2;
	gapfillWnd->m_ypos = gapfill_height/2;
	Center_BlendImage_View();
	return true;
}

bool Center_BlendImage_View()
{
	gapfillWnd->m_bi_zoom = bi_gapfill_width/2;
	gapfillWnd->m_bi_xpos = -bi_gapfill_width/2;
	gapfillWnd->m_bi_ypos = bi_gapfill_height/2;
	return true;
}

bool Init_GapFill_Window(int x, int y, int w, int h)
{
	gapfillWnd = new GLGAPFILLWND;
	gapfillWnd->Init_GLWindow(0, x, y, w, h, 24, false);
	redraw_gapfill_window = false;
	return true;
}

bool Free_GapFill_Window()
{
	delete gapfillWnd;
	return true;
}

bool Set_GapFill_Context()
{
	gapfillWnd->Set_Context();
	return true;
}

bool Set_GapFill_Frame_Size(int w, int h)
{
	gapfill_width = (float)w;
	gapfill_height = (float)h;
	Set_GapFill_Blend_Image_Size(w, h);
	return true;
}

bool Set_GapFill_Blend_Image_Size(float zoom, float xpos, float ypos)
{
	if(gapfillWnd)
	{
		gapfillWnd->m_bi_zoom = zoom;
		gapfillWnd->m_bi_xpos = xpos;
		gapfillWnd->m_bi_ypos = ypos;
		return true;
	}
	return false;
}

bool Get_GapFill_Blend_Image_Size(float *zoom, float *xpos, float *ypos)
{
	if(gapfillWnd)
	{
		*zoom = gapfillWnd->m_bi_zoom;
		*xpos = gapfillWnd->m_bi_xpos;
		*ypos = gapfillWnd->m_bi_ypos;
		return true;
	}
	return false;
}

bool Set_GapFill_Image_Size(float zoom, float xpos, float ypos)
{
	if(gapfillWnd)
	{
		gapfillWnd->m_zoom = zoom;
		gapfillWnd->m_xpos = xpos;
		gapfillWnd->m_ypos = ypos;
		return true;
	}
	return false;
}

bool Get_GapFill_Image_Size(float *zoom, float *xpos, float *ypos)
{
	if(gapfillWnd)
	{
		*zoom = gapfillWnd->m_zoom;
		*xpos = gapfillWnd->m_xpos;
		*ypos = gapfillWnd->m_ypos;
		return true;
	}
	return false;
}



bool Set_GapFill_Blend_Image_Size(int w, int h)
{
	bi_gapfill_width = (float)w;
	bi_gapfill_height = (float)h;
	return true;
}

bool Reset_GapFill_View()
{
	gapfillWnd->m_zoom = 5;
	gapfillWnd->m_xpos = 0;
	gapfillWnd->m_ypos = 0;
	Center_GapFill_View();
	gapfillWnd->Update_Scale();
	redraw_gapfill_window = true;
	return true;
}

bool Align_GapFill_Blend_Image()
{
	gapfillWnd->m_bi_zoom = gapfillWnd->m_zoom;
	gapfillWnd->m_bi_xpos = gapfillWnd->m_xpos;
	gapfillWnd->m_bi_ypos = gapfillWnd->m_ypos;
	return true;
}

extern float euv1[2];
extern float euv2[2];
extern float euv3[2];
extern float euv4[2];

int old_gapfill_width = 0;
int old_gapfill_height = 0;

bool Set_GapFill_Fullscreen_View(int w, int h)
{
	old_gapfill_width = gapfillWnd->w_width;
	old_gapfill_height = gapfillWnd->w_height;
	gapfillWnd->ReSizeGLScene(gapfillWnd->w_width, gapfillWnd->w_height);
	gapfillWnd->Update_Scale();
	return true;
}

bool Set_GapFill_Image_View()
{
	gapfillWnd->Set_2D_Edit_View();
	return true;
}

bool Set_GapFill_Blend_Image_View()
{
	gapfillWnd->Set_2D_Blend_Image_View();
	return true;
}

bool Restore_GapFill_View()
{
	gapfillWnd->ReSizeGLScene(old_gapfill_width, old_gapfill_height);
	gapfillWnd->Update_Scale();
	return true;
}


void Clear_GapFill_Screen()
{
	gapfillWnd->Clear_Screen();
}

bool Render_GapFill_Window()
{
	gapfillWnd->Set_Context();
	
	gapfillWnd->Clear_Screen();
	if(resize_gapfill_window)
	{
		gapfillWnd->ReSizeGLScene(gapfillWnd->w_width, gapfillWnd->w_height);
		resize_gapfill_window = false;
	}


	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	gapfillWnd->Update_Scale();

	if(render_background_blend_image&&gapfill_blend_texture>-1)
	{
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, gapfill_blend_texture);
		gapfillWnd->Set_2D_Blend_Image_View();
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);glVertex2f(0, 0);
		glTexCoord2f(bi_gapfill_width,0);glVertex2f(bi_gapfill_width, 0);
		glTexCoord2f(bi_gapfill_width,bi_gapfill_height);glVertex2f(bi_gapfill_width, bi_gapfill_height);
		glTexCoord2f(0,bi_gapfill_height);glVertex2f(0, bi_gapfill_height);
		glEnd();
	}


	gapfillWnd->Set_2D_Edit_View();
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, gapfill_texture);

	glEnable(GL_ALPHA_TEST);
	if(view_gapfill_left)
	{
		glAlphaFunc(GL_GEQUAL,left_gap_threshold);
	}
	else if(view_gapfill_right)
	{
		glAlphaFunc(GL_GEQUAL,right_gap_threshold);
	}
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);glVertex2f(0, 0);
	glTexCoord2f(gapfill_width,0);glVertex2f(gapfill_width, 0);
	glTexCoord2f(gapfill_width,gapfill_height);glVertex2f(gapfill_width, gapfill_height);
	glTexCoord2f(0,gapfill_height);glVertex2f(0, gapfill_height);
	glEnd();

	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	Render_Gap_Pixels();
	glDisable(GL_ALPHA_TEST);

	if(render_ghost_blend_image&&gapfill_blend_texture>-1)
	{
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, gapfill_blend_texture);
		glEnable(GL_BLEND);
		glColor4f(1,1,1,0.5f);
		gapfillWnd->Set_2D_Blend_Image_View();
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);glVertex2f(0, 0);
		glTexCoord2f(bi_gapfill_width,0);glVertex2f(bi_gapfill_width, 0);
		glTexCoord2f(bi_gapfill_width,bi_gapfill_height);glVertex2f(bi_gapfill_width, bi_gapfill_height);
		glTexCoord2f(0,bi_gapfill_height);glVertex2f(0, bi_gapfill_height);
		glEnd();
		glColor4f(1,1,1,1);
		glDisable(GL_BLEND);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
	}

	glDisable(GL_TEXTURE_RECTANGLE_ARB);

	gapfillWnd->Set_2D_Edit_View();
	glColor3f(0,0,0);
	glBegin(GL_LINES);
	glVertex2f(0,0);
	glVertex2f(gapfill_width, gapfill_height);
	glVertex2f(gapfill_width,0);
	glVertex2f(0, gapfill_height);
	glVertex2f(gapfill_width/2,0);
	glVertex2f(0, gapfill_height/2);
	glVertex2f(gapfill_width/2,0);
	glVertex2f(gapfill_width, gapfill_height/2);
	glVertex2f(gapfill_width/2,gapfill_height);
	glVertex2f(0, gapfill_height/2);
	glVertex2f(gapfill_width/2,gapfill_height);
	glVertex2f(gapfill_width, gapfill_height/2);
	glEnd();
	glColor3f(1,1,1);
	
	gapfillWnd->Set_2D_View();

	Print_GLText_Items();
	gapfillWnd->Update_Screen();
	return true;
}


bool Drag_GapFill_Blend_Image(int x, int y)
{
	gapfillWnd->m_bi_xpos += gapfillWnd->bi_xscale*x;
	gapfillWnd->m_bi_ypos += gapfillWnd->bi_yscale*y;
	redraw_gapfill_window = true;
	return true;
}

bool Drag_GapFill_Image(int x, int y)
{
	gapfillWnd->m_xpos += gapfillWnd->xscale*x;
	gapfillWnd->m_ypos += gapfillWnd->yscale*y;
	redraw_gapfill_window = true;
	Drag_GapFill_Blend_Image(x, y);
	return true;
}

bool Scale_GapFill_Blend_Image(int n)
{
	gapfillWnd->m_bi_zoom += (gapfillWnd->m_bi_zoom*gapfillWnd->zoom_speed)*n;
	if(gapfillWnd->m_bi_zoom<=0.05f)
	{
		gapfillWnd->m_bi_zoom = 0.05f;
	}
	if(gapfillWnd->m_bi_zoom>=bi_gapfill_width)
	{
		gapfillWnd->m_bi_zoom = bi_gapfill_width;
	}
	redraw_gapfill_window = true;
	gapfillWnd->Update_Scale();
	return true;
}

bool Scale_GapFill_Image(int n)
{
	gapfillWnd->m_zoom += (gapfillWnd->m_zoom*gapfillWnd->zoom_speed)*n;
	if(gapfillWnd->m_zoom<=0.05f)
	{
		gapfillWnd->m_zoom = 0.05f;
	}
	if(gapfillWnd->m_zoom>=gapfill_width)
	{
		gapfillWnd->m_zoom = gapfill_width;
	}
	redraw_gapfill_window = true;
	Scale_GapFill_Blend_Image(n);
	return true;
}




HDC GapFill_HDC()
{
	return gapfillWnd->_hDC;
}

HWND Get_GapFill_Window()
{
	if(gapfillWnd)
	{
		return gapfillWnd->_hWnd;
	}
	return 0;
}

HGLRC GapFill_HRC()
{
	return gapfillWnd->_hRC;
}

float GapFill_Pixel_Size()
{
	return gapfillWnd->pixel_size;
}

bool Show_GapFill_View(bool b)
{
	if(b)
	{
		Fit_GapFill_Window();
		ShowWindow(gapfillWnd->_hWnd, SW_SHOW);
		redraw_gapfill_window = true;
	}
	else
	{
		ShowWindow(gapfillWnd->_hWnd, SW_HIDE);
		redraw_gapfill_window = false;
	}
	return true;
}
