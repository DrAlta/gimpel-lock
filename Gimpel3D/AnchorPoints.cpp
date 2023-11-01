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
#include "AnchorPoints.h"
#include "G3DCoreFiles/Frame.h"
#include "G3DCoreFiles/Layers.h"
#include "GeometryTool/GLBasic.h"
#include "CircularMaps.h"
#include <math.h>
#include <gl\gl.h>
#include <vector>
#include "Plugins.h"
#include "Skin.h"
#include "FeaturePoints.h"
#include "Plugins.h"


using namespace std;

float Pixel_Size();

bool Open_Feature_Points_Tool();
int Get_Selected_Feature_Point_ID();
bool Get_Project_Aux_Data_Filename(char *res, char *extension);

bool Enable_Anchor_Type_Selection(bool b);

bool Update_Selected_Anchor_Point();

bool Try_Remove_Layer_Flag_Has_Anchor_Points(int layer_id);

bool render_anchor_points_tool = false;
bool render_anchor_points_2d = false;

float default_anchor_range = 10;
int default_anchor_point_falloff_type = 0;

bool update_anchor_points_in_realtime = true;

float VecLength(float* v);
void SetLength(float *v, float s);
bool Get_Session_Aux_Data_Filename(char *res, char *extension);

bool Layer_Has_Anchor_Points(int layer_id);
bool Set_Layer_Has_Anchor_Points(int layer_id, bool b);

bool Render_Anchor_Range(float px, float py, float size)
{
	int n = 50;
	float angle = 0;
	glBegin(GL_LINE_STRIP);
	for(int i = 0;i<n;i++)
	{
		angle = (((float)i/n)*360)*RAD;
		glVertex2f(px+(cos(angle)*size), py+(sin(angle)*size));
	}
	angle = 0;
	glVertex2f(px+(cos(angle)*size), py+(sin(angle)*size));
	glEnd();
	return true;
}

struct ANCHOR_POINT_KEYFRAMED_DATA
{
	float depth;
	float influence_range;
};

class ANCHOR_POINT
{
public:
	ANCHOR_POINT()
	{
		pos2d[0] = pos2d[1] = 0;
		pos3d[0] = pos3d[1] = pos3d[2] = 0;
		current_influence_range = default_anchor_range;
		px = py = -1;
		falloff_type = default_anchor_point_falloff_type;
		layer_id = -1;
		feature_point_id = -1;
		frames_data = 0;
	}
	~ANCHOR_POINT()
	{
		Free_Frames_Data();
	}
	bool Update_Positions()
	{
		//align to the attached feature point
		Get_Feature_Point_2D_Position(Find_Feature_Point_Index(feature_point_id), pos2d);
		//find the 3d position
		px = (int)pos2d[0];//get the pixel coordinates
		py = (int)pos2d[1];
		if(!frame->IsValidPixel(px, py))
		{
			//feature point pixel is out of frame, set it to the origin
			pos3d[0] = frame->view_origin[0];
			pos3d[1] = frame->view_origin[1];
			pos3d[2] = frame->view_origin[2];
			return false;
		}
		//get the frame vector at this point
		float *fv = frame->Get_Frame_Vector(px, py);
		float vec[3] = {fv[0],fv[1],fv[2]};//make a local copy
		SetLength(vec, current_depth);//set it to the depth of this anchor point
		pos3d[0] = frame->view_origin[0]+vec[0];//project from the origin
		pos3d[1] = frame->view_origin[1]+vec[1];//to get
		pos3d[2] = frame->view_origin[2]+vec[2];//the final position
		return true;
	}
	//flag or unflag the layer affected by this anchor point
	//called after points have moved
	bool Update_Affected_Layer()
	{
		int new_layer = -1;//assume it touched no layer
		int px = (int)pos2d[0];//get the pixel coordinates
		int py = (int)pos2d[1];
		if(frame->IsValidPixel(px, py))
		{
			new_layer = frame->Get_Pixel_Layer(px, py);//check the layer at the current pixel
		}
		if(new_layer!=layer_id)
		{
			//oops we are covering a different layer
			Try_Remove_Layer_Flag_Has_Anchor_Points(layer_id);
			//get index for the previous affected layer
			int index = Get_Layer_Index(layer_id);
			layer_id = new_layer;
			//fix that last layer
			Flag_Layer_For_ReProject(index);
			redraw_frame = true;
			//set the flag for the new one
			Set_Layer_Has_Anchor_Points(new_layer, true);
		}
		return true;
	}
	bool Allocate_Frames_Data()
	{
		int num_frames = Get_Num_Frames();
		if(num_frames>0)
		{
			frames_data = new ANCHOR_POINT_KEYFRAMED_DATA[num_frames];
			for(int i = 0;i<num_frames;i++)
			{
				frames_data[i].depth = current_depth;
				frames_data[i].influence_range = current_influence_range;
			}
		}
		return true;
	}
	bool Free_Frames_Data()
	{
		if(frames_data)delete[] frames_data;
		frames_data = 0;
		return true;
	}
	bool Interpolate_Keyframe_Data()
	{
		if(!frames_data)
		{
			return false;
		}
		int current_frame = Get_Current_Project_Frame();
		int fpi = Find_Feature_Point_Index(feature_point_id);
		if(Feature_Point_Is_Keyframed(fpi, current_frame))
		{
			current_depth = frames_data[current_frame].depth;
			current_influence_range = frames_data[current_frame].influence_range;
		}
		else
		{
			int next = Get_Next_Feature_Point_Keyframe(fpi, current_frame);
			int previous = Get_Previous_Feature_Point_Keyframe(fpi, current_frame);
			float p = 1;
			float ip = 0;
			if(next==-1)
			{
				next = previous;
				p = 1;
				ip = 0;
			}
			else if(previous==-1)
			{
				previous = next;
				p = 1;
				ip = 0;
			}
			else
			{
				p = ((float)(current_frame-previous))/((float)next-previous);
				ip = 1.0f-p;
			}
			frames_data[current_frame].depth = (frames_data[next].depth*p)+(frames_data[previous].depth*ip);
			frames_data[current_frame].influence_range = (frames_data[next].influence_range*p)+(frames_data[previous].influence_range*ip);
			current_depth = frames_data[current_frame].depth;
			current_influence_range = frames_data[current_frame].influence_range;
		}
		return true;
	}
	ANCHOR_POINT_KEYFRAMED_DATA *frames_data;
	int px, py;
	int feature_point_id;
	float pos2d[2];
	float pos3d[3];
	float current_depth;
	float current_influence_range;
	int falloff_type;
	int layer_id;
};

vector<ANCHOR_POINT*> anchor_points;

ANCHOR_POINT *selected_anchor_point = 0;

bool anchor_points_tool_open = false;

bool Open_Anchor_Points_Tool()
{
	if(anchor_points_tool_open)return false;
	Open_Anchor_Points_Dlg();
	render_anchor_points_tool = true;
	render_anchor_points_2d = true;
	anchor_points_tool_open = true;
	Open_Feature_Points_Tool();
	Update_Selected_Anchor_Point();
	return true;
}

bool Close_Anchor_Points_Tool()
{
	if(!anchor_points_tool_open)return false;
	Close_Anchor_Points_Dlg();
	render_anchor_points_tool = false;
	render_anchor_points_2d = false;
	anchor_points_tool_open = false;
	return true;
}

//we lost an anchor point, see if any other exists on the layer, otherwise, kill it
bool Try_Remove_Layer_Flag_Has_Anchor_Points(int layer_id)
{
	int n = anchor_points.size();
	for(int i = 0;i<n;i++)
	{
		if(anchor_points[i]->layer_id==layer_id)
		{
			//found another one, the layer still has at least one anchor point
			return false;
		}
	}
	//none found, clear the flag
	Set_Layer_Has_Anchor_Points(layer_id, false);
	return true;
}

//call before clearing all
bool Remove_All_Layers_Flag_Has_Anchor_Points()
{
	int n = anchor_points.size();
	for(int i = 0;i<n;i++)
	{
		Set_Layer_Has_Anchor_Points(anchor_points[i]->layer_id, false);
	}
	return true;
}

//call after loading a bunch
bool Set_All_Layers_Flag_Has_Anchor_Points()
{
	int n = anchor_points.size();
	for(int i = 0;i<n;i++)
	{
		Set_Layer_Has_Anchor_Points(anchor_points[i]->layer_id, true);
	}
	return true;
}


bool Render_Anchor_Points_2D()
{
	int n = anchor_points.size();

	glColor3f(0.2f,0.8f,0.2f);
	for(int i = 0;i<n;i++)
	{
		//single points are ok to re-render, avoid re-doing circle
		if(anchor_points[i]==selected_anchor_point)
		{
			glColor3f(1,0,0);//set highlight color
			Render_Anchor_Range(anchor_points[i]->pos2d[0], anchor_points[i]->pos2d[1], anchor_points[i]->current_influence_range);
			glColor3f(0.2f,0.8f,0.2f);//set it back
		}
		else
		{
			Render_Anchor_Range(anchor_points[i]->pos2d[0], anchor_points[i]->pos2d[1], anchor_points[i]->current_influence_range);
		}
	}
	glColor3f(1,1,1);

	return true;
}

bool Clear_Anchor_Points()
{
	Remove_All_Layers_Flag_Has_Anchor_Points();
	int n = anchor_points.size();
	for(int i = 0;i<n;i++)
	{
		delete anchor_points[i];
	}
	redraw_frame = true;
	redraw_edit_window = true;
	anchor_points.clear();
	selected_anchor_point = 0;
	Set_Displayed_Anchor_Range(default_anchor_range);
	Set_Displayed_Anchor_Type(default_anchor_point_falloff_type);
	return true;
}

bool Set_Anchor_Influence_Range(float v)
{
	if(selected_anchor_point)
	{
		selected_anchor_point->current_influence_range = v;
		redraw_edit_window = true;
		if(update_anchor_points_in_realtime)
		{
			Update_Affected_Layers_With_Selected_Anchor_Point();
		}
		if(selected_anchor_point->frames_data)
		{
			int f = Get_Current_Project_Frame();
			selected_anchor_point->frames_data[f].influence_range = selected_anchor_point->current_influence_range;
		}
	}
	else
	{
		default_anchor_range = v;
	}
	return true;
}

bool Set_Default_Anchor_Influence_Range(float v)
{
	default_anchor_range = v;
	return true;
}


bool Move_Selected_Anchor_Point_Depth(float v)
{
	if(!selected_anchor_point)
	{
		return false;
	}
	v = v/1000;
	if(selected_anchor_point->current_depth+v<0)
	{
		return false;
	}
	selected_anchor_point->current_depth+=v;
	float *fv = frame->Get_Frame_Vector(selected_anchor_point->px, selected_anchor_point->py);
	float vec[3] = {fv[0],fv[1],fv[2]};
	SetLength(vec, selected_anchor_point->current_depth);
	selected_anchor_point->pos3d[0] = frame->view_origin[0]+vec[0];
	selected_anchor_point->pos3d[1] = frame->view_origin[1]+vec[1];
	selected_anchor_point->pos3d[2] = frame->view_origin[2]+vec[2];
	redraw_frame = true;
	if(update_anchor_points_in_realtime)
	{
		Update_Affected_Layers_With_Selected_Anchor_Point();
	}
	if(selected_anchor_point->frames_data)
	{
		int f = Get_Current_Project_Frame();
		selected_anchor_point->frames_data[f].depth = selected_anchor_point->current_depth;
	}
	return true;
}

bool Apply_Anchor_Point(ANCHOR_POINT *ap)
{
	int total = 0;
	
	CIRCULAR_PIXEL_MAP *cpm = Get_Circular_Pixel_Map(ap->current_influence_range*2);
	CIRCULAR_FALLOFF_MAP *cfm = Get_Circular_Falloff_Map(ap->current_influence_range*2, ap->falloff_type);
	
	int px, py;
	int *map = cpm->pixel_map;
	float *falloff = cfm->falloff_map;
	float *fp = frame->Get_Pos(ap->px, ap->py);
	float disp = ap->pos3d[2]-fp[2];
	for(int i = 0;i<cpm->total;i++)
	{
		px = ap->px+map[i*2];
		py = ap->py+map[(i*2)+1];
		if(frame->IsValidPixel(px, py))
		{
			if(frame->Get_Pixel_Layer(px, py)==ap->layer_id)
			{
				float *p = frame->Get_Pos(px, py);
				float fa = 1.0f-falloff[i];
				float olddepth = frame->Get_Pixel_Depth(px,py);;
				float newdepth = olddepth+(disp*fa);//(olddepth*fp)+(ap->depth*(1.0f-fp));
				frame->Set_Pixel_Depth(px, py, newdepth);
			}
		}
	}
	return true;
}

bool Apply_Anchor_Points_To_Layer(int layer_id)
{
	int n = anchor_points.size();
	for(int i = 0;i<n;i++)
	{
		if(anchor_points[i]->layer_id==layer_id)
		{
			Apply_Anchor_Point(anchor_points[i]);
		}
	}
	return true;
}




bool Apply_Anchor_Point_To_Projected_Vertex(ANCHOR_POINT *ap, int x, int y, float *pos)
{
	float disp = ap->pos3d[2]-pos[2];
	float falloff = Get_Circular_Falloff(ap->current_influence_range*2, ap->falloff_type, ap->px-x, ap->py-y);
	float fa = 1.0f-falloff;
	float vec[3];
	vec[0] = pos[0]-frame->view_origin[0];
	vec[1] = pos[1]-frame->view_origin[1];
	vec[2] = pos[2]-frame->view_origin[2];
	float olddepth = MATH_UTILS::VecLength(vec);
	float newdepth = olddepth+(disp*fa);
	MATH_UTILS::SetLength(vec, newdepth);
	pos[0] = frame->view_origin[0]+vec[0];
	pos[1] = frame->view_origin[1]+vec[1];
	pos[2] = frame->view_origin[2]+vec[2];
	return true;
}


bool Apply_Anchor_Points_To_Projected_Vertex(int layer_id, int x, int y, float *pos)
{
	int n = anchor_points.size();
	for(int i = 0;i<n;i++)
	{
		if(anchor_points[i]->layer_id==layer_id)
		{
			Apply_Anchor_Point_To_Projected_Vertex(anchor_points[i], x, y, pos);
		}
	}
	return true;
}


bool Apply_Anchor_Points()
{
	int n = anchor_points.size();
	int i;
	for(i = 0;i<n;i++)
	{
		ReProject_Layer(Get_Layer_Index(anchor_points[i]->layer_id));
	}
	return true;
}

bool Update_Affected_Layers_With_Anchor_Points()
{
	Apply_Anchor_Points();
	redraw_frame = true;
	return true;
}

bool Update_Affected_Layers_With_Selected_Anchor_Point()
{
	if(!selected_anchor_point)return false;
	ReProject_Layer(Get_Layer_Index(selected_anchor_point->layer_id));
	redraw_frame = true;
	return true;
}


bool Set_Anchor_Point_Falloff(int type)
{
	if(selected_anchor_point)
	{
		selected_anchor_point->falloff_type = type;
	}
	default_anchor_point_falloff_type = type;
	return true;
}

bool Attach_Anchor_Point_To_Selected_Feature_Point()
{
	int id = Get_Selected_Feature_Point_ID();
	if(id==-1)
	{
		SkinMsgBox("No selected feature point, can't attach anchor point");
		return false;
	}
	ANCHOR_POINT *ap = new ANCHOR_POINT;
	anchor_points.push_back(ap);
	selected_anchor_point = anchor_points[anchor_points.size()-1];
	ap->feature_point_id = id;

	float pos2d[3];
	//get the initial depth from the current frame pixel depth
	Get_Feature_Point_2D_Position(Find_Feature_Point_Index(ap->feature_point_id), pos2d);
	int px = (int)pos2d[0];//get the pixel coordinates
	int py = (int)pos2d[1];
	if(frame->IsValidPixel(px, py))
	{
		ap->current_depth = frame->Get_Pixel_Depth(px, py);
	}
	else
	{
		ap->current_depth = 0;
	}
	ap->Update_Positions();
	ap->Update_Affected_Layer();
	ap->Allocate_Frames_Data();
	redraw_frame = true;
	redraw_edit_window = true;
	Update_Selected_Anchor_Point();
	return true;
}

bool Remove_Anchor_Point_From_Selected_Feature_Point()
{
	int fid = Get_Selected_Feature_Point_ID();
	if(fid!=-1)
	{
		int n = anchor_points.size();
		for(int i = 0;i<n;i++)
		{
			if(anchor_points[i]->feature_point_id==fid)
			{
				int layer_id = anchor_points[i]->layer_id;
				delete anchor_points[i];
				anchor_points.erase(anchor_points.begin()+i);
				Try_Remove_Layer_Flag_Has_Anchor_Points(layer_id);
				Update_Selected_Anchor_Point();
				Flag_Layer_For_ReProject(Find_Layer_Index(layer_id));
				redraw_frame = true;
				redraw_edit_window = true;
				return true;
			}
		}
	}
	return false;
}

bool Remove_All_Anchor_Points()
{
	Clear_Anchor_Points();
	Update_Selected_Anchor_Point();
	return false;
}

bool AnchorPointMayHaveMoved(void *p)
{
	if(selected_anchor_point)
	{
		FeaturePointInfo *fpi = (FeaturePointInfo*)p;
		if(selected_anchor_point->feature_point_id==fpi->unique_id)
		{
			selected_anchor_point->Update_Positions();
			selected_anchor_point->Update_Affected_Layer();
			if(update_anchor_points_in_realtime)
			{
				Update_Affected_Layers_With_Selected_Anchor_Point();
			}
		}
	}
	return true;
}


bool Update_Selected_Anchor_Point()
{
	selected_anchor_point = 0;
	int fid = Get_Selected_Feature_Point_ID();
	if(fid!=-1)
	{
		int n = anchor_points.size();
		for(int i = 0;i<n;i++)
		{
			if(anchor_points[i]->feature_point_id==fid)
			{
				selected_anchor_point = anchor_points[i];
				Set_Displayed_Anchor_Range(selected_anchor_point->current_influence_range);
				Set_Displayed_Anchor_Type(selected_anchor_point->falloff_type);
			}
		}
	}
	Enable_Anchor_Type_Selection(selected_anchor_point!=0);
	Enable_Attach_Anchor_Point_Button(fid!=-1&&!selected_anchor_point);
	Enable_Remove_Anchor_Point_Button(fid!=-1&&selected_anchor_point);
	Enable_Remove_All_Anchor_Points_Button(anchor_points.size()>0);
	return true;
}


bool AnchorPointSelectionChanged(void *p)
{
	Update_Selected_Anchor_Point();
	return true;
}

bool AnchorPointMayHaveBeenDeleted(void *p)
{
	FeaturePointInfo *fpi = (FeaturePointInfo*)p;
	int n = anchor_points.size();
	selected_anchor_point = 0;
	for(int i = 0;i<n;i++)
	{
		if(anchor_points[i]->feature_point_id==fpi->unique_id)
		{
			int layer_id = anchor_points[i]->layer_id;
			delete[] anchor_points[i];
			anchor_points.erase(anchor_points.begin()+i);
			Try_Remove_Layer_Flag_Has_Anchor_Points(layer_id);
			Update_Selected_Anchor_Point();
			return true;
		}
	}
	return true;
}

bool ClearAnchorPoints(void *p)
{
	Clear_Anchor_Points();
	Update_Selected_Anchor_Point();
	return true;
}


struct ANCHOR_POINT_DATA
{
	float depth;
	float influence_range;
	int falloff_type;
	int feature_point_id;
};

struct ANCHOR_POINTS_HEADER
{
	int version;
	int num_anchor_points;
	int num_frames;
};

#define ANCHOR_POINTS_FILE_VERSION 39

bool Save_Anchor_Points(char *file)
{
	FILE *f = fopen(file, "wb");
	if(!f)
	{
		return false;
	}
	ANCHOR_POINTS_HEADER aph;
	aph.num_anchor_points = anchor_points.size();
	aph.version = ANCHOR_POINTS_FILE_VERSION;
	aph.num_frames = Get_Num_Frames();
	fwrite(&aph, 1, sizeof(ANCHOR_POINTS_HEADER), f);
	int i, j;
	if(aph.num_anchor_points>0)
	{
		ANCHOR_POINT_DATA *apd = new ANCHOR_POINT_DATA[aph.num_anchor_points];
		for(i = 0;i<aph.num_anchor_points;i++)
		{
			apd[i].depth = anchor_points[i]->current_depth;
			apd[i].falloff_type = anchor_points[i]->falloff_type;
			apd[i].feature_point_id = anchor_points[i]->feature_point_id;
			apd[i].influence_range = anchor_points[i]->current_influence_range;
		}
		fwrite(apd, 1, sizeof(ANCHOR_POINT_DATA)*aph.num_anchor_points, f);
		delete[] apd;
		if(aph.num_frames>0)
		{
			ANCHOR_POINT_KEYFRAMED_DATA *apkd = new ANCHOR_POINT_KEYFRAMED_DATA[aph.num_frames*aph.num_anchor_points];
			for(i = 0;i<aph.num_anchor_points;i++)
			{
				for(j = 0;j<aph.num_frames;j++)
				{
					//store sequences of frames data for each anchor point
					apkd[(i*aph.num_frames)+j].depth = anchor_points[i]->frames_data[j].depth;
					apkd[(i*aph.num_frames)+j].influence_range = anchor_points[i]->frames_data[j].influence_range;
				}
			}
			fwrite(apkd, 1, sizeof(ANCHOR_POINT_KEYFRAMED_DATA)*aph.num_anchor_points*aph.num_frames, f);
			delete[] apkd;
		}
	}
	fclose(f);
	return true;
}

bool Load_Anchor_Points(char *file)
{
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		return false;
	}
	ANCHOR_POINTS_HEADER aph;
	
	fread(&aph, 1, sizeof(ANCHOR_POINTS_HEADER), f);

	if(aph.version!=ANCHOR_POINTS_FILE_VERSION)
	{
		SkinMsgBox("Error loading anchor points, version mis-match!", "This never happens");
		fclose(f);
		return false;
	}
	
	int i, j;
	int ap_offset = anchor_points.size();
	if(aph.num_anchor_points>0)
	{
		ANCHOR_POINT_DATA *apd = new ANCHOR_POINT_DATA[aph.num_anchor_points];
		fread(apd, 1, sizeof(ANCHOR_POINT_DATA)*aph.num_anchor_points, f);
		for(i = 0;i<aph.num_anchor_points;i++)
		{
			ANCHOR_POINT *ap = new ANCHOR_POINT;
			ap->current_depth = apd[i].depth;
			ap->falloff_type = apd[i].falloff_type;
			ap->feature_point_id = apd[i].feature_point_id;
			ap->current_influence_range = apd[i].influence_range;
			anchor_points.push_back(ap);
		}
		delete[] apd;
		if(aph.num_frames>0)
		{
			ANCHOR_POINT_KEYFRAMED_DATA *apkd = new ANCHOR_POINT_KEYFRAMED_DATA[aph.num_frames*aph.num_anchor_points];
			fread(apkd, 1, sizeof(ANCHOR_POINT_KEYFRAMED_DATA)*aph.num_anchor_points*aph.num_frames, f);
			for(i = 0;i<aph.num_anchor_points;i++)
			{
				anchor_points[i+ap_offset]->Allocate_Frames_Data();
				for(j = 0;j<aph.num_frames;j++)
				{
					//store sequences of frames data for each anchor point
					anchor_points[i+ap_offset]->frames_data[j].depth = apkd[(i*aph.num_frames)+j].depth;
					anchor_points[i+ap_offset]->frames_data[j].influence_range = apkd[(i*aph.num_frames)+j].influence_range;
				}
			}
			delete[] apkd;
		}
	}
	fclose(f);
	return true;
}


bool LoadAnchorPoints(void *p)
{
	char file[512];
	if(!Get_Project_Aux_Data_Filename(file, "anc"))
	{
		return false;
	}
	Load_Anchor_Points(file);
	int n = anchor_points.size();
	for(int i = 0;i<n;i++)
	{
		anchor_points[i]->Interpolate_Keyframe_Data();
		anchor_points[i]->Update_Positions();
		anchor_points[i]->Update_Affected_Layer();
	}
	Update_Selected_Anchor_Point();
	return true;
}

bool SaveAnchorPoints(void *p)
{
	char file[512];
	if(!Get_Project_Aux_Data_Filename(file, "anc"))
	{
		return false;
	}
	Save_Anchor_Points(file);
	return true;
}


bool UpdateAnchorPointsForCurrentFrame(void *p)
{
	Set_All_Layers_Flag_Has_Anchor_Points();
	int n = anchor_points.size();
	for(int i = 0;i<n;i++)
	{
		anchor_points[i]->Interpolate_Keyframe_Data();
		anchor_points[i]->Update_Positions();
		anchor_points[i]->Update_Affected_Layer();
	}
	Update_Selected_Anchor_Point();
	return true;
}

bool UnFlagAnchorPointsForCurrentFrame(void *p)
{
	Remove_All_Layers_Flag_Has_Anchor_Points();
	return true;
}

//catch all possible events to auto load and save anchor point data for any and all frames
//maybe change this in the future to ask?
bool Register_AnchorPoints_SharedData_Events()
{
	shared_data_interface.Register_Shared_Data_Update_Callback("FeaturePointMoved", AnchorPointMayHaveMoved);
	shared_data_interface.Register_Shared_Data_Update_Callback("FeaturePointSelectionChanged", AnchorPointSelectionChanged);
	shared_data_interface.Register_Shared_Data_Update_Callback("FeaturePointDeleted", AnchorPointMayHaveBeenDeleted);
	shared_data_interface.Register_Shared_Data_Update_Callback("FeaturePointsCleared", ClearAnchorPoints);
	shared_data_interface.Register_Shared_Data_Update_Callback("FeaturePointsLoaded", LoadAnchorPoints);
	shared_data_interface.Register_Shared_Data_Update_Callback("FeaturePointsSaved", SaveAnchorPoints);
	shared_data_interface.Register_Shared_Data_Update_Callback("FrameAboutToChange", UnFlagAnchorPointsForCurrentFrame);
	shared_data_interface.Register_Shared_Data_Update_Callback("FrameChanged", UpdateAnchorPointsForCurrentFrame);
	return false;
}
