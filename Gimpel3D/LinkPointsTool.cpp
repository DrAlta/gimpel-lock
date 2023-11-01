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
#include "LinkPointsTool.h"
#include "G3DMainFrame.h"
#include "GeometryTool.h"
#include "GeometryTool/Primitives.h"
#include "G3DCoreFiles/Frame.h"
#include <gl\gl.h>
#include "FeaturePoints.h"
#include "Skin.h"

bool Get_Frame_Screen_Coordinates(float *pos, float *x, float *y);

const int link_points_file_version = 2;

void Set_3D_View(float *rot, float *center, float zoom_out);
void Get_3D_View(float *rot, float *center, float *zoom_out);

void Push_3D_View();
void Pop_3D_View();

void Get_Matrices();

bool iterate_link_point_alignment = false;
bool iterate_link_point_alignment_position = false;
bool iterate_link_point_alignment_rotation = false;
bool iterate_link_point_alignment_depth = false;

bool Get_World_Pos(int x, int y, float *res);
bool Get_Screen_Coordinates(float *pos, float *x, float *y);


bool render_link_points_3d_tool = false;
bool render_link_points_vectors = false;

extern bool allow_layer_popup_menu;

extern bool dual_monitor;

bool Update_Link_Tool_Tracked_Feature_Point_Indices();

bool Update_Link_Tool_Model_Point_Indices();

bool model_linkpoints_modified = false;

bool view_link_points_model = true;
bool view_link_points_scene = false;

bool click_to_add_link_points = false;
bool click_to_delete_link_points = false;
bool click_to_move_link_points = true;

bool render_link_points_tool = false;

float lp_old_model_pos[3];
float lp_old_model_rot[3];
float lp_old_model_scale;

float lp_model_size[3];

int lp_model_index = -1;

int unique_model_point_id = 0;

class LINK_POINT
{
public:
	LINK_POINT()
	{
		feature_point_index = -1;
		feature_point_id = -1;
	}
	~LINK_POINT()
	{
	}
	void Update_Screen_Pos()
	{
		Get_Screen_Coordinates(pos, &px, &py);
	}
	float px, py;//for current frame
	float pos[3];
	//temp while gui is up
	int feature_point_index;
	int feature_point_id;
	int unique_id;
};

vector<LINK_POINT*> link_points_3d;

bool Clear_Link_Points()
{
	int n = link_points_3d.size();
	for(int i = 0;i<n;i++)
	{
		delete link_points_3d[i];
	}
	link_points_3d.clear();
	Update_Link_Tool_Model_Point_Indices();
	unique_model_point_id = 0;
	return true;
}

/*
void Flip_Link_Points_Model_Up()
{
	lp_model_rot[0] -= 90;
	Set_Scene_Primitive_Model_Info(lp_model_index, lp_model_pos, lp_model_rot, 1);
	redraw_frame = true;
}

void Flip_Link_Points_Model_Down()
{
	lp_model_rot[0] += 90;
	Set_Scene_Primitive_Model_Info(lp_model_index, lp_model_pos, lp_model_rot, 1);
	redraw_frame = true;
}

void Flip_Link_Points_Model_Left()
{
	lp_model_rot[1] -= 90;
	Set_Scene_Primitive_Model_Info(lp_model_index, lp_model_pos, lp_model_rot, 1);
	redraw_frame = true;
}

void Flip_Link_Points_Model_Right()
{
	lp_model_rot[1] += 90;
	Set_Scene_Primitive_Model_Info(lp_model_index, lp_model_pos, lp_model_rot, 1);
	redraw_frame = true;
}
*/

float Get_Best_Model_Distance(float *size)
{
	float *v = frame->Get_Identity_Vector(0, 0);
	float ratio = -(v[2]/v[1]);
	return ratio*size[1];
}

bool Get_Model_Link_Points_Filename(char *model_file, char *res)
{
	strcpy(res, model_file);
	char *c = strrchr(res, '.');
	if(!c){return false;}
	strcpy(c, ".lps");
	return true;
}

bool Load_Link_Points(char *file)
{
	FILE *f = fopen(file, "rb");
	if(!f){return false;}
	int version = 0;
	int n = 0;
	fread(&version, sizeof(int), 1, f);
	if(version!=link_points_file_version)
	{
		SkinMsgBox(0, "Can't load model link points, version mismatch!", "This should never happen", MB_OK);
		fclose(f);
		return false;
	}
	fread(&n, sizeof(int), 1, f);
	float *buf = new float[n*4];//pos and id
	fread(buf, sizeof(float), n*4, f);
	fclose(f);
	LINK_POINT *lp;
	for(int i = 0;i<n;i++)
	{
		lp = new LINK_POINT;
		lp->pos[0] = buf[i*4];
		lp->pos[1] = buf[(i*4)+1];
		lp->pos[2] = buf[(i*4)+2];
		lp->unique_id = (int)buf[(i*4)+3];
		unique_model_point_id = lp->unique_id+1;
		link_points_3d.push_back(lp);
	}
	delete[] buf;
	model_linkpoints_modified = false;
	return true;
}

bool Save_Link_Points(char *file)
{
	FILE *f = fopen(file, "wb");
	if(!f){return false;}
	int n = link_points_3d.size();
	fwrite(&link_points_file_version, sizeof(int), 1, f);
	fwrite(&n, sizeof(int), 1, f);
	float *buf = new float[n*4];
	for(int i = 0;i<n;i++)
	{
		buf[i*4] = link_points_3d[i]->pos[0];
		buf[(i*4)+1] = link_points_3d[i]->pos[1];
		buf[(i*4)+2] = link_points_3d[i]->pos[2];
		buf[(i*4)+3] = link_points_3d[i]->unique_id;
	}
	fwrite(buf, sizeof(float), n*4, f);
	fclose(f);
	delete[] buf;
	model_linkpoints_modified = false;
	return true;
}

bool Try_Save_Current_Model_Link_Points()
{
	char mfile[512];
	if(!Get_Scene_Primitive_Model_Filename(lp_model_index, mfile)){return false;}
	char lpfile[512];
	if(!Get_Model_Link_Points_Filename(mfile, lpfile)){return false;}
	if(!Save_Link_Points(lpfile)){return false;}
	return true;
}

bool Try_Load_Model_Link_Points()
{
	Clear_Link_Points();
	char mfile[512];
	if(!Get_Scene_Primitive_Model_Filename(lp_model_index, mfile)){return false;}
	char lpfile[512];
	if(!Get_Model_Link_Points_Filename(mfile, lpfile)){return false;}
	if(!Load_Link_Points(lpfile)){return false;}
	return true;
}

float last_lp_rot[3];
float last_lp_center[3];
float last_lp_zoom;

bool View_Link_Points_Model()
{
	if(view_link_points_model)return false;
	view_link_points_model = true;
	view_link_points_scene = false;
	//push the current scene view and restore the last zoom around the origin
	Push_3D_View();
	Set_3D_View(last_lp_rot, last_lp_center, last_lp_zoom);
	redraw_frame = true;
	float id[3] = {0,0,0};
	Set_Scene_Primitive_Model_Info(lp_model_index, id, id, 1);
	return true;
}

bool View_Link_Points_Scene()
{
	if(view_link_points_scene)return false;
	//save the current zoom and restore the previous pushed view
	Get_3D_View(last_lp_rot, last_lp_center, &last_lp_zoom);
	Pop_3D_View();
	redraw_frame = true;
	view_link_points_model = false;
	view_link_points_scene = true;
	float id[3] = {0,0,0};
	Set_Scene_Primitive_Model_Info(lp_model_index, lp_old_model_pos, lp_old_model_rot, lp_old_model_scale);
	return true;
}


bool link_points_tool_open = false;
bool Load_Layer_Links_Info();

bool Open_Link_Points_Tool(int layer_id)
{
	if(link_points_tool_open)return true;
	lp_model_index = Find_Scene_Model_Primitive_Index(layer_id);
	if(lp_model_index==-1)
	{
		SkinMsgBox(0, "Error opening link points tool, unable to find trimesh model for selected layer!", "This never happens", MB_OK);
		return false;
	}
	Get_Scene_Primitive_Model_Info(lp_model_index, lp_model_size, lp_old_model_pos, lp_old_model_rot, &lp_old_model_scale);
	Try_Load_Model_Link_Points();
	float id[3] = {0,0,0};
	Set_Scene_Primitive_Model_Info(lp_model_index, id, id, 1);
	render_link_points_3d_tool = true;
	render_link_points_vectors = true;
	redraw_frame = true;
	redraw_edit_window = true;
	if(!dual_monitor)
	{
		mainframe->OnSplitScreen();
	}
	mainframe->SetFocus();
	mainframe->OnFeaturePointsTool();
	allow_layer_popup_menu = false;
	render_link_points_tool = true;

	//set best camera zoom
	Push_3D_View();
	float zoom = Get_Best_Model_Distance(lp_model_size);
	Set_3D_View(id, id, zoom);
	link_points_tool_open = true;
	view_link_points_model = true;
	view_link_points_scene = false;
	mainframe->SetFocus();
	Open_Link_Points_Dlg();
	Update_Link_Points_Dlg_View_Checkboxes();
	Load_Layer_Links_Info();
	Update_Link_Tool_Tracked_Feature_Point_Indices();
	return true;
}

bool Close_Link_Points_Tool()
{
	if(iterate_link_point_alignment)Stop_Iterative_Link_Point_Alignment();
	if(model_linkpoints_modified)
	{
		if(SkinMsgBox("Alignments points associated with the model were modified! Save changes?", 0, MB_YESNO)==IDYES)
		{
			Try_Save_Current_Model_Link_Points();
		}
	}
	Save_Layer_Links_Info();
	Close_Link_Points_Dlg();
	render_link_points_3d_tool = false;
	redraw_frame = true;
	redraw_edit_window = true;
	allow_layer_popup_menu = true;
	render_link_points_tool = false;
	//restore old camera
	View_Link_Points_Scene();
	Clear_Link_Points();
	link_points_tool_open = false;
	return true;
}


LINK_POINT *clicked_link_point = 0;
float link_point_xclick_off = 0;
float link_point_yclick_off = 0;

void Update_Link_Point_Screen_Coordinates()
{
	int n = link_points_3d.size();
	for(int i = 0;i<n;i++)
	{
		link_points_3d[i]->Update_Screen_Pos();
	}
}

__forceinline bool Try_Select_Link_Point_3D(LINK_POINT *fp, float px, float py, float range)
{
	float dx = px-fp->px;if(dx<0)dx = -dx;
	float dy = py-fp->py;if(dy<0)dy = -dy;
	if(dx>range||dy>range)return false;
	link_point_xclick_off = px-fp->px;
	link_point_yclick_off = py-fp->py;
	return true;
}

LINK_POINT* Find_Closest_Clicked_Link_Point_3D(float px, float py)
{
	int n = link_points_3d.size();
	float pos[3];
	if(!Get_World_Pos((int)px, (int)py, pos))
	{
		return false;
	}
	float best_distance = 0;
	float d;
	LINK_POINT *best_lp = 0;
	for(int i = 0;i<n;i++)
	{
		if(Try_Select_Link_Point_3D(link_points_3d[i], px, py, 5))
		{
			d = glApp.math.Distance3D(link_points_3d[i]->pos, pos);
			if(d<best_distance||!best_lp)
			{
				best_lp = link_points_3d[i];
				best_distance = d;
			}
		}
	}
	return best_lp;
}

bool Find_Clicked_Link_Point_3D(float px, float py)
{
	clicked_link_point = Find_Closest_Clicked_Link_Point_3D(px, py);
	return clicked_link_point!=0;
}

bool Add_Link_Point_3D(float x, float y)
{
	float pos[3];
	if(!Get_World_Pos((int)x, (int)y, pos))
	{
		return false;
	}
	LINK_POINT *fp = new LINK_POINT;
	link_points_3d.push_back(fp);
	fp->unique_id = unique_model_point_id;
	unique_model_point_id++;
	clicked_link_point = fp;
	clicked_link_point->px = frame->width/2;
	clicked_link_point->py = frame->height/2;
	clicked_link_point->pos[0] = pos[0];
	clicked_link_point->pos[1] = pos[1];
	clicked_link_point->pos[2] = pos[2];
	redraw_frame = true;
	redraw_edit_window = true;
	model_linkpoints_modified = true;
	return true;
}

bool Delete_Clicked_Link_Point_3D(float px, float py)
{
	LINK_POINT *lp = Find_Closest_Clicked_Link_Point_3D(px, py);
	if(!lp)return false;
	int n = link_points_3d.size();
	for(int i = 0;i<n;i++)
	{
		if(link_points_3d[i]==lp)
		{
			delete link_points_3d[i];
			link_points_3d.erase(link_points_3d.begin()+i);
			Update_Link_Tool_Model_Point_Indices();
			model_linkpoints_modified = true;
			redraw_frame = true;
			return true;
		}
	}
	return false;
}

int Get_Selected_Link_Point_Index()
{
	if(!clicked_link_point)return -1;
	int n = link_points_3d.size();
	for(int i = 0;i<n;i++)
	{
		if(clicked_link_point==link_points_3d[i])
		{
			return i;
		}
	}
	return -1;
}

int Get_Selected_Link_Point_ID()
{
	if(!clicked_link_point)return -1;
	return clicked_link_point->unique_id;
}

int Find_Model_Point_Index(int unique_id)
{
	int n = link_points_3d.size();
	for(int i = 0;i<n;i++)
	{
		if(link_points_3d[i]->unique_id==unique_id)
		{
			return i;
		}
	}
	return -1;
}

bool Get_Link_Point_3D_Position(int index, float *pos)
{
	int n = link_points_3d.size();
	if(index<0||index>=n)
	{
		return false;
	}
	pos[0] = link_points_3d[index]->pos[0];
	pos[1] = link_points_3d[index]->pos[1];
	pos[2] = link_points_3d[index]->pos[2];
	return true;
}

bool Click_Link_Points_Tool_3D(float x, float y)
{
	if(click_to_add_link_points)
	{
		Add_Link_Point_3D(x, y);
	}
	else if(click_to_delete_link_points)
	{
		clicked_link_point = 0;
		Delete_Clicked_Link_Point_3D(x, y);
	}
	else if(click_to_move_link_points)
	{
		Find_Clicked_Link_Point_3D(x, y);
		if(clicked_link_point)
		{
			if(clicked_link_point->feature_point_id!=-1)
			{
				Select_Feature_Point(Find_Feature_Point_Index(clicked_link_point->feature_point_id));
				redraw_edit_window = true;
			}
		}
		redraw_frame = true;
	}
	return false;
}

bool Drag_Link_Points_Tool_3D(float x, float y)
{
	if(clicked_link_point)
	{
		x -= link_point_xclick_off;
		y -= link_point_yclick_off;
		float pos[3];
		if(!Get_World_Pos((int)x, (int)y, pos))
		{
			return false;
		}
		if(clicked_link_point->pos[0]!=pos[0]||clicked_link_point->pos[1]!=pos[1]||clicked_link_point->pos[2]!=pos[2])
		{
			model_linkpoints_modified = true;
		}
		clicked_link_point->pos[0] = pos[0];
		clicked_link_point->pos[1] = pos[1];
		clicked_link_point->pos[2] = pos[2];
		clicked_link_point->px = x;
		clicked_link_point->py = y;
		redraw_frame = true;
		return true;
	}
	return false;
}

void Render_Link_Points_Tool_3D_Model()
{
	Render_Scene_Primitive_Model(lp_model_index);
	int n = link_points_3d.size();
	glPointSize(10);
	glColor3f(0.2f,0.8f,0.2f);
	glBegin(GL_POINTS);
	for(int i = 0;i<n;i++)
	{
		glVertex3fv(link_points_3d[i]->pos);
	}
	if(clicked_link_point)
	{
		if(clicked_link_point->feature_point_id!=-1)
		{
			glColor3f(0,0.5f,0.5f);
		}
		else
		{
			glColor3f(1,0,0);
		}
		glVertex3fv(clicked_link_point->pos);
	}
	glEnd();
	glColor3f(1,1,1);
	glPointSize(1);
	Update_Link_Point_Screen_Coordinates();
}

class LINK_ASSOCIATION
{
public:
	LINK_ASSOCIATION()
	{
		pos_3d[0] = pos_3d[1] = pos_3d[2] = 0;
		pos_2d[0] = pos_2d[1] = 0;
		model_link_point_index = -1;
		model_link_point_id = -1;
		feature_point_index = -1;
		feature_point_id = -1;
	}
	~LINK_ASSOCIATION(){}
	float pos_3d[3];//at the time it was created
	float pos_2d[2];
	float vec_3d[3];//pixel vector
	//indices
	int model_link_point_index;
	int model_link_point_id;
	int feature_point_index;
	int feature_point_id;
};

vector<LINK_ASSOCIATION*> link_associations;


bool Update_Link_Tool_Tracked_Feature_Point_Indices()
{
	int n = link_points_3d.size();
	int i;
	for(i = 0;i<n;i++)
	{
		link_points_3d[i]->feature_point_index = Find_Feature_Point_Index(link_points_3d[i]->feature_point_id);
	}
	n = link_associations.size();
	for(i = 0;i<n;i++)
	{
		link_associations[i]->feature_point_index = Find_Feature_Point_Index(link_associations[i]->feature_point_id);
	}
	return true;
}

bool Update_Link_Tool_Model_Point_Indices()
{
	int n = link_associations.size();
	for(int i = 0;i<n;i++)
	{
		link_associations[i]->model_link_point_index = Find_Model_Point_Index(link_associations[i]->model_link_point_id);
	}
	return true;
}

bool Clear_All_Link_Associations()
{
	int n = link_associations.size();
	int i;
	for(i = 0;i<n;i++)
	{
		delete link_associations[i];
	}
	link_associations.clear();
	n = link_points_3d.size();
	for(i = 0;i<n;i++)
	{
		link_points_3d[i]->feature_point_index = -1;
		link_points_3d[i]->feature_point_id = -1;
	}
	redraw_frame = true;
	return false;
}

bool Update_Link_Association_Positions(LINK_ASSOCIATION *la)
{
	Get_Link_Point_3D_Position(la->model_link_point_index, la->pos_3d);
	Get_Feature_Point_2D_Position(la->feature_point_index, la->pos_2d);
	if(frame->IsValidPixel((int)la->pos_2d[0], (int)la->pos_2d[1]))
	{
		float *v = frame->Get_Frame_Vector((int)la->pos_2d[0], (int)la->pos_2d[1]);
		la->vec_3d[0] = v[0];la->vec_3d[1] = v[1];la->vec_3d[2] = v[2];
		return true;
	}
	else
	{
		la->vec_3d[0] = la->vec_3d[1] = la->vec_3d[2] = 0;
	}
	return false;
}

bool Update_Link_Association_Positions()
{
	int n = link_associations.size();
	for(int i = 0;i<n;i++)
	{
		Update_Link_Association_Positions(link_associations[i]);
	}
	return true;
}

bool Create_Link_Association(int link_point_id, int feature_point_id)
{
	int feature_point = Find_Feature_Point_Index(feature_point_id);
	if(feature_point==-1)
	{
		return false;
	}
	int link_point = Find_Model_Point_Index(link_point_id);
	if(link_point==-1)
	{
		return false;
	}
	int n = link_points_3d.size();
	char msg[512];
	if(link_point<0||link_point>=n)
	{
		sprintf(msg, "ERROR! Link data references invalid model point index %i, model has %i total link points", link_point, n);
		SkinMsgBox(msg);
		SkinMsgBox("Link will not be created, one or more other links may also have errors.");
		return false;
	}
	if(feature_point<0||feature_point>=Num_Feature_Points())
	{
		sprintf(msg, "ERROR! Link data references invalid feature point index %i, session has %i total feature points", feature_point, Num_Feature_Points());
		SkinMsgBox(msg);
		SkinMsgBox("Link will not be created, one or more other links may also have errors.");
		return false;
	}
	Print_Status("Linking 3d point %i and feature point %i", link_point, feature_point);
	LINK_ASSOCIATION *la = new LINK_ASSOCIATION;
	link_associations.push_back(la);
	la->model_link_point_index = link_point;
	la->model_link_point_id = link_point_id;
	la->feature_point_index = feature_point;
	la->feature_point_id = feature_point_id;
	Update_Link_Association_Positions(la);
	link_points_3d[link_point]->feature_point_index = feature_point;
	link_points_3d[link_point]->feature_point_id = feature_point_id;
	return true;
}

bool Save_Layer_Links_Info()
{
	vector<int> links;
	int n = link_associations.size();
	for(int i = 0;i<n;i++)
	{
		int t = link_associations[i]->model_link_point_id;
		links.push_back(t);
		t = link_associations[i]->feature_point_id;
		links.push_back(t);
	}
	if(!Set_Scene_Primitive_Model_Link_Info(lp_model_index, &links))
	{
		links.clear();
		return false;
	}
	links.clear();
	return true;
}

bool Load_Layer_Links_Info()
{
	vector<int> links;
	if(!Get_Scene_Primitive_Model_Link_Info(lp_model_index, &links))
	{
		return false;
	}
	int missing_cnt = 0;
	int n = links.size();
	for(int i = 0;i<n;i+=2)
	{
		if(!Create_Link_Association(links[i], links[i+1]))
		{
			//only returns false if invalid points
			missing_cnt++;
		}
	}
	links.clear();
	if(missing_cnt>0)
	{
		SkinMsgBox("Warning! The stored link information references one or more invalid feature and/or model points.");// One link association was not loaded.");
	}
	return true;
}

bool Destroy_Link_Association(int link_point_id, int feature_point_id)
{
	int n = link_associations.size();
	for(int i = 0;i<n;i++)
	{
		if(link_associations[i]->model_link_point_id==link_point_id&&link_associations[i]->feature_point_id==feature_point_id)
		{
			link_associations.erase(link_associations.begin()+i);
			return true;
		}
	}
	return false;
}

bool Link_Selected_Points()
{
	//associate selected model point with selected feature point
	int lpid = Get_Selected_Link_Point_ID();
	int fpid = Get_Selected_Feature_Point_ID();
	if(lpid==-1&&fpid==-1)
	{
		SkinMsgBox(0, "No selected points in either view, nothing to link");
		return false;
	}
	else if(lpid==-1)
	{
		SkinMsgBox(0, "No selected point on the 3d model view, nothing to link");
		return false;
	}
	else if(fpid==-1)
	{
		SkinMsgBox(0, "No selected feature point in the 2d view, nothing to link");
		return false;
	}
	if(!Create_Link_Association(lpid, fpid)){return false;}
	redraw_frame = true;
	return true;
}

bool UnLink_Selected_Points()
{
	//disassociate selected model point with selected feature point
	int lpid = Get_Selected_Link_Point_ID();
	int fpid = Get_Selected_Feature_Point_ID();
	if(lpid==-1&&fpid==-1){return false;}
	else if(lpid==-1){return false;}
	else if(fpid==-1){return false;}
	
	int lpi = Find_Model_Point_Index(lpid);
	int fpi = Find_Feature_Point_Index(fpid);
	
	if(lpi==-1&&fpi==-1){return false;}
	else if(lpi==-1){return false;}
	else if(fpi==-1){return false;}
	
	if(link_points_3d[lpi]->feature_point_index!=fpi||link_points_3d[lpi]->feature_point_id!=fpid){return false;}

	if(!Destroy_Link_Association(lpid, fpid)){return false;}
	
	link_points_3d[lpi]->feature_point_index = -1;
	link_points_3d[lpi]->feature_point_id = -1;

	redraw_frame = true;
	return true;
}

bool Notify_Link_Point_Tool_Changed_Feature_Point()
{
	int fpid = Get_Selected_Feature_Point_ID();
	if(fpid==-1){return false;}
	int n = link_points_3d.size();
	for(int i = 0;i<n;i++)
	{
		if(link_points_3d[i]->feature_point_id==fpid)
		{
			clicked_link_point = link_points_3d[i];
			redraw_frame = true;
			return true;
		}
	}
	return false;
}

__forceinline float Get_Link_Point_Alignment_Error(float *pos, float *rot, float scale)
{
	float res = 0;
	BB_MATRIX mm;
	float id[3] = {0,0,0};
	mm.Set(mm.m, id, rot);
	int n = link_associations.size();
	float *es = new float[n];//individual scores
	int i;
	for(i = 0;i<n;i++)
	{
		LINK_ASSOCIATION *la = link_associations[i];
		float tp1[3];
		//get the projetced point on the model in 3d
		tp1[0] = la->pos_3d[0]*scale;//to scale
		tp1[1] = la->pos_3d[1]*scale;
		tp1[2] = la->pos_3d[2]*scale;
		mm.Transform_Vertex(mm.m, tp1);//rotated
		tp1[0] += pos[0];//and moved
		tp1[1] += pos[1];
		tp1[2] += pos[2];
		//try screen space
		float px, py;
		Get_Frame_Screen_Coordinates(tp1, &px, &py);
		float dx = la->pos_2d[0]-px;
		float dy = la->pos_2d[1]-py;
		float e = sqrt((dx*dx)+(dy*dy));
		res += e;
		es[i] = e;
	}
	float avg = res/n;//average error
	float imbalance_penalty = 1;//5;//strive to balance all errors
	for(i = 0;i<n;i++)
	{
		//add deviation from average to total error
		if(es[i]>avg)
		{
			res += (es[i]-avg)*imbalance_penalty;
		}
		else
		{
			res -= (es[i]-avg)*imbalance_penalty;
		}
	}
	delete[] es;
	return res;
}

void Render_Link_Points_Vectors()
{
	Update_Link_Association_Positions();//check for any moved points
	//get fresh info for model orientation/scale
	float size[3];
	float pos[3];
	float rot[3];
	float scale;
	if(!Get_Scene_Primitive_Model_Info(lp_model_index, size, pos, rot, &scale))
	{
		return;
	}
	BB_MATRIX mm;
	float id[3] = {0,0,0};
	mm.Set(mm.m, id, rot);
	int n = link_associations.size();
	glBegin(GL_LINES);
	glColor3f(1,0,0);
	for(int i = 0;i<n;i++)
	{
		LINK_ASSOCIATION *la = link_associations[i];
		float tp[3];
		//get the projected point on the model in 3d
		tp[0] = la->pos_3d[0]*scale;//to scale
		tp[1] = la->pos_3d[1]*scale;
		tp[2] = la->pos_3d[2]*scale;
		mm.Transform_Vertex(mm.m, tp);//rotated
		tp[0] += pos[0];//and moved
		tp[1] += pos[1];
		tp[2] += pos[2];
		glColor3f(1,0.25f,0.25f);//draw a line
		glVertex3fv(frame->view_origin);
		glVertex3fv(tp);
		tp[0] = la->vec_3d[0];//get the screen vector
		tp[1] = la->vec_3d[1];
		tp[2] = la->vec_3d[2];
		glApp.math.Normalize(tp);
		glColor3f(0.25f,1,0.25f);//draw a line
		glVertex3fv(frame->view_origin);
		tp[0] += frame->view_origin[0];
		tp[1] += frame->view_origin[1];
		tp[2] += frame->view_origin[2];
		glVertex3fv(tp);
	}
	glEnd();
	glColor3f(1,1,1);
}

float lowest_lp_alignment_error = 0;
int num_lp_alignment_iterations = 0;

//to start with
float lp_rotation_alignment = 1;//1 degree at a time
float lp_position_alignment = 0.1f;//world units

//test this orientation against the current lowest error
bool Test_Link_Point_Alignment(float *pos, float *rot, float scale)
{
	float e = Get_Link_Point_Alignment_Error(pos, rot, scale);
	if(lowest_lp_alignment_error>e)
	{
		lowest_lp_alignment_error = e;
		return true;
	}
	return false;
}

__forceinline void Clamp180Range(float *v)
{
}

bool Test_Link_Point_Rotation_Alignment(float *pos, float *rot, float scale, float inc)
{
	if(iterate_link_point_alignment_rotation)
	{
		rot[2] += inc;Clamp180Range(&rot[2]);while(Test_Link_Point_Alignment(pos, rot, scale)){rot[2] += inc;Clamp180Range(&rot[2]);}rot[2] -= inc;Clamp180Range(&rot[2]);
		rot[0] += inc;Clamp180Range(&rot[0]);while(Test_Link_Point_Alignment(pos, rot, scale)){rot[0] += inc;Clamp180Range(&rot[0]);}rot[0] -= inc;Clamp180Range(&rot[0]);
		rot[1] += inc;Clamp180Range(&rot[1]);while(Test_Link_Point_Alignment(pos, rot, scale)){rot[1] += inc;Clamp180Range(&rot[1]);}rot[1] -= inc;Clamp180Range(&rot[1]);
	}
	return true;
}

bool Test_Link_Point_Position_Alignment(float *pos, float *rot, float scale, float inc)
{
	if(iterate_link_point_alignment_position)
	{
		pos[0] += inc;while(Test_Link_Point_Alignment(pos, rot, scale)){pos[0] += inc;}pos[0] -= inc;
		pos[1] += inc;while(Test_Link_Point_Alignment(pos, rot, scale)){pos[1] += inc;}pos[1] -= inc;
	}
	if(iterate_link_point_alignment_depth)
	{
		if(pos[2]+inc>0)
		{
			pos[2] += inc;while(Test_Link_Point_Alignment(pos, rot, scale))
			{
				pos[2] += inc;
				if(pos[2]<0)
				{
					break;
				}
			}
			pos[2] -= inc;
		}
	}
	return true;
}


bool Update_Link_Point_Alignment()
{
	float last_error = lowest_lp_alignment_error;
	float size[3];
	float pos[3];
	float rot[3];
	float scale;
	if(!Get_Scene_Primitive_Model_Info(lp_model_index, size, pos, rot, &scale))
	{
		SkinMsgBox(0, "INTERNAL ERROR! Attempt to iterate link point alignment with invalid selection!", "This never happens", MB_OK);
		Stop_Iterative_Link_Point_Alignment();
		return false;
	}
	Test_Link_Point_Rotation_Alignment(pos, rot, scale, lp_rotation_alignment);
	Test_Link_Point_Rotation_Alignment(pos, rot, scale, -lp_rotation_alignment);
	Test_Link_Point_Position_Alignment(pos, rot, scale, lp_position_alignment);
	Test_Link_Point_Position_Alignment(pos, rot, scale, -lp_position_alignment);
	num_lp_alignment_iterations++;
	Print_Status("Alignment pass %i error %f", num_lp_alignment_iterations, lowest_lp_alignment_error);
	if(lowest_lp_alignment_error<last_error)//improvement?
	{
		//save it
		Set_Scene_Primitive_Model_Info(lp_model_index, pos, rot, scale);
		return true;
	}
	lp_rotation_alignment = lp_rotation_alignment*0.5f;
	lp_position_alignment = lp_position_alignment*0.5f;
	lp_old_model_pos[0] = pos[0];
	lp_old_model_pos[1] = pos[1];
	lp_old_model_pos[2] = pos[2];
	lp_old_model_rot[0] = rot[0];
	lp_old_model_rot[1] = rot[1];
	lp_old_model_rot[2] = rot[2];
	return false;
}

bool Keyframe_Linked_Model_Layer()
{
	if(lp_model_index>=0&&lp_model_index<Num_Layers())
	{
		return Keyframe_Layer(lp_model_index, true);
	}
	return false;
}

bool Start_Iterative_Link_Point_Alignment(bool position, bool rotation, bool depth)
{
	float size[3];
	float pos[3];
	float rot[3];
	float scale;
	if(!Get_Scene_Primitive_Model_Info(lp_model_index, size, pos, rot, &scale))
	{
		SkinMsgBox(0, "INTERNAL ERROR! Attempt to start link point alignment with invalid selection!", "This never happens", MB_OK);
		return false;
	}
	float id[3] = {0,0,0};
	//start with no rotation
	
	iterate_link_point_alignment_position = position;
	iterate_link_point_alignment_rotation = rotation;
	iterate_link_point_alignment_depth = depth;

	iterate_link_point_alignment = true;
	Update_Link_Point_Dlg_Align_Button();
	Update_Link_Association_Positions();//check for any moved points
	//get the current alignment error
	lowest_lp_alignment_error = Get_Link_Point_Alignment_Error(pos, rot, scale);
	num_lp_alignment_iterations = 0;
	lp_rotation_alignment = 1;//1 degree at a time
	lp_position_alignment = 0.1f;//world units
	return true;
}

bool Stop_Iterative_Link_Point_Alignment()
{
	iterate_link_point_alignment = false;
	iterate_link_point_alignment_position = false;
	iterate_link_point_alignment_rotation = false;
	iterate_link_point_alignment_depth = false;
	Update_Link_Point_Dlg_Align_Button();
	Print_Status("Done.");
	return true;
}

bool Show_Link_Points_Vectors(bool b)
{
	render_link_points_vectors = b;
	return false;
}

bool Find_Initial_Link_Point_Alignment()
{
	float size[3];
	float pos[3];
	float rot[3];
	float scale;
	if(!Get_Scene_Primitive_Model_Info(lp_model_index, size, pos, rot, &scale))
	{
		SkinMsgBox("ERROR getting model info, can't set initial alignment");
		return false;
	}
	float deg_inc = 10;
	float npp = 360.0f/deg_inc;
	int np = (int)npp;
	int total = np*np*np;
	int i, j, k;
	float lowest = 99999;
	float best_rot[3] = {0,0,0};
	float error = 0;
	int cnt = 0;
	for(i = 0;i<np;i++)
	{
		for(j = 0;j<np;j++)
		{
			for(k = 0;k<np;k++)
			{
				rot[0] = deg_inc*i;
				rot[1] = deg_inc*j;
				rot[2] = deg_inc*k;
				Print_Status("Testing %i of %i", cnt, total);
				cnt++;
				error = Get_Link_Point_Alignment_Error(pos, rot, scale);
				if(error<lowest)
				{
					lowest = error;
					best_rot[0] = rot[0];
					best_rot[1] = rot[1];
					best_rot[2] = rot[2];
				}
			}
		}
	}
	Print_Status("Best rotation %f %f %f", best_rot[0], best_rot[1], best_rot[2]);
	Set_Scene_Primitive_Model_Info(lp_model_index, pos, best_rot, scale);
	redraw_frame = true;
	return true;
}






