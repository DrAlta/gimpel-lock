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
#include "Camera.h"
#include "GLWnd.h"
#include "Frame.h"
#include "Console.h"
#include <gl\gl.h>
#include <gl\glu.h>
#include <math.h>

float fastsqrt(float n);


double modelview_matrix[16];
double projection_matrix[16];
int viewport[8];

float camera_pos[3] = {0,0,0};
float camera_rot[3] = {0,0,0};

float camera_front[3] = {0,0,1};
float camera_side[3] = {1,0,0};
float camera_up[3] = {0,1,0};

float target_camera_pos[3] = {0,0,0};
float target_camera_rot[3] = {0,0,0};

float camera_tsnap = 0.06f*2;
float camera_rsnap = 0.08f*2;

bool camera_moving = false;

class SAVED_CAMERA_VIEW
{
public:
	SAVED_CAMERA_VIEW()
	{
	}
	~SAVED_CAMERA_VIEW()
	{
	}
	float pos[3];
	float rot[3];
};

const int max_saved_camera_views = 4;

SAVED_CAMERA_VIEW saved_camera_views[max_saved_camera_views];

bool Save_Camera_View(int n)
{
	if(n<0||n>=max_saved_camera_views)
	{
		return false;
	}
	SAVED_CAMERA_VIEW *scv = &saved_camera_views[n];
	Get_Camera(scv->pos, scv->rot);
	return true;
}

bool Set_Saved_Camera_View(int n)
{
	if(n<0||n>=max_saved_camera_views)
	{
		return false;
	}
	SAVED_CAMERA_VIEW *scv = &saved_camera_views[n];
	Set_Target_Camera(scv->pos, scv->rot);
	return true;
}

bool Save_Camera_Views(FILE *f)
{
	fwrite(&max_saved_camera_views, sizeof(int), 1, f);
	for(int i = 0;i<max_saved_camera_views;i++)
	{
		fwrite(&saved_camera_views[i].pos, sizeof(float), 3, f);
		fwrite(&saved_camera_views[i].rot, sizeof(float), 3, f);
	}
	return true;
}

bool Load_Camera_Views(FILE *f)
{
	int n = 0;
	fread(&n, sizeof(int), 1, f);
	if(n<=max_saved_camera_views)
	{
		for(int i = 0;i<n;i++)
		{
			fread(&saved_camera_views[i].pos, sizeof(float), 3, f);
			fread(&saved_camera_views[i].rot, sizeof(float), 3, f);
		}
	}
	return true;
}

void Get_Matrices()
{
    glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
}

void Get_World_Pos(int x, int y, float depth, float *res)
{
	double wx, wy, wz;
	gluUnProject(x, y, depth, modelview_matrix, projection_matrix, viewport, &wx, &wy, &wz);
	res[0] = -(float)wx;
	res[1] = (float)wy;
	res[2] = -(float)wz;
}

//used for clicking on the model to select link points, always at the origin
bool Get_World_Pos(int x, int y, float *res)
{
	double wx, wy, wz;
	float depth = 0;
	y = (Screenheight()-1)-y;
	glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	if(depth==1)
	{
		return false;
	}
	gluUnProject(x, y, depth, modelview_matrix, projection_matrix, viewport, &wx, &wy, &wz);
	res[0] = (float)wx;
	res[1] = (float)wy;
	res[2] = (float)wz;
	return true;
}

void Update_Camera_Dir()
{
    double *m = modelview_matrix;
	camera_side[0] = -(float)m[0];
	camera_side[1] = -(float)m[4];
	camera_side[2] = -(float)m[8];
	camera_up[0] = (float)m[1];
	camera_up[1] = (float)m[5];
	camera_up[2] = (float)m[9];
	camera_front[0] = (float)m[2];
	camera_front[1] = (float)m[6];
	camera_front[2] = (float)m[10];
}

void Get_Camera_Vectors(float *side, float *up, float *front)
{
	side[0] = camera_side[0];
	side[1] = camera_side[1];
	side[2] = camera_side[2];
	up[0] = camera_up[0];
	up[1] = camera_up[1];
	up[2] = camera_up[2];
	front[0] = camera_front[0];
	front[1] = camera_front[1];
	front[2] = camera_front[2];
}

void Set_Camera_Transform(float *pos, float *rot)
{
	camera_pos[0] = pos[0];
	camera_pos[1] = pos[1];
	camera_pos[2] = pos[2];
	camera_rot[0] = rot[0];
	camera_rot[1] = rot[1];
	camera_rot[2] = rot[2];
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(camera_rot[0], 1, 0, 0);
	glRotatef(camera_rot[1], 0, 1, 0);
	glTranslatef(-camera_pos[0], -camera_pos[1], -camera_pos[2]);
	Get_Matrices();
	Update_Camera_Dir();
}

void Set_GL_Camera_Transform()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(camera_rot[0], 1, 0, 0);
	glRotatef(camera_rot[1], 0, 1, 0);
	glTranslatef(-camera_pos[0], -camera_pos[1], -camera_pos[2]);
}

void Set_Camera_To_Target()
{
	camera_pos[0] = target_camera_pos[0];
	camera_pos[1] = target_camera_pos[1];
	camera_pos[2] = target_camera_pos[2];
	camera_rot[0] = target_camera_rot[0];
	camera_rot[1] = target_camera_rot[1];
	camera_rot[2] = target_camera_rot[2];
}


void Set_Camera(float *pos, float *rot)
{
	camera_pos[0] = pos[0];
	camera_pos[1] = pos[1];
	camera_pos[2] = pos[2];
	camera_rot[0] = rot[0];
	camera_rot[1] = rot[1];
	camera_rot[2] = rot[2];
	target_camera_pos[0] = pos[0];
	target_camera_pos[1] = pos[1];
	target_camera_pos[2] = pos[2];
	target_camera_rot[0] = rot[0];
	target_camera_rot[1] = rot[1];
	target_camera_rot[2] = rot[2];
}

void Get_Camera(float *pos, float *rot)
{
	pos[0] = camera_pos[0];
	pos[1] = camera_pos[1];
	pos[2] = camera_pos[2];
	rot[0] = camera_rot[0];
	rot[1] = camera_rot[1];
	rot[2] = camera_rot[2];
}


void Set_Target_Camera(float *pos, float *rot)
{
	target_camera_pos[0] = pos[0];
	target_camera_pos[1] = pos[1];
	target_camera_pos[2] = pos[2];
	target_camera_rot[0] = rot[0];
	target_camera_rot[1] = rot[1];
	target_camera_rot[2] = rot[2];
	redraw_frame = true;
	camera_moving = true;
}

__forceinline float IP(float v1, float v2, float p)
{
	return (v2*p)+v1*(1.0f-p);
}

void Move_To_Target_Camera()
{
	camera_pos[0] = IP(camera_pos[0], target_camera_pos[0], camera_tsnap);
	camera_pos[1] = IP(camera_pos[1], target_camera_pos[1], camera_tsnap);
	camera_pos[2] = IP(camera_pos[2], target_camera_pos[2], camera_tsnap);
	camera_rot[0] = IP(camera_rot[0], target_camera_rot[0], camera_rsnap);
	camera_rot[1] = IP(camera_rot[1], target_camera_rot[1], camera_rsnap);
	camera_rot[2] = IP(camera_rot[2], target_camera_rot[2], camera_rsnap);
}

float mfabs(float v)
{
	if(v<0)return -v;
	return v;
}

void Update_Camera()
{
	Move_To_Target_Camera();
	Set_Camera_Transform(camera_pos, camera_rot);
	Get_Camera_Vectors(camera_side, camera_up, camera_front);
	
	float xrdiff = mfabs(target_camera_rot[0]-camera_rot[0]);
	float yrdiff = mfabs(target_camera_rot[1]-camera_rot[1]);
	float zrdiff = mfabs(target_camera_rot[2]-camera_rot[2]);
	camera_moving = false;
	if(xrdiff>0.01f)
	{
		redraw_frame = true;
		camera_moving = true;
		return;
	}
	if(yrdiff>0.01f)
	{
		redraw_frame = true;
		camera_moving = true;
		return;
	}
	if(zrdiff>0.01f)
	{
		redraw_frame = true;
		camera_moving = true;
		return;
	}
	float xd = target_camera_pos[0]-camera_pos[0];
	float yd = target_camera_pos[1]-camera_pos[1];
	float zd = target_camera_pos[2]-camera_pos[2];
	float d = (sqrt(xd*xd)+(yd*yd)+(zd*zd))*1000;
	if(d>0.001f)
	{
		redraw_frame = true;
		camera_moving = true;
		return;
	}
	else if(d!=0)
	{
		camera_pos[0] = target_camera_pos[0];
		camera_pos[1] = target_camera_pos[1];
		camera_pos[2] = target_camera_pos[2];
		camera_rot[0] = target_camera_rot[0];
		camera_rot[1] = target_camera_rot[1];
		camera_rot[2] = target_camera_rot[2];
		redraw_frame = true;
		camera_moving = true;
	}
}

void Move_Forward(float f)
{
	target_camera_pos[0]-=camera_front[0]*f;
	target_camera_pos[1]-=camera_front[1]*f;
	target_camera_pos[2]-=camera_front[2]*f;
	redraw_frame = true;
	camera_moving = true;
}

void Move_Back(float f)
{
	target_camera_pos[0]+=camera_front[0]*f;
	target_camera_pos[1]+=camera_front[1]*f;
	target_camera_pos[2]+=camera_front[2]*f;
	redraw_frame = true;
	camera_moving = true;
}

void Move_Up(float f)
{
	target_camera_pos[0]+=camera_up[0]*f;
	target_camera_pos[1]+=camera_up[1]*f;
	target_camera_pos[2]+=camera_up[2]*f;
	redraw_frame = true;
	camera_moving = true;
}

void Move_Down(float f)
{
	target_camera_pos[0]-=camera_up[0]*f;
	target_camera_pos[1]-=camera_up[1]*f;
	target_camera_pos[2]-=camera_up[2]*f;
	redraw_frame = true;
	camera_moving = true;
}


void Move_Left(float f)
{
	target_camera_pos[0]+=camera_side[0]*f;
	target_camera_pos[1]+=camera_side[1]*f;
	target_camera_pos[2]+=camera_side[2]*f;
	redraw_frame = true;
	camera_moving = true;
}

void Move_Right(float f)
{
	target_camera_pos[0]-=camera_side[0]*f;
	target_camera_pos[1]-=camera_side[1]*f;
	target_camera_pos[2]-=camera_side[2]*f;
	redraw_frame = true;
	camera_moving = true;
}


void Turn_Up(float f)
{
	target_camera_rot[0] -= f;
	if(target_camera_rot[0]<-90)
	{
		target_camera_rot[0] = -90;
	}
	redraw_frame = true;
	camera_moving = true;
}

void Turn_Down(float f)
{
	target_camera_rot[0] += f;
	if(target_camera_rot[0]>90)
	{
		target_camera_rot[0] = 90;
	}
	redraw_frame = true;
	camera_moving = true;
}

void Turn_Left(float f)
{
	target_camera_rot[1] -= f;
	redraw_frame = true;
	camera_moving = true;
}

void Turn_Right(float f)
{
	target_camera_rot[1] += f;
	redraw_frame = true;
	camera_moving = true;
}

void Spin_Left(float f)
{
	target_camera_rot[2] += f;
	redraw_frame = true;
	camera_moving = true;
}

void Spin_Right(float f)
{
	target_camera_rot[2] -= f;
	redraw_frame = true;
	camera_moving = true;
}

