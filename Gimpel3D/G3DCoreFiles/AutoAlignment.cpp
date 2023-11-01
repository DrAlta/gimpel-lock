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
#include "G3D.h"
#include "Frame.h"
#include "bb_matrix.h"
#include "Layers.h"
#include <gl/gl.h>
#include <algorithm>
#include "../Plugins.h"



//alignment settings

bool align_depth = false;
bool align_xrot = false;
bool align_yrot = false;
bool align_angle = false;

bool align_primitive = false;
bool align_primitive_scale_x = false;
bool align_primitive_scale_y = false;
bool align_primitive_scale_z = false;
bool align_primitive_rot_x = false;
bool align_primitive_rot_y = false;
bool align_primitive_rot_z = false;

bool align_primitive_move_x = false;
bool align_primitive_move_y = false;
bool align_primitive_move_z = false;

int alignment_layer_id = -1;
int alignment_layer_index = -1;


bool auto_apply_alignment_operation_to_other_frames = false;


//////////////////////////////////////////////////////////

//the following is used to save info about any alignment pass
//in case the user saves it to auto-apply to other frames (to help maintain edges and combat drift)

struct ALIGNMENT_INFO
{
	bool align_depth;
	bool align_xrot;
	bool align_yrot;
	bool align_angle;
	bool align_primitive;
	bool align_primitive_scale_x;
	bool align_primitive_scale_y;
	bool align_primitive_scale_z;
	bool align_primitive_rot_x;
	bool align_primitive_rot_y;
	bool align_primitive_rot_z;
	bool align_primitive_move_x;
	bool align_primitive_move_y;
	bool align_primitive_move_z;
	int alignment_layer_id;
	int num_alignment_neighbors;
	int *alignment_neighbor_ids;
};

//a list of saved alignments that would be applied to future frames
vector<ALIGNMENT_INFO*> alignment_info;

//save info about the current alignment pass that just started
bool Save_Alignment_Operation_Info()
{
	ALIGNMENT_INFO *ai = new ALIGNMENT_INFO;
	ai->align_depth = align_depth;
	ai->align_xrot = align_xrot;
	ai->align_yrot = align_yrot;
	ai->align_angle = align_angle;
	ai->align_primitive = align_primitive;
	ai->align_primitive_scale_x = align_primitive_scale_x;
	ai->align_primitive_scale_y = align_primitive_scale_y;
	ai->align_primitive_scale_z = align_primitive_scale_z;
	ai->align_primitive_rot_x = align_primitive_rot_x;
	ai->align_primitive_rot_y = align_primitive_rot_y;
	ai->align_primitive_rot_z = align_primitive_rot_z;
	ai->align_primitive_move_x = align_primitive_move_x;
	ai->align_primitive_move_y = align_primitive_move_y;
	ai->align_primitive_move_z = align_primitive_move_z;
	ai->alignment_layer_id = alignment_layer_id;
	//find selected neightbors
	int ns = Num_Selected_Layers();
	ai->num_alignment_neighbors = ns-1;
	if(ai->num_alignment_neighbors<1)
	{
		//whoops, not enough layers selected to align to, should never reach this point in the code
		delete ai;
		return false;
	}
	ai->alignment_neighbor_ids = new int[ai->num_alignment_neighbors];
	int nl = Num_Layers();
	int cnt = 0;
	for(int i = 0;i<nl;i++)
	{
		if(Layer_Is_Selected(i))
		{
			int id = Get_Layer_ID(i);
			if(id!=ai->alignment_layer_id)
			{
				ai->alignment_neighbor_ids[cnt] = id;
				cnt++;
			}
		}
	}
	if(cnt==0)
	{
		//something went wrong, no neighbor layers to select to
		delete[] ai->alignment_neighbor_ids;
		delete ai;
		return false;
	}
	alignment_info.push_back(ai);
	return false;
}

//discrete function to start iterative alignment from stored data
bool Start_Stored_Alignment(ALIGNMENT_INFO *ai);

//called after frame changed to update any pieces with pre-determined auto-alignments
bool Check_For_Pending_Alignments()
{
	int n = alignment_info.size();
	for(int i = 0;i<n;i++)
	{
		if(!Layer_Is_Keyframed(Get_Layer_Index(alignment_info[i]->alignment_layer_id)))
		{
			//layer needs alignment as it not already keyframed, set that bitch up
			Start_Stored_Alignment(alignment_info[i]);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////

bool Pixel_Within_Brush(int x, int y);

float fastsqrt(float n);
void Normalize(float *v);
void SetLength(float *v, float s);

float Pixel_Size();

#define EPSILON 0.000001

bool render_2d_autoalignment_points = false;

bool activate_alignment_points = false;
bool deactivate_alignment_points = false;

float alignment_angle = 90;
bool iterate_alignment = false;

float last_alignment_error = 0;

float last_depth_align = 10;
float last_xrot_align = 10;
float last_yrot_align = 10;

float last_primitive_position_align[3] = {10,10,10};

float last_primitive_align[3] = {10,10,10};
float last_primitive_scale_align[3] = {1,1,1};


float alignment_plane_pos[3] = {0,0,0};
float alignment_plane_dir[3] = {0,0,0};
float alignment_plane_rotation[3] = {0,0,0};
float alignment_plane_offset[3] = {0,0,0};
int alignment_plane_origin_type = -1;
float alignment_neighbor_pos[3] = {0,0,0};
float alignment_neighbor_dir[3] = {0,0,0};
float alignment_neighbor_rotation[3] = {0,0,0};
float alignment_neighbor_offset[3] = {0,0,0};
int alignment_neighbor_origin_type = -1;
float current_alignment_plane_dir[3] = {0,0,0};

struct ALIGNMENT_POINT
{
	int ix, iy;//pixel that gets modified
	float x,y,z;//desired position
	bool enabled;
};

vector<ALIGNMENT_POINT> alignment_points;
int *alignment_neighbor_counts = 0;


bool Clear_Alignment_Points()
{
	alignment_points.clear();
	return true;
}

bool Respond_To_Alignment_Selection_Changed(void *p)
{
	alignment_points.clear();
	return true;
}

bool Init_Alignment_Selection_Changed_Callback()
{
	shared_data_interface.Register_Shared_Data_Update_Callback("SelectionChanged", Respond_To_Alignment_Selection_Changed);
	return true;
}


void Render_2D_AutoAlignment_Points()
{
	int n = alignment_points.size();
	glPointSize(Pixel_Size()*2);
	glBegin(GL_POINTS);
	for(int i = 0;i<n;i++)
	{
		if(alignment_points[i].enabled)
		{
			glColor3f(0,1,0);
		}
		else
		{
			glColor3f(1,0,0);
		}
		glVertex2f(alignment_points[i].ix, alignment_points[i].iy);
	}
	glEnd();
	glColor3f(1,1,1);
	glPointSize(1);
}

bool Enable_AutoAlignment_Points_Within_Brush(bool enable)
{
	int n = alignment_points.size();
	for(int i = 0;i<n;i++)
	{
		if(Pixel_Within_Brush(alignment_points[i].ix, alignment_points[i].iy))
		{
			alignment_points[i].enabled = enable;
		}
	}
	return false;
}


bool Allocate_Alignment_Neighbor_Counts()
{
	int n = Num_Layers();
	alignment_neighbor_counts = new int[n];
	for(int i = 0;i<n;i++)
	{
		alignment_neighbor_counts[i] = 0;
	}
	return true;
}

__forceinline bool Register_Alignment_Neighbor(int index)
{
	alignment_neighbor_counts[index]++;
	return true;
}


__forceinline float Alignment_Error(ALIGNMENT_POINT *ap)
{
	float p[3];
	Project_Pixel_To_Layer(alignment_layer_index, ap->ix, ap->iy, p);
	float dx = p[0]-ap->x;
	float dy = p[1]-ap->y;
	float dz = p[2]-ap->z;
	return fastsqrt((dx*dx)+(dy*dy)+(dz*dz));
}

float Dot(float *a, float *b)
{
	return (a[0]*b[0]+a[1]*b[1]+a[2]*b[2]);
}

float Get_Alignment_Angle_Error()
{
	float d = Dot(current_alignment_plane_dir, alignment_neighbor_dir);
	if(d<0)
	{
		return -d;
	}
	return d;
}

bool Get_Alignment_Plane()
{
	Get_Indexed_Layer_Plane(alignment_layer_index, alignment_plane_pos, alignment_plane_dir, alignment_plane_rotation, alignment_plane_offset, &alignment_plane_origin_type);
	current_alignment_plane_dir[0] = alignment_plane_dir[0];
	current_alignment_plane_dir[1] = alignment_plane_dir[1];
	current_alignment_plane_dir[2] = alignment_plane_dir[2];
	return true;
}

bool ReCenter_Alignment_Plane()
{
	int n = alignment_points.size();
	if(n==0)return false;
	Get_Alignment_Plane();
	float lowest_error = 99999;
	int best_index = -1;
	for(int i = 0;i<n;i++)
	{
		if(alignment_points[i].enabled)
		{
			float t = Alignment_Error(&alignment_points[i]);
			if(t<lowest_error)
			{
				lowest_error = t;
				best_index = i;
			}
		}
	}
	if(best_index!=-1)
	{
		float pp[3];
		Project_Pixel_To_Layer(alignment_layer_index, alignment_points[best_index].ix, alignment_points[best_index].iy, pp);
		alignment_plane_pos[0] = pp[0];
		alignment_plane_pos[1] = pp[1];
		alignment_plane_pos[2] = pp[2];
	}
	Project_Indexed_Layer_To_Plane(alignment_layer_index, alignment_plane_pos, alignment_plane_dir, alignment_plane_rotation, alignment_plane_offset, alignment_plane_origin_type, false);
	return true;
}

//
bool Start_Stored_Alignment(ALIGNMENT_INFO *ai)
{
	//assign alignment flags

	align_depth = ai->align_depth;
	align_xrot = ai->align_xrot;
	align_yrot = ai->align_yrot;
	align_angle = ai->align_angle;
	align_primitive = ai->align_primitive;
	align_primitive_scale_x = ai->align_primitive_scale_x;
	align_primitive_scale_y = ai->align_primitive_scale_y;
	align_primitive_scale_z = ai->align_primitive_scale_z;
	align_primitive_rot_x = ai->align_primitive_rot_x;
	align_primitive_rot_y = ai->align_primitive_rot_y;
	align_primitive_rot_z = ai->align_primitive_rot_z;
	align_primitive_move_x = ai->align_primitive_move_x;
	align_primitive_move_y = ai->align_primitive_move_y;
	align_primitive_move_z = ai->align_primitive_move_z;

	//select neighboring pieces
	Select_All_Layers(false);
	for(int i = 0;i<ai->num_alignment_neighbors;i++)
	{
		Select_Layer(Find_Layer_Index(ai->alignment_neighbor_ids[i]), true);
	}

	//select the primary piece
	Select_Layer(Find_Layer_Index(ai->alignment_layer_id), true);

	//get the alignment data is if it were user-initiated
	if(!Get_Alignment_Data())
	{
		return false;
	}
	else
	{
		Enable_Start_Auto_Alignment_Button(true);
		iterate_alignment = true;
	}

	//ready to go
	last_depth_align = 1000;
	last_primitive_position_align[0] = 1000;
	last_primitive_position_align[1] = 1000;
	last_primitive_position_align[2] = 1000;
	last_xrot_align = 10;
	last_yrot_align = 10;
	last_primitive_align[0] = 10;
	last_primitive_align[1] = 10;
	last_primitive_align[2] = 10;
	last_primitive_scale_align[0] = 1;
	last_primitive_scale_align[1] = 1;
	last_primitive_scale_align[2] = 1;
	if(!align_primitive)
	{
		ReCenter_Alignment_Plane();
	}
	return true;
}


//need to save variables and layers for application to other frames as needed

bool Start_Alignment()
{
	if(!Verify_Alignment_Data())
	{
		return false;
	}
	else
	{
		Enable_Start_Auto_Alignment_Button(true);
		iterate_alignment = true;
	}
	last_depth_align = 1000;
	last_primitive_position_align[0] = 1000;
	last_primitive_position_align[1] = 1000;
	last_primitive_position_align[2] = 1000;
	last_xrot_align = 10;
	last_yrot_align = 10;
	last_primitive_align[0] = 10;
	last_primitive_align[1] = 10;
	last_primitive_align[2] = 10;
	last_primitive_scale_align[0] = 1;
	last_primitive_scale_align[1] = 1;
	last_primitive_scale_align[2] = 1;
	if(!align_primitive)
	{
		ReCenter_Alignment_Plane();
	}
	Keyframe_Layer(alignment_layer_index, true);
	return true;
}

void Set_Initial_Relative_Alignment_Rotation(float xrot, float yrot)
{
	if(Verify_Alignment_Data())
	{
		alignment_plane_rotation[0] = alignment_neighbor_rotation[0]+yrot;
		alignment_plane_rotation[1] = alignment_neighbor_rotation[1]+xrot;
		alignment_plane_rotation[2] = alignment_neighbor_rotation[2];
		ReSet_Alignment_Plane();
		redraw_frame = true;
	}
}
void Update_Alignment_Angle()
{
	char text[32];
	Get_Alignment_Angle_Text(text);
	sscanf(text, "%f", &alignment_angle);
}

//xy is the mobile pixel
//pos is the desired target pos
__forceinline bool Register_Alignment_Point(int x, int y, float *pos)
{
	ALIGNMENT_POINT ap;
	ap.ix = x;
	ap.iy = y;
	ap.x = pos[0];
	ap.y = pos[1];
	ap.z = pos[2];
	ap.enabled = true;
	alignment_points.push_back(ap);
	return false;
}

__forceinline bool Check_Selected_Border_Pixel(int x, int y, int layer_id, float *pos)
{
	if(x<0||x>=frame->width||y<0||y>=frame->height)return false;
	int l = frame->Get_Pixel_Layer(x, y);
	if(l!=layer_id)
	{
		int index = Get_Layer_Index(l);
		if(index==-1)return false;
		if(Layer_Is_Selected(index))
		{
			float *pp = frame->Get_Pos(x, y);
			pos[0] = pp[0];
			pos[1] = pp[1];
			pos[2] = pp[2];
			Register_Alignment_Neighbor(index);
			return true;
		}
	}
	return false;
}

__forceinline bool Register_Selected_Border_Pixels(int x, int y, int layer_id)
{
	int cnt = 0;
	float pos[3] = {0,0,0};
	float tpos[3] = {0,0,0};
	if(Check_Selected_Border_Pixel(x-1, y-1, layer_id, tpos)){pos[0] += tpos[0];pos[1] += tpos[1];pos[2] += tpos[2];cnt++;}
	if(Check_Selected_Border_Pixel(x,   y-1, layer_id, tpos)){pos[0] += tpos[0];pos[1] += tpos[1];pos[2] += tpos[2];cnt++;}
	if(Check_Selected_Border_Pixel(x+1, y-1, layer_id, tpos)){pos[0] += tpos[0];pos[1] += tpos[1];pos[2] += tpos[2];cnt++;}
	if(Check_Selected_Border_Pixel(x-1, y,   layer_id, tpos)){pos[0] += tpos[0];pos[1] += tpos[1];pos[2] += tpos[2];cnt++;}
	if(Check_Selected_Border_Pixel(x+1, y,   layer_id, tpos)){pos[0] += tpos[0];pos[1] += tpos[1];pos[2] += tpos[2];cnt++;}
	if(Check_Selected_Border_Pixel(x-1, y+1, layer_id, tpos)){pos[0] += tpos[0];pos[1] += tpos[1];pos[2] += tpos[2];cnt++;}
	if(Check_Selected_Border_Pixel(x,   y+1, layer_id, tpos)){pos[0] += tpos[0];pos[1] += tpos[1];pos[2] += tpos[2];cnt++;}
	if(Check_Selected_Border_Pixel(x+1, y+1, layer_id, tpos)){pos[0] += tpos[0];pos[1] += tpos[1];pos[2] += tpos[2];cnt++;}
	if(cnt==0)
	{
		return false;
	}
	pos[0] = pos[0]/cnt;
	pos[1] = pos[1]/cnt;
	pos[2] = pos[2]/cnt;
	Register_Alignment_Point(x, y, pos);
	return true;
}

__forceinline bool Register_Selected_Border_Pixels(RLE_STRIP *rs, int layer_id)
{
	for(int i = rs->start_x;i<rs->end_x;i++)
	{
		Register_Selected_Border_Pixels(i, rs->y, layer_id);
	}
	return true;
}


bool Find_Selected_Border_Pixels(int layer_id)
{
	int li = Get_Layer_Index(layer_id);
	int n = Num_RLE_Strips_In_Layer(li);
	for(int i = 0;i<n;i++)
	{
		RLE_STRIP *rs = Get_RLE_Strip(li, i);
		Register_Selected_Border_Pixels(rs, layer_id);
	}
	return true;
}

bool Get_Most_Significant_Alignment_Neighbor()
{
	int n = Num_Layers();
	int highest = -1;
	int best_index = -1;
	for(int i = 0;i<n;i++)
	{
		if(highest<alignment_neighbor_counts[i])
		{
			highest = alignment_neighbor_counts[i];
			best_index = i;
		}
	}
	if(best_index!=-1)
	{
		Get_Indexed_Layer_Plane(best_index, alignment_neighbor_pos, alignment_neighbor_dir, alignment_neighbor_rotation, alignment_neighbor_offset, &alignment_neighbor_origin_type);
	}
	delete[] alignment_neighbor_counts;
	return true;
}


//NOTE: if this function is not inlined, then the actual code for "Alignment_Error"
//NEVER gets called, unless there is an "else" statement containing actual functioning code,
//a separate function is called from within if "if(true) block, OR if a counter variable
//is put within the "if(true) section, but then only if that is used somewhere else (like
//printing the number of hits at the end of the function
//OTHERWISE, the critical code gets optimized out, thanks VS6 for that last wasted hour
//of my life, I only tried inlining THIS function as a shot in the dark for trying to
//get the code to do what I programmed it for in the first place
__forceinline float Get_Alignment_Error()
{
	int n = alignment_points.size();
	float e = 0;
	for(int i = 0;i<n;i++)
	{
		if(alignment_points[i].enabled)
		{
			e += Alignment_Error(&alignment_points[i]);
		}
	}
	return e;
}
//I really would prefer that this function was not inlined, but i sort of needed
//to have the "enabled" flag as a way to disable certain tests, the code should be
//straightforward enough for even VS6 to compile correctly..
//and yes it does, without optimizations, after optimizations it runs extra fast
//because VS stripped the actual code out..
//HJMFC

//NOTE: if this function is not inlined, then the actual code for "Alignment_Error"
//NEVER gets called, unless there is an "else" statement containing actual functioning code,
//a separate function is called from within if "if(true) block, OR if a counter variable
//is put within the "if(true) section, but then only if that is used somewhere else (like
//printing the number of hits at the end of the function
//OTHERWISE, the critical code gets optimized out, thanks VS6 for that last wasted hour
//of my life, I only tried inlining THIS function as a shot in the dark for trying to
//get the code to do what I programmed it for in the first place
__forceinline float Get_Primitive_Alignment_Error()
{
	int n = alignment_points.size();
	float e = -1;
	int i;
	for(i = 0;i<n;i++)
	{
		if(alignment_points[i].enabled)
		{
			e += Alignment_Error(&alignment_points[i]);
		}
	}
	//now we have the highest error, add that on to every pixel that isn't hitting the primitive
	//float maxe = e;
	for(i = 0;i<n;i++)
	{
		if(alignment_points[i].enabled)
		{
			float p[3];
			if(!Pixel_Hits_Layer_Geometry(alignment_layer_index, alignment_points[i].ix, alignment_points[i].iy, p))
			{
				e += 10000;//maxe;//extra penalty error if pixels don't hit
			}
		}
	}
	return e;
}
//I really would prefer that this function was not inlined, but i sort of needed
//to have the "enabled" flag as a way to disable certain tests, the code should be
//straightforward enough for even VS6 to compile correctly..
//and yes it does, without optimizations, after optimizations it runs extra fast
//because VS stripped the actual code out..
//HJMFC

bool Get_Alignment_Data()
{
	alignment_layer_id = Get_Most_Recently_Selected_Layer(&alignment_layer_index);
	if(alignment_layer_id==-1)
	{
		return false;
	}
	Clear_Alignment_Points();
	Allocate_Alignment_Neighbor_Counts();
	Find_Selected_Border_Pixels(alignment_layer_id);
	Get_Most_Significant_Alignment_Neighbor();
	if(alignment_points.size()==0)
	{
		return false;
	}
	if(align_primitive)
	{
		last_alignment_error = Get_Primitive_Alignment_Error();
	}
	else
	{
		Get_Alignment_Plane();
		last_alignment_error = Get_Alignment_Error();
	}
	return true;
}

bool Verify_Alignment_Data()
{
	int n = alignment_points.size();
	if(n==0)
	{
		return Get_Alignment_Data();
	}
	return true;
}

bool ReSet_Alignment_Plane()
{
	float temp_dir[3];
	float temp_rotation[3];
	//flip rotations and ad 180 for matrix only!!!
	temp_rotation[0] = alignment_plane_rotation[1];
	temp_rotation[1] = (alignment_plane_rotation[0]);//+180;
	temp_rotation[2] = 0;
	oBB_MATRIX m;
	m.Set(m.m, alignment_plane_pos, temp_rotation);
	temp_dir[0] = m.m[2][0];
	temp_dir[1] = m.m[2][1];
	temp_dir[2] = m.m[2][2];
	Normalize(temp_dir);
	//temp_rotation[1] -= 180;
	current_alignment_plane_dir[0] = temp_dir[0];
	current_alignment_plane_dir[1] = temp_dir[1];
	current_alignment_plane_dir[2] = temp_dir[2];
	return Project_Indexed_Layer_To_Plane(alignment_layer_index, alignment_plane_pos, temp_dir, alignment_plane_rotation, alignment_plane_offset, alignment_plane_origin_type, false);
}

bool Stop_Alignment()
{
	if(iterate_alignment)
	{
		Enable_Start_Auto_Alignment_Button(false);
		iterate_alignment = false;
		align_depth = false;
		align_xrot = false;
		align_yrot = false;
		align_angle = false;
		align_primitive = false;
		float pos[3];
		ReCalc_Layer_Center(alignment_layer_id, pos);
		return true;
	}
	return false;
}

bool Stop_Primitive_Alignment()
{
	Enable_Start_Auto_Alignment_Button(false);
	iterate_alignment = false;
	align_depth = false;
	align_xrot = false;
	align_yrot = false;
	align_angle = false;
	align_primitive = false;
	return true;
}

bool Start_Horizontal_Convex_Alignment()
{
	Update_Alignment_Angle();
	Set_Initial_Relative_Alignment_Rotation(alignment_angle, 0);
	align_xrot = true;
	Start_Alignment();
	return true;
}

bool Start_Horizontal_Concave_Alignment()
{
	Update_Alignment_Angle();
	Set_Initial_Relative_Alignment_Rotation(-alignment_angle, 0);
	align_xrot = true;
	Start_Alignment();
	return true;
}

bool Start_Vertical_Convex_Alignment()
{
	Update_Alignment_Angle();
	Set_Initial_Relative_Alignment_Rotation(0, alignment_angle);
	align_yrot = true;
	Start_Alignment();
	return true;
}

bool Start_Vertical_Concave_Alignment()
{
	Update_Alignment_Angle();
	Set_Initial_Relative_Alignment_Rotation(0, -alignment_angle);
	align_yrot = true;
	Start_Alignment();
	return true;
}

bool Update_Alignment_Depth(float v)
{
	float old_error = Get_Alignment_Error();
	Move_Layer_Geometry_DepthWise(alignment_layer_index, v, false);//reproject_everytime);
	float new_error = Get_Alignment_Error();
	if(old_error<=new_error)
	{
		Move_Layer_Geometry_DepthWise(alignment_layer_index, -v, false);//reproject_everytime);
		return false;
	}
	return true;
}

bool Set_Modified_Alignment_Plane(float xrot, float yrot)
{
	float temp_dir[3];
	float temp_rotation[3];
	alignment_plane_rotation[0]+=xrot;
	alignment_plane_rotation[1]+=yrot;
	if(alignment_plane_rotation[0]>180)alignment_plane_rotation[0]-=360;
	if(alignment_plane_rotation[0]<-180)alignment_plane_rotation[0]+=360;
	if(alignment_plane_rotation[1]>180)alignment_plane_rotation[1]-=360;
	if(alignment_plane_rotation[1]<-180)alignment_plane_rotation[1]+=360;
	//flip rotations and ad 180 for matrix only!!!
	temp_rotation[0] = alignment_plane_rotation[1];
	temp_rotation[1] = (alignment_plane_rotation[0]);
	temp_rotation[2] = 0;
	oBB_MATRIX m;
	m.Set(m.m, alignment_plane_pos, temp_rotation);
	temp_dir[0] = m.m[2][0];
	temp_dir[1] = m.m[2][1];
	temp_dir[2] = m.m[2][2];
	Normalize(temp_dir);
	current_alignment_plane_dir[0] = temp_dir[0];
	current_alignment_plane_dir[1] = temp_dir[1];
	current_alignment_plane_dir[2] = temp_dir[2];
	return Project_Indexed_Layer_To_Plane(alignment_layer_index, alignment_plane_pos, temp_dir, alignment_plane_rotation, alignment_plane_offset, alignment_plane_origin_type, false);
}

bool Update_Alignment_XRot(float v)
{
	float old_error = Get_Alignment_Error();
	Set_Modified_Alignment_Plane(v, 0);
	float new_error = Get_Alignment_Error();
	if(old_error<=new_error)
	{
		Set_Modified_Alignment_Plane(-v, 0);
		return false;
	}
	return true;
}

bool Update_Alignment_YRot(float v)
{
	float old_error = Get_Alignment_Error();
	Set_Modified_Alignment_Plane(0, v);
	float new_error = Get_Alignment_Error();
	if(old_error<=new_error)
	{
		Set_Modified_Alignment_Plane(0, -v);
		return false;
	}
	return true;
}


bool Update_Alignment_Angle_XRot(float v)
{
	float old_error = Get_Alignment_Angle_Error();
	Set_Modified_Alignment_Plane(v, 0);
	float new_error = Get_Alignment_Angle_Error();
	if(old_error<=new_error)
	{
		Set_Modified_Alignment_Plane(-v, 0);
		return false;
	}
	return true;
}

bool Update_Alignment_Angle_YRot(float v)
{
	float old_error = Get_Alignment_Angle_Error();
	Set_Modified_Alignment_Plane(0, v);
	float new_error = Get_Alignment_Angle_Error();
	if(old_error<=new_error)
	{
		Set_Modified_Alignment_Plane(0, -v);
		return false;
	}
	return true;
}


bool Iterate_Alignment_Depth()
{
	if(!Update_Alignment_Depth(last_depth_align))
	{
		last_depth_align *= 0.5f;
		if(!Update_Alignment_Depth(last_depth_align))
		{
			last_depth_align *= -1;
		}
	}
	else
	{
	}
	return true;
}

bool Iterate_Alignment_XRot()
{
	if(!Update_Alignment_XRot(last_xrot_align))
	{
		last_xrot_align *= 0.5f;
		if(!Update_Alignment_XRot(last_xrot_align))
		{
			last_xrot_align *= -1;
		}
	}
	return true;
}

bool Iterate_Alignment_YRot()
{
	if(!Update_Alignment_YRot(last_yrot_align))
	{
		last_yrot_align *= 0.5f;
		if(!Update_Alignment_YRot(last_yrot_align))
		{
			last_yrot_align *= -1;
		}
	}
	return true;
}


bool Iterate_Alignment_Angle()
{
	if(!Update_Alignment_Angle_XRot(last_xrot_align))
	{
		last_xrot_align *= 0.5f;
		if(!Update_Alignment_Angle_XRot(last_xrot_align))
		{
			last_xrot_align *= -1;
		}
	}
	if(!Update_Alignment_Angle_YRot(last_yrot_align))
	{
		last_yrot_align *= 0.5f;
		if(!Update_Alignment_Angle_YRot(last_yrot_align))
		{
			last_yrot_align *= -1;
		}
	}
	return true;
}

bool ReAlign_Depth()
{
	last_depth_align = last_alignment_error;
	while(last_depth_align>EPSILON||last_depth_align<-EPSILON)
	{
		Iterate_Alignment_Depth();
	}
	ReCenter_Alignment_Plane();
	return true;
}

bool Restore_Alignment_Plane()
{
	return Project_Indexed_Layer_To_Plane(alignment_layer_index, alignment_plane_pos, current_alignment_plane_dir, alignment_plane_rotation, alignment_plane_offset, alignment_plane_origin_type, false);
}



void Set_Modified_Primitive_Rotation(int axis, float v)
{
	if(axis==0)
	{
		Rotate_Layer_Geometry(alignment_layer_index, v, 0, 0, false);
	}
	else if(axis==1)
	{
		Rotate_Layer_Geometry(alignment_layer_index, 0, v, 0, false);
	}
	else if(axis==2)
	{
		Rotate_Layer_Geometry(alignment_layer_index, 0, 0, v, false);
	}
}

void Set_Modified_Primitive_Scale(int axis, float v)
{
	if(axis==0)
	{
		Scale_Layer_Geometry(alignment_layer_index, v, 0, 0, false);
	}
	else if(axis==1)
	{
		Scale_Layer_Geometry(alignment_layer_index, 0, v, 0, false);
	}
	else if(axis==2)
	{
		Scale_Layer_Geometry(alignment_layer_index, 0, 0, v, false);
	}
}

void Set_Modified_Primitive_Position(int axis, float v)
{
	if(axis==0)
	{
		Move_Layer_Geometry_Horizontally(alignment_layer_index, v, false);
	}
	else if(axis==1)
	{
		Move_Layer_Geometry_Vertically(alignment_layer_index, v, false);
	}
	else if(axis==2)
	{
		Move_Layer_Geometry_DepthWise(alignment_layer_index, v, false);
	}
}

bool Update_Primitive_Alignment_Axis(int axis, float v)
{
	float old_error = Get_Primitive_Alignment_Error();
	//modify axis rotation by value
	Set_Modified_Primitive_Rotation(axis, v);
	float new_error = Get_Primitive_Alignment_Error();
	if(old_error<=new_error)
	{
		//set it back, no improvement
		Set_Modified_Primitive_Rotation(axis, -v);
		return false;
	}
	return true;
}

bool Iterate_Primitive_Alignment_Axis(int axis)
{
	if(!Update_Primitive_Alignment_Axis(axis, last_primitive_align[axis]))
	{
		last_primitive_align[axis] *= 0.5f;
		if(!Update_Primitive_Alignment_Axis(axis, last_primitive_align[axis]))
		{
			last_primitive_align[axis] *= -1;
		}
	}
	return true;
}

bool Update_Primitive_Scale_Axis(int axis, float v)
{
	float old_error = Get_Primitive_Alignment_Error();
	//modify scale axis by value
	Set_Modified_Primitive_Scale(axis, v);
	float new_error = Get_Primitive_Alignment_Error();
	if(old_error<=new_error)
	{
		//set it back, no improvement
		Set_Modified_Primitive_Scale(axis, -v);
		return false;
	}
	return true;
}


bool Iterate_Primitive_Scale_Axis(int axis)
{
	if(!Update_Primitive_Scale_Axis(axis, last_primitive_scale_align[axis]))
	{
		last_primitive_scale_align[axis] *= 0.5f;
		if(!Update_Primitive_Scale_Axis(axis, last_primitive_scale_align[axis]))
		{
			last_primitive_scale_align[axis] *= -1;
		}
	}
	return true;
}

bool Update_Primitive_Position(int axis, float v)
{
	float old_error = Get_Primitive_Alignment_Error();
	Set_Modified_Primitive_Position(axis, v);
	float new_error = Get_Primitive_Alignment_Error();
	if(old_error<=new_error)
	{
		Set_Modified_Primitive_Position(axis, -v);
		return false;
	}
	return true;
}

bool Iterate_Primitive_Position(int axis)
{
	if(!Update_Primitive_Position(axis, last_primitive_position_align[axis]))
	{
		last_primitive_position_align[axis] *= 0.5f;
		if(!Update_Primitive_Position(axis, last_primitive_position_align[axis]))
		{
			last_primitive_position_align[axis] *= -1;
		}
	}
	return true;
}

bool ReAlign_Primitive_Position()
{
	last_primitive_position_align[0] = 1;
	last_primitive_position_align[1] = 1;
	last_primitive_position_align[2] = 1;
	if(align_primitive_move_x)
	{
		while(last_primitive_position_align[0]>EPSILON||last_primitive_position_align[0]<-EPSILON)
		{
			Iterate_Primitive_Position(0);
		}
	}
	if(align_primitive_move_y)
	{
		while(last_primitive_position_align[1]>EPSILON||last_primitive_position_align[1]<-EPSILON)
		{
			Iterate_Primitive_Position(1);
		}
	}
	if(align_primitive_move_z)
	{
		while(last_primitive_position_align[2]>EPSILON||last_primitive_position_align[2]<-EPSILON)
		{
			Iterate_Primitive_Position(2);
		}
	}
	return true;
}


bool Update_Primitive_Alignment()
{
	char otext[32];
	char ntext[32];
	bool position_only = (!align_primitive_scale_x&&!align_primitive_scale_y&&!align_primitive_scale_z&&!align_primitive_rot_x&&!align_primitive_rot_y&&!align_primitive_rot_z);
	if(position_only)
	{
		ReAlign_Primitive_Position();
	}
	if(align_primitive_scale_x)
	{
		ReAlign_Primitive_Position();
		Iterate_Primitive_Scale_Axis(0);
	}
	if(align_primitive_scale_y)
	{
		ReAlign_Primitive_Position();
		Iterate_Primitive_Scale_Axis(1);
	}
	if(align_primitive_scale_z)
	{
		ReAlign_Primitive_Position();
		Iterate_Primitive_Scale_Axis(2);
	}
	if(align_primitive_rot_x)
	{
		ReAlign_Primitive_Position();
		Iterate_Primitive_Alignment_Axis(0);
	}
	if(align_primitive_rot_y)
	{
		ReAlign_Primitive_Position();
		Iterate_Primitive_Alignment_Axis(1);
	}
	if(align_primitive_rot_z)
	{
		ReAlign_Primitive_Position();
		Iterate_Primitive_Alignment_Axis(2);
	}
	float e = Get_Primitive_Alignment_Error();
	sprintf(ntext, "%f", e);
	Get_Alignment_Error_Text(otext);
	if(!strcmp(otext, ntext))
	{
		bool stop = false;
		if(align_primitive_scale_x)
		{
			if(last_primitive_scale_align[0]<EPSILON&&last_primitive_scale_align[0]>-EPSILON)
			{
				stop = true;
			}
		}
		if(align_primitive_scale_y)
		{
			if(last_primitive_scale_align[1]<EPSILON&&last_primitive_scale_align[1]>-EPSILON)
			{
				stop = true;
			}
		}
		if(align_primitive_scale_z)
		{
			if(last_primitive_scale_align[2]<EPSILON&&last_primitive_scale_align[2]>-EPSILON)
			{
				stop = true;
			}
		}
		if(align_primitive_rot_x)
		{
			if(last_primitive_align[0]<EPSILON&&last_primitive_align[0]>-EPSILON)
			{
				stop = true;
			}
		}
		if(align_primitive_rot_y)
		{
			if(last_primitive_align[1]<EPSILON&&last_primitive_align[1]>-EPSILON)
			{
				stop = true;
			}
		}
		if(align_primitive_rot_z)
		{
			if(last_primitive_align[2]<EPSILON&&last_primitive_align[2]>-EPSILON)
			{
				stop = true;
			}
		}
		if(position_only)
		{
			stop = true;
		}
		if(stop)
		{
			Stop_Primitive_Alignment();
		}
	}
	ReProject_Layer(alignment_layer_index);
	Set_Alignment_Error_Text(ntext);
	last_alignment_error = Get_Primitive_Alignment_Error();
	return true;
}

bool Update_Alignment()
{
	if(align_primitive)
	{
		return Update_Primitive_Alignment();
	}
	char otext[32];
	char ntext[32];
	if(align_depth)Iterate_Alignment_Depth();
	if(align_xrot)
	{
		ReAlign_Depth();
		Iterate_Alignment_XRot();
	}
	if(align_yrot)
	{
		ReAlign_Depth();
		Iterate_Alignment_YRot();
	}
	if(align_angle)Iterate_Alignment_Angle();
	float e = Get_Alignment_Error();
	sprintf(ntext, "%f", e);
	Get_Alignment_Error_Text(otext);
	if(!strcmp(otext, ntext))
	{
		bool stop = false;
		if(align_depth)
		{
			if(last_depth_align<EPSILON&&last_depth_align>-EPSILON)
			{
				stop = true;
			}
		}
		if(align_xrot)
		{
			if(last_xrot_align<EPSILON&&last_xrot_align>-EPSILON)
			{
				stop = true;
				Restore_Alignment_Plane();
			}
		}
		if(align_yrot)
		{
			if(last_yrot_align<EPSILON&&last_yrot_align>-EPSILON)
			{
				stop = true;
				Restore_Alignment_Plane();
			}
		}
		if(align_angle)
		{
			if(last_xrot_align<EPSILON&&last_xrot_align>-EPSILON&&last_yrot_align<EPSILON&&last_yrot_align>-EPSILON)
			{
				stop = true;
				Restore_Alignment_Plane();
			}
		}
		if(stop)
		{
			Stop_Alignment();
		}
	}
	ReProject_Layer(alignment_layer_index);
	Set_Alignment_Error_Text(ntext);
	last_alignment_error = Get_Alignment_Error();
	return true;
}
////////////////////////////////////////////////////////////////////////////
/*
Form-fit code below..

  basically:

  Find all surrounding pixels and then, for each pixel in the selected layer,
  get the combined average depth of ALL surrounding pixels and weight by distance
  This will form-fit the layer to any arbitrary surrounding outlines
  */

struct FORM_FIT_PIXEL
{
	int x, y;
	float depth;
	float distance;
	int selected_state;
};

vector<FORM_FIT_PIXEL> formfit_edge_pixels;//edge pixels for the selected layer
vector<FORM_FIT_PIXEL> formfit_interior_pixels;//edge pixels for the selected layer

__forceinline void Add_Form_Fit_Edge_Pixel(int x, int y)
{
	FORM_FIT_PIXEL ffp;
	ffp.x = x;
	ffp.y = y;
	float *p = frame->Get_Pos(x, y);
	ffp.depth = fastsqrt((p[0]*p[0])+(p[1]*p[1])+(p[2]*p[2]));
	formfit_edge_pixels.push_back(ffp);
}

__forceinline void Try_Form_Fit_Edge_Pixel(int layer_id, int x, int y)
{
	int id = frame->Get_Pixel_Layer(x, y);
	if(id!=-1&&id!=layer_id)
	{
		Add_Form_Fit_Edge_Pixel(x, y);
	}
}

__forceinline void Get_Form_Fit_Edge_Pixels(int layer_id, RLE_STRIP *rs)
{
	for(int i = rs->start_x;i<rs->end_x;i++)
	{
		Try_Form_Fit_Edge_Pixel(layer_id, i, rs->y-1);
		Try_Form_Fit_Edge_Pixel(layer_id, i, rs->y+1);
	}
	Try_Form_Fit_Edge_Pixel(layer_id, rs->start_x-1, rs->y);
	Try_Form_Fit_Edge_Pixel(layer_id, rs->end_x, rs->y);
}

__forceinline float Get_Closest_Form_Fit_Edge_Distance(int x, int y)
{
	float dx, dy;
	float res = 9999999;
	float d;
	int n = formfit_edge_pixels.size();
	int id = frame->Get_Pixel_Layer(x, y);
	id = id;
	for(int i = 0;i<n;i++)
	{
		dx = formfit_edge_pixels[i].x-x;
		dy = formfit_edge_pixels[i].y-y;
		d = fastsqrt((dx*dx)+(dy*dy));
		if(res>d)
		{
			res = d;
		}
	}
	return res;
}

__forceinline void Add_Form_Fit_Interior_Pixel(int x, int y)
{
	FORM_FIT_PIXEL ffp;
	ffp.x = x;
	ffp.y = y;
	float *p = frame->Get_Pos(x, y);
	ffp.distance = Get_Closest_Form_Fit_Edge_Distance(x, y);
	formfit_interior_pixels.push_back(ffp);
}


__forceinline void Get_Form_Fit_Interior_Pixels(int layer_id, RLE_STRIP *rs)
{
	for(int i = rs->start_x;i<rs->end_x;i++)
	{
		Add_Form_Fit_Interior_Pixel(i, rs->y);
	}
}

bool Compare_FormFit_Interior_Pixels(FORM_FIT_PIXEL p1, FORM_FIT_PIXEL p2)
{
	return p1.distance<p2.distance;
}

bool Mark_Form_Fit_Outline_Pixels(bool *mask, float *depths)
{
	int n = formfit_edge_pixels.size();
	for(int i = 0;i<n;i++)
	{
		mask[(formfit_edge_pixels[i].y*frame->width)+formfit_edge_pixels[i].x] = true;
		depths[(formfit_edge_pixels[i].y*frame->width)+formfit_edge_pixels[i].x] = formfit_edge_pixels[i].depth;
	}
	return true;
}

__forceinline bool Get_Form_Fit_Depth(bool *mask, float *depths, int x, int y, float *res)
{
	if(x<0||x>=frame->width||y<0||y>=frame->height)return false;
	int k = (y*frame->width)+x;
	if(!mask[k])return false;
	*res = depths[k];
	return true;
}

__forceinline float Get_Average_Depth(bool *mask, float *depths, int x, int y)
{
	int cnt = 0;
	float res = 0;
	float temp = 0;
	if(Get_Form_Fit_Depth(mask, depths, x-1, y-1, &temp)){res+=temp;cnt++;}
	if(Get_Form_Fit_Depth(mask, depths, x,   y-1, &temp)){res+=temp;cnt++;}
	if(Get_Form_Fit_Depth(mask, depths, x+1, y-1, &temp)){res+=temp;cnt++;}
	if(Get_Form_Fit_Depth(mask, depths, x-1, y,  &temp)){res+=temp;cnt++;}
	if(Get_Form_Fit_Depth(mask, depths, x+1, y,  &temp)){res+=temp;cnt++;}
	if(Get_Form_Fit_Depth(mask, depths, x-1, y+1, &temp)){res+=temp;cnt++;}
	if(Get_Form_Fit_Depth(mask, depths, x,   y+1, &temp)){res+=temp;cnt++;}
	if(Get_Form_Fit_Depth(mask, depths, x+1, y+1, &temp)){res+=temp;cnt++;}
	if(cnt==0)
	{
		return 0;
	}
	return res/cnt;
}

bool Form_Fit_Interior_Pixels(bool *mask, float *depths)
{
	int n = formfit_interior_pixels.size();
	float max_distance = 0;
	int i;
	for(int i = 0;i<n;i++)
	{
		if(max_distance<formfit_interior_pixels[i].distance)max_distance = formfit_interior_pixels[i].distance;
	}
	for(i = 0;i<n;i++)
	{
		formfit_interior_pixels[i].depth = Get_Average_Depth(mask, depths, formfit_interior_pixels[i].x, formfit_interior_pixels[i].y);
		depths[(formfit_interior_pixels[i].y*frame->width)+formfit_interior_pixels[i].x] = formfit_interior_pixels[i].depth;
		mask[(formfit_interior_pixels[i].y*frame->width)+formfit_interior_pixels[i].x] = true;
		float *v = frame->Get_Pos(formfit_interior_pixels[i].x, formfit_interior_pixels[i].y);
		float tp[3] = {v[0],v[1],v[2]};
		SetLength(tp, formfit_interior_pixels[i].depth);
		float p = formfit_interior_pixels[i].distance/max_distance;
		float ip = 1.0f-p;
		v[0] = (v[0]*p)+(tp[0]*ip);
		v[1] = (v[1]*p)+(tp[1]*ip);
		v[2] = (v[2]*p)+(tp[2]*ip);
	}
	return true;
}

bool Form_Fit_Interior_Pixels()
{
	int n = frame->width*frame->height;
	//create a mask for which pixels are filled with valid depth info
	bool *mask = new bool[frame->width*frame->height];
	float *depths = new float[frame->width*frame->height];
	memset(mask, 0, sizeof(bool)*n);
	Mark_Form_Fit_Outline_Pixels(mask, depths);
	Form_Fit_Interior_Pixels(mask, depths);
	delete[] mask;
	delete[] depths;
	return true;
}

bool Form_Fit_To_Surroundings()
{
	int index = -1;
	int layer_id = Get_Most_Recently_Selected_Layer(&index);
	if(layer_id==-1)
	{
		return false;
	}
	int ns = Num_RLE_Strips_In_Layer(index);
	int i;
	for(int i = 0;i<ns;i++)
	{
		Get_Form_Fit_Edge_Pixels(layer_id, Get_RLE_Strip(index, i));
	}
	for(i = 0;i<ns;i++)
	{
		Get_Form_Fit_Interior_Pixels(layer_id, Get_RLE_Strip(index, i));
	}
	sort(formfit_interior_pixels.begin(), formfit_interior_pixels.end(), Compare_FormFit_Interior_Pixels);
	Form_Fit_Interior_Pixels();
	formfit_edge_pixels.clear();
	formfit_interior_pixels.clear();
	redraw_frame = true;
	return true;
}



