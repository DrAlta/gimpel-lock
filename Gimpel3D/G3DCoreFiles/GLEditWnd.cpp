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
#include "GLWnd.h"
#include "Frame.h"
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>
#include <stdio.h>
#include "Controls.h"
#include "EditOps.h"
#include "ControlMesh.h"
#include "GLText.h"
#include "Console.h"
#include "AutoSegment.h"
#include "../PerspectiveTool.h"
#include "../AnchorPoints.h"
#include "../FeaturePoints.h"
#include "../LinkPointsTool.h"
#include "../Skin.h"

LRESULT CALLBACK _EditWndProc(HWND hWnd,UINT	uMsg,WPARAM	wParam,LPARAM lParam);

bool Report_Render2D_Event();

extern bool render_halo_tool;
bool Render_Halo_Tool_2D();


extern bool render_2d_autoalignment_points;
extern bool activate_alignment_points;
extern bool deactivate_alignment_points;

void Render_2D_AutoAlignment_Points();

bool Render_Tracking_Info();

bool Render_Edit_Brush();
bool Render_Axis_Lines();

extern bool render_2d_relief_layer_split;
void Render_2D_Relief_Layer_Split();

bool redraw_edit_window = false;

bool resize_edit_window = true;

extern bool render_brush;
extern bool render_crosshairs;

bool show_mouseover_info = true;

extern bool render_paint_tool;
extern int paint_grayscale_texture;
extern int paint_layermask_texture;
extern bool render_paint_frame_image;
extern bool render_paint_grayscale_image;
extern bool render_paint_layermask_image;


float edit_background_color[4] = {0,0,0,1};

float edit_width = -1;
float edit_height = -1;

extern int width_border, height_border;

bool Clear_All_Edit_Keys();

float xscale = 0;
float yscale = 0;

float   left;
float   right;
float   top;
float   bottom;
int  w_width;
int  w_height;
		float m_zoom = 5;
		float m_xpos = 0;
		float m_ypos = 0;
float zoom_speed = 0.01f;

class GLEDITWND
{
public:
	GLEDITWND()
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
	}
	~GLEDITWND()
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
		glClearColor(0,0,0,0);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		return true;
	}
	bool KillGLWindow()
	{
		Set_Context();
		if (_hRC)
		{
			glFinish();
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
		wc.lpfnWndProc		= (WNDPROC) _EditWndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= _hInstance;
		wc.hIcon			= LoadIcon(0, IDI_WINLOGO);
		wc.hCursor			= LoadCursor(0, IDC_ARROW);
		wc.hbrBackground	= 0;
		wc.lpszMenuName		= 0;
		wc.lpszClassName	= "EditDANGL";
		if (!RegisterClass(&wc)){SkinMsgBox(0, "Failed To Register The Window Class.", 0, MB_OK);return FALSE;}
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle=WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		if (!(_hWnd=CreateWindow("EditDANGL","Stereo 3D - Frame Editor - Unregistered Evaluation Version",dwStyle,xpos,ypos,width,height,pWnd,0,_hInstance,0))){KillGLWindow();SkinMsgBox(0, "Window Creation Error.", 0, MB_OK);return FALSE;}
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
		glFinish();
		wglMakeCurrent(_hDC, _hRC);
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
		float w = w_width;
		float h = w_height;
		glOrtho( (float)(w)/(float)(h)*-m_zoom-m_xpos, (float)(w)/(float)(h)*m_zoom-m_xpos, -m_zoom+m_ypos, m_zoom+m_ypos, -200.0f, 200.0f );
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	void Update_Scale()
	{
	RECT cr;

	GetClientRect(_hWnd, &cr );
  float cw = cr.right-cr.left;
  float ch = cr.bottom-cr.top;
  float width = cw;
  float height = ch;
  left = ((width/height)*-m_zoom)-m_xpos;
  right = ((width/height)*m_zoom)-m_xpos;
  top = -m_zoom+m_ypos;
  bottom = m_zoom+m_ypos;
  xscale = ((right-left)/width);
  yscale = ((bottom-top)/height);
  w_width = width;
  w_height = height;
  pixel_size = width/(right-left);
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
	void Set_GLWnd_Focus()
	{
		SetFocus(_hWnd);
	}
	bool Set_Window_Size(int w, int h)
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
};


GLEDITWND *editWnd = 0;

LRESULT CALLBACK _EditWndProc(HWND hWnd,UINT	uMsg,WPARAM	wParam,LPARAM lParam)
{
	POINT cursor_pos;//incase we get a right-click when no previous mousemove set current pos
	switch (uMsg)
	{
	case WM_ACTIVATE:
		{
			return 0;
		}
	case WM_KILLFOCUS:
		{
			Clear_All_Edit_Keys();
			return 0;
		}
		
	case WM_SYSCOMMAND:
		{
			if(wParam==SC_RESTORE)
			{
				redraw_frame = true;
				redraw_edit_window = true;
			}
			else
			{
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
            Update_Edit_Mouse(LOWORD(lParam), HIWORD(lParam));
			redraw_edit_window = true;
			return 0;
		}
	case WM_MOUSEWHEEL:
		{
			int x = wParam;
			if(x>=0)
			{
				Update_Edit_Mouse_Wheel(1);
			}
			else
			{
				Update_Edit_Mouse_Wheel(-1);
			}
			redraw_edit_window = true;
			return 0;
		}
	case WM_LBUTTONDOWN:
		{
			::SetCapture(editWnd->_hWnd);
			Update_Edit_Key(VK_LBUTTON, true);
			return 0;
		}
		
	case WM_LBUTTONUP:
		{
			::ReleaseCapture();
			Update_Edit_Key(VK_LBUTTON, false);
			return 0;
		}
		
	case WM_RBUTTONDOWN:
		{
			//selection might be fuct if mousemove isn't current (ie. occluded window)
			//simulate we got a mouse position
			GetCursorPos(&cursor_pos);
			ScreenToClient(hWnd, &cursor_pos);
            Update_Edit_Mouse(cursor_pos.x, cursor_pos.y);
			Update_Edit_Key(VK_RBUTTON, true);
			return 0;
		}
		
	case WM_RBUTTONUP:
		{
			Update_Edit_Key(VK_RBUTTON, false);
			return 0;
		}
		
	case WM_MBUTTONDOWN:
		{
			Update_Edit_Key(VK_MBUTTON, true);
			return 0;
		}
		
	case WM_MBUTTONUP:
		{
			Update_Edit_Key(VK_MBUTTON, false);
			return 0;
		}
	case WM_PAINT:
		{
			redraw_edit_window = true;
		}break;

		
	case WM_KEYDOWN:
		{
			Update_Edit_Key(wParam, true);
			return 0;
		}
		
	case WM_KEYUP:
		{
			Update_Edit_Key(wParam, false);
			return 0;
		}
	case WM_SYSKEYDOWN:
		{
			Update_Edit_Key(wParam, true);
			return 0;
		}
		
	case WM_SYSKEYUP:
		{
			Update_Edit_Key(wParam, false);
			return 0;
		}
	case WM_MOVE:
		{
			return 0;
		}
		case WM_SETFOCUS:
		{
			redraw_edit_window = true;
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
		}
	case WM_SIZE:
		{
			redraw_edit_window = true;
			resize_edit_window = true;
			editWnd->vp_width = LOWORD(lParam);
			editWnd->vp_height = HIWORD(lParam);
			return 0;
		}
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


bool Center_Edit_View()
{
	m_zoom = edit_width/2;
	m_xpos = -edit_width/2;
	m_ypos = edit_height/2;
	return true;
}

bool Init_Edit_Window(int x, int y, int w, int h)
{
	editWnd = new GLEDITWND;
	editWnd->Init_GLWindow(0, x, y, w, h, 24, false);
	editWnd->Set_2D_Edit_View();
	redraw_edit_window = true;
	return true;
}

bool Free_Edit_Window()
{
	delete editWnd;
	return true;
}

bool Set_Edit_Context()
{
	editWnd->Set_Context();
	return true;
}

extern int frame_texture;

int edit_texture = -1;

bool Set_Edit_Frame_Size(int w, int h)
{
	edit_width = (float)w;
	edit_height = (float)h;
	editWnd->Set_2D_Edit_View();
	return true;
}

bool Reset_Edit_View()
{
	m_zoom = 5;
	m_xpos = 0;
	m_ypos = 0;
	Center_Edit_View();
	editWnd->Update_Scale();
	return true;
}

float euv1[2] = {0,0};
float euv2[2] = {1,0};
float euv3[2] = {1,1};
float euv4[2] = {0,1};

//fixthis it is not used, eliminate variable vertices, use static
bool Flip_Edit_Image_Horizontal()
{
	if(euv1[1]==0)euv1[1]=1;else euv1[1]=0;
	if(euv2[1]==0)euv2[1]=1;else euv2[1]=0;
	if(euv3[1]==0)euv3[1]=1;else euv3[1]=0;
	if(euv4[1]==0)euv4[1]=1;else euv4[1]=0;
	return true;
}

bool Flip_Edit_Image_Vertical()
{
	return true;
}

void Render_Edit_Overlay();
bool Render_Layers_2D(float point_size, int lx, int hx, int ly, int hy);

extern float last_edit_px;
extern float last_edit_py;

extern bool edge_detection_enabled;

bool Update_Edit_Window_Text()
{
	int sx = 10;
	int sy = editWnd->Screenheight()-30;
	int spacing = 20;
	float red[4] = {1,0,0,1};
	float green[4] = {0,1,0,1};
	float blue[4] = {0,0,1,1};
	float white[4] = {1,1,1,0.5f};
	Set_GLPrint_Pos(sx, sy);
	Set_GLText_Color(red);
	Print_GLText("Mouse X: %i", (int)last_edit_px);sy-=spacing;Set_GLPrint_Pos(sx, sy);
	Print_GLText("Mouse Y: %i", (int)last_edit_py);sy-=spacing;Set_GLPrint_Pos(sx, sy);
	if(frame)
	{
		char layer_name[512];
		int ix = (int)last_edit_px;
		int iy = (int)last_edit_py;
		if(frame->IsValidPixel(ix, iy))
		{
			int id = frame->Get_Pixel_Layer(ix, iy);
			int index = Get_Layer_Index(id);
			if(index!=-1)
			{
				Layer_Name(index, layer_name);
				Print_GLText("Layer: %s", layer_name);sy-=spacing;Set_GLPrint_Pos(sx, sy);
				Print_GLText("Layer Index: %i", Get_Layer_Index(frame->Get_Pixel_Layer(ix, iy)));sy-=spacing;Set_GLPrint_Pos(sx, sy);
				Print_GLText("Layer ID: %i", frame->Get_Pixel_Layer(ix, iy));sy-=spacing;Set_GLPrint_Pos(sx, sy);
			}
			else
			{
				Print_GLText("Layer: INVALID LAYER ID");sy-=spacing;Set_GLPrint_Pos(sx, sy);
			}
		}
		else
		{
			Print_GLText("Layer: ---");sy-=spacing;Set_GLPrint_Pos(sx, sy);
		}
	}
	return true;
}

void Clear_Edit_Screen()
{
	editWnd->Clear_Screen();
}


void Render_Paint_Image(int texture)
{
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);
	glBegin(GL_QUADS);
	float xs = frame->width;
	float ys = frame->height;
	float xd = 1.0f/frame->width;
	float yd = 1.0f/frame->height;
	glTexCoord2f(0,0);glVertex2f(0, 0);
	glTexCoord2f(frame->width,0);glVertex2f(xs, 0);
	glTexCoord2f(frame->width,frame->height);glVertex2f(xs, ys);
	glTexCoord2f(0,frame->height);glVertex2f(0, ys);
	glEnd();
}

bool Render_Paint_Tool()
{
	editWnd->Clear_Screen();
	if(resize_edit_window)
	{
		editWnd->ReSizeGLScene(editWnd->vp_width, editWnd->vp_height);
		resize_edit_window = false;
	}
	if(frame)
	{
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		editWnd->Update_Scale();
		editWnd->Set_2D_Edit_View();

		//figure out some reasonable order for these combinations
		if(render_paint_frame_image)
		{
			Render_Paint_Image(frame_texture);
			if(render_paint_grayscale_image)
			{
				glEnable(GL_BLEND);
				glColor4f(1,1,1,0.75f);
				Render_Paint_Image(paint_grayscale_texture);
				if(render_paint_layermask_image)
				{
					glColor4f(1,1,1,0.25f);
					Render_Paint_Image(paint_layermask_texture);
				}
				glDisable(GL_BLEND);
			}
			else if(render_paint_layermask_image)
			{
				glEnable(GL_BLEND);
				glColor4f(1,1,1,0.5f);
				Render_Paint_Image(paint_layermask_texture);
				glDisable(GL_BLEND);
			}
		}
		else if(render_paint_grayscale_image)
		{
			Render_Paint_Image(paint_grayscale_texture);
			if(render_paint_layermask_image)
			{
				glEnable(GL_BLEND);
				glColor4f(1,1,1,0.25f);
				Render_Paint_Image(paint_layermask_texture);
				glDisable(GL_BLEND);
			}
		}
		else if(render_paint_layermask_image)
		{
			Render_Paint_Image(paint_layermask_texture);
		}
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
	}
	if(render_brush)Render_Edit_Brush();
	if(render_crosshairs)Render_Axis_Lines();
	editWnd->Set_2D_View();
	if(show_mouseover_info&&frame)
	{
		Update_Edit_Window_Text();
	}
	Print_GLText_Items();
	editWnd->Update_Screen();
	return true;
}

bool Render_Edit_Window()
{
	editWnd->Clear_Screen();

	if(resize_edit_window)
	{
		editWnd->ReSizeGLScene(editWnd->vp_width, editWnd->vp_height);
		resize_edit_window = false;
	}

	if(frame)
	{
		editWnd->Update_Scale();
		editWnd->Set_2D_Edit_View();

		Report_Render2D_Event();
		
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		if(edit_texture==-1)
		{
			edit_texture = frame_texture;
		}

		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, edit_texture);
		glBegin(GL_QUADS);
		float xs = frame->width;
		float ys = frame->height;

		glTexCoord2f(0,0);glVertex2f(0, 0);
		glTexCoord2f(frame->width,0);glVertex2f(xs, 0);
		glTexCoord2f(frame->width,frame->height);glVertex2f(xs, ys);
		glTexCoord2f(0,frame->height);glVertex2f(0, ys);
		glEnd();
		glDisable(GL_TEXTURE_RECTANGLE_ARB);

		//insert before overlay lines from layers
		//blended texture MUST match alphas perfectly
	if(render_halo_tool)
	{
		Render_Halo_Tool_2D();
	}

		Render_Layers_2D(editWnd->pixel_size/3, left, right, top, bottom);
		Report_Render2D_Event();
	}

	Render_Tracking_Info();
	
	if(render_brush)Render_Edit_Brush();
	if(render_crosshairs)Render_Axis_Lines();

	if(activate_alignment_points||deactivate_alignment_points)
	{
		if(!render_brush)Render_Edit_Brush();
		if(!render_crosshairs)Render_Axis_Lines();
	}

	Render_Edit_Overlay();

	if(render_2d_relief_layer_split)
	{
		Render_2D_Relief_Layer_Split();
	}

	if(render_autosegment_state)
	{
		Render_AutoSegment_Outlines();
	}

	if(render_perspective_tool)
	{
		Render_Perspective_Tool();
	}

	if(render_anchor_points_2d)
	{
		Render_Anchor_Points_2D();
	}

	if(render_feature_points_2d)
	{
		Render_Feature_Points_2D();
	}

	if(render_2d_autoalignment_points)
	{
		Render_2D_AutoAlignment_Points();
	}

	editWnd->Set_2D_View();
	
	if(show_mouseover_info&&frame)
	{
		Update_Edit_Window_Text();
	}
	
	Print_GLText_Items();
	editWnd->Update_Screen();
	return true;
}


bool Get_Edit_Pixel(int x, int y, float *px, float *py)
{
	if(!frame)
	{
		*px = 0;
		*py = 0;
		return false;
	}
	RECT cr;
	GetClientRect(editWnd->_hWnd, &cr);

	float w = cr.right-cr.left;
	float h = cr.bottom-cr.top;

	float xp = (((float)x)/((float)w));
    float yp = (((float)y)/((float)h));
	*px = (left*(1.0f-xp))+(right*xp);
	*py = ((bottom*(1.0f-yp))+(top*yp));
	return true;
}

bool Get_Edit_World_Pos(int x, int y, float *px, float *py)
{
	Get_Edit_Pixel(x, y, px, py);
	return true;
}

bool Drag_Edit_Image(int x, int y)
{
	m_xpos += xscale*x;
	m_ypos += yscale*y;
	return true;
}


bool Scale_Edit_Image(int n)
{
	m_zoom += (m_zoom*zoom_speed)*n;
	if(m_zoom<=0.05f)
	{
		m_zoom = 0.05f;
	}
	if(m_zoom>=edit_width)
	{
		m_zoom = edit_width;
	}
	redraw_edit_window = true;
	editWnd->Update_Scale();
	return true;
}


bool Set_Edit_Window_Cursor(int x, int y)
{
	RECT rect;
	GetWindowRect(editWnd->_hWnd, &rect);
	RECT rect2;
	GetClientRect(editWnd->_hWnd,&rect2);
	int yborder = editWnd->Screenheight()-(rect.bottom-rect.top);
	int xborder = editWnd->Screenwidth()-(rect.right-rect.left);
	xborder = 4;
	yborder += 4;
	SetCursorPos((x+rect.left)+xborder, (y+rect.top)-yborder);
	return true;
}

HDC Edit_HDC()
{
	return editWnd->_hDC;
}

HWND Edit_HWND()
{
	if(editWnd)
	{
		return editWnd->_hWnd;
	}
	return 0;
}

HGLRC Edit_HRC()
{
	return editWnd->_hRC;
}

float Pixel_Size()
{
	return editWnd->pixel_size;
}

int Edit_Window_Width()
{
	return editWnd->_screenwidth;
}

int Edit_Window_Height()
{
	return editWnd->_screenheight;
}

bool Get_Current_Edit_View(float *xpos, float *ypos, float *zoom)
{
	*zoom = m_zoom;
	*xpos = m_xpos;
	*ypos = m_ypos;
	return true;
}

bool Set_Current_Edit_View(float xpos, float ypos, float zoom)
{
	m_zoom = zoom;
	m_xpos = xpos;
	m_ypos = ypos;
	editWnd->Update_Scale();
	redraw_edit_window = true;
	return true;
}

bool Get_Pixel_At_Screen_Position(int sx, int sy, float *px, float *py)
{
	if(!frame)
	{
		*px = 0;
		*py = 0;
		return false;
	}
	RECT cr;
	GetClientRect(editWnd->_hWnd, &cr);

	float w = cr.right-cr.left;
	float h = cr.bottom-cr.top;

	float xp = (((float)sx)/((float)w));
    float yp = (((float)sy)/((float)h));
	*px = (left*(1.0f-xp))+(right*xp);
	*py = ((bottom*(1.0f-yp))+(top*yp));
	return true;
}
