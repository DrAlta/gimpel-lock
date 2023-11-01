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
#include "HaloTool.h"
#include "Skin.h"
#include "Plugins.h"
#include "GeometryTool/GLBasic.h"
#include "G3DCoreFiles/Frame.h"
#include <gl/gl.h>
#include <gl/glext.h>

bool Get_Layer_AABB(int index, int *lx, int *ly, int *hx, int *hy);
float Pixel_Size();
bool Get_Num_Layer_Outline_Pixels(int index, int *res);
bool Get_Layer_Outline_Pixel(int index, int pixel, int *x, int *y);
void Free_GL_Texture(unsigned int id);
unsigned int Create_32Bit_Frame_Texture(int width, int height, float *data);
int Find_Layer_Index(int id);

bool Update_Current_Frame_Texture();

bool Get_RGB_Grid24(int startx, int starty, int width, int height, float *grid);
bool Get_RGB_Grid32(int startx, int starty, int width, int height, float *grid);

bool Project_Pixel_To_Layer(int layer_index, int px, int py, float *res);

//called when preview halo is about to change layers or dump data completely
bool Save_Preview_Halo_Data();//user is asked first

void Update_Win32_Messages();

bool Render_Stored_Halos_2D();
bool Render_Stored_Halos_2D_AABB();
bool Render_Stored_Halos_3D();

bool Ask_Save_Halo();

extern double modelview_matrix[16];
extern double projection_matrix[16];
extern int viewport[8];

void Get_Matrices();

bool Anti_Alias_Image(unsigned char *rgb, bool *mask, int width, int height);

bool Get_Session_Aux_Data_Filename(char *res, char *extension);


bool Layer_Has_Halo(int layer_id);
bool Set_Layer_Has_Halo(int layer_id, bool b);


///////////////////////////////////

bool auto_load_halos = false;

float default_halo_range = 20;

bool halo_tool_open = false;

bool render_halo_tool = false;

bool render_2d_halo = true;
bool render_3d_halo = true;
bool blend_halo_with_background = true;

bool render_halo_alphamask = true;

bool Get_Selected_Halo_Info();

////////////////////////////////////////////////////////////////////////////////////
//buffer to optimize halo generation
//potential halo pixels are flagged to speed up generation
bool *halo_outline_hit_framebuffer = 0;

//init the hit framebuffer
bool Init_Halo_Outline_Hit_Frame_Buffer()
{
	if(frame)
	{
		if(halo_outline_hit_framebuffer)delete[] halo_outline_hit_framebuffer;
		halo_outline_hit_framebuffer = new bool[frame->width*frame->height];
		memset(halo_outline_hit_framebuffer,0, sizeof(bool)*frame->width*frame->height);
		return true;
	}
	return false;
}

//free the hit framebuffer
bool Free_Halo_Outline_Hit_Frame_Buffer()
{
	if(halo_outline_hit_framebuffer)delete[] halo_outline_hit_framebuffer;
	halo_outline_hit_framebuffer = 0;
	return true;
}

//flag hits for all pixels around px/py as potential halo pixels
__forceinline void Flag_Surrounding_Potential_Halo_Pixels(int px, int py, int range)
{
	int lx = px-(range+2);//give a little space
	int ly = py-(range+2);
	int hx = px+(range+3);//give a little space
	int hy = py+(range+3);
	if(lx<0)lx = 0;
	if(ly<0)ly = 0;
	if(hx>=frame->width)hx = frame->width;
	if(hy>=frame->height)hy = frame->height;
	int width = hx-lx;
	for(int j = ly;j<hy;j++)
	{
		memset(&halo_outline_hit_framebuffer[(j*frame->width)+lx], 1, sizeof(bool)*width);
	}
}

//flag all potential pixels in the hitbuffer around the "pixels" at the "range"
bool Flag_Potential_Halo_Outline_Hits(int num, int *pixels, int range)
{
	memset(halo_outline_hit_framebuffer,0, sizeof(bool)*frame->width*frame->height);
	for(int i = 0;i<num;i++)
	{
		Flag_Surrounding_Potential_Halo_Pixels(pixels[i*2], pixels[(i*2)+1], range);
	}
	return false;
}

//called when searching for halo pixels at runtime creation, should always be in valid range
__forceinline bool Is_Potential_Halo_Pixel(int x, int y)
{
	return halo_outline_hit_framebuffer[(y*frame->width)+x];
}

////////////////////////////////////////////////////////////////////////////////////

bool Open_Halo_Tool()
{
	if(halo_tool_open)
	{
		return Set_Halo_Tool_To_Selection();
	}
	halo_tool_open = true;
	render_halo_tool = true;
	Open_Halo_Dlg();
	List_Halo_Tool_Layers();
	Set_Halo_Tool_To_Selection();
	Init_Halo_Outline_Hit_Frame_Buffer();
	return true;
}

bool Close_Halo_Tool()
{
	if(!halo_tool_open)
	{
		return false;
	}
	halo_tool_open = false;
	render_halo_tool = false;
	redraw_edit_window = true;
	redraw_frame = true;
	Close_Halo_Dlg();
	Free_Halo_Outline_Hit_Frame_Buffer();
	return true;
}

class HALO_PIXEL
{
public:
	int x, y;
	int closest_x, closest_y;
	float distance, strength;
	float position[3];
	float foreground_rgba[4];
	float background_rgb[3];
	float original_alpha;
	float pixel_depth;
};

class HALO_INFO
{
public:
	HALO_INFO()
	{
		halo_mask_texture = -1;
		alpha_mask_texture = -1;
		tristrip_indices = 0;
		num_tristrip_indices = 0;
		halo_data_changed = false;
		layer_id = -1;
	}
	~HALO_INFO()
	{
		Free_Data();
	}
	bool Free_Data()
	{
		halo_pixels.clear();
		if(tristrip_indices)delete[] tristrip_indices;
		tristrip_indices = 0;
		num_tristrip_indices = 0;
		halo_data_changed = false;
		return true;
	}
	int lx, ly, hx, hy;
	int layer_id;
	float range;
	int halo_texture_lx, halo_texture_ly, halo_texture_hx, halo_texture_hy;
	vector<HALO_PIXEL> halo_pixels;
	int halo_mask_texture;
	int alpha_mask_texture;
	unsigned int *tristrip_indices;
	int num_tristrip_indices;
	bool halo_data_changed;
	float average_visible_depth;
};

HALO_INFO *selected_halo = 0;

//load pre-existing halo data for this layer for the current frame

bool Load_Halo_Preview_Data(int level_id);

//returns the halo in memory OR inits a new one
HALO_INFO* Get_Layer_Halo_Info(int layer_id);

bool Get_Selected_Halo_Info()
{
	//make sure we're not already viewing/editing the halo data for the currently selected layer

	int new_index = 0;
	int new_layer_id = Get_Most_Recently_Selected_Layer(&new_index);
	if(selected_halo)
	{
		if(selected_halo->layer_id==new_layer_id)
		{
			//we already have this one loaded, nothing to update
			return true;
		}
	}
	if(new_index==-1||new_layer_id==-1)
	{
		return false;
	}

	selected_halo = Get_Layer_Halo_Info(new_layer_id);

	return true;
}

bool Get_Selected_Halo_Info(int layer_index)
{
	//make sure we're not already viewing/editing the halo data for the currently selected layer

	int new_layer_id = Get_Layer_ID(layer_index);

	if(selected_halo)
	{
		if(selected_halo->layer_id==new_layer_id)
		{
			//we already have this one loaded, nothing to update
			return true;
		}
	}

	selected_halo = Get_Layer_Halo_Info(new_layer_id);

	return true;
}

//used to set gui to selected halo
int Get_Active_Halo_Layer()
{
	if(selected_halo)
	{
		return selected_halo->layer_id;
	}
	return -1;
}

bool Free_Halo_Textures(HALO_INFO *hi)
{
	if(hi->halo_mask_texture!=-1)
	{
		Set_Edit_Context();
		Free_GL_Texture(hi->halo_mask_texture);
		Free_GL_Texture(hi->alpha_mask_texture);
		Set_GLContext();
		hi->halo_mask_texture = -1;
		hi->alpha_mask_texture = -1;
		return true;
	}
	return false;
}

//reset all 3d vertices
bool Update_Halo_Geometry(HALO_INFO *hi)
{
	int layer_index = Find_Layer_Index(hi->layer_id);
	int n = hi->halo_pixels.size();
	for(int i = 0;i<n;i++)
	{
		Project_Pixel_To_Layer(layer_index, hi->halo_pixels[i].x, hi->halo_pixels[i].y, hi->halo_pixels[i].position);
		hi->halo_pixels[i].pixel_depth = MATH_UTILS::Distance3D(hi->halo_pixels[i].position, frame->view_origin);
	}
	return true;
}

//generates a rectangle image buffer containing rgba pixel data for the halo vertices
float* Create_Halo_RGBA_Image_Buffer(HALO_INFO *hi)
{
	int width = hi->halo_texture_hx-hi->halo_texture_lx;
	int height = hi->halo_texture_hy-hi->halo_texture_ly;
	float *rgba = new float[width*height*4];
	memset(rgba, 0, sizeof(float)*width*height*4);//sets rgb+alpha to zero
	//now add colors and alphas from the halo pixels
	int n = hi->halo_pixels.size();
	int ix, iy, k;//indices into buffer
	for(int i = 0;i<n;i++)
	{
		ix = hi->halo_pixels[i].x-hi->halo_texture_lx;
		iy = hi->halo_pixels[i].y-hi->halo_texture_ly;
		k = (iy*width)+ix;//and the linear index for the rectangle rgba buffer
		rgba[k*4] = hi->halo_pixels[i].foreground_rgba[0];
		rgba[(k*4)+1] = hi->halo_pixels[i].foreground_rgba[1];
		rgba[(k*4)+2] = hi->halo_pixels[i].foreground_rgba[2];
		rgba[(k*4)+3] = hi->halo_pixels[i].foreground_rgba[3];
	}
	return rgba;
}

//get vertex buffers to render triangle strips in space
///////////////rgba_image only covers the halo region
bool Create_Halo_TriStrips(HALO_INFO *hi)
{
	float *rgba_image = Create_Halo_RGBA_Image_Buffer(hi);
	//delete all old data
	
	if(hi->tristrip_indices)delete[] hi->tristrip_indices;

	//first we need a general grid of points to isolate the used ones
	int width = hi->halo_texture_hx-hi->halo_texture_lx;
	int height = hi->halo_texture_hy-hi->halo_texture_ly;

	
	bool *hits = new bool[width*height];
	memset(hits, 0, sizeof(bool)*width*height);

	//create a buffer of indices so each used xy spot in the grid is tagged according
	//to where it appears in the list of halo pixels, this puts a true index into
	//the 2d grid
	
	int *indices = new int[width*height];
	int n = hi->halo_pixels.size();
	
	int i, j, k;
	n = hi->halo_pixels.size();
	int ix, iy;

//set invalid indices
	for(i = 0;i<width*height;i++)
	{
		indices[i] = -42;
	}

	for(i = 0;i<n;i++)
	{
		//get local indices into the buffer from frame coordinates
		ix = hi->halo_pixels[i].x-hi->halo_texture_lx;
		iy = hi->halo_pixels[i].y-hi->halo_texture_ly;
		k = (iy*width)+ix;//and the linear index for the hit buffer
		//fill in the hit buffer at every halo pixel
		hits[k] = true;
		//fill in the index buffer at every halo pixel
		indices[k] = i;
	}

	//get some rle strips in local grid coordinates to find tri strips
	RLE_STRIP rs;
	vector<RLE_STRIP> rle_strips;

	//now check all horizontal strips for upper-and-lower hits to make a tri strip
	bool instrip = false;
	bool top_hit = false;
	bool bottom_hit = false;
	
	for(j = 0;j<height-1;j++)
	{
		instrip = false;
		for(i = 0;i<width;i++)
		{
			top_hit = hits[(j*width)+i];
			bottom_hit = hits[((j+1)*width)+i];
			if(top_hit&&bottom_hit)//we found a halo pixel at i,j (xy)
			{
				if(!instrip)//not already in  a strip
				{
					//start a new strip
					instrip = true;
					rs.start_x = i;
					rs.y = j;
				}
			}
			else//the pixel is not a halo pixel
			{
				if(instrip)//we were in the middle of a strip
				{
					//finish this strip
					instrip = false;
					rs.end_x = i;
					rle_strips.push_back(rs);
				}
			}
		}
		if(instrip)//we were in the middle of a strip
		{
			//finish this strip
			instrip = false;
			rs.end_x = i;
			rle_strips.push_back(rs);
		}
	}


	hi->num_tristrip_indices = 0;//how many indices required to render all tri strips
	//each rle strip adds a single tri strip

	n = rle_strips.size();
	for(i = 0;i<n;i++)
	{
		for(j = rle_strips[i].start_x;j<rle_strips[i].end_x;j++)
		{
			hi->num_tristrip_indices+=2;//top and bottom indices
		}
		//add a few more at the end
		if(i<n-1)
		{
			hi->num_tristrip_indices+=2;//2 to finish it off
			hi->num_tristrip_indices+=2;//2 to start the next one
		}
	}

	//allocate space for all indices
	hi->tristrip_indices = new unsigned int[hi->num_tristrip_indices];

	//and fill them in
	int cnt = 0;

	int index = 0;

	for(i = 0;i<n;i++)
	{
		for(j = rle_strips[i].start_x;j<rle_strips[i].end_x;j++)
		{
			//get the upper 2d index into the indices
			k = (rle_strips[i].y*width)+j;
			index = indices[k];//get the halo pixel index
			hi->tristrip_indices[cnt] = index;
			cnt++;
			//do the same for the lower index
			k = ((rle_strips[i].y+1)*width)+j;
			index = indices[k];//get the halo pixel index
			hi->tristrip_indices[cnt] = index;
			cnt++;
		}
		//add a few more at the end
		if(i<n-1)
		{
			//repeat the last 2 indices to finish it off
			k = ((rle_strips[i].y+1)*width)+(rle_strips[i].end_x-1);
			index = indices[k];//get the halo pixel index
			hi->tristrip_indices[cnt] = index;cnt++;
			hi->tristrip_indices[cnt] = index;cnt++;

			//add the first 2 indices of the next strip
			k = ((rle_strips[i+1].y)*width)+(rle_strips[i+1].start_x);
			index = indices[k];//get the halo pixel index
			hi->tristrip_indices[cnt] = index;cnt++;
			hi->tristrip_indices[cnt] = index;cnt++;
		}
	}

	//now the halo pixels will have a corresponding vertex in the buffers
	n = hi->halo_pixels.size();

	//and the tristrip indices will connect them in 3d space so we don't
	rle_strips.clear();//need the rle strips anymore


	delete[] hits;
	delete[] indices;
	delete[] rgba_image;
	return true;
}

//subtract color from background pixel, used to create "halo" pixels that are rendered over the background
__forceinline bool Subtract_Pixel_Color(float *original, float *subtract_color, float *result, float *remainder, float *alpha, float max_alpha)
{
	if(max_alpha==0)
	{
		result[0] = original[0];
		result[1] = original[1];
		result[2] = original[2];
		remainder[0] = remainder[1] = remainder[2] = 0;
		*alpha = 0;
		return true;
	}
	//original is the original frame pixel RGB
	//subtract_color is the color we want to remove from the original
	//result is the original pixel with the color subtracted (this will be rendered opaque in the background)
	//remainder is the amount of color subtracted from of the original (this will be rendered with alpha in the halo)
	//alpha is the final amount of alpha needed for the "remainder" pixel to blend with the background "result"
	//intermediate_error is a QA check to test re-combining before scaling
	//max alpha is the max percentage of subtractable color value we can apply for this particular pixel

	//colors will be re-combined together during rendering like this:
	//final = (background * (1.0f-alpha)) + (foreground*alpha);

	//the blended color should match the original!

	//so if alpha was at 50% the background and foreground would be equal amounts from the target (above or below)
	//this would cause both values to converge at the mid point, or the color of the original pixel

	//first get direction and magnitude for the value change for each channel from original values
	//the color we are trying to subtract

	float rdiff = subtract_color[0]-original[0];
	float gdiff = subtract_color[1]-original[1];
	float bdiff = subtract_color[2]-original[2];

	//the lower these difference values are, the closer the match to the subtractable color, and more
	//that can be subtracted..
	//this is however limited by the "max_alpha", which the amount of blending that can possibly
	//be performed at the current area of the halo

	//figure out the maximum color difference between the original and the color we are trying to
	//subtract, the maximum difference is 300% (3.0);

	float color_similarity = 1.0f-((fabs(rdiff)+fabs(gdiff)+fabs(bdiff))/3);//average difference inverted

	//the higher the color similarity, the more we can extract from the original pixel
	//100% similarity (all colors match)
	//0% similarity (all colors inverted to extremes)
	//50% similarity (colors may be inverted or have different values)

	//the color similarity is scaled by the max_alpha to get the total amount of separation
	//needed to separate and re-blend the background and foreground pixels

	float final_alpha = color_similarity * max_alpha;

	//Getting Final Colors For The Background And Foreground Pixels

	//IF the subtract_color value is higher than the target midpoint(original value), then the background
	//will have to be set to a lower value so they matchup correctly, and the foreground pixel set to higher

	//IF the subtract value is lower, then the background pixel have to be a higher value so the foreground
	//pixels will bring it back down to the target original mid-point when rendered

	//At This Point we know that the foreground and background pixels must be dropped or raised to a certain
	//extent to re-combine at the alpha percentage to generate the values from the original pixel color

	//Now we have to determine how far away from the original color we need to go to create background
	//and foreground pixels that re-combine at the specified alpha to generate the original color

	//get the value changes for the foreground color
	float foreground_diff[3];
	foreground_diff[0] = rdiff*(1.0f-final_alpha);
	foreground_diff[1] = gdiff*(1.0f-final_alpha);
	foreground_diff[2] = bdiff*(1.0f-final_alpha);

	//the background pixels will use the same changes but negative and scaled by the inverse amount

	float background_diff[3];
	background_diff[0] = -rdiff*(final_alpha);
	background_diff[1] = -gdiff*(final_alpha);
	background_diff[2] = -bdiff*(final_alpha);

	//now if we added those values to the original pixel we would get projected foreground and
	//background pixels that match at the specified alpha to re-create the original pixel color

	//BUT these projected values could exceed 0.0 - 1.0, so they need to be clamped to the available
	//pixel range used in computer graphics..
	
	//Clamping the values cannot be performed independently on either side, any change in value
	//for foreground or background must also include a proportional change in value to the other

	//Now test the projected values to see if they exceed the range of 0.0 to 1.0

	float max_value;//temp variable
	float scale_percent;//temp variable

	int i;

	for(i = 0;i<3;i++)
	{
		if(original[i]+background_diff[i]>1.0)
		{
			//ok, the background_diff is too large, it must be scaled back
			//first determine the maximum value possible for background_diff
			max_value = 1.0f-original[i];//background_diff cannot be larger than this
			//find the percentage needed to scale background_diff to match the maximum
			scale_percent = max_value/background_diff[i];//will always be less than 1.0
			//scale this value, and also the corresponding value for the opposite side
			background_diff[i] *= scale_percent;//now that the background value can't change as much
			foreground_diff[i] *= scale_percent;//we don't need the foreground value to change as much
		}

		//the previous test works if the background_rdiff is positive, do a similar check for negative
		if(original[i]+background_diff[i]<0)
		{
			max_value = original[i];//the original value is the most that can be subtracted
			//find the percentage needed to scale background_rdiff to match the maximum
			scale_percent = max_value/(-background_diff[i]);//will always be less than 1.0
			//scale this value, and also the corresponding value for the opposite side
			background_diff[i] *= scale_percent;//now that the background value can't change as much
			foreground_diff[i] *= scale_percent;//we don't need the foreground value to change as much
		}

		if(original[i]+foreground_diff[i]>1.0)
		{
			//ok, the foreground_diff is too large, it must be scaled back
			//first determine the maximum value possible for foreground_diff
			max_value = 1.0f-original[i];//foreground_diff cannot be larger than this
			//find the percentage needed to scale foreground_diff to match the maximum
			scale_percent = max_value/foreground_diff[i];//will always be less than 1.0
			//scale this value, and also the corresponding value for the opposite side
			foreground_diff[i] *= scale_percent;//now that the foreground value can't change as much
			background_diff[i] *= scale_percent;//we don't need the background value to change as much
		}

		//the previous test works if the foreground_rdiff is positive, do a similar check for negative
		if(original[i]+foreground_diff[i]<0)
		{
			max_value = original[i];//the original value is the most that can be subtracted
			//find the percentage needed to scale foreground_rdiff to match the maximum
			scale_percent = max_value/(-foreground_diff[i]);//will always be less than 1.0
			//scale this value, and also the corresponding value for the opposite side
			foreground_diff[i] *= scale_percent;//now that the foreground value can't change as much
			background_diff[i] *= scale_percent;//we don't need the background value to change as much
		}
	}

	//NOW the background and foreground pixels can be determined from the projected difference values
	//that have been scaled to the alpha percentage (determined by color similarity and max_alpha)
	//and clamped to the range of accetpable pixel values (0.0 to 1.0) on either side

	//get the new background result
	result[0] = original[0]+background_diff[0];
	result[1] = original[1]+background_diff[1];
	result[2] = original[2]+background_diff[2];

	//get the foreground color that was extracted from the background
	remainder[0] = original[0]+foreground_diff[0];
	remainder[1] = original[1]+foreground_diff[1];
	remainder[2] = original[2]+foreground_diff[2];

	//do a quick range clamp for any stray values caused by floating point imprecision
	for(i = 0;i<3;i++)
	{
		if(result[i]<0)result[i] = 0;if(result[i]>1)result[i] = 1;
		if(remainder[i]<0)remainder[i] = 0;if(remainder[i]>1)remainder[i] = 1;
	}

	*alpha = final_alpha;
	return true;
}


bool Render_Halo_2D_AABB(HALO_INFO *hi)
{
	if(hi->halo_pixels.size()==0&&hi!=selected_halo)
	{
		//it's not currently selected and there is no halo created
		return false;
	}
	if(hi==selected_halo)
	{
		//use bright color
		glColor3f(1,1,0.5f);
	}
	else
	{
		//use flat color
		glColor3f(0.7f,0.7f,0);
	}
	//render the AABB
	float p1[] = {hi->lx-hi->range, hi->ly-hi->range};
	float p2[] = {hi->hx+hi->range, hi->ly-hi->range};
	float p3[] = {hi->hx+hi->range, hi->hy+hi->range};
	float p4[] = {hi->lx-hi->range, hi->hy+hi->range};
	glBegin(GL_LINES);
	glVertex2fv(p1);glVertex2fv(p2);
	glVertex2fv(p2);glVertex2fv(p3);
	glVertex2fv(p3);glVertex2fv(p4);
	glVertex2fv(p4);glVertex2fv(p1);
	glEnd();
	return true;
}

bool Render_Halo_2D_Overlay(HALO_INFO *hi)
{
	int n = hi->halo_pixels.size();
	if(hi->halo_mask_texture!=-1)//if we have textures
	{
		if(render_halo_alphamask)
		{
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, hi->alpha_mask_texture);
		}
		else
		{
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, hi->halo_mask_texture);
		}
		int tw = hi->halo_texture_hx-hi->halo_texture_lx;
		int th = hi->halo_texture_hy-hi->halo_texture_ly;
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);glVertex2f(hi->halo_texture_lx, hi->halo_texture_ly);
		glTexCoord2f(tw,0);glVertex2f(hi->halo_texture_hx, hi->halo_texture_ly);
		glTexCoord2f(tw,th);glVertex2f(hi->halo_texture_hx, hi->halo_texture_hy);
		glTexCoord2f(0,th);glVertex2f(hi->halo_texture_lx, hi->halo_texture_hy);
		glEnd();
	}
	return true;
}

bool Render_Halo_3D_Overlay(HALO_INFO *hi)
{
	int n = hi->num_tristrip_indices;
	if(n>0)
	{
		glBegin(GL_TRIANGLE_STRIP);
		int k;
		int nv = hi->halo_pixels.size();
		for(int i = 0;i<n;i++)
		{
			k = hi->tristrip_indices[i];
			HALO_PIXEL *hp = &hi->halo_pixels[k];
			glColor4fv(hp->foreground_rgba);
			glVertex3fv(hp->position);
		}
		glEnd();
	}
	return true;
}

bool Render_Halo_Tool_2D()
{
	if(render_halo_alphamask||render_2d_halo)
	{
		if(blend_halo_with_background)glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		Render_Stored_Halos_2D();
		if(blend_halo_with_background)glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_RECTANGLE_ARB);
		glLineWidth(Pixel_Size());
		Render_Stored_Halos_2D_AABB();
		glColor3f(1,1,1);
		glLineWidth(1);
	}
	return true;
}

bool Render_Halo_Tool_3D()
{
	if(render_3d_halo)
	{
		glEnable(GL_BLEND);
		Render_Stored_Halos_3D();//true);
		glColor4f(1,1,1,1);
		glDisable(GL_BLEND);
	}
	return true;
}

float Halo_Pixel_Range()
{
	if(selected_halo)
	{
		return selected_halo->range;
	}
	return 0;
}

bool Set_Halo_Pixel_Range(float v)
{
	if(selected_halo)
	{
		selected_halo->range = v;
		return true;
	}
	return false;
}

__forceinline float Find_Closest_Pixel_In_Buffer(int num_pixels, int *pixels, int x, int y, int *res)
{
	int best = -1;
	float closest = 999999;
	float d = 0;
	for(int i = 0;i<num_pixels;i++)
	{
		d = MATH_UTILS::Distance2D(x, y, pixels[i*2], pixels[(i*2)+1]);
		if(d<closest)
		{
			closest = d;
			best = i;
		}
	}
	*res = best;
	return closest;
}

//uses the pixel subtraction method to separate a region of frame pixels into 2 layers
//rgb_image is the original buffer of pixel colors
//rgba_colors specifies a target rgb color to be subtracted from rgb_image
//the alpha component of the rgba buffer specifies the maximum amount to subtract
//backbuffer is the new reqion of pixel data with the background RGB colors
//frontbuffer is the new reqion of pixel data with the foreground RGBA colors
bool Separate_Image_Buffers(int num_pixels, float *rgb_image, float *rgba_colors, float *backbuffer, float *frontbuffer)
{
	for(int i = 0;i<num_pixels;i++)
	{
		Subtract_Pixel_Color(&rgb_image[i*3], &rgba_colors[i*4], &backbuffer[i*3], &frontbuffer[i*4], &frontbuffer[(i*4)+3], rgba_colors[(i*4)+3]);
	}
	return true;
}

//uses the pixel subtraction method to separate a region of frame pixels into 2 layers
//lx/ly/hx/hy are the region bounds (lo to hi)
//rgbs_colors specifies a target rgb color to be subtracted from the frame
//the alpha component of the rgba buffer specifies the maximum amount to subtract
//backbuffer is the new reqion of pixel data with the background RGB colors
//frontbuffer is the new reqion of pixel data with the foreground RGBA colors
bool Subtract_Frame_Region(int lx, int ly, int hx, int hy, float *rgba_colors, float *backbuffer, float *frontbuffer)
{
	int width = hx-lx;
	int height = hy-ly;
	int i, j, k;
	float original[3];
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			frame->Get_Original_Pixel_RGB(lx+i, ly+j, original);
			k = (j*width)+i;
			Subtract_Pixel_Color(original, &rgba_colors[k*4], &backbuffer[k*3], &frontbuffer[k*4], &frontbuffer[(k*4)+3], rgba_colors[(k*4)+3]);
		}
	}
	return true;
}

//analyze frame pixels to generate colors for halo pixels
//this only happens when a fresh halo is created in the app, never during loading
bool Get_Halo_Pixel_Information(HALO_INFO *hi)
{
	int w = hi->halo_texture_hx-hi->halo_texture_lx;
	int h = hi->halo_texture_hy-hi->halo_texture_ly;

	float *rgba_colors = new float[w*h*4];
	memset(rgba_colors, 0, sizeof(float)*w*h*4);
	int n = hi->halo_pixels.size();

	int ix, iy;
	int i;
	for(i = 0;i<n;i++)
	{
		//get the pixel coordinates for this point in the overlay grid
		ix = hi->halo_pixels[i].x-hi->halo_texture_lx;
		iy = hi->halo_pixels[i].y-hi->halo_texture_ly;
		//get the color at the closest outline pixel we are trying to extract into the halo
		frame->Get_Original_Pixel_RGB(hi->halo_pixels[i].closest_x, hi->halo_pixels[i].closest_y, &rgba_colors[((iy*w)+ix)*4]);
		rgba_colors[(((iy*w)+ix)*4)+3] = hi->halo_pixels[i].strength;
	}

	float *backbuffer = new float[w*h*3];
	float *frontbuffer = new float[w*h*4];
	Subtract_Frame_Region(hi->halo_texture_lx, hi->halo_texture_ly, hi->halo_texture_hx, hi->halo_texture_hy, rgba_colors, backbuffer, frontbuffer);
	delete[] rgba_colors;

	n = hi->halo_pixels.size();
	int k;
	for(i = 0;i<n;i++)
	{
		//get the pixel coordinates for this point in the overlay grid
		ix = hi->halo_pixels[i].x-hi->halo_texture_lx;
		iy = hi->halo_pixels[i].y-hi->halo_texture_ly;
		k = (((iy*w)+ix)*3);
		hi->halo_pixels[i].background_rgb[0] = backbuffer[k];
		hi->halo_pixels[i].background_rgb[1] = backbuffer[k+1];
		hi->halo_pixels[i].background_rgb[2] = backbuffer[k+2];
		k = (((iy*w)+ix)*4);
		hi->halo_pixels[i].foreground_rgba[0] = frontbuffer[k];
		hi->halo_pixels[i].foreground_rgba[1] = frontbuffer[k+1];
		hi->halo_pixels[i].foreground_rgba[2] = frontbuffer[k+2];
		hi->halo_pixels[i].foreground_rgba[3] = frontbuffer[k+3];
		hi->halo_pixels[i].original_alpha = frontbuffer[k+3];
	}

	delete[] backbuffer;
	delete[] frontbuffer;
	return true;
}

bool Create_Halo_Preview_Textures(HALO_INFO *hi)
{
	Free_Halo_Textures(hi);
	int w = hi->halo_texture_hx-hi->halo_texture_lx;
	int h = hi->halo_texture_hy-hi->halo_texture_ly;
	
	float *rgba = Create_Halo_RGBA_Image_Buffer(hi);
	
	Set_Edit_Context();
	hi->halo_mask_texture = Create_32Bit_Frame_Texture(w, h, rgba);
	//make alphamask
	int n = w*h;
	for(int i = 0;i<n;i++)
	{
		//darker for less alpha
		rgba[i*4] = rgba[(i*4)+1] = rgba[(i*4)+2] = rgba[(i*4)+3];
	}
	hi->alpha_mask_texture = Create_32Bit_Frame_Texture(w, h, rgba);
	Set_GLContext();	

	delete[] rgba;
	return true;
}


//generates a set of halo pixels from the settings, only happens
//when generating a halo at runtime in the app, never while loading
bool Get_Halo_Pixels(HALO_INFO *hi)
{
	//clear out the old data
	hi->halo_pixels.clear();
	//get copies of all outline pixels for the active layer
	int index = Get_Layer_Index(hi->layer_id);
	if(index==-1)return false;
	int nop = 0;
	if(!Get_Num_Layer_Outline_Pixels(index, &nop))return false;
	if(nop==0)return false;
	int i, j;
	int *pixels = new int[nop*2];
	int num_border_pixels = 0;
	int opx, opy;
	for(i = 0;i<nop;i++)
	{
		Get_Layer_Outline_Pixel(index, i, &opx, &opy);
		//ignore anything directly on the frame border, halo emanations do not occur outside of the image
		if(opx==0||opx==frame->width-1||opy==0||opy==frame->height)
		{
			num_border_pixels++;

		}
		else
		{
			pixels[(i-num_border_pixels)*2] = opx;
			pixels[((i-num_border_pixels)*2)+1] = opy;
		}
	}
	//clean up as we go, buffer may be oversized (*num_border_pixels) but we only need the valid ones
	nop -= num_border_pixels;

	//flag all the potential halo pixels around the outline edges
	Flag_Potential_Halo_Outline_Hits(nop, pixels, hi->range);

	//now check all possible pixels within the range against the outline pixels to find the closest one

	//get the clamped range of search in the AABB
	hi->halo_texture_lx = hi->lx-hi->range;
	hi->halo_texture_ly = hi->ly-hi->range;
	hi->halo_texture_hx = (hi->hx+hi->range)+1;
	hi->halo_texture_hy = (hi->hy+hi->range)+1;

	//indices "top out" at high end so x==width and y==height are ok

	if(hi->halo_texture_lx<0)hi->halo_texture_lx = 0;
	if(hi->halo_texture_hx>frame->width)hi->halo_texture_hx = frame->width;
	if(hi->halo_texture_ly<0)hi->halo_texture_ly = 0;
	if(hi->halo_texture_hy>frame->height)hi->halo_texture_hy = frame->height;

	//now test all pixels in the AABB that are NOT within the layer against all outline pixels
	int pi = 0;//index to the closest pixel
	float td;//distance to the closest pixel
	for(i = hi->halo_texture_lx;i<hi->halo_texture_hx;i++)
	{
		for(j = hi->halo_texture_ly;j<hi->halo_texture_hy;j++)
		{
			if(Is_Potential_Halo_Pixel(i, j))//check the hit buffer
			{
				if(frame->Get_Pixel_Layer(i, j)!=hi->layer_id)
				{
					//test i,j as a potential halo pixel against all outline pixels
					td = Find_Closest_Pixel_In_Buffer(nop, pixels, i, j, &pi);
					if(td<=hi->range)
					{
						HALO_PIXEL hp;
						hp.closest_x = pixels[pi*2];
						hp.closest_y = pixels[(pi*2)+1];
						hp.x = i;
						hp.y = j;
						hp.distance = td;
						hp.strength = 1.0f-(td/hi->range);
						//use sin falloff
						hp.strength = hp.strength*(sin(hp.strength));
						hi->halo_pixels.push_back(hp);
					}
				}
			}
		}
	}
	//add the outline pixels to the halo, this is needed for edge connectivity
	for(i = 0;i<nop;i++)
	{
		Get_Layer_Outline_Pixel(index, i, &pixels[i*2], &pixels[(i*2)+1]);
		HALO_PIXEL hp;
		hp.closest_x = pixels[i*2];
		hp.closest_y = pixels[(i*2)+1];
		hp.x = hp.closest_x;
		hp.y = hp.closest_y;
		hp.distance = 0;
		hp.strength = 1.0f;
		hi->halo_pixels.push_back(hp);
		//add the very next one and the 2 previous ones to connect halo strips to the layer during rendering
		//add only if these pixels are "inside the layer"
		
		//next pixel to the right, inside layer?
		if(hp.x+1<hi->halo_texture_hx)
		{
			if(frame->Get_Pixel_Layer(hp.x+1, hp.y)==hi->layer_id)
			{
				hp.closest_x++;
				hp.x++;
				hi->halo_pixels.push_back(hp);
				hp.closest_x--;//set it back
				hp.x--;
			}
		}

		//previous pixels inside layer?
		if(hp.x-1>=hi->halo_texture_lx)
		{
			if(frame->Get_Pixel_Layer(hp.x-1, hp.y)==hi->layer_id)
			{
				hp.closest_x--;
				hp.x--;
				hi->halo_pixels.push_back(hp);
				//try for one more
				if(hp.x-1>=hi->halo_texture_lx)
				{
					if(frame->Get_Pixel_Layer(hp.x-1, hp.y)==hi->layer_id)
					{
						hp.closest_x--;
						hp.x--;
						hi->halo_pixels.push_back(hp);
					}
				}
			}
		}
	}
	delete[] pixels;
	return true;
}

//only happens at runtime creation,just gets halo pixel data
bool Create_Halo_Pixels(HALO_INFO *hi)
{
	//find all the pixels in the halo
	Get_Halo_Pixels(hi);
	//find all the colors for the pixels (including alphas)
	Get_Halo_Pixel_Information(hi);
	return true;
}

//create halo data from pixel data, happen after initial creation
//and/or loading
bool Create_Halo_Data(HALO_INFO *hi)
{
	//generate halo and alpha preview textures
	Create_Halo_Preview_Textures(hi);
	//get index buffers for traingle strips for 3d rendering
	Create_Halo_TriStrips(hi);
	//update all 3d vertices to match the current model
	Update_Halo_Geometry(hi);
	return true;
}

//create a new halo and init all data
bool Generate_Halo_Mask(HALO_INFO *hi)
{
	Create_Halo_Pixels(hi);
	Create_Halo_Data(hi);
	hi->halo_data_changed = true;
	Set_Layer_Has_Halo(hi->layer_id, true);
	return true;
}

bool Generate_Halo_Mask()
{
	if(selected_halo)
	{
		Enable_Generate_Halo_Button(false);
		bool res = Generate_Halo_Mask(selected_halo);//&preview_halo);
		Enable_Generate_Halo_Button(true);
		return res;
	}
	return false;
}

//layer was changed from GUI
//save currently edited layer OR just move to the next one
bool Change_Active_Halo_Layer(int layer_index)
{
	//for now just dump the current layer halo and restart with a fresh new one
	if(Get_Selected_Halo_Info(layer_index))
	{
		return true;
	}
	return false;
}

//set the halo tool to the most recently selected layer
bool Set_Halo_Tool_To_Selection()
{
	if(!halo_tool_open)
	{
		//not even editing halos, screw it
		return false;
	}
	if(Get_Selected_Halo_Info())
	{
		Update_Halo_Range_Slider();
		Select_Halo_Tool_Layer(Get_Active_Halo_Layer());
	}
	redraw_edit_window = true;
	return false;
}


bool Delete_Halo_Mask(HALO_INFO *hi)
{
	hi->Free_Data();
	Free_Halo_Textures(hi);
	return true;
}


bool Delete_Halo_Mask()
{
	if(selected_halo)
	{
		return Delete_Halo_Mask(selected_halo);
	}
	return false;
}

////////////////////////////////////////////////////
//storage and management for halo data for all layers in the current frame

vector<HALO_INFO*> layer_halos;

bool Delete_All_Halo_Masks()
{
	int n = layer_halos.size();
	for(int i = 0;i<n;i++)
	{
		Set_Layer_Has_Halo(layer_halos[i]->layer_id, false);
		Delete_Halo_Mask(layer_halos[i]);
	}
	return true;
}

bool ReProject_Halo(int layer_id)
{
	int n = layer_halos.size();
	for(int i = 0;i<n;i++)
	{
		if(layer_halos[i]->layer_id)
		{
			Update_Halo_Geometry(layer_halos[i]);
			return true;
		}
	}
	return false;
}


bool Update_Halo_Geometry()
{
	if(selected_halo)
	{
		return Update_Halo_Geometry(selected_halo);
	}
	return false;
}

bool Update_All_Halo_Geometry()
{
	int n = layer_halos.size();
	for(int i = 0;i<n;i++)
	{
		Update_Halo_Geometry(layer_halos[i]);
	}
	return true;
}

bool Free_All_Layer_Halos()
{
	int n = layer_halos.size();
	for(int i = 0;i<n;i++)
	{
		Set_Layer_Has_Halo(layer_halos[i]->layer_id, false);
		Free_Halo_Textures(layer_halos[i]);
		delete layer_halos[i];
	}
	layer_halos.clear();
	return true;
}

HALO_INFO* Find_Saved_Halo_Data(int layer_id)
{
	int n = layer_halos.size();
	for(int i = 0;i<n;i++)
	{
		if(layer_halos[i]->layer_id==layer_id)
		{
			return layer_halos[i];
		}
	}
	return 0;
}

HALO_INFO* Get_Layer_Halo_Info(int layer_id)
{
	int index = Get_Layer_Index(layer_id);
	if(index==-1)
	{
		return 0;//no such layer exists in the current frame
	}
	HALO_INFO *res = Find_Saved_Halo_Data(layer_id);
	if(!res)
	{
		res = new HALO_INFO;
		res->layer_id = layer_id;
		layer_halos.push_back(res);
		Get_Layer_AABB(index, &res->lx, &res->ly, &res->hx, &res->hy);
		res->range = default_halo_range;
	}
	return res;
}


bool Render_Stored_Halos_2D()
{
	int n = layer_halos.size();
	for(int i = 0;i<n;i++)
	{
		Render_Halo_2D_Overlay(layer_halos[i]);
	}
	return true;
}

bool Render_Stored_Halos_2D_AABB()
{
	int n = layer_halos.size();
	for(int i = 0;i<n;i++)
	{
		Render_Halo_2D_AABB(layer_halos[i]);
	}
	return true;
}

bool Render_Stored_Halos_3D()
{
	int n = layer_halos.size();
	for(int i = 0;i<n;i++)
	{
		Render_Halo_3D_Overlay(layer_halos[i]);
	}
	return true;
}


bool Halos_Exist()
{
	int n = layer_halos.size();
	for(int i = 0;i<n;i++)
	{
		if(layer_halos[i]->halo_pixels.size()>0)
		{
			return true;
		}
	}
	return false;
}

//check for overlapping halos and set furthest alphas to zero so only
//the closest halo pixel background color is applied to the frame pixels

//this is basically per-pixel order independent depth sorting

bool Depth_Sort_Halo_Pixels()
{
	//make a frame buffer for pixel depths
	float *pixel_depths = new float[frame->width*frame->height];

	//set all initial depths to -1
	int total = frame->width*frame->height;
	int i, j, k;
	for(i = 0;i<total;i++)
	{
		pixel_depths[i] = -1;
	}
	//get average depth of all visible pixels for sorting later
	float average_visible_pixel_depth = 0;
	int num_visible_pixels = 0;
	//now check all halo pixels for where they hit in the frame
	int nh = layer_halos.size();
	for(i = 0;i<nh;i++)
	{
		average_visible_pixel_depth = 0;
		num_visible_pixels = 0;
		int np = layer_halos[i]->halo_pixels.size();
		for(j = 0;j<np;j++)
		{
			int px = layer_halos[i]->halo_pixels[j].x;
			int py = layer_halos[i]->halo_pixels[j].y;
			if(px>=0&&px<frame->width&&py>=0&&py<frame->height)
			{
				//if it's within the frame
				layer_halos[i]->halo_pixels[j].pixel_depth = MATH_UTILS::Distance3D(layer_halos[i]->halo_pixels[j].position, frame->view_origin);
				k = (py*frame->width)+px;
				if(pixel_depths[k]==-1)
				{
					//this is the first one, set the depth
					pixel_depths[k] = layer_halos[i]->halo_pixels[j].pixel_depth;
				}
				else if(pixel_depths[k]>layer_halos[i]->halo_pixels[j].pixel_depth)
				{
					//this one is closer, overwrite the older value
					pixel_depths[k] = layer_halos[i]->halo_pixels[j].pixel_depth;
				}

			}
		}
	}
	//some pixels may not have been hit leaving single pixel holes, fill these in along horizontal
	for(j = 0;j<frame->height;j++)
	{
		for(i = 1;i<frame->width-1;i++)
		{
			k = (j*frame->width)+i;
			if(pixel_depths[k]==-1)//might be a hole
			{
				//check surrounding
				if(pixel_depths[k-1]!=-1&&pixel_depths[k+1]!=-1)
				{
					//we have a hole, set the depth to the average of the next and previous
					pixel_depths[k] = (pixel_depths[k-1]&&pixel_depths[k+1])*0.5f;
				}
			}
		}
	}

	//now see which ones actually took and enable only those
	for(i = 0;i<nh;i++)
	{
		int np = layer_halos[i]->halo_pixels.size();
		for(j = 0;j<np;j++)
		{
			k = (layer_halos[i]->halo_pixels[j].y*frame->width)+layer_halos[i]->halo_pixels[j].x;
			if(pixel_depths[k]==layer_halos[i]->halo_pixels[j].pixel_depth)
			{
				//make sure it's not obscured by any frame pixels (like an interior space cutout on a wall)
				if(layer_halos[i]->halo_pixels[j].pixel_depth<frame->Get_Pixel_Depth(layer_halos[i]->halo_pixels[j].x, layer_halos[i]->halo_pixels[j].y))
				{
					//this one stuck, enable the alpha
					layer_halos[i]->halo_pixels[j].foreground_rgba[3] = layer_halos[i]->halo_pixels[j].original_alpha;//*0.5f;//666f;//666f;
					//and modify the frame pixel
					average_visible_pixel_depth += layer_halos[i]->halo_pixels[j].pixel_depth;
					num_visible_pixels++;
				}
				else
				{
					layer_halos[i]->halo_pixels[j].foreground_rgba[3] = 0;
				}
			}
			else
			{
				//this one didn't make it, make it invisible
				layer_halos[i]->halo_pixels[j].foreground_rgba[3] = 0;
			}
		}
		layer_halos[i]->average_visible_depth = average_visible_pixel_depth/num_visible_pixels;
	}
	delete[] pixel_depths;
	return true;
}

bool Restore_Halo_Pixels()
{
	return false;
}


void Render_Sorted_Halos_3D()
{
	//use low-efficiency simple sort for rendering front to back
	int n = layer_halos.size();
	bool *hits = new bool[n];
	memset(hits, 0, n);
	bool done = false;
	int cnt = 0;
	while(!done)
	{
		int best = -1;
		float lowest = 9999999;
		for(int i = 0;i<n;i++)
		{
			if(!hits[i])
			{
				if(best==-1)
				{
					best = i;
					lowest = layer_halos[i]->average_visible_depth;
				}
				else if(lowest>layer_halos[i]->average_visible_depth)
				{
					lowest = layer_halos[i]->average_visible_depth;
					best = i;
				}
			}
		}
		hits[best] = true;
		cnt++;
		if(cnt==n)
		{
			done = true;
		}
		Render_Halo_3D_Overlay(layer_halos[best]);
	}
}


bool Get_Halo_Pixel_Hits(bool *hits)
{
	Get_Matrices();
	int i, j, k;
	int nh = layer_halos.size();
	for(i = 0;i<nh;i++)
	{
		int np = layer_halos[i]->halo_pixels.size();
		for(j = 0;j<np;j++)
		{
			if(layer_halos[i]->halo_pixels[j].foreground_rgba[3]>0)
			{
				double wx, wy, wz;
				int px, py;
				gluProject(layer_halos[i]->halo_pixels[j].position[0], layer_halos[i]->halo_pixels[j].position[1], layer_halos[i]->halo_pixels[j].position[2], modelview_matrix, projection_matrix, viewport, &wx, &wy, &wz);
				px = (int)wx;
				py = (int)wy;
				if(px>=0&&px<frame->width&&py>=0&&py<frame->height)
				{
					//if it's within the frame
					k = (py*frame->width)+px;
					hits[k] = false;
				}
			}
		}
	}
	return true;
}

bool PostRender_Halos(unsigned char *rgb)
{
	Get_Matrices();
	Depth_Sort_Halo_Pixels();
	glColor4f(1,1,1,1);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	Render_Sorted_Halos_3D();
	glColor4f(1,1,1,1);
	
	glReadPixels(0,0,frame->width,frame->height,GL_RGB,GL_UNSIGNED_BYTE,rgb);
	bool *hits = new bool[frame->width*frame->height];
	memset(hits, 1, sizeof(bool)*frame->width*frame->height);

	Get_Halo_Pixel_Hits(hits);
	//anti-alias all the image pixels affected by halos
	Anti_Alias_Image(rgb, hits, frame->width, frame->height);

	delete[] hits;
	return true;
}


extern bool show_2d_view;
bool Render_Edit_Window();
void Clear_Screen();
void Update_Screen();

bool Generate_Default_Halos()
{
	float range = default_halo_range;
	int last_selected_id = -1;
	if(selected_halo)
	{
		range = selected_halo->range;
		last_selected_id = selected_halo->layer_id;
	}
	Free_All_Layer_Halos();
	int n = Num_Layers();
	char lname[512];
	for(int i = 0;i<n;i++)
	{
		HALO_INFO *hi = new HALO_INFO;
		hi->layer_id = Get_Layer_ID(i);
		hi->range = range;
		if(Get_Layer_AABB(i, &hi->lx, &hi->ly, &hi->hx, &hi->hy))
		{
			Layer_Name(i, lname);
			layer_halos.push_back(hi);
			Print_Status("Generating halo for layer \"%s\"", lname);
			Generate_Halo_Mask(hi);
		}
		else
		{
			delete hi;
		}
	}
	redraw_edit_window = true;
	redraw_frame = true;
	selected_halo = Get_Layer_Halo_Info(last_selected_id);
	Print_Status("Done", lname);
	SkinMsgBox("Done.");
	return true;
}

///////////////////////////////////////////////////
//loading/saving halo data

int HALO_FILE_VERSION = 43;

struct HALO_FILE_HEADER
{
	int version;//format version
	int num_halos;//total num halos
	int total_halo_pixels;//total halo pixels for ALL halos
};

//after header have array of these for each halo

struct HALO_FILE_INFO
{
	int layer_id;
	
	int halo_pixel_start_index;
	int num_halo_pixels;
	
	int lx, ly, hx, hy;
	float range;
	int halo_texture_lx, halo_texture_ly, halo_texture_hx, halo_texture_hy;
};

/*
//then one large array of all halo pixels for all halos

//missing data commented out for clarity
struct HALO_PIXEL_FILE_INFO
{
	int x, y;
	int closest_x, closest_y;
	float distance, strength;
	//float position[3];//not needed, position will be re-projected
	float foreground_rgba[4];
	float background_rgb[3];
	//float original_alpha;//not needed, will be stored in foreground_rgba
	//float pixel_depth;//not needed, determined at runtime sorting
};
*/
//add or retrieve data from structures to prepare for loading/saving

__forceinline void Fill_Halo_File_Info(HALO_FILE_INFO *hfi, HALO_INFO *hi)
{
	hfi->halo_texture_hx = hi->halo_texture_hx;
	hfi->halo_texture_hy = hi->halo_texture_hy;
	hfi->halo_texture_lx = hi->halo_texture_lx;
	hfi->halo_texture_ly = hi->halo_texture_ly;
	hfi->hx = hi->hx;
	hfi->hy = hi->hy;
	hfi->layer_id = hi->layer_id;
	hfi->lx = hi->lx;
	hfi->ly = hi->ly;
	hfi->num_halo_pixels = hi->halo_pixels.size();
	hfi->range = hi->range;
}

__forceinline void Get_Halo_File_Info(HALO_FILE_INFO *hfi, HALO_INFO *hi)
{
	hi->halo_texture_hx = hfi->halo_texture_hx;
	hi->halo_texture_hy = hfi->halo_texture_hy;
	hi->halo_texture_lx = hfi->halo_texture_lx;
	hi->halo_texture_ly = hfi->halo_texture_ly;
	hi->hx = hfi->hx;
	hi->hy = hfi->hy;
	hi->layer_id = hfi->layer_id;
	hi->lx = hfi->lx;
	hi->ly = hfi->ly;
	hi->range = hfi->range;
}

bool Save_Halos(char *file)
{
	int n = layer_halos.size();
	//create the header info
	HALO_FILE_HEADER hfh;
	hfh.num_halos = n;
	hfh.version = HALO_FILE_VERSION;
	hfh.total_halo_pixels = 0;
	int i;//, j;
	for(i = 0;i<n;i++)
	{
		hfh.total_halo_pixels += layer_halos[i]->halo_pixels.size();
	}
	if(hfh.total_halo_pixels==0)
	{
		return false;//nothing to save
	}

	FILE *f = fopen(file, "wb");
	if(!f)
	{
		return false;
	}
	//create some buffers
	HALO_FILE_INFO *hfi = new HALO_FILE_INFO[hfh.num_halos];

	int pixel_start_index_cnt = 0;
	for(i = 0;i<n;i++)
	{
		Fill_Halo_File_Info(&hfi[i], layer_halos[i]);
		hfi[i].halo_pixel_start_index = pixel_start_index_cnt;
		hfi[i].num_halo_pixels = layer_halos[i]->halo_pixels.size();
	}

	fwrite(&hfh, 1, sizeof(HALO_FILE_HEADER), f);
	fwrite(hfi, 1, sizeof(HALO_FILE_INFO)*hfh.num_halos, f);

	fclose(f);
	delete[] hfi;
	return true;
}

bool Load_Halos(char *file)
{
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		return false;
	}
	HALO_FILE_HEADER hfh;
	fread(&hfh, 1, sizeof(HALO_FILE_HEADER), f);

	if(hfh.version!=HALO_FILE_VERSION)
	{
		SkinMsgBox("Can't load halo data from file! Version mis-match!", "This never happens.", MB_OK);
		fclose(f);
		return false;
	}

	//create some buffers
	HALO_FILE_INFO *hfi = new HALO_FILE_INFO[hfh.num_halos];

	fread(hfi, 1, sizeof(HALO_FILE_INFO)*hfh.num_halos, f);

	//create halos and fill with pixel data
	
	int pixel_start_index_cnt = 0;
	int i;//, j;
	for(i = 0;i<hfh.num_halos;i++)
	{
		HALO_INFO *hi = new HALO_INFO;
		layer_halos.push_back(hi);
		Get_Halo_File_Info(&hfi[i], layer_halos[i]);
	}
	
	fclose(f);
	delete[] hfi;
	//init all halo data
	//make sure we have a matching hit buffer for this frame size
	Init_Halo_Outline_Hit_Frame_Buffer();
	int index;
	char lname[512];
	for(i = 0;i<hfh.num_halos;i++)
	{
		index = Get_Layer_Index(layer_halos[i]->layer_id);
		//make sure we have a valid layer
		if(index!=-1)
		{
			Layer_Name(index, lname);
			Print_Status("Generating halo for layer \"%s\"", lname);
			Create_Halo_Pixels(layer_halos[i]);
			Create_Halo_Data(layer_halos[i]);
			Set_Layer_Has_Halo(layer_halos[i]->layer_id, false);
		}
	}
	if(!halo_tool_open)
	{
		//tool isn't open, dump it
		Free_Halo_Outline_Hit_Frame_Buffer();
	}
	Print_Status("Done.", lname);
	return true;
}


bool Get_Halos_Save_File(char *res)
{
	if(!Get_Session_Aux_Data_Filename(res, "hls"))
	{
		return false;
	}
	return true;
}


bool Load_Halos_For_Frame()
{
	Free_All_Layer_Halos();
	char file[512];
	if(Get_Halos_Save_File(file))
	{
		return Load_Halos(file);
	}
	return false;
}

bool Load_Halos_For_Current_Frame(void *p)
{
	Free_All_Layer_Halos();
	if(halo_tool_open)
	{
		List_Halo_Tool_Layers();
	}
	if(auto_load_halos)
	{
		return Load_Halos_For_Frame();
	}
	return false;
}


bool Save_Halos_For_Current_Frame(void *p)
{
	char file[512];
	if(Get_Halos_Save_File(file))
	{
		return Save_Halos(file);
	}
	return false;
}


//catch all possible events to auto load and save halo data for any and all frames
//maybe change this in the future to ask?
bool Register_Halo_LoadSave_Events()
{
	shared_data_interface.Register_Shared_Data_Update_Callback("FrameChanged", Load_Halos_For_Current_Frame);
	shared_data_interface.Register_Shared_Data_Update_Callback("LoadingSingleFrameSession", Load_Halos_For_Current_Frame);
	shared_data_interface.Register_Shared_Data_Update_Callback("SavingProject", Save_Halos_For_Current_Frame);
	shared_data_interface.Register_Shared_Data_Update_Callback("SavingSingleFrameSession", Save_Halos_For_Current_Frame);
	shared_data_interface.Register_Shared_Data_Update_Callback("FrameAboutToChange", Save_Halos_For_Current_Frame);
	return false;
}


//////////////////////////////////////////////////////
//basic cache settings area to save and restore halo settings from one frame to another

vector<HALO_FILE_INFO> stored_halo_data;


//save just the basic info for range so we can re-constitute the layer later in another frame
bool Save_Halo_Info()
{
	stored_halo_data.clear();
	int n = layer_halos.size();
	for(int i = 0;i<n;i++)
	{
		if(layer_halos[i]->halo_pixels.size()>0)
		{
			//this halo has legitimate data attached
			HALO_FILE_INFO hfi;
			Fill_Halo_File_Info(&hfi, layer_halos[i]);
			stored_halo_data.push_back(hfi);
		}
	}
	return true;
}

bool Apply_Saved_Halo_Info()
{
	int n = stored_halo_data.size();
	for(int i = 0;i<n;i++)
	{
		//make sure we have a valid layer
		int index = Get_Layer_Index(Get_Layer_Index(stored_halo_data[i].layer_id));
		if(index!=-1)
		{
			char lname[512];
			Layer_Name(index, lname);
			Print_Status("Generating halo for layer \"%s\"", lname);
			//find existing halo or create new one
			HALO_INFO *hi = Get_Layer_Halo_Info(stored_halo_data[i].layer_id);
			//make sure we have a current aabb
			Get_Layer_AABB(index, &hi->lx, &hi->ly, &hi->hx, &hi->hy);
			//and the scale we wanted
			hi->range = stored_halo_data[i].range;
			//now create the halo data for this frame
			Generate_Halo_Mask(hi);
		}
	}
	Print_Status("Done.");
	return true;
}

bool Clear_Saved_Halo_Info()
{
	stored_halo_data.clear();
	return false;
}

