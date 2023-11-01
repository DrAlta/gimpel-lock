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
#include "G3DCoreFiles/Frame.h"
#include <gl\gl.h>
#include "Skin.h"



void Set_Projection_Dlg_FOV(float fov);
bool Set_Frame_Transform(float *pos, float *rot);


bool render_background_meshes = false;


#define BACKGROUND_MESH_FILE_VERSION 42

struct BACKGROUND_MESH_HEADER
{
	int version;
	int width, height;
	float fov;
	float view_origin[3];
	float view_rotation[3];
};

class BACKGROUND_MESH
{
public:
	BACKGROUND_MESH()
	{
		vertices = 0;
		colors = 0;
		tri_indices = 0;
		num_tri_indices = 0;
	}
	~BACKGROUND_MESH()
	{
		if(vertices)delete[] vertices;
		if(colors)delete[] colors;
		if(tri_indices)delete[] tri_indices;
		vertices = 0;
		colors = 0;
		tri_indices = 0;
	}
	BACKGROUND_MESH_HEADER header;
	float *vertices;
	float *colors;
	unsigned int *tri_indices;
	unsigned int num_tri_indices;
};

vector<BACKGROUND_MESH*> background_meshes;

bool Export_Background_Mesh(char *file)
{
	BACKGROUND_MESH m;
	m.header.version = BACKGROUND_MESH_FILE_VERSION;
	m.header.width = frame->width;
	m.header.height = frame->height;
	m.header.fov = _fov;
	m.header.view_origin[0] = frame->view_origin[0];
	m.header.view_origin[1] = frame->view_origin[1];
	m.header.view_origin[2] = frame->view_origin[2];
	m.header.view_rotation[0] = frame->view_rotation[0];
	m.header.view_rotation[1] = frame->view_rotation[1];
	m.header.view_rotation[2] = frame->view_rotation[2];
	FILE *f = fopen(file, "wb");
	if(!f)
	{
		SkinMsgBox("Can't open file for writing!", file, MB_OK);
		return false;
	}
	fwrite(&m.header, 1, sizeof(BACKGROUND_MESH_HEADER), f);
	fwrite(frame->vertices, 1, sizeof(float)*frame->width*frame->height*3, f);
	fwrite(frame->rgb_colors, 1, sizeof(float)*frame->width*frame->height*3, f);
	fclose(f);
	return true;
}

bool Init_Mesh_Tri_Indices(BACKGROUND_MESH *m)
{
	m->num_tri_indices = 0;
	m->tri_indices = 0;
	int i, j;
	for(i = 0;i<m->header.height-1;i++)
	{
		m->num_tri_indices += ((m->header.width)*2)+4;
	}
	m->tri_indices = new unsigned int[m->num_tri_indices];
	unsigned int cnt = 0;
	for(i = 0;i<m->header.height-1;i++)
	{
		for(j = 0;j<m->header.width;j++)
		{
			m->tri_indices[cnt] = (i*m->header.width)+j;cnt++;
			m->tri_indices[cnt] = ((i+1)*m->header.width)+j;cnt++;
		}
		if(i!=m->header.height-2)
		{
			m->tri_indices[cnt] = m->tri_indices[cnt-1];cnt++;
			m->tri_indices[cnt] = m->tri_indices[cnt-1];cnt++;
			m->tri_indices[cnt] = ((i+1)*m->header.width);cnt++;
			m->tri_indices[cnt] = ((i+1)*m->header.width);cnt++;
		}
	}
	m->num_tri_indices = cnt;
	return true;
}


bool Import_Background_Mesh(char *file)
{
	BACKGROUND_MESH *m = new BACKGROUND_MESH;
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		SkinMsgBox("Can't open file for reading!", file, MB_OK);
		return false;
	}
	fread(&m->header, 1, sizeof(BACKGROUND_MESH_HEADER), f);
	if(m->header.version!=BACKGROUND_MESH_FILE_VERSION)
	{
		SkinMsgBox("Error loading background mesh, version mismatch!", file, MB_OK);
		fclose(f);
		delete m;
		return false;
	}
	m->vertices = new float[m->header.width*m->header.height*3];
	m->colors = new float[m->header.width*m->header.height*3];
	fread(m->vertices, 1, sizeof(float)*m->header.width*m->header.height*3, f);
	fread(m->colors, 1, sizeof(float)*m->header.width*m->header.height*3, f);
	fclose(f);
	Init_Mesh_Tri_Indices(m);
	background_meshes.push_back(m);
	render_background_meshes = true;
	redraw_frame = true;
	aperture = _fov = DEFAULT_FOV = m->header.fov;
	Get_Flat_Projection_Vectors();
	Set_Projection_Dlg_FOV(_fov);

	Set_Frame_Transform(m->header.view_origin, m->header.view_rotation);

	Project_Layers();

	return true;
}

bool Free_Background_Meshes()
{
	int n = background_meshes.size();
	for(int i = 0;i<n;i++)
	{
		delete background_meshes[i];
	}
	background_meshes.clear();
	render_background_meshes = true;
	return true;
}

bool Render_Background_Mesh(BACKGROUND_MESH *m)
{
	glVertexPointer(3, GL_FLOAT, 0, m->vertices);
	glColorPointer(3, GL_FLOAT, 0, m->colors);
	glDrawElements(GL_TRIANGLE_STRIP, m->num_tri_indices, GL_UNSIGNED_INT, m->tri_indices);
	return true;
}

bool Render_Background_Meshes()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	int n = background_meshes.size();
	for(int i = 0;i<n;i++)
	{
		Render_Background_Mesh(background_meshes[i]);
	}
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	return true;
}

