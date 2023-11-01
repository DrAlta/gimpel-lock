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
#include "Camera.h"
#include "GLWnd.h"
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>
#include <gl\wglext.h>
#include "../Skin.h"

void Push_3D_View();
void Pop_3D_View();


PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB = 0;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = 0;
PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB = 0;
PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB = 0;
PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB = 0;
PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB = 0;
PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB = 0;
PFNWGLSETPBUFFERATTRIBARBPROC wglSetPbufferAttribARB = 0;

HPBUFFERARB pbuffer = 0;
HGLRC pbuffer_rc = 0;

int pbuffer_width = 0;
int pbuffer_height = 0;

bool Create_PBuffer(int width, int height)
{
	pbuffer_width = width;
	pbuffer_height = height;
	Set_GLContext();
	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	if(!wglGetExtensionsStringARB)
	{
		SkinMsgBox(0, "Can't get extension!", "wglGetExtensionsStringARB", MB_OK);
		return false;
	}
	char *extensionString = (char *)wglGetExtensionsStringARB(wglGetCurrentDC());
	if(!strstr(extensionString, "WGL_ARB_pbuffer"))
	{
		SkinMsgBox(0, "Can't get extension!", "WGL_ARB_pbuffer", MB_OK);
		return false;
	}
	if(!strstr(extensionString, "WGL_ARB_pixel_format"))
	{
		SkinMsgBox(0, "Can't get extension!", "WGL_ARB_pixel_format", MB_OK);
		return false;
	}
	if(!strstr(extensionString, "WGL_ARB_make_current_read"))
	{
		SkinMsgBox(0, "Can't get extension!", "WGL_ARB_make_current_read", MB_OK);
		return false;
	}
	wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)wglGetProcAddress("wglMakeContextCurrentARB");
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)wglGetProcAddress("wglCreatePbufferARB");
	wglDestroyPbufferARB = (PFNWGLDESTROYPBUFFERARBPROC)wglGetProcAddress("wglDestroyPbufferARB");
	wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)wglGetProcAddress("wglGetPbufferDCARB");
	wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)wglGetProcAddress("wglReleasePbufferDCARB");
	wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)wglGetProcAddress("wglQueryPbufferARB");
	wglSetPbufferAttribARB = (PFNWGLSETPBUFFERATTRIBARBPROC)wglGetProcAddress("wglSetPbufferAttribARB");
	
	int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
		WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,24,
		WGL_ALPHA_BITS_ARB,8,
		WGL_DEPTH_BITS_ARB,16,
		WGL_STENCIL_BITS_ARB,0,
		0,0};


//Get Current Device and Rendering Contexts
HDC win_dc  = wglGetCurrentDC();
HGLRC win_glrc  = wglGetCurrentContext();

int pixel_format[] = {
	WGL_DRAW_TO_PBUFFER_ARB, 1,
	WGL_SUPPORT_OPENGL_ARB, 1,
	WGL_ALPHA_BITS_ARB,8,
	WGL_DEPTH_BITS_ARB,16,
	WGL_STENCIL_BITS_ARB,0,
	0};

int buffer_format[] = {
	WGL_PBUFFER_LARGEST_ARB, 1,
	0};

	int best_format = -1;
	unsigned int num_formats = 0;

	if(!wglChoosePixelFormatARB(win_dc, (const int*)pixel_format, NULL, 1, &best_format, &num_formats))
	{
		SkinMsgBox(0, "Error choosing pixel format!", 0, MB_OK);
	}

	pbuffer = wglCreatePbufferARB(win_dc, best_format, width, height, (const int*)buffer_format);

	if(!pbuffer)
	{
		SkinMsgBox(0, "Error creating pbuffer!", 0, MB_OK);
	}

	pbuffer_rc = wglCreateContext(wglGetPbufferDCARB(pbuffer));

	wglMakeContextCurrentARB(wglGetPbufferDCARB(pbuffer), wglGetPbufferDCARB(pbuffer), pbuffer_rc);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_DEPTH_TEST);
	return true;
}

bool Destroy_PBuffer()
{
	HDC win_dc  = wglGetCurrentDC();
	HGLRC win_glrc  = wglGetCurrentContext();
	wglDeleteContext(pbuffer_rc);
	wglReleasePbufferDCARB(pbuffer, wglGetPbufferDCARB(pbuffer));
	wglMakeContextCurrentARB(win_dc, win_dc, win_glrc);
	wglDestroyPbufferARB(pbuffer);
	pbuffer = 0;
	return true;
}

void Set_PBuffer_Context(int width, int height)
{
	if(!pbuffer)
	{
		Create_PBuffer(width, height);
	}
	else
	{
		if(width!=pbuffer_width||height!=pbuffer_height)
		{
			Destroy_PBuffer();
			Create_PBuffer(width, height);
		}
	}
	if(pbuffer)
	{
		wglMakeContextCurrentARB(wglGetPbufferDCARB(pbuffer), wglGetPbufferDCARB(pbuffer), pbuffer_rc);
	}
}

float old_screen_width = 0;
float old_screen_height = 0;

float old_camera_pos[3];
float old_camera_rot[3];

bool Setup_PBuffer(int w, int h)
{
	Set_PBuffer_Context(w, h);
	glViewport(0,0,w,h);
	old_screen_width = (float)Screenwidth();
	old_screen_height = (float)Screenheight();
	ReSizeGLScene(w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(_fov,_aspect, _near_plane, _far_plane);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	Set_PBuffer_Context(w, h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	if(auto_center_stereoview)
	{
		Push_3D_View();
		old_camera_pos[0] = camera_pos[0];
		old_camera_pos[1] = camera_pos[1];
		old_camera_pos[2] = camera_pos[2];
		old_camera_rot[0] = camera_rot[0];
		old_camera_rot[1] = camera_rot[1];
		old_camera_rot[2] = camera_rot[2];
		Center_View();
		Set_Camera_To_Target();
		Set_GL_Camera_Transform();
		Get_Matrices();
		Update_Camera_Dir();
	}
	else
	{
		Set_GL_Camera_Transform();
		Get_Matrices();
		Update_Camera_Dir();
	}
	return true;
}

bool Undo_PBuffer()
{
	redraw_frame = true;
	Set_GLContext();
	ReSizeGLScene((int)old_screen_width, (int)old_screen_height);
	if(auto_center_stereoview)
	{
		Pop_3D_View();
		camera_pos[0] = old_camera_pos[0];
		camera_pos[1] = old_camera_pos[1];
		camera_pos[2] = old_camera_pos[2];
		camera_rot[0] = old_camera_rot[0];
		camera_rot[1] = old_camera_rot[1];
		camera_rot[2] = old_camera_rot[2];
		Set_Target_Camera(camera_pos, camera_rot);
		Set_Camera_To_Target();
		Set_GL_Camera_Transform();
		Get_Matrices();
		Update_Camera_Dir();
	}
	else
	{
		Set_GL_Camera_Transform();
		Get_Matrices();
		Update_Camera_Dir();
	}
	return true;
}
