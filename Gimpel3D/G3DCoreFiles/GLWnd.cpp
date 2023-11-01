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
#include "Console.h"
#include "../Skin.h"

extern bool redraw_edit_window;

extern bool enable_window_controls;

extern bool render_sorted_layers;

void Enable_All_Borders(bool b);

HDC _hDC = 0;
HGLRC _hRC;
HWND _hWnd = 0;
HWND _pWnd = 0;
HINSTANCE _hInstance;
POINT _mc;

bool redraw_frame = false;

void Mouse_Hover();

float background_color[4] = {0.4f,0.4f,0.4f,1};

PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
PFNGLMULTITEXCOORD3FARBPROC glMultiTexCoord3fARB;
PFNGLMULTITEXCOORD4FARBPROC glMultiTexCoord4fARB;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
PFNGLACTIVESTENCILFACEEXTPROC glActiveStencilFace;
GLint maxTexelUnits = 1;

int _screenheight = 0;
int _screenwidth = 0;
int _screenbits = 0;

//in feet
float _far_plane = 1000;
float _near_plane = 0.001f;
float _fov = 45;
float _aspect = 1;

int width_border = 0;
int height_border = 0;
int window_xpos = 0;
int window_ypos = 0;

int vp_xpos, vp_ypos, vp_width, vp_height;

bool stereo_mode = false;
bool anaglyph_mode = false;

bool running_fullscreen = false;

void Exit_Fullscreen();


LRESULT CALLBACK _WndProc(HWND hWnd,UINT	uMsg,WPARAM	wParam,LPARAM lParam);


// Always Check For Extension-Availability During Run-Time!
// Here We Go!
bool isInString(char *string, const char *search) {
	int pos=0;
	int maxpos=strlen(search)-1;
	int len=strlen(string);	
	char *other;
	for (int i=0; i<len; i++)
	{
		if ((i==0) || ((i>1) && string[i-1]=='\n'))
		{				// New Extension Begins Here!
			other=&string[i];			
			pos=0;													// Begin New Search
			while (string[i]!='\n') {								// Search Whole Extension-String
				if (string[i]==search[pos]) pos++;					// Next Position
				if ((pos>maxpos) && string[i+1]=='\n') return true; // We Have A Winner!
				i++;
			}			
		}
	}	
	return false;													// Sorry, Not Found!
}

bool initExtensions(void)
{
	char *extensions;	
	extensions=_strdup((char *) glGetString(GL_EXTENSIONS));			// Fetch Extension String
	if(!extensions)
	{
		SkinMsgBox(0, "Error getting gl extensions", 0, MB_OK);
		return false;
	}
	int len=strlen(extensions);
	for (int i=0; i<len; i++)										// Separate It By Newline Instead Of Blank
		if (extensions[i]==' ') extensions[i]='\n';
	if (isInString(extensions,"GL_ARB_multitexture")				// Is Multitexturing Supported?
		&& isInString(extensions,"GL_EXT_texture_env_combine"))		// Is texture_env_combining Supported?
	{	
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&maxTexelUnits);
		glMultiTexCoord1fARB	= (PFNGLMULTITEXCOORD1FARBPROC)		wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB	= (PFNGLMULTITEXCOORD3FARBPROC)		wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB	= (PFNGLMULTITEXCOORD4FARBPROC)		wglGetProcAddress("glMultiTexCoord4fARB");
		glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC)	wglGetProcAddress("glClientActiveTextureARB");		
		glActiveStencilFace = (PFNGLACTIVESTENCILFACEEXTPROC)wglGetProcAddress("glActiveStencilFaceEXT");
		return true;
	}
	SkinMsgBox(0, "Multitexture not supported!", 0, MB_OK);
	return false;
}

void ReSizeGLScene(int width, int height)
{
	Set_GLContext();
	if (height==0){height=1;}
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

bool delayedResizeGLScene = false;

void DelayedReSizeGLScene()
{
	delayedResizeGLScene = false;
	ReSizeGLScene(_screenwidth, _screenheight);
}


void Set_Projection_Dlg_FOV(float fov);
bool ReApply_Frame_Transform();

//NOTE: ONLY used for sequences, single frames directlset fov values and just set a single camera transform
//no need for single frames to be resetting view, etc.

//this can also be called manually from the GUI

void Set_Fov(float fov)
{
	_fov = aperture = DEFAULT_FOV = fov;
	Get_Flat_Projection_Vectors();
	if(frame)
	{
		ReApply_Frame_Transform();
		Project_Layers();
	}
	Set_Projection_Dlg_FOV(fov);
	ReSizeGLScene(vp_width, vp_height);
	redraw_frame = true;
}

void ReSizeGLScene(int width, int height, float fov)
{
	Set_GLContext();
	if (height==0){height=1;}
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,(GLfloat)width/(GLfloat)height,_near_plane,_far_plane);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDisable(GL_DITHER);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearStencil(0);
	glDisable(GL_COLOR_MATERIAL);
	glClearColor(background_color[0],background_color[1],background_color[2],1);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	return true;
}

bool KillGLWindow()
{
	Set_GLContext();
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
	wc.lpfnWndProc		= (WNDPROC) _WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= _hInstance;
	wc.hIcon			= LoadIcon(0, IDI_WINLOGO);
	wc.hCursor			= LoadCursor(0, IDC_ARROW);
	wc.hbrBackground	= 0;
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= "DANGL";
	if (!RegisterClass(&wc)){SkinMsgBox(0, "Failed To Register The Window Class.", 0, MB_OK);return FALSE;}
	
	dwExStyle=WS_EX_APPWINDOW;
	dwStyle=WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!(_hWnd=CreateWindow("DANGL","Gimpel3D - 3D View",dwStyle,xpos,ypos,width,height,pWnd,0,_hInstance,0))){KillGLWindow();SkinMsgBox(0, "Window Creation Error.", 0, MB_OK);return FALSE;}
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
	ReSizeGLScene(width, height);
	if (!InitGL()){KillGLWindow();SkinMsgBox(0, "Opengl window initialization failed.", 0, MB_OK);return FALSE;}
	Clear_Screen();
	Update_Screen();
	_hRC = wglGetCurrentContext();
	return true;
}

bool Set_GLContext()
{
//	glFinish();
		if(glGetError()==1281)
		{
			exit(0);
		}
	//glFlush();
	//	if(glGetError()==1281)
	//	{
	//		exit(0);
	//	}
	if(_hRC != wglGetCurrentContext())
	{
		wglMakeCurrent(NULL, NULL);
		if(glGetError()==1281)
		{
			exit(0);
		}
		int res = wglMakeCurrent(_hDC, _hRC);
		if(res!=TRUE)
		{
			exit(0);
		}
	}
		if(glGetError()==1281)
		{
			//ok, at this point, wglMakeCurrent has:
			//(a) - reported success
			//(b) - set the gl error flag to GL_INVALID_ARGUMENT
			//and most importantly
			//(c) - no actually set the context correct;y because that is apparent from the BUG that I am chasing down which leads me here..

			//SO, apparently wglMakeCurrent is "working" because it reports success, but also "failing" because it sets an error flag
			//and "flaky" because it reports success, flags an error, and Creates A State In WHich Later Bugs WIll Happen Downstream..

			//So, rinse and repeat

			bool retry = true;
			int max_its = 9999;
			int its = 0;

			while(retry)
			{
				//try setting the context
				int res = wglMakeCurrent(_hDC, _hRC);
				if(res!=TRUE)
				{
					//ok, setting the context failed, something is wrong..
					exit(0);
				}
				//whoop de mother fucking do, the context was set successfully..
				//so there should be no errors
				if(glGetError()==1281)
				{
					//wait a minute, there seems to be an erro flag for GL_INVALID_ARGUMENT..
					//APPARENTLY, the return value from wglMakeCurrent Was Incorrect, and this Will Lead To A Crash later on..

					//SO, since wglMakeCurrent THINKS it worked, let's give it another shot..
					retry = true;
					its++;
					if(its==max_its)
					{
						retry = false;
					}
				}
				else
				{
					//hey, looks like it actually worked..
					retry = false;

					//TO BE CLEAR - WHAT HAPPENED WHEN IT REACHRES THIS POINT IN THE CODE
					//opengl error state was fine, no error..
					//wglMakeCurrent was called and it reported success BUT set the error flag
					//the next time it was called, it also reported success but did NOT set an error flag..
				}
			}

			if(its==max_its)
			{
				//ok, that is "max_its" times that the wglMakeCurrent reported success when it actually failed..
				//when the fuck else would the driver like this function to be called?
				exit(0);
			}


			//ok we had to call wglMakeCurrent more than once because it (a) "works" and "fails" the first time (according to return value
			//and opengl error state), AND there is actually a legitimate bug caused by this error condition..later down the road..

			//exit(0);so don't exit, see if it actually makes a dman bit of difference later on when we actually, um, NEED the opengl
			//context set to a specific window, which is why things like wglMakeCurrent exist..
		}
	return true;
}


LRESULT CALLBACK _WndProc(HWND hWnd,UINT	uMsg,WPARAM	wParam,LPARAM lParam)
{
	POINT cursor_pos;//incase we get a right-click when no previous mousemove set current pos
	switch (uMsg)
	{
	case WM_ACTIVATE:
		{
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
			if(enable_window_controls)
			{
				Update_Mouse(LOWORD(lParam), HIWORD(lParam));
			}
			return 0;
		}
	case WM_LBUTTONDOWN:
		{
			if(enable_window_controls)
			{
				Update_Key(VK_LBUTTON, true);
			}
			return 0;
		}
		
	case WM_LBUTTONUP:
		{
			if(enable_window_controls)
			{
				Update_Key(VK_LBUTTON, false);
			}
			return 0;
		}
		
	case WM_RBUTTONDOWN:
		{
			if(enable_window_controls)
			{
				//selection might be fuct if mousemove isn't current (ie. occluded window)
				//simulate we got a mouse position
				GetCursorPos(&cursor_pos);
				ScreenToClient(hWnd, &cursor_pos);
				Update_Mouse(cursor_pos.x, cursor_pos.y);
				Update_Key(VK_RBUTTON, true);
			}
			return 0;
		}
		
	case WM_RBUTTONUP:
		{
			if(enable_window_controls)
			{
				Update_Key(VK_RBUTTON, false);
			}
			return 0;
		}
	case WM_MBUTTONDOWN:
		{
			if(enable_window_controls)
			{
				Update_Key(VK_MBUTTON, true);
			}
			return 0;
		}
	case WM_MBUTTONUP:
		{
			if(enable_window_controls)
			{
				Update_Key(VK_MBUTTON, false);
			}
			return 0;
		}
		
	case WM_KILLFOCUS:
		{
			Reset_Controls();
			break;
		}
//FIXTHIS should never occur
	case WM_CLOSE:
		{
			Quit();
			return 0;
		}
	case WM_PAINT:
		{
			redraw_frame = true;
		}break;
	
	case WM_KEYDOWN:
		{
			if(enable_window_controls)
			{
				Update_Key(wParam, true);
			}
			return 0;
		}
		
	case WM_KEYUP:
		{
			if(enable_window_controls)
			{
				Update_Key(wParam, false);
			}
			return 0;
		}
	case WM_SYSKEYDOWN:
		{
			if(enable_window_controls)
			{
				Update_Key(wParam, true);
			}
			return 0;
		}
		
	case WM_SYSKEYUP:
		{
			if(enable_window_controls)
			{
				Update_Key(wParam, false);
			}
			return 0;
		}
		case WM_MOVE:
		{
			RECT rect;
			GetWindowRect(_hWnd, &rect);
			window_xpos = rect.left;
			window_ypos = rect.top;
			return 0;
		}
		case WM_SETFOCUS:
		{
			redraw_frame = true;
			return DefWindowProc(hWnd,uMsg,wParam,lParam);
		}
		case WM_SIZE:
		{
			delayedResizeGLScene = true;
			//ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));
			_screenwidth = LOWORD(lParam);
			_screenheight = HIWORD(lParam);
			redraw_frame = true;
			return 0;
		}
	}
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}



bool Init_GLWindow(HWND pWnd, int xpos, int ypos, int width, int height, int bits, bool fullscreen)
{
	Extended_GL_Window(pWnd, 0, xpos, ypos, width, height, bits, fullscreen);
	//fix for "real" width and height of window thanks MS
	RECT rect;
	GetClientRect(_hWnd,&rect);
	width_border = _screenwidth-(rect.right-rect.left);
	height_border = _screenheight-(rect.bottom-rect.top);
	GetWindowRect(_hWnd, &rect);
	window_xpos = rect.left;
	window_ypos = rect.top;
	return true;
}

void Set_Perspective(float fov, float aspect)
{
	_fov = fov;
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

void Set_3D_View()
{
	glViewport(vp_xpos,vp_ypos,vp_width,vp_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(_fov,_aspect, _near_plane, _far_plane);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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

bool Set_Window_Pos(int x, int y)
{
	::SetWindowPos(_hWnd, 0, x, y, 0, 0, SWP_NOSIZE);
	return true;
}

bool Set_Anaglyph_Mode(bool b)
{
	Set_GLContext();
	anaglyph_mode = b;
	if(b)
	{
		glClearColor(0,0,0,0);
	}
	else
	{
		glClearColor(background_color[0],background_color[1],background_color[2],1);
	}
	redraw_frame = true;
	return true;
}

void Set_Screen(int width, int height, int bits)
{
	printf("setting screen to %i %i %i\n", width, height, bits);
	DEVMODE dmScreenSettings;
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
	dmScreenSettings.dmSize=sizeof(dmScreenSettings);
	dmScreenSettings.dmPelsWidth = width;
	dmScreenSettings.dmPelsHeight = height;
	dmScreenSettings.dmBitsPerPel = bits;
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
	if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{SkinMsgBox(0,"Error setting fullscreen mode!","ERROR",MB_OK);}
}

void Restore_Screen()
{
	ChangeDisplaySettings(0,0);
}

int restore_window_width = 0;
int restore_window_height = 0;
int restore_window_xpos = 0;
int restore_window_ypos = 0;

void Save_Window()
{
	restore_window_width = _screenwidth + width_border;
	restore_window_height = _screenheight + height_border;
	restore_window_xpos = window_xpos;
	restore_window_ypos = window_ypos;
}

void Restore_Window()
{
	ShowWindow(_hWnd, SW_RESTORE);
	ShowWindow(_hWnd, SW_SHOW);
	::SetWindowPos(_hWnd, 0, restore_window_xpos, 0, restore_window_width, restore_window_height, 0);
}

void Run_Fullscreen()
{
}

void Exit_Fullscreen()
{
}




