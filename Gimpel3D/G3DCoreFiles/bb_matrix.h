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
#ifndef BB_MATRIX_H
#define BB_MATRIX_H

#include <math.h>

//used to convert degrees to radians, == PI/180
#define RAD 0.017453f

//bare-bones 4x4 matrix class to handle object transforms
class oBB_MATRIX
{
public:
	oBB_MATRIX()
	{
	}
	~oBB_MATRIX()
	{
	}
	void Identity(float mat[4][4])
	{
		mat[0][0]=1; mat[0][1]=0; mat[0][2]=0; mat[0][3]=0;
		mat[1][0]=0; mat[1][1]=1; mat[1][2]=0; mat[1][3]=0;
		mat[2][0]=0; mat[2][1]=0; mat[2][2]=1; mat[2][3]=0;
		mat[3][0]=0; mat[3][1]=0; mat[3][2]=0; mat[3][3]=1;
	}
	void Multiply(float mat1[4][4], float mat2[4][4], float mat3[4][4])
	{
		int i,j;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
				mat3[i][j]=mat1[i][0]*mat2[0][j]+
				mat1[i][1]*mat2[1][j]+
				mat1[i][2]*mat2[2][j]+
				mat1[i][3]*mat2[3][j];
	}
	void Translate(float matrix[4][4], float* t)
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
	void Rotate(float m[4][4], float *a)
	{
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
		Identity(mat1);
		Identity(mat2);
		Multiply(m,ymat,mat1);
		Multiply(mat1,xmat,mat2);
		Multiply(mat2,zmat,m);
	}
	void Copy(float s[4][4],float d[4][4])
	{
		int i,j;
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
				d[i][j]=s[i][j];
	}
	void Set(float m[4][4], float* t, float* r)
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
		Multiply(m,xmat,mat1);
		Multiply(mat1,ymat,mat2);
		Multiply(mat2,zmat,m);
		if(t)
		{
			m[3][0]=t[0]; m[3][1]=t[1]; m[3][2]=t[2]; m[3][3]=1;
		}
	}

	void SetYXZ(float m[4][4], float* t, float* r)
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
		if(t)
		{
			m[3][0]=t[0]; m[3][1]=t[1]; m[3][2]=t[2]; m[3][3]=1;
		}
	}
	
	void Transform_Vertex(float m[4][4], float* s)
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
	float m[4][4];
};


#endif
