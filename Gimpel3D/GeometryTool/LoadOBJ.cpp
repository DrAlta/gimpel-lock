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


bool Get_AABB(TRIANGLE_MESH *tm);
bool Get_Triangle_Mesh_Search_Tree_File(char *file, char *res);
bool Load_Triangle_Mesh_Search_Tree(TRIANGLE_MESH *tm, char *file);
bool Save_Triangle_Mesh_Search_Tree(TRIANGLE_MESH *tm, char *file);

/*
4 sided face

f 1/1/1 2/2/2 3/3/3 4/4/4
f v/vt/vn v/vt/vn v/vt/vn v/vt/vn

vertex and normals only

f 1//1 2//2 3//3 4//4

*/

struct OBJ_MATERIAL
{
	char name[512];
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float opacity;
	float shininess;
	char texture_file[512];
	int rt_material_id;
};

struct OBJ_VERTEX{float x,y,z;};
struct OBJ_TEXCOORD{float u,v,w;};
struct OBJ_NORMAL{float x,y,z;};
struct OBJ_FACE{int va,vta,vna;int vb,vtb,vnb;int vc,vtc,vnc;int vd,vtd,vnd;};

vector<OBJ_VERTEX> obj_vertices;
vector<OBJ_TEXCOORD> obj_texcoords;
vector<OBJ_NORMAL> obj_normals;


class OBJ_FACE_BLOCK
{
public:
	OBJ_FACE_BLOCK(){}
	~OBJ_FACE_BLOCK(){faces.clear();}
	__forceinline bool Load_Face(char *line)
	{
		while(line[0]==' '){line = &line[1];}//skip spaces
		line = &line[1];//skip "f"
		OBJ_FACE f;
		f.vd = -1;//invalidate 4th component
		if(strstr(line, "//"))
		{
			//no texcoords, assume 4 components
			sscanf(line, "%i//%i %i//%i %i//%i %i//%i", &f.va, &f.vna, &f.vb, &f.vnb, &f.vc, &f.vnc, &f.vd, &f.vnd);
		}
		else
		{
			//with texcoords
			sscanf(line, "%i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i", &f.va, &f.vta, &f.vna, &f.vb, &f.vtb, &f.vnb, &f.vc, &f.vtc, &f.vnc, &f.vd, &f.vtd, &f.vnd);
		}
		faces.push_back(f);
		return true;
	}
	vector<OBJ_FACE> faces;
	int material;
};

class OBJ_GROUP
{
public:
	OBJ_GROUP()
	{
	}
	~OBJ_GROUP()
	{
		int n = face_blocks.size();
		for(int i = 0;i<n;i++)
		{
			delete face_blocks[i];
		}
		face_blocks.clear();
	}
	vector<OBJ_FACE_BLOCK*> face_blocks;
	char name[512];
};

vector<OBJ_GROUP*> obj_groups;
vector<OBJ_MATERIAL*> obj_materials;

bool Load_Default_OBJ_Material()
{
	OBJ_MATERIAL *m = new OBJ_MATERIAL;
	strcpy(m->name, "Default");
	m->ambient[0] = 0.2f;
	m->ambient[1] = 0.2f;
	m->ambient[2] = 0.2f;
	m->diffuse[0] = 0.8f;
	m->diffuse[1] = 0.8f;
	m->diffuse[2] = 0.8f;
	m->specular[0] = 1.0f;
	m->specular[1] = 1.0f;
	m->specular[2] = 1.0f;
	m->opacity = 1.0f;
	m->shininess = 0.0f;
	obj_materials.push_back(m);
	return true;
}


bool Free_OBJ_Groups()
{
	int n = obj_groups.size();
	for(int i = 0;i<n;i++)
	{
		delete obj_groups[i];
	}
	obj_groups.clear();
	return true;
}

bool Free_OBJ_Materials()
{
	int n = obj_materials.size();
	for(int i = 0;i<n;i++)
	{
		delete obj_materials[i];
	}
	obj_materials.clear();
	return true;
}

bool Free_OBJ_Vertex_Data()
{
	obj_vertices.clear();
	obj_texcoords.clear();
	obj_normals.clear();
	return true;
}

int Num_OBJ_Triangles()
{
	int cnt = 0;
	int ng = obj_groups.size();
	int i, j, k;
	for(i = 0;i<ng;i++)
	{
		int nfb = obj_groups[i]->face_blocks.size();
		for(j = 0;j<nfb;j++)
		{
			int nf = obj_groups[i]->face_blocks[j]->faces.size();
			for(k = 0;k<nf;k++)
			{
				if(obj_groups[i]->face_blocks[j]->faces[k].vd==-1)
				{
					cnt++;
				}
				else
				{
					cnt += 2;
				}
			}
		}
	}
	return cnt;
}

int Num_OBJ_Material_Blocks()
{
	int cnt = 0;
	int ng = obj_groups.size();
	int i, j;
	for(i = 0;i<ng;i++)
	{
		int nfb = obj_groups[i]->face_blocks.size();
		for(j = 0;j<nfb;j++)
		{
			int nf = obj_groups[i]->face_blocks[j]->faces.size();
			if(nf>0)
			{
				cnt++;
			}
		}
	}
	return cnt;
}

bool Register_OBJ_Materials()
{
	int n = obj_materials.size();
	for(int i = 0;i<n;i++)
	{
		OBJ_MATERIAL *m = obj_materials[i];
		float spec = (m->specular[0]+m->specular[1]+m->specular[2])/3;
		m->shininess = 0.5f;
		m->opacity = 1;
		m->rt_material_id = 0;
	}
	return true;
}

void Get_All_Normals(TRIANGLE_MESH *tm);

bool Create_Triangle_Mesh_From_OBJ_Data(TRIANGLE_MESH *tm)
{
	Register_OBJ_Materials();
	int tri_cnt = 0;
	int block_cnt = 0;
	int num_groups = obj_groups.size();
	int i, j, k;
	MATERIAL_BLOCK *current_block;
	ITRIANGLE *current_triangle;
	OBJ_FACE *current_face;
	tm->num_vertices = obj_vertices.size();
	tm->num_normals = obj_normals.size();
	tm->num_texcoords = obj_texcoords.size();
	tm->num_triangles = Num_OBJ_Triangles();
	tm->num_material_blocks = Num_OBJ_Material_Blocks();
	tm->vertices = new float[tm->num_vertices*3];
	float scale = 0.00006f;
	for(i = 0;i<tm->num_vertices;i++)
	{
		tm->vertices[i*3] = obj_vertices[i].x*scale;
		tm->vertices[(i*3)+1] = obj_vertices[i].y*scale;
		tm->vertices[(i*3)+2] = obj_vertices[i].z*scale;
	}
	obj_vertices.clear();
	tm->normals = new float[tm->num_normals*3];
	for(i = 0;i<tm->num_normals;i++)
	{
		tm->normals[i*3] = obj_normals[i].x;
		tm->normals[(i*3)+1] = obj_normals[i].y;
		tm->normals[(i*3)+2] = obj_normals[i].z;
	}
	obj_normals.clear();
	tm->texcoords = new float[tm->num_texcoords*3];
	for(i = 0;i<tm->num_texcoords;i++){tm->texcoords[i*3] = obj_texcoords[i].u;tm->texcoords[(i*3)+1] = obj_texcoords[i].v;tm->texcoords[(i*3)+2] = obj_texcoords[i].w;}
	obj_texcoords.clear();
	tm->material_blocks = new MATERIAL_BLOCK[tm->num_material_blocks];
	tm->triangles = new ITRIANGLE[tm->num_triangles];
	for(i = 0;i<num_groups;i++)
	{
		int num_face_blocks = obj_groups[i]->face_blocks.size();
		for(j = 0;j<num_face_blocks;j++)
		{
			current_block = &tm->material_blocks[block_cnt];block_cnt++;
			current_block->material = obj_materials[obj_groups[i]->face_blocks[j]->material]->rt_material_id;
			current_block->start_index = tri_cnt;
			current_block->num_triangles = 0;
			int num_faces = obj_groups[i]->face_blocks[j]->faces.size();
			if(num_faces>0)
			{
				if(obj_groups[i]->face_blocks[j]->faces[0].vd==-1)
				{
					for(k = 0;k<num_faces;k++)
					{
						current_triangle = &tm->triangles[tri_cnt];tri_cnt++;current_block->num_triangles++;
						current_triangle->material = current_block->material;
						current_face = &obj_groups[i]->face_blocks[j]->faces[k];
						current_triangle->va = (current_face->va-1)*3;current_triangle->vta = (current_face->vta-1)*3;current_triangle->vna = (current_face->vna-1)*3;
						current_triangle->vb = (current_face->vb-1)*3;current_triangle->vtb = (current_face->vtb-1)*3;current_triangle->vnb = (current_face->vnb-1)*3;
						current_triangle->vc = (current_face->vc-1)*3;current_triangle->vtc = (current_face->vtc-1)*3;current_triangle->vnc = (current_face->vnc-1)*3;
					}
				}
				else
				{
					for(k = 0;k<num_faces;k++)
					{
						current_triangle = &tm->triangles[tri_cnt];tri_cnt++;current_block->num_triangles++;
						current_triangle->material = current_block->material;
						current_face = &obj_groups[i]->face_blocks[j]->faces[k];
						current_triangle->va = (current_face->va-1)*3;current_triangle->vta = (current_face->vta-1)*3;current_triangle->vna = (current_face->vna-1)*3;
						current_triangle->vb = (current_face->vb-1)*3;current_triangle->vtb = (current_face->vtb-1)*3;current_triangle->vnb = (current_face->vnb-1)*3;
						current_triangle->vc = (current_face->vc-1)*3;current_triangle->vtc = (current_face->vtc-1)*3;current_triangle->vnc = (current_face->vnc-1)*3;
						//a tri might be slipped in here, verify it's a quad
						if(current_face->vd>=0)
						{
							current_triangle = &tm->triangles[tri_cnt];tri_cnt++;current_block->num_triangles++;
							current_triangle->material = current_block->material;
							current_triangle->va = (current_face->va-1)*3;current_triangle->vta = (current_face->vta-1)*3;current_triangle->vna = (current_face->vna-1)*3;
							current_triangle->vb = (current_face->vc-1)*3;current_triangle->vtb = (current_face->vtc-1)*3;current_triangle->vnb = (current_face->vnc-1)*3;
							current_triangle->vc = (current_face->vd-1)*3;current_triangle->vtc = (current_face->vtd-1)*3;current_triangle->vnc = (current_face->vnd-1)*3;
						}
					}
				}
			}
		}
	}
	Free_OBJ_Materials();
	Free_OBJ_Groups();
	Free_OBJ_Vertex_Data();
	Get_All_Normals(tm);
	return true;
}

int Find_OBJ_Material(char *name)
{
	int n = obj_materials.size();
	for(int i = 0;i<n;i++)
	{
		if(!strcmp(obj_materials[i]->name, name))
		{
			return i;
		}
	}
	return -1;
}

OBJ_GROUP* Start_OBJ_Group(char *line)
{
	char *c = strstr(line, "g");
	c = &c[1];
	while(c[0]==' ')
	{
		c = &c[1];
		if(c[0]==0)
		{
			printf("Error extracting name for new group at line \"%s\"\n", line);
			return 0;
		}
	}
	OBJ_GROUP *g = new OBJ_GROUP;
	strcpy(g->name, c);
	obj_groups.push_back(g);
	return g;
}

OBJ_MATERIAL* Start_OBJ_Material(char *line)
{
	char *c = strstr(line, "newmtl");
	c = &c[6];
	while(c[0]==' ')
	{
		c = &c[1];
		if(c[0]==0)
		{
			printf("Error extracting name for new material at line \"%s\"\n", line);
			return 0;
		}
	}
	OBJ_MATERIAL *m = new OBJ_MATERIAL;
	obj_materials.push_back(m);
	strcpy(m->name, c);
	m->ambient[0] = m->ambient[1] = m->ambient[2] = 0.2f;
	m->diffuse[0] = m->diffuse[1] = m->diffuse[2] = 0.8f;
	m->specular[0] = m->specular[1] = m->specular[2] = 1;
	m->opacity = 1;
	m->shininess = 0;
	m->texture_file[0] = 0;
	return m;
}

__forceinline bool Get_Next_OBJ_Line(FILE *f, char *line, char *token)
{
	line[0] = token[0] = 0;
	bool done = false;
	while(!done)
	{
		fgets(line, 512, f);
		if(line[0]==0&&feof(f))
		{
			done = true;
			return false;
		}
		else
		{
			char *c = strrchr(line, '\n');
			if(c)*c = 0;
			sscanf(line, "%s", token);
			return true;
		}
	}
	return false;
}

bool Load_MTL_File(char *file)
{
	FILE *f = fopen(file, "rt");
	if(!f)
	{
		printf("Can't open material file \"%s\" for reading!\n", file);
		return false;
	}
	printf("Loading MTL file \"%s\"\n", file);
	bool done = false;
	char line[512];
	char token[512];
	bool res = true;
	OBJ_MATERIAL *cmat = 0;
	while(!done)
	{
		if(!Get_Next_OBJ_Line(f, line, token))
		{
			done = true;
		}
		else
		{
			if(!strcmp(token, "newmtl"))//name starts a new material
			{
				cmat = Start_OBJ_Material(line);
				if(!cmat)
				{
					printf("Error loading material library!\n");
					return false;
				}
			}
			if(!strcmp(token, "Ka"))//r g b defines the ambient color of the material to be (r,g,b). The default is (0.2,0.2,0.2); 
			{
				if(!cmat){printf("Error loading material library, \"%s\" encountered with no material declared\n", line);return false;}
				sscanf(line, "%s %f %f %f", token, &cmat->ambient[0], &cmat->ambient[1], &cmat->ambient[2]);
			}
			if(!strcmp(token, "Kd"))//r g b defines the diffuse color of the material to be (r,g,b). The default is (0.8,0.8,0.8); 
			{
				if(!cmat){printf("Error loading material library, \"%s\" encountered with no material declared\n", line);return false;}
				sscanf(line, "%s %f %f %f", token, &cmat->diffuse[0], &cmat->diffuse[1], &cmat->diffuse[2]);
			}
			if(!strcmp(token, "Ks"))//r g b defines the specular color of the material to be (r,g,b). This color shows up in highlights. The default is (1.0,1.0,1.0); 
			{
				if(!cmat){printf("Error loading material library, \"%s\" encountered with no material declared\n", line);return false;}
				sscanf(line, "%s %f %f %f", token, &cmat->specular[0], &cmat->specular[1], &cmat->specular[2]);
			}
			if(!strcmp(token, "d"))//alpha defines the transparency of the material to be alpha. The default is 1.0 (not transparent at all) Some formats use Tr instead of d; 
			{
				if(!cmat){printf("Error loading material library, \"%s\" encountered with no material declared\n", line);return false;}
				sscanf(line, "%s %f", token, &cmat->opacity);
			}
			if(!strcmp(token, "Tr"))//alpha defines the transparency of the material to be alpha. The default is 1.0 (not transparent at all). Some formats use d instead of Tr; 
			{
				if(!cmat){printf("Error loading material library, \"%s\" encountered with no material declared\n", line);return false;}
				sscanf(line, "%s %f", token, &cmat->opacity);
			}
			if(!strcmp(token, "Ns"))//s defines the shininess of the material to be s. The default is 0.0; 
			{
				if(!cmat){printf("Error loading material library, \"%s\" encountered with no material declared\n", line);return false;}
				sscanf(line, "%s %f", token, &cmat->shininess);
			}
			if(!strcmp(token, "illum"))//n denotes the illumination model used by the material. illum = 1 indicates a flat material with no specular highlights, so the value of Ks is not used. illum = 2 denotes the presence of specular highlights, and so a specification for Ks is required. 
			{
				if(!cmat){printf("Error loading material library, \"%s\" encountered with no material declared\n", line);return false;}
			}
			if(!strcmp(token, "map_Ka"))//filename names a file containing a texture map, which should just be an ASCII dump of RGB values; 
			{
				if(!cmat){printf("Error loading material library, \"%s\" encountered with no material declared\n", line);return false;}
				sscanf(line, "%s %s", token, &cmat->texture_file);
			}
		}
	}
	fclose(f);
	return true;
}


bool Load_OBJ_Material_Lib(char *obj_file, char *line)
{
	char *c = strstr(line, "mtllib");
	c = &c[7];
	while(c[0]==' ')
	{
		c = &c[1];
		if(c[0]==0)
		{
			printf("Error loading OBJ material lib from line \"%s\", can't extract filename!\n", line);
			return false;
		}
	}
	char tfile[512];
	char sfile[512];
	char mfile[512];
	strcpy(tfile, obj_file);//full path + obj
	strcpy(sfile, c);//short filename from obj file
	c = strrchr(tfile, '\\');
	if(!c)
	{
		strcpy(mfile, tfile);
	}
	else
	{
		*c = 0;
		sprintf(mfile, "%s\\%s", tfile, sfile);
	}
	return Load_MTL_File(mfile);
}

TRIANGLE_MESH* Load_OBJ(char *file)
{
	FILE *f = fopen(file, "rt");
	if(!f)
	{
		printf("Can't open \"%s\" for reading!\n", file);
		return 0;
	}
	Print_Status("Loading OBJ file \"%s\"\n", file);
	bool done = false;
	char line[512];
	char token[512];
	bool res = true;
	OBJ_GROUP *cg = 0;
	OBJ_FACE_BLOCK *cfb = 0;
	int last_material_id = -1;

	while(!done)
	{
		if(!Get_Next_OBJ_Line(f, line, token))
		{
			done = true;
		}
		else
		{
			if(!strcmp(token, "mtllib"))
			{
				if(!Load_OBJ_Material_Lib(file, line))
				{
					Print_Status("Error loading materials for \"%s\"\n", file);
					Load_Default_OBJ_Material();
				}
			}
			if(!strcmp(token, "g"))
			{
				cg = Start_OBJ_Group(line);
				if(!cg)
				{
					Print_Status("Error loading OBJ group starting at line \"%s\"\n", line);
					done = true;
					res = false;
				}
				cfb = 0;
			}
			if(!strcmp(token, "v"))
			{
				OBJ_VERTEX v;
				sscanf(line, "%s %f %f %f", token, &v.x, &v.y, &v.z);
				float s = 500;
				v.x*=s;v.y*=s;v.z*=s;
				v.y += 100;
				obj_vertices.push_back(v);
			}
			if(!strcmp(token, "vt"))
			{
				OBJ_TEXCOORD vt;
				sscanf(line, "%s %f %f %f", token, &vt.u, &vt.v, &vt.w);
				obj_texcoords.push_back(vt);
			}
			if(!strcmp(token, "vn"))
			{
				OBJ_NORMAL vn;
				sscanf(line, "%s %f %f %f", token, &vn.x, &vn.y, &vn.z);
				obj_normals.push_back(vn);
			}
			if(!strcmp(token, "f"))
			{
				if(!cg){Print_Status("Error loading obj file, \"%s\" encountered with no group declared\n", line);done = true;res = false;}
				else
				{
					if(!cfb)
					{
						if(last_material_id==-1)
						{
							last_material_id = 0;
						}
						cfb = new OBJ_FACE_BLOCK;
						cfb->material = last_material_id;
						cg->face_blocks.push_back(cfb);
					}
					{
						if(!cfb->Load_Face(line))
						{
							Print_Status("Error loading face in group \"%s\" at line \"%s\"\n", cg->name, line);
							done = true;
							res = false;
						}
					}
				}
			}
		}
	}
	fclose(f);
	if(!res)
	{
		Free_OBJ_Materials();
		Free_OBJ_Groups();
		Free_OBJ_Vertex_Data();
		return 0;
	}
	else
	{
		TRIANGLE_MESH *triangle_mesh = new TRIANGLE_MESH;
		Print_Status("Loaded OBJ file \"%s\"\n", file);
		Print_Status("Creating triangle mesh..\n");
		Create_Triangle_Mesh_From_OBJ_Data(triangle_mesh);

		Get_AABB(triangle_mesh);
		strcpy(triangle_mesh->name, file);
		triangle_mesh->search_tree = new TRIANGLE_MESH_SEARCH_TREE;
		char tree_file[512];
		Get_Triangle_Mesh_Search_Tree_File(file, tree_file);
		if(!Load_Triangle_Mesh_Search_Tree(triangle_mesh, tree_file))
		{
			int max_triangles_per_branch = -1;
			triangle_mesh->search_tree->Create_Tree(triangle_mesh, max_triangles_per_branch);
			Save_Triangle_Mesh_Search_Tree(triangle_mesh, tree_file);
		}
		
		return triangle_mesh;
	}
	return 0;
}

class OBJ_MESH
{
public:
	OBJ_MESH()
	{
		tmesh = 0;
	}
	~OBJ_MESH()
	{
		if(tmesh)delete tmesh;
	}
	TRIANGLE_MESH *tmesh;
	char file[512];
};

vector<OBJ_MESH*> obj_meshes;

bool Free_All_OBJ_Meshes()
{
	int n = obj_meshes.size();
	for(int i = 0;i<n;i++)
	{
		delete obj_meshes[i];
	}
	obj_meshes.clear();
	return true;
}

TRIANGLE_MESH* Find_OBJ(char *file)
{
	int n = obj_meshes.size();
	for(int i = 0;i<n;i++)
	{
		if(!strcmp(obj_meshes[i]->file, file))
		{
			return obj_meshes[i]->tmesh;
		}
	}
	OBJ_MESH *om = new OBJ_MESH;
	om->tmesh = Load_OBJ(file);
	if(!om->tmesh)
	{
		delete om;
		return 0;
	}
	obj_meshes.push_back(om);
	strcpy(om->file, file);
	return om->tmesh;
}




