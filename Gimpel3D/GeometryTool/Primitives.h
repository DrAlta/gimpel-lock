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
#ifndef PRIMITIVES_H
#define PRIMITIVES_H


#define PI 3.14159
#define EPSILON 0.000001

#include "TriangleMesh.h"

extern GLUquadric *quadric;

class PRIMITIVE;

class RAY_HIT
{
public:
	RAY_HIT()
	{
	}
	~RAY_HIT()
	{
	}
	float hit_pos[3];
	float hit_normal[3];
	float hit_uv[2];
	float ray_start[3];
	float ray_dir[3];
	float final_color[3];
	float distance;
	float hit_size;
	bool hit;
	bool hit_light;
	int hit_type;
	PRIMITIVE *hit_object;
};



//object types that use bounding boxes
enum
{
	pt_BOX,
	pt_SPHERE,
	pt_CYLINDER,
	pt_PLANE,
	pt_TRIMESH
};

class PRIMITIVE
{
public:
	PRIMITIVE()
	{
		opengl_color[0] = glApp.math.RF(1);
		opengl_color[1] = glApp.math.RF(1);
		opengl_color[2] = glApp.math.RF(1);
	}
	~PRIMITIVE()
	{
	}
	virtual void Render(){}
	virtual void Scale(float x, float y, float z){}
	BOUNDING_BOX bounding_box;
	int num_queries;
	int num_hits;
	float opengl_color[3];
	int primitive_type;
};


class BOX : public PRIMITIVE
{
public:
	BOX()
	{
		primitive_type = pt_BOX;
	}
	~BOX()
	{
	}
	void Fill_Vertices()
	{
		BOUNDING_BOX *bb = &bounding_box;
		float *lul = bb->lul;float *lur = bb->lur;float *lll = bb->lll;float *llr = bb->llr;float *hul = bb->hul;float *hur = bb->hur;float *hll = bb->hll;float *hlr = bb->hlr;float *vtop = &vertices[0];float *vbottom = &vertices[12];float *vleft = &vertices[24];float *vright = &vertices[36];float *vfront = &vertices[48];float *vback = &vertices[60];float *ntop = &normals[0];float *nbottom = &normals[12];float *nleft = &normals[24];float *nright = &normals[36];float *nfront = &normals[48];float *nback = &normals[60];float *uvtop = &uvs[0];float *uvbottom = &uvs[8];float *uvleft = &uvs[16];float *uvright = &uvs[24];float *uvfront = &uvs[32];float *uvback = &uvs[40];
		memcpy(&vtop[0], hul, sizeof(float)*3);memcpy(&vtop[3], hll, sizeof(float)*3);memcpy(&vtop[6], hlr, sizeof(float)*3);memcpy(&vtop[9], hur, sizeof(float)*3);
		memcpy(&vbottom[0], lul, sizeof(float)*3);memcpy(&vbottom[3], lur, sizeof(float)*3);memcpy(&vbottom[6], llr, sizeof(float)*3);memcpy(&vbottom[9], lll, sizeof(float)*3);
		memcpy(&vleft[0], hul, sizeof(float)*3);memcpy(&vleft[3], lul, sizeof(float)*3);memcpy(&vleft[6], lll, sizeof(float)*3);memcpy(&vleft[9], hll, sizeof(float)*3);
		memcpy(&vright[0], hur, sizeof(float)*3);memcpy(&vright[3], hlr, sizeof(float)*3);memcpy(&vright[6], llr, sizeof(float)*3);memcpy(&vright[9], lur, sizeof(float)*3);
		memcpy(&vfront[0], hll, sizeof(float)*3);memcpy(&vfront[3], lll, sizeof(float)*3);memcpy(&vfront[6], llr, sizeof(float)*3);memcpy(&vfront[9], hlr, sizeof(float)*3);
		memcpy(&vback[0], hul, sizeof(float)*3);memcpy(&vback[3], hur, sizeof(float)*3);memcpy(&vback[6], lur, sizeof(float)*3);memcpy(&vback[9], lul, sizeof(float)*3);
		ntop[0] = ntop[3] = ntop[6] = ntop[9] = bb->top_normal[0];ntop[1] = ntop[4] = ntop[7] = ntop[10] = bb->top_normal[1];ntop[2] = ntop[5] = ntop[8] = ntop[11] = bb->top_normal[2];
		nbottom[0] = nbottom[3] = nbottom[6] = nbottom[9] = bb->bottom_normal[0];nbottom[1] = nbottom[4] = nbottom[7] = nbottom[10] = bb->bottom_normal[1];nbottom[2] = nbottom[5] = nbottom[8] = nbottom[11] = bb->bottom_normal[2];
		nleft[0] = nleft[3] = nleft[6] = nleft[9] = bb->left_normal[0];nleft[1] = nleft[4] = nleft[7] = nleft[10] = bb->left_normal[1];nleft[2] = nleft[5] = nleft[8] = nleft[11] = bb->left_normal[2];
		nright[0] = nright[3] = nright[6] = nright[9] = bb->right_normal[0];nright[1] = nright[4] = nright[7] = nright[10] = bb->right_normal[1];nright[2] = nright[5] = nright[8] = nright[11] = bb->right_normal[2];
		nfront[0] = nfront[3] = nfront[6] = nfront[9] = bb->front_normal[0];nfront[1] = nfront[4] = nfront[7] = nfront[10] = bb->front_normal[1];nfront[2] = nfront[5] = nfront[8] = nfront[11] = bb->front_normal[2];
		nback[0] = nback[3] = nback[6] = nback[9] = bb->back_normal[0];nback[1] = nback[4] = nback[7] = nback[10] = bb->back_normal[1];nback[2] = nback[5] = nback[8] = nback[11] = bb->back_normal[2];
		float uv = 0.005f;
		uvtop[0] = 0;uvtop[1] = 0;uvtop[2] = size[2]*uv;uvtop[3] = 0;uvtop[4] = size[2]*uv;uvtop[5] = size[0]*uv;uvtop[6] = 0;uvtop[7] = size[0]*uv;
		uvbottom[0] = 0;uvbottom[1] = 0;uvbottom[2] = size[0]*uv;uvbottom[3] = 0;uvbottom[4] = size[0]*uv;uvbottom[5] = size[2]*uv;uvbottom[6] = 0;uvbottom[7] = size[2]*uv;
		uvleft[0] = 0;uvleft[1] = 0;uvleft[2] = size[1]*uv;uvleft[3] = 0;uvleft[4] = size[1]*uv;uvleft[5] = size[2]*uv;uvleft[6] = 0;uvleft[7] = size[2]*uv;
		uvright[0] = 0;uvright[1] = 0;uvright[2] = size[2]*uv;uvright[3] = 0;uvright[4] = size[2]*uv;uvright[5] = size[1]*uv;uvright[6] = 0;uvright[7] = size[1]*uv;
		uvfront[0] = 0;uvfront[1] = 0;uvfront[2] = size[1]*uv;uvfront[3] = 0;uvfront[4] = size[1]*uv;uvfront[5] = size[0]*uv;uvfront[6] = 0;uvfront[7] = size[0]*uv;
		uvback[0] = 0;uvback[1] = 0;uvback[2] = size[0]*uv;uvback[3] = 0;uvback[4] = size[0]*uv;uvback[5] = size[1]*uv;uvback[6] = 0;uvback[7] = size[1]*uv;
	}
	void Render()
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, opengl_color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, opengl_color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, opengl_color);
		glBegin(GL_QUADS);
		for(int i = 0;i<24;i++)
		{
			glNormal3fv(&normals[i*3]);
			glVertex3fv(&vertices[i*3]);
		}
		glEnd();
	}
	__forceinline bool Clip_Ray(float *start, float *end, float *res)
	{
		if(bounding_box.Clip_Ray_To_Bounding_Box(start, end, res))
		{
			return true;
		}
		return false;
	}
	bool Point_Inside(float *p)
	{
		return bounding_box.Inside_Bounding_Box(p, 0);
	}
	void Scale(float x, float y, float z)
	{
		if(size[0]+x>0)size[0] += x;
		if(size[1]+y>0)size[1] += y;
		if(size[2]+z>0)size[2] += z;
		//when reproject layer is called, position will be updated
		//and transform and vertices will be updated too
		//if that gets optimized out then this will have to be done right here
	}
	float vertices[72];
	float normals[72];
	float uvs[48];
	float pos[3];
	float size[3];
	float rot[3];
	float uv_tiling;
};



class SPHERE : public PRIMITIVE
{
public:
	SPHERE()
	{
		primitive_type = pt_SPHERE;
	}
	~SPHERE()
	{
	}
	__forceinline bool Clip_Ray(float *start, float *end, float *res)
	{
		float ray_length = glApp.math.Distance3D(start, end);
		float dir[3] = {(end[0]-start[0])/ray_length,(end[1]-start[1])/ray_length,(end[2]-start[2])/ray_length};
		float v[3];
		v[0] = start[0] - pos[0];
		v[1] = start[1] - pos[1];
		v[2] = start[2] - pos[2];
		float b = -glApp.math.Dot(v, dir);
		float det = (b * b) - glApp.math.Dot(v, v) + (size*size);
		bool hit = false;
		if (det > 0)
		{
			det = fastsqrt(det);
			float i1 = b - det;
			float i2 = b + det;
			if (i2 > 0)
			{
				if (i1 < 0) 
				{
					if (i2 < ray_length) 
					{
						hit = true;
						res[0] = start[0]+(dir[0]*i2);
						res[1] = start[1]+(dir[1]*i2);
						res[2] = start[2]+(dir[2]*i2);
						float rrd = 1.0f/size;
					}
				}
				else
				{
					if (i1 < ray_length)
					{
						hit = true;
						res[0] = start[0]+(dir[0]*i1);
						res[1] = start[1]+(dir[1]*i1);
						res[2] = start[2]+(dir[2]*i1);
						float rrd = 1.0f/size;
					}
				}
			}
		}
		return hit;
	}
	void Render()
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, opengl_color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, opengl_color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, opengl_color);
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2]);
		glColor3fv(opengl_color);
		gluSphere(quadric, size, 16, 16);
		glPopMatrix();
	}
	bool Point_Inside(float *p)
	{
		float d = glApp.math.Distance3D(p, pos);
		return d<size;
	}
	void Scale(float x, float y, float z)
	{
		float t = x+y+z;
		if(size+t>0)size += t;
	}
	float pos[3];
	float rot[3];
	float size;
};

class CYLINDER : public PRIMITIVE
{
public:
	CYLINDER()
	{
		primitive_type = pt_CYLINDER;
	}
	~CYLINDER()
	{
	}
	bool TestIntersionCylinder(float *start, float *dir, float *direction, float radius, float *newpos)
	{
		float RC[3];
		double d;
		double t,s;
		float n[3];
		float O[3];
		double ln;
		double in,out;
		double lamda;
		
		RC[0] = start[0]-pos[0];
		RC[1] = start[1]-pos[1];
		RC[2] = start[2]-pos[2];
		
		
		glApp.math.Cross(dir, direction, n);
		
		ln = glApp.math.VecLength(n);
		
		if ( (ln<EPSILON)&&(ln>-EPSILON) ) return false;
		
		glApp.math.Normalize(n);
		
		d = fabs(glApp.math.Dot(RC, n));
		
		if (d<=radius)
		{
			glApp.math.Cross(RC, direction, O);
			
			t = - glApp.math.Dot(O, n)/ln;
			
			glApp.math.Cross(n, direction, O);
			
			glApp.math.Normalize(O);
			
			s = fabs(fastsqrt(radius*radius - d*d)/ glApp.math.Dot(dir, O));
			
			in = t-s;
			out = t+s;
			
			if (in<-EPSILON){
				if (out<-EPSILON) return false;
				else lamda=out;
			}
			else
				if (out<-EPSILON) {
					lamda=in;
				}
				else
					if (in<out) lamda=in;
					else lamda=out;
					
					newpos[0]=start[0]+(dir[0]*lamda);
					newpos[1]=start[1]+(dir[1]*lamda);
					newpos[2]=start[2]+(dir[2]*lamda);

					float HB[3];

					HB[0] = newpos[0]-pos[0];
					HB[1] = newpos[1]-pos[1];
					HB[2] = newpos[2]-pos[2];

					float hbd = glApp.math.Dot(HB, direction);
					
					return true;
		}
		return false;
	}
	__forceinline bool Clip_Top(float *start, float *end, float *res)
	{
		float *yv = bounding_box.mat.YVector();
		float top[3] = {pos[0]+(yv[0]*height), pos[1]+(yv[1]*height), pos[2]+(yv[2]*height)};
		int s1 = glApp.math.Side_Of_Plane(start, yv, top);
		int s2 = glApp.math.Side_Of_Plane(end, yv, top);
		if(s1==s2)return false;
		glApp.math.Plane_Intersection(start, end, res, top, yv);
		float d = glApp.math.Distance3D(res, top);
		if(d<base)
		{
			return true;
		}
		return false;
	}
	__forceinline bool Clip_Bottom(float *start, float *end, float *res)
	{
		float *yv = bounding_box.mat.YVector();
		float bottom[3] = {pos[0]-(yv[0]*height), pos[1]-(yv[1]*height), pos[2]-(yv[2]*height)};
		int s1 = glApp.math.Side_Of_Plane(start, yv, bottom);
		int s2 = glApp.math.Side_Of_Plane(end, yv, bottom);
		if(s1==s2)return false;
		glApp.math.Plane_Intersection(start, end, res, bottom, yv);
		float d = glApp.math.Distance3D(res, bottom);
		if(d<base)
		{
			return true;
		}
		return false;
	}
	__forceinline bool Clip_Body(float *start, float *dir, float *res)
	{
		float tstart[3] = {start[0],start[1],start[2]};
		float d = glApp.math.Distance3D(start, pos);
		if(base>height){d -= base;}
		else{d -= height;}
		if(d>0)
		{
			tstart[0] += dir[0]*d;
			tstart[1] += dir[1]*d;
			tstart[2] += dir[2]*d;
		}
		if(!TestIntersionCylinder(start, dir, bounding_box.mat.YVector(), base, res))
		{
			return false;
		}
		return true;
	}
	__forceinline bool Clip_Ray(float *start, float *end, float *res)//, float *normal, float *uv)
	{
		float ray_length = glApp.math.Distance3D(start, end);
		float dir[3] = {(end[0]-start[0])/ray_length,(end[1]-start[1])/ray_length,(end[2]-start[2])/ray_length};
		float body_res[3];
		float body_distance = ray_length;
		float top_res[3];
		float top_distance = ray_length;
		float bottom_res[3];
		float bottom_distance = ray_length;
		if(!Clip_Body(start, dir, body_res))
		{
			return false;
		}
		bool body = glApp.math.Dist_To_Plane(body_res, pos, bounding_box.mat.YVector())<=height;
		bool top = false;
		bool bottom = false;
		top = Clip_Top(start, end, top_res);
		bottom = Clip_Bottom(start, end, bottom_res);
		if(body)
		{
			body_distance = glApp.math.Distance3D(start, body_res);
			body = body_distance<ray_length;
		}
		if(top)
		{
			top_distance = glApp.math.Distance3D(start, top_res);
			top = top_distance<ray_length;
			if(body&&body_distance>top_distance)body = false;
		}
		if(bottom)
		{
			bottom_distance = glApp.math.Distance3D(start, bottom_res);
			bottom = bottom_distance<ray_length;
			if(body&&body_distance>bottom_distance)body = false;
		}
		if(!body&&top&&bottom)
		{
			if(top_distance<bottom_distance){bottom = false;}
			else{top = false;}
		}
		if(body)
		{
			memcpy(res, body_res, sizeof(float)*3);
			return true;
		}
		if(top)
		{
			memcpy(res, top_res, sizeof(float)*3);
			return true;
		}
		if(bottom)
		{
			memcpy(res, bottom_res, sizeof(float)*3);
			return true;
		}
		return false;
	}
	void Render()
	{
		glDisable(GL_CULL_FACE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, opengl_color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, opengl_color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, opengl_color);
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2]);
		glRotatef(rot[2], 0, 0, 1);
		glRotatef(rot[0], 1, 0, 0);
		glRotatef(rot[1], 0, 1, 0);
		glRotatef(90, 1, 0, 0);
		glColor3fv(opengl_color);
		glTranslatef(0, 0, -height);
		gluCylinder(quadric, base, top, height*2, 16, 1);
		gluDisk(quadric, 0, base, 16, 1);
		glTranslatef(0, 0, height*2);
		gluDisk(quadric, 0, base, 16, 1);
		glPopMatrix();
		glEnable(GL_CULL_FACE);
	}
	bool Point_Inside(float *p)
	{
		float d = glApp.math.Dist_To_Plane(p, pos, bounding_box.mat.YVector());
		if(d>height)return false;
		d = glApp.math.Dist_To_Line(p, pos, bounding_box.mat.YVector());
		if(d>base)return false;
		return true;
	}
	void Scale(float x, float y, float z)
	{
		x = x/2;
		y = y/2;
		base += x;
		top += x;
		height += y;
	}
	float pos[3];
	float rot[3];
	float base;
	float top;
	float height;
	float uv_tiling;
};

class PLANE : public PRIMITIVE
{
public:
	PLANE()
	{
		primitive_type = pt_PLANE;
	}
	~PLANE()
	{
	}
	__forceinline bool Clip_Ray(float *start, float *end, float *res)
	{
		int s1 = glApp.math.Side_Of_Plane(start, plane_dir, plane_pos);
		int s2 = glApp.math.Side_Of_Plane(end, plane_dir, plane_pos);
		if(s1==s2)
		{
			return false;
		}
		glApp.math.Plane_Intersection(start, end, res, plane_pos, plane_dir);
		return true;
	}
	void Render()
	{
	}
	void Get_UV_Axes()
	{
		u_axis[0] = plane_dir[1];
		u_axis[1] = plane_dir[2];
		u_axis[2] = -plane_dir[0];
		glApp.math.Cross(plane_dir, u_axis, v_axis);
	}
	bool Point_Inside(float *p)
	{
		int s = glApp.math.Side_Of_Plane(p, plane_dir, plane_pos);
		return s==0;
	}
	void Scale(float x, float y, float z)
	{
	}
	float plane_pos[3];
	float plane_dir[3];
	float u_axis[3];
	float v_axis[3];
	float uv_tiling;
};

void Update_GL_Light_Position(float scale);



class TRIMESH : public PRIMITIVE
{
public:
	TRIMESH()
	{
		primitive_type = pt_TRIMESH;
		triangle_mesh = 0;
		scale = 1;
		strcpy(file, "NOFILE");
	}
	~TRIMESH()
	{
		if(triangle_mesh)
		{
			delete triangle_mesh;
		}
	}
	__forceinline bool Clip_Ray(RAY_HIT *rh)
	{
		if(triangle_mesh)
		{
			if(Search_Cast_Ray_Into_Triangle_Mesh(triangle_mesh, rh))
			{
				return rh->hit;
			}
		}
		return false;
	}
	__forceinline void Update_Matrix()
	{
		float p[3] = {0,0,0};
		float r[3] = {-rot[0], -rot[1], -rot[2]};
		rot_matrix.SetZXY(rot_matrix.m, p, r);
	}
	__forceinline bool Clip_Ray(float *start, float *odir, float *end, float *res)
	{
		RAY_HIT rh;
		rh.ray_start[0] = start[0];
		rh.ray_start[1] = start[1];
		rh.ray_start[2] = start[2];
		rh.hit_pos[0] = end[0];
		rh.hit_pos[1] = end[1];
		rh.hit_pos[2] = end[2];
		rh.ray_dir[0] = (end[0]-start[0]);
		rh.ray_dir[1] = (end[1]-start[1]);
		rh.ray_dir[2] = (end[2]-start[2]);
		rh.hit = false;
		rh.distance = 100000;
		glApp.math.Normalize(rh.ray_dir);

		rh.ray_start[0] -= pos[0];
		rh.ray_start[1] -= pos[1];
		rh.ray_start[2] -= pos[2];
		rh.hit_pos[0] -= pos[0];
		rh.hit_pos[1] -= pos[1];
		rh.hit_pos[2] -= pos[2];

		rot_matrix.Transform_Vertex(rot_matrix.m, rh.ray_start);
		rot_matrix.Transform_Vertex(rot_matrix.m, rh.hit_pos);
		rot_matrix.Transform_Vertex(rot_matrix.m, rh.ray_dir);
		
		float is = 1.0f/scale;
		
		rh.ray_start[0] *= is;
		rh.ray_start[1] *= is;
		rh.ray_start[2] *= is;
		rh.hit_pos[0] *= is;
		rh.hit_pos[1] *= is;
		rh.hit_pos[2] *= is;

		if(Clip_Ray(&rh))
		{
			float dir[3] = {odir[0],odir[1],odir[2]};
			glApp.math.SetLength(dir, rh.distance*scale);
			res[0] = start[0]+dir[0];
			res[1] = start[1]+dir[1];
			res[2] = start[2]+dir[2];
			return true;
		}
		return false;
	}
	__forceinline bool Project_Vertex_To_Geometry(float *origin, float *dir, float *new_pos)
	{
		float far_pos[3];
		float center[3];
		float d = 100000;//FIXTHIS could be slimmed down A LOT
		far_pos[0] = origin[0]+(dir[0]*d);
		far_pos[1] = origin[1]+(dir[1]*d);
		far_pos[2] = origin[2]+(dir[2]*d);
		center[0] = origin[0];
		center[1] = origin[1];
		center[2] = origin[2];
		return Clip_Ray(center, dir, far_pos, new_pos);
	}
	void Render()
	{
		if(triangle_mesh)
		{
			glPushMatrix();
			glTranslatef(pos[0], pos[1], pos[2]);
			glRotatef(rot[2], 0, 0, 1);
			glRotatef(rot[0], 1, 0, 0);
			glRotatef(rot[1], 0, 1, 0);
			glScalef(scale, scale, scale);
			float diffuse[4] = {0.5f,0.5f,0.5f,0};
			float specular[4] = {1,1,1,0};
			float ambient[4] = {0.2f,0.2f,0.2f,0};
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
			Render_Triangle_Mesh(triangle_mesh);
			glPopMatrix();
		}
	}
	bool Point_Inside(float *p)
	{
		return false;
	}
	void Scale(float x, float y, float z)
	{
		float t = x+y+z;
		if(scale+t>0)scale += t;
	}
	bool Get_Feature_Point_Links(vector<int> *links)
	{
		int n = model_feature_links.size();
		for(int i = 0;i<n;i++)
		{
			int t = model_feature_links[i];
			links->push_back(t);
		}
		return true;
	}
	bool Set_Feature_Point_Links(vector<int> *links)
	{
		int n = links->size();
		model_feature_links.clear();
		for(int i = 0;i<n;i++)
		{
			int t = (*links)[i];
			model_feature_links.push_back(t);
		}
		return true;
	}
	bool Save_Link_Data(FILE *f)
	{
		int n = model_feature_links.size();
		int *buf = new int[n+1];
		buf[0] = n;
		for(int i = 0;i<n;i++)
		{
			int t = model_feature_links[i];
			buf[i+1] = t;
		}
		fwrite(buf, sizeof(int), n+1, f);
		delete[] buf;
		return true;
	}
	bool Load_Link_Data(FILE *f)
	{
		model_feature_links.clear();
		int n = 0;
		fread(&n, sizeof(int), 1, f);
		int *buf = new int[n];
		fread(buf, sizeof(int), n, f);
		for(int i = 0;i<n;i++)
		{
			int t = buf[i];
			model_feature_links.push_back(t);
		}
		delete[] buf;
		return true;
	}
	TRIANGLE_MESH *triangle_mesh;
	float pos[3];
	float rot[3];
	float scale;
	char file[512];
	BB_MATRIX rot_matrix;
	vector<int> model_feature_links;//indices to link to feature points
};


//holder for primitives used for layers, allows the primitive type to change
//without all layers knowing about it
class SCENE_PRIMITIVE
{
public:
	SCENE_PRIMITIVE()
	{
		primitive = 0;
		inverted = false;
		visible = false;
	}
	~SCENE_PRIMITIVE()
	{
		if(primitive)
		{
			delete primitive;
		}
	}
	__forceinline bool Project_Vertex_To_Geometry(float *origin, float *dir, float *new_pos)
	{
		float far_pos[3];
		float start[3];
		float d = 10000;//FIXTHIS could be slimmed down A LOT
		if(inverted)
		{
			start[0] = origin[0]+(dir[0]*d);
			start[1] = origin[1]+(dir[1]*d);
			start[2] = origin[2]+(dir[2]*d);
			far_pos[0] = origin[0];
			far_pos[1] = origin[1];
			far_pos[2] = origin[2];
		}
		else
		{
			far_pos[0] = origin[0]+(dir[0]*d);
			far_pos[1] = origin[1]+(dir[1]*d);
			far_pos[2] = origin[2]+(dir[2]*d);
			start[0] = origin[0];
			start[1] = origin[1];
			start[2] = origin[2];
		}

		bool hit = false;
		switch(primitive->primitive_type)
		{
		case pt_PLANE:
			hit = ((PLANE*)primitive)->Clip_Ray(start, far_pos, new_pos);
			break;
		case pt_SPHERE:
			hit = ((SPHERE*)primitive)->Clip_Ray(start, far_pos, new_pos);
			break;
		case pt_CYLINDER:
			hit = ((CYLINDER*)primitive)->Clip_Ray(start, far_pos, new_pos);
			break;
		case pt_BOX:
			hit = ((BOX*)primitive)->Clip_Ray(start, far_pos, new_pos);
			break;
			//does not happen in realtime, special case
//		case pt_TRIMESH:
			break;
		};
		return hit;
	}
	__forceinline bool Adjust_Scale(float x, float y, float z)
	{
		primitive->Scale(x, y, z);
		return true;
	}
	PRIMITIVE *primitive;
	int layer_id;
	bool inverted;
	bool visible;
};

#endif