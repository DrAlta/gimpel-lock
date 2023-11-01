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
#include <GL/gl.h>
#include "Console.h"
#include "PlanarProjection.h"
#include "Puffit.h"
#include "Relief.h"
#include <vector>
#include <algorithm>
#include "LayerMask.h"
#include "Image.h"
#include "Layers.h"
#include "../GeometryTool/Primitives.h"
#include "../GeometryTool.h"
#include "../Skin.h"

using namespace std;

//flags to conserve memory in tight situations

extern bool use_triangle_indices;

bool b_allowVertexArray = true;

class LAYER;

//for plugin event system
bool Report_Layers_ReProjected();
bool Report_Layer_ReProjected(int layer_id);

bool Set_Halo_Tool_To_Selection();

//if the layer has an attached anchor point
bool Apply_Anchor_Points_To_Layer(int layer_id);

bool ReProject_Halo(int layer_id);
void Get_Plane_Dir(float *orot, float *dir);


bool flip_loaded_layers_vertical = false;
bool flip_loaded_layers_horizontal = false;
int speckle_skip = 16;
bool render_borders = true;
bool render_normals = false;
bool render_layer_outlines = true;
bool render_layer_mask = true;
int next_layer_id = 0;

bool show_layer_colors = false;


bool Is_First_Or_Last_Frame();
float fastsqrt(float n);
float __Puff_Strength(int x, int y, int type, bool directional);
bool Set_Puff_Search_Range(int range);
bool Find_Puff_Edge_Pixels(int layer_id);
bool Clear_Puff_Edge_Pixels();
void Update_Puff_Preview_Center();
bool Set_Default_Relief_Info(RELIEF_INFO *ri);
void Print_Contour_Progress(char *layer_name, int percent);
void Set_Depth_Slider_To_Selection();
void Set_Scale_Slider_To_Selection();
void Set_Geometry_Dialog_To_Selection();
void Set_Relief_To_Selection();
void Set_Contour_To_Selection();
float Get_Depth_ID_Float(float v);
void Update_Relief(RELIEF_INFO *ri, unsigned int *indices, int num_indices);
void Update_GUI_Selection_Info();
void Get_Preview_Puff_Pixels();
void SetLength(float *v, float s);
LAYER* Find_Layer(int id);
SCENE_PRIMITIVE* Find_Scene_Primitive(int layer_id);
LAYER* Find_Layer_In_Frame(int frame, int layer_id);

extern float depth_scale_offset;
extern float depth_scale;
extern float puff_scale;
extern float puff_range;
extern float scene_center[3];
extern bool render_puff_preview;
extern int puff_search_range;
extern bool cull_backfaces;
extern bool redraw_edit_window;
extern bool anaglyph_mode;
extern float depth_increment;
extern bool directional_extrusion;
extern int current_puff_type;


vector<LAYER*> *active_layers = 0;

class FRAME_LAYERS
{
public:
	FRAME_LAYERS()
	{
	}
	~FRAME_LAYERS()
	{
	}
	vector<LAYER*> layers;
	int frame_id;
};

vector<FRAME_LAYERS> frame_layers;

FRAME_LAYERS *active_frame_layers = 0;


float RF(float max)
{
	return max*(((float)(rand()%10000))/10000.0f);
}

void Random_Color(float *res)
{
	res[0] = RF(1);
	res[1] = RF(1);
	res[2] = RF(1);
}


struct NBLEND
{
	int neighbor_id;
	int type;
	float range;
};


//used when strips are cut into pieces by pre-existing layers
vector<int> orphan_strips;

void Register_Orphan_Strip(int start, int end)
{
	orphan_strips.push_back(start);
	orphan_strips.push_back(end);
}

class LAYER
{
public:
	LAYER()
	{
		tri_indices = 0;
		num_tri_indices = 0;
		pixel_indices = 0;
		num_pixel_indices = 0;
		selected = false;
		freeze = true;
		visible = true;
		reproject = false;
		scale = 1;
		modified = true;
		keyframed = false;
		selection_count = 0;
		num_neighbor_blends = 0;
		layer_puff_type = -1;
		layer_puff_search_range = puff_search_range;
		layer_puff_range = puff_range;
		layer_puff_scale = 0;
		layer_puff_directional = directional_extrusion;
		layer_puff_angle = 0;
		render_layer_borders = render_borders;
		render_outline = false;
		render_speckles = false;
		Random_Color(color);
		color[0] = 0.5f+(color[0]/2);
		color[1] = 0.5f+(color[1]/2);
		color[2] = 0.5f+(color[2]/2);
		Set_Default_Relief_Info(&relief_info);
		lx = hx = ly = hy = -1;
		scene_primitive = 0;
		layer_plane_origin_type = PLANE_ORIGIN_SELECTION_CENTER;
		has_anchor_points = false;
		has_halo = false;
	}
	~LAYER()
	{
		Clear_Data();
	}
	//called after loading or fresh creation
	bool Get_Primitive()
	{
		scene_primitive = Find_Scene_Primitive(layer_id);
		return scene_primitive!=0;
	}
	SCENE_PRIMITIVE *scene_primitive;
	__forceinline int Get_Primitive_Type()
	{
		return scene_primitive->primitive->primitive_type;
	}
	__forceinline bool Update_Geometry_Orientation()
	{
		PLANE *plane;
		SPHERE *sphere;
		CYLINDER *cylinder;
		BOX *box;
		TRIMESH *trimesh;
		float size[3];
		switch(scene_primitive->primitive->primitive_type)
		{
		case pt_PLANE:
			plane = ((PLANE*)scene_primitive->primitive);
			plane->plane_pos[0] = layer_primitive_pos[0];
			plane->plane_pos[1] = layer_primitive_pos[1];
			plane->plane_pos[2] = layer_primitive_pos[2];
			plane->plane_dir[0] = layer_primitive_dir[0];
			plane->plane_dir[1] = layer_primitive_dir[1];
			plane->plane_dir[2] = layer_primitive_dir[2];
			break;
		case pt_SPHERE:
			sphere = ((SPHERE*)scene_primitive->primitive);
			sphere->pos[0] = layer_primitive_pos[0];
			sphere->pos[1] = layer_primitive_pos[1];
			sphere->pos[2] = layer_primitive_pos[2];
			break;
		case pt_CYLINDER:
			cylinder = ((CYLINDER*)scene_primitive->primitive);
			cylinder->pos[0] = layer_primitive_pos[0];
			cylinder->pos[1] = layer_primitive_pos[1];
			cylinder->pos[2] = layer_primitive_pos[2];
			cylinder->rot[2] = layer_primitive_rotation[0];
			cylinder->rot[0] = -layer_primitive_rotation[1];
			cylinder->rot[1] = layer_primitive_rotation[2];
			size[0] = cylinder->base;
			size[1] = cylinder->height;
			size[2] = cylinder->top;
			cylinder->bounding_box.Set_Transform(size, cylinder->pos, cylinder->rot);
			break;
		case pt_BOX:
			box = ((BOX*)scene_primitive->primitive);
			box->pos[0] = layer_primitive_pos[0];
			box->pos[1] = layer_primitive_pos[1];
			box->pos[2] = layer_primitive_pos[2];
			box->rot[1] = layer_primitive_rotation[0];
			box->rot[0] = -layer_primitive_rotation[1];
			box->rot[2] = layer_primitive_rotation[2];
			box->bounding_box.Set_Transform(box->size, box->pos, box->rot);
			box->Fill_Vertices();
			break;
		case pt_TRIMESH:
			trimesh = ((TRIMESH*)scene_primitive->primitive);
			trimesh->pos[0] = layer_primitive_pos[0];
			trimesh->pos[1] = layer_primitive_pos[1];
			trimesh->pos[2] = layer_primitive_pos[2];
			trimesh->rot[1] = layer_primitive_rotation[0];
			trimesh->rot[0] = -layer_primitive_rotation[1];
			trimesh->rot[2] = layer_primitive_rotation[2];
			trimesh->Update_Matrix();
			break;
		};
		return true;
	}
	__forceinline bool Project_Vertex_To_Model_Geometry(float *origin, float *dir, float *new_pos)
	{
		TRIMESH *trimesh = ((TRIMESH*)scene_primitive->primitive);
		if(!trimesh->Project_Vertex_To_Geometry(origin, dir, new_pos))
		{
			//simple plane intersection
			float far_pos[3];
			float d = 100000;//FIXTHIS could be slimmed down A LOT
			far_pos[0] = origin[0]+(dir[0]*d);
			far_pos[1] = origin[1]+(dir[1]*d);
			far_pos[2] = origin[2]+(dir[2]*d);
			float tpos[3] = {layer_primitive_pos[0],layer_primitive_pos[1],layer_primitive_pos[2]};
			float near_dist = Dist_To_Plane(origin, tpos, frame->view_direction);
			float far_dist = Dist_To_Plane(far_pos, tpos, frame->view_direction);
			float total = near_dist+far_dist;
			float p = near_dist/total;
			float ip = 1.0f-p;
			new_pos[0] = (far_pos[0]*p)+(origin[0]*ip);
			new_pos[1] = (far_pos[1]*p)+(origin[1]*ip);
			new_pos[2] = (far_pos[2]*p)+(origin[2]*ip);
			return false;
		}
		return true;
	}
	__forceinline bool Project_Vertex_To_Layer_Geometry(float *origin, float *dir, float *new_pos)
	{
		if(!scene_primitive->Project_Vertex_To_Geometry(origin, dir, new_pos))
		{
			//simple plane intersection
			float far_pos[3];
			float d = 100000;//FIXTHIS could be slimmed down A LOT
			far_pos[0] = origin[0]+(dir[0]*d);
			far_pos[1] = origin[1]+(dir[1]*d);
			far_pos[2] = origin[2]+(dir[2]*d);
			float tpos[3] = {layer_primitive_pos[0],layer_primitive_pos[1],layer_primitive_pos[2]};
			float near_dist = Dist_To_Plane(origin, tpos, frame->view_direction);
			float far_dist = Dist_To_Plane(far_pos, tpos, frame->view_direction);
			float total = near_dist+far_dist;
			float p = near_dist/total;
			float ip = 1.0f-p;
			new_pos[0] = (far_pos[0]*p)+(origin[0]*ip);
			new_pos[1] = (far_pos[1]*p)+(origin[1]*ip);
			new_pos[2] = (far_pos[2]*p)+(origin[2]*ip);
			return false;//the default plane hit
		}
		return true;
	}
	void Get_Bounding_Box()
	{
		int n =  strips.size();
		if(n==0)
		{
			lx = hx = ly = hy = -1;
			topmost_pixel[0] = bottommost_pixel[0] = leftmost_pixel[0] = rightmost_pixel[0] = -1;
			topmost_pixel[1] = bottommost_pixel[1] = leftmost_pixel[1] = rightmost_pixel[1] = -1;
			return;
		}
		lx = strips[0].start_x;
		hx = strips[0].end_x;
		ly = hy = strips[0].y;
		RLE_STRIP *rs;
		topmost_pixel[0] = bottommost_pixel[0] = (lx+hx)/2;
		leftmost_pixel[0] = lx;
		rightmost_pixel[0] = hx;
		topmost_pixel[1] = bottommost_pixel[1] = leftmost_pixel[1] = rightmost_pixel[1] = ly;
		for(int i = 1;i<n;i++)
		{
			rs = &strips[i];
			if(lx>rs->start_x)
			{
				lx = rs->start_x;
				leftmost_pixel[0] = lx;
				leftmost_pixel[1] = rs->y;
			}
			if(hx<rs->end_x)
			{
				hx = rs->end_x;
				rightmost_pixel[0] = hx;
				rightmost_pixel[1] = rs->y;
			}
			if(ly>rs->y)
			{
				ly = rs->y;
				bottommost_pixel[0] = (rs->start_x+rs->end_x)/2;
				bottommost_pixel[1] = ly;
			}
			if(hy<rs->y)
			{
				hy = rs->y;
				topmost_pixel[0] = (rs->start_x+rs->end_x)/2;
				topmost_pixel[1] = hy;
			}
		}
	}
	__forceinline bool Strip_Within_Bounding_Box(int startx, int endx, int y)
	{
		if(y<ly)return false;
		if(y>hy)return false;
		if(startx>hx)return false;
		if(endx<lx)return false;
		return true;
	}
	bool Within_Bounds(int _lx, int _hx, int _ly, int _hy)
	{
		if(hx<_lx)return false;
		if(hy<_ly)return false;
		if(lx>_hx)return false;
		if(ly>_hy)return false;
		return true;
	}
	//combine this new potential strip with any existing strips
	__forceinline bool Combine_RLE_Strip(int y, int *start_x, int *end_x)
	{
		int n = strips.size();
		for(int i = 0;i<n;i++)
		{
			if(strips[i].y==y)
			{
				int sx = strips[i].start_x;
				int ex = strips[i].end_x;
				bool start_inside = false;
				bool end_inside = false;
				if(*start_x>=sx&&*start_x<=ex)start_inside = true;
				if(*end_x>=sx&&*end_x<=ex)end_inside = true;
				if(start_inside||end_inside)
				{
					//something crosses over
					if(start_inside&&end_inside)//both inside existing strip
					{
						//forget about it, no need to add this strip at all
						*start_x = -1;*end_x = -1;return false;
					}
					else if(start_inside)
					{
						//modify start position to existing strip
						*start_x = strips[i].start_x;
						//and get rid of the old strip
						strips.erase(strips.begin()+i);i--;n--;modified = true;
						//and continue testing the rest of the strips
					}
					else if(end_inside)
					{
						//modify end position to existing strip
						*end_x = strips[i].end_x;
						//and get rid of the old strip
						strips.erase(strips.begin()+i);i--;n--;modified = true;
						//and continue testing the rest of the strips
					}
				}
				else if(*start_x<=sx&&*end_x>=ex)//covers it completely
				{
					//just remove the pre-existing interior strip
					strips.erase(strips.begin()+i);i--;n--;modified = true;
				}
			}
		}
		return true;
	}
	__forceinline void Add_RLE_Strip(int y, int start_x, int end_x, bool fresh_layer)
	{
		if(!fresh_layer)Combine_RLE_Strip(y, &start_x, &end_x);
		if(start_x!=-1&&end_x!=-1)
		{
			RLE_STRIP rs;
			rs.y = y;
			rs.start_x = start_x;
			rs.end_x = end_x;
			Set_Pixels_Layer(&rs);
			strips.push_back(rs);
			modified = true;
		}
	}
	__forceinline void Set_Pixels_Layer(RLE_STRIP *rs)
	{
		for(int i = rs->start_x;i<rs->end_x;i++)
		{
			frame->Set_Pixel_Layer(i, rs->y, layer_id);
		}
	}
	__forceinline void Set_Pixels_Layer_IDs()
	{
		int n = strips.size();
		for(int i = 0;i<n;i++)
		{
			Set_Pixels_Layer(&strips[i]);
		}
	}
	__forceinline void Flag_Selected_Pixels(RLE_STRIP *rs)
	{
		for(int i = rs->start_x;i<rs->end_x;i++)
		{
			frame->Set_Pixel_Selected(i, rs->y);
		}
	}
	__forceinline void Flag_Selected_Pixels()
	{
		int n = strips.size();
		for(int i = 0;i<n;i++)
		{
			Flag_Selected_Pixels(&strips[i]);
		}
	}
	__forceinline void Set_Pixels_Layer_Color(RLE_STRIP *rs)
	{
		for(int i = rs->start_x;i<rs->end_x;i++)
		{
			float *rgb = frame->Get_RGB(i, rs->y);
			rgb[0] = color[0];
			rgb[1] = color[1];
			rgb[2] = color[2];
		}
	}
	__forceinline void Set_Pixels_Layer_Colors()
	{
		int n = strips.size();
		for(int i = 0;i<n;i++)
		{
			Set_Pixels_Layer_Color(&strips[i]);
		}
	}
	//adds strips from this layer to another one
	__forceinline bool Merge_Strips(LAYER *l)
	{
		int n = strips.size();
		for(int i = 0;i<n;i++)
		{
			l->Add_RLE_Strip(strips[i].y, strips[i].start_x, strips[i].end_x, false);
		}
		return true;
	}
	//trims start and end against existing strips
	void Trim_Against_RLE_Strips(int y, int *start_x, int *end_x)
	{
		int n = strips.size();
		for(int i = 0;i<n;i++)
		{
			if(strips[i].y==y)
			{
				int sx = strips[i].start_x;
				int ex = strips[i].end_x;
				bool start_inside = false;
				bool end_inside = false;
				if(*start_x>=sx&&*start_x<=ex)start_inside = true;
				if(*end_x>=sx&&*end_x<=ex)end_inside = true;
				if(start_inside||end_inside)
				{
					//something crosses over
					if(start_inside&&end_inside)//both inside strip
					{
						//this strip is toast, fully inside
						*start_x = -1;
						*end_x = -1;
						return;
					}
					else if(start_inside)
					{
						//modify start position to end
						*start_x = ex;
					}
					else if(end_inside)
					{
						//modify end position to start
						*end_x = sx;
					}
				}
				else if(*start_x<=sx&&*end_x>=ex)//covers it completely
				{
					//the strip is being split into pieces
					//save the second half of the split for later
					Register_Orphan_Strip(ex, *end_x);
					//create the first half of the split by modifying the end
					*end_x = sx;
					//and resume
				}
			}
		}
	}
	//overwrites strips in an unfrozen layer with new strip info
	void Replace_RLE_Strips(int y, int start_x, int end_x)
	{
		int n = strips.size();
		for(int i = 0;i<n;i++)
		{
			if(strips[i].y==y)
			{
				int sx = strips[i].start_x;
				int ex = strips[i].end_x;
				bool start_inside = false;
				bool end_inside = false;
				if(start_x>=sx&&start_x<=ex)start_inside = true;
				if(end_x>=sx&&end_x<=ex)end_inside = true;
				if(start_inside||end_inside)
				{
					//something crosses over
					if(start_inside&&end_inside)//both inside strip
					{
						//check for possible split
						if(start_x>sx&&end_x<ex)
						{
							//split into 2 surrounding strips
							//create the first strip by modifying the existing end position
							strips[i].end_x = start_x;
							//create a new strip that fills out the other half
							RLE_STRIP ns;
							ns.y = y;
							ns.start_x = end_x;
							ns.end_x = ex;
							strips.push_back(ns);
							modified = true;
							//technically no need to continue because there should be no more matching strips
						}
						else if(start_x==sx&&end_x==ex)
						{
							//covers it exactly, just remove it
							strips.erase(strips.begin()+i);
							i--;
							n--;
							modified = true;
						}
						else if (start_x==sx)
						{
							//splits into 2, no first half remains, modify start
							strips[i].start_x = end_x;
							modified = true;
						}
						else if(end_x==ex)
						{
							//splits into 2, no second half remains, modify end
							strips[i].end_x = start_x;
							modified = true;
						}
					}
					else if(start_inside)
					{
						//start intersects
						if(start_x==ex)
						{
							//just touches outside, no action needed
						}
						else
						{
							//modify end position
							strips[i].end_x = start_x;
							modified = true;
						}
					}
					else if(end_inside)
					{
						//end intersects
						if(end_x==sx)
						{
							//just touches outside, no action needed
						}
						else
						{
							//modify start position
							strips[i].start_x = end_x;
							modified = true;
						}
					}
				}
				else if(start_x<=sx&&end_x>=ex)//covers it completely
				{
					//just remove this strip
					strips.erase(strips.begin()+i);
					i--;
					n--;
					modified = true;
				}
			}
		}
	}
	unsigned int* Add_Tri_Indices_No_Border(int frame_width, int y, int startx, int endx, unsigned int *indices)
	{
		int i = startx;
		int cnt = 0;
		bool first = true;
		if(!Is_Edge_Pixel(i, y))
		{
			indices[cnt] = (((y+1)*frame_width)+i);cnt++;
			indices[cnt] = (((y+1)*frame_width)+i);cnt++;
			first = false;
		}
		if(endx<frame_width-2)
		{
			endx++;
		}
		for(i = startx;i<endx;i++)
		{
			if(!Is_Edge_Pixel(i, y))
			{
				if(first)
				{
					indices[cnt] = (((y+1)*frame_width)+i);cnt++;
					indices[cnt] = (((y+1)*frame_width)+i);cnt++;
					first = false;
				}
				indices[cnt] = (((y+1)*frame_width)+i);cnt++;
				indices[cnt] = ((y*frame_width)+i);cnt++;
			}
		}
		if(!first)
		{
			indices[cnt] = indices[cnt+1] = indices[cnt-1];cnt+=2;
		}
		num_render_indices += cnt;
		return &indices[cnt];
	}
	unsigned int* Add_Tri_Indices(int frame_width, int y, int startx, int endx, unsigned int *indices)
	{
		int i = startx;
		int cnt = 0;
		indices[cnt] = (((y+1)*frame_width)+i);cnt++;
		indices[cnt] = (((y+1)*frame_width)+i);cnt++;
		if(endx<frame_width-2)
		{
			endx++;
		}
		for(i = startx;i<endx;i++)
		{
			indices[cnt] = (((y+1)*frame_width)+i);cnt++;
			indices[cnt] = ((y*frame_width)+i);cnt++;
		}
		i--;
		indices[cnt] = ((y*frame_width)+i);cnt++;
		indices[cnt] = ((y*frame_width)+i);cnt++;
		num_render_indices += cnt;
		return &indices[cnt];
	}
	//pre-calc render indices for with or without borders
	void Render_Borders(bool b)
	{
		render_layer_borders = b;
		if(!tri_indices)
		{
			//saved on memory, render in realtime
			return;
		}
		unsigned int *ip = tri_indices;
		int i;
		RLE_STRIP *s;
		num_render_indices = 0;
		int n = strips.size();
		if(b)
		{
			for(i = 0;i<n;i++)
			{
				s = &strips[i];
				if(s->y!=frame->height-1)
				{
					ip = Add_Tri_Indices(frame->width, s->y, s->start_x, s->end_x, ip);
				}
			}
		}
		else
		{
			for(i = 0;i<n;i++)
			{
				s = &strips[i];
				if(s->y!=frame->height-1)
				{
					ip = Add_Tri_Indices_No_Border(frame->width, s->y, s->start_x, s->end_x, ip);
				}
			}
		}
		num_render_indices -= 2;
	}
	void Update_Tri_Indices()
	{
		int n = strips.size();
		num_tri_indices = 0;
		if(tri_indices)delete[] tri_indices;
		tri_indices = 0;
		num_render_indices = 0;
		render_layer_borders = render_borders;
		if(!use_triangle_indices)
		{
			//save on memory, render in realtime
			return;
		}
		int i;
		for(i = 0;i<n;i++)
		{
			RLE_STRIP *s = &strips[i];
			if(s->y!=frame->height-1)
			{
				num_tri_indices += ((s->end_x-s->start_x)*2)+4;
				if(s->end_x<frame->width-2)num_tri_indices+=2;
			}
		}
		if(n==0)
		{
			num_render_indices = 0;
			return;
		}
		tri_indices = new unsigned int[num_tri_indices];
		int num_strips = strips.size();
		RLE_STRIP *rs = &strips[num_strips-1];
		int last_index = (rs->y*frame->width)+(rs->end_x-2);
		for(i = 0;i<num_tri_indices;i++)
		{
			tri_indices[i] = last_index;
		}
		Render_Borders(render_layer_borders);
	}
	void Update_Pixel_Indices()
	{
		int n = strips.size();
		num_pixel_indices = 0;
		if(pixel_indices)delete[] pixel_indices;
		pixel_indices = 0;
		int i;
		for(i = 0;i<n;i++)
		{
			RLE_STRIP *s = &strips[i];
			num_pixel_indices += s->end_x-s->start_x;
		}
		if(n==0)
		{
			return;
		}
		pixel_indices = new unsigned int[num_pixel_indices];
		int j;
		int cnt = 0;
		for(i = 0;i<n;i++)
		{
			RLE_STRIP *s = &strips[i];
			for(j = s->start_x;j<s->end_x;j++)
			{
				pixel_indices[cnt] = (s->y*frame->width)+j;
				cnt++;
			}
		}
		if(cnt!=num_pixel_indices)
		{
			SkinMsgBox(0, "what!", 0, MB_OK);
			exit(0);
		}
	}
	__forceinline bool Is_Outline_Pixel(int x, int y)
	{
		if(frame->Get_Pixel_Layer(x-1, y)!=layer_id)return true;
		if(frame->Get_Pixel_Layer(x+1, y)!=layer_id)return true;
		if(frame->Get_Pixel_Layer(x, y-1)!=layer_id)return true;
		if(frame->Get_Pixel_Layer(x, y+1)!=layer_id)return true;
		return false;
	}
	__forceinline bool Is_Edge_Pixel(int x, int y)
	{
		return Is_Outline_Pixel(x, y);//FIXTHIS just eliminate this function
	}
	__forceinline bool Is_Left_Outline_Pixel(int x, int y)
	{
		if(frame->Get_Pixel_Layer(x-1, y)!=layer_id)return true;
		return false;
	}
	__forceinline bool Is_Right_Outline_Pixel(int x, int y)
	{
		if(frame->Get_Pixel_Layer(x+1, y)!=layer_id)return true;
		return false;
	}
	__forceinline bool Is_Top_Outline_Pixel(int x, int y)
	{
		if(frame->Get_Pixel_Layer(x, y-1)!=layer_id)return true;
		return false;
	}
	__forceinline bool Is_Bottom_Outline_Pixel(int x, int y)
	{
		if(frame->Get_Pixel_Layer(x, y+1)!=layer_id)return true;
		return false;
	}
	void Update_Outline_Pixels(RLE_STRIP *s)
	{
		for(int i = s->start_x;i<s->end_x;i++)
		{
			if(Is_Outline_Pixel(i, s->y))
			{
				outline.push_back(i);
				outline.push_back(s->y);
			}
/*			if(Is_Left_Outline_Pixel(i, s->y))
			{
				left_outline.push_back(i);
				left_outline.push_back(s->y);
			}
			if(Is_Right_Outline_Pixel(i, s->y))
			{
				right_outline.push_back(i);
				right_outline.push_back(s->y);
			}
			if(Is_Top_Outline_Pixel(i, s->y))
			{
				top_outline.push_back(i);
				top_outline.push_back(s->y);
			}
			if(Is_Bottom_Outline_Pixel(i, s->y))
			{
				bottom_outline.push_back(i);
				bottom_outline.push_back(s->y);
			}*/
		}
	}
	void Update_Outline_Pixels()
	{
		outline.clear();
		int n = strips.size();
		for(int i = 0;i<n;i++)
		{
			Update_Outline_Pixels(&strips[i]);
		}
	}
	void Clear_Data()
	{
		if(tri_indices)delete[] tri_indices;
		if(pixel_indices)delete[] pixel_indices;
		tri_indices = 0;
		pixel_indices = 0;
		num_tri_indices = 0;
		num_pixel_indices = 0;
		strips.clear();
		outline.clear();
	}
	void Update_Data()
	{
		Update_Tri_Indices();
		Update_Pixel_Indices();
		Update_Outline_Pixels();
		Get_Bounding_Box();
		reproject = true;
	}
	//so am I just fucking crazy or does THIS PARTICULAR VIDEO CARD
	//have a problem with accessing array elements when it's not an index buffer?
	__forceinline void SafeArrayElement(unsigned int index)
	{
		//glArrayElement(index);
		glColor3fv(&frame->rgb_colors[index*3]);
		glVertex3fv(&frame->vertices[index*3]);
	}
	__forceinline void Render_Tri_Indices_No_Border(int frame_width, int y, int startx, int endx)
	{
		int i = startx;
		bool first = true;
		unsigned int last = 0;
		if(!Is_Edge_Pixel(i, y))
		{
			SafeArrayElement((((y+1)*frame_width)+i));
			SafeArrayElement((((y+1)*frame_width)+i));
			first = false;
		}
		if(endx<frame_width-2)
		{
			endx++;
		}
		for(i = startx;i<endx;i++)
		{
			if(!Is_Edge_Pixel(i, y))
			{
				if(first)
				{
					SafeArrayElement((((y+1)*frame_width)+i));
					SafeArrayElement((((y+1)*frame_width)+i));
					first = false;
				}
				SafeArrayElement((((y+1)*frame_width)+i));
				last = ((y*frame_width)+i);
				SafeArrayElement(last);
			}
		}
		if(!first&&last!=0)
		{
			SafeArrayElement(last);
			SafeArrayElement(last);
		}
	}
	__forceinline void Render_Tri_Indices(int frame_width, int y, int startx, int endx)
	{
		int i = startx;
		SafeArrayElement((((y+1)*frame_width)+i));
		SafeArrayElement((((y+1)*frame_width)+i));
		if(endx<frame_width-2)
		{
			endx++;
		}
		for(i = startx;i<endx;i++)
		{
			SafeArrayElement((((y+1)*frame_width)+i));
			SafeArrayElement(((y*frame_width)+i));
		}
		i--;
		SafeArrayElement(((y*frame_width)+i));
		SafeArrayElement(((y*frame_width)+i));
	}
	__forceinline void Render_Layer_In_Realtime()
	{
		int i;
		RLE_STRIP *s;
		num_render_indices = 0;
		int n = strips.size();
		glBegin(GL_TRIANGLE_STRIP);
		if(render_layer_borders)
		{
			for(i = 0;i<n;i++)
			{
				s = &strips[i];
				if(s->y!=frame->height-1)
				{
					Render_Tri_Indices(frame->width, s->y, s->start_x, s->end_x);
				}
			}
		}
		else
		{
			for(i = 0;i<n;i++)
			{
				s = &strips[i];
				if(s->y!=frame->height-1)
				{
					Render_Tri_Indices_No_Border(frame->width, s->y, s->start_x, s->end_x);
				}
			}
		}
		glEnd();
	}
	void Render_Tri_Indices(bool allowVertexArray)
	{
		if(reproject)ReProject_To_Geometry();
		if(!allowVertexArray)
		{
			Render_Layer_In_Realtime();
		}
		if(num_render_indices>0)
		{
			//we are using pre-calculated index buffer for rendering
			glDrawElements(GL_TRIANGLE_STRIP, num_render_indices, GL_UNSIGNED_INT, tri_indices);
		}
		else
		{
			//memory was conserved, have to draw it in realtime
			Render_Layer_In_Realtime();
		}
	}
	void Render_Tri_Indices()
	{
		Render_Tri_Indices(b_allowVertexArray);
	}
	__forceinline float Dist_To_Plane(float *p, float *tp, float *n)
	{
		float r = (((p[0]-tp[0])*n[0])+((p[1]-tp[1])*n[1])+((p[2]-tp[2])*n[2]));
		if(r<0)return -r;
		return r;
	}
	//called when puff dialog window is open and layer is selected
	//could be eliminated by just re-projecting
	void Render_Puff_Preview_Tri_Indices()//FIXTHIS eliminate puff preview and just DO IT
	{
		reproject = true;
		float *src, *dst;
		int i;
		float puff;
		if(layer_puff_type!=-1)
		{
			for(i = 0;i<num_render_indices;i++)
			{
				//puff with regular projection
				src = frame->Get_Frame_Vector(tri_indices[i]);
				dst = frame->Get_Pos(tri_indices[i]);
				puff = 1.0f+(puff_scale*frame->pixels[tri_indices[i]].contour_ratio);
				Project_Vertex_To_Layer_Geometry(frame->view_origin, src, dst);
				Apply_Relief_To_Vertex(tri_indices[i], dst);
				dst[0] *= puff;
				dst[1] *= puff;
				dst[2] *= puff;
			}
		}
		else
		{
			for(i = 0;i<num_render_indices;i++)
			{
				src = frame->Get_Frame_Vector(tri_indices[i]);
				dst = frame->Get_Pos(tri_indices[i]);
				Project_Vertex_To_Layer_Geometry(frame->view_origin, src, dst);
				Apply_Relief_To_Vertex(tri_indices[i], dst);
			}
		}
		if(scale!=1)Scale_Pixels();
		if(cull_backfaces)glDisable(GL_CULL_FACE);
		glDrawElements(GL_TRIANGLE_STRIP, num_render_indices, GL_UNSIGNED_INT, tri_indices);
		if(cull_backfaces)glEnable(GL_CULL_FACE);
	}
	__forceinline void Render_Outline_Vertices_3D(vector<int> *outline)
	{
		int n = outline->size();
		int x, y, k;
		for(int i = 0;i<n;i+=2)
		{
			x = (*outline)[i];
			y = (*outline)[i+1];
			k = ((y*frame->width)+x)*3;
			glVertex3fv(&frame->vertices[k]);
		}
	}
	void Render_Outline_Indices_3D()
	{
		glBegin(GL_POINTS);
		Render_Outline_Vertices_3D(&outline);
		glEnd();
	}
	__forceinline void _Get_Position(int x, int y, float *pos)
	{
		//for speckles
		int k = (y*frame->width)+x;
		if(render_puff_preview&&selected)
		{
			//puff preview using with normal projection
			float puff = 1.0f+(puff_scale*frame->pixels[k].contour_ratio);
			Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(k), pos);
			Apply_Relief_To_Vertex(k, pos);
			pos[0] *= puff;
			pos[1] *= puff;
			pos[2] *= puff;
		}
		else
		{
			//existing position
			float *fpos = frame->Get_Pos(k);
			pos[0] = fpos[0];
			pos[1] = fpos[1];
			pos[2] = fpos[2];
		}
	}
	void Render_Normal()
	{
		float s = 0.0025f;
		glBegin(GL_LINES);
		glVertex3fv(final_layer_center);
		glVertex3f(final_layer_center[0]+(layer_primitive_dir[0]*s), final_layer_center[1]+(layer_primitive_dir[1]*s), final_layer_center[2]+(layer_primitive_dir[2]*s));
		glEnd();
	}
	void Render_Speckles(int skip)
	{
		int n = strips.size();
		float pos[3];
		RLE_STRIP *rs;
		glBegin(GL_POINTS);
		for(int i = 0;i<n;i+=skip)
		{
			rs = &strips[i];
			for(int j = rs->start_x;j<rs->end_x;j+=skip)
			{
				_Get_Position(j, rs->y, pos);
				glVertex3fv(pos);
			}
		}
		glEnd();
	}
	void Scale_Pixels()
	{
		int n = num_pixel_indices;
		int k;
		for(int i = 0;i<n;i++)
		{
			k = pixel_indices[i];
			float *pos = frame->Get_Pos(k);
			pos[0] = final_layer_center[0]+((pos[0]-final_layer_center[0])*scale);
			pos[1] = final_layer_center[1]+((pos[1]-final_layer_center[1])*scale);
			pos[2] = final_layer_center[2]+((pos[2]-final_layer_center[2])*scale);
		}
	}
	void RePuff_Pixels()
	{
		int n = num_pixel_indices;
		int k;
		float puff;
		for(int i = 0;i<n;i++)
		{
			k = pixel_indices[i];
			puff = 1.0f+(layer_puff_scale*frame->pixels[k].contour_ratio);
			float *pos = frame->Get_Pos(k);
			pos[0] *= puff;
			pos[1] *= puff;
			pos[2] *= puff;
		}
	}
	void Set_Relief_Info(RELIEF_INFO *ri, bool update_values, bool reproject)
	{
		int n = strips.size();
		memcpy(&relief_info, ri, sizeof(RELIEF_INFO));
		if(ri->type==RELIEF_TYPE_RANDOM_NOISE)
		{
			srand(layer_id);
		}
		if(update_values)
		{
			Update_Relief(ri, pixel_indices, num_pixel_indices);
		}
		if(reproject)
		{
			ReProject_To_Geometry();
		}
	}
	__forceinline void Update_Initial_Layer_Relief()
	{
		Update_Relief(&relief_info, pixel_indices, num_pixel_indices);
		ReProject_To_Geometry();
	}
	__forceinline void Apply_Relief_To_Vertex(int k, float *pos)
	{
		//default vector size is 0.001 so multiply relief
		float relief = (frame->pixels[k].relief*relief_info.scale)*1000;
		float *dir = frame->Get_Frame_Vector(k);
		pos[0] += (dir[0]*relief);
		pos[1] += (dir[1]*relief);
		pos[2] += (dir[2]*relief);
	}
	//project to model
	void Project_To_Model()
	{
		if(scene_primitive->primitive->primitive_type!=pt_TRIMESH)
		{
			SkinMsgBox(0, "Error! Can't project to triangle mesh, wrong primitive type", 0, MB_OK);
			return;
		}
		Update_Geometry_Orientation();
		reproject = false;
		int n = num_pixel_indices;
		int k;
		float rpos[3];
		final_layer_center[0] = 0;//while we're at it..
		final_layer_center[1] = 0;
		final_layer_center[2] = 0;
		
		origin_layer_center[0] = 0;//while we're at it..
		origin_layer_center[1] = 0;
		origin_layer_center[2] = 0;

		//puff scale
		float pscale = 0;
		//adjust puff scale to preview settings OR just use the layer puff scale
		if(render_puff_preview){pscale = puff_scale;}else{pscale = layer_puff_scale;}

		for(int i = 0;i<n;i++)
		{
			k = pixel_indices[i];
			Project_Vertex_To_Model_Geometry(frame->view_origin, frame->Get_Frame_Vector(k), rpos);
			origin_layer_center[0] += rpos[0];origin_layer_center[1] += rpos[1];origin_layer_center[2] += rpos[2];
			final_layer_center[0] += rpos[0];final_layer_center[1] += rpos[1];final_layer_center[2] += rpos[2];
			Apply_Relief_To_Vertex(k, rpos);
			
			float *pos = frame->Get_Pos(k);
			pos[0] = rpos[0];pos[1] = rpos[1];pos[2] = rpos[2];
		}
		final_layer_center[0] = final_layer_center[0]/num_pixel_indices;final_layer_center[1] = final_layer_center[1]/num_pixel_indices;final_layer_center[2] = final_layer_center[2]/num_pixel_indices;
		origin_layer_center[0] = origin_layer_center[0]/num_pixel_indices;origin_layer_center[1] = origin_layer_center[1]/num_pixel_indices;origin_layer_center[2] = origin_layer_center[2]/num_pixel_indices;
		if(scale!=1)Scale_Pixels();
		if(layer_puff_type!=-1)RePuff_Pixels();
		if(has_anchor_points)Apply_Anchor_Points_To_Layer(layer_id);
		if(has_halo)ReProject_Halo(layer_id);
		Report_Layer_ReProjected(layer_id);
	}
	//re-position all pixels to correct 3d space
	void ReProject_To_Geometry()
	{
		Update_Geometry_Orientation();
		reproject = false;
		if(scene_primitive->primitive->primitive_type==pt_TRIMESH)
		{
			return;
		}
		int n = num_pixel_indices;
		int k;
		float rpos[3];
		final_layer_center[0] = 0;//while we're at it..
		final_layer_center[1] = 0;
		final_layer_center[2] = 0;
		
		origin_layer_center[0] = 0;//while we're at it..
		origin_layer_center[1] = 0;
		origin_layer_center[2] = 0;

		//puff scale
		float pscale = 0;
		//adjust puff scale to preview settings OR just use the layer puff scale
		if(render_puff_preview){pscale = puff_scale;}else{pscale = layer_puff_scale;}

		for(int i = 0;i<n;i++)
		{
			k = pixel_indices[i];
			Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(k), rpos);
			origin_layer_center[0] += rpos[0];origin_layer_center[1] += rpos[1];origin_layer_center[2] += rpos[2];
			final_layer_center[0] += rpos[0];final_layer_center[1] += rpos[1];final_layer_center[2] += rpos[2];
			Apply_Relief_To_Vertex(k, rpos);
			
			float *pos = frame->Get_Pos(k);
			pos[0] = rpos[0];pos[1] = rpos[1];pos[2] = rpos[2];
		}
		final_layer_center[0] = final_layer_center[0]/num_pixel_indices;final_layer_center[1] = final_layer_center[1]/num_pixel_indices;final_layer_center[2] = final_layer_center[2]/num_pixel_indices;
		origin_layer_center[0] = origin_layer_center[0]/num_pixel_indices;origin_layer_center[1] = origin_layer_center[1]/num_pixel_indices;origin_layer_center[2] = origin_layer_center[2]/num_pixel_indices;
		if(scale!=1)Scale_Pixels();
		if(layer_puff_type!=-1)RePuff_Pixels();
		if(has_anchor_points)Apply_Anchor_Points_To_Layer(layer_id);
		if(has_halo)ReProject_Halo(layer_id);
		Report_Layer_ReProjected(layer_id);
	}

	void ReCalc_Layer_Center()
	{
		int n = num_pixel_indices;
		int k;
		final_layer_center[0] = 0;
		final_layer_center[1] = 0;
		final_layer_center[2] = 0;
		origin_layer_center[0] = 0;
		origin_layer_center[1] = 0;
		origin_layer_center[2] = 0;
		double lc[3] = {0,0,0};
		float rpos[3];
		for(int i = 0;i<n;i++)
		{
			k = pixel_indices[i];
			float *pos = frame->Get_Pos(k);
			final_layer_center[0] += pos[0];
			final_layer_center[1] += pos[1];
			final_layer_center[2] += pos[2];
			Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(k), rpos);
			origin_layer_center[0] += rpos[0];
			origin_layer_center[1] += rpos[1];
			origin_layer_center[2] += rpos[2];
		}
		final_layer_center[0] = final_layer_center[0]/num_pixel_indices;
		final_layer_center[1] = final_layer_center[1]/num_pixel_indices;
		final_layer_center[2] = final_layer_center[2]/num_pixel_indices;
		origin_layer_center[0] = origin_layer_center[0]/num_pixel_indices;
		origin_layer_center[1] = origin_layer_center[1]/num_pixel_indices;
		origin_layer_center[2] = origin_layer_center[2]/num_pixel_indices;
	}
	//get the contour puff ratios for each pixel
	bool Puff_Pixels()
	{
		//save the old range
		int or = puff_search_range;

		//set the puff search range to match this layer
		Set_Puff_Search_Range(layer_puff_search_range);
		Find_Puff_Edge_Pixels(layer_id);

		int n = num_pixel_indices;
		int x, y, k;
		float ps;
		float ip = 0;
		int last_ip = 0;
		for(int i = 0;i<n;i++)
		{
			k = pixel_indices[i];
			x = frame->pixels[k].x;
			y = frame->pixels[k].y;
			//get the puff strength for this pixel
			ps = __Puff_Strength(x, y, layer_puff_type, layer_puff_directional);
			frame->pixels[k].contour_ratio = ps;
			//report progress
			ip = (((float)i)/n)*100;
			if((int)ip!=last_ip)
			{
				last_ip = (int)ip;
				Print_Contour_Progress(name, last_ip);
			}
		}
		//reset the puff search range
		Set_Puff_Search_Range(or);
		Clear_Puff_Edge_Pixels();
		//reproject so pixels get puffed to final position
		ReProject_To_Geometry();
		Print_Status("Done.");
		return true;
	}
	bool Set_Puff_Info(int search_range, float scale, float range, int type, bool directional, float angle)
	{
		if(scale==0)
		{
			layer_puff_type = -1;
			ReProject_To_Geometry();
			return true;
		}
		if(layer_puff_search_range==search_range&&layer_puff_type==type&&layer_puff_angle==angle&&layer_puff_directional==directional)
		{
			layer_puff_scale = scale;
			ReProject_To_Geometry();
			return true;
		}
		layer_puff_search_range = search_range;
		layer_puff_scale = scale;
		layer_puff_range = range;
		layer_puff_type = type;
		layer_puff_directional = directional;
		layer_puff_angle = angle;
		Puff_Pixels();
		return true;
	}
	//FIXTHIS change to object orientation, it's not just a plane anymore
	void Set_Plane_Projection(float *pos, float *dir, float *rotation, float *offset, int origin_type, bool reproject_pixels)
	{
		layer_primitive_pos[0] = pos[0];
		layer_primitive_pos[1] = pos[1];
		layer_primitive_pos[2] = pos[2];
		if(dir)
		{
			layer_primitive_dir[0] = dir[0];
			layer_primitive_dir[1] = dir[1];
			layer_primitive_dir[2] = dir[2];
			layer_primitive_rotation[0] = rotation[0];
			layer_primitive_rotation[1] = rotation[1];
			layer_primitive_rotation[2] = rotation[2];
		}
		layer_plane_offset[0] = offset[0];
		layer_plane_offset[1] = offset[1];
		layer_plane_offset[2] = offset[2];
		if(origin_type!=-1)
		{
			layer_plane_origin_type = origin_type;
		}
		if(reproject_pixels)
		{
			ReProject_To_Geometry();
		}
		else
		{
			reproject = true;
		}
	}
	void Set_Rotation(float *dir, float *rotation)
	{
		layer_primitive_dir[0] = dir[0];
		layer_primitive_dir[1] = dir[1];
		layer_primitive_dir[2] = dir[2];
		layer_primitive_rotation[0] = rotation[0];
		layer_primitive_rotation[1] = rotation[1];
		layer_primitive_rotation[2] = rotation[2];
		ReProject_To_Geometry();
	}
	void Get_Plane_Projection(float *pos, float *dir, float *rotation, float *offset, int *origin_type)
	{
		pos[0] = layer_primitive_pos[0];
		pos[1] = layer_primitive_pos[1];
		pos[2] = layer_primitive_pos[2];
		dir[0] = layer_primitive_dir[0];
		dir[1] = layer_primitive_dir[1];
		dir[2] = layer_primitive_dir[2];
		rotation[0] = layer_primitive_rotation[0];
		rotation[1] = layer_primitive_rotation[1];
		rotation[2] = layer_primitive_rotation[2];
		offset[0] = 0;
		offset[1] = 0;
		offset[2] = 0;
		*origin_type = layer_plane_origin_type;
	}
	void Get_Rotation(float *dir, float *rotation, int *origin_type)
	{
		dir[0] = layer_primitive_dir[0];
		dir[1] = layer_primitive_dir[1];
		dir[2] = layer_primitive_dir[2];
		rotation[0] = layer_primitive_rotation[0];
		rotation[1] = layer_primitive_rotation[1];
		rotation[2] = layer_primitive_rotation[2];
		*origin_type = layer_plane_origin_type;
	}
	void Render_Mask_2D(float point_size)
	{
		int n = strips.size();
		for(int i = 0;i<n;i++)
		{
			RLE_STRIP *rs = &strips[i];
			glVertex2f(rs->start_x, rs->y);
			glVertex2f(rs->end_x, rs->y);
		}
	}
	void Render_Outline_Vertices_2D(float point_size)
	{
		int n = outline.size();
		point_size *= 1.5f;
		if(selected)
		{
			glPointSize(point_size*2);
		}
		else
		{
			glPointSize(point_size);
		}
		float normal_color[3] = {0,1,0};
		float normal_selected_color[3] = {1,0,0};
		float gray = 0.5f;
		float frozen_color[3] = {gray,gray,1};
		float frozen_selected_color[3] = {1,1,1};
		if(selected)
		{
			if(freeze)
			{
				glColor3fv(frozen_selected_color);
			}
			else
			{
				glColor3fv(normal_selected_color);
			}
		}
		else
		{
			if(freeze)
			{
				glColor3fv(frozen_color);
			}
			else
			{
				glColor3fv(normal_color);
			}
		}
		glBegin(GL_POINTS);
		for(int i = 0;i<n;i+=2)
		{
			glVertex2f(outline[i]+0.5f, outline[i+1]+0.5f);
		}
		glEnd();
	}
	//these are ONLY for planar pieces ONLY after depth has changed, this keeps the corner LOCKED to that point in space
	__forceinline void Set_Origin_Top()
	{
		Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(topmost_pixel[0], topmost_pixel[1]), layer_primitive_pos);
	}
	__forceinline void Set_Origin_Bottom()
	{
		Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(bottommost_pixel[0], bottommost_pixel[1]), layer_primitive_pos);
	}
	__forceinline void Set_Origin_Left()
	{
		Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(leftmost_pixel[0], leftmost_pixel[1]), layer_primitive_pos);
	}
	__forceinline void Set_Origin_Right()
	{
		Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(rightmost_pixel[0], rightmost_pixel[1]), layer_primitive_pos);
	}
	__forceinline void Set_Origin_Center()
	{
		layer_primitive_pos[0] = origin_layer_center[0];
		layer_primitive_pos[1] = origin_layer_center[1];
		layer_primitive_pos[2] = origin_layer_center[2];
	}
	//ONLY called for when depth changes, rotations can move center and edges around
	__forceinline void ReCalc_Planar_Origin_Point()
	{
		switch(layer_plane_origin_type)
		{
			case PLANE_ORIGIN_SELECTION_TOP:
				Set_Origin_Top();
				break;
			case PLANE_ORIGIN_SELECTION_LEFT:
				Set_Origin_Left();
				break;
			case PLANE_ORIGIN_SELECTION_CENTER:
				Set_Origin_Center();
				break;
			case PLANE_ORIGIN_SELECTION_RIGHT:
				Set_Origin_Right();
				break;
			case PLANE_ORIGIN_SELECTION_BOTTOM:
				Set_Origin_Bottom();
				break;
			default:
				break;
		}
	}
	bool Dump_Disposable_Data()
	{
		if(tri_indices)delete[] tri_indices;
		if(pixel_indices)delete[] pixel_indices;
		tri_indices = 0;
		pixel_indices = 0;
		num_tri_indices = 0;
		num_render_indices = 0;
		num_pixel_indices = 0;
		outline.clear();
		return true;
	}
	char name[64];
	bool modified;
	bool keyframed;
	float layer_primitive_pos[3];
	float layer_primitive_dir[3];
	float layer_primitive_rotation[3];
	float layer_plane_offset[3];
	float final_layer_center[3];
	float origin_layer_center[3];
	//extra data, going to repurpose some bytes
	float extra_data[24];
	//first float is a flag for "formfit to surroundings"
	bool Layer_Formfits_To_Surrounding()
	{
		return extra_data[0]==0.1234f;//numerically specific value
	}
	void Set_Layer_Formfits_To_Surrounding(bool b)
	{
		if(b){extra_data[0] = 0.1234f;}
		else{extra_data[0] = 0;}
	}
	int topmost_pixel[2];
	int bottommost_pixel[2];
	int leftmost_pixel[2];
	int rightmost_pixel[2];
	int soften_edge_range;
	float soften_edge_strength;
	int layer_plane_origin_type;
	int layer_id;
	int num_tri_indices;
	int num_render_indices;
	int num_pixel_indices;
	float scale;
	bool selected, freeze, visible;
	bool render_outline;
	bool render_speckles;
	bool reproject;
	float color[3];
	unsigned int *tri_indices;
	//pixel indices are absolute linear index into frame array
	unsigned int *pixel_indices;
	int layer_puff_search_range;
	float layer_puff_scale;
	float layer_puff_range;
	int layer_puff_type;
	bool layer_puff_directional;
	float layer_puff_angle;
	bool render_layer_borders;
	int selection_count;
	int	num_neighbor_blends;
	int lx, hx, ly, hy;
	bool has_anchor_points;
	bool has_halo;
	vector<NBLEND> neighbor_blends;
	RELIEF_INFO relief_info;
	vector<RLE_STRIP> strips;
	//outline indices are literal interleaved x/y values
	vector<int> outline;
};


vector<LAYER*> selected_layers;
vector<int> selected_layer_indices;


bool Fill_Selection_Buffer()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->Flag_Selected_Pixels();
	}
	return true;
}

bool Update_Selected_Layers()
{
	selected_layers.clear();
	selected_layer_indices.clear();
	if(!active_frame_layers)
	{
		return false;
	}
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->selected)
		{
			selected_layers.push_back(active_frame_layers->layers[i]);
			selected_layer_indices.push_back(i);
		}
	}
	return true;
}


bool Transfer_Layer_Selection_States(int old_frame, int new_frame)
{
	FRAME_LAYERS *src = &frame_layers[old_frame];
	FRAME_LAYERS *dst = &frame_layers[new_frame];
	int on = src->layers.size();
	int nn = dst->layers.size();
	for(int i = 0;i<on;i++)
	{
		if(i<nn)
		{
			if(dst->layers[i]->layer_id==src->layers[i]->layer_id)
			{
				dst->layers[i]->selected = src->layers[i]->selected;
				dst->layers[i]->selection_count = src->layers[i]->selection_count;
			}
			else
			{
				LAYER *tl = Find_Layer_In_Frame(new_frame, src->layers[i]->layer_id);
				if(tl)
				{
					tl->selected = src->layers[i]->selected;
					tl->selection_count = src->layers[i]->selection_count;
				}
				else
				{
				}
			}
		}
		else
		{
			LAYER *tl = Find_Layer_In_Frame(new_frame, src->layers[i]->layer_id);
			if(tl)
			{
				tl->selected = src->layers[i]->selected;
				tl->selection_count = src->layers[i]->selection_count;
			}
			else
			{
			}
		}
	}
	return true;
}

bool Transfer_Layer_Names(int old_frame, int new_frame)
{
	FRAME_LAYERS *src = &frame_layers[old_frame];
	FRAME_LAYERS *dst = &frame_layers[new_frame];
	int on = src->layers.size();
	int nn = dst->layers.size();
	for(int i = 0;i<on;i++)
	{
		if(i<nn)
		{
			if(dst->layers[i]->layer_id==src->layers[i]->layer_id)
			{
				strcpy(dst->layers[i]->name, src->layers[i]->name);
			}
			else
			{
				LAYER *tl = Find_Layer_In_Frame(new_frame, src->layers[i]->layer_id);
				if(tl)strcpy(tl->name, src->layers[i]->name);
				else
				{
				}
			}
		}
		else
		{
			LAYER *tl = Find_Layer_In_Frame(new_frame, src->layers[i]->layer_id);
			if(tl)strcpy(tl->name, src->layers[i]->name);
			else
			{
			}
		}
	}
	return true;
}

bool Set_Layer_Name(int index, char *name)
{
	int layer_id = active_frame_layers->layers[index]->layer_id;
	strcpy(active_frame_layers->layers[index]->name, name);
	int n = frame_layers.size();
	for(int i = 0;i<n;i++)
	{
		if(i!=active_frame_layers->frame_id)
		{
			if(frame_layers[i].layers.size()==n)
			{
				if(frame_layers[i].layers[index]->layer_id==layer_id)
				{
					strcpy(frame_layers[i].layers[index]->name, name);
				}
				else
				{
					LAYER *tl = Find_Layer_In_Frame(i, layer_id);
					if(tl)
					{
						strcpy(tl->name, name);
					}
					else
					{
					}
				}
			}
			else
			{
				LAYER *tl = Find_Layer_In_Frame(i, layer_id);
				if(tl)
				{
					strcpy(tl->name, name);
				}
				else
				{
				}
			}
		}
	}
	return true;
}

LAYER* Duplicate_Layer(LAYER *l)
{
	LAYER *nl = new LAYER;
	strcpy(nl->name, l->name);
	nl->modified = l->modified;
	nl->keyframed = l->keyframed;
	memcpy(nl->layer_primitive_pos, l->layer_primitive_pos, sizeof(float)*3);
	memcpy(nl->layer_primitive_dir, l->layer_primitive_dir, sizeof(float)*3);
	memcpy(nl->layer_primitive_rotation, l->layer_primitive_rotation, sizeof(float)*3);
	memcpy(nl->layer_plane_offset, l->layer_plane_offset, sizeof(float)*3);
	memcpy(nl->final_layer_center, l->final_layer_center, sizeof(float)*3);
	memcpy(nl->origin_layer_center, l->origin_layer_center, sizeof(float)*3);
	memcpy(nl->extra_data, l->extra_data, sizeof(float)*24);
	nl->soften_edge_range = l->soften_edge_range;
	nl->soften_edge_strength = l->soften_edge_strength;
	nl->layer_plane_origin_type = l->layer_plane_origin_type;
	nl->layer_id = l->layer_id;
	nl->num_tri_indices = l->num_tri_indices;
	nl->num_render_indices = l->num_render_indices;
	nl->num_pixel_indices = l->num_pixel_indices;
	nl->scale = l->scale;
	nl->selected = l->selected;
	nl->freeze = l->freeze;
	nl->visible = l->visible;
	nl->render_outline = l->render_outline;
	nl->render_speckles = l->render_speckles;
	nl->reproject = l->reproject;
	memcpy(nl->color, l->color, sizeof(float)*3);
	nl->tri_indices = new unsigned int[l->num_tri_indices];
	memcpy(nl->tri_indices, l->tri_indices, sizeof(unsigned int)*l->num_tri_indices);
	nl->pixel_indices = new unsigned int[l->num_pixel_indices];
	memcpy(nl->pixel_indices, l->pixel_indices, sizeof(unsigned int)*l->num_pixel_indices);
	nl->layer_puff_search_range = l->layer_puff_search_range;
	nl->layer_puff_scale = l->layer_puff_scale;
	nl->layer_puff_range = l->layer_puff_range;
	nl->layer_puff_type = l->layer_puff_type;
	nl->layer_puff_directional = l->layer_puff_directional;
	nl->layer_puff_angle = l->layer_puff_angle;
	nl->render_layer_borders = l->render_layer_borders;
	nl->selection_count = l->selection_count;
	nl->num_neighbor_blends = l->num_neighbor_blends;
	nl->lx = l->lx;
	nl->hx = l->hx;
	nl->ly = l->ly;
	nl->hy = l->hy;
	int n = l->neighbor_blends.size();
	int i;
	for(i = 0;i<n;i++)
	{
		nl->neighbor_blends.push_back(l->neighbor_blends[i]);
	}
	memcpy(&nl->relief_info, &l->relief_info, sizeof(RELIEF_INFO));
	n = l->strips.size();
	for(i = 0;i<n;i++)
	{
		nl->strips.push_back(l->strips[i]);
	}
	n = l->outline.size();for(i = 0;i<n;i++){nl->outline.push_back(l->outline[i]);}
	nl->Get_Primitive();
	return nl;
}

LAYER* Duplicate_Layer_Info(LAYER *l)
{
	LAYER *nl = new LAYER;
	strcpy(nl->name, l->name);
	nl->modified = l->modified;
	nl->keyframed = l->keyframed;
	memcpy(nl->layer_primitive_pos, l->layer_primitive_pos, sizeof(float)*3);
	memcpy(nl->layer_primitive_dir, l->layer_primitive_dir, sizeof(float)*3);
	memcpy(nl->layer_primitive_rotation, l->layer_primitive_rotation, sizeof(float)*3);
	memcpy(nl->layer_plane_offset, l->layer_plane_offset, sizeof(float)*3);
	memcpy(nl->final_layer_center, l->final_layer_center, sizeof(float)*3);
	memcpy(nl->origin_layer_center, l->origin_layer_center, sizeof(float)*3);
	memcpy(nl->extra_data, l->extra_data, sizeof(float)*24);
	nl->soften_edge_range = l->soften_edge_range;
	nl->soften_edge_strength = l->soften_edge_strength;
	nl->layer_plane_origin_type = l->layer_plane_origin_type;
	nl->layer_id = l->layer_id;
	nl->num_tri_indices = 0;
	nl->num_render_indices = 0;
	nl->num_pixel_indices = 0;
	nl->scale = l->scale;
	nl->selected = l->selected;
	nl->freeze = l->freeze;
	nl->visible = l->visible;
	nl->render_outline = l->render_outline;
	nl->render_speckles = l->render_speckles;
	nl->reproject = l->reproject;
	memcpy(nl->color, l->color, sizeof(float)*3);
	nl->tri_indices = 0;
	nl->pixel_indices = 0;
	nl->layer_puff_search_range = l->layer_puff_search_range;
	nl->layer_puff_scale = l->layer_puff_scale;
	nl->layer_puff_range = l->layer_puff_range;
	nl->layer_puff_type = l->layer_puff_type;
	nl->layer_puff_directional = l->layer_puff_directional;
	nl->layer_puff_angle = l->layer_puff_angle;
	nl->render_layer_borders = l->render_layer_borders;
	nl->selection_count = l->selection_count;
	nl->num_neighbor_blends = l->num_neighbor_blends;
	nl->lx = l->lx;
	nl->hx = l->hx;
	nl->ly = l->ly;
	nl->hy = l->hy;
	int n = l->neighbor_blends.size();
	for(int i = 0;i<n;i++)
	{
		nl->neighbor_blends.push_back(l->neighbor_blends[i]);
	}
	memcpy(&nl->relief_info, &l->relief_info, sizeof(RELIEF_INFO));
	return nl;
}


bool Copy_Frame_Layers(FRAME_LAYERS *src, FRAME_LAYERS *dst)
{
	int n = dst->layers.size();
	int i;
	for(i = 0;i<n;i++)
	{
		delete dst->layers[i];
	}
	dst->layers.clear();
	n = src->layers.size();
	for(i = 0;i<n;i++)
	{
		dst->layers.push_back(Duplicate_Layer(src->layers[i]));
	}
	return true;
}

bool Verify_Frame_Loaded(int frame);

bool Copy_Layer_Edits(LAYER *src, LAYER *dst)
{
	dst->layer_plane_offset[0] = src->layer_plane_offset[0];
	dst->layer_plane_offset[1] = src->layer_plane_offset[1];
	dst->layer_plane_offset[2] = src->layer_plane_offset[2];
	dst->layer_plane_origin_type = src->layer_plane_origin_type;
	dst->layer_primitive_dir[0] = src->layer_primitive_dir[0];
	dst->layer_primitive_dir[1] = src->layer_primitive_dir[1];
	dst->layer_primitive_dir[2] = src->layer_primitive_dir[2];
	dst->layer_primitive_pos[0] = src->layer_primitive_pos[0];
	dst->layer_primitive_pos[1] = src->layer_primitive_pos[1];
	dst->layer_primitive_pos[2] = src->layer_primitive_pos[2];
	dst->layer_primitive_rotation[0] = src->layer_primitive_rotation[0];
	dst->layer_primitive_rotation[1] = src->layer_primitive_rotation[1];
	dst->layer_primitive_rotation[2] = src->layer_primitive_rotation[2];
	dst->layer_puff_angle = src->layer_puff_angle;
	dst->layer_puff_directional = src->layer_puff_directional;
	dst->layer_puff_range = src->layer_puff_range;
	dst->layer_puff_scale = src->layer_puff_scale;
	dst->layer_puff_search_range = src->layer_puff_search_range;
	dst->layer_puff_type = src->layer_puff_type;
	dst->scale = src->scale;
	memcpy(&dst->relief_info, &src->relief_info, sizeof(RELIEF_INFO));
	dst->scene_primitive = src->scene_primitive;
	return true;
}

bool Copy_Selected_Frame_Layer_Edits(FRAME_LAYERS *src, FRAME_LAYERS *dst)
{
	Verify_Frame_Loaded(dst->frame_id);
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		LAYER *l = Find_Layer_In_Frame(dst->frame_id, selected_layers[i]->layer_id);
		if(l)
		{
			Copy_Layer_Edits(selected_layers[i], l);
		}
	}
	return true;
}


bool Copy_Reference_Layer(int src_frame, int dst_frame, int layer_id)
{
	FRAME_LAYERS *src = &frame_layers[src_frame];
	FRAME_LAYERS *dst = &frame_layers[dst_frame];
	LAYER *src_layer= 0;
	LAYER *dst_layer= 0;
	int dst_index = -1;
	int n = src->layers.size();
	int i;
	for(i = 0;i<n;i++)
	{
		if(src->layers[i]->layer_id==layer_id)
		{
			src_layer = src->layers[i];
			i = n;
		}
	}
	if(!src_layer)
	{
		return false;
	}
	n = dst->layers.size();
	for(i = 0;i<n;i++)
	{
		if(dst->layers[i]->layer_id==layer_id)
		{
			dst_layer = dst->layers[i];
			dst_index = i;
			i = n;
		}
	}
	if(dst_layer)
	{
		Delete_Layer(dst_frame, layer_id);
	}
	dst_layer = Duplicate_Layer_Info(src_layer);
	dst->layers.push_back(dst_layer);
	return true;
}

//FIXTHIS move these into layer class
//find previous strip
__forceinline int Find_Matching_Strip_End(LAYER *l, RLE_STRIP *s)
{
	int n = l->strips.size();
	for(int i = 0;i<n;i++)
	{
		if(l->strips[i].y==s->y)
		{
			if(l->strips[i].end_x==s->start_x)
			{
				return i;
			}
		}
	}
	return -1;
}

//find next strip
__forceinline int Find_Matching_Strip_Start(LAYER *l, RLE_STRIP *s)
{
	int n = l->strips.size();
	for(int i = 0;i<n;i++)
	{
		if(l->strips[i].y==s->y)
		{
			if(l->strips[i].start_x==s->end_x)
			{
				return i;
			}
		}
	}
	return -1;
}

//FIXTHIS I think this exists in the layer class
__forceinline void Update_Pixel_Layer_IDs(RLE_STRIP *s, int layer_id)
{
	for(int i = s->start_x;i<s->end_x;i++)
	{
		frame->Set_Pixel_Layer(i, s->y, layer_id);
	}
}

void Report_Invalid_Pixel(int x, int y)
{
	char msg[512];
	sprintf(msg, "INTERNAL ERROR! Pixel at %i %i has invalid layer id, can't evaporate strip!", x, y);
	SkinMsgBox(0, msg, 0, MB_OK);
}

__forceinline bool Evaporate_Strip(RLE_STRIP *s, int layer_id)
{
	if(s->start_x==0&&s->end_x==frame->width)//special case, covers image
	{
		//assign to primary layer
		LAYER *pl = active_frame_layers->layers[0];//primary layer
		RLE_STRIP new_strip;
		new_strip.start_x = s->start_x;
		new_strip.end_x = s->end_x;
		new_strip.y = s->y;
		pl->strips.push_back(new_strip);
		Update_Pixel_Layer_IDs(s, pl->layer_id);
		return true;
	}
	if(s->start_x==0)//special case, no previous strip
	{
		int next_layer_id = frame->Get_Pixel_Layer(s->end_x, s->y);
		if(next_layer_id==-1)
		{
			SkinMsgBox(0, "INTERNAL ERROR! Next pixels have invalid layer id, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
			Report_Invalid_Pixel(s->end_x, s->y);
			return false;
		}
		LAYER *next_layer = Find_Layer(next_layer_id);
		if(!next_layer)
		{
			SkinMsgBox(0, "INTERNAL ERROR! Next pixels have no valid layer, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
			return false;
		}
		int next_strip_index = Find_Matching_Strip_Start(next_layer, s);
		if(next_strip_index==-1)
		{
			SkinMsgBox(0, "INTERNAL ERROR! Next layer has no matching RLE strip for end strip, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
			return false;
		}
		RLE_STRIP *next_strip = &next_layer->strips[next_strip_index];
		next_strip->start_x = s->start_x;
		Update_Pixel_Layer_IDs(next_strip, next_layer_id);
		return true;
	}
	if(s->end_x==frame->width)//special case, no next strip
	{
		int previous_layer_id = frame->Get_Pixel_Layer(s->start_x-1, s->y);
		if(previous_layer_id==-1)
		{
			SkinMsgBox(0, "INTERNAL ERROR! Previous pixels have invalid layer id, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
			Report_Invalid_Pixel(s->start_x-1, s->y);
			return false;
		}
		LAYER *previous_layer = Find_Layer(previous_layer_id);
		if(!previous_layer)
		{
			SkinMsgBox(0, "INTERNAL ERROR! Previous pixels have no valid layer, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
			return false;
		}
		int previous_strip_index = Find_Matching_Strip_End(previous_layer, s);
		if(previous_strip_index==-1)
		{
			SkinMsgBox(0, "INTERNAL ERROR! Previous layer has no matching RLE strip for end strip, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
			return false;
		}
		RLE_STRIP *previous_strip = &previous_layer->strips[previous_strip_index];
		previous_strip->end_x = s->end_x;
		Update_Pixel_Layer_IDs(previous_strip, previous_layer_id);
		return true;
	}
	int previous_layer_id = frame->Get_Pixel_Layer(s->start_x-1, s->y);
	int next_layer_id = frame->Get_Pixel_Layer(s->end_x, s->y);
	if(previous_layer_id==-1)
	{
		SkinMsgBox(0, "INTERNAL ERROR! Previous pixels have invalid layer id, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
		Report_Invalid_Pixel(s->start_x-1, s->y);
		return false;
	}
	if(next_layer_id==-1)
	{
		SkinMsgBox(0, "INTERNAL ERROR! Next pixels have invalid layer id, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
		Report_Invalid_Pixel(s->end_x, s->y);
		return false;
	}
	//special case, they are the same
	if(previous_layer_id==next_layer_id)
	{
		LAYER *layer = Find_Layer(previous_layer_id);
		if(!layer)
		{
			SkinMsgBox(0, "INTERNAL ERROR! Next/Previous pixels have no valid layer, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
			return false;
		}
		int previous_strip_index = Find_Matching_Strip_End(layer, s);
		int next_strip_index = Find_Matching_Strip_Start(layer, s);
		if(previous_strip_index==-1)
		{
			SkinMsgBox(0, "INTERNAL ERROR! Previous layer has no matching RLE strip for surrounded layer, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
			return false;
		}
		if(next_strip_index==-1)
		{
			SkinMsgBox(0, "INTERNAL ERROR! Next layer has no matching RLE strip for surrounded layer, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
			return false;
		}
		RLE_STRIP *previous_strip = &layer->strips[previous_strip_index];
		RLE_STRIP *next_strip = &layer->strips[next_strip_index];
		RLE_STRIP new_strip;
		new_strip.start_x = previous_strip->start_x;
		new_strip.end_x = next_strip->end_x;
		new_strip.y = s->y;
		if(next_strip_index>previous_strip_index)
		{
			layer->strips.erase(layer->strips.begin()+next_strip_index);
			layer->strips[previous_strip_index].start_x = new_strip.start_x;
			layer->strips[previous_strip_index].end_x = new_strip.end_x;
			layer->strips[previous_strip_index].y = new_strip.y;
		}
		else
		{
			layer->strips.erase(layer->strips.begin()+previous_strip_index);
			layer->strips[next_strip_index].start_x = new_strip.start_x;
			layer->strips[next_strip_index].end_x = new_strip.end_x;
			layer->strips[next_strip_index].y = new_strip.y;
		}
		Update_Pixel_Layer_IDs(&new_strip, previous_layer_id);
		return true;
	}
	LAYER *previous_layer = Find_Layer(previous_layer_id);
	LAYER *next_layer = Find_Layer(next_layer_id);
	if(!previous_layer)
	{
		SkinMsgBox(0, "INTERNAL ERROR! Previous pixels have no valid layer, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
		return false;
	}
	if(!next_layer)
	{
		SkinMsgBox(0, "INTERNAL ERROR! Next pixels have no valid layer, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
		return false;
	}
	int previous_strip_index = Find_Matching_Strip_End(previous_layer, s);
	int next_strip_index = Find_Matching_Strip_Start(next_layer, s);
	if(previous_strip_index==-1)
	{
		SkinMsgBox(0, "INTERNAL ERROR! Previous layer has no matching RLE strip for middle layer, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
		return false;
	}
	if(next_strip_index==-1)
	{
		SkinMsgBox(0, "INTERNAL ERROR! Next layer has no matching RLE strip for middle layer, can't evaporate strip!", "INTERNAL ERROR IN CORE!", MB_OK);
		return false;
	}
	RLE_STRIP *previous_strip = &previous_layer->strips[previous_strip_index];
	RLE_STRIP *next_strip = &next_layer->strips[next_strip_index];
	float fmx = (s->start_x+s->end_x);
	fmx *= 0.5f;

	int mx = (int)fmx;
	previous_strip->end_x = mx;
	next_strip->start_x = mx;
	Update_Pixel_Layer_IDs(previous_strip, previous_layer_id);
	Update_Pixel_Layer_IDs(next_strip, next_layer_id);
	return true;
}

bool Evaporate_Layer(int layer_id)
{
	LAYER *l = Find_Layer(layer_id);
	if(!l)
	{
		return false;
	}
	int n = l->strips.size();
	for(int i = 0;i<n;i++)
	{
		if(!Evaporate_Strip(&l->strips[i], layer_id))
		{
			SkinMsgBox(0, "INTERNAL ERROR! Can't evaporate layer, the frame layer data has been corrupted.", "This should never happen", MB_OK);
			return false;
		}
	}
	l->strips.clear();
	Update_Selected_Layers();
	return true;
}

bool Shift_Entire_Layer_Vertically(LAYER *l, int index, int ydiff)
{
	int n = l->strips.size();
	int i;
	for(i = 0;i<n;i++)
	{
		if(!Evaporate_Strip(&l->strips[i], l->layer_id))
		{
			SkinMsgBox(0, "INTERNAL ERROR! Can't evaporate layer to shift up, the frame layer data has been corrupted.", "This should never happen", MB_OK);
			return false;
		}
	}
	vector<RLE_STRIP> new_strips;
	for(i = 0;i<n;i++)
	{
		l->strips[i].y += ydiff;
		if(l->strips[i].y>=0||l->strips[i].y<frame->height)
		{
			new_strips.push_back(l->strips[i]);
		}
	}
	l->strips.clear();
	n = new_strips.size();
	for(i = 0;i<n;i++)
	{
		//basically a fresh layer because it just got wiped out
		Force_Add_Layer_RLE_Strip(active_frame_layers->frame_id, index, new_strips[i].y, new_strips[i].start_x, new_strips[i].end_x, true);
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
	return true;
}

bool Shift_Selected_Layers_Vertically(int ydiff)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		Shift_Entire_Layer_Vertically(selected_layers[i], selected_layer_indices[i], ydiff);
	}
	return true;
}


bool Copy_Layers_To_All_Frames(int frame)
{
	int n = frame_layers.size();
	for(int i = 0;i<n;i++)
	{
		if(i!=frame)
		{
			Print_Status("Copying layers to frame %i", i);
			Copy_Frame_Layers(&frame_layers[frame], &frame_layers[i]);
		}
	}
	Print_Status("Done.");
	return true;
}

bool Copy_Layers_To_Frame(int src, int dst)
{
	Print_Status("Copying layers to frame %i", dst);
	Copy_Frame_Layers(&frame_layers[src], &frame_layers[dst]);
	Print_Status("Done.");
	return true;
}

bool Copy_Selected_Layer_Edits_To_All_Frames(int frame)
{
	int n = frame_layers.size();
	for(int i = 0;i<n;i++)
	{
		if(i!=frame)
		{
			Print_Status("Copying layers to frame %i", i);
			Copy_Selected_Frame_Layer_Edits(&frame_layers[frame], &frame_layers[i]);
		}
	}
	Print_Status("Done.");
	return true;
}


bool Copy_Selected_Layer_Edits_To_Frame(int src, int dst)
{
	Print_Status("Copying layers to frame %i", dst);
	Copy_Selected_Frame_Layer_Edits(&frame_layers[src], &frame_layers[dst]);
	Print_Status("Done.");
	return true;
}

bool Generate_Disposable_Layer_Data()
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->Update_Data();
	}
	return true;
}

bool Set_Default_Layer_Relief_Image(char *file)
{
	strcpy(active_frame_layers->layers[0]->relief_info.image, file);
	return true;
}


bool Set_Active_Frame_Layers(int frame, bool regen_data)
{
	active_frame_layers = &frame_layers[frame];
	if(regen_data)
	{
		Update_Pixels_Layer_IDs();
		Generate_Disposable_Layer_Data();
		Enable_All_Borders(render_borders);
	}
	Update_Selected_Layers();
	return true;
}

bool Set_Temp_Active_Frame_Layers(int frame)
{
	active_frame_layers = &frame_layers[frame];
	return true;
}

bool Dump_Disposable_Layer_Data()
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->Dump_Disposable_Data();
	}
	Backup_Current_Contours();
	return true;
}

void Update_Pixels_Layer_IDs()
{
	if(!active_frame_layers)return;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->Set_Pixels_Layer_IDs();
	}
}

//FIXTHIS move layermask code into separate file, can be queried from frame
bool Save_Layer_Map(char *file)
{
	if(!frame)
	{
		return false;
	}
	float *data = new float[frame->width*frame->height*3];
	memcpy(data, frame->rgb_colors, sizeof(float)*frame->width*frame->height*3);
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->Set_Pixels_Layer_Colors();
	}
	Save_Clean_Image(frame->rgb_colors, frame->width, frame->height, file);
	memcpy(frame->rgb_colors, data, sizeof(float)*frame->width*frame->height*3);
	delete[] data;
	return true;
}


bool Free_Frame_Layers()
{
	int n = frame_layers.size();
	for(int i = 0;i<n;i++)
	{
		Set_Temp_Active_Frame_Layers(i);
		Free_Layers();
	}
	frame_layers.clear();
	active_frame_layers = 0;
	return false;
}

bool Allocate_Frame_Layers(int num_frames)
{
	Free_Frame_Layers();
	for(int i = 0;i<num_frames;i++)
	{
		FRAME_LAYERS fl;
		fl.frame_id = i;
		frame_layers.push_back(fl);
	}
	return true;
}

bool Append_Single_Frame_Layers()
{
	FRAME_LAYERS fl;
	fl.frame_id = frame_layers.size();
	frame_layers.push_back(fl);
	return true;
}

bool Keyframe_All_Layers()
{
	int n = Num_Layers();
	for(int i = 0;i<n;i++)
	{
		Keyframe_Layer(i, true);
	}
	return true;
}

LAYER* Find_Layer_In_Frame(int frame, int layer_id)
{
	FRAME_LAYERS *fl = &frame_layers[frame];
	int n = fl->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(fl->layers[i]->layer_id==layer_id)
		{
			return fl->layers[i];
		}
	}
	return 0;
}

LAYER* Find_Next_Layer_Keyframe(int frame, int layer_id, int *frame_id)
{
	int n = frame_layers.size();
	for(int i = frame+1;i<n;i++)
	{
		LAYER *l = Find_Layer_In_Frame(i, layer_id);
		if(l)
		{
			if(l->keyframed)
			{
				*frame_id = i;
				return l;
			}
			else
			{
				if(i==n-1)
				{
					//it's the last frame, consider it keyframed
					*frame_id = i;
					return l;
				}
			}
		}
	}
	return 0;
}

LAYER* Find_Previous_Layer_Keyframe(int frame, int layer_id, int *frame_id)
{
	for(int i = frame-1;i>-1;i--)
	{
		LAYER *l = Find_Layer_In_Frame(i, layer_id);
		if(l)
		{
			if(l->keyframed)
			{
				*frame_id = i;
				return l;
			}
		}
	}
	return 0;
}

bool Interpolate_Relief_Info(RELIEF_INFO *ri, RELIEF_INFO *previous, RELIEF_INFO *next, float pp, float np)
{
	ri->contrast = (previous->contrast*pp)+(next->contrast*np);
	ri->contrast_slider_range = (previous->contrast_slider_range*pp)+(next->contrast_slider_range*np);
	ri->tiling = (previous->tiling*pp)+(next->tiling*np);
	ri->tiling_slider_range = (previous->tiling_slider_range*pp)+(next->tiling_slider_range*np);
	ri->bias = (previous->bias*pp)+(next->bias*np);
	ri->concave_color[0] = (unsigned char)((float)((((float)previous->concave_color[0])*pp)+(((float)next->concave_color[0])*np)));
	ri->concave_color[1] = (unsigned char)((float)((((float)previous->concave_color[1])*pp)+(((float)next->concave_color[1])*np)));
	ri->concave_color[2] = (unsigned char)((float)((((float)previous->concave_color[2])*pp)+(((float)next->concave_color[2])*np)));
	ri->convex_color[0] = (unsigned char)((float)((((float)previous->convex_color[0])*pp)+(((float)next->convex_color[0])*np)));
	ri->convex_color[1] = (unsigned char)((float)((((float)previous->convex_color[1])*pp)+(((float)next->convex_color[1])*np)));
	ri->convex_color[2] = (unsigned char)((float)((((float)previous->convex_color[2])*pp)+(((float)next->convex_color[2])*np)));
	ri->scale = (previous->scale*pp)+(next->scale*np);
	ri->scale_slider_range = (previous->scale_slider_range*pp)+(next->scale_slider_range*np);
	return true;
}


void Clamp_Safe_Rotation(float *ref_rot, float *test_rot, float *res);
float Clamp_Euler_Angle(float angle);

bool Interpolate_Layers(LAYER *layer, LAYER *previous, LAYER *next, float pp, float np)
{
	layer->final_layer_center[0] = (previous->final_layer_center[0]*pp)+(next->final_layer_center[0]*np);
	layer->final_layer_center[1] = (previous->final_layer_center[1]*pp)+(next->final_layer_center[1]*np);
	layer->final_layer_center[2] = (previous->final_layer_center[2]*pp)+(next->final_layer_center[2]*np);
	layer->origin_layer_center[0] = (previous->origin_layer_center[0]*pp)+(next->origin_layer_center[0]*np);
	layer->origin_layer_center[1] = (previous->origin_layer_center[1]*pp)+(next->origin_layer_center[1]*np);
	layer->origin_layer_center[2] = (previous->origin_layer_center[2]*pp)+(next->origin_layer_center[2]*np);
	layer->layer_primitive_dir[0] = (previous->layer_primitive_dir[0]*pp)+(next->layer_primitive_dir[0]*np);
	layer->layer_primitive_dir[1] = (previous->layer_primitive_dir[1]*pp)+(next->layer_primitive_dir[1]*np);
	layer->layer_primitive_dir[2] = (previous->layer_primitive_dir[2]*pp)+(next->layer_primitive_dir[2]*np);
	layer->layer_plane_offset[0] = (previous->layer_plane_offset[0]*pp)+(next->layer_plane_offset[0]*np);
	layer->layer_plane_offset[1] = (previous->layer_plane_offset[1]*pp)+(next->layer_plane_offset[1]*np);
	layer->layer_plane_offset[2] = (previous->layer_plane_offset[2]*pp)+(next->layer_plane_offset[2]*np);
	layer->layer_primitive_pos[0] = (previous->layer_primitive_pos[0]*pp)+(next->layer_primitive_pos[0]*np);
	layer->layer_primitive_pos[1] = (previous->layer_primitive_pos[1]*pp)+(next->layer_primitive_pos[1]*np);
	layer->layer_primitive_pos[2] = (previous->layer_primitive_pos[2]*pp)+(next->layer_primitive_pos[2]*np);

	float trot[3];
	//get matching rotations within the same 180 degree bounds
	Clamp_Safe_Rotation(previous->layer_primitive_rotation, next->layer_primitive_rotation, trot);
	
	layer->layer_primitive_rotation[0] = Clamp_Euler_Angle((previous->layer_primitive_rotation[0]*pp)+(trot[0]*np));
	layer->layer_primitive_rotation[1] = Clamp_Euler_Angle((previous->layer_primitive_rotation[1]*pp)+(trot[1]*np));
	layer->layer_primitive_rotation[2] = Clamp_Euler_Angle((previous->layer_primitive_rotation[2]*pp)+(trot[2]*np));
	
	Get_Plane_Dir(layer->layer_primitive_rotation, layer->layer_primitive_dir);

	
	layer->Update_Geometry_Orientation();
	
	if(previous->layer_puff_type!=next->layer_puff_type)
	{
		if(pp>np)
		{
			layer->layer_puff_type = previous->layer_puff_type;
		}
		else
		{
			layer->layer_puff_type = next->layer_puff_type;
		}
	}
	else
	{
		layer->layer_puff_type = previous->layer_puff_type;
	}

	layer->layer_puff_angle = (previous->layer_puff_angle*pp)+(next->layer_puff_angle*np);
	layer->layer_puff_range = (previous->layer_puff_range*pp)+(next->layer_puff_range*np);
	layer->layer_puff_scale = (previous->layer_puff_scale*pp)+(next->layer_puff_scale*np);
	layer->layer_puff_search_range = (int)((float)((((float)previous->layer_puff_search_range)*pp)+(((float)next->layer_puff_search_range)*np)));
	Interpolate_Relief_Info(&layer->relief_info, &previous->relief_info, &next->relief_info, pp, np);
	layer->scale = (previous->scale*pp)+(next->scale*np);
	return true;
}

bool Interpolate_All_Layers(int frame)
{
	FRAME_LAYERS *fl = &frame_layers[frame];
	int n = fl->layers.size();
	LAYER *next_layer, *previous_layer;
	int next_frame, previous_frame;
	bool res = true;
	for(int i = 0;i<n;i++)
	{
		if(!fl->layers[i]->keyframed)
		{
			next_layer = Find_Next_Layer_Keyframe(frame, fl->layers[i]->layer_id, &next_frame);
			previous_layer = Find_Previous_Layer_Keyframe(frame, fl->layers[i]->layer_id, &previous_frame);
			if(!next_layer||!previous_layer)
			{
				char msg[512];
				if(!next_layer&&!previous_layer)
				{
					sprintf(msg, "ERROR! Layer %s (internal id %i) in frame %i has no next or previous keyframes!", fl->layers[i]->name, fl->layers[i]->layer_id, frame);
				}
				else if(!next_layer)
				{
					sprintf(msg, "ERROR! Layer %s (internal id %i) in frame %i has no next keyframe!", fl->layers[i]->name, fl->layers[i]->layer_id, frame);
				}
				else
				{
					sprintf(msg, "ERROR! Layer %s (internal id %i) in frame %i has no previous keyframe!", fl->layers[i]->name, fl->layers[i]->layer_id, frame);
				}
				res = false;
			}
			else
			{
				float p = ((float)(frame-previous_frame))/(next_frame-previous_frame);
				Interpolate_Layers(fl->layers[i], previous_layer, next_layer, 1.0f-p, p);
			}
		}
	}
	return res;
}

bool Reset_Next_Layer_ID()
{
	next_layer_id = 0;
	return true;
}

bool Set_Next_Highest_Layer_ID()
{
	int n = frame_layers.size();
	next_layer_id = 0;
	for(int i = 0;i<n;i++)
	{
		int nl = frame_layers[i].layers.size();
		for(int j = 0;j<nl;j++)
		{
			if(frame_layers[i].layers[j]->layer_id>=next_layer_id)
			{
				next_layer_id = frame_layers[i].layers[j]->layer_id+1;
			}
		}
	}
	next_layer_id++;
	return true;
}

void Update_Initial_Layer_Relief()
{
	if(!active_frame_layers)return;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->Update_Initial_Layer_Relief();
	}
}

bool Clear_Modified_Layers()
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->modified = false;
	}
	return true;
}

bool Update_Modified_Layers()
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	int i;
	for(i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->modified)
		{
			active_frame_layers->layers[i]->Update_Data();
			active_frame_layers->layers[i]->ReProject_To_Geometry();
			session_altered = true;
		}
	}
	for(i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->modified)
		{
			active_frame_layers->layers[i]->Update_Outline_Pixels();
		}
	}
	return true;
}

bool ReProject_Layer(int index)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)
	{
		return false;
	}
	active_frame_layers->layers[index]->ReProject_To_Geometry();
	return true;
}


bool Flag_Layer_For_ReProject(int index)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)
	{
		return false;
	}
	active_frame_layers->layers[index]->reproject = true;
	return true;
}


bool Project_Layer_To_Model_Geometry(int index)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)
	{
		return false;
	}
	active_frame_layers->layers[index]->Project_To_Model();
	return true;
}

int New_Layer_ID()
{
	next_layer_id++;
	return next_layer_id;
}

int Num_Layers()
{
	if(!active_frame_layers)return 0;
	return active_frame_layers->layers.size();
}

int Num_Selected_Layers()
{
	return selected_layers.size();
}

int Get_Selected_Layer_ID(int index)
{
	int n = selected_layers.size();
	if(index<0||index>=n)return -1;
	return selected_layers[index]->layer_id;
}

void Set_Layer_Relief_Info(int index, RELIEF_INFO *ri, bool update_values)
{
	LAYER *l = active_frame_layers->layers[index];
	l->Set_Relief_Info(ri, update_values, true);//assume it needs reproject
}

void Get_Layer_Relief_Info(int index, RELIEF_INFO *ri)
{
	LAYER *l = active_frame_layers->layers[index];
	memcpy(ri, &l->relief_info, sizeof(RELIEF_INFO));
}


float* Get_Layer_Primitive_Pos(int layer_index)
{
	return &active_frame_layers->layers[layer_index]->layer_primitive_pos[0];
}

float* Get_Layer_Primitive_Dir(int layer_index)
{
	return &active_frame_layers->layers[layer_index]->layer_primitive_dir[0];
}

bool Free_Layers()
{
	if(!active_frame_layers)
	{
		return false;
	}
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		delete active_frame_layers->layers[i];
	}
	active_frame_layers->layers.clear();
	Update_Selected_Layers();
	return true;
}

LAYER* Find_Layer(int id)
{
	if(!active_frame_layers)return 0;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->layer_id==id)
		{
			return active_frame_layers->layers[i];
		}
	}
	return 0;
}

int Layer_ID(int index)
{
	return active_frame_layers->layers[index]->layer_id;
}

bool Layer_Name(int index, char *res)
{
	if(!active_frame_layers)
	{
		return false;
	}
	if(index<0||index>=(int)active_frame_layers->layers.size())
	{
		return false;
	}
	strcpy(res, active_frame_layers->layers[index]->name);
	return true;
}

void Set_All_Layers_Visible()
{
	if(!active_frame_layers)return;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(!active_frame_layers->layers[i]->visible)
		{
			active_frame_layers->layers[i]->visible = true;
			session_altered = true;
		}
	}
}

void Set_All_Layers_Invisible()
{
	if(!active_frame_layers)return;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->visible)
		{
			active_frame_layers->layers[i]->visible = false;
			session_altered = true;
		}
	}
}

int Find_Layer_Index(char *name)
{
	if(!active_frame_layers)return -1;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(!strcmp(active_frame_layers->layers[i]->name, name))
		{
			return i;
		}
	}
	return -1;
}

int Find_Layer_Index(int id)
{
	if(!active_frame_layers)return -1;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->layer_id==id)
		{
			return i;
		}
	}
	return -1;
}

bool Find_Layer(char *name, int *id)
{
	int index = Find_Layer_Index(name);
	if(index==-1)return false;
	*id = active_frame_layers->layers[index]->layer_id;
	return true;
}

//FIXTHIS could consolidate the bottom 2 functions
bool New_Layer(int *id, char *name, unsigned char *color)
{
	LAYER *l = new LAYER;
	if(name)
	{
		if(Find_Layer(name, &l->layer_id))
		{
			delete l;
			return false;
		}
		strcpy(l->name, name);
	}
	else
	{
		int n = active_frame_layers->layers.size();
		sprintf(l->name, "Layer %i", n);
		while(Find_Layer(l->name, &l->layer_id))
		{
			n++;sprintf(l->name, "Layer %i", n);
		}
	}
	l->layer_id = New_Layer_ID();
	if(color)
	{
		l->color[0] = ((float)color[0])/255;
		l->color[1] = ((float)color[1])/255;
		l->color[2] = ((float)color[2])/255;
	}
	else
	{
		Get_Enumerated_Color(l->layer_id, l->color);
	}
	*id = l->layer_id;
	active_frame_layers->layers.push_back(l);
	Print_To_Console("Created New Layer \"%s\"", l->name);
	session_altered = true;
	if(Is_First_Or_Last_Frame())
	{
		l->keyframed = true;
	}
	//get a fresh default plane primitive for this layer
	l->Get_Primitive();
	return true;
}

bool New_Layer_Force_ID(int id, char *name, unsigned char *color)
{
	LAYER *l = new LAYER;
	if(name)
	{
		if(Find_Layer(name, &l->layer_id))
		{
			delete l;
			return false;
		}
		strcpy(l->name, name);
	}
	else
	{
		int n = active_frame_layers->layers.size();
		sprintf(l->name, "Layer %i", n);
		while(Find_Layer(l->name, &l->layer_id))
		{
			n++;sprintf(l->name, "Layer %i", n);
		}
	}
	l->layer_id = id;
	if(color)
	{
		l->color[0] = ((float)color[0])/255;
		l->color[1] = ((float)color[1])/255;
		l->color[2] = ((float)color[2])/255;
	}
	else
	{
		Get_Enumerated_Color(l->layer_id, l->color);
	}
	active_frame_layers->layers.push_back(l);
	Print_To_Console("Created New Layer \"%s\"", l->name);
	session_altered = true;
	if(Is_First_Or_Last_Frame())
	{
		l->keyframed = true;
	}
	//get a fresh default plane primitive for this layer
	l->Get_Primitive();
	return true;
}

int Get_Layer_Index(int layer_id)
{
	if(!active_frame_layers)return -1;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->layer_id==layer_id)
		{
			return i;
		}
	}
	return -1;
}

int Get_Layer_Index(int frame, int layer_id)
{
	int n = frame_layers[frame].layers.size();
	for(int i = 0;i<n;i++)
	{
		if(frame_layers[frame].layers[i]->layer_id==layer_id)
		{
			return i;
		}
	}
	return -1;
}

void Force_Layer_RLE_Strip(int index, int y, int start_x, int end_x)
{
	active_frame_layers->layers[index]->Add_RLE_Strip(y, start_x, end_x, false);
}

bool Trim_Strip_Against_Frozen_Layers(int y, int *start_x, int *end_x, LAYER *l)
{
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]!=l)
		{
			if(active_frame_layers->layers[i]->freeze)
			{
				if(active_frame_layers->layers[i]->Strip_Within_Bounding_Box(*start_x, *end_x, y))
				{
					active_frame_layers->layers[i]->Trim_Against_RLE_Strips(y, start_x, end_x);
				}
			}
		}
	}
	return true;
}

//if it's a fresh layer no need to self-check overlaps
bool Add_Layer_RLE_Strip(int index, int y, int start_x, int end_x, bool fresh_layer)
{
	//FIXTHIS add inline function to frame to see if xy coordinate are valid
	//and another to clamp coordinates, this happens WAY too many times in the code
	if(y<0||y>=Frame_Height())
	{
		return false;
	}
	if(start_x<0)
	{
		start_x = 0;
	}
	if(end_x>Frame_Width())
	{
		end_x = Frame_Width();
	}
	if(start_x>=Frame_Width())
	{
		return false;
	}
	if(end_x<=0)
	{
		return false;
	}
	if(start_x==end_x)
	{
		return false;
	}
	LAYER *l = active_frame_layers->layers[index];
	int n = active_frame_layers->layers.size();
	//push this strip onto the stack
	orphan_strips.push_back(start_x);
	orphan_strips.push_back(end_x);
	int nstrips = orphan_strips.size();

	while(nstrips>0)
	{
		nstrips = orphan_strips.size();
		if(nstrips>0)
		{
			start_x = orphan_strips[0];
			end_x = orphan_strips[1];
			//trim this potential strip against any other layer
			//this might possible create more strips if it gets split by existing frozen layer
			Trim_Strip_Against_Frozen_Layers(y, &start_x, &end_x, l);
			if(start_x!=-1&&end_x!=-1)//check if it was totally wiped out
			{
				//replace all unfrozen layer strips with this new strip
				for(int i = 0;i<n;i++)
				{
					if(active_frame_layers->layers[i]!=l)
					{
						if(!active_frame_layers->layers[i]->freeze)
						{
							if(active_frame_layers->layers[i]->Strip_Within_Bounding_Box(start_x, end_x, y))
							{
								//active_frame_layers->layers[i]->Replace_RLE_Strips(y, start_x, end_x, l);
								active_frame_layers->layers[i]->Replace_RLE_Strips(y, start_x, end_x);
							}
						}
					}
				}
				//add the strip to the layer
				l->Add_RLE_Strip(y, start_x, end_x, fresh_layer);
			}
			orphan_strips.erase(orphan_strips.begin());
			orphan_strips.erase(orphan_strips.begin());
		}
	}
	l->modified = true;
	return true;
}

//if it's a fresh layer no need to self-check overlaps
bool Force_Add_Layer_RLE_Strip(int frame, int index, int y, int start_x, int end_x, bool fresh_layer)
{
	if(y<0||y>=Frame_Height())
	{
		return false;
	}
	if(start_x<0)
	{
		start_x = 0;
	}
	if(end_x>Frame_Width())
	{
		end_x = Frame_Width();
	}
	if(start_x>=Frame_Width())
	{
		return false;
	}
	if(end_x<=0)
	{
		return false;
	}
	if(start_x==end_x)
	{
		return false;
	}
	LAYER *l = frame_layers[frame].layers[index];
	int n = frame_layers[frame].layers.size();
	for(int i = 0;i<n;i++)
	{
		if(frame_layers[frame].layers[i]!=l)
		{
				if(frame_layers[frame].layers[i]->Strip_Within_Bounding_Box(start_x, end_x, y))
				{
					frame_layers[frame].layers[i]->Replace_RLE_Strips(y, start_x, end_x);
				}
		}
	}
	//add the strip to the layer
	l->Add_RLE_Strip(y, start_x, end_x, fresh_layer);
	return true;
}




bool Update_All_Layer_Data()
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->Update_Data();
	}
	if(render_puff_preview)Get_Preview_Puff_Pixels();
	return true;
}

bool Update_Layer_Data(int id)
{
	LAYER *l = Find_Layer(id);if(!l)return false;
	l->Update_Data();
	return true;
}


//FIXTHIS could still consolidate the bottom 2 functions
bool Delete_Layer(int frame, int id)
{
	LAYER *ml = frame_layers[frame].layers[0];//primary layer
	int n = active_frame_layers->layers.size();
	for(int i = 1;i<n;i++)
	{
		if(frame_layers[frame].layers[i]->layer_id==id)
		{
			frame_layers[frame].layers[i]->Merge_Strips(ml);
			delete frame_layers[frame].layers[i];
			frame_layers[frame].layers.erase(frame_layers[frame].layers.begin()+i);
			session_altered = true;
			return true;
		}
	}
	Update_Selected_Layers();
	return false;
}


bool Delete_Layer(int id)
{
	LAYER *ml = active_frame_layers->layers[0];//primary layer
	int n = active_frame_layers->layers.size();
	for(int i = 1;i<n;i++)
	{
		if(active_frame_layers->layers[i]->layer_id==id)
		{
			active_frame_layers->layers[i]->Merge_Strips(ml);
			delete active_frame_layers->layers[i];
			active_frame_layers->layers.erase(active_frame_layers->layers.begin()+i);
			session_altered = true;
			return true;
		}
	}
	Update_Selected_Layers();
	return false;
}

bool Absorb_Layer(int layer_id, int new_layer_id)
{
	if(layer_id==new_layer_id)return false;
	LAYER *nl = Find_Layer(new_layer_id);
	if(!nl)return false;
	nl->modified = true;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->layer_id==layer_id)
		{
			active_frame_layers->layers[i]->Merge_Strips(nl);
			delete active_frame_layers->layers[i];
			active_frame_layers->layers.erase(active_frame_layers->layers.begin()+i);
			session_altered = true;
			return true;
		}
	}
	nl->Update_Data();
	Update_Selected_Layers();
	return false;
}

bool Init_Default_Layer()
{
	Free_Layers();
	int w = Frame_Width();
	int h = Frame_Height();
	int id;
	New_Layer(&id, "Default Layer", 0);
	LAYER *l = Find_Layer(id);
	for(int i = 0;i<h;i++)
	{
		RLE_STRIP rs;
		rs.y = i;
		rs.start_x = 0;
		rs.end_x = w;
		l->strips.push_back(rs);
	}
	Freeze_Layer(0, false);
	return true;
}

void Set_Selected_Layers_Depth_ID_Float(float f)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->layer_primitive_pos[2] = f*depth_increment;
		selected_layers[i]->ReProject_To_Geometry();
		if(selected_layers[i]->scene_primitive->primitive->primitive_type==pt_PLANE)
		{
			selected_layers[i]->ReCalc_Planar_Origin_Point();//needed for proper rotation around pivot point
		}
		session_altered = true;
		Update_Selection_Status_Depth(selected_layers[i]->layer_primitive_pos[2]);
	}
	if(render_puff_preview)Update_Puff_Preview_Center();
	redraw_frame = true;
}

void Set_All_Layers_Depth_ID_Float(float f)
{
	if(!active_frame_layers)return;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->layer_primitive_pos[2] = f*depth_increment;
		active_frame_layers->layers[i]->ReProject_To_Geometry();
		if(active_frame_layers->layers[i]->selected)
		{
			Update_Selection_Status_Depth(active_frame_layers->layers[i]->layer_primitive_pos[2]);
		}
		if(active_frame_layers->layers[i]->scene_primitive->primitive->primitive_type==pt_PLANE)
		{
			active_frame_layers->layers[i]->ReCalc_Planar_Origin_Point();//needed for proper rotation around pivot point
		}
		session_altered = true;
	}
	if(render_puff_preview)Update_Puff_Preview_Center();
	redraw_frame = true;
}

//FIXTHIS this could be somewhere else externally controlled
void Tab_Selection()
{
	if(!active_frame_layers)return;
	int n = active_frame_layers->layers.size();
	int last = -1;
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->selected)
		{
			Select_Layer(i, false);
			last = i;
		}
	}
	if(last!=-1)
	{
		last++;
		if(last>=(int)active_frame_layers->layers.size()-1)
		{
			last = 0;
		}
		Select_Layer(last, true);
	}
	selection_changed = true;
}

//FIXTHIS it could be elsewhere externally controlled
void Select_All_Layers(bool b)
{
	if(!active_frame_layers)return;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->selected!=b)
		{
			active_frame_layers->layers[i]->selected = b;
			if(b)active_frame_layers->layers[i]->selection_count = 0;
			redraw_frame = true;
			redraw_edit_window = true;
			session_altered = true;
		}
	}
	selection_changed = true;
	Update_Selected_Layers();
}

//FIXTHIS also could be external control
void Adjust_Selected_Layers_Depth_ID_Float(float f)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		if(selected_layers[i]->layer_primitive_pos[2]+f*depth_increment>0)
		{
			selected_layers[i]->layer_primitive_pos[2] += f*depth_increment;
			selected_layers[i]->ReProject_To_Geometry();
			if(selected_layers[i]->scene_primitive->primitive->primitive_type==pt_PLANE)
			{
				selected_layers[i]->ReCalc_Planar_Origin_Point();//needed for proper rotation around pivot point
			}
			session_altered = true;
			Update_Selection_Status_Depth(selected_layers[i]->layer_primitive_pos[2]);
		}
	}
	if(render_puff_preview)Update_Puff_Preview_Center();
	redraw_frame = true;
}


//same here
bool Set_Layer_Depth_ID_Float(int layer_id, float f)
{
	LAYER *l = Find_Layer(layer_id);if(!l)return false;
	l->layer_primitive_pos[2] = f*depth_increment;
	l->ReProject_To_Geometry();
	session_altered = true;
	if(render_puff_preview)Update_Puff_Preview_Center();
	Update_Selection_Status_Depth(l->layer_primitive_pos[2]);
	return true;
}

bool Get_Layer_Geometry_Type(int index, int *type)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)
	{
		return false;
	}
	*type = active_frame_layers->layers[index]->Get_Primitive_Type();
	return true;
}

bool Get_Layer_Primitive_Transform(int index, float *pos, float *rot)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)
	{
		return false;
	}
	pos[0] = active_frame_layers->layers[index]->layer_primitive_pos[0];
	pos[1] = active_frame_layers->layers[index]->layer_primitive_pos[1];
	pos[2] = active_frame_layers->layers[index]->layer_primitive_pos[2];
	rot[0] = active_frame_layers->layers[index]->layer_primitive_rotation[0];
	rot[1] = active_frame_layers->layers[index]->layer_primitive_rotation[1];
	rot[2] = active_frame_layers->layers[index]->layer_primitive_rotation[2];
	return true;
}

bool Layer_Geometry_Inverted(int index)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)
	{
		return false;
	}
	return active_frame_layers->layers[index]->scene_primitive->inverted;
}

bool Layer_Geometry_Visible(int index)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)
	{
		return false;
	}
	return active_frame_layers->layers[index]->scene_primitive->visible;
}

bool Get_Num_Layer_Outline_Pixels(int index, int *res)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)return false;
	LAYER *l = active_frame_layers->layers[index];
	*res = l->outline.size()/2;
	return true;
}

bool Get_Layer_Outline_Pixel(int index, int pixel, int *x, int *y)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)return false;
	LAYER *l = active_frame_layers->layers[index];
	if(pixel>=(int)l->outline.size()/2)
	{
		return false;
	}
	*x = l->outline[pixel*2];
	*y = l->outline[(pixel*2)+1];
	return true;
}

bool Get_Num_Layer_Pixels(int index, int *res)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)return false;
	LAYER *l = active_frame_layers->layers[index];
	*res = l->num_pixel_indices;
	return true;
}

bool Get_Layer_Pixel(int index, int pixel, int *x, int *y)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)return false;
	LAYER *l = active_frame_layers->layers[index];
	if(pixel>=l->num_pixel_indices)
	{
		return false;
	}
	return frame->IndexToPixel(l->pixel_indices[index], x, y);
}

bool Get_Layer_AABB(int index, int *lx, int *ly, int *hx, int *hy)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)return false;
	LAYER *l = active_frame_layers->layers[index];
	*lx = l->lx;
	*ly = l->ly;
	*hx = l->hx;
	*hy = l->hy;
	return true;
}



//FIXTHIS externally controlled
void Set_Selected_Layers_Scale(float scale)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->scale = scale;
		selected_layers[i]->ReProject_To_Geometry();
		session_altered = true;
	}
	if(render_puff_preview)Update_Puff_Preview_Center();
	redraw_frame = true;
}


//and the same
void Adjust_Selected_Layers_Scale(float scale)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->scale += scale;
		selected_layers[i]->ReProject_To_Geometry();
		session_altered = true;
	}
	if(render_puff_preview)Update_Puff_Preview_Center();
	redraw_frame = true;
}

bool Render_Layers_2D(float point_size, int lx, int hx, int ly, int hy)
{
	if(!active_frame_layers)return false;
	if(!render_layer_mask&&!render_layer_outlines)return false;
	int n = active_frame_layers->layers.size();
	int i;
	if(render_layer_mask)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		if(!show_layer_colors)
		{
			glColor4f(0,0,0,0.25f);
		}
		glLineWidth(0.25f);
		glBegin(GL_LINES);
		for(i = 0;i<n;i++)
		{
			if(active_frame_layers->layers[i]->Within_Bounds(lx, hx, ly, hy)&&active_frame_layers->layers[i]->freeze)
			{
				if(show_layer_colors)
				{
					if(active_frame_layers->layers[i]->selected)
					{
						glColor4f(active_frame_layers->layers[i]->color[0],active_frame_layers->layers[i]->color[1],active_frame_layers->layers[i]->color[2],0.5f);
					}
					else
					{
						glColor4f(active_frame_layers->layers[i]->color[0],active_frame_layers->layers[i]->color[1],active_frame_layers->layers[i]->color[2],0.25f);
					}
				}
				active_frame_layers->layers[i]->Render_Mask_2D(point_size);
			}
		}
		glEnd();
		glLineWidth(1);
		glDisable(GL_BLEND);
	}
	if(render_layer_outlines)
	{
		for(i = 0;i<n;i++)
		{
			if(active_frame_layers->layers[i]->Within_Bounds(lx, hx, ly, hy))
			{
				active_frame_layers->layers[i]->Render_Outline_Vertices_2D(point_size);
			}
		}
		glPointSize(1);
	}
	glColor3f(1,1,1);
	return true;
}

bool Render_Layers_3D()
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->visible)
		{
			if(active_frame_layers->layers[i]->selected&&render_puff_preview&&active_frame_layers->layers[i]->layer_puff_type!=-1)
			{
				active_frame_layers->layers[i]->Render_Puff_Preview_Tri_Indices();
			}
			else
			{
				active_frame_layers->layers[i]->Render_Tri_Indices();
			}
		}
	}
	return true;
}

void Render_Layers_For_Selection()
{
	if(!frame)
	{
		return;
	}
	glPushMatrix();

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, frame->vertices);
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		glLoadName(active_frame_layers->layers[i]->layer_id);
		//glLoadName(i);
		if(active_frame_layers->layers[i]->selected&&render_puff_preview&&active_frame_layers->layers[i]->layer_puff_type!=-1)
		{
			active_frame_layers->layers[i]->Render_Puff_Preview_Tri_Indices();
		}
		else
		{
			active_frame_layers->layers[i]->Render_Tri_Indices(false);
		}
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}


bool Render_Selection_Outline_3D()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		if(selected_layers[i]->visible)selected_layers[i]->Render_Outline_Indices_3D();
	}
	return true;
}

//FIXTHIS move this up the where the primitive rotation is set
void Set_Selection_Rotation(float *normal, float *rotation)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->Set_Rotation(normal, rotation);
		session_altered = true;
		Update_Selection_Status_Orientation(selected_layers[i]->layer_primitive_rotation[0], selected_layers[i]->layer_primitive_rotation[1]);
	}
	if(render_puff_preview)
	{
		Update_Puff_Preview_Center();
	}
}

bool Set_Selection_Transform(float *pos, float *normal, float *rotation)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->layer_primitive_pos[0] = pos[0];
		selected_layers[i]->layer_primitive_pos[1] = pos[1];
		selected_layers[i]->layer_primitive_pos[2] = pos[2];
		selected_layers[i]->Set_Rotation(normal, rotation);
		session_altered = true;
		Update_Selection_Status_Orientation(selected_layers[i]->layer_primitive_rotation[0], selected_layers[i]->layer_primitive_rotation[1]);
	}
	if(render_puff_preview)
	{
		Update_Puff_Preview_Center();
	}
	return true;
}

bool Get_Selection_Transform(float *pos, float *normal, float *rotation)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		pos[0] = selected_layers[i]->layer_primitive_pos[0];
		pos[1] = selected_layers[i]->layer_primitive_pos[1];
		pos[2] = selected_layers[i]->layer_primitive_pos[2];
		rotation[0] = selected_layers[i]->layer_primitive_rotation[0];
		rotation[1] = selected_layers[i]->layer_primitive_rotation[1];
		rotation[2] = selected_layers[i]->layer_primitive_rotation[2];
		normal[0] = selected_layers[i]->layer_primitive_dir[0];
		normal[1] = selected_layers[i]->layer_primitive_dir[1];
		normal[2] = selected_layers[i]->layer_primitive_dir[2];
		return true;
	}
	return false;
}

//used ONLY for auto-alignment
bool Project_Indexed_Layer_To_Plane(int index, float *pos, float *dir, float *rotation, float *offset, int origin_type, bool reproject)
{
	if(!active_frame_layers)return false;
	//bad hack into general function
	active_frame_layers->layers[index]->Set_Plane_Projection(pos, dir, rotation, offset, origin_type, reproject);
	if(!reproject)
	{
		//that doesn't call THIS
		active_frame_layers->layers[index]->Update_Geometry_Orientation();
	}
	session_altered = true;
	if(render_puff_preview)
	{
		Update_Puff_Preview_Center();
	}
	return true;
}


bool Get_Indexed_Layer_Plane(int index, float *pos, float *dir, float *rotation, float *offset, int *origin_type)
{
	if(!active_frame_layers)return false;
	active_frame_layers->layers[index]->Get_Plane_Projection(pos, dir, rotation, offset, origin_type);
	return true;
}

//FIXTHIS external control
void Project_Everything_To_Plane(float *pos, float *dir, float *rotation, float *offset, int origin_type)
{
	if(!active_frame_layers)return;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->Set_Plane_Projection(pos, dir, rotation, offset, origin_type, true);
	}
	if(frame)session_altered = true;
}

bool Get_Selection_Puff_Info(int *search_range, float *scale, float *range, int *type, bool *directional, float *angle)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->visible&&active_frame_layers->layers[i]->selected&&active_frame_layers->layers[i]->layer_puff_type!=-1)
		{
			*search_range = active_frame_layers->layers[i]->layer_puff_search_range;
			*scale = active_frame_layers->layers[i]->layer_puff_scale;
			*range = active_frame_layers->layers[i]->layer_puff_range;
			*type = active_frame_layers->layers[i]->layer_puff_type;
			*directional = active_frame_layers->layers[i]->layer_puff_directional;
			*angle = active_frame_layers->layers[i]->layer_puff_angle;
			return true;
		}
	}
	return false;
}

void Get_Selection_Rotation(float *dir, float *rotation, int *origin_type)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->Get_Rotation(dir, rotation, origin_type);
		return;
	}
}

bool Project_Layer_To_Plane(int layer_id, float *pos, float *dir, float *rotation, float *offset, int origin_type)
{
	LAYER *l = Find_Layer(layer_id);if(!l)return false;
	l->Set_Plane_Projection(pos, dir, rotation, offset, origin_type, true);
	return true;
}

bool Puff_Layer(int layer_index, int search_range, float puff_scale, float puff_range, int puff_type, bool directional, float angle)
{
	return active_frame_layers->layers[layer_index]->Set_Puff_Info(search_range, puff_scale, puff_range, puff_type, directional, angle);
}

bool Get_Layer_Center(int layer_id, float *pos)
{
	LAYER *l = Find_Layer(layer_id);if(!l)return false;
	pos[0] = l->final_layer_center[0];
	pos[1] = l->final_layer_center[1];
	pos[2] = l->final_layer_center[2];
	return true;
}

bool Get_Indexed_Layer_Center(int index, float *pos)
{
	LAYER *l = active_frame_layers->layers[index];
	pos[0] = l->final_layer_center[0];
	pos[1] = l->final_layer_center[1];
	pos[2] = l->final_layer_center[2];
	return true;
}

bool ReCalc_Layer_Center(int layer_id, float *pos)
{
	LAYER *l = Find_Layer(layer_id);if(!l)return false;
	l->ReCalc_Layer_Center();
	return Get_Layer_Center(layer_id, pos);
}

bool ReCalc_And_Set_Layer_Center(int layer_id)
{
	LAYER *l = Find_Layer(layer_id);if(!l)return false;
	l->ReCalc_Layer_Center();
	l->layer_primitive_pos[0] = l->origin_layer_center[0];
	l->layer_primitive_pos[1] = l->origin_layer_center[1];
	l->layer_primitive_pos[2] = l->origin_layer_center[2];
	return true;
}

bool Get_Frame_Center(float *pos)
{
	pos[0] = scene_center[0];
	pos[1] = scene_center[1];
	pos[2] = scene_center[2];
	return true;
}

//FIXTHIS is this used?
//YES it is used to find the total overall extents of a selectionb, could be multiple
//MAYBE use bounding boxes of selected layers
//USE SELECTED LIST!
bool Get_Selection_Extents(int *lx, int *ly, int *hx, int *hy)
{
	if(!active_frame_layers)return false;
	int num_pixels = 0;
	*lx = -1;
	*ly = -1;
	*hx = -1;
	*hy = -1;
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		int k = selected_layers[i]->strips.size();
		for(int j = 0;j<k;j++)
		{
			if(*lx==-1)
			{
				*lx = selected_layers[i]->strips[j].start_x;
				*hx = selected_layers[i]->strips[j].end_x;
				*ly = *hy = selected_layers[i]->strips[j].y;
			}
			else
			{
				if(*lx>selected_layers[i]->strips[j].start_x)*lx=selected_layers[i]->strips[j].start_x;
				if(*hx<selected_layers[i]->strips[j].end_x)*hx=selected_layers[i]->strips[j].end_x;
				if(*ly>selected_layers[i]->strips[j].y)*ly=selected_layers[i]->strips[j].y;
				if(*hy<selected_layers[i]->strips[j].y)*hy=selected_layers[i]->strips[j].y;
			}
		}
	}
	if(*lx==-1)
	{
		return false;
	}
	return true;
}

//used to find matching top or bottom extents
int Find_Matching_X(int y)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		int k = selected_layers[i]->strips.size();
		for(int j = 0;j<k;j++)
		{
			if(selected_layers[i]->strips[j].y==y)
			{
				return (selected_layers[i]->strips[j].start_x + selected_layers[i]->strips[j].end_x)/2;
			}
		}
	}
	return -1;
}


//used to find matching left or right extents
int Find_Matching_Y(int x)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		int k = selected_layers[i]->strips.size();
		for(int j = 0;j<k;j++)
		{
			if(x>=selected_layers[i]->strips[j].start_x&&x<=selected_layers[i]->strips[j].end_x)
			{
				return selected_layers[i]->strips[j].y;
			}
		}
	}
	return -1;
}

bool Set_Selection_Origin_Top()
{
	int lx, ly, hx, hy;
	float pos[3];
	if(!Get_Selection_Extents(&lx, &ly, &hx, &hy))
	{
		return false;
	}
	int x = Find_Matching_X(hy);
	int y = hy;
	int k = (y*frame->width)+x;
	LAYER *l = Find_Layer(frame->Get_Pixel_Layer(x, y));
	l->Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(k), pos);
	l->layer_primitive_pos[0] = pos[0];
	l->layer_primitive_pos[1] = pos[1];
	l->layer_primitive_pos[2] = pos[2];
	return true;
}

bool Set_Selection_Origin_Left()
{
	int lx, ly, hx, hy;
	float pos[3];
	if(!Get_Selection_Extents(&lx, &ly, &hx, &hy))
	{
		return false;
	}
	int x = lx;
	int y = Find_Matching_Y(lx);
	int k = (y*frame->width)+x;
	LAYER *l = Find_Layer(frame->Get_Pixel_Layer(x, y));
	l->Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(k), pos);
	l->layer_primitive_pos[0] = pos[0];
	l->layer_primitive_pos[1] = pos[1];
	l->layer_primitive_pos[2] = pos[2];
	return true;
}




//FIXTHIS could consolidate the 2 bottom functions

bool Set_Selection_Origin_Center()
{
	float pos[3];
	if(!active_frame_layers)return false;
	float x = 0;
	float y = 0;
	int i;
	float average_pos[3] = {0,0,0};
	int n = selected_layers.size();
	int num_selected_layers = n;
	if(num_selected_layers==0)
	{
		return false;
	}
	for(i = 0;i<n;i++)
	{
		average_pos[0] += selected_layers[i]->origin_layer_center[0];
		average_pos[1] += selected_layers[i]->origin_layer_center[1];
		average_pos[2] += selected_layers[i]->origin_layer_center[2];
	}

	pos[0] = average_pos[0]/num_selected_layers;
	pos[1] = average_pos[1]/num_selected_layers;
	pos[2] = average_pos[2]/num_selected_layers;
	for(i = 0;i<n;i++)
	{
		selected_layers[i]->layer_primitive_pos[0] = pos[0];
		selected_layers[i]->layer_primitive_pos[1] = pos[1];
		selected_layers[i]->layer_primitive_pos[2] = pos[2];
	}
	return true;
}

bool Get_Selection_Origin_Center(float *pos)
{
	if(!active_frame_layers)return false;
	float x = 0;
	float y = 0;
	int i;
	float average_pos[3] = {0,0,0};
	int n = selected_layers.size();
	int num_selected_layers = n;
	if(num_selected_layers==0)
	{
		return false;
	}
	for(i = 0;i<n;i++)
	{
		average_pos[0] += selected_layers[i]->origin_layer_center[0];
		average_pos[1] += selected_layers[i]->origin_layer_center[1];
		average_pos[2] += selected_layers[i]->origin_layer_center[2];
	}
	pos[0] = average_pos[0]/num_selected_layers;
	pos[1] = average_pos[1]/num_selected_layers;
	pos[2] = average_pos[2]/num_selected_layers;
	return true;
}


bool Set_Selection_Origin_Right()
{
	float pos[3];
	int lx, ly, hx, hy;
	if(!Get_Selection_Extents(&lx, &ly, &hx, &hy))
	{
		return false;
	}
	int x = hx-1;
	int y = Find_Matching_Y(hx);
	int k = (y*frame->width)+x;
	LAYER *l = Find_Layer(frame->Get_Pixel_Layer(x, y));
	l->Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(k), pos);
	l->layer_primitive_pos[0] = pos[0];
	l->layer_primitive_pos[1] = pos[1];
	l->layer_primitive_pos[2] = pos[2];
	return true;
}

bool Set_Selection_Origin_Bottom()
{
	float pos[3];
	int lx, ly, hx, hy;
	if(!Get_Selection_Extents(&lx, &ly, &hx, &hy))
	{
		return false;
	}
	int x = Find_Matching_X(ly);
	int y = ly;
	int k = (y*frame->width)+x;
	LAYER *l = Find_Layer(frame->Get_Pixel_Layer(x, y));
	l->Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(k), pos);
	l->layer_primitive_pos[0] = pos[0];
	l->layer_primitive_pos[1] = pos[1];
	l->layer_primitive_pos[2] = pos[2];
	return true;
}

bool Get_Selection_Center(float *pos)
{
	if(!active_frame_layers)return false;
	float x = 0;
	float y = 0;
	int i;
	float average_pos[3] = {0,0,0};
	int n = selected_layers.size();
	int num_selected_layers = n;
	if(num_selected_layers==0)
	{
		return false;
	}
	for(i = 0;i<n;i++)
	{
		average_pos[0] += selected_layers[i]->origin_layer_center[0];
		average_pos[1] += selected_layers[i]->origin_layer_center[1];
		average_pos[2] += selected_layers[i]->origin_layer_center[2];
	}

	pos[0] = average_pos[0]/num_selected_layers;
	pos[1] = average_pos[1]/num_selected_layers;
	pos[2] = average_pos[2]/num_selected_layers;
	return true;
}


bool Get_Selection_Bottom(float *pos)
{
	int lx, ly, hx, hy;
	if(!Get_Selection_Extents(&lx, &ly, &hx, &hy))
	{
		return false;
	}
	int x = Find_Matching_X(ly);
	int y = ly;
	int k = (y*frame->width)+x;
	LAYER *l = Find_Layer(frame->Get_Pixel_Layer(x, y));
	l->Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(k), pos);
	return true;
}

bool Get_Selection_Top(float *pos)
{
	int lx, ly, hx, hy;
	if(!Get_Selection_Extents(&lx, &ly, &hx, &hy))
	{
		return false;
	}
	int x = Find_Matching_X(hy);
	int y = hy;
	int k = (y*frame->width)+x;
	LAYER *l = Find_Layer(frame->Get_Pixel_Layer(x, y));
	l->Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(k), pos);
	return true;
}

bool Set_Selection_Plane_Origin(int type)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->layer_plane_origin_type = type;
	}
	if(type==PLANE_ORIGIN_SELECTION_TOP)
	{
		Set_Selection_Origin_Top();
		return true;
	}
	else if(type==PLANE_ORIGIN_SELECTION_LEFT)
	{
		Set_Selection_Origin_Left();
		return true;
	}
	else if(type==PLANE_ORIGIN_SELECTION_CENTER)
	{
		Set_Selection_Origin_Center();
		return true;
	}
	else if(type==PLANE_ORIGIN_SELECTION_RIGHT)
	{
		Set_Selection_Origin_Right();
		return true;
	}
	else if(type==PLANE_ORIGIN_SELECTION_BOTTOM)
	{
		Set_Selection_Origin_Bottom();
		return true;
	}
	return false;
}

void Update_GUI_Selection_Info()
{
	List_Selected_Layers();
	Set_Depth_Slider_To_Selection();
	Set_Scale_Slider_To_Selection();
	Set_Relief_To_Selection();
	Set_Geometry_Dialog_To_Selection();
	Set_Plane_Dialog_To_Selection();
	Set_Halo_Tool_To_Selection();
	//FIXTHIS just update it and add is_visible flag to dialog
	if(render_puff_preview)
	{
		Set_Contour_To_Selection();
		Get_Preview_Puff_Pixels();
	}
	Update_Selection_Status_Info();
}

void Update_Selection_Counts()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->selection_count++;
	}
}

bool Select_Layer(int index, bool b)
{
	if(active_frame_layers->layers[index]->selected==b)return true;
	active_frame_layers->layers[index]->selected = b;
	Update_Selected_Layers();
	if(b)
	{
		Update_Selection_Counts();
		active_frame_layers->layers[index]->selection_count = 0;
	}
	redraw_frame = true;
	redraw_edit_window = true;
	session_altered = true;
	selection_changed = true;
	return true;
}

int Get_Oldest_Selected_Layer(int *index)
{
	int highest_cnt = -1;
	*index = -1;
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		if(highest_cnt<selected_layers[i]->selection_count)
		{
			highest_cnt = selected_layers[i]->selection_count;
			*index = i;
		}
	}
	if(*index==-1)
	{
		return -1;
	}
	*index = selected_layer_indices[*index];
	return active_frame_layers->layers[*index]->layer_id;
}

int Get_Most_Recently_Selected_Layer(int *index)
{
	int lowest_cnt = 9999;
	*index = -1;
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		if(lowest_cnt>selected_layers[i]->selection_count)
		{
			lowest_cnt = selected_layers[i]->selection_count;
			*index = i;
		}
	}
	if(*index==-1)
	{
		return -1;
	}
	*index = selected_layer_indices[*index];
	return active_frame_layers->layers[*index]->layer_id;
}

bool Get_Layer_Depth_ID_Float(int index, float *res)
{
	LAYER *l = active_frame_layers->layers[index];
	*res = Get_Depth_ID_Float(active_frame_layers->layers[index]->layer_primitive_pos[2]);
	return true;
}

int Num_RLE_Strips_In_Layer(int index)
{
	return active_frame_layers->layers[index]->strips.size();
}

RLE_STRIP* Get_RLE_Strip(int layer_index, int strip_index)
{
	return &active_frame_layers->layers[layer_index]->strips[strip_index];
}

bool Get_Layer_Scale(int index, float *res)
{
	*res = active_frame_layers->layers[index]->scale;
	return true;
}

bool Layer_Is_Selected(int index)
{
 	return active_frame_layers->layers[index]->selected;
}

bool Clear_Layer(int index)
{
	LAYER *l = active_frame_layers->layers[index];
	l->strips.clear();
	l->Update_Data();
	return true;
}

int Get_Layer_ID(int index)
{
	if(!active_frame_layers)return -1;
	int n = active_frame_layers->layers.size();
	if(index<0||index>=n)return -1;
	return active_frame_layers->layers[index]->layer_id;
}

bool Toggle_Select_Layer(int id)
{
	LAYER *l = Find_Layer(id);if(!l)return false;
	Select_Layer(Get_Layer_Index(id), !l->selected);
	return true;
}

void Render_3D_Layer_Outline(int layer_id)
{
	LAYER *l = Find_Layer(layer_id);if(!l)return;
	glPointSize(3);
	glColor3f(1,0,0);
	glPushMatrix();
	l->Render_Outline_Indices_3D();
	glPopMatrix();
	glColor3f(1,1,1);
	glPointSize(1);
}


bool Get_Single_Selection_Info(char *name, char *geotype, float *depth, float *rotation)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		LAYER *l = selected_layers[i];
		strcpy(name, l->name);
		int gt = l->scene_primitive->primitive->primitive_type;
		if(gt==pt_PLANE)strcpy(geotype, "Plane");
		else if(gt==pt_SPHERE)strcpy(geotype, "Sphere");
		else if(gt==pt_CYLINDER)strcpy(geotype, "Cylinder");
		else if(gt==pt_BOX)strcpy(geotype, "Box");
		else if(gt==pt_TRIMESH)strcpy(geotype, "Model");
		else {strcpy(geotype, "Unknown");}
		*depth = l->layer_primitive_pos[2];
		rotation[0] = l->layer_primitive_rotation[0];
		rotation[1] = l->layer_primitive_rotation[1];
		return true;
	}
	return false;
}


void Render_Outlined_Layer_Outlines()
{
	if(!active_frame_layers)return;
	glPointSize(3);
	glColor3f(1,0,1);
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->visible&&active_frame_layers->layers[i]->render_outline)
		{
			active_frame_layers->layers[i]->Render_Outline_Indices_3D();
		}
	}
	glColor3f(1,1,1);
	glPointSize(1);
}

void Render_Speckled_Layer_Speckles()
{
	if(!active_frame_layers)return;
	glPointSize(3);
	glColor3f(0,1,1);
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->visible&&active_frame_layers->layers[i]->render_speckles)
		{
			active_frame_layers->layers[i]->Render_Speckles(speckle_skip);
		}
	}
	glColor3f(1,1,1);
	glPointSize(1);
}

void Render_Normals()
{
	if(!active_frame_layers)return;
	glLineWidth(3);
	glColor3f(0,0,1);
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->visible)
		{
			active_frame_layers->layers[i]->Render_Normal();
		}
	}
	glLineWidth(1);
}


bool Freeze_Layer(int index, bool b)
{
	if(active_frame_layers->layers[index]->freeze!=b)session_altered = true;
	active_frame_layers->layers[index]->freeze = b;
	return true;
}

bool Hide_Layer(int index, bool b)
{
	if(active_frame_layers->layers[index]->visible!=b)session_altered = true;
	active_frame_layers->layers[index]->visible = !b;
	redraw_frame = true;
	return true;
}

bool Keyframe_Layer(int index, bool b)
{
	if(active_frame_layers->layers[index]->keyframed!=b)session_altered = true;
	active_frame_layers->layers[index]->keyframed = b;
	return true;
}

bool Outline_Layer(int index, bool b)
{
	if(active_frame_layers->layers[index]->render_outline!=b)session_altered = true;
	active_frame_layers->layers[index]->render_outline = b;
	redraw_frame = true;
	return true;
}

bool Speckle_Layer(int index, bool b)
{
	if(active_frame_layers->layers[index]->render_speckles!=b)session_altered = true;
	active_frame_layers->layers[index]->render_speckles = b;
	redraw_frame = true;
	return true;
}

bool Toggle_Freeze_Selection()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->freeze = !selected_layers[i]->freeze;
	}
	Update_Selection_Info();
	return true;
}

//used by anchor point system to flag/unflag layers that need extra projection due to anchor points

bool Layer_Has_Anchor_Points(int layer_id)
{
	int index = Get_Layer_Index(layer_id);
	if(index==-1)return false;
	return active_frame_layers->layers[index]->has_anchor_points;
}

bool Set_Layer_Has_Anchor_Points(int layer_id, bool b)
{
	int index = Get_Layer_Index(layer_id);
	if(index==-1)return false;
	if(active_frame_layers->layers[index]->has_anchor_points!=b)
	{
		active_frame_layers->layers[index]->reproject = true;
	}
	active_frame_layers->layers[index]->has_anchor_points = b;
	return true;
}

//used by halo system to flag/unflag layers that have halos that need projection updates

bool Layer_Has_Halo(int layer_id)
{
	int index = Get_Layer_Index(layer_id);
	if(index==-1)return false;
	return active_frame_layers->layers[index]->has_halo;
}

bool Set_Layer_Has_Halo(int layer_id, bool b)
{
	int index = Get_Layer_Index(layer_id);
	if(index==-1)return false;
	active_frame_layers->layers[index]->has_halo = b;
	return true;
}

bool Layer_Is_Frozen(int index)
{
	return active_frame_layers->layers[index]->freeze;
}

bool Layer_Is_Hidden(int index)
{
	return !active_frame_layers->layers[index]->visible;
}

bool Layer_Is_Keyframed(int index)
{
	return active_frame_layers->layers[index]->keyframed;
}

bool Layer_Is_Outlined(int index)
{
	return active_frame_layers->layers[index]->render_outline;
}

bool Layer_Is_Speckled(int index)
{
	return active_frame_layers->layers[index]->render_speckles;
}

bool Load_Neighbor_Blend(FILE *f, NBLEND *b)
{
	fread(b, sizeof(NBLEND), 1, f);
	return true;
}

bool Save_Neighbor_Blend(FILE *f, NBLEND *b)
{
	fwrite(b, sizeof(NBLEND), 1, f);
	return true;
}

bool Save_Layer(LAYER *l, FILE *f)
{
	fwrite(&l->name, sizeof(char), 64, f);
	fwrite(&l->layer_id, sizeof(int), 1, f);
	fwrite(&l->num_neighbor_blends, sizeof(int), 1, f);
	fwrite(&l->color, sizeof(float), 3, f);
	fwrite(&l->num_tri_indices, sizeof(int), 1, f);
	fwrite(&l->num_pixel_indices, sizeof(int), 1, f);
	fwrite(&l->scale, sizeof(float), 1, f);
	fwrite(&l->keyframed, sizeof(bool), 1, f);
	fwrite(&l->freeze, sizeof(bool), 1, f);
	fwrite(&l->visible, sizeof(bool), 1, f);
	fwrite(l->layer_primitive_pos, sizeof(float), 3, f);
	fwrite(l->layer_primitive_dir, sizeof(float), 3, f);
	fwrite(l->layer_primitive_rotation, sizeof(float), 3, f);
	fwrite(l->layer_plane_offset, sizeof(float), 3, f);
	fwrite(&l->layer_plane_origin_type, sizeof(int), 1, f);
	fwrite(&l->layer_puff_search_range, sizeof(int), 1, f);
	fwrite(&l->layer_puff_scale, sizeof(float), 1, f);
	fwrite(&l->layer_puff_range, sizeof(float), 1, f);
	fwrite(&l->layer_puff_type, sizeof(int), 1, f);
	fwrite(&l->layer_puff_directional, sizeof(bool), 1, f);
	fwrite(&l->layer_puff_angle, sizeof(float), 1, f);
	fwrite(&l->relief_info, sizeof(RELIEF_INFO), 1, f);
	fwrite(l->extra_data, sizeof(float), 24, f);
	fwrite(&l->soften_edge_range, sizeof(float), 1, f);
	fwrite(&l->soften_edge_strength, sizeof(int), 1, f);
	int n = l->strips.size();
	fwrite(&n, sizeof(int), 1, f);
	int i;
	for(i = 0;i<n;i++)
	{
		fwrite(&l->strips[i].start_x, sizeof(int), 1, f);
		fwrite(&l->strips[i].end_x, sizeof(int), 1, f);
		fwrite(&l->strips[i].y, sizeof(int), 1, f);
	}
	for(i = 0;i<l->num_neighbor_blends;i++)
	{
		Save_Neighbor_Blend(f, &l->neighbor_blends[i]);
	}
	return true;
}

bool Load_Layer(FILE *f)
{
	LAYER *l = new LAYER;
	fread(&l->name, sizeof(char), 64, f);
	fread(&l->layer_id, sizeof(int), 1, f);
	fread(&l->num_neighbor_blends, sizeof(int), 1, f);
	fread(&l->color, sizeof(float), 3, f);
	fread(&l->num_tri_indices, sizeof(int), 1, f);
	fread(&l->num_pixel_indices, sizeof(int), 1, f);
	fread(&l->scale, sizeof(float), 1, f);
	fread(&l->keyframed, sizeof(bool), 1, f);
	fread(&l->freeze, sizeof(bool), 1, f);
	fread(&l->visible, sizeof(bool), 1, f);
	fread(l->layer_primitive_pos, sizeof(float), 3, f);
	fread(l->layer_primitive_dir, sizeof(float), 3, f);
	fread(l->layer_primitive_rotation, sizeof(float), 3, f);
	fread(l->layer_plane_offset, sizeof(float), 3, f);
	fread(&l->layer_plane_origin_type, sizeof(int), 1, f);
	fread(&l->layer_puff_search_range, sizeof(int), 1, f);
	fread(&l->layer_puff_scale, sizeof(float), 1, f);
	fread(&l->layer_puff_range, sizeof(float), 1, f);
	fread(&l->layer_puff_type, sizeof(int), 1, f);
	fread(&l->layer_puff_directional, sizeof(bool), 1, f);
	fread(&l->layer_puff_angle, sizeof(float), 1, f);
	fread(&l->relief_info, sizeof(RELIEF_INFO), 1, f);
	fread(l->extra_data, sizeof(float), 24, f);
	fread(&l->soften_edge_range, sizeof(float), 1, f);
	fread(&l->soften_edge_strength, sizeof(int), 1, f);
	int n = 0;
	fread(&n, sizeof(int), 1, f);
	int total_rle_size = 3*n;

	int *rle_buffer = new int[total_rle_size];
	fread(rle_buffer, sizeof(int), total_rle_size, f);

	int sx, ex, y;

	if(flip_loaded_layers_vertical&&flip_loaded_layers_horizontal)
	{
		for(int i = 0;i<n;i++)
		{
			sx = rle_buffer[(i*3)];
			ex = rle_buffer[(i*3)+1];
			y = rle_buffer[(i*3)+2];
			rle_buffer[(i*3)+2] = (frame->height-1)-y;
			rle_buffer[(i*3)] = (frame->width)-ex;
			rle_buffer[(i*3)+1] = (frame->width)-sx;
		}
	}
	else if(flip_loaded_layers_vertical)
	{
		for(int i = 0;i<n;i++)
		{
			sx = rle_buffer[(i*3)];
			ex = rle_buffer[(i*3)+1];
			y = rle_buffer[(i*3)+2];
			rle_buffer[(i*3)+2] = (frame->height-1)-y;
		}
	}
	else if(flip_loaded_layers_horizontal)
	{
		for(int i = 0;i<n;i++)
		{
			sx = rle_buffer[(i*3)];
			ex = rle_buffer[(i*3)+1];
			y = rle_buffer[(i*3)+2];
			rle_buffer[(i*3)] = (frame->width)-ex;
			rle_buffer[(i*3)+1] = (frame->width)-sx;
		}
	}

	int i;
	for(i = 0;i<n;i++)
	{
		RLE_STRIP s;
		s.start_x = rle_buffer[(i*3)];
		s.end_x = rle_buffer[(i*3)+1];
		s.y = rle_buffer[(i*3)+2];
		l->strips.push_back(s);
	}
	delete[] rle_buffer;
	n = l->strips.size();
	int default_layer_id = -1;
	if(Num_Layers()!=0)
	{
		default_layer_id = Get_Layer_ID(0);
	}
	for(i = 0;i<n;i++)
	{
		RLE_STRIP *rs = &l->strips[i];
		for(int j = rs->start_x;j<rs->end_x;j++)
		{
			frame->Set_Pixel_Layer(j, rs->y, l->layer_id);
		}
	}

	for(i = 0;i<l->num_neighbor_blends;i++)
	{
		NBLEND b;
		Load_Neighbor_Blend(f, &b);
		l->neighbor_blends.push_back(b);
	}
	active_frame_layers->layers.push_back(l);
	if(next_layer_id<l->layer_id)
	{
		next_layer_id = l->layer_id;
	}
	//get a primitive pointer for this layer
	l->Get_Primitive();
	return true;
}

bool Update_Layers_Relief(bool reproject)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->Set_Relief_Info(&active_frame_layers->layers[i]->relief_info, true, reproject);
	}
	return true;
}

//FIXTHIS holy shit this should all be moved into export options or something

__forceinline float Get_Blended_Distance(int x, int y, float *distance_map)
{
	float d = 0;
	d += distance_map[((y-1)*frame->width)+(x-1)];
	d += distance_map[((y-1)*frame->width)+(x)];
	d += distance_map[((y-1)*frame->width)+(x+1)];
	d += distance_map[((y)*frame->width)+(x-1)];
	d += distance_map[((y)*frame->width)+(x)];
	d += distance_map[((y)*frame->width)+(x+1)];
	d += distance_map[((y+1)*frame->width)+(x-1)];
	d += distance_map[((y+1)*frame->width)+(x)];
	d += distance_map[((y+1)*frame->width)+(x+1)];
	return d/9;
}

bool Get_Frame_Depth_Map(float *res)
{
	int n = frame->width*frame->height;
	float *v;
	for(int i = 0;i<n;i++)
	{
		v = &frame->vertices[i*3];
		res[i] = (float)fastsqrt((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2]));
	}
	return true;
}

bool Get_Frame_Depth_Map(unsigned char *res)
{
	int n = frame->width*frame->height;
	float *fd = new float[n];
	Get_Frame_Depth_Map(fd);
	float min = fd[0];
	float max = min;
	int i;
	for(i = 1;i<n;i++)
	{
		if(min>fd[i])min = fd[i];
		if(max<fd[i])max = fd[i];
	}
	float range = max-min;
	unsigned char c = 0;
	for(i = 0;i<n;i++)
	{
		fd[i] = (1.0f-((fd[i]-min)/range))*255;
		c = (unsigned char)fd[i];
		res[i*3] = c;
		res[(i*3)+1] = c;
		res[(i*3)+2] = c;
	}
	delete[] fd;
	return true;
}

bool Save_Frame_Depth_Map(char *file)
{
	unsigned char *data = new unsigned char[frame->width*frame->height*3];
	Get_Frame_Depth_Map(data);
	Save_Clean_Image(data, frame->width, frame->height, file);
	delete[] data;
	return true;
}

bool Convert_Image_To_Bytes(unsigned char *dst, float *src, int total);

bool Get_Frame_Layer_Map(unsigned char *res)
{
	if(!frame)
	{
		return false;
	}
	float *data = new float[frame->width*frame->height*3];
	memcpy(data, frame->rgb_colors, sizeof(float)*frame->width*frame->height*3);
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->Set_Pixels_Layer_Colors();
	}
	int total = frame->width*frame->height;
	Convert_Image_To_Bytes(res, frame->rgb_colors, total);
	memcpy(frame->rgb_colors, data, sizeof(float)*frame->width*frame->height*3);
	delete[] data;
	return true;
}


bool Project_Layers()
{
	if(!active_frame_layers)return false;
	Print_Status("Projecting all layers..");
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->scene_primitive->primitive->primitive_type==pt_TRIMESH)
		{
			active_frame_layers->layers[i]->Project_To_Model();
		}
		else
		{
			active_frame_layers->layers[i]->ReProject_To_Geometry();
		}
	}
	Print_Status("Done");
	Report_Layers_ReProjected();
	return true;
}

bool Save_Layers(FILE *f)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		Save_Layer(active_frame_layers->layers[i], f);
	}
	return true;
}

bool Load_Layers(FILE *f, int n)
{
	Free_Layers();
	for(int i = 0;i<n;i++)
	{
		Load_Layer(f);
	}
	return true;
}


LAYER* Find_Layer_By_Color(float *rgb)
{
	int n = active_frame_layers->layers.size();
	float *tc;
	for(int i = 0;i<n;i++)
	{
		tc = active_frame_layers->layers[i]->color;
		if(tc[0]==rgb[0]&&tc[1]==rgb[1]&&tc[2]==rgb[2])
		{
			return active_frame_layers->layers[i];
		}
	}
	return 0;
}


//only some data needs to be loaded, but ALL of it has to be read from file for serialization
bool Import_Layer_Edit(FILE *f)
{
	LAYER temp;//temp reciver for loaded data
	LAYER *tl = &temp;
	fread(&tl->name, sizeof(char), 64, f);
	fread(&tl->layer_id, sizeof(int), 1, f);
	fread(&tl->num_neighbor_blends, sizeof(int), 1, f);
	fread(&tl->color, sizeof(float), 3, f);
	fread(&tl->num_tri_indices, sizeof(int), 1, f);
	fread(&tl->num_pixel_indices, sizeof(int), 1, f);
	fread(&tl->scale, sizeof(float), 1, f);
	fread(&tl->keyframed, sizeof(bool), 1, f);
	fread(&tl->freeze, sizeof(bool), 1, f);
	fread(&tl->visible, sizeof(bool), 1, f);
	fread(tl->layer_primitive_pos, sizeof(float), 3, f);
	fread(tl->layer_primitive_dir, sizeof(float), 3, f);
	fread(tl->layer_primitive_rotation, sizeof(float), 3, f);
	fread(tl->layer_plane_offset, sizeof(float), 3, f);
	fread(&tl->layer_plane_origin_type, sizeof(int), 1, f);
	fread(&tl->layer_puff_search_range, sizeof(int), 1, f);
	fread(&tl->layer_puff_scale, sizeof(float), 1, f);
	fread(&tl->layer_puff_range, sizeof(float), 1, f);
	fread(&tl->layer_puff_type, sizeof(int), 1, f);
	fread(&tl->layer_puff_directional, sizeof(bool), 1, f);
	fread(&tl->layer_puff_angle, sizeof(float), 1, f);
	fread(&tl->relief_info, sizeof(RELIEF_INFO), 1, f);
	fread(tl->extra_data, sizeof(float), 24, f);
	fread(&tl->soften_edge_range, sizeof(float), 1, f);
	fread(&tl->soften_edge_strength, sizeof(int), 1, f);
	int n = 0;
	fread(&n, sizeof(int), 1, f);
	int total_rle_size = 3*n;

	int *rle_buffer = new int[total_rle_size];
	fread(rle_buffer, sizeof(int), total_rle_size, f);

	//we don't meed any of the rle strips
	delete[] rle_buffer;

	for(int i = 0;i<tl->num_neighbor_blends;i++)
	{
		NBLEND b;
		Load_Neighbor_Blend(f, &b);
	}

	//find the layer that this edit applies to
	LAYER *l = Find_Layer_By_Color(tl->color);
	if(!l)
	{
		SkinMsgBox(0, "Can't find matching color!", 0, MB_OK);
		//no matching layer with this color!
		return false;
	}

	//set the layer id so it can be matched up with any primitive geometry
	l->layer_id = tl->layer_id;
	//copy the geometry settings
	l->layer_primitive_pos[0] = tl->layer_primitive_pos[0];
	l->layer_primitive_pos[1] = tl->layer_primitive_pos[1];
	l->layer_primitive_pos[2] = tl->layer_primitive_pos[2];

	l->layer_primitive_dir[0] = tl->layer_primitive_dir[0];
	l->layer_primitive_dir[1] = tl->layer_primitive_dir[1];
	l->layer_primitive_dir[2] = tl->layer_primitive_dir[2];
	
	l->layer_primitive_rotation[0] = tl->layer_primitive_rotation[0];
	l->layer_primitive_rotation[1] = tl->layer_primitive_rotation[1];
	l->layer_primitive_rotation[2] = tl->layer_primitive_rotation[2];

	l->layer_plane_offset[0] = tl->layer_plane_offset[0];
	l->layer_plane_offset[1] = tl->layer_plane_offset[1];
	l->layer_plane_offset[2] = tl->layer_plane_offset[2];

	l->scale = tl->scale;
	l->keyframed = tl->keyframed;
	l->freeze = tl->freeze;
	l->visible = tl->visible;

	l->layer_plane_origin_type = tl->layer_plane_origin_type;
	l->layer_puff_search_range = tl->layer_puff_search_range;
	l->layer_puff_scale = tl->layer_puff_scale;
	l->layer_puff_range = tl->layer_puff_range;
	l->layer_puff_type = tl->layer_puff_type;
	l->layer_puff_directional = tl->layer_puff_directional;
	l->layer_puff_angle = tl->layer_puff_angle;
	memcpy(&l->relief_info, &tl->relief_info, sizeof(RELIEF_INFO));
	l->soften_edge_range = tl->soften_edge_range;
	l->soften_edge_strength = tl->soften_edge_strength;

	l->Update_Geometry_Orientation();
	return true;
}

bool Import_Layer_Edits(FILE *f, int n)
{
	if(n!=Num_Layers())
	{
		char msg[512];
		sprintf(msg, "Warning! Importing %i layer edits, current session has %i. Attempt import anyway?", n, Num_Layers());
		if(SkinMsgBox(msg, 0, MB_YESNO)==IDNO)
		{
			return false;
		}
		if(Num_Layers()<n)
		{
			n = Num_Layers();
		}
	}
	for(int i = 0;i<n;i++)
	{
		Import_Layer_Edit(f);
	}
	return true;
}

bool Refresh_All_Layer_Primitives(bool reproject)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->Get_Primitive();
		if(reproject)active_frame_layers->layers[i]->ReProject_To_Geometry();
	}
	return true;
}


//FIXTHIS this could all be in the contour file

bool Load_Contour_Pixels(char *file)
{
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		SkinMsgBox(0, "ERROR opening file for reading!\nCan't load contour data.", file, MB_OK);
		return false;
	}
	unsigned int total_contour_pixels = 0;
	fread(&total_contour_pixels, sizeof(unsigned int), 1, f);
	int *indices = new int[total_contour_pixels];
	float *contours = new float[total_contour_pixels];
	fread(indices, sizeof(int), total_contour_pixels, f);
	fread(contours, sizeof(float), total_contour_pixels, f);
	fclose(f);
	for(int i = 0;i<(int)total_contour_pixels;i++)
	{
		frame->pixels[indices[i]].contour_ratio = contours[i];
	}
	delete[] indices;
	delete[] contours;
	return true;
}

bool Save_Contour_Pixels(char *file)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	int i, j;
	unsigned int total_contour_pixels = 0;
	for(i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->layer_puff_type!=-1)
		{
			total_contour_pixels += active_frame_layers->layers[i]->num_pixel_indices;
		}
	}
	if(total_contour_pixels==0)
	{
		return true;
	}
	int *indices = new int[total_contour_pixels];
	float *contours = new float[total_contour_pixels];
	int cnt = 0;
	for(i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->layer_puff_type!=-1)
		{
			for(j = 0;j<active_frame_layers->layers[i]->num_pixel_indices;j++)
			{
				indices[cnt] = active_frame_layers->layers[i]->pixel_indices[j];
				contours[cnt] = frame->pixels[indices[cnt]].contour_ratio;
				cnt++;
			}
		}
	}
	FILE *f = fopen(file, "wb");
	if(!f)
	{
		SkinMsgBox(0, "ERROR opening file for writing!\nCan't save contour data.", file, MB_OK);
		return false;
	}
	fwrite(&total_contour_pixels, sizeof(unsigned int), 1, f);
	fwrite(indices, sizeof(int), total_contour_pixels, f);
	fwrite(contours, sizeof(float), total_contour_pixels, f);
	fclose(f);
	delete[] indices;
	delete[] contours;
	return true;
}

bool Check_For_Contoured_Layers()
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	int i;
	for(i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->layer_puff_type!=-1)
		{
			active_frame_layers->layers[i]->Puff_Pixels();
		}
	}
	return true;
}

bool RePuff_All()
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->layer_puff_type!=-1)
		{
			active_frame_layers->layers[i]->ReProject_To_Geometry();
		}
	}
	return true;
}

//external control again
void Enable_All_Borders(bool b)
{
	if(!active_frame_layers)return;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->Render_Borders(b);
	}
}

__forceinline void Modify_RLE_Strip_Start(int layer, int x, int y, int v)
{
	LAYER *l = Find_Layer(layer);
	if(!l)
	{
		return;
	}
	int n = l->strips.size();
	for(int i = 0;i<n;i++)
	{
		if(l->strips[i].start_x==x&&l->strips[i].y==y)
		{
			l->modified = true;
			l->strips[i].start_x += v;
			if(l->strips[i].start_x==l->strips[i].end_x)
			{
				l->strips.erase(l->strips.begin()+i);
			}
			return;
		}
	}
}

__forceinline void Modify_RLE_Strip_End(int layer, int x, int y, int v)
{
	LAYER *l = Find_Layer(layer);
	if(!l)
	{
		return;
	}
	int n = l->strips.size();
	for(int i = 0;i<n;i++)
	{
		if(l->strips[i].end_x==x&&l->strips[i].y==y)
		{
			l->modified = true;
			l->strips[i].end_x += v;
			if(l->strips[i].start_x==l->strips[i].end_x)
			{
				l->strips.erase(l->strips.begin()+i);
			}
			return;
		}
	}
}

//list of touching strips created by shifting edges horizontally
vector<RLE_STRIP*> merge_strips;//they can be eliminated after the shift is done

__forceinline void Shift_Left_Out(RLE_STRIP *rs, int layer)
{
	if(rs->start_x<=0||rs->y>=frame->height-1)return;
	int nl = frame->Get_Pixel_Layer(rs->start_x-1, rs->y);
	if(nl==layer)
	{
		return;
	}
	if(frame->Get_Pixel_Layer(rs->start_x-2, rs->y)==layer)
	{
		//ran into another strip within this layer, we can merge them
		merge_strips.push_back(rs);
	}
	frame->Set_Pixel_Layer(rs->start_x-1, rs->y, layer);
	Modify_RLE_Strip_End(nl, rs->start_x, rs->y, -1);
	rs->start_x--;
}


__forceinline int Get_Closest_Vertical_Pixel_Layer(int old_layer, int x, int y)
{
	int bottom_layer = -1;
	int top_layer = -1;
	int bottom_dist = -1;
	int top_dist = -1;
	int i;
	for(i = y;i<frame->height;i++)
	{
		bottom_layer = frame->Get_Pixel_Layer(x, i);
		if(bottom_layer!=-1&&bottom_layer!=old_layer)
		{
			bottom_dist = i-y;
			i = frame->height;
		}
	}
	for(i = y;i>-1;i--)
	{
		top_layer = frame->Get_Pixel_Layer(x, i);
		if(top_layer!=-1&&top_layer!=old_layer)
		{
			top_dist = y-i;
			i = 0;
		}
	}
	if(bottom_dist<top_dist)
	{
		return bottom_layer;
	}
	return top_layer;
}


__forceinline void Shift_Left_In(RLE_STRIP *rs, int layer)
{
	if(rs->start_x>=rs->end_x-2||rs->y>=frame->height-1)return;
	int nl = frame->Get_Pixel_Layer(rs->start_x-1, rs->y);
	if(nl==layer)
	{
		return;
	}
	frame->Set_Pixel_Layer(rs->start_x, rs->y, nl);
	Modify_RLE_Strip_End(nl, rs->start_x, rs->y, 1);
	if(nl==-1)//pulling from the left of the frame
	{
		//find closest higher or lower layer
		//and add strip to that
		nl = Get_Closest_Vertical_Pixel_Layer(layer, 0, rs->y);
		if(nl==-1)
		{
			//no closer one available, just use defaultlayer
			nl = active_frame_layers->layers[0]->layer_id;
		}
		LAYER *l = Find_Layer(nl);
		l->Add_RLE_Strip(rs->y, 0, 1, false);//add a single pixel strip
	}
	rs->start_x++;
}

__forceinline void Shift_Right_In(RLE_STRIP *rs, int layer)
{
	if(rs->end_x-2<=rs->start_x||rs->y>=frame->height-1)return;
	int nl = frame->Get_Pixel_Layer(rs->end_x, rs->y);
	if(nl==layer)
	{
		return;
	}
	frame->Set_Pixel_Layer(rs->end_x-1, rs->y, nl);
	Modify_RLE_Strip_Start(nl, rs->end_x, rs->y, -1);
	if(nl==-1)
	{
		//find closest higher or lower layer
		//and add strip to that
		nl = Get_Closest_Vertical_Pixel_Layer(layer, frame->width-1, rs->y);
		if(nl==-1)
		{
			nl = active_frame_layers->layers[0]->layer_id;
		}
		LAYER *l = Find_Layer(nl);
		l->Add_RLE_Strip(rs->y, frame->width-1, frame->width, false);
	}
	rs->end_x--;
}

__forceinline void Shift_Right_Out(RLE_STRIP *rs, int layer)
{
	if(rs->end_x>=frame->width||rs->y>=frame->height-1)return;
	int nl = frame->Get_Pixel_Layer(rs->end_x, rs->y);
	if(nl==layer)
	{
		return;
	}
	if(frame->Get_Pixel_Layer(rs->end_x+1, rs->y)==layer)
	{
		//ran into another strip within this layer, we can merge them
		merge_strips.push_back(rs);
	}
	frame->Set_Pixel_Layer(rs->end_x, rs->y, layer);
	Modify_RLE_Strip_Start(nl, rs->end_x, rs->y, +1);
	rs->end_x++;
}

__forceinline bool Merge_Partner_Already_Flagged(int index, int nmp, vector<int> *merge_partners)
{
	for(int i = 0;i<nmp;i++)
	{
		if((*merge_partners)[i]==index)
		{
			return true;
		}
	}
	return false;
}

__forceinline int Find_Merge_Partner(LAYER *l, int n, RLE_STRIP *rs, vector<int> *merge_partners)
{
	int nmp = merge_partners->size();
	for(int i = 0;i<n;i++)
	{
		if(&l->strips[i]!=rs)
		{
			if(rs->y==l->strips[i].y)
			{
				if(!Merge_Partner_Already_Flagged(i, nmp, merge_partners))
				{
					if(rs->end_x==l->strips[i].start_x)
					{
						rs->end_x = l->strips[i].end_x;
						return i;
					}
					if(rs->start_x==l->strips[i].end_x)
					{
						rs->start_x = l->strips[i].start_x;
						return i;
					}
				}
			}
		}
	}
	return -1;
}

__forceinline void Merge_Merge_Strips(LAYER *l, int nms)
{
	vector<int> merge_partners;
	int ns = l->strips.size();

	int mp;
	bool done = false;
	int i;
	for(i = 0;i<nms;i++)
	{
		done = false;//some may be chained together
		while(!done)//keep going until they are all processed
		{
			mp = Find_Merge_Partner(l, ns, merge_strips[i], &merge_partners);
			if(mp!=-1)
			{
				merge_partners.push_back(mp);
			}
			else
			{
				done = true;
			}
		}
	}

	//sort the merge partner ids
	sort(merge_partners.begin(), merge_partners.end());
	//remove them in reverse order
	int nmp = merge_partners.size();
	for(i = nmp-1;i>-1;i--)
	{
		RLE_STRIP *rs = &l->strips[merge_partners[i]];
		l->strips.erase(l->strips.begin()+merge_partners[i]);
	}
	merge_strips.clear();
}

__forceinline void Shift_Left_Out(LAYER *l)
{
	l->modified = true;
	int n = l->strips.size();
	for(int i = 0;i<n;i++)
	{
		Shift_Left_Out(&l->strips[i], l->layer_id);
	}
	int nms = merge_strips.size();
	if(nms>0)
	{
		Merge_Merge_Strips(l, nms);
	}
}

__forceinline void Shift_Left_In(LAYER *l)
{
	l->modified = true;
	int n = l->strips.size();
	for(int i = 0;i<n;i++)
	{
		Shift_Left_In(&l->strips[i], l->layer_id);
	}
}

__forceinline void Shift_Right_In(LAYER *l)
{
	l->modified = true;
	int n = l->strips.size();
	for(int i = 0;i<n;i++)
	{
		Shift_Right_In(&l->strips[i], l->layer_id);
	}
}

__forceinline void Shift_Right_Out(LAYER *l)
{
	l->modified = true;
	int n = l->strips.size();
	for(int i = 0;i<n;i++)
	{
		Shift_Right_Out(&l->strips[i], l->layer_id);
	}
	int nms = merge_strips.size();
	if(nms>0)
	{
		Merge_Merge_Strips(l, nms);
	}
}

__forceinline RLE_STRIP* Find_Matching_Strip_End(LAYER *l, int x, int y)
{
	int n = l->strips.size();
	x++;
	for(int i = 0;i<n;i++)
	{
		if(l->strips[i].y==y)
		{
			if(l->strips[i].end_x==x)
			{
				return &l->strips[i];
			}
		}
	}
	return 0;
}

void Shift_Border_Left(int layer_id, int x, int y)
{
	if(!active_frame_layers)return;
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->layer_id==layer_id)
		{
			RLE_STRIP *strip = Find_Matching_Strip_End(active_frame_layers->layers[i], x, y);
			if(!strip)
			{
				SkinMsgBox(0, "INTERNAL ERROR! Can't shift border, no matching RLE strip, the frame layer data has been corrupted.", "This should never happen", MB_OK);
			}
			else
			{
				active_frame_layers->layers[i]->modified = true;
				Shift_Right_In(strip, layer_id);
			}
			return;
		}
	}
	SkinMsgBox(0, "INTERNAL ERROR! Can't shift border, no matching layer, the frame layer data has been corrupted.", "This should never happen", MB_OK);
}

void Shift_Border_Right(int layer_id, int x, int y)
{
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		if(active_frame_layers->layers[i]->layer_id==layer_id)
		{
			RLE_STRIP *strip = Find_Matching_Strip_End(active_frame_layers->layers[i], x, y);
			if(!strip)
			{
				SkinMsgBox(0, "INTERNAL ERROR! Can't shift border, no matching RLE strip, the frame layer data has been corrupted.", "This should never happen", MB_OK);
			}
			else
			{
				active_frame_layers->layers[i]->modified = true;
				Shift_Right_Out(strip, layer_id);
			}
			return;
		}
	}
	SkinMsgBox(0, "INTERNAL ERROR! Can't shift border, no matching layer, the frame layer data has been corrupted.", "This should never happen", MB_OK);
}

void Shift_Left_Out()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->modified = true;
		Shift_Left_Out(selected_layers[i]);
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
}

void Shift_Left_In()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->modified = true;
		Shift_Left_In(selected_layers[i]);
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
}

void Shift_Right_In()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->modified = true;
		Shift_Right_In(selected_layers[i]);
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
}

void Shift_Right_Out()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->modified = true;
		Shift_Right_Out(selected_layers[i]);
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
}

//used for resampling strips,temp fix for SFI_VERSION 2
void Delete_All_RLE_Strips()
{
	int n = active_frame_layers->layers.size();
	for(int i = 0;i<n;i++)
	{
		active_frame_layers->layers[i]->strips.clear();
	}
}


bool Move_Selected_Geometry_Horizontally(float v)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->layer_primitive_pos[0]+=v;
		selected_layers[i]->Update_Geometry_Orientation();
		selected_layers[i]->ReProject_To_Geometry();
	}
	return true;
}

bool Move_Selected_Geometry_Vertically(float v)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->layer_primitive_pos[1]+=v;
		selected_layers[i]->Update_Geometry_Orientation();
		selected_layers[i]->ReProject_To_Geometry();
	}
	return true;
}

bool Scale_Selected_Geometry(float x, float y, float z)
{
	int n =  selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->scene_primitive->Adjust_Scale(x, y, z);
		selected_layers[i]->Update_Geometry_Orientation();
		selected_layers[i]->ReProject_To_Geometry();
	}
	return true;
}

extern float horizontal_rotation;
extern float vertical_rotation;
extern float z_rotation;

extern int plane_origin_type;

void Get_Plane_Dir(float *orot, float *dir)
{
	BB_MATRIX m;
	float pos[3] = {0,0,0};
	float rot[3];
	rot[0] = orot[1];
	rot[1] = orot[0];
	rot[2] = 0;
	m.Set(m.m, pos, rot);
	dir[0] = m.m[2][0];
	dir[1] = m.m[2][1];
	dir[2] = m.m[2][2];
}




bool Move_Layer_Geometry_Horizontally(int layer_index, float v, bool reproject)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(layer_index<0||layer_index>=n)return false;
	LAYER *l = active_frame_layers->layers[layer_index];
	l->layer_primitive_pos[0]+=v;
	l->Update_Geometry_Orientation();
	if(reproject)l->ReProject_To_Geometry();
	return true;
}

bool Move_Layer_Geometry_Vertically(int layer_index, float v, bool reproject)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(layer_index<0||layer_index>=n)return false;
	LAYER *l = active_frame_layers->layers[layer_index];
	l->layer_primitive_pos[1]+=v;
	l->Update_Geometry_Orientation();
	if(reproject)l->ReProject_To_Geometry();
	return true;
}

bool Move_Layer_Geometry_DepthWise(int layer_index, float v, bool reproject)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(layer_index<0||layer_index>=n)return false;
	LAYER *l = active_frame_layers->layers[layer_index];
	l->layer_primitive_pos[2]+=v;
	l->Update_Geometry_Orientation();
	if(reproject)l->ReProject_To_Geometry();
	return true;
}

bool Scale_Layer_Geometry(int layer_index, float x, float y, float z, bool reproject)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(layer_index<0||layer_index>=n)return false;
	LAYER *l = active_frame_layers->layers[layer_index];
	l->scene_primitive->Adjust_Scale(x, y, z);
	l->Update_Geometry_Orientation();
	if(reproject)l->ReProject_To_Geometry();
	return true;
}

bool Rotate_Layer_Geometry(int layer_index, float x, float y, float z, bool reproject)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(layer_index<0||layer_index>=n)return false;
	LAYER *l = active_frame_layers->layers[layer_index];
	l->layer_primitive_rotation[0] += x;
	l->layer_primitive_rotation[1] += y;
	l->layer_primitive_rotation[2] += z;
	if(l->layer_primitive_rotation[0]>=180)l->layer_primitive_rotation[0]-=360;
	if(l->layer_primitive_rotation[0]<=-180)l->layer_primitive_rotation[0]+=360;
	if(reproject)
	{
		l->ReProject_To_Geometry();
		Get_Plane_Dir(l->layer_primitive_rotation, l->layer_primitive_dir);
		if(l->selected)
		{
			horizontal_rotation = l->layer_primitive_rotation[0];
			vertical_rotation = l->layer_primitive_rotation[1];
			z_rotation = l->layer_primitive_rotation[2];
			Update_Selection_Status_Orientation(l->layer_primitive_rotation[0], l->layer_primitive_rotation[1]);
			Update_Planar_GUI();
		}
	}
	return true;
}

bool Project_Pixel_To_Layer(int layer_index, int px, int py, float *res)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(layer_index<0||layer_index>=n)return false;
	LAYER *l = active_frame_layers->layers[layer_index];
	if(l->scene_primitive->primitive->primitive_type==pt_TRIMESH)
	{
		l->Project_Vertex_To_Model_Geometry(frame->view_origin, frame->Get_Frame_Vector(px, py), res);
	}
	else
	{
		l->Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(px, py), res);
	}
	return true;
}

bool Pixel_Hits_Layer_Geometry(int layer_index, int px, int py, float *res)
{
	if(!active_frame_layers)return false;
	int n = active_frame_layers->layers.size();
	if(layer_index<0||layer_index>=n)return false;
	LAYER *l = active_frame_layers->layers[layer_index];
	if(l->scene_primitive->primitive->primitive_type==pt_TRIMESH)
	{
		return l->Project_Vertex_To_Model_Geometry(frame->view_origin, frame->Get_Frame_Vector(px, py), res);
	}
	else
	{
		return l->Project_Vertex_To_Layer_Geometry(frame->view_origin, frame->Get_Frame_Vector(px, py), res);
	}
	return false;
}


//used for link point aut-alignment
bool Set_Layer_Transform(int layer_id, float *pos, float *rot)
{
	LAYER *l = Find_Layer(layer_id);
	if(!l)
	{
		return false;
	}
	l->layer_primitive_pos[0] = pos[0];
	l->layer_primitive_pos[1] = pos[1];
	l->layer_primitive_pos[2] = pos[2];
	l->layer_primitive_rotation[0] = rot[0];
	l->layer_primitive_rotation[1] = rot[1];
	l->layer_primitive_rotation[2] = rot[2];
	Get_Plane_Dir(l->layer_primitive_rotation, l->layer_primitive_dir);
	if(l->selected)
	{
		Update_Selection_Status_Orientation(l->layer_primitive_rotation[0], l->layer_primitive_rotation[1]);
		Update_Selection_Status_Depth(l->layer_primitive_pos[2]);
		horizontal_rotation = l->layer_primitive_rotation[0];
		vertical_rotation = l->layer_primitive_rotation[1];
		z_rotation = l->layer_primitive_rotation[2];
		Update_Planar_GUI();
	}
	return true;
}


void Rotate_Horizontal_Selection(float v)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->layer_primitive_rotation[0] += v;
		if(selected_layers[i]->layer_primitive_rotation[0]>=180)selected_layers[i]->layer_primitive_rotation[0]-=360;
		if(selected_layers[i]->layer_primitive_rotation[0]<=-180)selected_layers[i]->layer_primitive_rotation[0]+=360;
		horizontal_rotation = selected_layers[i]->layer_primitive_rotation[0];
		Get_Plane_Dir(selected_layers[i]->layer_primitive_rotation, selected_layers[i]->layer_primitive_dir);
		selected_layers[i]->ReProject_To_Geometry();
		Update_Selection_Status_Orientation(selected_layers[i]->layer_primitive_rotation[0], selected_layers[i]->layer_primitive_rotation[1]);
	}
	Update_Planar_GUI();
}

void Rotate_Vertical_Selection(float v)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->layer_primitive_rotation[1] += v;
		if(selected_layers[i]->layer_primitive_rotation[1]>=180)selected_layers[i]->layer_primitive_rotation[1]-=360;
		if(selected_layers[i]->layer_primitive_rotation[1]<=-180)selected_layers[i]->layer_primitive_rotation[1]+=360;
		vertical_rotation = selected_layers[i]->layer_primitive_rotation[1];
		Get_Plane_Dir(selected_layers[i]->layer_primitive_rotation, selected_layers[i]->layer_primitive_dir);
		selected_layers[i]->ReProject_To_Geometry();
		Update_Selection_Status_Orientation(selected_layers[i]->layer_primitive_rotation[0], selected_layers[i]->layer_primitive_rotation[1]);
	}
	Update_Planar_GUI();
}

void Rotate_Z_Selection(float v)
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->layer_primitive_rotation[2] += v;
		if(selected_layers[i]->layer_primitive_rotation[2]>=180)selected_layers[i]->layer_primitive_rotation[2]-=360;
		if(selected_layers[i]->layer_primitive_rotation[2]<=-180)selected_layers[i]->layer_primitive_rotation[2]+=360;
		z_rotation = selected_layers[i]->layer_primitive_rotation[2];
		Get_Plane_Dir(selected_layers[i]->layer_primitive_rotation, selected_layers[i]->layer_primitive_dir);
		selected_layers[i]->ReProject_To_Geometry();
		Update_Selection_Status_Orientation(selected_layers[i]->layer_primitive_rotation[0], selected_layers[i]->layer_primitive_rotation[1]);
	}
	Update_Planar_GUI();
}


void Reset_Rotate_Selection()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->layer_primitive_rotation[0] = 0;
		selected_layers[i]->layer_primitive_rotation[1] = 0;
		selected_layers[i]->layer_primitive_rotation[2] = 0;
		horizontal_rotation = 0;
		vertical_rotation = 0;
		z_rotation = 0;
		Get_Plane_Dir(selected_layers[i]->layer_primitive_rotation, selected_layers[i]->layer_primitive_dir);
		selected_layers[i]->ReProject_To_Geometry();
		Update_Selection_Status_Orientation(selected_layers[i]->layer_primitive_rotation[0], selected_layers[i]->layer_primitive_rotation[1]);
	}
	Update_Planar_GUI();
}

__forceinline bool Get_Adjacent_Vertical_Strips(int dir, int layer_id, RLE_STRIP *rs, vector<RLE_STRIP> *tstrips, vector<int> *strip_layers)
{
	bool instrip = false;
	int lid;
	int strip_layer = -1;
	RLE_STRIP temp;
	temp.y = rs->y+dir;
	if(temp.y>=frame->height||temp.y<0)return false;
	for(int i = rs->start_x;i<rs->end_x;i++)
	{
		lid = frame->Get_Pixel_Layer(i, temp.y);
		if(!instrip)
		{
			if(lid!=layer_id)
			{
				instrip = true;
				temp.start_x = i;
				strip_layer = lid;
			}
		}
		else
		{
			if(lid==layer_id)
			{
				instrip = false;
				temp.end_x = i;
				tstrips->push_back(temp);
				if(strip_layers)strip_layers->push_back(strip_layer);
			}
			else if(lid!=strip_layer&&strip_layers)
			{
				temp.end_x = i;
				tstrips->push_back(temp);
				strip_layers->push_back(strip_layer);
				temp.start_x = i;
				strip_layer = lid;
			}
		}
	}
	if(instrip)
	{
		temp.end_x = rs->end_x;
		tstrips->push_back(temp);
		if(strip_layers)strip_layers->push_back(strip_layer);
	}
	return true;
}

void Shift_Top_Down(LAYER *l, int index)
{
	vector<RLE_STRIP> tstrips;
	vector<int> strip_layers;
	int n = l->strips.size();
	int i;
	for(i = 0;i<n;i++)
	{
		Get_Adjacent_Vertical_Strips(1, l->layer_id, &l->strips[i], &tstrips, &strip_layers);
	}
	n = tstrips.size();
	for(i = 0;i<n;i++)
	{
		index = Get_Layer_Index(strip_layers[i]);
		if(index==-1)
		{
			//this never happpens
		}
		else
		{
			Force_Add_Layer_RLE_Strip(active_frame_layers->frame_id, index, tstrips[i].y-1, tstrips[i].start_x, tstrips[i].end_x, false);
		}
	}
	tstrips.clear();
}

void Shift_Top_Up(LAYER *l, int index)
{
	vector<RLE_STRIP> tstrips;
	int n = l->strips.size();
	int i;
	for(i = 0;i<n;i++)
	{
		Get_Adjacent_Vertical_Strips(1, l->layer_id, &l->strips[i], &tstrips, 0);
	}
	n = tstrips.size();
	for(i = 0;i<n;i++)
	{
		Force_Add_Layer_RLE_Strip(active_frame_layers->frame_id, index, tstrips[i].y, tstrips[i].start_x, tstrips[i].end_x, false);
	}
	tstrips.clear();
}

void Shift_Bottom_Down(LAYER *l, int index)
{
	vector<RLE_STRIP> tstrips;
	int n = l->strips.size();
	int i;
	for(i = 0;i<n;i++)
	{
		Get_Adjacent_Vertical_Strips(-1, l->layer_id, &l->strips[i], &tstrips, 0);
	}
	n = tstrips.size();
	for(i = 0;i<n;i++)
	{
		Force_Add_Layer_RLE_Strip(active_frame_layers->frame_id, index, tstrips[i].y, tstrips[i].start_x, tstrips[i].end_x, false);
	}
	tstrips.clear();
}

void Shift_Bottom_Up(LAYER *l, int index)
{
	vector<RLE_STRIP> tstrips;
	vector<int> strip_layers;
	int n = l->strips.size();
	int i;
	for(i = 0;i<n;i++)
	{
		Get_Adjacent_Vertical_Strips(-1, l->layer_id, &l->strips[i], &tstrips, &strip_layers);
	}
	n = tstrips.size();
	for(i = 0;i<n;i++)
	{
		index = Get_Layer_Index(strip_layers[i]);
		if(index==-1)
		{
			//this never happpens
		}
		else
		{
			Force_Add_Layer_RLE_Strip(active_frame_layers->frame_id, index, tstrips[i].y+1, tstrips[i].start_x, tstrips[i].end_x, false);
		}
	}
	tstrips.clear();
}

void Shift_Top_Down()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->modified = true;
		Shift_Top_Down(selected_layers[i], i);
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
}

void Shift_Top_Up()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->modified = true;
		Shift_Top_Up(selected_layers[i], i);
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
}

void Shift_Bottom_Down()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->modified = true;
		Shift_Bottom_Down(selected_layers[i], i);
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
}

void Shift_Bottom_Up()
{
	int n = selected_layers.size();
	for(int i = 0;i<n;i++)
	{
		selected_layers[i]->modified = true;
		Shift_Bottom_Up(selected_layers[i], i);
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
}

