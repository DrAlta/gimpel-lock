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
#include "TriangleMesh.h"
#include "Primitives.h"



__forceinline void Get_Normal(float u, float v, float *n1, float *n2, float *n3, float *res)
{
	res[0] = ((1.0-(u+v))*n2[0]+n1[0]*u+n3[0]*v);
	res[1] = ((1.0-(u+v))*n2[1]+n1[1]*u+n3[1]*v);
	res[2] = ((1.0-(u+v))*n2[2]+n1[2]*u+n3[2]*v);
}

#define EPSILON 0.000001

__forceinline void CROSS(float *dest,float *v1,float *v2)
{
	dest[0] = v1[1]*v2[2]-v1[2]*v2[1];
	dest[1] = v1[2]*v2[0]-v1[0]*v2[2];
	dest[2] = v1[0]*v2[1]-v1[1]*v2[0];
}

__forceinline float DOT(float *v1,float *v2)
{
	return (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2]);
}

__forceinline void SUB(float *dest,float *v1,float *v2)
{
	dest[0] = v1[0]-v2[0];
	dest[1] = v1[1]-v2[1];
	dest[2] = v1[2]-v2[2];
}


//http://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
//Möller–Trumbore intersection algorithm

__forceinline bool intersect_triangle(float orig [3], float dir [3], float vert0 [3], float vert1 [3], float vert2 [3], float *t, float *u, float *v)
{
	float edge1 [3], edge2 [3], tvec [3], pvec [3], qvec [3];
	float det,inv_det;
	/* find vectors for two edges sharing vert0 */
	SUB(edge1, vert1, vert0);
	SUB(edge2, vert2, vert0);
	/* begin calculating determinant - also used to calculate U parameter */
	CROSS(pvec, dir, edge2);
	/* if determinant is near zero, ray lies in plane of triangle */
	det = DOT(edge1, pvec);
#ifdef TEST_CULL
	/* define TEST_CULL if culling is desired */
	if (det < EPSILON)
		return false;
	/* calculate distance from vert0 to ray origin */
	SUB(tvec, orig, vert0);
	/* calculate U parameter and test bounds */
	*u = DOT(tvec, pvec);
	if (*u < 0.0 || *u > det)
		return false;
	/* prepare to test V parameter */
	CROSS(qvec, tvec, edge1);
	/* calculate V parameter and test bounds */
	*v = DOT(dir, qvec);
	if (*v < 0.0 || *u + *v > det)
		return 0;
	/* calculate t, scale parameters, ray intersects triangle */
	*t = DOT(edge2, qvec);
	inv_det = 1.0 / det;
	*t *= inv_det;
	*u *= inv_det;
	*v *= inv_det;
#else
	/* the non-culling branch */
	if (det > -EPSILON && det < EPSILON)
		return false;
	inv_det = 1.0 / det;
	/* calculate distance from vert0 to ray origin */
	SUB(tvec, orig, vert0);
	/* calculate U parameter and test bounds */
	*u = DOT(tvec, pvec) * inv_det;
	if (*u < 0.0 || *u > 1.0)
		return false;
	/* prepare to test V parameter */
	CROSS(qvec, tvec, edge1);
	/* calculate V parameter and test bounds */
	*v = DOT(dir, qvec) * inv_det;
	if (*v < 0.0 || *u + *v > 1.0)
		return false;
	/* calculate t, ray intersects triangle */
	*t = DOT(edge2, qvec) * inv_det;
#endif
	return true;
}


bool Get_AABB(TRIANGLE_MESH *tm)
{
	int n = tm->num_vertices;
	if(n==0)return false;
	tm->aabb_lo[0] = tm->aabb_hi[0] =  tm->vertices[0];
	tm->aabb_lo[1] = tm->aabb_hi[1] =  tm->vertices[1];
	tm->aabb_lo[2] = tm->aabb_hi[2] =  tm->vertices[2];
	for(int i = 1;i<n;i++)
	{
		if(tm->aabb_lo[0]>tm->vertices[i*3])tm->aabb_lo[0]=tm->vertices[i*3];
		if(tm->aabb_lo[1]>tm->vertices[(i*3)+1])tm->aabb_lo[1]=tm->vertices[(i*3)+1];
		if(tm->aabb_lo[2]>tm->vertices[(i*3)+2])tm->aabb_lo[2]=tm->vertices[(i*3)+2];
		if(tm->aabb_hi[0]<tm->vertices[i*3])tm->aabb_hi[0]=tm->vertices[i*3];
		if(tm->aabb_hi[1]<tm->vertices[(i*3)+1])tm->aabb_hi[1]=tm->vertices[(i*3)+1];
		if(tm->aabb_hi[2]<tm->vertices[(i*3)+2])tm->aabb_hi[2]=tm->vertices[(i*3)+2];
	}
	return true;
}

bool Get_Triangle_Mesh_Search_Tree_File(char *file, char *res)
{
	strcpy(res, file);
	char *c = strrchr(res, '.');
	if(c)
	{
		strcpy(c, ".rst");
		return true;
	}
	return false;
}

bool Enumerate_Triangles(TRIANGLE_MESH *tm)
{
	int n = tm->num_triangles;
	for(int i = 0;i<n;i++)
	{
		tm->triangles[i].material = i;
	}
	return true;
}

bool Set_Triangle_Materials(TRIANGLE_MESH *tm)
{
	int n = tm->num_material_blocks;
	for(int i = 0;i<n;i++)
	{
		for(int j = 0;j<tm->material_blocks[i].num_triangles;j++)
		{
			tm->triangles[j+tm->material_blocks[i].start_index].material = tm->material_blocks[i].material;
		}
	}
	return true;
}

__forceinline bool Print_TST_Node(TRIANGLE_MESH_KD_NODE *node, FILE *f)
{
	fprintf(f, "\n---------------\nNode index %i\n", node->node_index);
	fprintf(f, "Num tries %i\n", node->num_tris);
	fprintf(f, "AABB lo %f %f %f\n", node->aabb_lo[0], node->aabb_lo[1], node->aabb_lo[2]);
	fprintf(f, "AABB hi %f %f %f\n", node->aabb_hi[0], node->aabb_hi[1], node->aabb_hi[2]);
	fprintf(f, "Axes %i %i %i\n", node->axis, node->axis2, node->axis3);
	if(node->pos_branch)
	{
		fprintf(f, "Pos branch index %i\n", node->pos_branch->node_index);
	}
	else
	{
		fprintf(f, "Pos branch index -1\n");
	}
	if(node->neg_branch)
	{
		fprintf(f, "Neg branch index %i\n", node->neg_branch->node_index);
	}
	else
	{
		fprintf(f, "Neg branch index -1\n");
	}
	for(int i = 0;i<node->num_tris;i++)
	{
		fprintf(f, "Tri %i index %i\n", i, node->tris[i]->material);
	}
	return true;
}

__forceinline bool Print_Nodes(TRIANGLE_MESH *tm, char *file)
{
	FILE *f = fopen(file, "wt");
	int n = tm->search_tree->num_nodes;
	for(int i = 0;i<n;i++)
	{
		Print_TST_Node(tm->search_tree->nodes[i], f);
	}
	fclose(f);
	return true;
}

__forceinline bool Load_TST_Node(TRIANGLE_MESH *tm, FILE *f, TRIANGLE_MESH_KD_NODE *node)
{
	fread(&node->node_index, sizeof(int), 1, f);
	fread(&node->num_tris, sizeof(int), 1, f);
	fread(node->aabb_lo, sizeof(float), 3, f);
	fread(node->aabb_hi, sizeof(float), 3, f);
	fread(node->split_pos, sizeof(float), 3, f);
	fread(&node->axis, sizeof(int), 1, f);
	fread(&node->axis2, sizeof(int), 1, f);
	fread(&node->axis3, sizeof(int), 1, f);
	int ni = -1;
	fread(&ni, sizeof(int), 1, f);
	if(ni!=-1)
	{
		node->pos_branch = tm->search_tree->nodes[ni];
	}
	else
	{
		node->pos_branch = 0;
	}
	fread(&ni, sizeof(int), 1, f);
	if(ni!=-1)
	{
		node->neg_branch = tm->search_tree->nodes[ni];
	}
	else
	{
		node->neg_branch = 0;
	}
	if(node->num_tris>0)
	{
		node->tris = new ITRIANGLE*[node->num_tris];
		int ti = 0;
		for(int i = 0;i<node->num_tris;i++)
		{
			fread(&ti, sizeof(int), 1, f);
			node->tris[i] = &tm->triangles[ti];
		}
	}
	else
	{
		node->tris = 0;
	}
	return true;
}

bool Load_Triangle_Mesh_Search_Tree(TRIANGLE_MESH *tm, char *file)
{
	printf("Attemping to load search tree from \"%s\"\n", file);
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		printf("Can't open file \"%s\" for reading!\n", file);
		return false;
	}
	int n = 0;
	fread(&n, sizeof(int), 1, f);
	int root_index = 0;
	fread(&root_index, sizeof(int), 1, f);
	tm->search_tree->num_nodes = n;
	TRIANGLE_MESH_KD_NODE *memnodes = new TRIANGLE_MESH_KD_NODE[n];
	tm->search_tree->nodes = new TRIANGLE_MESH_KD_NODE*[n];
	int i;
	for(i = 0;i<n;i++)
	{
		tm->search_tree->nodes[i] = &memnodes[i];
	}
	for(i = 0;i<n;i++)
	{
		Load_TST_Node(tm, f, tm->search_tree->nodes[i]);
	}
	fclose(f);
	tm->search_tree->root = tm->search_tree->nodes[root_index];
	tm->search_tree->search_nodes = new TRIANGLE_MESH_KD_NODE*[tm->search_tree->num_nodes];
	tm->search_tree->num_search_nodes = 0;
	Enumerate_Triangles(tm);
	Set_Triangle_Materials(tm);
	return true;
}

__forceinline bool Save_TST_Node(FILE *f, TRIANGLE_MESH_KD_NODE *node)
{
	fwrite(&node->node_index, sizeof(int), 1, f);
	fwrite(&node->num_tris, sizeof(int), 1, f);
	fwrite(node->aabb_lo, sizeof(float), 3, f);
	fwrite(node->aabb_hi, sizeof(float), 3, f);
	fwrite(node->split_pos, sizeof(float), 3, f);
	fwrite(&node->axis, sizeof(int), 1, f);
	fwrite(&node->axis2, sizeof(int), 1, f);
	fwrite(&node->axis3, sizeof(int), 1, f);
	int ni = -1;
	if(node->pos_branch)
	{
		fwrite(&node->pos_branch->node_index, sizeof(int), 1, f);
	}
	else
	{
		fwrite(&ni, sizeof(int), 1, f);
	}
	if(node->neg_branch)
	{
		fwrite(&node->neg_branch->node_index, sizeof(int), 1, f);
	}
	else
	{
		fwrite(&ni, sizeof(int), 1, f);
	}
	for(int i = 0;i<node->num_tris;i++)
	{
		fwrite(&node->tris[i]->material, sizeof(int), 1, f);
	}
	return true;
}

bool Save_Triangle_Mesh_Search_Tree(TRIANGLE_MESH *tm, char *file)
{
	printf("Saving search tree to \"%s\"\n", file);
	FILE *f = fopen(file, "wb");
	if(!f)
	{
		printf("Can't open file \"%s\" for writing!\n", file);
		return false;
	}
	Enumerate_Triangles(tm);
	int n = tm->search_tree->num_nodes;
	fwrite(&n, sizeof(int), 1, f);
	fwrite(&tm->search_tree->root->node_index, sizeof(int), 1, f);
	for(int i = 0;i<n;i++)
	{
		Save_TST_Node(f, tm->search_tree->nodes[i]);
	}
	fclose(f);
	Set_Triangle_Materials(tm);
	return true;
}


bool Render_Material_Block(TRIANGLE_MESH *tm, MATERIAL_BLOCK *mb)
{
	glBegin(GL_TRIANGLES);
	for(int i = 0;i<mb->num_triangles;i++)
	{
		ITRIANGLE *it = &tm->triangles[mb->start_index+i];
		glNormal3fv(&tm->normals[it->vna]);
		glVertex3fv(&tm->vertices[it->va]);
		glNormal3fv(&tm->normals[it->vnb]);
		glVertex3fv(&tm->vertices[it->vb]);
		glNormal3fv(&tm->normals[it->vnc]);
		glVertex3fv(&tm->vertices[it->vc]);
	}
	glEnd();
	return true;
}

bool Render_Triangle_Mesh(TRIANGLE_MESH *tm)
{
	for(int i = 0;i<tm->num_material_blocks;i++)
	{
		Render_Material_Block(tm, &tm->material_blocks[i]);
	}
	return true;
}

//INACCURATE AND MISSES EDGE HITS!!!

__forceinline bool oCheck_Ray_Against_Triangle(TRIANGLE_MESH *tm, RAY_HIT *rh, ITRIANGLE *tri)
{
	float t,u,v;
	if(intersect_triangle(rh->ray_start, rh->ray_dir, &tm->vertices[tri->va], &tm->vertices[tri->vb], &tm->vertices[tri->vc], &t, &u, &v))
	{
		if(t<rh->distance)
		{
			rh->hit = true;
			rh->distance = t;
			rh->hit_pos[0] = rh->ray_start[0]+(rh->ray_dir[0]*t);
			rh->hit_pos[1] = rh->ray_start[1]+(rh->ray_dir[1]*t);
			rh->hit_pos[2] = rh->ray_start[2]+(rh->ray_dir[2]*t);
			return true;
		}
	}
	return false;
}

__forceinline void Get_Normals(TRIANGLE_MESH *tm, ITRIANGLE *tri)
{
	float *p1 = &tm->vertices[tri->va];
	float *p2 = &tm->vertices[tri->vb];
	float *p3 = &tm->vertices[tri->vc];
	float sa[3] = {p2[0]-p1[0],p2[1]-p1[1],p2[2]-p1[2]};
	float sb[3] = {p3[0]-p2[0],p3[1]-p2[1],p3[2]-p2[2]};
	float sc[3] = {p1[0]-p3[0],p1[1]-p3[1],p1[2]-p3[2]};
	glApp.math.Cross(sa, sb, tri->n);
	glApp.math.Normalize(tri->n);
	glApp.math.Cross(sa, tri->n, tri->tna);
	glApp.math.Cross(sb, tri->n, tri->tnb);
	glApp.math.Cross(sc, tri->n, tri->tnc);
}

void Get_All_Normals(TRIANGLE_MESH *tm)
{
	for(int i = 0;i<tm->num_triangles;i++)
	{
		Get_Normals(tm, &tm->triangles[i]);
	}
}

//more accurate but slower due to edge checks

__forceinline bool Check_Ray_Against_Triangle(TRIANGLE_MESH *tm, RAY_HIT *rh, ITRIANGLE *tri)
{
	float *p1 = &tm->vertices[tri->va];
	float *p2 = &tm->vertices[tri->vb];
	float *p3 = &tm->vertices[tri->vc];
	if(glApp.math.Side_Of_Plane(rh->ray_start, tri->n, p1)==glApp.math.Side_Of_Plane(rh->hit_pos, tri->n, p1))
	{
		return false;
	}
	float hit[3];
	glApp.math.Plane_Intersection(rh->ray_start, rh->hit_pos, hit, p1, tri->n);
	if(glApp.math.Side_Of_Plane(hit, tri->tna, p1)==1)return false;
	if(glApp.math.Side_Of_Plane(hit, tri->tnb, p2)==1)return false;
	if(glApp.math.Side_Of_Plane(hit, tri->tnc, p3)==1)return false;
	rh->hit = true;
	rh->hit_pos[0] = hit[0];
	rh->hit_pos[1] = hit[1];
	rh->hit_pos[2] = hit[2];
	rh->distance = glApp.math.Distance3D(rh->ray_start, rh->hit_pos);
	return true;
}

__forceinline bool Test_Node_Triangles(TRIANGLE_MESH *tm, RAY_HIT *rh, TRIANGLE_MESH_KD_NODE *node, ITRIANGLE **hit_tri)
{
	for(int i = 0;i<node->num_tris;i++)
	{
		if(Check_Ray_Against_Triangle(tm, rh, node->tris[i]))
		{
			*hit_tri = node->tris[i];
		}
	}
	return rh->hit;
}

bool Search_Cast_Ray_Into_Triangle_Mesh(TRIANGLE_MESH *tm, RAY_HIT *rh)
{
	TRIANGLE_MESH_KD_NODE *current_node = tm->search_tree->root;
	TRIANGLE_MESH_KD_NODE *near_node, *far_node;
	bool done = false;
	tm->search_tree->num_search_nodes = 0;
	memcpy(current_node->ray_start, rh->ray_start, sizeof(float)*3);
	ITRIANGLE *hit_tri = 0;
	while(!done)
	{
		//test current boxes, they will overlap into near and far regions
		Test_Node_Triangles(tm, rh, current_node, &hit_tri);
		Get_Near_Far_Nodes(rh->hit_pos, rh->ray_dir, current_node, (KD_NODE**)&near_node, (KD_NODE**)&far_node);
		if(near_node&&far_node)
		{
			tm->search_tree->search_nodes[tm->search_tree->num_search_nodes] = far_node;tm->search_tree->num_search_nodes++;
			current_node = near_node;
		}
		else
		{
			if(near_node)
			{
				current_node = near_node;
			}
			else if(far_node)
			{
				current_node = far_node;
			}
			else if(tm->search_tree->num_search_nodes>0)
			{
				tm->search_tree->num_search_nodes--;
				current_node = tm->search_tree->search_nodes[tm->search_tree->num_search_nodes];
			}
			else done = true;
		}
	}
	if(rh->hit)
	{
		return true;
	}
	return false;
}

