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
#include "Puffit.h"
#include "Frame.h"
#include "Layers.h"
#include "Console.h"
#include "GLWnd.h"
#include <math.h>
#include <vector>
#include <algorithm>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>

using namespace std;


float VecLength(float* v);
void SetLength(float *v, float s);

void Cross(float *v1, float *v2, float *v3);
void Normalize(float *v);

extern float depth_increment;

bool render_puff_preview = false;
extern float plane_pos[3];
extern float plane_dir[3];

extern float depth_scale;

float puff_scale = 0.0f;

int puff_preview_spacing = 8;

float* Get_Layer_Primitive_Pos(int layer_index);
float* Get_Layer_Primitive_Dir(int layer_index);

float puff_preview_center[3] = {0,0,0};

extern float vertical_rotation;

float last_selection_puff_angle = 0;

struct PUFF_PIXEL
{
	int x, y;
	float linear_ratio;
	float spherical_ratio;
};

#define NUM_DIRECTIONAL_PIXELS 20

PUFF_PIXEL directional_pixels[NUM_DIRECTIONAL_PIXELS];

bool Puff_Layer(int layer_index, int search_range, float puff_scale, float puff_range, int puff_type, bool directional, float angle);

int current_puff_type = PUFF_SPHERICAL;

float directional_puff_angle = 180;

int puff_search_range = -1;

float puff_range = 1;


bool directional_extrusion = false;

#define RAD 0.017453f

void Rotate_Point(float *x, float *y, float angle)
{
	float c = (float)cos(angle*(3.14159f/180.0f));
	float s = (float)sin(angle*(3.14159f/180.0f));
	float tx = *x*c - *y*s;
	float ty = *x*s + *y*c;
	*x = tx;
	*y = ty;
}

//resets an array of vectors that repeat a partoicular direction
//for directional puff searches
void Reset_Directional_Pixels()
{
	int n = NUM_DIRECTIONAL_PIXELS;
	float x = 1;
	float y = 0;
	Rotate_Point(&x, &y, -directional_puff_angle);
	int i;
	float p;
	for(i = 0;i<n;i++)
	{
		directional_pixels[i].x = x*(i+1);
		directional_pixels[i].y = y*(i+1);
		p = ((float)i)/(n-1);
		directional_pixels[i].linear_ratio = p;
		float angle = (directional_pixels[i].linear_ratio*90)*RAD;
		directional_pixels[i].spherical_ratio = sin(angle);
	}
}


//creates a lookup table for a range of pixels
bool Create_Puff_Search_Info(int range)
{
	puff_search_range = range;
	Reset_Directional_Pixels();
	return true;
}

bool Free_Puff_Search_Info()
{
	return true;
}

float fastsqrt(float n);

vector<PUFF_PIXEL> puff_edge_pixels;

__forceinline void Add_Puff_Edge_Pixel(int x, int y)
{
	PUFF_PIXEL pp;
	pp.x = x;
	pp.y = y;
	puff_edge_pixels.push_back(pp);
}

__forceinline void Try_Puff_Edge_Pixel(int x, int y, int layer_id)
{
	if(frame->Get_Pixel_Layer(x, y)!=layer_id)
	{
		Add_Puff_Edge_Pixel(x, y);
	}
}

bool Find_Puff_Edge_Pixels(int layer_id)
{
	int lindex = Get_Layer_Index(layer_id);
	int ns = Num_RLE_Strips_In_Layer(lindex);
	RLE_STRIP *rs;
	int i, j;
	for(i = 0;i<ns;i++)
	{
		rs = Get_RLE_Strip(lindex, i);
		//check before and after
		if(frame->IsValidPixel(rs->start_x-1, rs->y))
		{
			Try_Puff_Edge_Pixel(rs->start_x-1, rs->y, layer_id);
		}
		else
		{
			Add_Puff_Edge_Pixel(rs->start_x-1, rs->y);
		}
		//and after
		if(frame->IsValidPixel(rs->end_x, rs->y))
		{
			Try_Puff_Edge_Pixel(rs->end_x, rs->y, layer_id);
		}
		else
		{
			Add_Puff_Edge_Pixel(rs->end_x, rs->y);
		}
		//then above and below
		bool top_valid = frame->IsValidPixel(rs->start_x, rs->y-1);
		bool bottom_valid = frame->IsValidPixel(rs->start_x, rs->y+1);
		for(j = rs->start_x;j<rs->end_x;j++)
		{
			if(top_valid)
			{
				Try_Puff_Edge_Pixel(j, rs->y-1, layer_id);
			}
			else
			{
				Add_Puff_Edge_Pixel(j, rs->y-1);
			}
			if(bottom_valid)
			{
				Try_Puff_Edge_Pixel(j, rs->y+1, layer_id);
			}
			else
			{
				Add_Puff_Edge_Pixel(j, rs->y+1);
			}
		}
	}
	return true;
}

bool Clear_Puff_Edge_Pixels()
{
	puff_edge_pixels.clear();
	return false;
}

__forceinline PUFF_PIXEL* Get_Closest_Puff_Edge_Pixel(int x, int y)
{
	PUFF_PIXEL *best = 0;
	PUFF_PIXEL *pp;
	float lowest = 0;
	float d;
	int n = puff_edge_pixels.size();
	float dx, dy;
	for(int i = 0;i<n;i++)
	{
		pp = &puff_edge_pixels[i];
		dx = pp->x-x;
		dy = pp->y-y;
		d = (dx*dx)+(dy*dy);
		if(d<lowest||!best)
		{
			best = pp;
			lowest = d;
		}
	}
	if(!best)
	{
		return 0;
	}
	d = fastsqrt(lowest);
	if(d>puff_search_range)
	{
		best->linear_ratio = 1;
	}
	else
	{
		best->linear_ratio = d/puff_search_range;
	}
	return best;
}

//gets the pre-calculated puff strength of a pixel xy for the specified layer
//at lookup offset "n"
__forceinline float Spherical_Puff_Strength(int x, int y, int layer_id)
{
	PUFF_PIXEL *pp = Get_Closest_Puff_Edge_Pixel(x, y);
	if(!pp){return 0;}
	return sin((pp->linear_ratio*90)*RAD);
}

//gets the pre-calculated puff strength of a pixel xy for the specified layer
//at lookup offset "n"
__forceinline float Linear_Puff_Strength(int x, int y, int layer_id)//, int n)
{
	PUFF_PIXEL *pp = Get_Closest_Puff_Edge_Pixel(x, y);
	if(!pp){return 0;}
	return pp->linear_ratio;
}


//get the maximum steps away from the pixel xy possible until another layer is hit
__forceinline int Get_Positive_Directional_Distance(int x, int y, int layer_id, int n, int *tx, int *ty)
{
	int cnt = 0;
	while(1)
	{
		for(int i = 0;i<n;i++)
		{
			int rx = x + directional_pixels[i].x;
			int ry = y + directional_pixels[i].y;
			cnt++;
			if(frame->Get_Pixel_Layer(rx, ry)!=layer_id)
			{
				//hit another layer, this is as far as we go
				return cnt;
			}
			*tx = rx;
			*ty = ry;
		}
		//made it through the lookup table, advance the starting point and try again
		x += directional_pixels[n-1].x;
		y += directional_pixels[n-1].y;
	}
	return -1;
}

//get the maximum steps away from the pixel xy possible until another layer is hit
__forceinline int Get_Negative_Directional_Distance(int x, int y, int layer_id, int n, int *tx, int *ty)
{
	int cnt = 0;
	while(1)
	{
		for(int i = 0;i<n;i++)
		{
			int rx = x - directional_pixels[i].x;
			int ry = y - directional_pixels[i].y;
			cnt++;
			if(frame->Get_Pixel_Layer(rx, ry)!=layer_id)
			{
				//hit another layer, this is as far as we go
				return cnt;
			}
			*tx = rx;
			*ty = ry;
		}
		//made it through the lookup table, advance the starting point and try again
		x -= directional_pixels[n-1].x;
		y -= directional_pixels[n-1].y;
	}
	return -1;
}

//gets the spherical puff strength of xy when the distribution is biased along a directional axis
__forceinline float Directional_Spherical_Puff_Strength(int x, int y, int layer_id, int n)
{
	int tx1 = 0;
	int ty1 = 0;
	int tx2 = 0;
	int ty2 = 0;
	//find the positive and negative extents perpendicular to the central axis
	int nd = Get_Negative_Directional_Distance(x, y, layer_id, n, &tx1, &ty1);
	int pd = Get_Positive_Directional_Distance(x, y, layer_id, n, &tx2, &ty2);
	//get the projection vectors at the edge pixels
	float *d1 = frame->Get_Frame_Vector(tx1, ty1);
	float *d2 = frame->Get_Frame_Vector(tx2, ty2);
	float p1[3] = {d1[0], d1[1], d1[2]};
	float p2[3] = {d2[0], d2[1], d2[2]};
	SetLength(p1, 1);
	SetLength(p2, 1);
	float v[3] = {p1[0]-p2[0],p1[1]-p2[1],p1[2]-p2[2]};
	float distance = VecLength(v);
	float t = pd+nd;//new "total search range
	float p;
	//percentage p will be whichever is closest to an edge
	if(pd>nd)
	{
		//use nd, closest
		p = ((float)nd)/t;
	}
	else
	{
		//use pd, closest
		p = ((float)pd)/t;
	}
	//find closest index into directional pixels by percent
	p *= 2;
	float s = sin((p*90)*RAD);
	s = s*((distance/2));
	p = p/4;
	float d = cos((p*90)*RAD);
	s *= d;
	return s*10;
}

//gets the linear puff strength of xy when the distribution is biased along a directional axis
__forceinline float Directional_Linear_Puff_Strength(int x, int y, int layer_id, int n)
{
	int tx1 = 0;
	int ty1 = 0;
	int tx2 = 0;
	int ty2 = 0;
	//find the positive and negative extents perpendicular to the central axis
	int nd = Get_Negative_Directional_Distance(x, y, layer_id, n, &tx1, &ty1);
	int pd = Get_Positive_Directional_Distance(x, y, layer_id, n, &tx2, &ty2);
	//use whichever is closest to an edge
	float d = 0;
	if(pd>nd)
	{
		//use nd
		d = nd;
	}
	else
	{
		//use pd
		d = pd;
	}
	if(d>=puff_search_range)
	{
		return 1;
	}
	return d/puff_search_range;
}

//gets the puff strength of a particular pixel
float __Puff_Strength(int x, int y, int type, bool directional)
{
	int layer_id = 0;
	Get_Frame_Pixel_Layer(x, y, &layer_id);
	if(directional)
	{
		int n = NUM_DIRECTIONAL_PIXELS;
		switch(type)
		{
			case PUFF_SPHERICAL: return Directional_Spherical_Puff_Strength(x, y, layer_id, n);
			case PUFF_LINEAR: return Directional_Linear_Puff_Strength(x, y, layer_id, n);
		}
	}
	switch(type)
	{
		case PUFF_SPHERICAL: return Spherical_Puff_Strength(x, y, layer_id);
		case PUFF_LINEAR: return Linear_Puff_Strength(x, y, layer_id);
	}
	return 0;
}

void ReSizeGLScene(int width, int height);
void Get_Matrices();

bool Set_Puff_Search_Range(int range)
{
	if(puff_search_range==range)return true;
	Free_Puff_Search_Info();
	Create_Puff_Search_Info(range);
	return true;
}

__forceinline float Dist_To_Plane(float *p, float *tp, float *n)
{
	float r = (((p[0]-tp[0])*n[0])+((p[1]-tp[1])*n[1])+((p[2]-tp[2])*n[2]));
	if(r<0)return -r;
	return r;
}


__forceinline void Project_Puff_Preview_Vertex_To_Plane(float *old_pos, float *new_pos, float *pos, float *dir)
{
	float far_pos[3];
	float center[3] = {0,0,0};
	float d = 100000;
	far_pos[0] = old_pos[0] * d;
	far_pos[1] = old_pos[1] * d;
	far_pos[2] = old_pos[2] * d;
	float near_dist = Dist_To_Plane(center, pos, dir);
	float far_dist = Dist_To_Plane(far_pos, pos, dir);
	float total = near_dist+far_dist;
	float p = near_dist/total;
	new_pos[0] = far_pos[0] * p;
	new_pos[1] = far_pos[1] * p;
	new_pos[2] = far_pos[2] * p;
}


struct PREVIEW_PUFF_PIXEL
{
	int x, y;
	float ratio;
	float *src;
	float *plane_pos;
	float *plane_dir;
	float pos[3];
};

vector<PREVIEW_PUFF_PIXEL> preview_puff_pixels;

//puffs "pos" out by "dir" scaled by "puff" amount, saving result in "res"
__forceinline void Get_Puff_Position(float *pos, float *dir, float *res, float puff)
{
	res[0] = pos[0] + (dir[0]*puff);
	res[1] = pos[1] + (dir[1]*puff);
	res[2] = pos[2] + (dir[2]*puff);
}

//puffs out a preview vertex from the layer plane
__forceinline void Puff_Preview_Puff_Pixel(PREVIEW_PUFF_PIXEL *pp)
{
	Project_Puff_Preview_Vertex_To_Plane(pp->src, pp->pos, pp->plane_pos, pp->plane_dir);
	float puff = (1.0f+(puff_scale*pp->ratio));
	pp->pos[0] *= puff;
	pp->pos[1] *= puff;
	pp->pos[2] *= puff;
}

//puffs out the preview vertices from the layer plane
void Puff_Preview_Pixels()
{
	int n = preview_puff_pixels.size();
	for(int i = 0;i<n;i++)
	{
		Puff_Preview_Puff_Pixel(&preview_puff_pixels[i]);
	}
}

void Free_Preview_Puff_Pixels()
{
	preview_puff_pixels.clear();
}

//re-centers the "starting point" of the puff (where max puff strength occurs)
void Update_Puff_Preview_Center()
{
	Get_Selection_Origin_Center(puff_preview_center);
}

//creates puff preview vertices for the specified layer
void Get_Layer_Preview_Puff_Pixels(int index)
{
	Find_Puff_Edge_Pixels(Get_Layer_ID(index));
	RLE_STRIP *rs;
	PREVIEW_PUFF_PIXEL pp;
	int n = Num_RLE_Strips_In_Layer(index);
	float *layer_primitive_pos = Get_Layer_Primitive_Pos(index);
	float *layer_primitive_dir = Get_Layer_Primitive_Dir(index);
	for(int i = 0;i<n;i+=puff_preview_spacing)
	{
		rs = Get_RLE_Strip(index, i);
		pp.y = rs->y;
		for(int j = rs->start_x;j<rs->end_x;j+=puff_preview_spacing)
		{
			pp.x = j;
			pp.ratio = __Puff_Strength(pp.x, pp.y, current_puff_type, directional_extrusion);
			pp.src = frame->Get_Pos(pp.x, pp.y);
			pp.plane_pos = layer_primitive_pos;
			pp.plane_dir = layer_primitive_dir;
			preview_puff_pixels.push_back(pp);
		}
		pp.x = rs->end_x-1;
		pp.ratio = __Puff_Strength(pp.x, pp.y, current_puff_type, directional_extrusion);
		pp.src = frame->Get_Pos(pp.x, pp.y);
		pp.plane_pos = layer_primitive_pos;
		pp.plane_dir = layer_primitive_dir;
		preview_puff_pixels.push_back(pp);
	}
	Clear_Puff_Edge_Pixels();
	Update_Puff_Preview_Center();
}

//create puff preview vertices for all selected layers
void Get_Preview_Puff_Pixels()
{
	Free_Preview_Puff_Pixels();
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Get_Layer_Preview_Puff_Pixels(i);
		}
	}
}

//render a line to specify the directional bias
void Render_Puff_Direction()
{
	int n = NUM_DIRECTIONAL_PIXELS;
	float dir[3];
	float ww = Frame_World_Width();
	float wh = Frame_World_Height();
	float s = 0.001f;
	dir[0] = directional_pixels[n-1].y*s;
	dir[1] = -directional_pixels[n-1].x*s;
	dir[2] = 0;

	glPushMatrix();

	float puff = 1.0f+puff_scale;

	glTranslatef(puff_preview_center[0]*puff,puff_preview_center[1]*puff,puff_preview_center[2]*puff);

	float tdir[3];
	float rot[3];
	int type;
	Get_Selection_Rotation(tdir, rot, &type);
	glRotatef(rot[1], 1, 0, 0);

	glLineWidth(4);
	glColor3f(0,0,1);
	glBegin(GL_LINES);
	glVertex3f(dir[0],dir[1],dir[2]);
	glVertex3f(-dir[0],-dir[1],-dir[2]);
	glEnd();
	glPopMatrix();
	glLineWidth(1);
	glColor3f(1,1,1);
}

//render the puff preview vertices
void Render_Puff_Preview()
{
	int i;
	//render puff preview vertices projected from the selected layer plane
	Puff_Preview_Pixels();
	glColor3f(0,1,0);

	glPushMatrix();
	int n = preview_puff_pixels.size();
	glPointSize(4);
	glBegin(GL_POINTS);
	for(i = 0;i<n;i++)
	{
		glVertex3fv(preview_puff_pixels[i].pos);
	}
	glEnd();
	glPopMatrix();
	glPointSize(1);
	if(directional_extrusion)Render_Puff_Direction();
}

//apply the puff edits to the selected layer(s)
bool Puff_Selection()
{
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Puff_Layer(i, puff_search_range, puff_scale, puff_range, current_puff_type, directional_extrusion, directional_puff_angle);
			session_altered = true;
			redraw_frame = true;
		}
	}
	return true;
}

//////////////////////////////////////////
//set puff options from the gui

void Set_Spherical_Extrusion()
{
	if(current_puff_type==PUFF_SPHERICAL)
	{
		return;
	}
	current_puff_type = PUFF_SPHERICAL;
	Get_Preview_Puff_Pixels();
}

void Set_Linear_Extrusion()
{
	if(current_puff_type==PUFF_LINEAR)
	{
		return;
	}
	current_puff_type = PUFF_LINEAR;
	Get_Preview_Puff_Pixels();
}

void Set_Directional_Extrusion(bool b)
{
	if(directional_extrusion==b)return;
	directional_extrusion = b;
	Get_Preview_Puff_Pixels();
}

void Set_Directional_Extrusion_Angle(float angle)
{
	if(angle<0||angle>360)return;
	directional_puff_angle = angle;
	if(directional_extrusion)
	{
		Reset_Directional_Pixels();
		Get_Preview_Puff_Pixels();
	}
}




