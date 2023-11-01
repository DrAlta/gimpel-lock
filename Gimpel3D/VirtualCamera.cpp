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
#include <GL/gl.h>
#include "G3DCoreFiles/Frame.h"
#include "GeometryTool/GLBasic.h"
#include "G3DMainFrame.h"
#include <GL/gl.h>
#include "Skin.h"
#include "G3DCoreFiles/bb_matrix.h"

bool Set_Frame_Transform(float *pos, float *rot);
bool Get_Frame_Transform(float *pos, float *rot);

bool Set_Generate_Camera_Path_Button_Text(char *text);

bool Get_Feature_Point_2D_Position(int index, float *pos);
bool Get_Feature_Point_2D_Position(int index, int frame, float *pos);
bool Get_Feature_Point_3D_Position(int index, float *pos);

void SetLength(float *v, float s);
void Get_Frame_View_Vector(int axis, float *res);

bool Get_VC_Tracking_Points(int frame_index);
bool Free_VC_Tracking_Points();

bool iterate_virtual_camera_alignment = false;
bool iterate_vc_full_path = false;
bool Render_VC_Tracking_Points();
int Find_Feature_Point_Index(int unique_id);
int Get_Selected_Feature_Point_ID();

bool iterate_virtual_camera_yaw = false;
bool iterate_virtual_camera_pitch = false;
bool iterate_virtual_camera_yaw_first = true;

bool track_vc_yaw = true;
bool track_vc_pitch = true;

int num_camera_smooth_steps = 5;


bool Stop_VC_Alignment();

bool Select_VC_Dialog_Frame(int frame);


void Normalize(float *v);
void Cross(float *v1, float *v2, float *v3);
float VecLength(float* v);
#define EPSILON 0.000001
float Dot(float *a, float *b);

bool Open_Virtual_Camera_Dlg();
bool Close_Virtual_Camera_Dlg();

bool render_camera = false;

bool virtual_camera_tool_open = false;


bool select_tracking_feature_points = false;
bool unselect_tracking_feature_points = false;

bool render_camera_vectors_to_points = true;
bool render_camera_path = true;
bool render_camera_direction = true;

int reference_camera_station = -1;
int selected_camera_station = -1;
int render_camera_station = -1;


class VC_STATION
{
public:
	VC_STATION()
	{
		pos[0] = 0;
		pos[1] = 0;
		pos[2] = 0;
		rot[0] = 0;
		rot[1] = 0;
		rot[2] = 0;
		dir[0] = 0;
		dir[1] = 0;
		dir[2] = 1;
	}
	~VC_STATION()
	{
	}
	void Update_Dir()
	{
		oBB_MATRIX mat;
		mat.Set(mat.m, pos, rot);
		dir[0] = mat.m[2][0];
		dir[1] = mat.m[2][1];
		dir[2] = mat.m[2][2];
	}
	float pos[3];
	float rot[3];
	float dir[3];
};

class VIRTUAL_CAMERA
{
public:
	VIRTUAL_CAMERA()
	{
	}
	~VIRTUAL_CAMERA()
	{
		int n = stations.size();
		for(int i = 0;i<n;i++)
		{
			delete stations[i];
		}
		stations.clear();
		tracking_feature_point_indices.clear();
		tracking_feature_point_ids.clear();
	}
	vector<VC_STATION*> stations;
	vector<int> tracking_feature_point_ids;
	vector<int> tracking_feature_point_indices;
};

int selected_vc_station = -1;

VIRTUAL_CAMERA *virtual_camera = 0;

VC_STATION* Get_Virtual_Camera_Position(int frame)
{
	if(!virtual_camera)return 0;
	int n = virtual_camera->stations.size();
	if(frame<0||frame>=n)return 0;
	return virtual_camera->stations[frame];
}

bool Free_Virtual_Camera()
{
	if(virtual_camera){delete virtual_camera;}
	virtual_camera = 0;
	reference_camera_station = -1;
	selected_camera_station = -1;
	render_camera_station = -1;
	return true;
}

bool Init_Virtual_Camera()
{
	Free_Virtual_Camera();
	int n = Get_Num_Frames();
	if(n==0)
	{
		return false;
	}
	virtual_camera = new VIRTUAL_CAMERA;
	for(int i = 0;i<n;i++)
	{
		VC_STATION *vcs = new VC_STATION;
		//use current frame transform
		vcs->pos[0] = frame->view_origin[0];
		vcs->pos[1] = frame->view_origin[1];
		vcs->pos[2] = frame->view_origin[2];
		vcs->rot[0] = frame->view_rotation[0];
		vcs->rot[1] = frame->view_rotation[1];
		vcs->rot[2] = frame->view_rotation[2];
		vcs->dir[0] = frame->view_direction[0];
		vcs->dir[1] = frame->view_direction[1];
		vcs->dir[2] = frame->view_direction[2];
		virtual_camera->stations.push_back(vcs);
	}
	return true;
}

int Find_VC_Tracking_Point(int feature_point_id)
{
	if(!virtual_camera)return -1;
	int n = virtual_camera->tracking_feature_point_ids.size();
	for(int i = 0;i<n;i++)
	{
		if(virtual_camera->tracking_feature_point_ids[i]==feature_point_id)
		{
			return i;
		}
	}
	return -1;
}

bool Select_Tracking_Feature_Point(int feature_point_id)
{
	if(!virtual_camera)return false;
	int index = Find_VC_Tracking_Point(feature_point_id);
	if(index==-1)
	{
		int fpi = Find_Feature_Point_Index(feature_point_id);
		if(fpi!=-1)
		{
			virtual_camera->tracking_feature_point_ids.push_back(feature_point_id);
			virtual_camera->tracking_feature_point_indices.push_back(fpi);
			return true;
		}
	}
	return false;//already selected
}

bool UnSelect_Tracking_Feature_Point(int feature_point_id)
{
	if(!virtual_camera)return false;
	int index = Find_VC_Tracking_Point(feature_point_id);
	if(index!=-1)
	{
		virtual_camera->tracking_feature_point_ids.erase(virtual_camera->tracking_feature_point_ids.begin()+index);
		virtual_camera->tracking_feature_point_indices.erase(virtual_camera->tracking_feature_point_indices.begin()+index);
		return true;
	}
	return false;//wasn't already selected
}

bool Get_Virtual_Camera_Filename(char *res)
{
	char pfile[512];
	Get_Project_Filename(pfile);
	char *c = strrchr(pfile, '.');
	if(!c)
	{
		return false;
	}
	strcpy(c, ".vci");
	strcpy(res, pfile);
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//load/save virtual camera data

//sets the VC at the current frame
bool Set_Virtual_Camera(float *pos, float *rot)
{
	if(!virtual_camera)
	{
		return false;
	}
	int n = virtual_camera->stations.size();
	int frame = Get_Current_Project_Frame();
	if(frame<0||frame>=n)
	{
		return false;
	}
	virtual_camera->stations[frame]->pos[0] = pos[0];
	virtual_camera->stations[frame]->pos[1] = pos[1];
	virtual_camera->stations[frame]->pos[2] = pos[2];
	virtual_camera->stations[frame]->rot[0] = rot[0];
	virtual_camera->stations[frame]->rot[1] = rot[1];
	virtual_camera->stations[frame]->rot[2] = rot[2];
	virtual_camera->stations[frame]->Update_Dir();
	return true;
}

bool Set_VC_Frame_Transform(int frame, float *pos, float *rot)
{
	int n = virtual_camera->stations.size();
	if(frame<0||frame>=n)
	{
		Set_Frame_Transform(pos, rot);
		return false;
	}
	virtual_camera->stations[frame]->pos[0] = pos[0];
	virtual_camera->stations[frame]->pos[1] = pos[1];
	virtual_camera->stations[frame]->pos[2] = pos[2];
	virtual_camera->stations[frame]->rot[0] = rot[0];
	virtual_camera->stations[frame]->rot[1] = rot[1];
	virtual_camera->stations[frame]->rot[2] = rot[2];
	virtual_camera->stations[frame]->Update_Dir();
	if(frame==Get_Current_Project_Frame())
	{
		Set_Frame_Transform(pos, rot);
	}
	return true;
}

bool Get_VC_Frame_Transform(int frame, float *pos, float *rot)
{
	int n = virtual_camera->stations.size();
	if(frame<0||frame>=n)
	{
		Get_Frame_Transform(pos, rot);
		return false;
	}
	pos[0] = virtual_camera->stations[frame]->pos[0];
	pos[1] = virtual_camera->stations[frame]->pos[1];
	pos[2] = virtual_camera->stations[frame]->pos[2];
	rot[0] = virtual_camera->stations[frame]->rot[0];
	rot[1] = virtual_camera->stations[frame]->rot[1];
	rot[2] = virtual_camera->stations[frame]->rot[2];
	if(frame==Get_Current_Project_Frame())
	{
		Get_Frame_Transform(pos, rot);
	}
	return true;
}

bool Set_Virtual_Camera_Frame_Transform(int frame)
{
	if(!virtual_camera)return false;
	int n = virtual_camera->stations.size();
	if(frame<0||frame>=n)
	{
		return false;
	}
	Set_Frame_Transform(virtual_camera->stations[frame]->pos, virtual_camera->stations[frame]->rot);
	return true;
}

const int virtual_camera_file_version = 4;

bool Save_Virtual_Camera_Info()
{
	if(!virtual_camera)
	{
		return false;//fresh import, camera not created
	}
	char file[512];
	if(!Get_Virtual_Camera_Filename(file))
	{
		return false;
	}
	FILE *f = fopen(file, "wb");
	if(!f)
	{
		SkinMsgBox("ERROR! Can't open file for writing, virtual camera info cannot be saved", file);
		return false;
	}
	int version = virtual_camera_file_version;
	int num_frames = Get_Num_Frames();
	//save file version
	fwrite(&version, sizeof(int), 1, f);
	//save num frames
	fwrite(&num_frames, sizeof(int), 1, f);
	//save positions, rotations, and directions for all frames
	float *buf = new float[num_frames*9];//9 floats per frame station
	int i;
	for(int i = 0;i<num_frames;i++)
	{
		buf[(i*9)] = virtual_camera->stations[i]->pos[0];
		buf[(i*9)+1] = virtual_camera->stations[i]->pos[1];
		buf[(i*9)+2] = virtual_camera->stations[i]->pos[2];
		buf[(i*9)+3] = virtual_camera->stations[i]->rot[0];
		buf[(i*9)+4] = virtual_camera->stations[i]->rot[1];
		buf[(i*9)+5] = virtual_camera->stations[i]->rot[2];
		buf[(i*9)+6] = virtual_camera->stations[i]->dir[0];
		buf[(i*9)+7] = virtual_camera->stations[i]->dir[1];
		buf[(i*9)+8] = virtual_camera->stations[i]->dir[2];
	}
	fwrite(buf, sizeof(float)*num_frames*9, 1, f);
	delete[] buf;

	//save link points
	int nlp = virtual_camera->tracking_feature_point_ids.size();
	int *ibuf = new int[nlp];
	for(i = 0;i<nlp;i++)
	{
		ibuf[i] = virtual_camera->tracking_feature_point_ids[i];
	}
	fwrite(&nlp, sizeof(int), 1, f);
	fwrite(ibuf, sizeof(int)*nlp, 1, f);
	delete[] ibuf;
	
	fclose(f);
	return true;
}

bool Load_Virtual_Camera_Info()
{
	char file[512];
	if(!Get_Virtual_Camera_Filename(file))
	{
		return false;
	}
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		Init_Virtual_Camera();//use default
		return false;
	}
	int version = 0;
	int num_frames = 0;
	//load/verify file version
	fread(&version, sizeof(int), 1, f);
	if(version!=virtual_camera_file_version)
	{
		SkinMsgBox("Can't load virtual camera info, file version mismatch!", "This never happens");
		fclose(f);
		Init_Virtual_Camera();//use default
		return false;
	}
	//load/verify num frames
	fread(&num_frames, sizeof(int), 1, f);
	if(num_frames!=Get_Num_Frames())
	{
		SkinMsgBox("Can't load virtual camera info, num frames mismatch!", "This never happens");
		fclose(f);
		Init_Virtual_Camera();//use default
		return false;
	}
	float *buf = new float[num_frames*9];//9 floats per frame station
	//load positions, rotations, and directions for all frames
	fread(buf, sizeof(float)*num_frames*9, 1, f);

	//load link points
	int nlp = 0;
	fread(&nlp, sizeof(int), 1, f);
	int *ibuf = new int[nlp];
	fread(ibuf, sizeof(int)*nlp, 1, f);

	fclose(f);
	Free_Virtual_Camera();
	virtual_camera = new VIRTUAL_CAMERA;
	//allocate camera stations
	int i;
	for(i = 0;i<num_frames;i++)
	{
		VC_STATION *vcs = new VC_STATION;
		vcs->pos[0] = buf[(i*9)];
		vcs->pos[1] = buf[(i*9)+1];
		vcs->pos[2] = buf[(i*9)+2];
		vcs->rot[0] = buf[(i*9)+3];
		vcs->rot[1] = buf[(i*9)+4];
		vcs->rot[2] = buf[(i*9)+5];
		vcs->dir[0] = buf[(i*9)+6];
		vcs->dir[1] = buf[(i*9)+7];
		vcs->dir[2] = buf[(i*9)+8];
		virtual_camera->stations.push_back(vcs);
	}
	for(i = 0;i<nlp;i++)
	{
		virtual_camera->tracking_feature_point_ids.push_back(ibuf[i]);
		//whoops, there won't be any feature points, better update this when the tool opens up..
		virtual_camera->tracking_feature_point_indices.push_back(Find_Feature_Point_Index(ibuf[i]));
	}
	delete[] ibuf;
	delete[] buf;
	return true;
}

bool Update_Virtual_Camera_Tracked_Feature_Point_Indices()
{
	if(!virtual_camera)return false;
	int n = virtual_camera->tracking_feature_point_ids.size();
	for(int i = 0;i<n;i++)
	{
		virtual_camera->tracking_feature_point_indices[i] = Find_Feature_Point_Index(virtual_camera->tracking_feature_point_ids[i]);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Set_Current_Frame_As_Reference()
{
	reference_camera_station = Get_Current_Project_Frame();
	VC_STATION *s = Get_Virtual_Camera_Position(reference_camera_station);
	if(s)
	{
		s->pos[0] = frame->view_origin[0];
		s->pos[1] = frame->view_origin[1];
		s->pos[2] = frame->view_origin[2];
		s->rot[0] = frame->view_rotation[0];
		s->rot[1] = frame->view_rotation[1];
		s->rot[2] = frame->view_rotation[2];
		s->Update_Dir();
		return true;
	}
	return false;
}

bool Select_Camera_Path_Frame(int index)
{
	selected_camera_station = index;
	render_camera_station = index;
	Get_VC_Tracking_Points(index);
	return false;
}

bool Clear_Tracked_Feature_Points()
{
	return false;
}

bool Clear_Virtual_Camera_Info()
{
	return false;
}

bool Init_Virtual_Camera_Info()
{
	Init_Virtual_Camera();
	return true;
}

bool Reset_Virtual_Camera_Info()
{
	Clear_Virtual_Camera_Info();
	Init_Virtual_Camera_Info();
	Free_VC_Tracking_Points();
	return true;
}

bool Create_Default_Virtual_Camera_Path()
{
	Clear_Virtual_Camera_Info();
	Init_Virtual_Camera_Info();
	Free_VC_Tracking_Points();
	return true;
}

bool Open_Virtual_Camera_Tool()
{
	if(virtual_camera_tool_open)return false;
	mainframe->SetFocus();
	mainframe->OnFeaturePointsTool();
	Open_Virtual_Camera_Dlg();
	virtual_camera_tool_open = true;
	if(!virtual_camera)
	{
		Init_Virtual_Camera_Info();
	}
	Update_Virtual_Camera_Tracked_Feature_Point_Indices();
	return true;
}

bool Close_Virtual_Camera_Tool()
{
	if(!virtual_camera_tool_open)return false;
	Close_Virtual_Camera_Dlg();
	virtual_camera_tool_open = false;
	if(iterate_virtual_camera_alignment)Stop_VC_Alignment();
	Free_VC_Tracking_Points();
	redraw_frame = true;
	return true;
}

float focal_plane_p1[3] = {0,0,0};
float focal_plane_p2[3] = {0,0,0};
float focal_plane_p3[3] = {0,0,0};
float focal_plane_p4[3] = {0,0,0};

bool ReCalc_Focal_Plane_Preview()
{
	if(!frame){return false;}
	float *p1 = frame->Get_Frame_Vector(0, 0);
	float *p2 = frame->Get_Frame_Vector(frame->width-1, 0);
	float *p3 = frame->Get_Frame_Vector(frame->width-1, frame->height-1);
	float *p4 = frame->Get_Frame_Vector(0, frame->height-1);
	float fp1[3] = {p1[0],p1[1],p1[2]};
	float fp2[3] = {p2[0],p2[1],p2[2]};
	float fp3[3] = {p3[0],p3[1],p3[2]};
	float fp4[3] = {p4[0],p4[1],p4[2]};
	SetLength(fp1, (focal_length*depth_scale)*2);
	SetLength(fp2, (focal_length*depth_scale)*2);
	SetLength(fp3, (focal_length*depth_scale)*2);
	SetLength(fp4, (focal_length*depth_scale)*2);

	fp1[0] += frame->view_origin[0];fp1[1] += frame->view_origin[1];fp1[2] += frame->view_origin[2];
	fp2[0] += frame->view_origin[0];fp2[1] += frame->view_origin[1];fp2[2] += frame->view_origin[2];
	fp3[0] += frame->view_origin[0];fp3[1] += frame->view_origin[1];fp3[2] += frame->view_origin[2];
	fp4[0] += frame->view_origin[0];fp4[1] += frame->view_origin[1];fp4[2] += frame->view_origin[2];

	float point[3];
	point[0] = frame->view_origin[0] + (frame->view_direction[0]*(focal_length*depth_scale));
	point[1] = frame->view_origin[1] + (frame->view_direction[1]*(focal_length*depth_scale));
	point[2] = frame->view_origin[2] + (frame->view_direction[2]*(focal_length*depth_scale));
	MATH_UTILS::Plane_Intersection(frame->view_origin, fp1, focal_plane_p1, point, frame->view_direction);//normal);
	MATH_UTILS::Plane_Intersection(frame->view_origin, fp2, focal_plane_p2, point, frame->view_direction);//normal);
	MATH_UTILS::Plane_Intersection(frame->view_origin, fp3, focal_plane_p3, point, frame->view_direction);//normal);
	MATH_UTILS::Plane_Intersection(frame->view_origin, fp4, focal_plane_p4, point, frame->view_direction);//normal);
	return true;
}


bool Render_Focal_Plane()
{
	glEnable(GL_BLEND);
	glColor4f(0.5f, 0.5f, 1, 0.25);
	glBegin(GL_QUADS);
	glVertex3fv(focal_plane_p1);
	glVertex3fv(focal_plane_p2);
	glVertex3fv(focal_plane_p3);
	glVertex3fv(focal_plane_p4);
	glEnd();
	glColor4f(0.8f, 0.8f, 1, 0.25);
	glBegin(GL_LINE_STRIP);
	glVertex3fv(focal_plane_p1);
	glVertex3fv(focal_plane_p2);
	glVertex3fv(focal_plane_p3);
	glVertex3fv(focal_plane_p4);
	glVertex3fv(focal_plane_p1);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3fv(frame->view_origin);glVertex3fv(focal_plane_p1);
	glVertex3fv(frame->view_origin);glVertex3fv(focal_plane_p2);
	glVertex3fv(frame->view_origin);glVertex3fv(focal_plane_p3);
	glVertex3fv(frame->view_origin);glVertex3fv(focal_plane_p4);
	glEnd();

	glDisable(GL_BLEND);
	glColor4f(0,1,1,1);
	float fpos[3] = {frame->view_origin[0]+(frame->view_direction[0]*(focal_length*depth_scale)), frame->view_origin[1]+(frame->view_direction[1]*(focal_length*depth_scale)), frame->view_origin[2]+(frame->view_direction[2]*(focal_length*depth_scale))};
	float side[3];
	Get_Frame_View_Vector(0, side);
	float hes = (eye_separation*depth_scale)/2;
	side[0] *= hes;
	side[1] *= hes;
	side[2] *= hes;
	glBegin(GL_LINES);
	glVertex3fv(fpos);
	glVertex3f(frame->view_origin[0]-side[0], frame->view_origin[1]-side[1], frame->view_origin[2]-side[2]);
	glVertex3fv(fpos);
	glVertex3f(frame->view_origin[0]+side[0], frame->view_origin[1]+side[1], frame->view_origin[2]+side[2]);
	glEnd();
	glColor3f(1,1,1);
	return true;
}

extern GLUquadric *quadric;

bool Render_Camera()
{
	glPushMatrix();
	glTranslatef(frame->view_origin[0], frame->view_origin[1], frame->view_origin[2]);
	float diffuse[4] = {1,0,0,0};
	float specular[4] = {1,1,1,0};
	float ambient[4] = {0.2f,0,0,0};
	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	gluSphere(quadric, (eye_separation*depth_scale)/2, 16, 16);
	glDisable(GL_LIGHTING);
	glPopMatrix();
	Render_Focal_Plane();
	return true;
}

//render a line from each tracking point to the current camera position
//and selected station
bool Render_Camera_Vectors_To_Points()
{
	int n = virtual_camera->tracking_feature_point_indices.size();
	float pos[3];
	glColor3f(1,1,0);
	glBegin(GL_LINES);
	for(int i = 0;i<n;i++)
	{
		if(Get_Feature_Point_3D_Position(virtual_camera->tracking_feature_point_indices[i], pos))
		{
			glVertex3fv(pos);
			glVertex3fv(frame->view_origin);
		}
	}
	glEnd();
	if(render_camera_direction)
	{
		glColor3f(1,0,0);
		glBegin(GL_LINES);
		for(int i = 0;i<n;i++)
		{
			VC_STATION *vcs = Get_Virtual_Camera_Position(render_camera_station);
			if(vcs)
			{
				glVertex3fv(vcs->pos);
				glVertex3f(vcs->pos[0]+vcs->dir[0], vcs->pos[1]+vcs->dir[1], vcs->pos[2]+vcs->dir[2]);
			}
		}
		glEnd();
	}

	glColor3f(0,1,0);

	if(render_camera_station!=-1)
	{
		VC_STATION *vcs = Get_Virtual_Camera_Position(render_camera_station);
		if(vcs)
		{
			glBegin(GL_LINES);
			for(int i = 0;i<n;i++)
			{
				if(Get_Feature_Point_3D_Position(virtual_camera->tracking_feature_point_indices[i], pos))
				{
					glVertex3fv(pos);
					glVertex3fv(vcs->pos);
				}
			}
			glEnd();
		}
	}
	glColor3f(1,1,1);
	return true;
}

__forceinline void Render_Camera_Station_Sphere(float *pos)
{
	glPushMatrix();
	glTranslatef(pos[0], pos[1], pos[2]);
	gluSphere(quadric, (eye_separation*depth_scale)/2, 16, 16);
	glPopMatrix();
}

//render a series of connected station points
bool Render_Camera_Path()
{
	glColor3f(1,0,0);
	int n = virtual_camera->stations.size();
	glLineWidth(5);
	glBegin(GL_LINE_STRIP);
	int i;
	for(i = 0;i<n;i++)
	{
		glVertex3fv(virtual_camera->stations[i]->pos);
	}
	glEnd();
	glLineWidth(1);
	glColor3f(1,1,1);

	float diffuse[4] = {1,0,1,0};
	float specular[4] = {1,1,1,0};
	float ambient[4] = {0.2f,0,0.2f,0};
	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);

	float alt_diffuse[4] = {0,1,0,0};
	float alt_specular[4] = {1,1,1,0};
	float alt_ambient[4] = {0,0.2f,0,0};
	
	for(i = 0;i<n;i++)
	{
		if(i==render_camera_station)
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, alt_diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, alt_specular);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, alt_ambient);
			Render_Camera_Station_Sphere(virtual_camera->stations[i]->pos);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		}
		else
		{
			Render_Camera_Station_Sphere(virtual_camera->stations[i]->pos);
		}
	}
	glDisable(GL_LIGHTING);
	return true;
}


bool Render_Virtual_Camera_Tool()
{
	if(!virtual_camera){return false;}
	if(render_camera_vectors_to_points)
	{
		Render_Camera_Vectors_To_Points();
	}
	if(render_camera_path)
	{
		Render_Camera_Path();
	}
	return true;
}

bool Notify_Virtual_Camera_Tool_Changed_Feature_Point()
{
	int fpid = Get_Selected_Feature_Point_ID();
	if(fpid==-1){return false;}
	if(select_tracking_feature_points)
	{
		if(Select_Tracking_Feature_Point(fpid))
		{
			redraw_frame = true;
		}
	}
	else if(unselect_tracking_feature_points)
	{
		if(UnSelect_Tracking_Feature_Point(fpid))
		{
			redraw_frame = true;
		}
	}
	return true;
}


float Distance_Between_Reversed_View_Vectors(float *start1, float *dir1, float *start2, float *dir2)
{
	float rc[3];
	rc[0] = start1[0]-start2[0];
	rc[1] = start1[1]-start2[1];
	rc[2] = start1[2]-start2[2];
	float n[3];
	Cross(dir1, dir2, n);
	float ln = VecLength(n);
	if((ln<EPSILON)&&(ln>-EPSILON))
	{
		return 0;//lines close to parallel
	}
	Normalize(n);
	float d = fabs(Dot(rc, n));
	return d;
}

class VC_TRACKING_POINT
{
public:
	VC_TRACKING_POINT()
	{
		pos3d[0] = 0;
		pos3d[1] = 0;
		pos3d[2] = 0;
		identity_vector[0] = 0;
		identity_vector[1] = 0;
		identity_vector[2] = 0;
		transformed_vector[0] = 0;
		transformed_vector[1] = 0;
		transformed_vector[2] = 0;
	}
	~VC_TRACKING_POINT()
	{
	}
	float pos3d[3];
	float identity_vector[3];
	float transformed_vector[3];
};

class VC_CONVERGENCE_POINT
{
public:
	VC_CONVERGENCE_POINT()
	{
	}
	~VC_CONVERGENCE_POINT()
	{
	}
	float pos[3];
};

vector<VC_TRACKING_POINT*> vc_tracking_points;
vector<VC_CONVERGENCE_POINT> vc_convergence_points;
float vc_alignment_rot[3] = {0,0,0};
float vc_alignment_pos[3] = {0,0,0};
float starting_vc_alignment_inc = 0.125f;
float vc_alignment_inc = starting_vc_alignment_inc;

bool Add_VC_Tracking_Point(int fpi, int frame_index)
{
	float current_p2d[3];
	float other_p2d[3];
	if(!Get_Feature_Point_2D_Position(fpi, frame_index, other_p2d))return false;
	if(!Get_Feature_Point_2D_Position(fpi, current_p2d))return false;
	int current_px = (int)current_p2d[0];
	int current_py = (int)current_p2d[1];
	int other_px = (int)other_p2d[0];
	int other_py = (int)other_p2d[1];
	if(!frame->IsValidPixel(current_px, current_py))return false;
	if(!frame->IsValidPixel(other_px, other_py))return false;
	VC_TRACKING_POINT *vctp = new VC_TRACKING_POINT;
	float *pos = frame->Get_Pos(current_px, current_py);//position in the current reference frame
	float *iv = frame->Get_Identity_Vector(other_px, other_py);//view vector from the other frame
	vctp->pos3d[0] = pos[0];
	vctp->pos3d[1] = pos[1];
	vctp->pos3d[2] = pos[2];
	vctp->identity_vector[0] = iv[0];
	vctp->identity_vector[1] = iv[1];
	vctp->identity_vector[2] = iv[2];
	Normalize(vctp->identity_vector);
	vctp->transformed_vector[0] = vctp->identity_vector[0];
	vctp->transformed_vector[1] = vctp->identity_vector[1];
	vctp->transformed_vector[2] = vctp->identity_vector[2];
	vc_tracking_points.push_back(vctp);
	return true;
}

bool Free_VC_Tracking_Points()
{
	int n = vc_tracking_points.size();
	for(int i = 0;i<n;i++)
	{
		delete vc_tracking_points[i];
	}
	vc_tracking_points.clear();
	return true;
}

bool Get_VC_Tracking_Points(int frame_index)
{
	Free_VC_Tracking_Points();
	int n = virtual_camera->tracking_feature_point_indices.size();
	for(int i = 0;i<n;i++)
	{
		Add_VC_Tracking_Point(virtual_camera->tracking_feature_point_indices[i], frame_index);
	}
	return true;
}

bool Transform_VC_Tracking_Points(float *rot)
{
	oBB_MATRIX frame_transform;
	float id[3] = {0,0,0};
	frame_transform.Set(frame_transform.m, id, rot);
	int n = vc_tracking_points.size();
	VC_TRACKING_POINT *vp;
	for(int i = 0;i<n;i++)
	{
		vp = vc_tracking_points[i];
		vp->transformed_vector[0] = vp->identity_vector[0];
		vp->transformed_vector[1] = vp->identity_vector[1];
		vp->transformed_vector[2] = vp->identity_vector[2];
		frame_transform.Transform_Vertex(frame_transform.m, vp->transformed_vector);
	}
	return true;
}

bool Render_VC_Tracking_Points()
{
	int n = vc_tracking_points.size();
	glColor3f(1,0,0);
	float s = -10;
	VC_TRACKING_POINT *vp;
	glBegin(GL_LINES);
	for(int i = 0;i<n;i++)
	{
		vp = vc_tracking_points[i];
		glVertex3fv(vp->pos3d);
		glVertex3f(vp->pos3d[0]+(vp->transformed_vector[0]*s),vp->pos3d[1]+(vp->transformed_vector[1]*s), vp->pos3d[2]+(vp->transformed_vector[2]*s));
	}
	glEnd();
	glColor3f(1,1,1);

	glPushMatrix();
	glTranslatef(vc_alignment_pos[0], vc_alignment_pos[1], vc_alignment_pos[2]);
	float diffuse[4] = {0,1,0,0};
	float specular[4] = {1,1,1,0};
	float ambient[4] = {0,0.2f,0,0};
	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	gluSphere(quadric, (eye_separation*depth_scale)/2, 16, 16);
	glDisable(GL_LIGHTING);
	glPopMatrix();

	return true;
}

//check tracking point against all others
float VC_Tracking_Point_Error(int index)
{
	int n = vc_tracking_points.size();
	float res = 0;
	VC_TRACKING_POINT *vp = vc_tracking_points[index];
	float *start1 = vp->pos3d;
	float *start2;
	float dir1[3] = {-vp->transformed_vector[0], -vp->transformed_vector[1], -vp->transformed_vector[2]};
	float dir2[3];
	for(int i = 0;i<n;i++)
	{
		if(i!=index)
		{	
			dir2[0] = -vc_tracking_points[i]->transformed_vector[0];
			dir2[1] = -vc_tracking_points[i]->transformed_vector[1];
			dir2[2] = -vc_tracking_points[i]->transformed_vector[2];
			start2 = vc_tracking_points[i]->pos3d;
			res += Distance_Between_Reversed_View_Vectors(start1, dir1, start2, dir2);
		}
	}
	return (res+1)*(res+1);
}

float VC_Tracking_Point_Error(int index, float *pos)
{
	VC_TRACKING_POINT *vp = vc_tracking_points[index];
	float *s1 = vp->pos3d;
	float dir1[3] = {-vp->transformed_vector[0], -vp->transformed_vector[1], -vp->transformed_vector[2]};
	float dir2[3] = {pos[0]-s1[0],pos[1]-s1[1],pos[2]-s1[2]};
	Normalize(dir1);
	Normalize(dir2);
	float d = Dot(dir1, dir2);
	return 1.0f-d;
}

bool Get_Average_VCT_Convergence_Point(float *pos);

__forceinline float Virtual_Camera_Alignment_Error()
{
	int n = vc_tracking_points.size();
	float res = 0;
	float avg_pos[3];
	Get_Average_VCT_Convergence_Point(avg_pos);
	for(int i = 0;i<n;i++)
	{
		res += VC_Tracking_Point_Error(i);
	}
	return res;
}

bool Get_Best_Starting_Camera_Alignment(float *rot)
{
	float deg_inc = starting_vc_alignment_inc;
	float npp = 360.0f/deg_inc;
	int np = (int)npp;
	int total = np*np;
	int i, j;
	float lowest = 99999;
	float best_rot[3] = {0,0,0};
	float error = 0;
	int cnt = 0;
	//start at reference
	VC_STATION *ref_vcs = Get_Virtual_Camera_Position(reference_camera_station);
	if(ref_vcs)
	{
		rot[0] = ref_vcs->rot[0];
		rot[1] = ref_vcs->rot[1];
		rot[2] = ref_vcs->rot[2];
	}
	else
	{
		rot[0] = 0;
		rot[1] = 0;
		rot[2] = 0;
	}
	int ni = np;
	int nj = np;
	Print_Status("Finding best start alignment..");
	if(track_vc_yaw)
	{
		for(i = 0;i<np;i++)
		{
			rot[1] = deg_inc*i;
			Transform_VC_Tracking_Points(rot);
			cnt++;
			error = Virtual_Camera_Alignment_Error();
			if(error<lowest)
			{
				lowest = error;
				best_rot[0] = rot[0];
				best_rot[1] = rot[1];
				best_rot[2] = rot[2];
			}
		}
	}
	if(track_vc_pitch)
	{
		for(j = 0;j<np;j++)
		{
			rot[0] = deg_inc*j;
			Transform_VC_Tracking_Points(rot);
			cnt++;
			error = Virtual_Camera_Alignment_Error();
			if(error<lowest)
			{
				lowest = error;
				best_rot[0] = rot[0];
				best_rot[1] = rot[1];
				best_rot[2] = rot[2];
			}
		}
	}
	Print_Status("Done.");
	rot[0] = best_rot[0];
	rot[1] = best_rot[1];
	rot[2] = best_rot[2];
	return true;
}

__forceinline void VC_Tracking_Points_Convergence(int index1, int index2, float *res)
{
	VC_TRACKING_POINT *vp1 = vc_tracking_points[index1];
	VC_TRACKING_POINT *vp2 = vc_tracking_points[index2];
	float side[3];
	float normal[3];
	Cross(vp1->transformed_vector, vp2->transformed_vector, side);
	Cross(vp1->transformed_vector, side, normal);
	float epos[3];
	epos[0] = vp2->pos3d[0] - vp2->transformed_vector[0]*10;
	epos[1] = vp2->pos3d[1] - vp2->transformed_vector[1]*10;
	epos[2] = vp2->pos3d[2] - vp2->transformed_vector[2]*10;
	glApp.math.Plane_Intersection(vp2->pos3d, epos, res, vp1->pos3d, normal);
}

//check tracking point against all others
__forceinline void Get_VC_Convergence_Points(int index)
{
	int n = vc_tracking_points.size();
	VC_CONVERGENCE_POINT vcp;
	for(int i = 0;i<n;i++)
	{
		if(i!=index)
		{
			vc_convergence_points.push_back(vcp);
			VC_CONVERGENCE_POINT *vcpp = &vc_convergence_points[vc_convergence_points.size()-1];
			VC_Tracking_Points_Convergence(index, i, vcpp->pos);
		}
	}
}

/*
__forceinline float Get_VC_Convergence_Point_Dissimilarity(int n, int index)
{
	float res = 0;
	float *cpos = vc_convergence_points[index].pos;
	for(int i = 0;i<n;i++)
	{
		if(i!=index)
		{
			res += glApp.math.Distance3D(vc_convergence_points[i].pos, cpos);
		}
	}
	return res;
}
*/

bool Get_Average_VCT_Convergence_Point(float *pos)
{
	vc_convergence_points.clear();
	int n = vc_tracking_points.size();
	float res = 0;
	int i;
	for(i = 0;i<n;i++)
	{
		Get_VC_Convergence_Points(i);
	}
	int ncp = vc_convergence_points.size();
	pos[0] = 0;
	pos[1] = 0;
	pos[2] = 0;
	for(i = 0;i<ncp;i++)
	{
		pos[0] += vc_convergence_points[i].pos[0];
		pos[1] += vc_convergence_points[i].pos[1];
		pos[2] += vc_convergence_points[i].pos[2];
	}
	pos[0] = pos[0]/ncp;
	pos[1] = pos[1]/ncp;
	pos[2] = pos[2]/ncp;
	vc_convergence_points.clear();
	return true;
}

/*
float Virtual_Camera_Alignment_Error()
{
	Get_Average_Convergence_Point(float *pos);
	vc_convergence_points.clear();
	int n = vc_tracking_points.size();
	float res = 0;
	for(int i = 0;i<n;i++)
	{
		Get_VC_Convergence_Points(i);
	}
	int ncp = vc_convergence_points.size();
	vc_alignment_pos[0] = 0;
	vc_alignment_pos[1] = 0;
	vc_alignment_pos[2] = 0;
	for(i = 0;i<ncp;i++)
	{
		res += Get_VC_Convergence_Point_Dissimilarity(ncp, i);
		vc_alignment_pos[0] += vc_convergence_points[i].pos[0];
		vc_alignment_pos[1] += vc_convergence_points[i].pos[1];
		vc_alignment_pos[2] += vc_convergence_points[i].pos[2];
	}
	vc_alignment_pos[0] = vc_alignment_pos[0]/ncp;
	vc_alignment_pos[1] = vc_alignment_pos[1]/ncp;
	vc_alignment_pos[2] = vc_alignment_pos[2]/ncp;
	vc_convergence_points.clear();
	return res;
}
*/

/*
bool Iterate_Virtual_Camera_Alignment(float *rot, int index, float inc)
{
	float old_error = Virtual_Camera_Alignment_Error();
	rot[index]+=inc;Transform_VC_Tracking_Points(rot);
	float new_error = Virtual_Camera_Alignment_Error();
	if(new_error>old_error)
	{
		rot[index]-=inc;Transform_VC_Tracking_Points(rot);
		return false;
	}
	return true;
}
*/

bool Iterate_Virtual_Camera_Alignment(float *rot, int index, float inc)
{
	float old_error = Virtual_Camera_Alignment_Error();
	rot[index]+=inc;Transform_VC_Tracking_Points(rot);
	float new_error = Virtual_Camera_Alignment_Error();
	if(new_error>=old_error)
	{
		rot[index]-=inc;Transform_VC_Tracking_Points(rot);
		return false;
	}
	return true;
}

bool Stop_VC_Alignment()
{
	Print_Status("Done.");
	iterate_virtual_camera_alignment = false;
	VC_STATION *sel_vcs = Get_Virtual_Camera_Position(selected_camera_station);
	if(sel_vcs)
	{
		float pos[3];
		Get_Average_VCT_Convergence_Point(pos);
		sel_vcs->rot[0] = vc_alignment_rot[0];
		sel_vcs->rot[1] = vc_alignment_rot[1];
		sel_vcs->rot[2] = vc_alignment_rot[2];
		sel_vcs->pos[0] = pos[0];
		sel_vcs->pos[1] = pos[1];
		sel_vcs->pos[2] = pos[2];
		sel_vcs->Update_Dir();
	}
	render_camera_station = selected_camera_station;
	return true;
}

bool Start_VC_Alignment(int frame_index)
{
	Print_Status("Starting virtual camera alignment..");
	Get_VC_Tracking_Points(frame_index);
	iterate_virtual_camera_alignment = true;
	iterate_virtual_camera_yaw = true;
	iterate_virtual_camera_pitch = false;
	Get_Best_Starting_Camera_Alignment(vc_alignment_rot);
	return true;
}

bool Generate_Relative_Camera_Position()
{
	VC_STATION *ref_vcs = Get_Virtual_Camera_Position(reference_camera_station);
	if(!ref_vcs)
	{
		Set_Current_Frame_As_Reference();
		ref_vcs = Get_Virtual_Camera_Position(reference_camera_station);
		if(!ref_vcs)
		{
			SkinMsgBox("INTERNALE ERROR can't create reference frame, current frame is invalid.", "This never happens.");
			return false;
		}
	}
	VC_STATION *sel_vcs = Get_Virtual_Camera_Position(selected_camera_station);
	if(!sel_vcs)
	{
		SkinMsgBox("Can't generate relative camera position, no frame is selected.");
		return false;
	}
	vc_alignment_rot[0] = ref_vcs->rot[0];//should never change!
	vc_alignment_rot[1] = ref_vcs->rot[1];
	vc_alignment_rot[2] = ref_vcs->rot[2];
	vc_alignment_inc = starting_vc_alignment_inc;
	Start_VC_Alignment(selected_camera_station);
	Transform_VC_Tracking_Points(vc_alignment_rot);
	return true;
}

bool Generate_Next_Relative_Camera_Position()
{
	vc_alignment_inc = starting_vc_alignment_inc;
	Start_VC_Alignment(selected_camera_station);
	Transform_VC_Tracking_Points(vc_alignment_rot);
	return true;
}

bool Generate_Relative_Camera_Path()
{
	if(reference_camera_station==-1)
	{
		Set_Current_Frame_As_Reference();
	}
	if(reference_camera_station==0)
	{
		Select_Camera_Path_Frame(1);
	}
	else
	{
		Select_Camera_Path_Frame(0);
	}
	iterate_vc_full_path = true;
	Select_VC_Dialog_Frame(selected_camera_station);
	Generate_Relative_Camera_Position();
	Set_Generate_Camera_Path_Button_Text("Cancel");
	return true;
}

bool Stop_Generate_Relative_Camera_Path()
{
	iterate_vc_full_path = false;
	Set_Generate_Camera_Path_Button_Text("Generate Relative Camera Path");
	return true;
}

bool Toggle_Generate_Relative_Camera_Path()
{
	if(!iterate_vc_full_path)
	{
		Generate_Relative_Camera_Path();
	}
	else
	{
		Stop_Generate_Relative_Camera_Path();
	}
	return true;
}

bool Update_Iterative_Virtual_Camera_Alignment()
{
	float old_error = Virtual_Camera_Alignment_Error();
	Print_Status("Converging to %f %f %f", vc_alignment_rot[0], vc_alignment_rot[1], vc_alignment_rot[2]);

	if(iterate_virtual_camera_yaw)
	{
		if(track_vc_yaw)//track it
		{
			bool yp = Iterate_Virtual_Camera_Alignment(vc_alignment_rot, 1, -vc_alignment_inc);
			bool yn = Iterate_Virtual_Camera_Alignment(vc_alignment_rot, 1, vc_alignment_inc);
			if(!yp&&!yn)
			{
				vc_alignment_inc = vc_alignment_inc/2;
			}
		}
		else
		{
			vc_alignment_inc = 0;//duck out early
		}
	}
	else if(iterate_virtual_camera_pitch)
	{
		if(track_vc_pitch)//track it
		{
			bool xp = Iterate_Virtual_Camera_Alignment(vc_alignment_rot, 0, -vc_alignment_inc);
			bool xn = Iterate_Virtual_Camera_Alignment(vc_alignment_rot, 0, vc_alignment_inc);
			if(!xp&&!xn)
			{
				vc_alignment_inc = vc_alignment_inc/2;
			}
		}
		else
		{
			vc_alignment_inc = 0;//duck out early
		}
	}

	float new_error = Virtual_Camera_Alignment_Error();
	if(new_error>=old_error||vc_alignment_inc<(EPSILON))
	{
		if(iterate_virtual_camera_yaw&&iterate_virtual_camera_yaw_first)
		{
			iterate_virtual_camera_yaw = false;//done with yaw
			iterate_virtual_camera_pitch = true;//give pitch a try
			vc_alignment_inc = starting_vc_alignment_inc;
		}
		else
		{
			Stop_VC_Alignment();
			if(iterate_vc_full_path)
			{
				selected_camera_station++;
				if(selected_camera_station==reference_camera_station)
				{
					selected_camera_station++;
				}
				if(selected_camera_station<Get_Num_Frames())
				{
					Start_VC_Alignment(selected_camera_station);
					Select_VC_Dialog_Frame(selected_camera_station);
				}
				else
				{
					Stop_Generate_Relative_Camera_Path();
				}
			}
		}
	}
	redraw_frame = true;
	return true;
}

VC_STATION* Get_Safe_Camera_Station(int i)
{
	int n = virtual_camera->stations.size();
	if(i>=n)i = n-1;
	if(i<0)i = 0;
	return virtual_camera->stations[i];
}

//check for rotations that swung past 180 and get them closer to the reference (ABS (359-1) is really TWO)
__forceinline float Clamp_Euler_Angle_To_Ref(float ref, float test)
{
	//these should never be off by more than a few degrees but may wrap
	if(ref<180)
	{
		if(test>180)
		{
			test -= 360;
		}
	}
	else//ref>180
	{
		if(test<180)
		{
			test += 360;
		}
	}
	return test;
}

float Clamp_Euler_Angle(float angle)
{
	while(angle>=360){angle-=360;}
	while(angle<0){angle+=360;}
	return angle;
}

//make sure rotations don't suddently swing around more than 180 degrees from the reference
void Clamp_Safe_Rotation(float *ref_rot, float *test_rot, float *res)
{
	res[0] = Clamp_Euler_Angle_To_Ref(ref_rot[0], test_rot[0]);
	res[1] = Clamp_Euler_Angle_To_Ref(ref_rot[1], test_rot[1]);
	res[2] = Clamp_Euler_Angle_To_Ref(ref_rot[2], test_rot[2]);
}

bool Get_Averaged_Camera_Station(int index, VC_STATION *res)
{
	VC_STATION *vs = Get_Safe_Camera_Station(index);
	res->pos[0] = vs->pos[0];
	res->pos[1] = vs->pos[1];
	res->pos[2] = vs->pos[2];
	res->rot[0] = vs->rot[0];
	res->rot[1] = vs->rot[1];
	res->rot[2] = vs->rot[2];
	int cnt = 1;
	float safe_rot[3];//get a safe rotation that doesn't fly off by more than 180 degrees
	float ref_rot[3];//a reference rotation that everyone can agree with
	for(int i = 0;i<num_camera_smooth_steps;i++)
	{
		vs = Get_Safe_Camera_Station(index+i);
		res->pos[0] += vs->pos[0];res->pos[1] += vs->pos[1];res->pos[2] += vs->pos[2];
		if(i==0)
		{
			res->rot[0] += vs->rot[0];res->rot[1] += vs->rot[1];res->rot[2] += vs->rot[2];
			//safe this as a reference for the others
			ref_rot[0] = vs->rot[0];ref_rot[1] = vs->rot[1];ref_rot[2] = vs->rot[2];
		}
		else
		{
			Clamp_Safe_Rotation(ref_rot, vs->rot, safe_rot);
			res->rot[0] += safe_rot[0];res->rot[1] += safe_rot[1];res->rot[2] += safe_rot[2];
		}
		cnt++;
		vs = Get_Safe_Camera_Station(index-i);
		res->pos[0] += vs->pos[0];res->pos[1] += vs->pos[1];res->pos[2] += vs->pos[2];
		Clamp_Safe_Rotation(ref_rot, vs->rot, safe_rot);
		res->rot[0] += safe_rot[0];res->rot[1] += safe_rot[1];res->rot[2] += safe_rot[2];
		cnt++;
	}
	res->pos[0] = res->pos[0]/cnt;
	res->pos[1] = res->pos[1]/cnt;
	res->pos[2] = res->pos[2]/cnt;
	res->rot[0] = Clamp_Euler_Angle(res->rot[0]/cnt);
	res->rot[1] = Clamp_Euler_Angle(res->rot[1]/cnt);
	res->rot[2] = Clamp_Euler_Angle(res->rot[2]/cnt);
	return true;
}

bool Smooth_Camera_Path()
{
	if(!virtual_camera)return false;
	int n = virtual_camera->stations.size();
	vector<VC_STATION*> temp_stations;
	int i;
	for(i = 1;i<n-1;i++)
	{
		VC_STATION *vs = new VC_STATION;
		Get_Averaged_Camera_Station(i, vs);
		temp_stations.push_back(vs);
	}
	for(i = 1;i<n-1;i++)
	{
		VC_STATION *src = temp_stations[i-1];
		VC_STATION *dst = virtual_camera->stations[i];
		dst->pos[0] = src->pos[0];
		dst->pos[1] = src->pos[1];
		dst->pos[2] = src->pos[2];
		dst->rot[0] = src->rot[0];
		dst->rot[1] = src->rot[1];
		dst->rot[2] = src->rot[2];
		dst->Update_Dir();
		delete src;
	}
	temp_stations.clear();
	redraw_frame = true;
	return true;
}
