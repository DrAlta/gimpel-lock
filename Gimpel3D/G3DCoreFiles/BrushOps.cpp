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
#include "Console.h"


float fastsqrt(float n);

extern float last_edit_px;
extern float last_edit_py;
extern int brush_size;

//this could be in a brush header for all ops
__forceinline float Get_Brush_Strength(float x, float y)
{
	float dx = x-last_edit_px;
	float dy = y-last_edit_py;
	float d = fastsqrt((dx*dx)+(dy*dy));
	if(d>brush_size)
	{
		return 0;
	}
	return 1.0f-(d/brush_size);
}

bool Pixel_Within_Brush(int x, int y)
{
	float dx = x-last_edit_px;
	float dy = y-last_edit_py;
	float d = fastsqrt((dx*dx)+(dy*dy));
	if(d>brush_size)
	{
		return false;
	}
	return true;
}

bool Get_Brush_Position(float *px, float *py)
{
	*px = last_edit_px;
	*py = last_edit_py;
	return true;
}

float Get_Brush_Size()
{
	return brush_size;
}

__forceinline float Get_Average_Map_Value(float *map, int x, int y, int w, int h)
{
	float d = 0;
	int cnt = 0;
	if(map[((y-1)*w)+(x-1)]>0){d += map[((y-1)*w)+(x-1)];cnt++;}
	if(map[((y-1)*w)+(x)]>0){d += map[((y-1)*w)+(x)];cnt++;}
	if(map[((y-1)*w)+(x+1)]>0){d += map[((y-1)*w)+(x+1)];cnt++;}
	if(map[((y)*w)+(x-1)]>0){d += map[((y)*w)+(x-1)];cnt++;}
	if(map[((y)*w)+(x)]>0){d += map[((y)*w)+(x)];cnt++;}
	if(map[((y)*w)+(x+1)]>0){d += map[((y)*w)+(x+1)];cnt++;}
	if(map[((y+1)*w)+(x-1)]>0){d += map[((y+1)*w)+(x-1)];cnt++;}
	if(map[((y+1)*w)+(x)]>0){d += map[((y+1)*w)+(x)];cnt++;}
	if(map[((y+1)*w)+(x+1)]>0){d += map[((y+1)*w)+(x+1)];cnt++;}
	if(cnt==0)
	{
		return map[((y)*w)+(x)];
	}
	return d/cnt;
}

//abstract to generic function in the future
//applies blur operation to "map", storing result in "res"
bool AntiAlias_Float_Grid(int w, int h, float *map, float *res)
{
	int i, j;
	for(i = 1;i<w-1;i++)
	{
		for(j = 1;j<h-1;j++)
		{
			if(map[(j*w)+i]>0)
			{
				res[(j*w)+i] = Get_Average_Map_Value(map, i, j, w, h);
			}
			else
			{
				res[(j*w)+i] = -1;
			}
		}
	}
	return true;
}

int Get_Brush_Grid_Size()
{
	int dsize = (brush_size*2)+4;
	return dsize;
}

bool Get_Brush_Grid_Start(int *px, int *py)
{
	int dsize = (brush_size*2)+4;
	*px = (last_edit_px-(dsize/2));
	*py = (last_edit_py-(dsize/2));
	return true;
}


///////
//store semi-static bufers for brush depth and strength grids

float *brush_depth_grid = 0;
float *float_grid = 0;
float *brush_strength_grid = 0;

int last_brush_depth_grid_size = 0;
int last_float_grid_size = 0;
int last_brush_strength_grid_size = 0;

__forceinline float* Get_Brush_Depth_Grid_Buffer(int size)
{
	if(size!=last_brush_depth_grid_size)
	{
		if(brush_depth_grid)delete[] brush_depth_grid;
		last_brush_depth_grid_size = size;
		brush_depth_grid = new float[size*size];
	}
	return brush_depth_grid;
}

__forceinline float* Get_Float_Grid_Buffer(int size)
{
	if(size!=last_float_grid_size)
	{
		if(float_grid)delete[] float_grid;
		last_float_grid_size = size;
		float_grid = new float[size*size];
	}
	return float_grid;
}

__forceinline float* Get_Brush_Strength_Grid_Buffer(int size)
{
	if(size!=last_brush_strength_grid_size)
	{
		if(brush_strength_grid)delete[] brush_strength_grid;
		last_brush_strength_grid_size = size;
		brush_strength_grid = new float[size*size];
	}
	return brush_strength_grid;
}

bool Free_Brush_Grid_Buffers()
{
	if(brush_depth_grid)delete[] brush_depth_grid;
	if(float_grid)delete[] float_grid;
	if(brush_strength_grid)delete[] brush_strength_grid;
	brush_depth_grid = 0;
	float_grid = 0;
	brush_strength_grid = 0;
	last_brush_depth_grid_size = 0;
	last_float_grid_size = 0;
	last_brush_strength_grid_size = 0;
	return true;
}


//get a grid of depth values for the specified pixel range
float* Get_Brush_Depth_Grid(int startx, int starty, int grid_size)
{
	//needs to be filled in fresh for each call
	float *grid = Get_Brush_Depth_Grid_Buffer(grid_size);
	int i, j;
	for(i = 0;i<grid_size;i++)
	{
		for(j = 0;j<grid_size;j++)
		{
			grid[(j*grid_size)+i] = frame->Get_Clamped_Pixel_Depth(startx+i, starty+j);
		}
	}
	return grid;
}

//get a grid of depth values for the specified pixel range, only for selected pixels
float* Get_Brush_Selected_Depth_Grid(int startx, int starty, int grid_size)
{
	//needs to be filled in fresh for each call
	float *grid = Get_Brush_Depth_Grid_Buffer(grid_size);
	int i, j;
	for(i = 0;i<grid_size;i++)
	{
		for(j = 0;j<grid_size;j++)
		{
			if(frame->Pixel_Is_Selected(startx+i, starty+j))
			{
				grid[(j*grid_size)+i] = frame->Get_Clamped_Pixel_Depth(startx+i, starty+j);
			}
			else
			{
				grid[(j*grid_size)+i] = -1;
			}
		}
	}
	return grid;
}


//get a grid of anti-aliased depth values for the specified pixel range
float* Get_AntiAliased_Brush_Depth_Grid(float *depth_grid, int grid_size)
{
	float *aa_grid = Get_Float_Grid_Buffer(grid_size);
	AntiAlias_Float_Grid(grid_size, grid_size, depth_grid, aa_grid);
	return aa_grid;
}

//get the average value of a grid of depth values for the specified pixel range
float Get_Averaged_Brush_Depth_Grid(float *depth_grid, int grid_size)
{
	int n = grid_size*grid_size;
	float res = 0;
	int cnt = 0;
	for(int i = 0;i<n;i++)
	{
		if(depth_grid[i]>0)//only valid depths!
		{
			res += depth_grid[i];
			cnt++;
		}
	}
	res = res/cnt;
	return res;
}


//get the brush strength for brush at bx/by at size bsize, tested with pixel px/py
__forceinline float Get_Linear_Brush_Strength(float bx, float by, float bsize, float px, float py)
{
	float dx = px-bx;
	float dy = py-by;
	float d = fastsqrt((dx*dx)+(dy*dy));
	if(d>bsize)
	{
		return 0;
	}
	return 1.0f-(d/bsize);
}

//fills a grid of floats with brush strength for pixels given the
//brush position, grid start x/y, and grid size
float* Get_Linear_Brush_Strength_Grid(float bx, float by, float bsize, int startx, int starty, int grid_size)
{
	if(last_brush_strength_grid_size==grid_size)
	{
		//last values are still good
		return brush_strength_grid;
	}
	float *grid = Get_Brush_Strength_Grid_Buffer(grid_size);
	int i, j;
	memset(grid, 0, sizeof(float)*grid_size*grid_size);
	for(i = 0;i<grid_size;i++)
	{
		for(j = 0;j<grid_size;j++)
		{
			grid[(j*grid_size)+i] = Get_Linear_Brush_Strength(bx, by, bsize, startx+i, starty+j);
		}
	}
	return grid;
}


void Shift_Border_Left(int layer_id, int x, int y);
void Shift_Border_Right(int layer_id, int x, int y);

bool Shift_Pixels_Within_Brush(bool left, bool right)
{
	Clear_Modified_Layers();
	int dsize = (brush_size*2);
	int startx = (last_edit_px-(dsize/2));
	int starty = (last_edit_py-(dsize/2));
	int i, j;
	float bs = 0;
	bool instrip = false;
	int last_layer_id;
	int layer_id;
	for(j = 0;j<dsize;j++)
	{
		instrip = false;
		for(i = 0;i<dsize;i++)
		{
			bs = Get_Brush_Strength(startx+i, starty+j);
			if(bs>0)
			{
				if(!instrip)
				{
					instrip = true;
					last_layer_id = frame->Get_Pixel_Layer(startx+i, starty+j);
				}
				else
				{
					layer_id = frame->Get_Pixel_Layer(startx+i, starty+j);
					if(layer_id!=last_layer_id&&(Layer_Is_Selected(Get_Layer_Index(layer_id))||Layer_Is_Selected(Get_Layer_Index(last_layer_id))))
					{
						if(left)
						{
							Shift_Border_Left(last_layer_id, (startx+i)-1, starty+j);
						}
						else if(right)
						{
							Shift_Border_Right(last_layer_id, (startx+i)-1, starty+j);
							i++;
						}
					}
					last_layer_id = layer_id;
				}
			}
			else
			{
				if(instrip)
				{
					instrip = false;
				}
			}
		}
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
	redraw_frame = true;
	redraw_edit_window = true;
	return true;
}
