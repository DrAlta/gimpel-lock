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
#include "Layers.h"
#include "ProjectFile.h"
#include "KLTInterface.h"
#include <vector>
#include <GL/gl.h>
#include <math.h>
#include "../Skin.h"

using namespace std;
void FillLine(float x1, float x2, float y1, float y2, bool *grid, int w, int h);
void Fill_Lasso_Selection_Grid(bool *grid, int w, int h);
bool Get_Grid_Mask_RLE_Strips(bool *grid, int w, int h, vector<RLE_STRIP> *strips);
float Pixel_Size();
bool Print_To_Console(const char *fmt, ...);
void Cross(float *v1, float *v2, float *v3);
void Normalize(float *v);
void SetLength(float *v, float s);

bool Close_Tracking_Outline_Dlg();

extern float last_edit_px;
extern float last_edit_py;
extern int brush_size;

__forceinline float Get_Brush_Strength_At_Pixel(float x, float y)
{
	float dx = x-last_edit_px;
	float dy = y-last_edit_py;
	float d = sqrt((dx*dx)+(dy*dy));
	if(d>brush_size)
	{
		return 0;
	}
	return 1.0f-(d/brush_size);
}

//single outline contained in a layer
class OUTLINE
{
public:
	OUTLINE()
	{
	}
	~OUTLINE()
	{
		original_pixels.clear();
		tracked_pixels.clear();
	}
	bool Point_Exists(int x, int y)
	{
		int n = original_pixels.size()/2;
		for(int i = 0;i<n;i++)
		{
			if(original_pixels[i*2]==x&&original_pixels[(i*2)+1]==y)
			{
				return true;
			}
		}
		return false;
	}
	bool Get_Extents()
	{
		int n = tracked_pixels.size()/2;
		tlx = thx = tly = thy = -1;
		for(int i = 0;i<n;i++)
		{
			float x = tracked_pixels[i*2];
			float y = tracked_pixels[(i*2)+1];
			if(x>=0)
			{
				if(tlx==-1)
				{
					tlx = x;
					thx = x;
					tly = y;
					thy = y;
				}
				else
				{
					if(tlx>x)tlx=x;
					if(thx<x)thx=x;
					if(tly>y)tly=y;
					if(thy<y)thy=y;
				}
			}
		}
		float border = 10;
		tlx -= border;
		thx += border;
		tly -= border;
		thy += border;
		return true;
	}
	float olx, oly, ohx, ohy;
	float tlx, tly, thx, thy;
	vector<float> original_pixels;
	vector<float> tracked_pixels;
};

//collection of outlines for a single layer, may contain multiple or holes
class LAYER_OUTLINES
{
public:
	LAYER_OUTLINES()
	{
	}
	~LAYER_OUTLINES()
	{
		int n = outlines.size();
		for(int i = 0;i<n;i++)
		{
			delete outlines[i];
		}
		outlines.clear();
		strips.clear();
		new_strips.clear();
	}
	bool Start_New_Outline()
	{
		OUTLINE *o = new OUTLINE;
		outlines.push_back(o);
		return true;
	}
	bool Add_To_Outline(int x, int y)
	{
		OUTLINE *o = outlines[outlines.size()-1];
		o->original_pixels.push_back(x);
		o->original_pixels.push_back(y);
		o->tracked_pixels.push_back(x);
		o->tracked_pixels.push_back(y);
		return true;
	}
	int layer_index;
	int layer_id;
	int original_frame;
	vector<bool> start_point_used;
	vector<bool> end_point_used;
	vector<RLE_STRIP*> strips;
	vector<RLE_STRIP> new_strips;
	vector<OUTLINE*> outlines;
};

//list of layers with outlines
vector<LAYER_OUTLINES*> layer_outlines;

//free data
bool Clear_Layer_Outlines()
{
	int n = layer_outlines.size();
	for(int i = 0;i<n;i++)
	{
		delete layer_outlines[i];
	}
	layer_outlines.clear();
	return true;
}

//check if strip start or end is in the middle of the layer
//should not happen but would kill outlining search algorithm
bool Start_Point_Buried(int x, int y, int layer_id)
{
	if(frame->Get_Pixel_Layer(x-1, y)!=layer_id)return false;
	return true;
}

bool End_Point_Buried(int x, int y, int layer_id)
{
	if(frame->Get_Pixel_Layer(x, y)!=layer_id)return false;
	return true;
}

//get strips for outlines
bool Get_Layer_Strips(int layer_index)
{
	int ns = Num_RLE_Strips_In_Layer(layer_index);
	if(ns==0)return true;
	LAYER_OUTLINES *lo = new LAYER_OUTLINES;
	lo->original_frame = Get_Current_Project_Frame();
	layer_outlines.push_back(lo);
	lo->layer_index = layer_index;
	lo->layer_id = Get_Layer_ID(layer_index);
	for(int i = 0;i<ns;i++)
	{
		RLE_STRIP *rs = Get_RLE_Strip(layer_index, i);
		bool start_used = Start_Point_Buried(rs->start_x, rs->y, lo->layer_id);
		bool end_used = End_Point_Buried(rs->end_x, rs->y, lo->layer_id);
		if(!start_used||!end_used)//if both ends are not buried
		{
			//save them for the outline search
			lo->start_point_used.push_back(start_used);
			lo->end_point_used.push_back(end_used);
			lo->strips.push_back(rs);
		}
	}
	return true;
}

//get all strips for selected layers
bool Find_All_Layer_Strips()
{
	int nl = Num_Layers();
	for(int i = 0;i<nl;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Get_Layer_Strips(i);
		}
	}
	return true;
}

//get a random unused start point for the outline from the first unused endpoint
bool Get_Start_Point(LAYER_OUTLINES *lo, int *x, int *y)
{
	int n = lo->strips.size();
	for(int i = 0;i<n;i++)
	{
		if(!lo->start_point_used[i])
		{
			*x = lo->strips[i]->start_x;
			*y = lo->strips[i]->y;
			return true;
		}
	}
	return false;
}

__forceinline bool Get_Next_Clockwise_Pixel(int x, int y, int *rx, int *ry, int layer_id, int *search_order)
{
	//find matching or non-matching pixels in a 3x3 grid
	bool top    = frame->Get_Pixel_Layer(x, y-1)==layer_id;
	bool bottom = frame->Get_Pixel_Layer(x, y+1)==layer_id;
	bool left   = frame->Get_Pixel_Layer(x-1, y)==layer_id;
	bool right  = frame->Get_Pixel_Layer(x+1, y)==layer_id;
	bool ul     = frame->Get_Pixel_Layer(x-1, y-1)==layer_id;
	bool ur     = frame->Get_Pixel_Layer(x+1, y-1)==layer_id;
	bool ll     = frame->Get_Pixel_Layer(x-1, y+1)==layer_id;
	bool lr     = frame->Get_Pixel_Layer(x+1, y+1)==layer_id;
	//sort them in clockwise order, triple it for searches
	bool order[] = {top, ur, right, lr, bottom, ll, left, ul, top, ur, right, lr, bottom, ll, left, ul, top, ur, right, lr, bottom, ll, left, ul};
	//for all 8 positions, double that because start position might occur anywhere in the first 8
	for(int i = *search_order;i<16;i++)
	//for(int i = 0;i<8;i++)
	{
		//find the first non-matching pixel so we know the edge we are on
		if(!order[i])
		{
			//and the first matching pixel clockwise from that
			for(int j = 1;j<8;j++)
			{
				if(order[i+j])
				{
					int k = i+j;
					if(k>=16)k-=16;//get it back to 0-7
					if(k>=8)k-=8;
					//we have a match
					switch(k)
					{
						case 0:*rx = x;  *ry = y-1;break;//top
						case 1:*rx = x+1;*ry = y-1;break;//ur
						case 2:*rx = x+1;*ry = y;  break;//right
						case 3:*rx = x+1;*ry = y+1;break;//lr
						case 4:*rx = x;  *ry = y+1;break;//bottom
						case 5:*rx = x-1;*ry = y+1;break;//ll
						case 6:*rx = x-1;*ry = y;  break;//left
						case 7:*rx = x-1;*ry = y-1;break;//ur
						default:break;
					};
					//set the next search start opposite to the direction we just moved
					*search_order = k-4;//this will be the "previous" pixel for the next search
					if(*search_order<0)//to ensure the correct side is checked first for single-pixel areas
					{
						*search_order += 8;
					}
					return true;
				}
			}
		}
	}
	return false;
}

//to avoid error with max loop for outline search
int Get_Max_Possible_Outline_Pixels(LAYER_OUTLINES *lo)
{
	int max = 0;
	int n = lo->strips.size();
	for(int i = 0;i<n;i++)
	{
		max += lo->strips[i]->end_x-lo->strips[i]->start_x;
	}
	//single horizontal strip will have upper and lower side
	return max*2;
}

//generate outlines from a collection of strips
bool Create_Outlines(LAYER_OUTLINES *lo)
{
	int start_x = 0;
	int start_y = 0;
	if(!Get_Start_Point(lo, &start_x, &start_y))
	{
		return false;
	}
	lo->Start_New_Outline();
	lo->Add_To_Outline(start_x, start_y);
	bool done = false;
	int last_x = start_x;
	int last_y = start_y;
	int next_x = start_x;
	int next_y = start_y;
	int max_outline_pixels = Get_Max_Possible_Outline_Pixels(lo);
	int cnt = 0;
	int search_order = 0;
	while(!done)
	{
		if(Get_Next_Clockwise_Pixel(last_x, last_y, &next_x, &next_y, lo->layer_id, &search_order))
		{
			//check for near pixel miss
			if(next_x==start_x&&next_y==start_y)
			{
				done = true;
			}
			else
			{
				if(!lo->Add_To_Outline(next_x, next_y))
				{
					//done = true;
				}
				//else
				{
					last_x = next_x;
					last_y = next_y;
					cnt++;
					if(cnt>max_outline_pixels)
					{
						//houston we have a problem
						SkinMsgBox(0, "Outline wraps around, tracking may be slower", "M.E.H", MB_OK);
						done = true;
						//OOPS it does happen if the wraparound happens to skip over the first pixel
						//due to angle creating clustered block of hits along a corner
						//then it repeats, no easy way to check
						//UPDATE could check pixel 2 or 3 but that's a hack :)
						//easier to let it spin out and wrap around
					}
				}
			}
		}
		else
		{
			done = true;
		}
	}
	return true;
}

//generate outlines for all selected layers
bool Create_Outlines()
{
	int n = layer_outlines.size();
	for(int i = 0;i<n;i++)
	{
		Create_Outlines(layer_outlines[i]);
	}
	return true;
}

bool Update_Outline_Extents()
{
	int n = layer_outlines.size();
	for(int i = 0;i<n;i++)
	{
		int np = layer_outlines[i]->outlines.size();
		for(int j = 0;j<np;j++)
		{
			layer_outlines[i]->outlines[j]->Get_Extents();
		}
	}
	return true;
}

//check for special case "drifting" corner when left and bottom pixels already filled
__forceinline bool Is_Creeping_Corner(bool *grid, bool *fgrid, int x, int y, int xoff, int yoff, int w, int h0, int layer_id)
{
	bool left = grid[((y)*w)+(x+1)];
	bool bottom = grid[((y+1)*w)+(x)];

	if(left&&bottom)//this is IT but only for one side
	{
		return fgrid[((y-1)*w)+(x-1)];
	}
	return false;
}

//flood fill grid mask to leave ONLY valid blank areas to extract rle strips
bool Floodfill_Grid(bool *grid, int w, int h, int xoff, int yoff, int layer_id)
{
	int n = w*h;
	bool *tgrid = new bool[n];
	bool *tgrid2 = new bool[n];
	memcpy(tgrid, grid, sizeof(bool)*n);
	//floodfill using original selection algorithm
	Fill_Lasso_Selection_Grid(tgrid, w, h);
	memcpy(tgrid2, tgrid, sizeof(bool)*n);
	int i, j, k;
	//clear out original outline pixels
	for(i = 1;i<w-1;i++)
	{
		for(j = 1;j<h-1;j++)
		{
			k = (j*w)+i;
			if(grid[k])
			{
				//except for creeping corner case
				if(!Is_Creeping_Corner(grid, tgrid2, i, j, xoff, yoff, w, h, layer_id))
				{
					tgrid[k] = false;
				}
			}
		}
	}
	memcpy(grid, tgrid, sizeof(bool)*n);
	delete[] tgrid;
	delete[] tgrid2;
	return true;
}

bool Get_Outline_RLE_Strips(LAYER_OUTLINES *lo)
{
	//first get the lo/hi extents for the outlines
	int no = lo->outlines.size();
	if(no==0){return false;}
	int i, j;
	int lx = lo->outlines[0]->tracked_pixels[0];
	int ly = lo->outlines[0]->tracked_pixels[1];
	int hx = lx;
	int hy = ly;
	for(i = 0;i<no;i++)
	{
		int np = lo->outlines[i]->tracked_pixels.size()/2;
		for(j = 0;j<np;j++)
		{
			int tx = lo->outlines[i]->tracked_pixels[j*2];
			int ty = lo->outlines[i]->tracked_pixels[(j*2)+1];
			if(lx>tx)lx=tx;
			if(ly>ty)ly=ty;
			if(hx<tx)hx=tx;
			if(hy<ty)hy=ty;
		}
	}
	int w = (hx-lx)+4;//get dimensions
	int h = (hy-ly)+4;
	//make a grid
	bool *grid = new bool[w*h];
	memset(grid, 0, sizeof(bool)*w*h);
	//register all outline segments
	float x1, y1, x2, y2;
	for(i = 0;i<no;i++)
	{
		int np = lo->outlines[i]->tracked_pixels.size()/2;
		for(j = 0;j<np-1;j++)
		{
			x1 = lo->outlines[i]->tracked_pixels[j*2];//this pixel
			y1 = lo->outlines[i]->tracked_pixels[(j*2)+1];
			x2 = lo->outlines[i]->tracked_pixels[(j+1)*2];//the next pixel
			y2 = lo->outlines[i]->tracked_pixels[((j+1)*2)+1];
			FillLine(x1-(lx-2), x2-(lx-2), y1-(ly-2), y2-(ly-2), grid, w, h);
		}
		j = np-1;
		x1 = lo->outlines[i]->tracked_pixels[j*2];//the last pixel
		y1 = lo->outlines[i]->tracked_pixels[(j*2)+1];
		x2 = lo->outlines[i]->tracked_pixels[0];//the first pixel
		y2 = lo->outlines[i]->tracked_pixels[1];
		FillLine(x1-(lx-2), x2-(lx-2), y1-(ly-2), y2-(ly-2), grid, w, h);
	}
	Floodfill_Grid(grid, w, h, lx-2, ly-2, lo->layer_id);
	Get_Grid_Mask_RLE_Strips(grid, w, h, &lo->new_strips);
	delete[] grid;
	//now correct the strips back to screen space
	int ons = lo->strips.size();
	int ns = lo->new_strips.size();
	for(i = 0;i<ns;i++)
	{
		lo->new_strips[i].start_x += lx-2;
		lo->new_strips[i].end_x += lx-2;
		lo->new_strips[i].y += ly-2;
	}
	return true;
}


bool Remove_Stray_Pixels(OUTLINE *o)
{
	int n = o->tracked_pixels.size()/2;
	for(int i = 0;i<n;i++)
	{
		if(o->tracked_pixels[i*2]==-1)
		{
			o->tracked_pixels.erase(o->tracked_pixels.begin()+(i*2));
			o->tracked_pixels.erase(o->tracked_pixels.begin()+(i*2));
			i--;
			n--;
		}
	}
	return true;
}


bool Smooth_Tracking_Outlines(OUTLINE *o, bool use_brush)
{
	Remove_Stray_Pixels(o);
	int n = o->tracked_pixels.size()/2;
	float ax, ay;
	float bx, by;
	float cx, cy;
	float *temp = new float[n*2];
	int i;
	for(i = 1;i<n-1;i++)
	{
		ax = o->tracked_pixels[(i-1)*2];
		ay = o->tracked_pixels[((i-1)*2)+1];
		bx = o->tracked_pixels[(i)*2];
		by = o->tracked_pixels[((i)*2)+1];
		cx = o->tracked_pixels[(i+1)*2];
		cy = o->tracked_pixels[((i+1)*2)+1];
		temp[(i*2)] = (ax+bx+cx)/3;
		temp[(i*2)+1] = (ay+by+cy)/3;
	}
	for(i = 1;i<n-1;i++)
	{
		if(use_brush)
		{
			float p = Get_Brush_Strength_At_Pixel(o->tracked_pixels[(i*2)], o->tracked_pixels[(i*2)+1]);
			float ip = 1.0f-p;
			o->tracked_pixels[(i*2)] = (temp[(i*2)]*p)+(o->tracked_pixels[(i*2)]*ip);
			o->tracked_pixels[(i*2)+1] = (temp[(i*2)+1]*p)+(o->tracked_pixels[(i*2)+1]*ip);
		}
		else
		{
			o->tracked_pixels[(i*2)] = temp[(i*2)];
			o->tracked_pixels[(i*2)+1] = temp[(i*2)+1];
		}
	}
	delete[] temp;
	return false;
}

__forceinline bool Get_Next_Pixel(OUTLINE *o, int p, int max, int *res)
{
	for(int i = p+1;i<max;i++)
	{
		if(o->tracked_pixels[i*2]>=0)
		{
			*res = i;
			return true;
		}
	}
	*res = -1;
	return false;
}

__forceinline bool Get_Previous_Pixel(OUTLINE *o, int p, int *res)
{
	for(int i = p-1;i>-1;i--)
	{
		if(o->tracked_pixels[i*2]>=0)
		{
			*res = i;
			return true;
		}
	}
	*res = -1;
	return false;
}

__forceinline int Side_Of_Plane2D(float *p, float *n, float *t)
{
	double xx = (double)t[0] - (double)p[0];
	double yy = (double)t[1] - (double)p[1];
	double zz = (double)t[2] - (double)p[2];
	if((((double)n[0] * xx) + ((double)n[1] * yy))>0.0000){return 0;}
	return 1;
}

__forceinline bool Pixel_Crossed(OUTLINE *o, int a, int b, int c)
{
	float apos[3] = {o->tracked_pixels[(a*2)],o->tracked_pixels[(a*2)+1], 0};
	float bpos[3] = {o->tracked_pixels[(b*2)],o->tracked_pixels[(b*2)+1], 0};
	float cpos[3] = {o->tracked_pixels[(c*2)],o->tracked_pixels[(c*2)+1], 0};
	float dir[3];
	dir[0] = apos[0] - cpos[0];
	dir[1] = apos[1] - cpos[1];
	dir[2] = 0;
	int s1 = Side_Of_Plane2D(bpos, dir, apos);
	int s2 = Side_Of_Plane2D(bpos, dir, cpos);
	return s1==s2;
}

bool Eliminate_Crossed_Pixels(OUTLINE *o)
{
	int n = o->tracked_pixels.size()/2;
	int next, previous;
	vector<int> crossed_pixels;
	int i;
	for(i = 1;i<n-1;i++)
	{
		if(o->tracked_pixels[i*2]>=0)
		{
			if(Get_Next_Pixel(o, i, n, &next)&&Get_Previous_Pixel(o, i, &previous))
			{
				if(Pixel_Crossed(o, previous, i, next))
				{
					crossed_pixels.push_back(i);
				}
			}
		}
	}
	n = crossed_pixels.size();
	for(i = 0;i<n;i++)
	{
		o->tracked_pixels[crossed_pixels[i]*2] = -1;
		o->tracked_pixels[(crossed_pixels[i]*2)+1] = -1;
	}
	crossed_pixels.clear();
	return true;
}

__forceinline bool Get_Pixel_Vector(OUTLINE *o, int a, int b, float *res, bool use_brush)
{
	float apos[3] = {o->tracked_pixels[(a*2)],o->tracked_pixels[(a*2)+1], 0};
	float bpos[3] = {o->tracked_pixels[(b*2)],o->tracked_pixels[(b*2)+1], 0};
	float vec[3] = {apos[0]-bpos[0],apos[1]-bpos[1],0};
	float pull = 1;
	if(use_brush)
	{
		pull = Get_Brush_Strength_At_Pixel(bpos[0], bpos[1]);
		if(pull==0)
		{
			res[0] = 0;
			res[1] = 0;
			return true;
		}
	}
	float up[3] = {0,0,1};
	float dir[3];
	Cross(vec, up, dir);
	SetLength(dir, pull);
	res[0] = dir[0];
	res[1] = dir[1];
	return true;
}

bool Expand_Tracking_Outlines(OUTLINE *o, float dir, bool use_brush)
{
	int n = o->tracked_pixels.size()/2;
	float *temp_dir = new float[n*2];
	int next, previous;
	int i;
	for(i = 1;i<n-1;i++)
	{
		if(o->tracked_pixels[i*2]>=0)
		{
			if(Get_Next_Pixel(o, i, n, &next)&&Get_Previous_Pixel(o, i, &previous))
			{
				Get_Pixel_Vector(o, next, previous, &temp_dir[i*2], use_brush);
			}
		}
	}
	for(i = 1;i<n-1;i++)
	{
		if(o->tracked_pixels[i*2]>=0)
		{
			o->tracked_pixels[(i*2)] += temp_dir[(i*2)]*dir;
			o->tracked_pixels[(i*2)+1] += temp_dir[(i*2)+1]*dir;
		}
	}
	delete[] temp_dir;
	return true;
}

//track outlines for this layer
bool Track_Outlines(LAYER_OUTLINES *lo)
{
	int n = lo->outlines.size();
	for(int i = 0;i<n;i++)
	{
		lo->outlines[i]->tracked_pixels.clear();
		Track_KLT_Points(&lo->outlines[i]->original_pixels, &lo->outlines[i]->tracked_pixels);
		Eliminate_Crossed_Pixels(lo->outlines[i]);
	}
	return true;
}

bool Copy_Tracked_Outlines(LAYER_OUTLINES *lo)
{
	int n = lo->outlines.size();
	for(int i = 0;i<n;i++)
	{
		int np = lo->outlines[i]->tracked_pixels.size();
		for(int j = 0;j<np;j++)
		{
			lo->outlines[i]->original_pixels[i] = lo->outlines[i]->tracked_pixels[i];
		}
	}
	return true;
}


extern bool redraw_edit_window;

bool track_outlines_tool_open = false;

bool Open_Track_Outlines_Tool()
{
	if(track_outlines_tool_open)
	{
		return false;
	}
	track_outlines_tool_open = true;
	Open_Tracking_Outline_Dlg();
	Generate_Tracking_Info_For_Selection();
	return false;
}

bool Close_Track_Outlines_Tool()
{
	if(!track_outlines_tool_open)
	{
		return false;
	}
	Close_Tracking_Outline_Dlg();
	Clear_Tracking_Info();
	track_outlines_tool_open = false;
	return false;
}

bool Generate_Tracking_Info_For_Selection()
{
	Stop_KLT_Tracking();
	Clear_Layer_Outlines();
	int current_frame = Get_Current_Project_Frame();
	redraw_edit_window = true;
	Find_All_Layer_Strips();
	Create_Outlines();
	Update_Outline_Extents();
	if(layer_outlines.size()==0)
	{
		return true;
	}
	char file[512];
	Get_Frame_Image(current_frame, file);
	if(Get_Num_Frames()==0)
	{
		strcpy(file, frame->image_file);
	}
	Reset_KLT_Tracking(file);
	return true;
}

bool Clear_Tracking_Info()
{
	Stop_KLT_Tracking();
	Clear_Layer_Outlines();
	redraw_edit_window = true;
	return true;
}

bool Interpolate_Missing_Tracked_Pixels(OUTLINE *o)
{
	Remove_Stray_Pixels(o);
	return true;
}

bool Interpolate_Missing_Tracked_Pixels(LAYER_OUTLINES *lo)
{
	int n = lo->outlines.size();
	for(int i = 0;i<n;i++)
	{
		Interpolate_Missing_Tracked_Pixels(lo->outlines[i]);
	}
	return true;
}

bool Create_Layers_From_Outlines(LAYER_OUTLINES *lo)
{
	int current_frame = Get_Current_Project_Frame();
	if(lo->original_frame==current_frame)
	{
		return false;
	}
	Interpolate_Missing_Tracked_Pixels(lo);
	Get_Outline_RLE_Strips(lo);
	Copy_Reference_Layer(lo->original_frame, current_frame, lo->layer_id);
	int n = lo->new_strips.size();
	int index = Get_Layer_Index(current_frame, lo->layer_id);
	for(int i = 0;i<n;i++)
	{
		RLE_STRIP *rs = &lo->new_strips[i];
		Force_Add_Layer_RLE_Strip(current_frame, index, rs->y, rs->start_x, rs->end_x, true);
	}
	lo->new_strips.clear();
	return true;
}

bool Replace_Layers_From_Outlines(LAYER_OUTLINES *lo)
{
	int current_frame = Get_Current_Project_Frame();
	if(!Evaporate_Layer(lo->layer_id))
	{
		return false;
	}
	Interpolate_Missing_Tracked_Pixels(lo);
	Get_Outline_RLE_Strips(lo);
	int n = lo->new_strips.size();
	int index = Get_Layer_Index(current_frame, lo->layer_id);
	for(int i = 0;i<n;i++)
	{
		RLE_STRIP *rs = &lo->new_strips[i];
		Force_Add_Layer_RLE_Strip(current_frame, index, rs->y, rs->start_x, rs->end_x, true);
	}
	lo->new_strips.clear();
	return true;
}


bool Create_Layers_From_Tracking_Info()
{
	int n = layer_outlines.size();
	for(int i = 0;i<n;i++)
	{
		Create_Layers_From_Outlines(layer_outlines[i]);
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
	redraw_edit_window = true;
	return true;
}


bool Replace_Layers_From_Tracking_Info()
{
	int n = layer_outlines.size();
	for(int i = 0;i<n;i++)
	{
		Replace_Layers_From_Outlines(layer_outlines[i]);
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
	redraw_frame = true;
	redraw_edit_window = true;
	return true;
}

bool Interpolate_Selection_Between_Frames()
{
	return false;
}

bool Render_Tracking_Outline(OUTLINE *lo)
{
	int n = lo->tracked_pixels.size()/2;
	glColor3f(1,0,1);
	glBegin(GL_LINE_STRIP);
	for(int i = 0;i<n;i++)
	{
		if(lo->tracked_pixels[i*2]>0)
		{
			glVertex2f(lo->tracked_pixels[i*2]+0.5f, lo->tracked_pixels[(i*2)+1]+0.5f);
		}
	}
	glEnd();
	glColor3f(1,1,0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(lo->tlx, lo->tly);
	glVertex2f(lo->thx, lo->tly);
	glVertex2f(lo->thx, lo->thy);
	glVertex2f(lo->tlx, lo->thy);
	glVertex2f(lo->tlx, lo->tly);
	glEnd();
	glColor3f(1,1,1);
	return true;
}

bool Render_Tracking_Outlines(LAYER_OUTLINES *lo)
{
	int n = lo->outlines.size();
	glLineWidth(Pixel_Size()*2);
	for(int i = 0;i<n;i++)
	{
		Render_Tracking_Outline(lo->outlines[i]);
	}
	glLineWidth(1);
	return true;
}

bool Tracking_Outline_Is_Active()
{
	return layer_outlines.size()>0;
}

bool Render_Tracking_Info()
{
	int n = layer_outlines.size();
	for(int i = 0;i<n;i++)
	{
		Render_Tracking_Outlines(layer_outlines[i]);
	}
	return false;
}

bool Update_Motion_Tracking()
{
	int n = layer_outlines.size();
	for(int i = 0;i<n;i++)
	{
		Track_Outlines(layer_outlines[i]);
	}
	Update_Outline_Extents();
	return true;
}

bool Extra_Smooth_Tracking_Outlines()
{
	int n = layer_outlines.size();
	for(int i = 0;i<n;i++)
	{
		int no = layer_outlines[i]->outlines.size();
		for(int j = 0;j<no;j++)
		{
			Smooth_Tracking_Outlines(layer_outlines[i]->outlines[j], false);
		}
	}
	redraw_edit_window = true;
	return true;
}

bool ExpandContract_Outlines(float amount, bool use_brush)
{
	int n = layer_outlines.size();
	for(int i = 0;i<n;i++)
	{
		int no = layer_outlines[i]->outlines.size();
		for(int j = 0;j<no;j++)
		{
			Expand_Tracking_Outlines(layer_outlines[i]->outlines[j], amount, use_brush);
		}
	}
	redraw_edit_window = true;
	return true;
}


bool Expand_Tracking_Outline_Within_Brush()
{
	ExpandContract_Outlines(1, true);
	return true;
}

bool Contract_Tracking_Outline_Within_Brush()
{
	ExpandContract_Outlines(-1, true);
	return true;
}

bool Smooth_Tracking_Outline_Within_Brush()
{
	int n = layer_outlines.size();
	for(int i = 0;i<n;i++)
	{
		int no = layer_outlines[i]->outlines.size();
		for(int j = 0;j<no;j++)
		{
			Smooth_Tracking_Outlines(layer_outlines[i]->outlines[j], true);
		}
	}
	redraw_edit_window = true;
	return true;
}
