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
#include "../GeometryTool.h"
#include "../G3DCoreFiles/Frame.h"
#include "../G3DCoreFiles/Layers.h"
#include "Primitives.h"
#include "../Skin.h"

GL_BASIC_APP glApp;

bool show_geometry = false;
bool show_geometry_wireframe = false;

bool Reset_Show_Geometry_Flag();


GLUquadric *quadric = 0;


TRIANGLE_MESH* Find_OBJ(char *file);

//separate global list of primitives used for all layers for all frames

//this can be re-used for all layers that share the id regardless of frame

vector<SCENE_PRIMITIVE*> scene_primitives;

bool Free_Scene_Primitives()
{
	int n = scene_primitives.size();
	for(int i = 0;i<n;i++)
	{
		delete scene_primitives[i];
	}
	scene_primitives.clear();
	show_geometry = false;
	Free_All_OBJ_Meshes();
	return true;
}

//create a default plane for new layers
SCENE_PRIMITIVE* New_Scene_Primitive(int layer_id)
{
	SCENE_PRIMITIVE *sp = new SCENE_PRIMITIVE;
	sp->primitive = new PLANE;
	sp->layer_id = layer_id;
	scene_primitives.push_back(sp);
	return sp;
}

SCENE_PRIMITIVE* Find_Scene_Primitive(int layer_id)
{
	int n = scene_primitives.size();
	for(int i = 0;i<n;i++)
	{
		if(scene_primitives[i]->layer_id==layer_id)
		{
			return scene_primitives[i];
		}
	}
	return New_Scene_Primitive(layer_id);
}

SCENE_PRIMITIVE* Get_Scene_Primitive(int layer_id)
{
	int n = scene_primitives.size();
	for(int i = 0;i<n;i++)
	{
		if(scene_primitives[i]->layer_id==layer_id)
		{
			return scene_primitives[i];
		}
	}
	return 0;
}

bool Get_Geometry_Filename(char *res)
{
	char file[512];
	if(!Get_Project_Filename(file))
	{
		if(!frame)
		{
			return false;
		}
		strcpy(file, frame->filename);
	}
	char *c = strrchr(file, '.');
	if(!c)
	{
		//this never happens
		return false;
	}
	strcpy(c, ".sgm");
	strcpy(res, file);
	return true;
}

bool Get_Geometry_Filename(char *file, char *res)
{
	strcpy(res, file);
	char *c = strrchr(res, '.');
	if(!c)
	{
		//this never happens
		return false;
	}
	strcpy(c, ".sgm");
	return true;
}


struct SCENE_PRIMITIVE_HEADER
{
	int type;
	int layer_id;
	bool inverted;
	float size[3];
	float pos[3];
	float rot[3];
};

bool Save_Scene_Primitive(SCENE_PRIMITIVE *sp, FILE *f)
{
	SCENE_PRIMITIVE_HEADER sph;
	sph.inverted = sp->inverted;
	sph.layer_id = sp->layer_id;
	sph.type = sp->primitive->primitive_type;
	PLANE *plane;
	SPHERE *sphere;
	CYLINDER *cylinder;
	BOX *box;
	TRIMESH *trimesh;
	switch(sp->primitive->primitive_type)
	{
		case pt_PLANE:
			plane = (PLANE*)sp->primitive;
			sph.pos[0] = plane->plane_pos[0];
			sph.pos[1] = plane->plane_pos[1];
			sph.pos[2] = plane->plane_pos[2];
			sph.rot[0] = plane->plane_dir[0];
			sph.rot[1] = plane->plane_dir[1];
			sph.rot[2] = plane->plane_dir[2];
			break;
		case pt_SPHERE:
			sphere = (SPHERE*)sp->primitive;
			sph.pos[0] = sphere->pos[0];
			sph.pos[1] = sphere->pos[1];
			sph.pos[2] = sphere->pos[2];
			sph.size[0] = sphere->size;
			break;
		case pt_CYLINDER:
			cylinder = (CYLINDER*)sp->primitive;
			sph.pos[0] = cylinder->pos[0];
			sph.pos[1] = cylinder->pos[1];
			sph.pos[2] = cylinder->pos[2];
			sph.rot[0] = cylinder->rot[0];
			sph.rot[1] = cylinder->rot[1];
			sph.rot[2] = cylinder->rot[2];
			sph.size[0] = cylinder->base;
			sph.size[1] = cylinder->top;
			sph.size[2] = cylinder->height;
			break;
		case pt_BOX:
			box = (BOX*)sp->primitive;
			sph.pos[0] = box->pos[0];
			sph.pos[1] = box->pos[1];
			sph.pos[2] = box->pos[2];
			sph.rot[0] = box->rot[0];
			sph.rot[1] = box->rot[1];
			sph.rot[2] = box->rot[2];
			sph.size[0] = box->size[0];
			sph.size[1] = box->size[1];
			sph.size[2] = box->size[2];
			break;
		case pt_TRIMESH:
			trimesh = (TRIMESH*)sp->primitive;
			sph.pos[0] = trimesh->pos[0];
			sph.pos[1] = trimesh->pos[1];
			sph.pos[2] = trimesh->pos[2];
			sph.rot[0] = trimesh->rot[0];
			sph.rot[1] = trimesh->rot[1];
			sph.rot[2] = trimesh->rot[2];
			sph.size[0] = trimesh->scale;
			break;
	};
	fwrite(&sph, sizeof(SCENE_PRIMITIVE_HEADER), 1, f);
	//special case, must occur AFTER sph
	if(sp->primitive->primitive_type==pt_TRIMESH)
	{
		fwrite(trimesh->file, 1, 512, f);
		trimesh->Save_Link_Data(f);
	}
	return true;
}

bool Load_Scene_Primitive(FILE *f)
{
	SCENE_PRIMITIVE_HEADER sph;
	fread(&sph, sizeof(SCENE_PRIMITIVE_HEADER), 1, f);
	SCENE_PRIMITIVE *sp = new SCENE_PRIMITIVE;
	sp->inverted = sph.inverted;
	sp->layer_id = sph.layer_id;
	sp->primitive = 0;
	PLANE *plane;
	SPHERE *sphere;
	CYLINDER *cylinder;
	BOX *box;
	TRIMESH *trimesh;
	switch(sph.type)
	{
		case pt_PLANE:
			plane = new PLANE;;
			plane->plane_pos[0] = sph.pos[0];
			plane->plane_pos[1] = sph.pos[1];
			plane->plane_pos[2] = sph.pos[2];
			plane->plane_dir[0] = sph.rot[0];
			plane->plane_dir[1] = sph.rot[1];
			plane->plane_dir[2] = sph.rot[2];
			sp->primitive = plane;
			break;
		case pt_SPHERE:
			sphere = new SPHERE;
			sphere->pos[0] = sph.pos[0];
			sphere->pos[1] = sph.pos[1];
			sphere->pos[2] = sph.pos[2];
			sphere->size = sph.size[0];
			sp->primitive = sphere;
			break;
		case pt_CYLINDER:
			cylinder = new CYLINDER;
			sph.pos[0] = cylinder->pos[0];
			sph.pos[1] = cylinder->pos[1];
			sph.pos[2] = cylinder->pos[2];
			sph.rot[0] = cylinder->rot[0];
			sph.rot[1] = cylinder->rot[1];
			sph.rot[2] = cylinder->rot[2];
			cylinder->base = sph.size[0];
			cylinder->top = sph.size[1];
			cylinder->height = sph.size[2];
			sp->primitive = cylinder;
			break;
		case pt_BOX:
			box = new BOX;
			box->pos[0] = sph.pos[0];
			box->pos[1] = sph.pos[1];
			box->pos[2] = sph.pos[2];
			box->rot[0] = sph.rot[0];
			box->rot[1] = sph.rot[1];
			box->rot[2] = sph.rot[2];
			box->size[0] = sph.size[0];
			box->size[1] = sph.size[1];
			box->size[2] = sph.size[2];
			sp->primitive = box;
			break;
		case pt_TRIMESH:
			trimesh = new TRIMESH;
			trimesh->pos[0] = sph.pos[0];
			trimesh->pos[1] = sph.pos[1];
			trimesh->pos[2] = sph.pos[2];
			trimesh->rot[0] = sph.rot[0];
			trimesh->rot[1] = sph.rot[1];
			trimesh->rot[2] = sph.rot[2];
			trimesh->scale = sph.size[0];
			fread(trimesh->file, 1, 512, f);
			if(strcmp(trimesh->file, "NOFILE"))
			{
				trimesh->triangle_mesh = Find_OBJ(trimesh->file);
				if(!trimesh->triangle_mesh)
				{
					SkinMsgBox(0, "Error loading OBJ file!", trimesh->file, MB_OK);
					delete trimesh;
					return false;
				}
			}
			trimesh->Load_Link_Data(f);
			sp->primitive = trimesh;
			break;
	};
	scene_primitives.push_back(sp);
	return true;
}

bool Save_Scene_Primitives()
{
	char file[512];
	if(!Get_Geometry_Filename(file))
	{
		return false;
	}
	FILE *f = fopen(file, "wb");
	if(!f)
	{
		return false;
	}
	int n = scene_primitives.size();
	fwrite(&n, sizeof(int), 1, f);
	for(int i = 0;i<n;i++)
	{
		Save_Scene_Primitive(scene_primitives[i], f);
	}
	fclose(f);
	return true;
}

bool Load_Scene_Primitives(bool reproject)
{
	char file[512];
	if(!Get_Geometry_Filename(file))
	{
		return false;
	}
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		return false;
	}
	//first frame loaded (or single session)
	//will have already loaded and created some default primitives
	Free_Scene_Primitives();//wipe them out
	int n = 0;
	fread(&n, sizeof(int), 1, f);
	for(int i = 0;i<n;i++)
	{
		Load_Scene_Primitive(f);
	}
	fclose(f);
	//re-get all the primitives and reproject
	Refresh_All_Layer_Primitives(reproject);
	return true;
}


bool Import_Scene_Primitives(char *file)
{
	char gfile[512];
	if(!Get_Geometry_Filename(file, gfile))
	{
		return false;
	}
	FILE *f = fopen(gfile, "rb");
	if(!f)
	{
		return false;
	}
	//will have already loaded and created some default primitives
	Free_Scene_Primitives();//wipe them out
	int n = 0;
	fread(&n, sizeof(int), 1, f);
	for(int i = 0;i<n;i++)
	{
		Load_Scene_Primitive(f);
	}
	fclose(f);
	//re-get all the primitives and reproject
	Refresh_All_Layer_Primitives(false);
	return true;
}


void Update_GL_Light_Position(float scale)
{
	float lpos[] = {1.0f*scale,1.0f*scale,2.0f*scale,0};
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
}

bool Init_Geometry_System()
{
	quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);
	float fspec[] = {1,1,1,1};	
	float fshine[] = {60};
	float bspec[] = {1,1,1,1};	
	float bshine[] = {150};
	float lite[] = {1,1,1,1};
	float lpos[] = {1,1,2,0};
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_BACK, GL_SPECULAR, bspec);
	glMaterialfv(GL_BACK, GL_SHININESS, bshine);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fspec);
	glMaterialfv(GL_FRONT, GL_SHININESS, fshine);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, lite);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lite);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lite);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lite);
	glEnable(GL_LIGHT0);
	return true;
}

bool Free_Geometry_System()
{
	return true;
}


//in case they back out of changing from model because it has linked points
bool Force_TriMesh_Checkbox();

//sets the new primitive type outside of the layers knowledge
bool Set_Layer_Geometry_Type(int index, int type)
{
	float pos[3];
	float rot[3];
	Get_Layer_Primitive_Transform(index, pos, rot);
	int layer_id = Get_Layer_ID(index);
	SCENE_PRIMITIVE *sp = Get_Scene_Primitive(layer_id);
	if(!sp)
	{
		char msg[512];
		sprintf(msg, "ERROR setting primitive type %i for layer index %i, id %i", type, index, layer_id);
		SkinMsgBox(0, msg, 0, MB_OK);
		return false;
	}

	//watch out for meshes with linked points, the user might lose valuable work
	if(sp->primitive->primitive_type==pt_TRIMESH)
	{
		TRIMESH *tm = (TRIMESH*)sp->primitive;
		if(tm->model_feature_links.size()>0)
		{
			if(SkinMsgBox("Warning! The model used for this layer has been linked with 2D feature points, if you change the geometry type all links will be deleted. Are you sure you want to do this?", 0, MB_YESNO)==IDNO)
			{
				Force_TriMesh_Checkbox();
				return false;
			}
		}
	}

	if(sp->primitive->primitive_type==type)
	{
		//don't bother, it's the same type
		return true;
	}
	delete sp->primitive;
	float size[3] = {0,0,0};
	if(type==pt_PLANE)
	{
		sp->primitive = new PLANE;
	}
	if(type==pt_SPHERE)
	{
		SPHERE *sphere =  new SPHERE;
		sp->primitive = sphere;
		sphere->size = 0.053f;
	}
	if(type==pt_BOX)
	{
		BOX *box = new BOX;
		sp->primitive = box;
		box->size[0] = 0.013f;
		box->size[1] = 0.013f;
		box->size[2] = 0.013f;
		box->pos[0] = pos[0];
		box->pos[1] = pos[1];
		box->pos[2] = pos[2];
		box->rot[0] = rot[0];
		box->rot[1] = rot[1];
		box->rot[2] = rot[2];
		box->bounding_box.Set_Transform(box->size, box->pos, box->rot);
		box->Fill_Vertices();
	}
	if(type==pt_CYLINDER)
	{
		CYLINDER *cylinder = new CYLINDER;
		sp->primitive = cylinder;
		cylinder->base = cylinder->top = 0.015f;
		cylinder->height = 0.03f;
		size[0] = cylinder->base;
		size[1] = cylinder->height;
		size[2] = cylinder->top;
		cylinder->bounding_box.Set_Transform(size, cylinder->pos, cylinder->rot);
	}
	if(type==pt_TRIMESH)
	{
		sp->primitive = new TRIMESH;
	}
	return true;
}

bool Layer_Geometry_Model_Loaded(int index)
{
	int layer_id = Get_Layer_ID(index);
	SCENE_PRIMITIVE *sp = Get_Scene_Primitive(layer_id);
	if(sp->primitive->primitive_type!=pt_TRIMESH)
	{
		return false;
	}
	TRIMESH *tm = (TRIMESH*)sp->primitive;
	return tm->triangle_mesh!=0;
}

//sets the new primitive type outside of the layers knowledge
bool Set_Layer_Geometry_Model(int index, char *file)
{
	int layer_id = Get_Layer_ID(index);
	SCENE_PRIMITIVE *sp = Get_Scene_Primitive(layer_id);
	char msg[512];
	if(!sp)
	{
		sprintf(msg, "ERROR setting model geometry for layer index %i, id %i", index, layer_id);
		SkinMsgBox(0, msg, file, MB_OK);
		return false;
	}
	if(sp->primitive->primitive_type!=pt_TRIMESH)
	{
		if(sp->primitive->primitive_type==pt_BOX)sprintf(msg, "ERROR setting model geometry, the selected geometry type is a box");
		if(sp->primitive->primitive_type==pt_SPHERE)sprintf(msg, "ERROR setting model geometry, the selected geometry type is a sphere");
		if(sp->primitive->primitive_type==pt_CYLINDER)sprintf(msg, "ERROR setting model geometry, the selected geometry type is a cylinder");
		if(sp->primitive->primitive_type==pt_PLANE)sprintf(msg, "ERROR setting model geometry, the selected geometry type is a plane");
		SkinMsgBox(0, msg, 0, MB_OK);
		return false;
	}
	TRIMESH *tm = (TRIMESH*)sp->primitive;
	tm->triangle_mesh = Find_OBJ(file);
	if(!tm->triangle_mesh)
	{
		SkinMsgBox(0, "Error loading OBJ model!", 0, MB_OK);
		return false;
	}
	strcpy(tm->file, file);
	return true;
}


bool Invert_Layer_Geometry(int index, bool b)
{
	int layer_id = Get_Layer_ID(index);
	SCENE_PRIMITIVE *sp = Get_Scene_Primitive(layer_id);
	if(!sp)
	{
		char msg[512];
		sprintf(msg, "ERROR inverting geometry for layer index %i, id %i", index, layer_id);
		SkinMsgBox(0, msg, 0, MB_OK);
		return false;
	}
	sp->inverted = b;
	return true;
}

//set the global flag to show geometry if ANY are set to visible
bool Reset_Show_Geometry_Flag()
{
	show_geometry = false;
	int n = scene_primitives.size();
	for(int i = 0;i<n;i++)
	{
		if(scene_primitives[i]->visible)
		{
			show_geometry = true;
			return true;
		}
	}
	return true;
}

bool Show_Layer_Geometry(int index, bool b)
{
	int layer_id = Get_Layer_ID(index);
	SCENE_PRIMITIVE *sp = Get_Scene_Primitive(layer_id);
	if(!sp)
	{
		char msg[512];
		sprintf(msg, "ERROR setting primitive visibility for layer index %i, id %i", index, layer_id);
		SkinMsgBox(0, msg, 0, MB_OK);
		return false;
	}
	sp->visible = b;
	Reset_Show_Geometry_Flag();
	return true;
}

bool Open_Geometry_Tool()
{
	Open_Geometry_Dialog();
	return true;
}


bool Close_Geometry_Tool()
{
	Close_Geometry_Dialog();
	return true;
}

//used for link points tool
int Find_Scene_Model_Primitive_Index(int layer_id)
{
	int n = scene_primitives.size();
	for(int i = 0;i<n;i++)
	{
		if(scene_primitives[i]->layer_id==layer_id)
		{
			if(scene_primitives[i]->primitive->primitive_type==pt_TRIMESH)
			{
				return i;
			}
		}
	}
	return -1;
}

//used for link points tool
bool Get_Scene_Primitive_Model_Filename(int index, char *file)
{
	int n = scene_primitives.size();
	if(index<0||index>=n)
	{
		return false;
	}
	if(scene_primitives[index]->primitive->primitive_type!=pt_TRIMESH)
	{
		return false;
	}
	TRIMESH *tm = (TRIMESH*)scene_primitives[index]->primitive;
	if(!tm->triangle_mesh)
	{
		return false;
	}
	strcpy(file, tm->file);
	return true;
}

//used for link points tool
bool Get_Scene_Primitive_Model_Link_Info(int index, vector<int> *links)
{
	int n = scene_primitives.size();
	if(index<0||index>=n)
	{
		return false;
	}
	if(scene_primitives[index]->primitive->primitive_type!=pt_TRIMESH)
	{
		return false;
	}
	TRIMESH *tm = (TRIMESH*)scene_primitives[index]->primitive;
	tm->Get_Feature_Point_Links(links);
	return true;
}

//used for link points tool
bool Set_Scene_Primitive_Model_Link_Info(int index, vector<int> *links)
{
	int n = scene_primitives.size();
	if(index<0||index>=n)
	{
		return false;
	}
	if(scene_primitives[index]->primitive->primitive_type!=pt_TRIMESH)
	{
		return false;
	}
	TRIMESH *tm = (TRIMESH*)scene_primitives[index]->primitive;
	tm->Set_Feature_Point_Links(links);
	return true;
}

//used for link points tool
bool Get_Scene_Primitive_Model_Info(int index, float *size, float *pos, float *rot, float *scale)
{
	int n = scene_primitives.size();
	if(index<0||index>=n)
	{
		return false;
	}
	if(scene_primitives[index]->primitive->primitive_type!=pt_TRIMESH)
	{
		return false;
	}
	TRIMESH *tm = (TRIMESH*)scene_primitives[index]->primitive;
	pos[0] = tm->pos[0];
	pos[1] = tm->pos[1];
	pos[2] = tm->pos[2];
	rot[0] = tm->rot[0];
	rot[1] = tm->rot[1];
	rot[2] = tm->rot[2];
	*scale = tm->scale;
	if(tm->triangle_mesh)
	{
		size[0] = tm->triangle_mesh->aabb_hi[0]-tm->triangle_mesh->aabb_lo[0];
		size[1] = tm->triangle_mesh->aabb_hi[1]-tm->triangle_mesh->aabb_lo[1];
		size[2] = tm->triangle_mesh->aabb_hi[2]-tm->triangle_mesh->aabb_lo[2];
	}
	else
	{
		size[0] = 0;
		size[1] = 0;
		size[2] = 0;
	}
	return true;
}

//used for link points tool
bool Set_Scene_Primitive_Model_Info(int index, float *pos, float *rot, float scale)
{
	int n = scene_primitives.size();
	if(index<0||index>=n)
	{
		return false;
	}
	if(scene_primitives[index]->primitive->primitive_type!=pt_TRIMESH)
	{
		return false;
	}
	TRIMESH *tm = (TRIMESH*)scene_primitives[index]->primitive;
	tm->pos[0] = pos[0];
	tm->pos[1] = pos[1];
	tm->pos[2] = pos[2];
	tm->rot[0] = rot[0];
	tm->rot[1] = rot[1];
	tm->rot[2] = rot[2];
	tm->scale = scale;
	tm->Update_Matrix();

	float trot[3];
	//here is how the layer positions the model
	//trimesh->rot[1] = layer_primitive_rotation[0];
	//trimesh->rot[0] = -layer_primitive_rotation[1];
	//trimesh->rot[2] = layer_primitive_rotation[2];
	//so the rot passed to the layer should be:
	trot[0] = rot[1];
	trot[1] = -rot[0];
	trot[2] = rot[2];
	
	Set_Layer_Transform(scene_primitives[index]->layer_id, pos, trot);
	return true;
}

//used for link points tool
bool Render_Scene_Primitive_Model(int index)
{
	int n = scene_primitives.size();
	if(index<0||index>=n)
	{
		return false;
	}
	glEnable(GL_LIGHTING);
	scene_primitives[index]->primitive->Render();
	glDisable(GL_LIGHTING);
	return true;
}

bool Render_Geometry()
{
	glEnable(GL_LIGHTING);
	int n = scene_primitives.size();
	if(show_geometry_wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		if(!cull_backfaces)glEnable(GL_CULL_FACE);
	}
	for(int i = 0;i<n;i++)
	{
		if(scene_primitives[i]->visible)
		{
			if(!show_geometry_wireframe)
			{
				if(!scene_primitives[i]->inverted)
				{
					glCullFace(GL_BACK);
				}
				else
				{
					glCullFace(GL_FRONT);
				}
			}
			scene_primitives[i]->primitive->Render();
		}
	}
	glDisable(GL_LIGHTING);
	glCullFace(GL_FRONT);
	if(!cull_backfaces)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
	}
	if(show_geometry_wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	return true;
}

