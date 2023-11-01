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
#ifndef GL_BASIC_H
#define GL_BASIC_H

#include <windows.h>
#include <stdio.h>
#include <GL\glut.h>
#include <GL\gl.h>
#include <math.h>
#include <vector>

extern unsigned int fast_sqrt_table[0x10000];
#define FP_BITS(fp) (*(DWORD *)&(fp))

float fastsqrt(float n);

__forceinline void FloatToInt(int *int_pointer, float f) 
{
	__asm  fld  f
  __asm  mov  edx,int_pointer
  __asm  FRNDINT
  __asm  fistp dword ptr [edx];

}


using namespace std;

#define INSIDE_FRUSTUM 1
#define OUTSIDE_FRUSTUM -1
#define INTERSECTS_FRUSTUM 0

#define RAD 0.017453f

class BB_MATRIX
{
public:
	BB_MATRIX()
	{
	}
	~BB_MATRIX()
	{
	}
	__forceinline void Identity(float mat[4][4])
	{
		mat[0][0]=1; mat[0][1]=0; mat[0][2]=0; mat[0][3]=0;
		mat[1][0]=0; mat[1][1]=1; mat[1][2]=0; mat[1][3]=0;
		mat[2][0]=0; mat[2][1]=0; mat[2][2]=1; mat[2][3]=0;
		mat[3][0]=0; mat[3][1]=0; mat[3][2]=0; mat[3][3]=1;
	}
	__forceinline void Multiply(float mat1[4][4], float mat2[4][4], float mat3[4][4])
	{
		int i,j;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
				mat3[i][j]=mat1[i][0]*mat2[0][j]+
				mat1[i][1]*mat2[1][j]+
				mat1[i][2]*mat2[2][j]+
				mat1[i][3]*mat2[3][j];
	}
	__forceinline void Translate(float matrix[4][4], float* t)
	{
		float tmat[4][4];
		float mat1[4][4];
		Identity(mat1);
		tmat[0][0]=1;  tmat[0][1]=0;  tmat[0][2]=0;  tmat[0][3]=0;
		tmat[1][0]=0;  tmat[1][1]=1;  tmat[1][2]=0;  tmat[1][3]=0;
		tmat[2][0]=0;  tmat[2][1]=0;  tmat[2][2]=1;  tmat[2][3]=0;
		tmat[3][0]=t[0]; tmat[3][1]=t[1]; tmat[3][2]=t[2]; tmat[3][3]=1;
		Multiply(matrix,tmat,mat1);
		Copy(mat1,matrix);
	}
	__forceinline void Copy(float s[4][4],float d[4][4])
	{
		int i,j;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
				d[i][j]=s[i][j];
	}
	__forceinline void SetZXY(float m[4][4], float* t, float* r)
	{
		float a[3] = {r[0]*RAD,r[1]*RAD,r[2]*RAD};
		float xmat[4][4], ymat[4][4], zmat[4][4];
		xmat[0][0]=1;        xmat[0][1]=0;        xmat[0][2]=0;
		xmat[0][3]=0;
		xmat[1][0]=0;        xmat[1][1]=(float)cos(a[0]);  xmat[1][2]=(float)sin(a[0]);
		xmat[1][3]=0;
		xmat[2][0]=0;        xmat[2][1]=-(float)sin(a[0]); xmat[2][2]=(float)cos(a[0]);
		xmat[2][3]=0;
		xmat[3][0]=0;        xmat[3][1]=0;        xmat[3][2]=0;
		xmat[3][3]=1;
		
		ymat[0][0]=(float)cos(a[1]);  ymat[0][1]=0;        ymat[0][2]=-(float)sin(a[1]);
		ymat[0][3]=0;
		ymat[1][0]=0;        ymat[1][1]=1;        ymat[1][2]=0;
		ymat[1][3]=0;
		ymat[2][0]=(float)sin(a[1]);  ymat[2][1]=0;        ymat[2][2]=(float)cos(a[1]);
		ymat[2][3]=0;
		ymat[3][0]=0;        ymat[3][1]=0;        ymat[3][2]=0;
		ymat[3][3]=1;
		
		zmat[0][0]=(float)cos(a[2]);  zmat[0][1]=(float)sin(a[2]);  zmat[0][2]=0;
		zmat[0][3]=0;
		zmat[1][0]=-(float)sin(a[2]); zmat[1][1]=(float)cos(a[2]);  zmat[1][2]=0;
		zmat[1][3]=0;
		zmat[2][0]=0;        zmat[2][1]=0;        zmat[2][2]=1;
		zmat[2][3]=0;
		zmat[3][0]=0;        zmat[3][1]=0;        zmat[3][2]=0;
		zmat[3][3]=1;
		float mat1[4][4];
		float mat2[4][4];
		Identity(m);
		Identity(mat1);
		Identity(mat2);
		Multiply(m,zmat,mat1);
		Multiply(mat1,xmat,mat2);
		Multiply(mat2,ymat,m);
		if(t){m[3][0]=t[0]; m[3][1]=t[1]; m[3][2]=t[2]; m[3][3]=1;}
	}
	__forceinline void Set(float m[4][4], float* t, float* r)
	{
		float a[3] = {r[0]*RAD,r[1]*RAD,r[2]*RAD};
		float xmat[4][4], ymat[4][4], zmat[4][4];
		xmat[0][0]=1;        xmat[0][1]=0;        xmat[0][2]=0;
		xmat[0][3]=0;
		xmat[1][0]=0;        xmat[1][1]=(float)cos(a[0]);  xmat[1][2]=(float)sin(a[0]);
		xmat[1][3]=0;
		xmat[2][0]=0;        xmat[2][1]=-(float)sin(a[0]); xmat[2][2]=(float)cos(a[0]);
		xmat[2][3]=0;
		xmat[3][0]=0;        xmat[3][1]=0;        xmat[3][2]=0;
		xmat[3][3]=1;
		
		ymat[0][0]=(float)cos(a[1]);  ymat[0][1]=0;        ymat[0][2]=-(float)sin(a[1]);
		ymat[0][3]=0;
		ymat[1][0]=0;        ymat[1][1]=1;        ymat[1][2]=0;
		ymat[1][3]=0;
		ymat[2][0]=(float)sin(a[1]);  ymat[2][1]=0;        ymat[2][2]=(float)cos(a[1]);
		ymat[2][3]=0;
		ymat[3][0]=0;        ymat[3][1]=0;        ymat[3][2]=0;
		ymat[3][3]=1;
		
		zmat[0][0]=(float)cos(a[2]);  zmat[0][1]=(float)sin(a[2]);  zmat[0][2]=0;
		zmat[0][3]=0;
		zmat[1][0]=-(float)sin(a[2]); zmat[1][1]=(float)cos(a[2]);  zmat[1][2]=0;
		zmat[1][3]=0;
		zmat[2][0]=0;        zmat[2][1]=0;        zmat[2][2]=1;
		zmat[2][3]=0;
		zmat[3][0]=0;        zmat[3][1]=0;        zmat[3][2]=0;
		zmat[3][3]=1;
		float mat1[4][4];
		float mat2[4][4];
		Identity(m);
		Identity(mat1);
		Identity(mat2);
		Multiply(m,ymat,mat1);
		Multiply(mat1,xmat,mat2);
		Multiply(mat2,zmat,m);
		if(t){m[3][0]=t[0]; m[3][1]=t[1]; m[3][2]=t[2]; m[3][3]=1;}
	}
	__forceinline void ISet(float m[4][4], float* t, float* r)
	{
		float a[3] = {r[0]*RAD,r[1]*RAD,r[2]*RAD};
		float xmat[4][4], ymat[4][4], zmat[4][4];
		xmat[0][0]=1;        xmat[0][1]=0;        xmat[0][2]=0;
		xmat[0][3]=0;
		xmat[1][0]=0;        xmat[1][1]=(float)cos(a[0]);  xmat[1][2]=(float)sin(a[0]);
		xmat[1][3]=0;
		xmat[2][0]=0;        xmat[2][1]=-(float)sin(a[0]); xmat[2][2]=(float)cos(a[0]);
		xmat[2][3]=0;
		xmat[3][0]=0;        xmat[3][1]=0;        xmat[3][2]=0;
		xmat[3][3]=1;
		
		ymat[0][0]=(float)cos(a[1]);  ymat[0][1]=0;        ymat[0][2]=-(float)sin(a[1]);
		ymat[0][3]=0;
		ymat[1][0]=0;        ymat[1][1]=1;        ymat[1][2]=0;
		ymat[1][3]=0;
		ymat[2][0]=(float)sin(a[1]);  ymat[2][1]=0;        ymat[2][2]=(float)cos(a[1]);
		ymat[2][3]=0;
		ymat[3][0]=0;        ymat[3][1]=0;        ymat[3][2]=0;
		ymat[3][3]=1;
		
		zmat[0][0]=(float)cos(a[2]);  zmat[0][1]=(float)sin(a[2]);  zmat[0][2]=0;
		zmat[0][3]=0;
		zmat[1][0]=-(float)sin(a[2]); zmat[1][1]=(float)cos(a[2]);  zmat[1][2]=0;
		zmat[1][3]=0;
		zmat[2][0]=0;        zmat[2][1]=0;        zmat[2][2]=1;
		zmat[2][3]=0;
		zmat[3][0]=0;        zmat[3][1]=0;        zmat[3][2]=0;
		zmat[3][3]=1;
		float mat1[4][4];
		float mat2[4][4];
		Identity(m);
		Identity(mat1);
		Identity(mat2);
		Multiply(m,zmat,mat1);
		Multiply(mat1,xmat,mat2);
		Multiply(mat2,ymat,m);
		if(t){m[3][0]=t[0]; m[3][1]=t[1]; m[3][2]=t[2]; m[3][3]=1;}
	}
	__forceinline void Transform_Vertex(float m[4][4], float* s)
	{
		float t[3];
		t[0]=s[0]*m[0][0]+
			s[1]*m[1][0]+
			s[2]*m[2][0]+
			m[3][0];
		t[1]=s[0]*m[0][1]+
			s[1]*m[1][1]+
			s[2]*m[2][1]+
			m[3][1];
		t[2]=s[0]*m[0][2]+
			s[1]*m[1][2]+
			s[2]*m[2][2]+
			m[3][2];
		s[0] = t[0];
		s[1] = t[1];
		s[2] = t[2];
	}
	__forceinline float* XVector()
	{
		return &m[0][0];
	}
	__forceinline float* YVector()
	{
		return &m[1][0];
	}
	__forceinline float* ZVector()
	{
		return &m[2][0];
	}
	float m[4][4];
};

class BOUNDING_BOX
{
public:
	BOUNDING_BOX()
	{
		lul = &vertices[0];
		lur = &vertices[3];
		lll = &vertices[6];
		llr = &vertices[9];
		hul = &vertices[12];
		hur = &vertices[15];
		hll = &vertices[18];
		hlr = &vertices[21];
	}
	~BOUNDING_BOX()
	{
	}
	void Set_Transform(float *_size, float *_pos, float *_rot)
	{
		size[0] = _size[0];
		size[1] = _size[1];
		size[2] = _size[2];
		pos[0] = _pos[0];
		pos[1] = _pos[1];
		pos[2] = _pos[2];
		rot[0] = _rot[0];
		rot[1] = _rot[1];
		rot[2] = _rot[2];
		lul[0] = -size[0];lul[1] = -size[1];lul[2] = -size[2];
		lur[0] = +size[0];lur[1] = -size[1];lur[2] = -size[2];
		lll[0] = -size[0];lll[1] = -size[1];lll[2] = +size[2];
		llr[0] = +size[0];llr[1] = -size[1];llr[2] = +size[2];
		hul[0] = -size[0];hul[1] = +size[1];hul[2] = -size[2];
		hur[0] = +size[0];hur[1] = +size[1];hur[2] = -size[2];
		hll[0] = -size[0];hll[1] = +size[1];hll[2] = +size[2];
		hlr[0] = +size[0];hlr[1] = +size[1];hlr[2] = +size[2];
		top_normal[0] = 0;
		top_normal[1] = 1;
		top_normal[2] = 0;
		bottom_normal[0] = 0;
		bottom_normal[1] = -1;
		bottom_normal[2] = 0;
		left_normal[0] = -1;
		left_normal[1] = 0;
		left_normal[2] = 0;
		right_normal[0] = 1;
		right_normal[1] = 0;
		right_normal[2] = 0;
		front_normal[0] = 0;
		front_normal[1] = 0;
		front_normal[2] = 1;
		back_normal[0] = 0;
		back_normal[1] = 0;
		back_normal[2] = -1;
		aabb_lo[0] = pos[0];
		aabb_lo[1] = pos[1];
		aabb_lo[2] = pos[2];
		aabb_hi[0] = pos[0];
		aabb_hi[1] = pos[1];
		aabb_hi[2] = pos[2];
		float irot[3] = {-rot[0],-rot[1],-rot[2]};
		mat.Set(mat.m, 0, rot);
		imat.ISet(imat.m, 0, irot);
		float *v;
		for(int i = 0;i<8;i++)
		{
			v = &vertices[i*3];
			mat.Transform_Vertex(mat.m, v);
			v[0] += pos[0];
			v[1] += pos[1];
			v[2] += pos[2];
			if(aabb_lo[0]>v[0])aabb_lo[0]=v[0];
			if(aabb_hi[0]<v[0])aabb_hi[0]=v[0];
			if(aabb_lo[1]>v[1])aabb_lo[1]=v[1];
			if(aabb_hi[1]<v[1])aabb_hi[1]=v[1];
			if(aabb_lo[2]>v[2])aabb_lo[2]=v[2];
			if(aabb_hi[2]<v[2])aabb_hi[2]=v[2];
		}
		mat.Transform_Vertex(mat.m, top_normal);
		mat.Transform_Vertex(mat.m, bottom_normal);
		mat.Transform_Vertex(mat.m, left_normal);
		mat.Transform_Vertex(mat.m, right_normal);
		mat.Transform_Vertex(mat.m, front_normal);
		mat.Transform_Vertex(mat.m, back_normal);
	}
	void Render_Outline()
	{
		glVertex3fv(hul);
		glVertex3fv(hur);
		glVertex3fv(hur);
		glVertex3fv(hlr);
		glVertex3fv(hlr);
		glVertex3fv(hll);
		glVertex3fv(hll);
		glVertex3fv(hul);
		
		glVertex3fv(lul);
		glVertex3fv(lur);
		glVertex3fv(lur);
		glVertex3fv(llr);
		glVertex3fv(llr);
		glVertex3fv(lll);
		glVertex3fv(lll);
		glVertex3fv(lul);
		
		glVertex3fv(hul);
		glVertex3fv(lul);
		glVertex3fv(hur);
		glVertex3fv(lur);
		glVertex3fv(hlr);
		glVertex3fv(llr);
		glVertex3fv(hll);
		glVertex3fv(lll);
	}
	void Render_AABB()
	{
		glVertex3f(aabb_lo[0],aabb_lo[1],aabb_lo[2]);
		glVertex3f(aabb_lo[0],aabb_hi[1],aabb_lo[2]);
		glVertex3f(aabb_lo[0],aabb_hi[1],aabb_lo[2]);
		glVertex3f(aabb_lo[0],aabb_hi[1],aabb_hi[2]);
		glVertex3f(aabb_lo[0],aabb_hi[1],aabb_hi[2]);
		glVertex3f(aabb_lo[0],aabb_lo[1],aabb_hi[2]);
		glVertex3f(aabb_lo[0],aabb_lo[1],aabb_hi[2]);
		glVertex3f(aabb_lo[0],aabb_lo[1],aabb_lo[2]);

		glVertex3f(aabb_hi[0],aabb_lo[1],aabb_lo[2]);
		glVertex3f(aabb_hi[0],aabb_hi[1],aabb_lo[2]);
		glVertex3f(aabb_hi[0],aabb_hi[1],aabb_lo[2]);
		glVertex3f(aabb_hi[0],aabb_hi[1],aabb_hi[2]);
		glVertex3f(aabb_hi[0],aabb_hi[1],aabb_hi[2]);
		glVertex3f(aabb_hi[0],aabb_lo[1],aabb_hi[2]);
		glVertex3f(aabb_hi[0],aabb_lo[1],aabb_hi[2]);
		glVertex3f(aabb_hi[0],aabb_lo[1],aabb_lo[2]);
	
		glVertex3f(aabb_hi[0],aabb_lo[1],aabb_lo[2]);
		glVertex3f(aabb_lo[0],aabb_lo[1],aabb_lo[2]);
		glVertex3f(aabb_hi[0],aabb_hi[1],aabb_lo[2]);
		glVertex3f(aabb_lo[0],aabb_hi[1],aabb_lo[2]);
		glVertex3f(aabb_hi[0],aabb_hi[1],aabb_hi[2]);
		glVertex3f(aabb_lo[0],aabb_hi[1],aabb_hi[2]);
		glVertex3f(aabb_hi[0],aabb_lo[1],aabb_hi[2]);
		glVertex3f(aabb_lo[0],aabb_lo[1],aabb_hi[2]);
	}
	__forceinline bool Inside_Bounding_Box(float *v, float range)
	{
		float tpos[3] = {pos[0]-v[0],pos[1]-v[1],pos[2]-v[2]};
		imat.Transform_Vertex(imat.m, tpos);
		if(tpos[0]<size[0]+range&&tpos[0]>-size[0]-range&&
			tpos[1]<size[1]+range&&tpos[1]>-size[1]-range&&
			tpos[2]<size[2]+range&&tpos[2]>-size[2]-range)
		{
			return true;
		}
		return false;
	}
	__forceinline bool Clip_Vertex_To_Bounding_Box(float *v, float range)
	{
		if(v[0]>aabb_hi[0]+range||v[1]>aabb_hi[1]+range||v[2]>aabb_hi[2]+range||
			v[0]<aabb_lo[0]-range||v[1]<aabb_lo[1]-range||v[2]<aabb_lo[2]-range)
		{
			return false;
		}
		float tpos[3] = {pos[0]-v[0],pos[1]-v[1],pos[2]-v[2]};
		imat.Transform_Vertex(imat.m, tpos);
		if(tpos[0]>size[0]+range||tpos[1]>size[1]+range||tpos[2]>size[2]+range||
			tpos[0]<-size[0]-range||tpos[1]<-size[1]-range||tpos[2]<-size[2]-range)
		{
			return false;
		}
		int hit_count = 0;
		float move[3] = {0,0,0};
		for(int i = 0;i<3;i++)
		{
			if(tpos[i]>0)
			{
				if(tpos[i]<size[i]+range)
				{
					move[i] = (size[i]+range)-tpos[i];
					hit_count++;
				}
			}
			else
			{
				if(tpos[i]>(-size[i])-range)
				{
					move[i] = ((-size[i])-range)-tpos[i];
					hit_count++;
				}
			}
		}
		if(hit_count==3)
		{
			float tm[3] = {fabs(move[0]),fabs(move[1]),fabs(move[2])};
			if(tm[0]<tm[1]&&tm[0]<tm[2]){move[1] = move[2] = 0;}
			if(tm[1]<tm[2]&&tm[1]<tm[0]){move[2] = move[0] = 0;}
			if(tm[2]<tm[1]&&tm[2]<tm[0]){move[1] = move[0] = 0;}
			mat.Transform_Vertex(mat.m, move);
			v[0] -= (move[0])*1.001f;
			v[1] -= (move[1])*1.001f;
			v[2] -= (move[2])*1.001f;
			return true;
		}
		return false;
	}
	__forceinline bool Clip_Positive_Plane(float *start, float *end, float *res, int a, int b, int c)
	{
		if(start[a]<size[a]||end[a]>size[a])return false;
		if(size[a]<end[a])
		{
			res[0] = end[0];
			res[1] = end[1];
			res[2] = end[2];
		}
		else
		{
			float p1 = start[a]-size[a];
			float p2 = size[a]-end[a];
			float t = p1+p2;
			p1 = p1/t;
			p2 = p2/t;
			res[0] = (start[0]*p2)+(end[0]*p1);
			res[1] = (start[1]*p2)+(end[1]*p1);
			res[2] = (start[2]*p2)+(end[2]*p1);
		}
		if(res[b]<size[b]&&res[b]>(-size[b])&&res[c]<size[c]&&res[c]>(-size[c]))
		{
			res[a] = size[a];
			return true;
		}
		return false;
	}
	__forceinline bool Clip_Negative_Plane(float *start, float *end, float *res, int a, int b, int c)
	{
		if(start[a]>(-size[a])||end[a]<(-size[a]))return false;
		if(end[a]<-size[a])
		{
			res[0] = end[0];
			res[1] = end[1];
			res[2] = end[2];
		}
		else
		{
			float p1 = -size[a]-start[a];
			float p2 = end[a]-(-size[a]);
			float t = p1+p2;
			p1 = p1/t;
			p2 = p2/t;
			res[0] = (start[0]*p2)+(end[0]*p1);
			res[1] = (start[1]*p2)+(end[1]*p1);
			res[2] = (start[2]*p2)+(end[2]*p1);
		}
		if(res[b]<size[b]&&res[b]>(-size[b])&&res[c]<size[c]&&res[c]>(-size[c]))
		{
			res[a] = -size[a];
			return true;
		}
		return false;
	}
	__forceinline bool Clip_Ray_To_Bounding_Box(float *start, float *end, float *res)
	{
		float tstart[3] = {start[0]-pos[0],start[1]-pos[1],start[2]-pos[2]};
		float tend[3] = {end[0]-pos[0],end[1]-pos[1],end[2]-pos[2]};
		imat.Transform_Vertex(imat.m, tstart);
		imat.Transform_Vertex(imat.m, tend);
		if(tstart[0]<size[0]&&tstart[0]>-size[0]&&
			tstart[1]<size[1]&&tstart[1]>-size[1]&&
			tstart[2]<size[2]&&tstart[2]>-size[2])
		{
			//ray starts inside box, flip order so intersection can be found from outside in
			float temp[3] = {tstart[0],tstart[1],tstart[2]};
			tstart[0] = tend[0];tstart[1] = tend[1];tstart[2] = tend[2];
			tend[0] = temp[0];tend[1] = temp[1];tend[2] = temp[2];
			//returned intersection will be on outside of box with outward facing normal
		}
		float intersection[3];
		bool hit = false;
		if(tstart[0]>0)
		{
			hit = Clip_Positive_Plane(tstart, tend, intersection, 0, 1, 2);
		}
		else
		{
			hit = Clip_Negative_Plane(tstart, tend, intersection, 0, 1, 2);
		}
		if(!hit)
		{
			if(tstart[1]>0)
			{
				hit = Clip_Positive_Plane(tstart, tend, intersection, 1, 2, 0);
			}
			else
			{
				hit = Clip_Negative_Plane(tstart, tend, intersection, 1, 2, 0);
			}
		}
		if(!hit)
		{
			if(tstart[2]>0)
			{
				hit = Clip_Positive_Plane(tstart, tend, intersection, 2, 0, 1);
			}
			else
			{
				hit = Clip_Negative_Plane(tstart, tend, intersection, 2, 0, 1);
			}
		}
		if(hit)
		{
			mat.Transform_Vertex(mat.m, intersection);
			res[0] = intersection[0]+pos[0];
			res[1] = intersection[1]+pos[1];
			res[2] = intersection[2]+pos[2];
			return true;
		}
		//skip move outside range check
		return false;

		if(tend[0]>size[0]||tend[1]>size[1]||tend[2]>size[2]||
			tend[0]<-size[0]||tend[1]<-size[1]||tend[2]<-size[2])
		{
			return false;
		}

		int hit_count = 0;
		float move[3] = {0,0,0};
		for(int i = 0;i<3;i++)
		{
			if(tend[i]>0)
			{
				if(tend[i]<size[i])
				{
					move[i] = ((size[i])-tend[i]);
					hit_count++;
				}
			}
			else
			{
				if(tend[i]>(-size[i]))
				{
					move[i] = ((-size[i]))-tend[i];
					hit_count++;
				}
			}
		}
		if(hit_count==3)
		{
			float tm[3] = {fabs(move[0]),fabs(move[1]),fabs(move[2])};
			if(tm[0]<tm[1]&&tm[0]<tm[2]){move[1] = move[2] = 0;}
			if(tm[1]<tm[2]&&tm[1]<tm[0]){move[2] = move[0] = 0;}
			if(tm[2]<tm[1]&&tm[2]<tm[0]){move[1] = move[0] = 0;}
			intersection[0] = tend[0]+move[0];
			intersection[1] = tend[1]+move[1];
			intersection[2] = tend[2]+move[2];
			mat.Transform_Vertex(mat.m, intersection);
			res[0] = intersection[0]+pos[0];
			res[1] = intersection[1]+pos[1];
			res[2] = intersection[2]+pos[2];
			return true;
		}
		return false;
	}
	float vertices[24];
	float *lul;
	float *lur;
	float *lll;
	float *llr;
	float *hul;
	float *hur;
	float *hll;
	float *hlr;
	float aabb_lo[3];
	float aabb_hi[3];
	float top_normal[3];
	float bottom_normal[3];
	float left_normal[3];
	float right_normal[3];
	float front_normal[3];
	float back_normal[3];
	float pos[3];
	float rot[3];
	float size[3];
	int octree_cell_id;
	int object_type;
	void *object_pointer;
	BB_MATRIX mat;
	BB_MATRIX imat;
};

class MATH_UTILS
{
public:
	MATH_UTILS()
	{
	}
	~MATH_UTILS()
	{
	}
	static __forceinline float Dist_To_Line(float *p, float *tp, float *dir)
	{
		float v[3] = {p[0]-tp[0],p[1]-tp[1],p[2]-tp[2]};
		float side[3];
		float plane_dir[3];
		Cross(v, dir, side);
		Cross(dir, side, plane_dir);
		Normalize(plane_dir);
		return Dist_To_Plane(p, tp, plane_dir);
	}
	static __forceinline bool IsEven(int n)
	{
		float fn = (float)n * 0.5f;
		int in = n/2;
		return fn==in;
	}
	static __forceinline float Dot(float *a, float *b)
	{
		return (a[0]*b[0]+a[1]*b[1]+a[2]*b[2]);
	}
	static __forceinline void Cross(float *v1, float *v2, float *v3)
	{
		v3[0] = (v1[1]*v2[2])-(v1[2]*v2[1]);
		v3[1] = (v1[2]*v2[0])-(v1[0]*v2[2]);
		v3[2] = (v1[0]*v2[1])-(v1[1]*v2[0]);
	}
	static __forceinline float Distance3D(float* v1, float* v2)
	{
		return (float)(fastsqrt(((v1[0]-v2[0])*(v1[0]-v2[0]))+((v1[1]-v2[1])*(v1[1]-v2[1]))+((v1[2]-v2[2])*(v1[2]-v2[2]))));
	}
	static __forceinline float Distance2D(float x1, float y1, float x2, float y2)
	{
		return (float)(fastsqrt(((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2))));
	}
	static __forceinline float VecLength(float* v)
	{
		return (float)fastsqrt((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2]));
	}
	static __forceinline void SetLength(float *v, float s)
	{
		float len = s / VecLength(v);
		v[0] *= len;
		v[1] *= len;
		v[2] *= len;
	}
	static __forceinline void Normalize(float *v)
	{
		float len = 1.0f / VecLength(v);
		v[0] *= len;
		v[1] *= len;
		v[2] *= len;
	}
	static __forceinline float RF(float max)
	{
		return max*(((float)(rand()%1000))/1000.f);
	}
	static __forceinline float NRF(float max)
	{
		return (-max)+RF(max*2);
	}
	static __forceinline float Get_Random_Value(int v)
	{
		srand(v);
		return NRF(1);
	}
	static __forceinline float Dist_To_Plane(float *p, float *tp, float *n)
	{
		float res = (((p[0]-tp[0])*n[0])+((p[1]-tp[1])*n[1])+((p[2]-tp[2])*n[2]));
		if(res<0)return -res;
		return res;
	}
	static __forceinline void Plane_Intersection(float *start, float *finish, float *result, float *point, float *normal)
	{
		float d1 = Dist_To_Plane(start, point, normal);
		float d2 = Dist_To_Plane(finish, point, normal);
		if(d1<0){d1=-d1;}
		if(d2<0){d2=-d2;}
		float td = d1+d2;
		float p1 = d2/td;
		float p2 = d1/td;
		result[0] = (start[0]*p1)+(finish[0]*p2);
		result[1] = (start[1]*p1)+(finish[1]*p2);
		result[2] = (start[2]*p1)+(finish[2]*p2);
	}
	static __forceinline int Side_Of_Plane(float *p, float *n, float *t)
	{
		float xx = t[0] - p[0];
		float yy = t[1] - p[1];
		float zz = t[2] - p[2];
		if(((n[0] * xx) + (n[1] * yy) + (n[2] * zz))>=0.0000){return 0;}
		return 1;
	}
};

class CAMERA_POSITION
{
public:
	CAMERA_POSITION()
	{
	}
	~CAMERA_POSITION()
	{
	}
	float pos[3];
	float rot[3];
	float time;
};

class CAMERA_SEQUENCE
{
public:
	CAMERA_SEQUENCE()
	{
		playback_speed = 0.5f;
	}
	~CAMERA_SEQUENCE()
	{
	}
	void Add_Camera_Position(float *pos, float *rot, float delta_time)
	{
		CAMERA_POSITION cp;
		cp.pos[0] = pos[0];cp.pos[1] = pos[1];cp.pos[2] = pos[2];
		cp.rot[0] = rot[0];cp.rot[1] = rot[1];cp.rot[2] = rot[2];
		cp.time = delta_time;
		camera_positions.push_back(cp);
	}
	void Get_Total_Length()
	{
		int n = camera_positions.size();
		total_length = 0;
		for(int i = 0;i<n;i++)
		{
			total_length += camera_positions[i].time;
			camera_positions[i].time = total_length;
		}
	}
	void Interpolate_Camera_Positions(float *pos, float *rot, float stime, CAMERA_POSITION *a, CAMERA_POSITION *b)
	{
		float t = b->time-a->time;
		float p1 = (b->time-stime)/t;
		float p2 = 1.0f-p1;
		pos[0] = (a->pos[0]*p1)+(b->pos[0]*p2);
		pos[1] = (a->pos[1]*p1)+(b->pos[1]*p2);
		pos[2] = (a->pos[2]*p1)+(b->pos[2]*p2);
		rot[0] = (a->rot[0]*p1)+(b->rot[0]*p2);
		rot[1] = (a->rot[1]*p1)+(b->rot[1]*p2);
		rot[2] = (a->rot[2]*p1)+(b->rot[2]*p2);
	}
	bool Get_Camera_At_Time(int start, float *pos, float *rot, float stime)
	{
		int n = camera_positions.size();
		CAMERA_POSITION *a, *b;
		for(int i = start;i<n-1;i++)
		{
			a = &camera_positions[i];
			b = &camera_positions[i+1];
			if(stime>=a->time&&stime<=b->time)
			{
				last_playback_start = i;
				Interpolate_Camera_Positions(pos, rot, stime, a, b);
				return true;
			}
		}
		return false;
	}
	void Reset_Playback()
	{
		playback_time = 0;
		last_playback_start = 0;
	}
	bool Advance_Playback(float *pos, float *rot, float delta_time)
	{
		playback_time += delta_time*playback_speed;
		if(playback_time>=total_length)return false;
		return Get_Camera_At_Time(last_playback_start, pos, rot, playback_time);
	}
	bool Start_Recording()
	{
		camera_positions.clear();
		return true;
	}
	bool Stop_Recording()
	{
		Get_Total_Length();
		return true;
	}
	bool Save(char *file)
	{
		FILE *f = fopen(file, "wb");
		if(!f)
		{
			return false;
		}
		int n = camera_positions.size();
		fwrite(&n, sizeof(int), 1, f);
		fwrite(&total_length, sizeof(float), 1, f);
		for(int i = 0;i<n;i++)
		{
			fwrite(&camera_positions[i], sizeof(CAMERA_POSITION), 1, f);
		}
		fclose(f);
		return true;
	}
	bool Load(char *file)
	{
		FILE *f = fopen(file, "rb");
		if(!f)
		{
			return false;
		}
		int n = 0;
		CAMERA_POSITION cp;
		fread(&n, sizeof(int), 1, f);
		fread(&total_length, sizeof(float), 1, f);
		for(int i = 0;i<n;i++)
		{
			fread(&cp, sizeof(CAMERA_POSITION), 1, f);
			camera_positions.push_back(cp);
		}
		fclose(f);
		return true;
	}
	vector<CAMERA_POSITION> camera_positions;
	float total_length;
	float start_time;
	float end_time;
	float playback_time;
	float playback_speed;
	int last_playback_start;
};

class CAMERA
{
public:
	float camera_pos[3];
	float camera_rot[3];
	float camera_front[3];
	float camera_side[3];
	float camera_up[3];
	float target_camera_pos[3];
	float target_camera_rot[3];
	float camera_tsnap;
	float camera_rsnap;
	float max_pitch;
	float frustum[6][4];
	
	CAMERA()
	{
		Reset_View();
		camera_tsnap = 0.06f;
		camera_rsnap = 0.08f;
		max_pitch = 90;
	}
	~CAMERA(){}
	void Reset_View()
	{
		camera_pos[0] = camera_pos[1] = camera_pos[2] = 0;
		camera_rot[0] = camera_rot[1] = camera_rot[2] = 0;
		target_camera_pos[0] = target_camera_pos[1] = target_camera_pos[2] = 0;
		target_camera_rot[0] = target_camera_rot[1] = target_camera_rot[2] = 0;
		camera_front[0] = 0;
		camera_front[1] = 0;
		camera_front[2] = 1;
		camera_side[0] = 1;
		camera_side[1] = 0;
		camera_side[2] = 0;
		camera_up[0] = 0;
		camera_up[1] = 1;
		camera_up[2] = 0;
	}
	void Reset_Target_View()
	{
		target_camera_pos[0] = target_camera_pos[1] = target_camera_pos[2] = 0;
		target_camera_rot[0] = target_camera_rot[1] = target_camera_rot[2] = 0;
		while(camera_rot[1]>180)camera_rot[1]-=360;
		while(camera_rot[1]<-180)camera_rot[1]+=360;
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
	void Set_GL_View()
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(camera_rot[0], 1, 0, 0);
		glRotatef(camera_rot[1], 0, 1, 0);
		glTranslatef(-camera_pos[0], -camera_pos[1], -camera_pos[2]);
	}
	void Update_Camera_Vectors()
	{
		GLdouble m[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, m);
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
	void Update_Camera_View()
	{
		Set_GL_View();
		Update_Camera_Vectors();
		Get_Frustum();
	}
	void Move_Forward(float f)
	{
		target_camera_pos[0]-=camera_front[0]*f;
		target_camera_pos[1]-=camera_front[1]*f;
		target_camera_pos[2]-=camera_front[2]*f;
	}
	void Move_Back(float f)
	{
		target_camera_pos[0]+=camera_front[0]*f;
		target_camera_pos[1]+=camera_front[1]*f;
		target_camera_pos[2]+=camera_front[2]*f;
	}
	void Move_Up(float f)
	{
		target_camera_pos[0]+=camera_up[0]*f;
		target_camera_pos[1]+=camera_up[1]*f;
		target_camera_pos[2]+=camera_up[2]*f;
	}
	void Move_Down(float f)
	{
		target_camera_pos[0]-=camera_up[0]*f;
		target_camera_pos[1]-=camera_up[1]*f;
		target_camera_pos[2]-=camera_up[2]*f;
	}
	void Move_Left(float f)
	{
		target_camera_pos[0]+=camera_side[0]*f;
		target_camera_pos[1]+=camera_side[1]*f;
		target_camera_pos[2]+=camera_side[2]*f;
	}
	void Move_Right(float f)
	{
		target_camera_pos[0]-=camera_side[0]*f;
		target_camera_pos[1]-=camera_side[1]*f;
		target_camera_pos[2]-=camera_side[2]*f;
	}
	void Turn_Up(float f)
	{
		target_camera_rot[0] -= f;
		if(target_camera_rot[0]<-max_pitch)target_camera_rot[0] = -max_pitch;
	}
	void Turn_Down(float f)
	{
		target_camera_rot[0] += f;
		if(target_camera_rot[0]>max_pitch)target_camera_rot[0] = max_pitch;
	}
	void Turn_Left(float f)
	{
		target_camera_rot[1] -= f;
	}
	void Turn_Right(float f)
	{
		target_camera_rot[1] += f;
	}
	void Get_Frustum()
	{
		//NOTE: This is the standard example code for getting the current opengl view frustum,
		//copied verbatim, more information can be found at www.opengl.org
		float   proj[16];
		float   modl[16];
		float   clip[16];
		float   t;
		
		/* Get the current PROJECTION matrix from OpenGL */
		glGetFloatv( GL_PROJECTION_MATRIX, proj );
		
		/* Get the current MODELVIEW matrix from OpenGL */
		glGetFloatv( GL_MODELVIEW_MATRIX, modl );
		
		/* Combine the two matrices (multiply projection by
		modelview) */
		clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
		clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
		clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
		clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];
		
		clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
		clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
		clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
		clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];
		
		clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[4] + modl[10] * proj[ 8] + modl[11] * proj[12];
		clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[5] + modl[10] * proj[ 9] + modl[11] * proj[13];
		clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
		clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];
		
		clip[12] = modl[12] * proj[ 0] + modl[13] * proj[4] + modl[14] * proj[ 8] + modl[15] * proj[12];
		clip[13] = modl[12] * proj[ 1] + modl[13] * proj[5] + modl[14] * proj[ 9] + modl[15] * proj[13];
		clip[14] = modl[12] * proj[ 2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
		clip[15] = modl[12] * proj[ 3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];
		
		/* Extract the numbers for the RIGHT plane */
		frustum[0][0] = clip[ 3] - clip[ 0];
		frustum[0][1] = clip[ 7] - clip[ 4];
		frustum[0][2] = clip[11] - clip[ 8];
		frustum[0][3] = clip[15] - clip[12];
		
		/* Normalize the result */
		t = (float)fastsqrt( frustum[0][0] * frustum[0][0] +frustum[0][1] * frustum[0][1] + frustum[0][2] *frustum[0][2] );
		frustum[0][0] /= t;
		frustum[0][1] /= t;
		frustum[0][2] /= t;
		frustum[0][3] /= t;
		
		/* Extract the numbers for the LEFT plane */
		frustum[1][0] = clip[ 3] + clip[ 0];
		frustum[1][1] = clip[ 7] + clip[ 4];
		frustum[1][2] = clip[11] + clip[ 8];
		frustum[1][3] = clip[15] + clip[12];
		
		/* Normalize the result */
		t = (float)fastsqrt( frustum[1][0] * frustum[1][0] +frustum[1][1] * frustum[1][1] + frustum[1][2] *frustum[1][2] );
		frustum[1][0] /= t;
		frustum[1][1] /= t;
		frustum[1][2] /= t;
		frustum[1][3] /= t;
		
		/* Extract the BOTTOM plane */
		frustum[2][0] = clip[ 3] + clip[ 1];
		frustum[2][1] = clip[ 7] + clip[ 5];
		frustum[2][2] = clip[11] + clip[ 9];
		frustum[2][3] = clip[15] + clip[13];
		
		/* Normalize the result */
		t = (float)fastsqrt( frustum[2][0] * frustum[2][0] +frustum[2][1] * frustum[2][1] + frustum[2][2] *frustum[2][2] );
		frustum[2][0] /= t;
		frustum[2][1] /= t;
		frustum[2][2] /= t;
		frustum[2][3] /= t;
		
		/* Extract the TOP plane */
		frustum[3][0] = clip[ 3] - clip[ 1];
		frustum[3][1] = clip[ 7] - clip[ 5];
		frustum[3][2] = clip[11] - clip[ 9];
		frustum[3][3] = clip[15] - clip[13];
		
		/* Normalize the result */
		t = (float)fastsqrt( frustum[3][0] * frustum[3][0] +frustum[3][1] * frustum[3][1] + frustum[3][2] *frustum[3][2] );
		frustum[3][0] /= t;
		frustum[3][1] /= t;
		frustum[3][2] /= t;
		frustum[3][3] /= t;
		
		/* Extract the FAR plane */
		frustum[4][0] = clip[ 3] - clip[ 2];
		frustum[4][1] = clip[ 7] - clip[ 6];
		frustum[4][2] = clip[11] - clip[10];
		frustum[4][3] = clip[15] - clip[14];
		
		/* Normalize the result */
		t = (float)fastsqrt( frustum[4][0] * frustum[4][0] +frustum[4][1] * frustum[4][1] + frustum[4][2] *frustum[4][2] );
		frustum[4][0] /= t;
		frustum[4][1] /= t;
		frustum[4][2] /= t;
		frustum[4][3] /= t;
		
		/* Extract the NEAR plane */
		frustum[5][0] = clip[ 3] + clip[ 2];
		frustum[5][1] = clip[ 7] + clip[ 6];
		frustum[5][2] = clip[11] + clip[10];
		frustum[5][3] = clip[15] + clip[14];
		
		/* Normalize the result */
		t = (float)fastsqrt( frustum[5][0] * frustum[5][0] +frustum[5][1] * frustum[5][1] + frustum[5][2] *frustum[5][2] );
		frustum[5][0] /= t;
		frustum[5][1] /= t;
		frustum[5][2] /= t;
		frustum[5][3] /= t;
	}
	__forceinline float dot4(float *a, float *b)
	{
		return (a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3]);
	}
	int AABB_In_Frustum(float *min, float *max)
	{
		int result = INSIDE_FRUSTUM;
		float pv[4] = {0,0,0,1};
		float nv[4] = {0,0,0,1};
		float n, m;
		for (int i = 0; i < 6; ++i)
		{
			float *plane = frustum[i];
			pv[0] = plane[0] > 0 ? max[0] : min[0];
			pv[1] = plane[1] > 0 ? max[1] : min[1];
			pv[2] = plane[2] > 0 ? max[2] : min[2];
			nv[0] = plane[0] < 0 ? max[0] : min[0];
			nv[1] = plane[1] < 0 ? max[1] : min[1];
			nv[2] = plane[2] < 0 ? max[2] : min[2];
			n = dot4(pv, plane);
			if (n < 0) return OUTSIDE_FRUSTUM;
			m = dot4(nv, plane);
			if (m < 0) result = INTERSECTS_FRUSTUM;
		}
		return result;
	}
};

class CLOCK
{
public:
	float delta_time;
	float last_delta_time;
	int fps;
	float fps_timer;
	float elapsed_time;
	int frame_count;
	LARGE_INTEGER tickspersecond;
	LARGE_INTEGER currentticks;
	LARGE_INTEGER framedelay;
	CLOCK()
	{
		delta_time = 1;
		last_delta_time = 1;
		tickspersecond;
		currentticks;
		framedelay;
		fps = 0;
		fps_timer = 0;
		frame_count = 0;
		elapsed_time = 0;
	}
	~CLOCK(){}
	bool Init_Clock()
	{
		QueryPerformanceCounter(&framedelay);
		QueryPerformanceFrequency(&tickspersecond);
		Update_Clock();
		delta_time = 0;
		fps = 0;
		fps_timer = 0;
		frame_count = 0;
		elapsed_time = 0;
		return true;
	}
	bool Reset_Clock()
	{
		QueryPerformanceCounter(&framedelay);
		QueryPerformanceFrequency(&tickspersecond);
		Update_Clock();
		delta_time = 0;
		fps = 0;
		fps_timer = 0;
		frame_count = 0;
		elapsed_time = 0;
		return true;
	}
	void Update_Clock()
	{
		QueryPerformanceCounter(&currentticks);
		delta_time = (float)(currentticks.QuadPart-framedelay.QuadPart)/((float)tickspersecond.QuadPart);
		if(delta_time <= 0){delta_time = last_delta_time;}
		last_delta_time = delta_time;
		framedelay = currentticks;
		frame_count++;
		fps_timer+=delta_time;
		while(fps_timer>1.0f)
		{
			fps_timer -= 1;
			fps = frame_count;
			frame_count = 0;
		}
		elapsed_time += delta_time;
	}
	float Delta_Time()
	{
		return delta_time;
	}
};


class OCTREE_CELL;

class SCENE_GRAPH
{
public:
	SCENE_GRAPH(){}
	~SCENE_GRAPH(){}
	OCTREE_CELL *Get_Free_Octree_Cell();
	bool Register_Free_Octree_Cells(OCTREE_CELL* *cells);
	bool Free_Scenegraph();
	bool Set_World_Extents(float *lo, float *hi);
	bool Get_World_Extents(float *lo, float *hi);
	bool Clear_All_Bounding_Boxes();
	bool Register_New_Bounding_Box(BOUNDING_BOX *bb, OCTREE_CELL *cell);
	bool Add_Bounding_Box(BOUNDING_BOX *bb);
	bool Remove_Bounding_Box(BOUNDING_BOX *bb);
	bool Update_Bounding_Box(BOUNDING_BOX *bb);
	bool Update_Visible_Bounding_Boxes();
	bool Register_Visible_Octree_Cell(OCTREE_CELL *c);
	bool Render_Scenegraph();
	bool Find_Bounding_Boxes_In_Range(float *lo, float *hi);
	bool Find_Intersected_Bounding_Boxes(float *start, float *end);
	bool Find_All_Bounding_Boxes();
	vector<OCTREE_CELL*> octree_cells;
	vector<OCTREE_CELL*> bounding_box_cells;
	vector<OCTREE_CELL*> visible_cells;
	vector<BOUNDING_BOX*> visible_bounding_boxes;
	vector<BOUNDING_BOX*> in_range_bounding_boxes;
	OCTREE_CELL *octree_root;
	float min[3];
	float max[3];
};

class GL_BASIC_APP
{
public:
	bool keys_pressed[256];
	int move_forward_key;
	int move_back_key;
	int move_left_key;
	int move_right_key;
	int move_up_key;
	int move_down_key;
	int turn_left_key;
	int turn_right_key;
	int turn_up_key;
	int turn_down_key;
	int reset_view_key;
	int stats_key;
	int quit_key;
	int toggle_record_key;
	int toggle_play_key;
	bool recording_sequence;
	bool playing_sequence;
	float move_speed;
	float turn_speed;
	float speed;
	float acceleration;
	float max_acceleration;
	float acceleration_increase;
	CAMERA camera;
	CLOCK clock;
	MATH_UTILS math;
	SCENE_GRAPH scene;
	CAMERA_SEQUENCE sequence;
	
	GL_BASIC_APP()
	{
		move_forward_key = 101;//up arrow
		move_back_key = 103;   //down arrow
		move_left_key = 113;   //'Q' key
		move_right_key = 119;  //'W' key
		move_up_key = 97;      //'A' key
		move_down_key = 122;   //'Z' key
		turn_left_key = 100;   //left arrow
		turn_right_key = 102;  //right arrow
		turn_up_key = 107;     //end key
		turn_down_key = 127;   //delete key
		reset_view_key = 114;  //'R' key
		stats_key = 32;        //spacebar
		quit_key = 27;         //esc key
		
		move_speed = 65;
		turn_speed = 80.5f;
		acceleration = 1.0f;
		max_acceleration = 25;
		acceleration_increase = 4.0f;
		speed = 1;
		recording_sequence = false;
		playing_sequence = false;
	}
	~GL_BASIC_APP(){}
	void Reset_Controls()
	{
		clock.Reset_Clock();
		camera.Reset_View();
		Clear_Keys();
	}
	void Update_Controls()
	{
		if(!keys_pressed[move_forward_key]&&!keys_pressed[move_back_key]&&!keys_pressed[move_up_key]&&!keys_pressed[move_down_key]&&!keys_pressed[move_left_key]&&!keys_pressed[move_right_key])
		{
			acceleration = 1.0f;
		}
		else
		{
			acceleration *= 1.0f+((acceleration_increase-1.0f)*clock.delta_time);
			if(acceleration>max_acceleration)acceleration=max_acceleration;
		}
		float t = (move_speed*acceleration*clock.delta_time)*speed;
		float r = (turn_speed*clock.delta_time);
		if(keys_pressed[move_forward_key])camera.Move_Forward(t);
		if(keys_pressed[move_back_key])camera.Move_Back(t);
		if(keys_pressed[move_left_key])camera.Move_Left(t);
		if(keys_pressed[move_right_key])camera.Move_Right(t);
		if(keys_pressed[move_up_key])camera.Move_Up(t);
		if(keys_pressed[move_down_key])camera.Move_Down(t);
		if(keys_pressed[turn_left_key])camera.Turn_Left(r);
		if(keys_pressed[turn_right_key])camera.Turn_Right(r);
		if(keys_pressed[turn_up_key])camera.Turn_Up(r);
		if(keys_pressed[turn_down_key])camera.Turn_Down(r);
		if(keys_pressed[reset_view_key])camera.Reset_Target_View();
		if(keys_pressed[quit_key])
		{
			exit(0);
		}
	}
	void Update_OpenGL_View()
	{
		if(playing_sequence)
		{
			if(!sequence.Advance_Playback(camera.camera_pos, camera.camera_rot, clock.delta_time))
			{
				Toggle_Playback();
			}
		}
		camera.Update_Camera_View();
		if(recording_sequence)
		{
			sequence.Add_Camera_Position(camera.camera_pos, camera.camera_rot, clock.delta_time);
		}
	}
	void Clear_Keys()
	{
		memset(keys_pressed, 0, sizeof(bool)*256);
	}
	void Print_Stats()
	{
		printf("\nCurrent stats from last frame:\n");
		printf("Average FPS: %i\n", clock.fps);
		printf("Camera Position: %f %f %f\n", camera.camera_pos[0], camera.camera_pos[1], camera.camera_pos[2]);
		printf("Camera Rotation: %f %f %f\n", camera.camera_rot[0], camera.camera_rot[1], camera.camera_rot[2]);
		printf("Num Visible Bounding Boxes: %i\n", scene.visible_bounding_boxes.size());
		printf("Num Bounding Boxes In Range (from last query): %i\n", scene.in_range_bounding_boxes.size());
	}
	bool Toggle_Recording()
	{
		if(playing_sequence)
		{
			return false;
		}
		recording_sequence = !recording_sequence;
		if(recording_sequence)
		{
			sequence.Start_Recording();
			printf("Started recording..\n");
			sequence.Add_Camera_Position(camera.camera_pos, camera.camera_rot, 0);
		}
		else
		{
			sequence.Stop_Recording();
			printf("Stopped recording..\n");
			printf("Total sequence length is %f\n", sequence.total_length);
		}
		return true;
	}
	bool Toggle_Playback()
	{
		if(recording_sequence)
		{
			return false;
		}
		playing_sequence = !playing_sequence;
		if(playing_sequence)
		{
			printf("Started playing..\n");
			sequence.Reset_Playback();
			sequence.Advance_Playback(camera.camera_pos, camera.camera_rot, 0);
			sequence.Advance_Playback(camera.target_camera_pos, camera.target_camera_rot, 0);
		}
		else
		{
			printf("Stopped playing..\n");
		}
		return true;
	}
	void Press_Key(int key, bool pressed)
	{
		keys_pressed[key] = pressed;
		if(pressed&&key==stats_key)
		{
			Print_Stats();
		}
	}
};

extern GL_BASIC_APP glApp;

#endif