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
#include "PerspectiveTool.h"
#include "G3DCoreFiles/Frame.h"
#include "G3DCoreFiles/Layers.h"
#include "G3DCoreFiles/bb_matrix.h"
#include "Skin.h"
#include <gl/gl.h>
#include <gl/glu.h>

float Pixel_Size();

void Set_Fov(float fov);
void Stop_Perspective_Alignment();

void ReSizeGLScene(int width, int height, float fov);

#define EPSILON 0.000001

extern double modelview_matrix[16];
extern double projection_matrix[16];
extern int viewport[8];
int Screenwidth();
int Screenheight();
void ReSizeGLScene(int width, int height);
void Get_Matrices();
void SetLength(float *v, float s);
void Normalize(float *v);
float Dot(float *a, float *b);

bool adjustable_fov = false;


bool horizontal_parallel_lines = false;

float fov_alignment_inc = 1.0f;
float perspective_fov = 45;

float perspective_alignment_inc = 1.0f;
float lowest_perspective_alignment_error = 360;

bool render_perspective_tool = false;

bool use_single_vanishing_point = true;


bool align_perspective_forward = false;
bool align_perspective_back = false;
bool align_perspective_left = false;
bool align_perspective_right = true;


int perspective_projection_type = pps_PARALLEL;

float perspective_alignment_angle = 90;

bool iterate_perspective_alignment = false;


float control_point_xclick_off = 0;
float control_point_yclick_off = 0;

float perspective_alignment_center[3] = {0,0,0};
float perspective_alignment_dir[3] = {0,0,0};

oBB_MATRIX perspective_alignment_matrix;

float perspective_alignment_horizontal_rotation = 0;
float perspective_alignment_vertical_rotation = 0;


class CONTROL_POINT
{
public:
	CONTROL_POINT()
	{
		clr[0] = clr[1] = clr[2] = 1;
		pos[0] = pos[1] = 0;
		dir[0] = dir[1] = dir[2] = 0;
	}
	~CONTROL_POINT()
	{
	}
	float pos[2];
	float clr[3];
	float dir[3];
};

//used for corner angle projection

CONTROL_POINT pps_vanishing_point;//center
CONTROL_POINT pps_dir1;//user defined directions
CONTROL_POINT pps_dir2;
CONTROL_POINT pps_pos1;//temporary
CONTROL_POINT pps_pos2;

//used for parallel edges projection
CONTROL_POINT pps_p1a;
CONTROL_POINT pps_p1b;
CONTROL_POINT pps_p2a;
CONTROL_POINT pps_p2b;
//also re-used for rectangle


CONTROL_POINT *clicked_control_point = 0;

__forceinline bool Try_Select_Control_Point(CONTROL_POINT *cp, float x, float y, float range)
{
	float dx = x-cp->pos[0];if(dx<0)dx = -dx;
	float dy = y-cp->pos[1];if(dy<0)dy = -dy;
	if(dx>range||dy>range)return false;
	clicked_control_point = cp;
	control_point_xclick_off = x-cp->pos[0];
	control_point_yclick_off = y-cp->pos[1];
	return true;
}

bool Find_Clicked_Control_Point(float px, float py)
{
	float range = 5*(1.0f/Pixel_Size());
	clicked_control_point = 0;
	if(Try_Select_Control_Point(&pps_vanishing_point, px, py, range))return true;
	if(Try_Select_Control_Point(&pps_dir1, px, py, range))return true;
	if(Try_Select_Control_Point(&pps_dir2, px, py, range))return true;
	if(Try_Select_Control_Point(&pps_p1a, px, py, range))return true;
	if(Try_Select_Control_Point(&pps_p1b, px, py, range))return true;
	if(Try_Select_Control_Point(&pps_p2a, px, py, range))return true;
	if(Try_Select_Control_Point(&pps_p2b, px, py, range))return true;
	return false;
}

bool Init_Default_Perspective_Control_Points()
{
	if(frame)
	{
		pps_vanishing_point.pos[0] = 0.4f*frame->width;
		pps_vanishing_point.pos[1] = 0.5f*frame->height;
		pps_dir1.pos[0] = 0.6f*frame->width;
		pps_dir1.pos[1] = 0.3f*frame->height;
		pps_dir2.pos[0] = 0.6f*frame->width;
		pps_dir2.pos[1] = 0.8f*frame->height;
		pps_p1a.pos[0] = 0.3f*frame->width;
		pps_p2a.pos[0] = 0.3f*frame->width;
		pps_p1b.pos[0] = 0.8f*frame->width;
		pps_p2b.pos[0] = 0.8f*frame->width;
		
		pps_p1a.pos[1] = 0.3f*frame->height;
		pps_p1b.pos[1] = 0.3f*frame->height;
		pps_p2a.pos[1] = 0.8f*frame->height;
		pps_p2b.pos[1] = 0.8f*frame->height;
	}
	return true;
}

bool Render_Perspective_Corner_Control_Points()
{
	float p1[2];
	float p2[2];
	p1[0] = pps_vanishing_point.pos[0]+((pps_dir1.pos[0]-pps_vanishing_point.pos[0])*1000);
	p1[1] = pps_vanishing_point.pos[1]+((pps_dir1.pos[1]-pps_vanishing_point.pos[1])*1000);
	p2[0] = pps_vanishing_point.pos[0]+((pps_dir2.pos[0]-pps_vanishing_point.pos[0])*1000);
	p2[1] = pps_vanishing_point.pos[1]+((pps_dir2.pos[1]-pps_vanishing_point.pos[1])*1000);

	glColor3f(0,1,0);
	glBegin(GL_LINES);
	glVertex2fv(pps_vanishing_point.pos);
	glVertex2fv(p1);
	glVertex2fv(pps_vanishing_point.pos);
	glVertex2fv(p2);
	glEnd();

	glPointSize(10);
	glColor3f(1,0,0);
	glBegin(GL_POINTS);
	glVertex2fv(pps_vanishing_point.pos);
	glVertex2fv(pps_dir1.pos);
	glVertex2fv(pps_dir2.pos);
	glEnd();

	glColor3f(1,1,1);
	glPointSize(1);
	return true;
}

bool Render_Perspective_Parallel_Control_Points()
{
	float p1a[2];
	float p1b[2];
	p1a[0] = pps_p1a.pos[0]+((pps_p1a.pos[0]-pps_p1b.pos[0])*1000);
	p1a[1] = pps_p1a.pos[1]+((pps_p1a.pos[1]-pps_p1b.pos[1])*1000);
	p1b[0] = pps_p1b.pos[0]+((pps_p1b.pos[0]-pps_p1a.pos[0])*1000);
	p1b[1] = pps_p1b.pos[1]+((pps_p1b.pos[1]-pps_p1a.pos[1])*1000);

	float p2a[2];
	float p2b[2];
	p2a[0] = pps_p2a.pos[0]+((pps_p2a.pos[0]-pps_p2b.pos[0])*1000);
	p2a[1] = pps_p2a.pos[1]+((pps_p2a.pos[1]-pps_p2b.pos[1])*1000);
	p2b[0] = pps_p2b.pos[0]+((pps_p2b.pos[0]-pps_p2a.pos[0])*1000);
	p2b[1] = pps_p2b.pos[1]+((pps_p2b.pos[1]-pps_p2a.pos[1])*1000);

	glColor3f(0,1,0);
	glBegin(GL_LINES);
	
	glVertex2fv(p1a);
	glVertex2fv(p1b);

	glVertex2fv(p2a);
	glVertex2fv(p2b);
	glEnd();

	glPointSize(10);
	glColor3f(1,0,0);
	glBegin(GL_POINTS);
	glVertex2fv(pps_p1a.pos);
	glVertex2fv(pps_p1b.pos);
	glVertex2fv(pps_p2a.pos);
	glVertex2fv(pps_p2b.pos);
	glEnd();

	glColor3f(1,1,1);
	glPointSize(1);
	return true;
}


bool Render_Perspective_Rectangle_Control_Points()
{
	float p1a[2];
	float p1b[2];
	p1a[0] = pps_p1a.pos[0]+((pps_p1a.pos[0]-pps_p1b.pos[0])*1000);
	p1a[1] = pps_p1a.pos[1]+((pps_p1a.pos[1]-pps_p1b.pos[1])*1000);
	p1b[0] = pps_p1b.pos[0]+((pps_p1b.pos[0]-pps_p1a.pos[0])*1000);
	p1b[1] = pps_p1b.pos[1]+((pps_p1b.pos[1]-pps_p1a.pos[1])*1000);

	float p2a[2];
	float p2b[2];
	p2a[0] = pps_p2a.pos[0]+((pps_p2a.pos[0]-pps_p2b.pos[0])*1000);
	p2a[1] = pps_p2a.pos[1]+((pps_p2a.pos[1]-pps_p2b.pos[1])*1000);
	p2b[0] = pps_p2b.pos[0]+((pps_p2b.pos[0]-pps_p2a.pos[0])*1000);
	p2b[1] = pps_p2b.pos[1]+((pps_p2b.pos[1]-pps_p2a.pos[1])*1000);

	glColor3f(0,1,0);
	glBegin(GL_LINES);
	
	glVertex2fv(p1a);
	glVertex2fv(p1b);

	glVertex2fv(p2a);
	glVertex2fv(p2b);

	p1a[0] = pps_p1a.pos[0]+((pps_p1a.pos[0]-pps_p2a.pos[0])*1000);
	p1a[1] = pps_p1a.pos[1]+((pps_p1a.pos[1]-pps_p2a.pos[1])*1000);
	p1b[0] = pps_p2a.pos[0]+((pps_p2a.pos[0]-pps_p1a.pos[0])*1000);
	p1b[1] = pps_p2a.pos[1]+((pps_p2a.pos[1]-pps_p1a.pos[1])*1000);

	p2a[0] = pps_p1b.pos[0]+((pps_p1b.pos[0]-pps_p2b.pos[0])*1000);
	p2a[1] = pps_p1b.pos[1]+((pps_p1b.pos[1]-pps_p2b.pos[1])*1000);
	p2b[0] = pps_p2b.pos[0]+((pps_p2b.pos[0]-pps_p1b.pos[0])*1000);
	p2b[1] = pps_p2b.pos[1]+((pps_p2b.pos[1]-pps_p1b.pos[1])*1000);

	glVertex2fv(p1a);
	glVertex2fv(p1b);

	glVertex2fv(p2a);
	glVertex2fv(p2b);

	glEnd();

	glPointSize(10);
	glColor3f(1,0,0);
	glBegin(GL_POINTS);
	glVertex2fv(pps_p1a.pos);
	glVertex2fv(pps_p1b.pos);
	glVertex2fv(pps_p2a.pos);
	glVertex2fv(pps_p2b.pos);
	glEnd();

	glColor3f(1,1,1);
	glPointSize(1);
	return true;
}


bool Render_Control_Points()
{
	if(perspective_projection_type==pps_ANGLE)
	{
		Render_Perspective_Corner_Control_Points();
	}
	if(perspective_projection_type==pps_RECTANGLE)
	{
		Render_Perspective_Rectangle_Control_Points();
	}
	if(perspective_projection_type==pps_PARALLEL)
	{
		Render_Perspective_Parallel_Control_Points();
	}
	return true;
}

bool Open_Perspective_Tool()
{
	Open_Perspective_Dlg();
	render_perspective_tool = true;
	if(pps_dir1.pos[0]==0&&pps_dir1.pos[1]==0)
	{
		Init_Default_Perspective_Control_Points();
	}
	redraw_edit_window = true;
	return true;
}

bool Close_Perspective_Tool()
{
	render_perspective_tool = false;
	Close_Perspective_Dlg();
	redraw_edit_window = true;
	Stop_Perspective_Alignment();
	return true;
}


bool Render_Perspective_Tool()
{
	Render_Control_Points();
	return true;
}

bool Click_Perspective_Tool(float x, float y)
{
	Find_Clicked_Control_Point(x, y);
	return true;
}

bool Drag_Perspective_Tool(float x, float y)
{
	if(clicked_control_point)
	{
		clicked_control_point->pos[0] = x-control_point_xclick_off;
		clicked_control_point->pos[1] = y-control_point_yclick_off;
	}
	return true;
}

bool Get_Control_Point_Vector(CONTROL_POINT *cp)
{
	double wx, wy, wz;
	gluUnProject(cp->pos[0], cp->pos[1], 1, modelview_matrix, projection_matrix, viewport, &wx, &wy, &wz);
	cp->dir[0] = -wx;
	cp->dir[1] = wy;
	cp->dir[2] = -wz;
	SetLength(cp->dir, 0.001f);
	return true;
}

bool Get_Control_Point_Vectors()
{
	float ow = Screenwidth();
	float oh = Screenheight();
	
	ReSizeGLScene(frame->width, frame->height, perspective_fov);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	Get_Matrices();
	Get_Control_Point_Vector(&pps_vanishing_point);
	Get_Control_Point_Vector(&pps_dir1);
	Get_Control_Point_Vector(&pps_dir2);
	Get_Control_Point_Vector(&pps_pos1);
	Get_Control_Point_Vector(&pps_pos2);
	Get_Control_Point_Vector(&pps_p1a);
	Get_Control_Point_Vector(&pps_p1b);
	Get_Control_Point_Vector(&pps_p2a);
	Get_Control_Point_Vector(&pps_p2b);
	glPopMatrix();
	ReSizeGLScene(ow, oh);
	Set_View();
	return true;
}

void Get_Plane_Dir(float *orot, float *dir);

__forceinline void Update_Perspective_Alignment_Dir()
{
	float pos[3] = {0,0,0};
	float rot[3];
	rot[0] = perspective_alignment_horizontal_rotation;
	rot[1] = perspective_alignment_vertical_rotation;
	rot[2] = 0;
	Get_Plane_Dir(rot, perspective_alignment_dir);
}

__forceinline float Dist_To_Plane(float *p, float *tp, float *n)
{
	float r = (((p[0]-tp[0])*n[0])+((p[1]-tp[1])*n[1])+((p[2]-tp[2])*n[2]));
	if(r<0)return -r;
	return r;
}

__forceinline void Project_To_Perspective_Alignment_Plane(float *dir)
{
	//simple plane intersection
	float far_pos[3];
	float d = 100000;//FIXTHIS could be slimmed down A LOT
	far_pos[0] = frame->view_origin[0]+(dir[0]*d);
	far_pos[1] = frame->view_origin[1]+(dir[1]*d);
	far_pos[2] = frame->view_origin[2]+(dir[2]*d);
	float near_dist = Dist_To_Plane(frame->view_origin, perspective_alignment_center, perspective_alignment_dir);
	float far_dist = Dist_To_Plane(far_pos, perspective_alignment_center, perspective_alignment_dir);
	float total = near_dist+far_dist;
	float p = near_dist/total;
	float ip = 1.0f-p;
	dir[0] = (far_pos[0]*p)+(frame->view_origin[0]*ip);
	dir[1] = (far_pos[1]*p)+(frame->view_origin[1]*ip);
	dir[2] = (far_pos[2]*p)+(frame->view_origin[2]*ip);
}

__forceinline float NotNan(float v)
{
	char text[32];
	sprintf(text, "%f", v);
	if(strchr(text, '#'))
	{
		return 0;
	}
	return v;
}

__forceinline float Get_Perspective_Parallel_Angle()
{
	Project_To_Perspective_Alignment_Plane(pps_p1a.dir);
	Project_To_Perspective_Alignment_Plane(pps_p1b.dir);
	Project_To_Perspective_Alignment_Plane(pps_p2a.dir);
	Project_To_Perspective_Alignment_Plane(pps_p2b.dir);
	//get vectors to center position
	float v1[3];
	float v2[3];
	v1[0] = pps_p1a.dir[0]-pps_p1b.dir[0];
	v1[1] = pps_p1a.dir[1]-pps_p1b.dir[1];
	v1[2] = pps_p1a.dir[2]-pps_p1b.dir[2];
	v2[0] = pps_p2a.dir[0]-pps_p2b.dir[0];
	v2[1] = pps_p2a.dir[1]-pps_p2b.dir[1];
	v2[2] = pps_p2a.dir[2]-pps_p2b.dir[2];
	Normalize(v1);
	Normalize(v2);
	float dot = Dot(v1, v2);
	float angle = acos(dot);
	//used to convert degrees to radians, == PI/180
	const double r2d = (180.0/3.14159);
	angle *= r2d;
	return NotNan(angle);
}

__forceinline float Get_Perspective_Parallel_Horizontal_Angle()
{
	//same basic check but check against perfectly flat horizon line
	Project_To_Perspective_Alignment_Plane(pps_p1a.dir);
	Project_To_Perspective_Alignment_Plane(pps_p1b.dir);
	Project_To_Perspective_Alignment_Plane(pps_p2a.dir);
	Project_To_Perspective_Alignment_Plane(pps_p2b.dir);
	//get vectors to center position
	float v1[3];
	float v2[3];
	//get 2 vectors for each horizontal line

	v1[0] = 1;
	v1[1] = pps_p1a.dir[1]-pps_p1b.dir[1];
	v1[2] = 0;
	v2[0] = 1;
	v2[1] = pps_p2a.dir[1]-pps_p2b.dir[1];
	v2[2] = 0;
	Normalize(v1);
	Normalize(v2);
	//get a flat horizontal vector
	float hvec[3] = {1,0,0};

	//measure error from each vector

	float dot1 = Dot(v1, hvec);
	float angle1 = acos(dot1);
	float dot2 = Dot(v2, hvec);
	float angle2 = acos(dot2);
	//used to convert degrees to radians, == PI/180
	const double r2d = (180.0/3.14159);
	angle1 *= r2d;
	angle2 *= r2d;
	return NotNan(angle1)+NotNan(angle2);
}

__forceinline float Get_Perspective_Parallel_Error()
{
	float angle = Get_Perspective_Parallel_Angle();
	if(angle>90)
	{
		angle = 180-angle;
	}

	if(horizontal_parallel_lines)
	{
		angle += Get_Perspective_Parallel_Horizontal_Angle();
	}

	return angle;

}


__forceinline float Get_Perspective_Rectangle_Angle(CONTROL_POINT *a, CONTROL_POINT *b, CONTROL_POINT *c)
{
	//get vectors to center position
	float v1[3];
	float v2[3];
	v1[0] = a->dir[0]-b->dir[0];
	v1[1] = a->dir[1]-b->dir[1];
	v1[2] = a->dir[2]-b->dir[2];
	v2[0] = b->dir[0]-c->dir[0];
	v2[1] = b->dir[1]-c->dir[1];
	v2[2] = b->dir[2]-c->dir[2];
	Normalize(v1);
	Normalize(v2);
	float dot = Dot(v1, v2);
	float angle = acos(dot);
	//used to convert degrees to radians, == PI/180
	const double r2d = (180.0/3.14159);
	angle *= r2d;
	return angle;
}

__forceinline float Get_Perspective_Parallel_Angle(CONTROL_POINT *a, CONTROL_POINT *b, CONTROL_POINT *c, CONTROL_POINT *d)
{
	//get vectors to center position
	float v1[3];
	float v2[3];
	v1[0] = a->dir[0]-b->dir[0];
	v1[1] = a->dir[1]-b->dir[1];
	v1[2] = a->dir[2]-b->dir[2];
	v2[0] = c->dir[0]-d->dir[0];
	v2[1] = c->dir[1]-d->dir[1];
	v2[2] = c->dir[2]-d->dir[2];
	Normalize(v1);
	Normalize(v2);
	float dot = Dot(v1, v2);
	float angle = acos(dot);
	//used to convert degrees to radians, == PI/180
	const double r2d = (180.0/3.14159);
	angle *= r2d;
	return NotNan(angle);
}


__forceinline float Get_Perspective_Rectangle_Error()
{
	Project_To_Perspective_Alignment_Plane(pps_p1a.dir);
	Project_To_Perspective_Alignment_Plane(pps_p1b.dir);
	Project_To_Perspective_Alignment_Plane(pps_p2a.dir);
	Project_To_Perspective_Alignment_Plane(pps_p2b.dir);
	CONTROL_POINT *a = &pps_p1a;
	CONTROL_POINT *b = &pps_p1b;
	CONTROL_POINT *c = &pps_p2b;
	CONTROL_POINT *d = &pps_p2a;
	float angle1 = Get_Perspective_Rectangle_Angle(a, b, c);
	float angle2 = Get_Perspective_Rectangle_Angle(b, c, d);
	float angle3 = Get_Perspective_Rectangle_Angle(c, d, a);
	float angle4 = Get_Perspective_Rectangle_Angle(d, a, b);
	float angle5 = Get_Perspective_Parallel_Angle(a, b, c, d);
	float angle6 = Get_Perspective_Parallel_Angle(a, d, b, c);
	float res = 0;
	if(angle1>90){res+=angle1-90;}else{res+=90-angle1;}
	if(angle2>90){res+=angle2-90;}else{res+=90-angle2;}
	if(angle3>90){res+=angle3-90;}else{res+=90-angle3;}
	if(angle4>90){res+=angle4-90;}else{res+=90-angle4;}
	if(angle5>90){res+=180-angle5;}else{res+=angle5;}
	if(angle6>90){res+=180-angle6;}else{res+=angle6;}
	return res;
}


__forceinline bool Test_Perspective_Parallel_Alignment_Rotation(float hr, float vr)
{
	float last_error = Get_Perspective_Parallel_Error();
	float oh = perspective_alignment_horizontal_rotation;
	float ov = perspective_alignment_vertical_rotation;
	perspective_alignment_horizontal_rotation += hr;
	perspective_alignment_vertical_rotation += vr;
	if(perspective_alignment_horizontal_rotation>180)perspective_alignment_horizontal_rotation -= 360;
	if(perspective_alignment_horizontal_rotation<-180)perspective_alignment_horizontal_rotation += 360;
	if(perspective_alignment_vertical_rotation>180)perspective_alignment_vertical_rotation -= 360;
	if(perspective_alignment_vertical_rotation<-180)perspective_alignment_vertical_rotation += 360;
	Update_Perspective_Alignment_Dir();
	float new_error = Get_Perspective_Parallel_Error();
	if(new_error>=last_error||new_error>lowest_perspective_alignment_error)
	{
		perspective_alignment_horizontal_rotation = oh;
		perspective_alignment_vertical_rotation = ov;
		Update_Perspective_Alignment_Dir();
		Project_To_Perspective_Alignment_Plane(pps_pos1.dir);
		Project_To_Perspective_Alignment_Plane(pps_pos2.dir);
		Project_To_Perspective_Alignment_Plane(pps_dir1.dir);
		Project_To_Perspective_Alignment_Plane(pps_dir2.dir);
		return false;
	}
	return true;
}

bool Update_Perspective_Parallel_Alignment()
{
	char etext[512];
	char netext[512];
	float oerror = Get_Perspective_Parallel_Error();
	bool ph = false;
	bool nh = false;
	bool pv = false;
	bool nv = false;
	bool res = true;

	if(align_perspective_left){ph = Test_Perspective_Parallel_Alignment_Rotation(perspective_alignment_inc, 0);}
	if(align_perspective_right){nh = Test_Perspective_Parallel_Alignment_Rotation(-perspective_alignment_inc, 0);}
	if(align_perspective_forward){pv = Test_Perspective_Parallel_Alignment_Rotation(0, perspective_alignment_inc);}
	if(align_perspective_back){nv = Test_Perspective_Parallel_Alignment_Rotation(0, -perspective_alignment_inc);}
	
	float nerror = Get_Perspective_Parallel_Error();
	sprintf(etext, "%f", oerror);
	sprintf(netext, "%f", nerror);
	if((!ph&&!nh&&!pv&&!nv)||(!strcmp(etext, netext)))
	{
		perspective_alignment_inc *= 0.5f;
		if(perspective_alignment_inc<EPSILON)
		{
			res = false;
		}
	}
	else
	{
		if(lowest_perspective_alignment_error>nerror)lowest_perspective_alignment_error = nerror;
	}
	float rot[3] = {perspective_alignment_horizontal_rotation,perspective_alignment_vertical_rotation, 0};
//	get_alignment_plane
	Set_Selection_Transform(perspective_alignment_center, perspective_alignment_dir, rot);
	lowest_perspective_alignment_error;
	return res;
}


__forceinline bool Test_Perspective_Rectangle_Alignment_Rotation(float hr, float vr)
{
	float last_error = Get_Perspective_Rectangle_Error();
	float oh = perspective_alignment_horizontal_rotation;
	float ov = perspective_alignment_vertical_rotation;
	perspective_alignment_horizontal_rotation += hr;
	perspective_alignment_vertical_rotation += vr;
	if(perspective_alignment_horizontal_rotation>180)perspective_alignment_horizontal_rotation -= 360;
	if(perspective_alignment_horizontal_rotation<-180)perspective_alignment_horizontal_rotation += 360;
	if(perspective_alignment_vertical_rotation>180)perspective_alignment_vertical_rotation -= 360;
	if(perspective_alignment_vertical_rotation<-180)perspective_alignment_vertical_rotation += 360;
	Update_Perspective_Alignment_Dir();
	float new_error = Get_Perspective_Rectangle_Error();
	if(new_error>=last_error||new_error>lowest_perspective_alignment_error)
	{
		perspective_alignment_horizontal_rotation = oh;
		perspective_alignment_vertical_rotation = ov;
		Update_Perspective_Alignment_Dir();
		Project_To_Perspective_Alignment_Plane(pps_pos1.dir);
		Project_To_Perspective_Alignment_Plane(pps_pos2.dir);
		Project_To_Perspective_Alignment_Plane(pps_dir1.dir);
		Project_To_Perspective_Alignment_Plane(pps_dir2.dir);
		return false;
	}
	return true;
}

bool Update_Perspective_Rectangle_Alignment()
{
	char etext[512];
	char netext[512];
	float oerror = Get_Perspective_Rectangle_Error();
	bool ph = false;
	bool nh = false;
	bool pv = false;
	bool nv = false;
	bool res = true;

	if(align_perspective_left){ph = Test_Perspective_Rectangle_Alignment_Rotation(perspective_alignment_inc, 0);}
	if(align_perspective_right){nh = Test_Perspective_Rectangle_Alignment_Rotation(-perspective_alignment_inc, 0);}
	if(align_perspective_forward){pv = Test_Perspective_Rectangle_Alignment_Rotation(0, perspective_alignment_inc);}
	if(align_perspective_back){nv = Test_Perspective_Rectangle_Alignment_Rotation(0, -perspective_alignment_inc);}
	
	float nerror = Get_Perspective_Rectangle_Error();
	sprintf(etext, "%f", oerror);
	sprintf(netext, "%f", nerror);
	if((!ph&&!nh&&!pv&&!nv)||(!strcmp(etext, netext)))
	{
		perspective_alignment_inc *= 0.5f;
		if(perspective_alignment_inc<EPSILON)
		{
			res = false;
		}
	}
	else
	{
		if(lowest_perspective_alignment_error>nerror)lowest_perspective_alignment_error = nerror;
	}
	float rot[3] = {perspective_alignment_horizontal_rotation,perspective_alignment_vertical_rotation, 0};
//	get_alignment_plane
	Set_Selection_Transform(perspective_alignment_center, perspective_alignment_dir, rot);
	lowest_perspective_alignment_error;
	return res;
}



//currently requires vanishing point and end points to be visible

bool Project_Parallel_Lines()
{
	Get_Control_Point_Vectors();

	float ep[3];
	Get_Selection_Origin_Center(ep);
	perspective_alignment_center[0] = ep[0];
	perspective_alignment_center[1] = ep[1];
	perspective_alignment_center[2] = ep[2];
	Update_Perspective_Alignment_Dir();

	perspective_alignment_inc = 1.0f;
	lowest_perspective_alignment_error = 360;

	iterate_perspective_alignment = true;
	return true;
}

bool Project_Rectangle_Lines()
{
	Get_Control_Point_Vectors();

	float ep[3];
	Get_Selection_Origin_Center(ep);
	perspective_alignment_center[0] = ep[0];
	perspective_alignment_center[1] = ep[1];
	perspective_alignment_center[2] = ep[2];
	Update_Perspective_Alignment_Dir();

	perspective_alignment_inc = 1.0f;
	lowest_perspective_alignment_error = 360;

	iterate_perspective_alignment = true;
	return true;
}

__forceinline float Get_Perspective_Corner_Angle()
{
	Project_To_Perspective_Alignment_Plane(pps_pos1.dir);
	Project_To_Perspective_Alignment_Plane(pps_pos2.dir);
	//get vectors to center position
	float v1[3];
	float v2[3];
	v1[0] = perspective_alignment_center[0]-pps_pos1.dir[0];
	v1[1] = perspective_alignment_center[1]-pps_pos1.dir[1];
	v1[2] = perspective_alignment_center[2]-pps_pos1.dir[2];
	v2[0] = perspective_alignment_center[0]-pps_pos2.dir[0];
	v2[1] = perspective_alignment_center[1]-pps_pos2.dir[1];
	v2[2] = perspective_alignment_center[2]-pps_pos2.dir[2];
	Normalize(v1);
	Normalize(v2);
	float dot = Dot(v1, v2);
	float angle = acos(dot);
	//used to convert degrees to radians, == PI/180
	const double r2d = (180.0/3.14159);
	angle *= r2d;
	return angle;
}

__forceinline float Get_Perspective_Corner_Rotation()
{
	Project_To_Perspective_Alignment_Plane(pps_pos1.dir);
	Project_To_Perspective_Alignment_Plane(pps_pos2.dir);
	//get vectors to center position
	float v1[3];
	float v2[3];
	v1[0] = perspective_alignment_center[0]-pps_pos1.dir[0];
	v1[1] = 0;
	v1[2] = perspective_alignment_center[2]-pps_pos1.dir[2];
	v2[0] = 0;
	v2[1] = 0;
	v2[2] = 1;
	Normalize(v1);
	Normalize(v2);
	float dot = Dot(v1, v2);
	float angle = acos(dot);
	//used to convert degrees to radians, == PI/180
	const double r2d = (180.0/3.14159);
	angle *= r2d;
	return angle;
}


__forceinline float Get_Perspective_Corner_Error()
{
	float angle = Get_Perspective_Corner_Angle();
	float target_angle = perspective_alignment_angle;
	if(angle>target_angle)
	{
		return angle-target_angle;
	}
	return target_angle - angle;
}

__forceinline bool Test_Perspective_Corner_Alignment_Rotation(float hr, float vr)
{
	float last_error = Get_Perspective_Corner_Error();
	float oh = perspective_alignment_horizontal_rotation;
	float ov = perspective_alignment_vertical_rotation;
	perspective_alignment_horizontal_rotation += hr;
	perspective_alignment_vertical_rotation += vr;
	if(perspective_alignment_horizontal_rotation>180)perspective_alignment_horizontal_rotation -= 360;
	if(perspective_alignment_horizontal_rotation<-180)perspective_alignment_horizontal_rotation += 360;
	if(perspective_alignment_vertical_rotation>180)perspective_alignment_vertical_rotation -= 360;
	if(perspective_alignment_vertical_rotation<-180)perspective_alignment_vertical_rotation += 360;
	Update_Perspective_Alignment_Dir();
	float new_error = Get_Perspective_Corner_Error();
	if(new_error>=last_error||new_error>lowest_perspective_alignment_error)
	{
		perspective_alignment_horizontal_rotation = oh;
		perspective_alignment_vertical_rotation = ov;
		Update_Perspective_Alignment_Dir();
		Project_To_Perspective_Alignment_Plane(pps_pos1.dir);
		Project_To_Perspective_Alignment_Plane(pps_pos2.dir);
		return false;
	}
	return true;
}


bool Update_Perspective_Corner_Alignment()
{
	char etext[512];
	char netext[512];
	float oerror = Get_Perspective_Corner_Error();
	bool ph = false;
	bool nh = false;
	bool pv = false;
	bool nv = false;
	bool res = true;

	if(align_perspective_left){ph = Test_Perspective_Corner_Alignment_Rotation(perspective_alignment_inc, 0);}
	if(align_perspective_right){nh = Test_Perspective_Corner_Alignment_Rotation(-perspective_alignment_inc, 0);}
	if(align_perspective_forward){pv = Test_Perspective_Corner_Alignment_Rotation(0, perspective_alignment_inc);}
	if(align_perspective_back){nv = Test_Perspective_Corner_Alignment_Rotation(0, -perspective_alignment_inc);}
	
	float nerror = Get_Perspective_Corner_Error();
	sprintf(etext, "%f", oerror);
	sprintf(netext, "%f", nerror);
	if((!ph&&!nh&&!pv&&!nv)||(!strcmp(etext, netext)))
	{
		perspective_alignment_inc *= 0.5f;
		if(perspective_alignment_inc<EPSILON)
		{
			res = false;
		}
	}
	else
	{
		if(lowest_perspective_alignment_error>nerror)lowest_perspective_alignment_error = nerror;
	}
	float rot[3] = {perspective_alignment_horizontal_rotation,perspective_alignment_vertical_rotation, 0};
	Set_Selection_Transform(perspective_alignment_center, perspective_alignment_dir, rot);
	lowest_perspective_alignment_error;
	return res;
}

bool Update_Perspective_FOV_Adjustment()
{
	float old_error = 0;
	float new_error = 0;
	Get_Control_Point_Vectors();
	if(perspective_projection_type==pps_ANGLE)old_error = Get_Perspective_Corner_Error();
	if(perspective_projection_type==pps_RECTANGLE)old_error = Get_Perspective_Rectangle_Error();
	if(perspective_projection_type==pps_PARALLEL)old_error = Get_Perspective_Parallel_Error();
	float old_fov = perspective_fov;//try positive fov change
	perspective_fov = old_fov+fov_alignment_inc;
	Get_Control_Point_Vectors();
	
	if(perspective_projection_type==pps_ANGLE)new_error = Get_Perspective_Corner_Error();
	if(perspective_projection_type==pps_RECTANGLE)new_error = Get_Perspective_Rectangle_Error();
	if(perspective_projection_type==pps_PARALLEL)new_error = Get_Perspective_Parallel_Error();
	if(new_error<old_error)
	{
		return true;//it worked
	}
	perspective_fov = old_fov-fov_alignment_inc;//try negative fov change
	Get_Control_Point_Vectors();
	
	if(perspective_projection_type==pps_ANGLE)new_error = Get_Perspective_Corner_Error();
	if(perspective_projection_type==pps_RECTANGLE)new_error = Get_Perspective_Rectangle_Error();
	if(perspective_projection_type==pps_PARALLEL)new_error = Get_Perspective_Parallel_Error();
	if(new_error<old_error)
	{
		return true;//it worked
	}
	//both failed
	fov_alignment_inc = fov_alignment_inc/2;
	if(fov_alignment_inc>EPSILON)
	{
		//smaller motion to test, keep going
		return true;
	}
	perspective_fov = old_fov;
	return false;//forget it
}

void Stop_Perspective_Alignment()
{
	iterate_perspective_alignment = false;
}

bool Update_Perspective_Alignment()
{
	bool res = false;
	bool rotres = false;
	if(adjustable_fov)
	{
		//check for differences in the printed version of the fov value
		//here we go
		char otext[32];//text string for fov before
		char ntext[32];//and after
		sprintf(otext, "%s", perspective_fov);
		res = Update_Perspective_FOV_Adjustment();
		sprintf(ntext, "%s", perspective_fov);
		if(!strcmp(otext, ntext))
		{
			//the fov didn't change enough to worry about, call it false
			res = false;
		}
		//BECAUSE the compiler optimization causes the function to always return true
		//even if there is no actual change, so if it doesn't get checked the alignment
		//goes on forever, making no adjustments to the fov OR returning "false" so
		//this function knows when to stop.

	}
	if(perspective_projection_type==pps_ANGLE)
	{
		rotres = Update_Perspective_Corner_Alignment();
	}
	if(perspective_projection_type==pps_RECTANGLE)
	{
		rotres = Update_Perspective_Rectangle_Alignment();
	}
	if(perspective_projection_type==pps_PARALLEL)
	{
		rotres = Update_Perspective_Parallel_Alignment();
	}
	if(!rotres&&!res)
	{
		Stop_Perspective_Alignment();
		Print_Status("Done.");
		SkinMsgBox("Perspective alignment done.");
		if(adjustable_fov)
		{
			iterate_perspective_alignment = false;
			Set_Fov(perspective_fov);
			Print_Status("Setting new FOV %f", perspective_fov);
		}
	}
	else
	{
		Print_Status("Testing alignment...");
	}
	return rotres;
}


bool Project_Corner_Angle()
{
	Get_Control_Point_Vectors();
	perspective_alignment_center[0] = pps_vanishing_point.dir[0]*100;
	perspective_alignment_center[1] = pps_vanishing_point.dir[1]*100;
	perspective_alignment_center[2] = pps_vanishing_point.dir[2]*100;
	Update_Perspective_Alignment_Dir();

	perspective_alignment_inc = 1.0f;
	lowest_perspective_alignment_error = 360;

	//get small vectors for each direction
	float v1[3];
	float v2[3];

	v1[0] = pps_vanishing_point.pos[0]-pps_dir1.pos[0];v1[1] = pps_vanishing_point.pos[1]-pps_dir1.pos[1];v1[2] = 0;
	v2[0] = pps_vanishing_point.pos[0]-pps_dir2.pos[0];v2[1] = pps_vanishing_point.pos[1]-pps_dir2.pos[1];v2[2] = 0;

	SetLength(v1, 1);
	SetLength(v2, 1);

	//get new pixel positions
	float vp1[2];
	float vp2[2];

	vp1[0] = pps_vanishing_point.pos[0]-v1[0];vp1[1] = pps_vanishing_point.pos[1]-v1[1];
	vp2[0] = pps_vanishing_point.pos[0]-v2[0];vp2[1] = pps_vanishing_point.pos[1]-v2[1];

	pps_pos1.pos[0] = vp1[0];pps_pos1.pos[1] = vp1[1];
	pps_pos2.pos[0] = vp2[0];pps_pos2.pos[1] = vp2[1];

	Get_Control_Point_Vectors();

	iterate_perspective_alignment = true;

	return true;
}

bool Apply_Perspective()
{
	fov_alignment_inc = 1.0f;
	perspective_fov = 45;//to start with 45 _fov;

	Print_Status("Starting alignment..");
	if(perspective_projection_type==pps_PARALLEL)
	{
		return Project_Parallel_Lines();
	}
	if(perspective_projection_type==pps_RECTANGLE)
	{
		return Project_Rectangle_Lines();
	}
	if(perspective_projection_type==pps_ANGLE)
	{
		return Project_Corner_Angle();
	}
	return false;
}



bool Align_Perspective_Forward()
{
	align_perspective_forward = true;
	align_perspective_back = true;
	align_perspective_left = false;
	align_perspective_right = false;
	perspective_alignment_horizontal_rotation = 0;
	perspective_alignment_vertical_rotation = 45;
	Apply_Perspective();
	return true;
}

bool Align_Perspective_Back()
{
	align_perspective_forward = true;
	align_perspective_back = true;
	align_perspective_left = false;
	align_perspective_right = false;
	perspective_alignment_horizontal_rotation = 0;
	perspective_alignment_vertical_rotation = -45;
	Apply_Perspective();
	return true;
}

bool Align_Perspective_Left()
{
	align_perspective_forward = false;
	align_perspective_back = false;
	align_perspective_left = true;
	align_perspective_right = true;
	perspective_alignment_horizontal_rotation = 45;
	perspective_alignment_vertical_rotation = 0;
	Apply_Perspective();
	return true;
}

bool Align_Perspective_Right()
{
	align_perspective_forward = false;
	align_perspective_back = false;
	align_perspective_left = true;
	align_perspective_right = true;
	perspective_alignment_horizontal_rotation = -45;
	perspective_alignment_vertical_rotation = 0;
	Apply_Perspective();
	return true;
}

bool Align_Perspective_All()
{
	align_perspective_forward = true;
	align_perspective_back = true;
	align_perspective_left = true;
	align_perspective_right = true;
	float rotation[3];
	if(!Get_Selection_Transform(perspective_alignment_center, perspective_alignment_dir, rotation))
	{
		return false;
	}
	perspective_alignment_horizontal_rotation = rotation[0];
	perspective_alignment_vertical_rotation = rotation[1];
	Apply_Perspective();
	return true;
}

//for the current frame
bool Set_Frame_Transform(float *pos, float *rot);

//for the virtual camera path
bool Set_Virtual_Camera(float *pos, float *rot);


bool Set_Grid_To_Perspective_Floor()
{
	float pos[3];
	float fpos[3];
	float normal[3];
	float rotation[3];
	if(!Get_Selection_Transform(pos, normal, rotation))
	{
		return false;
	}
	Get_Selection_Center(pos);
	fpos[0] = 0;
	fpos[1] = -pos[1];
	fpos[2] = 0;
	
	pos[0] = 0;
	pos[1] = 0;
	pos[2] = 0;

	float id[3] = {0,90,0};
	Get_Plane_Dir(id, normal);
	Set_Selection_Transform(pos, normal, id);
	float rot[3];
	rot[0] = -rotation[1];
	rot[1] = -rotation[0];
	rot[2] = -rotation[2];
	rot[0] += 90;
	Set_Frame_Transform(fpos, rot);
	Set_Virtual_Camera(pos, rot);
	Project_Layers();
	redraw_frame = true;
	return true;
}

bool Set_Grid_To_Perspective_Wall()
{
	float pos[3];
	float normal[3];
	float rotation[3];
	if(!Get_Selection_Transform(pos, normal, rotation))
	{
		return false;
	}
	Get_Selection_Center(pos);
	float id[3] = {0,0,0};
	Get_Plane_Dir(id, normal);
	Set_Selection_Transform(pos, normal, id);
	float rot[3];
	rot[0] = -rotation[1];
	rot[1] = -rotation[0];
	rot[2] = -rotation[2];
	
	//first apply the new rotation without any translation
	pos[0] = pos[1] = pos[2] = 0;
	Set_Frame_Transform(pos, rot);
	Set_Virtual_Camera(pos, rot);
	Project_Layers();

	//now we know how far above or below the new projected wall is from the zero groundplane

	Get_Selection_Bottom(pos);

	//now we have to move the camera vertically so the wall sits flat on the ground
	pos[0] = 0;
	pos[1] = -pos[1];
	pos[2] = 0;

	//and do it again
	Set_Frame_Transform(pos, rot);
	Set_Virtual_Camera(pos, rot);
	Project_Layers();
	redraw_frame = true;
	return true;
}


bool Set_Grid_To_Perspective_Origin()
{
	float id[3] = {0,0,0};
	Set_Frame_Transform(id, id);
	Project_Layers();
	redraw_frame = true;
	return true;
}



