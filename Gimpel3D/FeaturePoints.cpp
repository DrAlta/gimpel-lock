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
#include "FeaturePoints.h"
#include "CircularMaps.h"
#include "G3DCoreFiles/Frame.h"
#include "G3DCoreFiles/KLTInterface.h"
#include "G3DCoreFiles/ProjectFile.h"
#include "LinkPointsTool.h"
#include <gl\gl.h>
#include <vector>
#include "Skin.h"


extern bool anchor_points_tool_open;

extern bool virtual_camera_tool_open;
bool Notify_Virtual_Camera_Tool_Changed_Feature_Point();
bool Update_Selected_Feature_Point_GUI();

bool Update_Virtual_Camera_Tracked_Feature_Point_Indices();
bool Update_Link_Tool_Tracked_Feature_Point_Indices();

bool Report_Feature_Point_Added(int id);
bool Report_Feature_Point_Deleted(int id);
bool Report_Feature_Point_Moved(int id);
bool Report_Feature_Point_Selection_Changed(int id);
bool Report_Feature_Points_Cleared();
bool Report_Feature_Points_Loaded();
bool Report_Feature_Points_Saved();


using namespace std;

float Pixel_Size();

const int feature_points_file_version = 3;

bool click_to_add_feature_points = false;
bool click_to_delete_feature_points = false;
bool click_to_move_feature_points = true;

bool render_feature_points_tool = false;
bool render_feature_points_2d = false;
bool render_feature_points_3d = false;

bool track_feature_points = false;

int feature_point_range = 10;

//IMPORTANT!

//all access through feature point functions uses direct indices..
//if the indices change, the calling code must re-index the points (for their internal data)
//external modules using feature points should ALWAYS save the id, NOT the index
//failure to comply may result in incorrect or broken linkage between feature points and external modules

//use unique ids for each feature point in case some get deleted at any point
int last_feature_point_id = 0;

//this gets reset on clear points, and also reset to the value of the very last feature point
//when loaded from file, as it will have the highest value

class FEATURE_POINT
{
public:
	FEATURE_POINT()
	{
		frame_positions = 0;
		keyframes = 0;
		unique_id = -1;
	}
	~FEATURE_POINT()
	{
		if(frame_positions)delete[] frame_positions;
		if(keyframes)delete[] keyframes;
	}
	float *frame_positions;//for all frames
	bool *keyframes;
	float px, py;//for current frame
	int unique_id;
};

vector<FEATURE_POINT*> feature_points;

FEATURE_POINT *clicked_feature_point = 0;
float feature_point_xclick_off = 0;
float feature_point_yclick_off = 0;

int Find_Feature_Point_Index(int unique_id)
{
	int n = feature_points.size();
	for(int i = 0;i<n;i++)
	{
		if(feature_points[i]->unique_id==unique_id)
		{
			return i;
		}
	}
	return -1;
}

int Get_Feature_Point_Unique_ID(int index)
{
	int n = feature_points.size();
	if(index<0||index>=n)
	{
		return -1;
	}
	return feature_points[index]->unique_id;
}

__forceinline bool Try_Select_Feature_Point(FEATURE_POINT *fp, float x, float y, float range)
{
	float dx = x-fp->px;if(dx<0)dx = -dx;
	float dy = y-fp->py;if(dy<0)dy = -dy;
	if(dx>range||dy>range)return false;
	feature_point_xclick_off = x-fp->px;
	feature_point_yclick_off = y-fp->py;
	return true;
}

bool Find_Clicked_Feature_Point(float px, float py)
{
	float range = 5*(1.0f/Pixel_Size());
	clicked_feature_point = 0;
	int n = feature_points.size();
	for(int i = 0;i<n;i++)
	{
		if(Try_Select_Feature_Point(feature_points[i], px, py, range))
		{
			clicked_feature_point = feature_points[i];
			return true;
		}
	}
	return false;
}

bool Delete_Clicked_Feature_Point(float px, float py)
{
	float range = 5*(1.0f/Pixel_Size());
	int n = feature_points.size();
	for(int i = 0;i<n;i++)
	{
		if(Try_Select_Feature_Point(feature_points[i], px, py, range))
		{
			int id = feature_points[i]->unique_id;
			delete feature_points[i];
			feature_points.erase(feature_points.begin()+i);
			Update_Virtual_Camera_Tracked_Feature_Point_Indices();
			Update_Link_Tool_Tracked_Feature_Point_Indices();
			Report_Feature_Point_Deleted(id);
			redraw_frame = true;
			redraw_edit_window = true;
			return true;
		}
	}
	return false;
}


bool Render_Feature_Points_2D()
{
	int n = feature_points.size();
	glPointSize(10);
	glColor3f(0.2f,0.8f,0.2f);
	glBegin(GL_POINTS);
	for(int i = 0;i<n;i++)
	{
		glVertex2f(feature_points[i]->px, feature_points[i]->py);
	}
	if(clicked_feature_point)
	{
		glColor3f(1,0,0);
		glVertex2f(clicked_feature_point->px, clicked_feature_point->py);
	}
	glEnd();
	glColor3f(1,1,1);
	glPointSize(1);
	return true;
}

bool Render_Feature_Points_3D()
{
	int n = feature_points.size();
	glPointSize(10);
	glColor3f(0.2f,0.8f,0.2f);
	glBegin(GL_POINTS);
	for(int i = 0;i<n;i++)
	{
		int px = (int)feature_points[i]->px;
		int py = (int)feature_points[i]->py;
		if(frame->IsValidPixel(px, py))
		{
			if(clicked_feature_point==feature_points[i])
			{
				glColor3f(1,0,0);
				glVertex3fv(frame->Get_Pos(px, py));
				glColor3f(0.2f,0.8f,0.2f);
			}
			else
			{
				glVertex3fv(frame->Get_Pos(px, py));
			}
		}
	}
	glEnd();
	glColor3f(1,1,1);
	glPointSize(1);
	return true;
}


bool Add_Feature_Point(float x, float y)
{
	int px = (int)x;
	int py = (int)y;
	if(!frame->IsValidPixel(px, py))
	{
		return false;
	}
	FEATURE_POINT *fp = new FEATURE_POINT;
	feature_points.push_back(fp);
	clicked_feature_point = fp;
	clicked_feature_point->px = x;
	clicked_feature_point->py = y;
	//get a sequential id for this point
	clicked_feature_point->unique_id = last_feature_point_id;
	last_feature_point_id++;
	int num_frames = Get_Num_Frames();
	if(num_frames>0)
	{
		fp->frame_positions = new float[num_frames*2];
		fp->keyframes = new bool[num_frames];
	}
	for(int i = 0;i<num_frames;i++)
	{
		fp->frame_positions[i*2] = x;
		fp->frame_positions[(i*2)+1] = y;
		fp->keyframes[i] = false;
	}
	if(fp->keyframes)
	{
		//only for sequences, no keyframe buffer in single frame sessions
		fp->keyframes[Get_Current_Project_Frame()] = true;
		//make sure first and last frame are keyframed
		fp->keyframes[0] = true;
		fp->keyframes[num_frames-1] = true;
	}
	redraw_frame = true;
	redraw_edit_window = true;
	Update_Selected_Feature_Point_GUI();
	Report_Feature_Point_Added(clicked_feature_point->unique_id);
	Report_Feature_Point_Selection_Changed(clicked_feature_point->unique_id);
	return true;
}

bool Click_Feature_Points_Tool(float x, float y)
{
	FEATURE_POINT *ofp = clicked_feature_point;
	if(click_to_add_feature_points)
	{
		Add_Feature_Point(x, y);
	}
	else if(click_to_delete_feature_points)
	{
		clicked_feature_point = 0;
		Delete_Clicked_Feature_Point(x, y);
	}
	else if(click_to_move_feature_points)
	{
		Find_Clicked_Feature_Point(x, y);
		if(link_points_tool_open)
		{
			Notify_Link_Point_Tool_Changed_Feature_Point();
		}
		if(virtual_camera_tool_open)
		{
			Notify_Virtual_Camera_Tool_Changed_Feature_Point();
		}
	}
	if(ofp!=clicked_feature_point)
	{
		redraw_frame = true;
		if(clicked_feature_point)
		{
			Report_Feature_Point_Selection_Changed(clicked_feature_point->unique_id);
		}
		else
		{
			Report_Feature_Point_Selection_Changed(-1);
		}
	}
	Update_Selected_Feature_Point_GUI();
	return true;
}

bool Drag_Feature_Points_Tool(float x, float y)
{
	if(clicked_feature_point&&click_to_move_feature_points)
	{
		float nx = x-feature_point_xclick_off;
		float ny = y-feature_point_yclick_off;
		
		if(clicked_feature_point->px!=nx||clicked_feature_point->py!=ny)
		{
			clicked_feature_point->px = nx;
			clicked_feature_point->py = ny;
			Report_Feature_Point_Moved(clicked_feature_point->unique_id);
			if(clicked_feature_point->frame_positions)
			{
				int f = Get_Current_Project_Frame();
				clicked_feature_point->frame_positions[f*2] = clicked_feature_point->px;
				clicked_feature_point->frame_positions[(f*2)+1] = clicked_feature_point->py;
			}
		}
		redraw_frame = true;
		if(link_points_tool_open)
		{
			redraw_frame = true;
		}
		if(virtual_camera_tool_open)
		{
			redraw_frame = true;
		}
		if(anchor_points_tool_open)
		{
			redraw_frame = true;
		}
	}
	return true;
}

int Num_Feature_Points()
{
	return feature_points.size();
}

bool Clear_Feature_Points()
{
	int n = feature_points.size();
	for(int i = 0;i<n;i++)
	{
		delete feature_points[i];
	}
	feature_points.clear();
	clicked_feature_point = 0;
	//reset the sequential id
	last_feature_point_id = 0;
	Update_Virtual_Camera_Tracked_Feature_Point_Indices();
	Update_Link_Tool_Tracked_Feature_Point_Indices();
	Report_Feature_Points_Cleared();
	return true;
}

bool feature_points_tool_open = false;

bool Open_Feature_Points_Tool()
{
	if(feature_points_tool_open)return true;
	Open_Feature_Points_Dlg();
	render_feature_points_tool = true;
	render_feature_points_2d = true;
	render_feature_points_3d = true;
	redraw_edit_window = true;
	redraw_frame = true;
	int current_frame = Get_Current_Project_Frame();
	char file[512];
	Get_Frame_Image(current_frame, file);
	if(Get_Num_Frames()>0)
	{
		Reset_KLT_Tracking(file);
	}
	feature_points_tool_open = true;
	Update_Feature_Points_Tracking();
	return true;
}

bool Close_Feature_Points_Tool()
{
	Close_Feature_Points_Dlg();
	render_feature_points_tool = false;
	render_feature_points_2d = false;
	render_feature_points_3d = false;
	redraw_edit_window = true;
	redraw_frame = true;
	if(Get_Num_Frames()>0)
	{
		Stop_KLT_Tracking();
	}
	feature_points_tool_open = false;
	return true;
}

//store the keyframed pixels for the current frame
bool Store_Feature_Points()
{
	int n = feature_points.size();
	int f = Get_Current_Project_Frame();
	for(int i = 0;i<n;i++)
	{
		feature_points[i]->frame_positions[(f*2)] = feature_points[i]->px;
		feature_points[i]->frame_positions[(f*2)+1] = feature_points[i]->py;
	}
	return true;
}

//set the pixels for the current frame
bool Set_Feature_Points()
{
	int n = feature_points.size();
	int f = Get_Current_Project_Frame();
	for(int i = 0;i<n;i++)
	{
		feature_points[i]->px = feature_points[i]->frame_positions[(f*2)];
		feature_points[i]->py = feature_points[i]->frame_positions[(f*2)+1];
	}
	return true;
}

int Get_Next_Feature_Point_Keyframe(int fpi, int frame)
{
	int n = Get_Num_Frames();
	if(frame<0||frame>=n)return -1;
	//verify keyframes exist (only used for sequences)
	if(!feature_points[fpi]->keyframes)
	{
		return -1;//keyframes don't exist in a single frame session
	}
	for(int i = frame+1;i<n;i++)
	{
		if(feature_points[fpi]->keyframes[i])
		{
			return i;
		}
	}
	return -1;
}

int Get_Previous_Feature_Point_Keyframe(int fpi, int frame)
{
	int n = Get_Num_Frames();
	if(frame<0||frame>=n)return -1;
	//verify keyframes exist (only used for sequences)
	if(!feature_points[fpi]->keyframes)
	{
		return -1;//keyframes don't exist in a single frame session
	}
	for(int i = frame-1;i>-1;i--)
	{
		if(feature_points[fpi]->keyframes[i])
		{
			return i;
		}
	}
	return -1;
}

bool Feature_Point_Is_Keyframed(int fpi, int frame)
{
	int n = Get_Num_Frames();
	if(frame<0||frame>=n)return -1;
	//verify keyframes exist (only used for sequences)
	if(!feature_points[fpi]->keyframes)
	{
		return true;//keyframes don't exist in a single frame session, everything is "key"
	}
	return feature_points[fpi]->keyframes[frame];
}


bool Get_Feature_Points_At_Frame(int fpi, int f, float *rx, float *ry)
{
	if(!feature_points[fpi]->keyframes)
	{
		//single frame session, no keyframes buffer, just using default
		*rx = feature_points[fpi]->px;//no frame positions buffer either
		*ry = feature_points[fpi]->py;
		return true;
	}
	if(feature_points[fpi]->keyframes[f])
	{
		*rx = feature_points[fpi]->frame_positions[f*2];
		*ry = feature_points[fpi]->frame_positions[(f*2)+1];
		return true;
	}
	int next = Get_Next_Feature_Point_Keyframe(fpi, f);
	int previous = Get_Previous_Feature_Point_Keyframe(fpi, f);
	float p, ip;
	if(next==-1&&previous==-1)
	{
		*rx = feature_points[fpi]->px;
		*ry = feature_points[fpi]->py;
		return false;
	}
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
		p = ((float)(f-previous))/((float)next-previous);
		ip = 1.0f-p;
	}
	float nx, ny, px, py;
	nx = feature_points[fpi]->frame_positions[next*2];
	ny = feature_points[fpi]->frame_positions[(next*2)+1];
	px = feature_points[fpi]->frame_positions[previous*2];
	py = feature_points[fpi]->frame_positions[(previous*2)+1];
	*rx = (nx*p)+(px*ip);
	*ry = (ny*p)+(py*ip);
	return true;
}

//interpolate from the nearest keyframes
bool Interpolate_Feature_Points(int f)
{
	int n = feature_points.size();
	for(int i = 0;i<n;i++)
	{
		Get_Feature_Points_At_Frame(i, f, &feature_points[i]->px, &feature_points[i]->py);
	}
	return true;
}

bool Update_Feature_Points_Tracking()
{
	if(!render_feature_points_tool)
	{
		return false;
	}
	Update_Selected_Feature_Point_GUI();
	if(!track_feature_points)
	{
		Interpolate_Feature_Points(Get_Current_Project_Frame());
		return false;
	}
	CIRCULAR_PIXEL_MAP *cpm = Get_Circular_Pixel_Map(feature_point_range);

	vector<float> old_pixels;
	vector<float> new_pixels;
	int n = feature_points.size();
	int nrp = cpm->total;
	int i, j;
	for(i = 0;i<n;i++)
	{
		for(j = 0;j<nrp;j++)
		{
			old_pixels.push_back(feature_points[i]->px+cpm->pixel_map[(j*2)]);
			old_pixels.push_back(feature_points[i]->py+cpm->pixel_map[(j*2)+1]);
		}
	}
	Track_KLT_Points(&old_pixels, &new_pixels);
	float tx, ty;
	float tpx, tpy;
	float mx, my;
	int cnt;
	int k = 0;
	float max_motion = 5;
	for(i = 0;i<n;i++)
	{
		cnt = 0;
		tpx = 0;
		tpy = 0;
		for(j = 0;j<nrp;j++)
		{
			tx = new_pixels[k*2];
			ty = new_pixels[(k*2)+1];
			k++;
			if(tx>=0)
			{
				//get average motion
				mx = (feature_points[i]->px+cpm->pixel_map[(j*2)])-tx;
				my = (feature_points[i]->py+cpm->pixel_map[(j*2)+1])-ty;
				if(mx<max_motion&&my<max_motion)
				{
					tpx += mx;
					tpy += my;
					cnt++;
				}
			}
		}
		if(cnt>0)
		{
			feature_points[i]->px -= (tpx/cnt)+0.5f;
			feature_points[i]->py -= (tpy/cnt)+0.5f;
		}
	}
	old_pixels.clear();
	new_pixels.clear();
	return true;
}

bool Update_Selected_Feature_Point_GUI()
{
	if(!clicked_feature_point)
	{
		Set_Feature_Point_Dlg_Selected_Point_Keyframed(false, false);
	}
	else
	{
		if(clicked_feature_point->keyframes)
		{
			//only for sequences, single frame sessions will not have keyframes buffer
			Set_Feature_Point_Dlg_Selected_Point_Keyframed(true, clicked_feature_point->keyframes[Get_Current_Project_Frame()]);
		}
	}
	return true;
}

bool Keyframe_Selected_Feature_Point(bool b)
{
	if(!clicked_feature_point)
	{
		return false;
	}
	clicked_feature_point->keyframes[Get_Current_Project_Frame()] = b;
	Update_Selected_Feature_Point_GUI();
	return true;
}

bool Clear_All_Keyframes_For_Selected_Feature_Point()
{
	if(!clicked_feature_point)
	{
		return false;
	}
	int n = Get_Num_Frames();
	for(int i = 0;i<n;i++)
	{
		clicked_feature_point->keyframes[i] = false;
	}
	Update_Selected_Feature_Point_GUI();
	return true;
}


bool Set_All_Feature_Point_Keyframes(bool b)
{
	int f = Get_Current_Project_Frame();
	int n = feature_points.size();
	for(int i = 0;i<n;i++)
	{
		if(feature_points[i]->keyframes)
		{
			//verify keyframes buffer exists
			feature_points[i]->keyframes[f] = b;
			//could be single frame session
		}
	}
	Update_Selected_Feature_Point_GUI();
	return true;
}

bool Get_Feature_Points_Filename_Single_Frame(char *file, char *res)
{
	strcpy(res, file);
	char *c = strrchr(res, '.');
	if(!c)return false;
	strcpy(c, ".fpt");
	return true;
}

bool Get_Feature_Points_Filename_Sequence(char *file, char *res)
{
	strcpy(res, file);
	char *c = strrchr(res, '.');
	if(!c)return false;
	strcpy(c, ".fps");
	return true;
}

bool Load_Feature_Points_Single_Frame(char *file)
{
	char nfile[512];
	if(!Get_Feature_Points_Filename_Single_Frame(file, nfile))return false;
	FILE *f = fopen(nfile, "rb");
	if(!f)return false;
	Clear_Feature_Points();
	int version = 0;
	int n = 0;
	fread(&version, sizeof(int), 1, f);
	if(version!=feature_points_file_version)
	{
		SkinMsgBox(0, "Can't load feature points, version mismatch!", "This should never happen", MB_OK);
		fclose(f);
		return false;
	}
	fread(&n, sizeof(int), 1, f);
	float *buf = new float[n*3];//pos and id for each
	fread(buf, sizeof(float), n*3, f);
	fclose(f);
	FEATURE_POINT *fp;
	for(int i = 0;i<n;i++)
	{
		fp = new FEATURE_POINT;
		fp->px = buf[i*3];
		fp->py = buf[(i*3)+1];
		fp->unique_id = (int)buf[(i*3)+2];
		feature_points.push_back(fp);
		last_feature_point_id = fp->unique_id+1;
	}
	delete[] buf;
	Report_Feature_Points_Loaded();
	return true;
}

bool Load_Feature_Points_Sequence(char *file)
{
	char nfile[512];
	if(!Get_Feature_Points_Filename_Sequence(file, nfile))return false;
	FILE *f = fopen(nfile, "rb");
	if(!f)return false;
	Clear_Feature_Points();
	int version = 0;
	int num_points = 0;
	int num_frames = 0;
	fread(&version, sizeof(int), 1, f);
	if(feature_points_file_version!=version)
	{
		SkinMsgBox("Error loading feature points, file version mismatch!", "This never happens");
		fclose(f);
		return false;
	}

	fread(&num_points, sizeof(int), 1, f);//number of feature points
	fread(&num_frames, sizeof(int), 1, f);//number of frames

	if(num_frames!=Get_Num_Frames())
	{
		SkinMsgBox("Error loading feature points, frame count mismatch!", "This never happens");
		fclose(f);
		return false;
	}

	float *buf = new float[num_points*3];
	fread(buf, sizeof(float), num_points*3, f);//current positions
	int i;
	for(i = 0;i<num_points;i++)
	{
		FEATURE_POINT *fp = new FEATURE_POINT;
		feature_points.push_back(fp);
		fp->px = buf[i*3];
		fp->py = buf[(i*3)+1];
		fp->unique_id = (int)buf[(i*3)+2];
		last_feature_point_id = fp->unique_id+1;
		fp->frame_positions = new float[num_frames*2];
		fp->keyframes = new bool[num_frames];
	}
	delete[] buf;
	for(i = 0;i<num_points;i++)
	{
		//position buffers for all frames
		fread(feature_points[i]->frame_positions, sizeof(float), num_frames*2, f);//frame positions
		fread(feature_points[i]->keyframes, sizeof(bool), num_frames, f);//frame keyframes
	}
	
	fclose(f);
	Report_Feature_Points_Loaded();
	return true;
}

bool Save_Feature_Points_Single_Frame(char *file)
{
	char nfile[512];
	if(!Get_Feature_Points_Filename_Single_Frame(file, nfile))return false;
	if(feature_points.size()==0)
	{
		remove(nfile);
		return true;
	}
	FILE *f = fopen(nfile, "wb");
	if(!f)return false;
	int n = feature_points.size();
	fwrite(&feature_points_file_version, sizeof(int), 1, f);
	fwrite(&n, sizeof(int), 1, f);
	float *buf = new float[n*3];
	for(int i = 0;i<n;i++)
	{
		buf[i*3] = feature_points[i]->px;
		buf[(i*3)+1] = feature_points[i]->py;
		buf[(i*3)+2] = feature_points[i]->unique_id;
	}
	fwrite(buf, sizeof(float), n*3, f);
	fclose(f);
	delete[] buf;
	Report_Feature_Points_Saved();
	return true;
}

bool Save_Feature_Points_Sequence(char *file)
{
	char nfile[512];
	if(!Get_Feature_Points_Filename_Sequence(file, nfile))return false;
	if(feature_points.size()==0)
	{
		remove(nfile);
		return true;
	}
	FILE *f = fopen(nfile, "wb");
	if(!f)return false;
	int n = feature_points.size();
	int nf = Get_Num_Frames();
	fwrite(&feature_points_file_version, sizeof(int), 1, f);
	fwrite(&n, sizeof(int), 1, f);//number of feature points
	fwrite(&nf, sizeof(int), 1, f);//number of frames
	float *buf = new float[n*3];
	int i;
	for(i = 0;i<n;i++)
	{
		buf[i*3] = feature_points[i]->px;//current feature points
		buf[(i*3)+1] = feature_points[i]->py;
		buf[(i*3)+2] = feature_points[i]->unique_id;
	}
	fwrite(buf, sizeof(float), n*3, f);//save current positions
	for(i = 0;i<n;i++)
	{
		//save position and keyframe buffers for all frames
		fwrite(feature_points[i]->frame_positions, sizeof(float), nf*2, f);//save frame positions
		fwrite(feature_points[i]->keyframes, sizeof(bool), nf, f);//save frame positions
	}
	fclose(f);
	delete[] buf;
	Report_Feature_Points_Saved();
	return true;
}

bool Select_Feature_Point(int index)
{
	int n = feature_points.size();
	if(index<0||index>=n)
	{
		return false;
	}
	clicked_feature_point = feature_points[index];
	return true;
}

int Get_Selected_Feature_Point_ID()
{
	if(!clicked_feature_point)return -1;
	return clicked_feature_point->unique_id;
}

bool Get_Feature_Point_2D_Position(int index, float *pos)
{
	int n = feature_points.size();
	if(index<0||index>=n)
	{
		return false;
	}
	pos[0] = feature_points[index]->px;
	pos[1] = feature_points[index]->py;
	return true;
}

bool Get_Feature_Point_2D_Position(int index, int frame, float *pos)
{
	int n = feature_points.size();
	if(index<0||index>=n)
	{
		return false;
	}
	float px = 0;
	float py = 0;
	if(!Get_Feature_Points_At_Frame(index, frame, &px, &py))return false;
	pos[0] = px;
	pos[1] = py;
	return true;
}


bool Get_Feature_Point_3D_Position(int index, float *pos)
{
	float p2[3];
	if(!Get_Feature_Point_2D_Position(index, p2)){return false;}
	int px = (int)p2[0];
	int py = (int)p2[1];
	if(!frame->IsValidPixel(px, py)){return false;}
	float *p = frame->Get_Pos(px, py);
	pos[0] = p[0];
	pos[1] = p[1];
	pos[2] = p[2];
	return true;
}

