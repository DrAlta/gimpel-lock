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
#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "GLBasic.h"

class TRIANGLE_MESH_SEARCH_TREE;

struct ITRIANGLE
{
	    //vertex  texcoord  normals
	int va,       vta,      vna;
	int vb,       vtb,      vnb;
	int vc,       vtc,      vnc;
	int material;
	float n[3];//face normal
	float tna[3];//edge normals
	float tnb[3];
	float tnc[3];
};

struct MATERIAL_BLOCK
{
	int material;
	int start_index;
	int num_triangles;
};

class TRIANGLE_MESH
{
public:
	TRIANGLE_MESH()
	{
		vertices = 0;
		normals = 0;
		texcoords = 0;
		triangles = 0;
		material_blocks = 0;
		num_vertices = 0;
		num_normals = 0;
		num_texcoords = 0;
		num_triangles = 0;
		num_material_blocks = 0;
		smooth_lighting = false;
	}
	~TRIANGLE_MESH()
	{
		if(vertices)delete[] vertices;
		if(normals)delete[] normals;
		if(texcoords)delete[] texcoords;
		if(triangles)delete[] triangles;
		if(material_blocks)delete[] material_blocks;
	}
	float *vertices;//all xyz vertices
	float *normals;//all xyz normals
	float *texcoords;//all uvw coordinates
	ITRIANGLE *triangles;
	MATERIAL_BLOCK *material_blocks;
	int num_vertices;
	int num_normals;
	int num_texcoords;
	int num_triangles;
	int num_material_blocks;
	bool smooth_lighting;
	char name[512];
	float aabb_lo[3];
	float aabb_hi[3];
	TRIANGLE_MESH_SEARCH_TREE *search_tree;
};


#include "KDNode.h"

class TRIANGLE_MESH_KD_NODE : public KD_NODE
{
public:
	TRIANGLE_MESH_KD_NODE()
	{
		num_tris = 0;;
		tris = 0;
		pos_branch = 0;
		neg_branch = 0;
	}
	~TRIANGLE_MESH_KD_NODE()
	{
		Free_Data();
	}
	__forceinline bool Free_Data()
	{
		if(tris)delete[] tris;
		num_tris = 0;;
		tris = 0;
		pos_branch = 0;
		neg_branch = 0;
		return true;
	}
	int num_tris;
	ITRIANGLE* *tris;
};

#define MAX_TRIANGLES_PER_NODE 1

class TRIANGLE_MESH_SEARCH_TREE
{
public:
	TRIANGLE_MESH_SEARCH_TREE()
	{
		num_nodes = 0;
		num_search_nodes = 0;
		search_nodes = 0;
		root = 0;
		nodes = 0;
	}
	~TRIANGLE_MESH_SEARCH_TREE()
	{
		Free_Data();
	}
	__forceinline bool Free_Data()
	{
		for(int i = 0;i<num_nodes;i++)
		{
			delete nodes[i];
		}
		if(nodes)delete[] nodes;
		if(search_nodes)delete[] search_nodes;
		num_nodes = 0;
		nodes = 0;
		search_nodes = 0;
		root = 0;
		return true;
	}
	__forceinline void Get_Triangle_AABB(ITRIANGLE *t, float *lo, float *hi)
	{
		float *p1 = &trimesh->vertices[t->va];
		float *p2 = &trimesh->vertices[t->vb];
		float *p3 = &trimesh->vertices[t->vc];
		lo[0] = hi[0] = p1[0];
		lo[1] = hi[1] = p1[1];
		lo[2] = hi[2] = p1[2];
		if(lo[0]>p2[0])lo[0]=p2[0];if(lo[1]>p2[1])lo[1]=p2[1];if(lo[2]>p2[2])lo[2]=p2[2];
		if(hi[0]<p2[0])hi[0]=p2[0];if(hi[1]<p2[1])hi[1]=p2[1];if(hi[2]<p2[2])hi[2]=p2[2];
		if(lo[0]>p3[0])lo[0]=p3[0];if(lo[1]>p3[1])lo[1]=p3[1];if(lo[2]>p3[2])lo[2]=p3[2];
		if(hi[0]<p3[0])hi[0]=p3[0];if(hi[1]<p3[1])hi[1]=p3[1];if(hi[2]<p3[2])hi[2]=p3[2];
	}
	__forceinline void Get_Triangle_Center(ITRIANGLE *t, float *res)
	{
		float *p1 = &trimesh->vertices[t->va];
		float *p2 = &trimesh->vertices[t->vb];
		float *p3 = &trimesh->vertices[t->vc];
		res[0] = (p1[0]+p2[0]+p3[0])/3;
		res[1] = (p1[1]+p2[1]+p3[1])/3;
		res[2] = (p1[2]+p2[2]+p3[2])/3;
	}
	__forceinline bool Get_Average_Pos(vector<ITRIANGLE*> *tris, float *pos)
	{
		pos[0] = 0;pos[1] = 0;pos[2] = 0;
		int n = tris->size();
		float p[3];
		for(int i = 0;i<n;i++)
		{
			Get_Triangle_Center((*tris)[i], p);
			pos[0] += p[0];
			pos[1] += p[1];
			pos[2] += p[2];
		}
		pos[0] = pos[0]/n;
		pos[1] = pos[1]/n;
		pos[2] = pos[2]/n;
		return true;
	}
	__forceinline ITRIANGLE* Get_Closest_Triangle(vector<ITRIANGLE*> *tris, float *pos)
	{
		int best = 0;
		float diff;
		int n = tris->size();
		float p[3];
		Get_Triangle_Center((*tris)[0], p);
		float lowest = glApp.math.Distance3D(pos, p);
		for(int i = 1;i<n;i++)
		{
			Get_Triangle_Center((*tris)[i], p);
			diff = glApp.math.Distance3D(pos, p);
			if(diff<lowest)
			{
				best = i;
			}
		}
		return (*tris)[best];
	}
	__forceinline void Check_LOHI(ITRIANGLE *t, float *lo, float *hi)
	{
		float aabb_lo[3];
		float aabb_hi[3];
		Get_Triangle_AABB(t, aabb_lo, aabb_hi);
		if(lo[0]>aabb_lo[0])lo[0]=aabb_lo[0];
		if(lo[1]>aabb_lo[1])lo[1]=aabb_lo[1];
		if(lo[2]>aabb_lo[2])lo[2]=aabb_lo[2];
		if(hi[0]<aabb_hi[0])hi[0]=aabb_hi[0];
		if(hi[1]<aabb_hi[1])hi[1]=aabb_hi[1];
		if(hi[2]<aabb_hi[2])hi[2]=aabb_hi[2];
	}
	__forceinline void Get_Space(vector<ITRIANGLE*> *tris, float *lo, float *hi)
	{
		int n = tris->size();
		Get_Triangle_AABB((*tris)[0], lo, hi);
		for(int i = 1;i<n;i++)
		{
			Check_LOHI((*tris)[i], lo, hi);
		}
	}
	__forceinline int Check_Side(ITRIANGLE *t, float *pos, int axis)
	{
		float *p1 = &trimesh->vertices[t->va];
		float *p2 = &trimesh->vertices[t->vb];
		float *p3 = &trimesh->vertices[t->vc];
		int a = 1;
		int b = 1;
		int c = 1;
		if(p1[axis]<=pos[axis]&&p2[axis]<=pos[axis]&&p3[axis]<=pos[axis])return -1;
		if(p1[axis]>=pos[axis]&&p2[axis]>=pos[axis]&&p3[axis]>=pos[axis])return 1;
		return 0;
	}
	__forceinline bool Test_Split_Balance(vector<ITRIANGLE*> *tris, float *pos, int axis, int *pos_cnt, int *neg_cnt, int *split_cnt)
	{
		int n = tris->size();
		int side;
		for(int i = 0;i<n;i++)
		{
			side = Check_Side((*tris)[i], pos, axis);
			if(side==-1)(*neg_cnt)++;
			if(side==1)(*pos_cnt)++;
			if(side==0)(*split_cnt)++;
		}
		return true;
	}
	__forceinline int Get_ImBalance(int neg, int pos, int split)
	{
		int diff = pos-neg;
		if(diff<0)diff = -diff;
		return diff + split;
	}
	__forceinline int Lowest(int *v)
	{
		if(v[0]<=v[1]&&v[0]<=v[2])return 0;
		if(v[1]<=v[0]&&v[1]<=v[2])return 1;
		if(v[2]<=v[1]&&v[2]<=v[0])return 2;
		return -1;
	}
	__forceinline bool Get_Best_Split(vector<ITRIANGLE*> *tris, ITRIANGLE *t, float *pos, int *axis, int *neg_cnt, int *pos_cnt, int *split_cnt)
	{
		int lo_pos_cnt[3] = {0,0,0};//all 3 axes for aabb_lo
		int lo_neg_cnt[3] = {0,0,0};
		int lo_split_cnt[3] = {0,0,0};
		int hi_pos_cnt[3] = {0,0,0};//all 3 axes for aabb_hi
		int hi_neg_cnt[3] = {0,0,0};
		int hi_split_cnt[3] = {0,0,0};
		int lo_balance[3] = {0,0,0};
		int hi_balance[3] = {0,0,0};
		float aabb_lo[3];
		float aabb_hi[3];
		Get_Triangle_AABB(t, aabb_lo, aabb_hi);
		Test_Split_Balance(tris, aabb_lo, 0, &lo_pos_cnt[0], &lo_neg_cnt[0], &lo_split_cnt[0]);
		Test_Split_Balance(tris, aabb_lo, 1, &lo_pos_cnt[1], &lo_neg_cnt[1], &lo_split_cnt[1]);
		Test_Split_Balance(tris, aabb_lo, 2, &lo_pos_cnt[2], &lo_neg_cnt[2], &lo_split_cnt[2]);
		Test_Split_Balance(tris, aabb_hi, 0, &hi_pos_cnt[0], &hi_neg_cnt[0], &hi_split_cnt[0]);
		Test_Split_Balance(tris, aabb_hi, 1, &hi_pos_cnt[1], &hi_neg_cnt[1], &hi_split_cnt[1]);
		Test_Split_Balance(tris, aabb_hi, 2, &hi_pos_cnt[2], &hi_neg_cnt[2], &hi_split_cnt[2]);
		int lo_ib[3];
		int hi_ib[3];
		lo_ib[0] = Get_ImBalance(lo_neg_cnt[0], lo_pos_cnt[0], lo_split_cnt[0]);
		lo_ib[1] = Get_ImBalance(lo_neg_cnt[1], lo_pos_cnt[1], lo_split_cnt[1]);
		lo_ib[2] = Get_ImBalance(lo_neg_cnt[2], lo_pos_cnt[2], lo_split_cnt[2]);
		hi_ib[0] = Get_ImBalance(hi_neg_cnt[0], hi_pos_cnt[0], hi_split_cnt[0]);
		hi_ib[1] = Get_ImBalance(hi_neg_cnt[1], hi_pos_cnt[1], hi_split_cnt[1]);
		hi_ib[2] = Get_ImBalance(hi_neg_cnt[2], hi_pos_cnt[2], hi_split_cnt[2]);
		int lo = Lowest(lo_ib);
		int hi = Lowest(hi_ib);
		if(lo<hi)
		{
			*axis = lo;
			pos[0] = aabb_lo[0];
			pos[1] = aabb_lo[1];
			pos[2] = aabb_lo[2];
			*neg_cnt = lo_neg_cnt[lo];
			*pos_cnt = lo_pos_cnt[lo];
			*split_cnt = lo_split_cnt[lo];
		}
		else
		{
			*axis = hi;
			pos[0] = aabb_hi[0];
			pos[1] = aabb_hi[1];
			pos[2] = aabb_hi[2];
			*neg_cnt = hi_neg_cnt[hi];
			*pos_cnt = hi_pos_cnt[hi];
			*split_cnt = hi_split_cnt[hi];
		}
		if(*neg_cnt==0&&*split_cnt==0)return false;//lopsided
		if(*pos_cnt==0&&*split_cnt==0)return false;
		return true;
	}
	__forceinline bool Find_Best_Split(vector<ITRIANGLE*> *tris, float *pos, int *axis)
	{
		int pos_cnt = 0;
		int neg_cnt = 0;
		int split_cnt = 0;
		int best = -1;
		int n = tris->size();
		int lowest_ib = n*5;
		int ib = 0;
		float tpos[3];
		int taxis;
		if(n<=max_triangles_per_branch||max_triangles_per_branch==-1)
		{
			//test all variations
			for(int i = 0;i<n;i++)
			{
				pos_cnt = 0;
				neg_cnt = 0;
				split_cnt = 0;
				if(Get_Best_Split(tris, (*tris)[i], tpos, &taxis, &neg_cnt, &pos_cnt, &split_cnt))
				{
					ib = Get_ImBalance(neg_cnt, pos_cnt, split_cnt);
					if(ib<lowest_ib||best==-1)
					{
						best = i;
						lowest_ib = ib;
						pos[0] = tpos[0];
						pos[1] = tpos[1];
						pos[2] = tpos[2];
						*axis = taxis;
					}
				}
			}
		}
		if(best==-1)
		{
			//no best split found OR way to many to test all possible branches
			Get_Average_Pos(tris, pos);
			Get_Best_Split(tris, Get_Closest_Triangle(tris, pos), tpos, axis, &neg_cnt, &pos_cnt, &split_cnt);
			if((neg_cnt==0&&split_cnt==0)||(pos_cnt==0&&split_cnt==0))
			{
				return false;
			}
		}
		return true;
	}
	__forceinline bool Get_Split_Lists(vector<ITRIANGLE*> *tris, float *pos, int axis, vector<ITRIANGLE*> *pos_list, vector<ITRIANGLE*> *neg_list, vector<ITRIANGLE*> *split_list)
	{
		int n = tris->size();
		int side;
		for(int i = 0;i<n;i++)
		{
			side = Check_Side((*tris)[i], pos, axis);
			if(side==-1)neg_list->push_back((*tris)[i]);
			if(side==1)pos_list->push_back((*tris)[i]);
			if(side==0)split_list->push_back((*tris)[i]);
		}
		return true;
	}
	__forceinline TRIANGLE_MESH_KD_NODE* Create_Node(vector<ITRIANGLE*> *tris)
	{
		static int cnt = 0;
		cnt++;if(cnt>=100){printf(".");cnt=0;}
		int n = tris->size();
		if(n==0){return 0;}
		float center[3];
		int axis;
		Get_Average_Pos(tris, center);
		TRIANGLE_MESH_KD_NODE *kn = new TRIANGLE_MESH_KD_NODE;
		vnodes.push_back(kn);
		Get_Space(tris, kn->aabb_lo, kn->aabb_hi);
		if(n<=MAX_TRIANGLES_PER_NODE)
		{
			kn->num_tris = n;
			kn->tris = new ITRIANGLE*[n];
			for(int i = 0;i<n;i++)
			{
				kn->tris[i] = (*tris)[i];
			}
			kn->axis = -1;
			return kn;
		}
		if(!Find_Best_Split(tris, center, &axis))
		{
			kn->num_tris = n;
			kn->tris = new ITRIANGLE*[n];
			for(int i = 0;i<n;i++)
			{
				kn->tris[i] = (*tris)[i];
			}
			kn->axis = -1;
			return kn;
		}
		vector<ITRIANGLE*> pos_list, neg_list, split_list;
		Get_Split_Lists(tris, center, axis, &pos_list, &neg_list, &split_list);
		int num_neg = neg_list.size();
		int num_pos = pos_list.size();
		int num_split = split_list.size();
		tris->clear();
		kn->num_tris = split_list.size();
		if(kn->num_tris>0)
		{
			kn->tris = new ITRIANGLE*[kn->num_tris];
			for(int i = 0;i<kn->num_tris;i++)
			{
				kn->tris[i] = split_list[i];
			}
		}
		split_list.clear();
		kn->pos_branch = Create_Node(&pos_list);
		pos_list.clear();
		kn->neg_branch = Create_Node(&neg_list);
		neg_list.clear();
		kn->axis = axis;
		kn->split_pos[0] = center[0];
		kn->split_pos[1] = center[1];
		kn->split_pos[2] = center[2];
		if(axis==0){kn->axis2 = 1;kn->axis3 = 2;}
		if(axis==1){kn->axis2 = 0;kn->axis3 = 2;}
		if(axis==2){kn->axis2 = 0;kn->axis3 = 1;}
		return kn;
	}
	__forceinline bool Create_Tree(TRIANGLE_MESH *tm, int max_per_branch)
	{
		Free_Data();
		int n = tm->num_triangles;
		vector<ITRIANGLE*> tri_list;
		int i;
		for(i = 0;i<n;i++)
		{
			tri_list.push_back(&tm->triangles[i]);
		}
		trimesh = tm;
		max_triangles_per_branch = max_per_branch;
		root = Create_Node(&tri_list);
		num_nodes = vnodes.size();
		nodes = new TRIANGLE_MESH_KD_NODE*[num_nodes];
		search_nodes = new TRIANGLE_MESH_KD_NODE*[num_nodes];
		for(i = 0;i<num_nodes;i++)
		{
			nodes[i] = vnodes[i];
			nodes[i]->node_index = i;
		}
		vnodes.clear();
		return true;
	}
	vector<TRIANGLE_MESH_KD_NODE*> vnodes;
	int num_nodes;
	int num_search_nodes;
	int max_triangles_per_branch;
	TRIANGLE_MESH_KD_NODE* *nodes;
	TRIANGLE_MESH_KD_NODE* *search_nodes;
	TRIANGLE_MESH_KD_NODE *root;
	TRIANGLE_MESH *trimesh;
};

bool Register_Triangle_Mesh(char *name, char *file);
TRIANGLE_MESH* Find_OBJ(char *file);
bool Free_All_OBJ_Meshes();
bool Render_Triangle_Mesh(TRIANGLE_MESH *tm);

class RAY_HIT;

bool Search_Cast_Ray_Into_Triangle_Mesh(TRIANGLE_MESH *tm, RAY_HIT *rh);


#endif