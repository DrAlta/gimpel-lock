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
#include "ControlMesh.h"
#include <math.h>
#include <GL/gl.h>
#include <vector>

//FIXTHIS create separate control mesh delauney triangulation setup

using namespace std;

void Get_World_Pos(int x, int y, float depth, float *res);


int last_control_mesh_point_id = 0;

class CONTROL_MESH_POINT
{
public:
	CONTROL_MESH_POINT()
	{
	}
	~CONTROL_MESH_POINT()
	{
	}
	float px, py;
	float depth;
	float pos[3];
	float uv[2];
	int id;
	bool used;
};

class CONTROL_MESH_TRIANGLE
{
public:
	CONTROL_MESH_TRIANGLE()
	{
	}
	~CONTROL_MESH_TRIANGLE()
	{
	}
	int a, b, c;
};

class CONTROL_MESH
{
public:
	CONTROL_MESH()
	{
	}
	~CONTROL_MESH()
	{
	}
	bool Clear_Data()
	{
		cm_points.clear();
		cm_triangles.clear();
		return true;
	}
	bool Reset_Used_Points()
	{
		int n = cm_points.size();
		for(int i = 0;i<n;i++)
		{
			cm_points[i].used = false;
		}
		return true;
	}
	vector<CONTROL_MESH_POINT> cm_points;
	vector<CONTROL_MESH_TRIANGLE> cm_triangles;
};

CONTROL_MESH cmesh;

bool Reset_Control_Mesh()
{
	last_control_mesh_point_id = 0;
	cmesh.cm_points.clear();
	return true;
}

bool Update_Samples_From_Control_Mesh()
{
	return false;
}

bool Render_Control_Mesh()
{
	glBegin(GL_LINES);


	glEnd();
	return false;
}

float VecLength(float* v);


float Control_Point_Distance(int p1, int p2)
{
	float v[3];
	v[0] = cmesh.cm_points[p1].px-cmesh.cm_points[p2].px;
	v[1] = cmesh.cm_points[p1].py-cmesh.cm_points[p2].py;
	v[2] = 0;
	return VecLength(v);
}

bool Get_2_Closest_Points(int a, int *b, int *c)
{
	int best1 = -1;
	int best2 = -1;
	float lo1 = 100000;
	float lo2 = 100000;
	float d;
	int n = cmesh.cm_points.size();
	for(int i = 0;i<n;i++)
	{
		if(i!=a&&!cmesh.cm_points[i].used)
		{
			d = Control_Point_Distance(a, i);
			if(d<lo1)
			{
				if(best2==-1||lo2>lo1)
				{
					best2 = best1;
					lo2 = lo1;
				}
				best1 = i;
				lo1 = d;
			}
			else if(d<lo2)
			{
				best2 = i;
				lo2 = d;
			}
		}
	}
	if(best1==-1||best2==-1)
	{
		return false;
	}
	*b = best1;
	*c = best2;
	return true;
}

void Cross(float *v1, float *v2, float *v3)
{
	v3[0] = (v1[1]*v2[2])-(v1[2]*v2[1]);
	v3[1] = (v1[2]*v2[0])-(v1[0]*v2[2]);
	v3[2] = (v1[0]*v2[1])-(v1[1]*v2[0]);
}

int Side_Of_Plane(float *p, float *n, float *t)
{
	double xx = (double)t[0] - (double)p[0];
	double yy = (double)t[1] - (double)p[1];
	double zz = (double)t[2] - (double)p[2];
	if((((double)n[0] * xx) + ((double)n[1] * yy) + ((double)n[2] * zz))>0.0000){return 0;}
	return 1;
}

bool Render_Control_Mesh_Triangles_2D()
{
	int n = cmesh.cm_triangles.size();
	glBegin(GL_LINES);
	for(int i = 0;i<n;i++)
	{
		glVertex2f(cmesh.cm_points[cmesh.cm_triangles[i].a].px, cmesh.cm_points[cmesh.cm_triangles[i].a].py);
		glVertex2f(cmesh.cm_points[cmesh.cm_triangles[i].b].px, cmesh.cm_points[cmesh.cm_triangles[i].b].py);
		glVertex2f(cmesh.cm_points[cmesh.cm_triangles[i].b].px, cmesh.cm_points[cmesh.cm_triangles[i].b].py);
		glVertex2f(cmesh.cm_points[cmesh.cm_triangles[i].c].px, cmesh.cm_points[cmesh.cm_triangles[i].c].py);
		glVertex2f(cmesh.cm_points[cmesh.cm_triangles[i].c].px, cmesh.cm_points[cmesh.cm_triangles[i].c].py);
		glVertex2f(cmesh.cm_points[cmesh.cm_triangles[i].a].px, cmesh.cm_points[cmesh.cm_triangles[i].a].py);
	}
	glEnd();
	return true;
}

bool Render_Control_Mesh_Triangles_3D()
{
	return true;
}

bool Render_Control_Mesh_Points_2D()
{
	int n = cmesh.cm_points.size();
	glColor3f(1,0,0);
	glPointSize(4);
	glBegin(GL_POINTS);
	for(int i = 0;i<n;i++)
	{
		glVertex2f(cmesh.cm_points[i].px, cmesh.cm_points[i].py);//sx, sy);
	}
	glEnd();
	glPointSize(1);
	Render_Control_Mesh_Triangles_2D();
	return true;
}

bool Render_Control_Mesh_Points_3D()
{
	int n = cmesh.cm_points.size();
	glColor3f(1,0,0);
	glPointSize(4);
	glBegin(GL_POINTS);
	for(int i = 0;i<n;i++)
	{
		glVertex3fv(cmesh.cm_points[i].pos);
	}
	glEnd();
	glPointSize(1);
	return true;
}

int Find_Control_Mesh_Point_Index(float x, float y, float range)
{
	int n = cmesh.cm_points.size();
	int best = -1;
	float lx = range*2;
	float ly = range*2;
	float dx, dy;
	for(int i = 0;i<n;i++)
	{
		dx = cmesh.cm_points[i].px-x;if(dx<0)dx=-dx;
		dy = cmesh.cm_points[i].py-y;if(dy<0)dy=-dy;
		if(dx<range&&dy<range)
		{
			if(dx<lx||dy<ly)
			{
				lx = dx;
				ly = dy;
				best = i;
			}
		}
	}
	return best;
}

int Find_Control_Mesh_Point_Index(int id)
{
	int n = cmesh.cm_points.size();
	for(int i = 0;i<n;i++)
	{
		if(cmesh.cm_points[i].id==id)
		{
			return i;
		}
	}
	return -1;
}

int Add_Control_Mesh_Point(float x, float y)
{
	last_control_mesh_point_id++;
	CONTROL_MESH_POINT cmp;
	cmp.id = last_control_mesh_point_id;
	cmp.px = x;
	cmp.py = y;
	cmp.depth = 0.5f;
	Get_World_Pos((int)cmp.px, (int)cmp.py, cmp.depth, cmp.pos);
	cmesh.cm_points.push_back(cmp);
	return last_control_mesh_point_id;
}

bool Remove_Control_Mesh_Point(int id)
{
	int index = Find_Control_Mesh_Point_Index(id);
	if(index==-1)
	{
		return false;
	}
	cmesh.cm_points.erase(cmesh.cm_points.begin()+index);
	return true;
}

int Get_Selected_Control_Mesh_Point(float x, float y, float range)
{
	int index = Find_Control_Mesh_Point_Index(x, y, range);
	if(index==-1)
	{
		return -1;
	}
	return cmesh.cm_points[index].id;
}

float Get_Control_Mesh_Point_Depth(int id)
{
	int index = Find_Control_Mesh_Point_Index(id);
	if(index==-1)
	{
		return -1;
	}
	return cmesh.cm_points[index].depth;
}

bool Set_Control_Mesh_Point_Depth(int id, float depth)
{
	if(depth>=1)return false;
	if(depth<=0)return false;
	int index = Find_Control_Mesh_Point_Index(id);
	if(index==-1)
	{
		return false;
	}
	cmesh.cm_points[index].depth = depth;
	Get_World_Pos((int)cmesh.cm_points[index].px, (int)cmesh.cm_points[index].py, cmesh.cm_points[index].depth, cmesh.cm_points[index].pos);
	return true;
}

bool Init_Control_Mesh_Data()
{
	return true;
}

bool Free_Control_Mesh_Data()
{
	Reset_Control_Mesh();
	return true;
}

bool Add_Control_Point()
{
	return false;
}

bool Calculate_Angle_From_Control_Points()
{
	return false;
}
