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
#include "EditOps.h"
#include "ControlMesh.h"
#include "Frame.h"
#include "Layers.h"
#include "RLECircleGrid.h"
#include "Console.h"
#include "Layers.h"
#include "PlanarProjection.h"
#include "EdgeDetection.h"
#include <math.h>
#include <GL/GL.h>
#include <vector>

using namespace std;

bool Get_Frame_Pixel_Depth(int x, int y, float *res);
bool Set_Frame_Pixel_Depth(int x, int y, float d);

void Get_Current_Mouse_Edit_Pixel(float *px, float *py);

bool Set_Layer_Depth_ID_Float(int layer_id, float f);

float DEFAULT_LAYER_DEPTH = 0.1f;

void Increment_Default_Layer_Depth()
{
	DEFAULT_LAYER_DEPTH += 0.005f;
}

void Reset_Default_Layer_Depth()
{
	DEFAULT_LAYER_DEPTH = 0.1f;
}

bool Get_Edit_World_Pos(int x, int y, float *px, float *py);

extern int last_brush_x, last_brush_y;

float Pixel_Size();


void Default_StartStop_Callback(bool b)
{
	if(b)
	{
	}
	else
	{
	}
}

void Add_Control_Point_Callback(float x, float y, float range, float amount)
{
	Add_Control_Mesh_Point(x, y);
}

void Delete_Control_Point_Callback(float x, float y, float range, float amount)
{
	Remove_Control_Mesh_Point(Get_Selected_Control_Mesh_Point(x, y, range));
}

void PushPull_Control_Point_Callback(float x, float y, float range, float amount)
{
	int id = Get_Selected_Control_Mesh_Point(x, y, range);
	if(id==-1)return;
	float depth = Get_Control_Mesh_Point_Depth(id);
	amount = (2000.0f+amount)/2000.0f;
	depth *= amount;
	Set_Control_Mesh_Point_Depth(id, depth);
}

vector<float> overlay_line;
vector<float> temp_overlay_line;
vector<int> segment_point_counts;
int overlay_undo = 0;

void Reset_Overlay_Line()
{
	overlay_line.clear();
	overlay_undo = 0;
}

void Clear_Overlay_Undo()
{
	int i;
	for(i= 0;i<overlay_undo;i++)
	{
		segment_point_counts.pop_back();
	}
	overlay_undo = 0;
	int n = segment_point_counts.size();
	if(n>0)
	{
		int pc = segment_point_counts[n-1]*2;
		int ls = overlay_line.size();
		int cnt = ls-pc;
		for(i = 0;i<cnt;i++)
		{
			overlay_line.pop_back();
		}
	}
	else
	{
		overlay_line.clear();
	}
}

bool keep_adding = false;
bool line_detection_failed = false;

int start_add_points = 0;

bool Test_Add_Point(float x, float y, vector<float> *line)
{
	int n = line->size();
	n -= start_add_points;
	for(int i = 0;i<n;i+=2)
	{
		if((*line)[i+start_add_points]==x&&(*line)[i+start_add_points+1]==y)
		{
			//point already exists, incremental add has looped around
			//delete this point and everything beyond, it apparently leads to loop condition
			//next point will be slightly different and incremental add will resume
			int extra = n-i;
			for(int j = 0;j<extra;j++)
			{
				line->pop_back();
			}
			return false;
		}
	}
	return true;
}

//used for edge detection
void Add_Horizontal_Edge_Point(float x1, float y1, float x2, float y2, vector<float> *line)
{
	int ix1 = x1;
	int ix2 = x2;
	float inc = 1.0f/fabs((float)(ix1-ix2));
	float p = inc;
	float ny;
	int iny;
	int i;
	float d = fabs(x2-x1);
	if(d<2)
	{
		keep_adding = false;
		return;
	}
	if(x2>x1)
	{
		i = ix1 + 1;
		ny = (y1*(1.0f-p))+(y2*p);
		iny = ny;
		Detect_Vertical_Edge_Selection(i, &iny);
		if(Test_Add_Point(i, iny, line))
		{
			line->push_back(i);
			line->push_back(iny);
		}
		else
		{
			//try unaltered point
			iny = ny;
			if(Test_Add_Point(i, iny, line))
			{
				line->push_back(i);
				line->push_back(iny);
			}
			else
			{
				//skip over trouble spot
				i++;
				if(Test_Add_Point(i, iny, line))
				{
					line->push_back(i);
					line->push_back(iny);
				}
				else
				{
					//try new modified point
					Detect_Vertical_Edge_Selection(i, &iny);
					if(Test_Add_Point(i, iny, line))
					{
						line->push_back(i);
						line->push_back(iny);
					}
					else
					{
						line_detection_failed = true;
					}
				}
			}
		}
	}
	else
	{
		i = ix1 - 1;
		ny = (y1*(1.0f-p))+(y2*p);
		iny = ny;
		Detect_Vertical_Edge_Selection(i, &iny);
		if(Test_Add_Point(i, iny, line))
		{
			line->push_back(i);
			line->push_back(iny);
		}
		else
		{
			//try unaltered point
			iny = ny;
			if(Test_Add_Point(i, iny, line))
			{
				line->push_back(i);
				line->push_back(iny);
			}
			else
			{
				//skip over trouble spot
				i--;
				if(Test_Add_Point(i, iny, line))
				{
					line->push_back(i);
					line->push_back(iny);
				}
				else
				{
					Detect_Vertical_Edge_Selection(i, &iny);
					if(Test_Add_Point(i, iny, line))
					{
						line->push_back(i);
						line->push_back(iny);
					}
					else
					{
						line_detection_failed = true;
					}
				}
			}
		}
	}
}

//used for edge detection
void Add_Vertical_Edge_Point(float x1, float y1, float x2, float y2, vector<float> *line)
{
	int iy1 = y1;
	int iy2 = y2;
	float inc = 1.0f/fabs((float)(iy1-iy2));
	float p = inc;
	float nx;
	int inx;
	int i;
	float d = fabs(y2-y1);
	if(d<2)
	{
		keep_adding = false;
		return;
	}
	if(y2>y1)
	{
		i = iy1 + 1;
		nx = (x1*(1.0f-p))+(x2*p);
		inx = nx;
		Detect_Horizontal_Edge_Selection(&inx, i);
		if(Test_Add_Point(inx, i, line))
		{
			line->push_back(inx);
			line->push_back(i);
		}
		else
		{
			//try unaltered point
			inx = nx;
			if(Test_Add_Point(inx, i, line))
			{
				line->push_back(inx);
				line->push_back(i);
			}
			else
			{
				//skip over trouble spot
				i++;
				if(Test_Add_Point(inx, i, line))
				{
					line->push_back(inx);
					line->push_back(i);
				}
				else
				{
					Detect_Horizontal_Edge_Selection(&inx, i);
					if(Test_Add_Point(inx, i, line))
					{
						line->push_back(inx);
						line->push_back(i);
					}
					else
					{
						line_detection_failed = true;
					}
				}
			}
		}
	}
	else
	{
		i = iy1 - 1;
		nx = (x1*(1.0f-p))+(x2*p);
		inx = nx;
		Detect_Horizontal_Edge_Selection(&inx, i);
		if(Test_Add_Point(inx, i, line))
		{
			line->push_back(inx);
			line->push_back(i);
		}
		else
		{
			//try unaltered point
			inx = nx;
			if(Test_Add_Point(inx, i, line))
			{
				line->push_back(inx);
				line->push_back(i);
			}
			else
			{
				i--;
				if(Test_Add_Point(inx, i, line))
				{
					line->push_back(inx);
					line->push_back(i);
				}
				else
				{
					Detect_Horizontal_Edge_Selection(&inx, i);
					if(Test_Add_Point(inx, i, line))
					{
						line->push_back(inx);
						line->push_back(i);
					}
					else
					{
						line_detection_failed = true;
					}
				}
			}
		}
	}
}



bool Prune_Backtracked_Points(float x, float y)
{
	return false;
	float closest_distance = 100000;
	int closest_index = -1;
	int n = overlay_line.size();
	if(n<3)
	{
		return false;
	}
	int i;
	for(i = start_add_points;i<n;i+=2)
	{
		float dx = overlay_line[i]-x;
		float dy = overlay_line[i]-y;
		float d = (float)sqrt((dx*dx)+(dy*dy));
		if(d<closest_distance)
		{
			closest_distance = d;
			closest_index = i;
		}
	}
	int extra = n-closest_index;
	for(i = 0;i<extra;i++)
	{
		overlay_line.pop_back();
	}
	return true;
}

void Detect_Edges(float x, float y)
{
	int n = overlay_line.size();
	if(n==0)
	{
		//first point, adjust to nearest contrast point within range
		overlay_line.push_back(x);
		overlay_line.push_back(y);
	}
	else
	{
		//compare with previous point, fill in all pixels
		//compare xy deltas
		float last_x = overlay_line[n-2];
		float last_y = overlay_line[n-1];
		float dx = fabs(last_x-x);
		float dy = fabs(last_y-y);
		keep_adding = true;

		start_add_points = overlay_line.size();
		
		int num_expected_passes = 0;
		int num_passes = 0;
		if(dx>dy)
		{
			num_expected_passes = (int)fabs(dx*5);
		}
		else
		{
			num_expected_passes = (int)fabs(dy*5);
		}
		line_detection_failed = false;
		while(keep_adding)
		{
			if(num_passes>num_expected_passes||line_detection_failed)
			{
				keep_adding = false;
				return;
			}
			n = overlay_line.size();
			float last_x = overlay_line[n-2];
			float last_y = overlay_line[n-1];
			float dx = fabs(last_x-x);
			float dy = fabs(last_y-y);
			
			if(dx>dy)
			{
				//horizontal stroke
				Add_Horizontal_Edge_Point(overlay_line[n-2], overlay_line[n-1], x, y, &overlay_line);
			}
			else
			{
				//vertical stroke
				Add_Vertical_Edge_Point(overlay_line[n-2], overlay_line[n-1], x, y, &overlay_line);
			}
			num_passes++;
		}
	}
	Prune_Backtracked_Points(x, y);
}

void Add_Overlay_Line_Point(float x, float y)
{
	Clear_Overlay_Undo();
	if(edge_detection_enabled)
	{
		Detect_Edges(x, y);
	}
	else
	{
		overlay_line.push_back(x);
		overlay_line.push_back(y);
	}
}

void Render_Overlay_Line()
{
	int n = overlay_line.size();
	if(n==0)
	{
		return;
	}
	n -= overlay_undo*2;
	glLineWidth(Pixel_Size()*2.5f);
	glPointSize(5);
	if(n==0)
	{
		glColor3f(0,1,0);
		glBegin(GL_POINTS);
		glVertex2f(overlay_line[0], overlay_line[1]);
		glEnd();
	}
	else
	{
		int ns = (segment_point_counts.size()-1)-overlay_undo;
		int npc = segment_point_counts[ns]*2;

		glColor3f(1,0,0);
		glBegin(GL_LINE_STRIP);
		for(int i = 0;i<npc;i+=2)
		{
			glVertex2f(overlay_line[i], overlay_line[i+1]);
		}
		glEnd();

		glColor3f(0,1,0);
		glBegin(GL_POINTS);
		glVertex2f(overlay_line[npc-2], overlay_line[npc-1]);
		glEnd();
	}
	glPointSize(1);
	glLineWidth(1);
	glColor3f(1,1,1);
}

bool Render_Temp_Automatic_Line(float x, float y)
{
	int n;
	float last_x = temp_overlay_line[0];
	float last_y = temp_overlay_line[1];
	float dx = fabs(last_x-x);
	float dy = fabs(last_y-y);
	keep_adding = true;

	start_add_points = 2;
		
	int num_expected_passes = 0;
	int num_passes = 0;
	if(dx>dy)
	{
		num_expected_passes = (int)fabs(dx*5);
	}
	else
	{
		num_expected_passes = (int)fabs(dy*5);
	}
	line_detection_failed = false;
	while(keep_adding)
	{
		if(num_passes>num_expected_passes||line_detection_failed)
		{
			keep_adding = false;
			return false;
		}
		n = temp_overlay_line.size();
		float last_x = temp_overlay_line[n-2];
		float last_y = temp_overlay_line[n-1];
		float dx = fabs(last_x-x);
		float dy = fabs(last_y-y);
		
		if(dx>dy)
		{
			//horizontal stroke
			Add_Horizontal_Edge_Point(temp_overlay_line[n-2], temp_overlay_line[n-1], x, y, &temp_overlay_line);
		}
		else
		{
			//vertical stroke
			Add_Vertical_Edge_Point(temp_overlay_line[n-2], temp_overlay_line[n-1], x, y, &temp_overlay_line);
		}
		num_passes++;
	}
	n = temp_overlay_line.size();
	glColor3f(1,0,0);
	glLineWidth(Pixel_Size()*2.5f);
	glBegin(GL_LINE_STRIP);
	for(int i = 0;i<n;i+=2)
	{
		glVertex2f(temp_overlay_line[i], temp_overlay_line[i+1]);
	}
	glEnd();
	glPointSize(1);
	glLineWidth(1);
	glColor3f(1,1,1);
	return true;
}

bool Render_Axis_Lines()
{
	float px, py;
	Get_Current_Mouse_Edit_Pixel(&px, &py);
	if(edge_detection_enabled)
	{
		glColor3f(1,0,0);
	}
	else
	{
		glColor3f(0,1,0);
	}
	glBegin(GL_LINES);
	glVertex2f(px-10000, py);
	glVertex2f(px+10000, py);
	glVertex2f(px, py-10000);
	glVertex2f(px, py+10000);
	glEnd();

	int n = overlay_line.size();
	if(n>0)
	{
		if(edge_detection_enabled)
		{
			temp_overlay_line.push_back(overlay_line[n-2]);
			temp_overlay_line.push_back(overlay_line[n-1]);
			Render_Temp_Automatic_Line(px, py);
			temp_overlay_line.clear();
		}
		else
		{
			glBegin(GL_LINES);
			glVertex2f(px, py);
			n -= overlay_undo*2;
			glVertex2f(overlay_line[n-2], overlay_line[n-1]);
			glEnd();
		}
	}
	return true;
}


__forceinline void Plot(int x, int y, bool *grid, int w, int h)
{
	if(x<0||x>=w||y<0||y>=h)return;
	grid[(y*w)+x] = true;
}

float VecLength(float* v);

//fill points in a grid based on line segment
void FillLine(float x1, float x2, float y1, float y2, bool *grid, int w, int h)
{
	float v[3] = {0, x1-x2, y1-y2};
	float d = VecLength(v);
	int nd = d*2;
	Plot(x1, y1, grid, w, h);
	Plot(x2, y2, grid, w, h);
	float nx, ny;
	for(int i = 0;i<nd;i++)
	{
		float p = (float)i/(nd+1);
		nx = (x1*p)+(x2*(1.0f-p));
		ny = (y1*p)+(y2*(1.0f-p));
		Plot(nx, ny, grid, w, h);
	}
}

void Fill_Lasso_Selection_Strip(bool *strip, int w)
{
	bool inside = false;
	for(int i = 0;i<w;i++)
	{
		if(strip[i])
		{
			inside = !inside;
			while(strip[i])i++;
		}
		strip[i] = inside;
	}
}

//lasso fill recurse pos
struct LFRP
{
	int x, y;
};

__forceinline bool Grid_Filled(int x, int y, bool *grid, int w, int h)
{
	if(x<0||x>=w||y<0||y>=h)return true;
	return grid[(y*w)+x];
}

//flood-fill the lasso selection
void Fill_Lasso_Selection_Grid(bool *grid, int w, int h)
{
	vector<LFRP> rps;
	bool done = false;
	int n;
	LFRP lp, tlp, *plp;
	lp.x = 0;
	lp.y = 0;
	rps.push_back(lp);
	while(!done)
	{
		n = rps.size();
		if(n==0)
		{
			done = true;
		}
		else
		{
			plp = &rps[n-1];
			tlp.x = plp->x;
			tlp.y = plp->y;
			rps.pop_back();
			if(!Grid_Filled(tlp.x-1, tlp.y, grid, w, h))
			{
				lp.x = tlp.x-1;
				lp.y = tlp.y;
				rps.push_back(lp);
			}
			if(!Grid_Filled(tlp.x+1, tlp.y, grid, w, h))
			{
				lp.x = tlp.x+1;
				lp.y = tlp.y;
				rps.push_back(lp);
			}
			if(!Grid_Filled(tlp.x, tlp.y-1, grid, w, h))
			{
				lp.x = tlp.x;
				lp.y = tlp.y-1;
				rps.push_back(lp);
			}
			if(!Grid_Filled(tlp.x, tlp.y+1, grid, w, h))
			{
				lp.x = tlp.x;
				lp.y = tlp.y+1;
				rps.push_back(lp);
			}
			Plot(tlp.x, tlp.y, grid, w, h);
		}
	}
}

bool Get_Grid_Mask_RLE_Strips(bool *grid, int w, int h, vector<RLE_STRIP> *strips)
{
	int start_x, end_x;
	bool instrip = false;
	int j, i;
	RLE_STRIP rs;
	for(j = 0;j<h;j++)
	{
		for(i = 0;i<w;i++)
		{
			if(!instrip)
			{
				if(!grid[(j*w)+i])
				{
					instrip = true;
					start_x = i;
				}
			}
			else
			{
				if(grid[(j*w)+i])
				{
					instrip = false;
					end_x = i;
					rs.y = j;
					rs.start_x = start_x;
					rs.end_x = end_x;
					strips->push_back(rs);
				}
			}
		}
		if(instrip)
		{
			instrip = false;
			end_x = i;
		}
	}
	return true;
}


void Create_Layer_From_Selection_Grid(int x, int y, bool *grid, int w, int h)
{
	int i, j;
	bool instrip = false;
	int start_x, end_x;
	int layer_id;
	New_Layer(&layer_id, 0, 0);
	int layer_index = Get_Layer_Index(layer_id);
	for(j = 0;j<h;j++)
	{
		for(i = 0;i<w;i++)
		{
			if(!instrip)
			{
				if(!grid[(j*w)+i])
				{
					instrip = true;
					start_x = i;
				}
			}
			else
			{
				if(grid[(j*w)+i])
				{
					instrip = false;
					end_x = i;
					Add_Layer_RLE_Strip(layer_index, y+j, x+start_x, x+end_x, true);
				}
			}
		}
		if(instrip)
		{
			instrip = false;
			end_x = i;
		}
	}
	int num_strips = Num_RLE_Strips_In_Layer(layer_index);
	if(num_strips==0)
	{
		Delete_Layer(layer_id);
	}
	else
	{
		float pos[3];
		float dir[3] = {0, 0, 1};
		float rotation[3] = {0,0,0};
		float offset[3] = {0,0,0};
		Update_Modified_Layers();
		Clear_Modified_Layers();
		ReCalc_Layer_Center(layer_id, pos);
		if(pos[2]<0)
		{
			pos[2] = -pos[2];
		}
		Increment_Default_Layer_Depth();
		pos[2] = DEFAULT_LAYER_DEPTH;
		Set_Layer_Depth_ID_Float(layer_id, pos[2]);
		Project_Layer_To_Plane(layer_id, pos, dir, rotation, offset, PLANE_ORIGIN_SELECTION_CENTER);
		ReCalc_And_Set_Layer_Center(layer_id);
		Freeze_Layer(Find_Layer_Index(layer_id), true);
	}
	Update_Layer_List();
}

void Create_Lasso_Selection()
{
	int lx, hx, ly, hy;
	int n = overlay_line.size();
	if(n==0)return;
	int i, k;
	int px, py, npx, npy;
	float red[] = {1,0,0};
	float green[] = {0,1,0};
	lx = hx = overlay_line[0];
	ly = hy = overlay_line[1];
	for(i = 0;i<n;i+=2)
	{
		px = overlay_line[i];
		py = overlay_line[i+1];
		if(hx<px)hx = px;
		if(lx>px)lx = px;
		if(hy<py)hy = py;
		if(ly>py)ly = py;
	}
	int w = (hx-lx)+4;
	int h = (hy-ly)+4;
	bool *grid = new bool[w*h];
	memset(grid, 0, sizeof(bool)*w*h);
	for(i = 0;i<n-2;i+=2)
	{
		px = (overlay_line[i])-(lx-2);
		py = (overlay_line[i+1])-(ly-2);
		npx = (overlay_line[i+2])-(lx-2);
		npy = (overlay_line[i+3])-(ly-2);
		k = (py*w)+px;
		FillLine(px, npx, py, npy, grid, w, h);
	}
	Fill_Lasso_Selection_Grid(grid, w, h);
	int total = w*h;
	for(i = 0;i<total;i++)
	{
		if(!grid[i])
		{
			i = total;
			Create_Layer_From_Selection_Grid((lx-2), (ly-2), grid, w, h);
		}
	}
	delete[] grid;
}

void Create_Outline_Selection()
{
	Create_Lasso_Selection();
}


void StartStop_Lasso_Callback(bool b)
{
	Default_StartStop_Callback(b);
	if(!b)
	{
		Create_Lasso_Selection();
	}
	Reset_Overlay_Line();
}

void Lasso_Select_Callback(float x, float y, float range, float amount)
{
	Get_Current_Mouse_Edit_Pixel(&x, &y);
	Add_Overlay_Line_Point(x, y);
}

void Select_Layer_Callback(float x, float y, float range, float amount)
{
	int layer = frame->Get_Pixel_Layer(x, y);
	Toggle_Select_Layer(layer);
	List_Selected_Layers();
}

void StartStop_Outline_Callback(bool b)
{
}

void Outline_Select_Callback(float x, float y, float range, float amount)
{
	Get_Current_Mouse_Edit_Pixel(&x, &y);
	if(y<-100||x<-100)return;
	if(y>Frame_Height()+100||x>Frame_Width()+100)return;
	int n = overlay_line.size();
	if(n==0)
	{
		Print_To_Console("Starting new selection..");
	}
	Add_Overlay_Line_Point(x, y);
	n = overlay_line.size();
	segment_point_counts.push_back(n/2);
}

bool Start_Outline_Select()
{
	Default_StartStop_Callback(true);
	return true;
}

bool Cancel_Outline_Select()
{
	int n = overlay_line.size();
	if(n!=0)
	{
		Print_To_Console("Cancelling selection..");
	}
	Default_StartStop_Callback(false);
	Reset_Overlay_Line();
	return true;
}

bool Finish_Outline_Select()
{
	Default_StartStop_Callback(false);
	Clear_Overlay_Undo();
	int n = overlay_line.size();
	if(n==0)return false;
	overlay_line.push_back(overlay_line[0]);
	overlay_line.push_back(overlay_line[1]);
	Create_Outline_Selection();
	Reset_Overlay_Line();
	redraw_frame = true;
	return true;
}

bool Undo_Point()
{
	int n = overlay_line.size();
	if((overlay_undo+1)*2>=n)
	{
		return false;
	}
	overlay_undo++;
	return true;
}

bool Redo_Point()
{
	if(overlay_undo<=0)
	{
		return false;
	}
	int n = overlay_line.size();
	if(n==0)
	{
		return false;
	}
	overlay_undo--;
	return true;
}

void Circle_Select_Callback(float x, float y, float range, float amount)
{
	//find first selected layer
	int selected_layer_index = -1;
	int n = Num_Layers();
	int layer_id;
	int i;
	Clear_Modified_Layers();
	for(i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			selected_layer_index = i;
			layer_id = Get_Layer_ID(selected_layer_index);
			i = n;
		}
	}

	bool list_layers = false;
	bool fresh_layer = false;
	if(selected_layer_index==-1)
	{
		New_Layer(&layer_id, 0, 0);
		Update_Layer_List();
		selected_layer_index = Find_Layer_Index(layer_id);
		Select_Layer(selected_layer_index, true);
		Freeze_Layer(selected_layer_index, true);
		list_layers = true;
		fresh_layer = true;
	}
	int lx = x-range;
	int ly = y-range;
	int hx = x+range;
	int hy = y+range;
	Set_RLE_Circle_Grid((int)range);

	n = Num_Circle_Grid_RLE_Strips();
	for(i = 0;i<n;i++)
	{
		RLE_STRIP *rs = Get_Circle_Grid_RLE_Strip(i);
		Add_Layer_RLE_Strip(selected_layer_index, i+ly, rs->start_x+lx, rs->end_x+lx, fresh_layer);
	}

	Update_Modified_Layers();
	Clear_Modified_Layers();
	float pos[3];
	float dir[3] = {0, 0, 1};
	float rotation[3] = {0,0,0};
	float offset[3] = {0,0,0};
	ReCalc_Layer_Center(layer_id, pos);
	if(pos[2]<0)
	{
		pos[2] = -pos[2];
	}
	if(list_layers)
	{
		Increment_Default_Layer_Depth();
		pos[2] = DEFAULT_LAYER_DEPTH;
		Set_Layer_Depth_ID_Float(layer_id, pos[2]);
		Project_Layer_To_Plane(layer_id, pos, dir, rotation, offset, PLANE_ORIGIN_SELECTION_CENTER);
		ReCalc_And_Set_Layer_Center(layer_id);
		List_Selected_Layers();
	}
	redraw_frame = true;
}

void StartStop_Circle_Callback(bool b)
{
}

//FIXTHIS eliminate this system and just do these operations, no need to register and callback
bool Init_Basic_Edit_Ops()
{
	Register_Edit_Op(0, "Outline Create Layer", false, Outline_Select_Callback, StartStop_Outline_Callback);
	Register_Edit_Op(0, "Click Create Layer", false, Circle_Select_Callback, StartStop_Circle_Callback);
	Register_Edit_Op(0, "Select Layer(s)", true, Select_Layer_Callback, Default_StartStop_Callback);
	return true;
}

void Render_Edit_Overlay()
{
	Render_Overlay_Line();
}





