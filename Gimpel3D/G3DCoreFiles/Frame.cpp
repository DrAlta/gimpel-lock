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
#include "Image.h"
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>
#include <math.h>
#include "GLWnd.h"
#include "GLText.h"
#include "Layers.h"
#include "ControlMesh.h"
#include "Console.h"
#include "Controls.h"
#include "Camera.h"
#include "Relief.h"
#include "PlanarProjection.h"
#include "StereoView.h"
#include "PBuffer.h"
#include "../GeometryTool.h"
#include "../PerspectiveTool.h"
#include "../AnchorPoints.h"
#include "../LinkPointsTool.h"
#include "../FeaturePoints.h"
#include "../Skin.h"
#include "../GeometryTool/GLBasic.h"

//HI-RES ISSUE STUFF

bool manually_rasterize_frames = false;
bool conserve_memory_during_output = false;

//flag to keep separate buffer for original identify vectors
//OR to just reuse the same buffer for both (if camera doesn't move it's no problem)
bool keep_original_identity_vectors = true;

//flags to conserve memory in tight situations

bool use_triangle_indices = true;

extern int random_seed;

bool Halos_Exist();
bool PostRender_Halos(unsigned char *rgb);

bool Save_Camera_Views(FILE *f);
bool Load_Camera_Views(FILE *f);

extern bool render_background_meshes;
bool Render_Background_Meshes();
void Set_Projection_Dlg_FOV(float fov);

bool Free_Background_Meshes();

bool Report_Render3D_Event();
bool Report_Loading_Single_Frame_Session();
bool Report_Saving_Single_Frame_Session();


bool Get_Left_Raycast_Map(float *res);
bool Get_Right_Raycast_Map(float *res);

bool Replace_RGB_Values(unsigned char *rgb);

extern bool render_halo_tool;
bool Render_Halo_Tool_3D();

bool Apply_All_TouchUps();

void Push_3D_View();
void Pop_3D_View();

bool Clear_Virtual_Camera_Info();
void Get_View_Center(float *pos, float *rot);

bool update_2d_view_texture = false;

extern bool render_preview_data;

bool Render_Preview_3D();


extern bool virtual_camera_tool_open;
bool Render_Virtual_Camera_Tool();

extern bool render_camera;
bool Render_Camera();
bool ReCalc_Focal_Plane_Preview();
void Reset_Default_Layer_Depth();
bool Update_Selected_Layers();

double frame_modelview_matrix[16];
double frame_projection_matrix[16];
int frame_viewport[8];

FRAME *frame = 0;
int NEW_FRAME_LAYER_DEPTH = 100;
float DEFAULT_FOV = 45;
bool auto_center_stereoview = true;
bool cull_backfaces = false;
bool session_altered = false;
int frame_texture = -1;
float scene_center[3] = {0,0,0};

extern float _fov,_aspect, _near_plane, _far_plane;
extern int edge_texture;
extern int edit_texture;
extern bool view_edges;
extern bool keys_pressed[256];
extern bool edit_keys_pressed[256];
extern bool redraw_edit_window;
extern int width_border;
extern int height_border;
extern int window_xpos;
extern int window_ypos;
extern HWND _hWnd;
extern bool show_3d_view;
extern bool anaglyph_mode;
extern bool render_puff_preview;
extern bool render_sorted_layers;
extern bool render_borders;
extern bool render_normals;
extern int puff_preview_spacing;
extern int speckle_skip;
extern float camera_pos[3];
extern float camera_rot[3];
extern float target_camera_pos[3];
extern float target_camera_rot[3];
extern float camera_front[3];
extern float camera_up[3];
extern float _near_plane, _far_plane;
extern bool run_layer_popup_menu;
extern int last_clicked_layer;

bool Update_Edge_Texture();
void Resample_Strips();
void ReSizeGLScene(int width, int height);
bool Register_Alternate_Image_Path(char *apath);
bool Try_Alternate_Image_Paths(char *file, char *res);
bool Set_Current_Tracking_Frame(unsigned char *data, int w, int h);
bool BrowseForFolder(char *path, char *title);
bool Browse(char *res, char *ext, bool save);
bool Exists(char *file);
bool Get_File_From_Full_Path(char *path, char *file);
bool AntiAlias_Edges(unsigned char *rgb, float *positions);
bool Fill_Left_Side(unsigned char *rgb, int w, int h);
bool Fill_Right_Side(unsigned char *rgb, int w, int h);
void Set_All_Layers_Depth_ID_Float(float f);
bool Center_Edit_View();
bool Set_Edit_Context();
bool Set_Edit_Frame_Size(int w, int h);
bool Save_Layers_Info(FILE *f);
void Render_Edge_Fade();
void Update_GUI_Selection_Info();
void Update_Contour_Dialog();
void Render_Normals();
void Project_All_Layers(float depth_id);
void Render_Puff_Preview();
void Update_Stereo_Sliders();
void Get_World_Pos(int x, int y, float depth, float *res);
void Render_Outlined_Layer_Outlines();
void Render_Speckled_Layer_Speckles();
bool Render_Frame(FRAME *f, bool frame_only);
bool Render_Grid();
bool Set_Grid_Offset_View();
void Render_3D_Layer_Outline(int layer_id);

bool ReCalc_Focal_Plane_Preview();


#include "bb_matrix.h"

oBB_MATRIX frame_transform;


bool Update_Frame_View_Matrices()
{
	float ow = Screenwidth();
	float oh = Screenheight();
	
	ReSizeGLScene(frame->width, frame->height);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	float pos[3] = {0,0,0};
	float rot[3] = {0,180,0};
	pos[0] = frame->view_origin[0];
	pos[1] = frame->view_origin[1];
	pos[2] = frame->view_origin[2];
	rot[0] = (frame->view_rotation[0]);//flip pitch because we're spinning around vertical axis
	rot[1] = (-frame->view_rotation[1])+180;
	rot[2] = 0;

	glRotatef(rot[0], 1, 0, 0);
	glRotatef(rot[1], 0, 1, 0);
	glTranslatef(-pos[0], -pos[1], -pos[2]);


    glGetDoublev(GL_PROJECTION_MATRIX, frame_projection_matrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, frame_modelview_matrix);
    glGetIntegerv(GL_VIEWPORT, frame_viewport);
	glPopMatrix();
	ReSizeGLScene(ow, oh);
	return true;
}


void Get_Frame_View_Vector(int axis, float *res)
{
	res[0] = frame_transform.m[axis][0];
	res[1] = frame_transform.m[axis][1];
	res[2] = frame_transform.m[axis][2];
}

bool Set_Frame_Transform(float *pos, float *rot)
{
	
	if(frame->identity_vectors==frame->frame_vectors)
	{
		//we are using the same buffer for both, re-get the identify vectors
		Get_Flat_Projection_Vectors();
	}

	float id[3] = {0,0,0};
	frame_transform.Set(frame_transform.m, id, rot);
	int total = frame->width*frame->height;
	float v[3];
	float *iv, *fv;
	for(int i = 0;i<total;i++)
	{
		iv = frame->Get_Identity_Vector(i);
		fv = frame->Get_Frame_Vector(i);
		v[0] = iv[0];
		v[1] = iv[1];
		v[2] = iv[2];
		frame_transform.Transform_Vertex(frame_transform.m, v);
		fv[0] = v[0];
		fv[1] = v[1];
		fv[2] = v[2];
	}
	frame->view_origin[0] = pos[0];
	frame->view_origin[1] = pos[1];
	frame->view_origin[2] = pos[2];
	frame->view_rotation[0] = rot[0];
	frame->view_rotation[1] = rot[1];
	frame->view_rotation[2] = rot[2];
	Get_Frame_View_Vector(2, frame->view_direction);
	ReCalc_Focal_Plane_Preview();
	Update_Frame_View_Matrices();
	return true;
}

bool Get_Frame_Transform(float *pos, float *rot)
{
	if(frame)
	{
		pos[0] = frame->view_origin[0];
		pos[1] = frame->view_origin[1];
		pos[2] = frame->view_origin[2];
		rot[0] = frame->view_rotation[0];
		rot[1] = frame->view_rotation[1];
		rot[2] = frame->view_rotation[2];
		return true;
	}
	return false;
}


bool ReApply_Frame_Transform()
{
	return Set_Frame_Transform(frame->view_origin, frame->view_rotation);
}

bool Set_Default_Frame_Transform()
{
	float pos[3] = {0,0,0};
	float rot[3] = {0,0,0};
	Set_Frame_Transform(pos, rot);
	return true;
}

bool Render_Left_Frame(int w, int h)
{
	if(!frame)return false;
	Set_Left_View(w, h);
	Render_Frame(frame, true);
	return true;
}

bool Render_Right_Frame(int w, int h)
{
	if(!frame)return false;
	Set_Right_View(w, h);
	Render_Frame(frame, true);
	return true;
}

bool Get_Frame(unsigned char *res, int w, int h)
{
	glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,res);
	return true;
}


//FIXTHIS maybe put anaglyph stuff in stereo header? should be inline
//from
//http://research.csc.ncsu.edu/stereographics/LS.pdf

//
//The method uses least squares approximation in CIE
//color space and is the work of Eric Dubois at Ottawa U. It is designed for LCD monitors
//using LCD spectral distribution data and filter transmission functions collected by Vu
//Tran, also at Ottawa U.

float p1[9] ={.4155f, .4710f, .1670f,
			-.0458f, -.0484f, -.0258f,
			-.0545f, -.0614f, .0128f};

float p2[9] ={-.0109f, -.0365f, -.0060f,
			.3756f, .7333f, .0111f,
			-.0651f, -.1286f, 1.2968f};

__forceinline void Multiply_Vector_By_Matrix3x3(float *s, float *m)
{
	float t[3];
		t[0]=s[0]*m[0]+
			s[1]*m[1]+
			s[2]*m[2];
		t[1]=s[0]*m[3]+
			s[1]*m[4]+
			s[2]*m[5];
		t[2]=s[0]*m[6]+
			s[1]*m[7]+
			s[2]*m[8];
		s[0] = t[0];
		s[1] = t[1];
		s[2] = t[2];
}

__forceinline void Normalize_RGB(float *rgb)
{
	if(rgb[0]>1)rgb[0]=1;
	if(rgb[0]<0)rgb[0]=0;
	if(rgb[1]>1)rgb[1]=1;
	if(rgb[1]<0)rgb[1]=0;
	if(rgb[2]>1)rgb[2]=1;
	if(rgb[2]<0)rgb[2]=0;
}


__forceinline void Get_Anaglyph_Pixel(float *c1, float *c2, float *rgb)
{
	Multiply_Vector_By_Matrix3x3(c1, p1);
	Multiply_Vector_By_Matrix3x3(c2, p2);
	rgb[0] = c1[0]+c2[0];
	rgb[1] = c1[1]+c2[1];
	rgb[2] = c1[2]+c2[2];
	Normalize_RGB(rgb);
}

__forceinline void Get_Anaglyph_Pixel(unsigned char *c1, unsigned char *c2, unsigned char *rgb)
{
	float fc1[3] = {((float)c1[0])/255,((float)c1[1])/255,((float)c1[2])/255};
	float fc2[3] = {((float)c2[0])/255,((float)c2[1])/255,((float)c2[2])/255};
	float frgb[3];
	Get_Anaglyph_Pixel(fc1, fc2, frgb);
	frgb[0] *= 255;
	frgb[1] *= 255;
	frgb[2] *= 255;
	rgb[0] = (unsigned char)frgb[0];
	rgb[1] = (unsigned char)frgb[1];
	rgb[2] = (unsigned char)frgb[2];
}

bool Render_Anaglyph_Frame(int w, int h)
{
	if(!frame)return false;

	int total = w*h;
	unsigned char *left = new unsigned char[total*4];
	unsigned char *right = new unsigned char[total*4];
	memset(left, 255, total*3);
	memset(right, 255, total*3);
	Render_Left_Frame(w, h);
	Get_Frame(left, w, h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	Render_Right_Frame(w, h);
	Get_Frame(right, w, h);

	float c1[3];
	float c2[3];
	float rgb[3];
	for(int i = 0;i<total;i++)
	{
		c1[0] = (float)right[(i*3)+0]/255;
		c1[1] = (float)right[(i*3)+1]/255;
		c1[2] = (float)right[(i*3)+2]/255;
		c2[0] = (float)left[(i*3)+0]/255;
		c2[1] = (float)left[(i*3)+1]/255;
		c2[2] = (float)left[(i*3)+2]/255;
		Get_Anaglyph_Pixel(c1, c2, rgb);
		left[(i*3)+0] = (unsigned char)(rgb[0]*255);
		left[(i*3)+1] = (unsigned char)(rgb[1]*255);
		left[(i*3)+2] = (unsigned char)(rgb[2]*255);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	glRasterPos2f(0,0);
	glDrawPixels(w,h,GL_RGB,GL_UNSIGNED_BYTE,left);
	Set_3D_View();

	delete[] left;
	delete[] right;
	return true;
}

bool Render_Temp_Anaglyph_Frame(int w, int h)
{
	float opos[3];
	float orot[3];
	opos[0] = camera_pos[0];opos[1] = camera_pos[1];opos[2] = camera_pos[2];
	orot[0] = camera_rot[0];orot[1] = camera_rot[1];orot[2] = camera_rot[2];
	Get_View_Center(camera_pos, camera_rot);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Set_GL_Camera_Transform();
	Get_Matrices();
	Update_Camera_Dir();
	Render_Anaglyph_Frame(w, h);
	camera_pos[0] = opos[0];camera_pos[1] = opos[1];camera_pos[2] = opos[2];
	camera_rot[0] = orot[0];camera_rot[1] = orot[1];camera_rot[2] = orot[2];
	Set_3D_View();
	return true;
}

bool Render_Temp_Left_Frame(int w, int h)
{
	float opos[3];
	float orot[3];
	opos[0] = camera_pos[0];opos[1] = camera_pos[1];opos[2] = camera_pos[2];
	orot[0] = camera_rot[0];orot[1] = camera_rot[1];orot[2] = camera_rot[2];
	Get_View_Center(camera_pos, camera_rot);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Set_GL_Camera_Transform();
	Get_Matrices();
	Update_Camera_Dir();
	Render_Left_Frame(w, h);
	camera_pos[0] = opos[0];camera_pos[1] = opos[1];camera_pos[2] = opos[2];
	camera_rot[0] = orot[0];camera_rot[1] = orot[1];camera_rot[2] = orot[2];
	Set_3D_View();
	return true;
}

bool Render_Temp_Right_Frame(int w, int h)
{
	float opos[3];
	float orot[3];
	opos[0] = camera_pos[0];opos[1] = camera_pos[1];opos[2] = camera_pos[2];
	orot[0] = camera_rot[0];orot[1] = camera_rot[1];orot[2] = camera_rot[2];
	Get_View_Center(camera_pos, camera_rot);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Set_GL_Camera_Transform();
	Get_Matrices();
	Update_Camera_Dir();
	Render_Right_Frame(w, h);
	camera_pos[0] = opos[0];camera_pos[1] = opos[1];camera_pos[2] = opos[2];
	camera_rot[0] = orot[0];camera_rot[1] = orot[1];camera_rot[2] = orot[2];
	Set_3D_View();
	return true;
}

bool Get_Screen_Coordinates(float *pos, float *x, float *y)
{
	double wx, wy, wz;
	gluProject(pos[0], pos[1], pos[2], modelview_matrix, projection_matrix, viewport, &wx, &wy, &wz);
	//flip for regular win32 top-down coordinates vs opengl bottom-up layout
	wy = (Screenheight()-1)-wy;

	*x = wx;
	*y = wy;
	return true;
}

bool Get_Frame_Screen_Coordinates(float *pos, float *x, float *y)
{
	double wx, wy, wz;
	gluProject(pos[0], pos[1], pos[2], frame_modelview_matrix, frame_projection_matrix, frame_viewport, &wx, &wy, &wz);
	//flip for regular win32 top-down coordinates vs opengl bottom-up layout

	*x = wx;
	*y = wy;
	return true;
}

__forceinline bool Get_Export_Frame(unsigned char *rgb, bool left, bool right)
{
/*	glViewport(0,0,frame->width,frame->height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, frame->width, 0, frame->height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(0,0,0);
	glBegin(GL_LINES);
	glVertex2f(0,0);
	glVertex2f(frame->width, frame->height);
	glVertex2f(frame->width,0);
	glVertex2f(0, frame->height);
	glVertex2f(frame->width/2,0);
	glVertex2f(0, frame->height/2);
	glVertex2f(frame->width/2,0);
	glVertex2f(frame->width, frame->height/2);
	glVertex2f(frame->width/2,frame->height);
	glVertex2f(0, frame->height/2);
	glVertex2f(frame->width/2,frame->height);
	glVertex2f(frame->width, frame->height/2);
	srand(Get_Current_Project_Frame()+random_seed);
	for(int i = 0;i<5;i++)
	{
		int row = rand()%frame->height;
		int column = rand()%frame->width;
		glVertex2f(column, 0);
		glVertex2f(column, frame->height);
		glVertex2f(0, row);
		glVertex2f(frame->width, row);
	}
	glEnd();*/
	glReadPixels(0,0,frame->width,frame->height,GL_RGB,GL_UNSIGNED_BYTE,rgb);

	float *dmap = frame->rgb_colors;

	if(left)
	{
		Get_Left_Raycast_Map(dmap);
	}
	if(right)
	{
		Get_Right_Raycast_Map(dmap);
	}
	AntiAlias_Edges(rgb, dmap);
	Replace_RGB_Values(frame->original_rgb);
	return true;
}

bool Generate_Anaglyph_Image_From_32Bit(int n, unsigned char *left, unsigned char *right, unsigned char *res)
{
	memset(res, 0, frame->width*frame->height*3);
	for(int i = 0;i<n;i++)
	{
		Get_Anaglyph_Pixel(&right[i*4], &left[i*4], &res[i*3]);
	}
	return true;
}

bool Generate_Anaglyph_Image_From_24Bit(int n, unsigned char *left, unsigned char *right, unsigned char *res)
{
	memset(res, 0, frame->width*frame->height*3);
	for(int i = 0;i<n;i++)
	{
		Get_Anaglyph_Pixel(&right[i*3], &left[i*3], &res[i*3]);
	}
	return true;
}

//FIXTHIS put all these into getframes or something similar
bool Save_Watermarked_Image(unsigned char *rgb, int w, int h, char *file);
bool Save_Stereo_Watermarked_Image(unsigned char *rgb, int w, int h, char *file);

bool Auto_Fill_Gaps_In_Left_Image(unsigned char *rgb, float *positions);
bool Auto_Fill_Gaps_In_Right_Image(unsigned char *rgb, float *positions);

bool auto_gapfill = true;

__forceinline void Plot_Black_Pixel(unsigned char *rgb, int x, int y, int width, int height)
{
	unsigned char *c = &rgb[((y*width)+x)*3];
	c[0] = c[1] = c[2] = 0;
}

__forceinline void Add_Line_To_Image(unsigned char *rgb, int *x, int *y, int *xdir, int *ydir, int width, int height)
{
	bool hit_edge = false;
	while(!hit_edge)
	{
		if((*x)>=width){*x -= *xdir;*xdir = -(*xdir);return;}
		if((*x)<0){*x -= *xdir;*xdir = -(*xdir);return;}
		if((*y)>=height){*y -= *ydir;*ydir = -(*ydir);return;}
		if((*y)<0){*y -= *ydir;*ydir = -(*ydir);return;}
		Plot_Black_Pixel(rgb, *x, *y, width, height);
		*x += *xdir;
		*y += *ydir;
	}
}

bool Add_Random_Lines_To_Image(unsigned char *rgb, int width, int height, int bounce)
{
	int x = rand()%(width-1);
	int y = rand()%(height-1);
	int xdir = 1;
	int ydir = 1;
	if(rand()%2==1)
	{
		xdir = -1;
	}
	if(rand()%2==1)
	{
		ydir = -1;
	}
	for(int i = 0;i<bounce+1;i++)
	{
		Add_Line_To_Image(rgb, &x, &y, &xdir, &ydir, width, height);
		if(rand()%2==1)
		{
			if(xdir<0)
			{
				xdir = -2;
			}
			else
			{
				xdir = 2;
			}
		}
		else
		{
			if(xdir<0)
			{
				xdir = -1;
			}
			else
			{
				xdir = 1;
			}
		}
		if(rand()%2==1)
		{
			if(ydir<0)
			{
				ydir = -2;
			}
			else
			{
				ydir = 2;
			}
		}
		else
		{
			if(ydir<0)
			{
				ydir = -1;
			}
			else
			{
				ydir = 1;
			}
		}
	}
	return true;
}



bool Add_Lines_To_Image(unsigned char *rgb, int width, int height);

bool PostRender_Halos(unsigned char *rgb, float *depth_buffer, int width, int height, bool left, bool right)
{
	Setup_PBuffer(width, height);
	
	glDrawPixels(width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer);
	glDepthMask(GL_FALSE);

	glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, rgb);
	
	glDepthMask(GL_TRUE);
	if(left)
	{
		Set_Left_View(width, height);
	}
	if(right)
	{
		Set_Right_View(width, height);
	}

	PostRender_Halos(rgb);

	//get the final image
	Undo_PBuffer();
	return true;
}

__forceinline bool BlankRGB(unsigned char *rgb)
{
	return (rgb[0]==0&&rgb[1]==0&&rgb[2]==0);
}

__forceinline void Fill_Blank_Pixel(unsigned char *a, unsigned char *b, unsigned char *res)
{
	float _r = (((float)a[0])+((float)b[0]))*0.5f;
	float _g = (((float)a[1])+((float)b[1]))*0.5f;
	float _b = (((float)a[2])+((float)b[2]))*0.5f;
	res[0] = (unsigned char)_r;
	res[1] = (unsigned char)_g;
	res[2] = (unsigned char)_b;
}

//interpolate blank pixels across a hole, return the number of filled pixels
__forceinline int Interpolate_Hole(unsigned char *start, int start_index)
{
	//save the start color
	float start_rgb[3] = {(float)start[0],(float)start[1],(float)start[2]};
	float end_rgb[3] = {(float)start[0],(float)start[1],(float)start[2]};

	//find the next valid color after the 2nd pixel (the one following "start" will be blank)
	int cnt = 1;//we know there is at least 1 blank pixel
	bool hit = false;
	int i;
	for(i = start_index+2;i<frame->width;i++)
	{
		if(!BlankRGB(&start[(cnt+1)*3]))
		{
			//we found the end
			end_rgb[0] = start[((cnt+1)*3)];
			end_rgb[1] = start[((cnt+1)*3)+1];
			end_rgb[2] = start[((cnt+1)*3)+2];
			i = frame->width;//duck out early
			hit = true;
		}
		else
		{
			cnt++;
		}
	}
	if(!hit)return frame->width;//nothing hit, blank pixels go to end of frame
	//now fill the blank pixels from some mixed percentag eof the start and end colors
	unsigned char *dst;
	float r, g, b, p, ip;
	for(i = 0;i<cnt;i++)
	{
		p = ((float)(i+1))/(cnt+4);
		ip = 1.0f-p;
		r = (start_rgb[0]*ip)+(end_rgb[0]*p);
		g = (start_rgb[1]*ip)+(end_rgb[1]*p);
		b = (start_rgb[2]*ip)+(end_rgb[2]*p);
		dst = &start[(i+1)*3];
		dst[0] = (unsigned char)r;
		dst[1] = (unsigned char)g;
		dst[2] = (unsigned char)b;
	}
	return cnt;
}

void Interpolate_Holes(unsigned char *rgb)
{
	int i, j, k;
	for(j = 0;j<frame->height;j++)
	{
		for(i = 1;i<frame->width-1;i++)
		{
			k = (j*frame->width)+i;
			if(BlankRGB(&rgb[k*3]))
			{
				//found a blank pixel
				if(!BlankRGB(&rgb[(k-1)*3]))
				{
					//the previous one is fine
					i += Interpolate_Hole(&rgb[(k-1)*3], i);
				}
			}
		}
	}
}

//accumulate a blended pixel into the buffer
__forceinline void Accumulate_Pixel(int sx, int sy, float *positions, float *percentages, float *colors)
{
	//index into the buffers
	int sk = (frame->width*sy)+sx;
	//get the floating point target pixel
	float px = positions[sk*3];
	float py = positions[(sk*3)+1];
	float pz = positions[(sk*3)+2];

	//get the percentage into the pixel in xy
	float rx = px-((int)px);
	float ry = py-((int)py);
	//get a 2x2 block of 4 pixels that this affects
	int lx, ly, hx, hy;
	//and x and y percentage as applied to upper and lower bound
	float lxp, lyp, hxp, hyp;
	if(rx<0.5f)
	{
		//it is left of center
		lx = (int)px;//low index will be this pixel
		hx = lx+1;//high index will be the next one
		hxp = rx;
		lxp = 1.0f-rx;
	}
	else
	{
		//it is right of center
		hx = (int)px;//high index will be this pixel
		lx = hx-1;//low index will be the previous one
		lxp = rx-0.5f;
		hxp = 1.0f-lxp;
	}
	if(ry<0.5f)
	{
		//it is top of center
		ly = (int)py;//low index will be this pixel
		hy = ly+1;//high index will be the next one
		hyp = ry;
		lyp = 1.0f-ry;
	}
	else
	{
		//it is bottom of center
		hy = (int)py;//high index will be this pixel
		ly = hx-1;//low index will be the previous one
		lyp = ry-0.5f;
		hyp = 1.0f-lyp;
	}
	int tx, ty, dk;
	float percent = 1;
	tx = (int)px;
	ty = (int)py;
	if(frame->IsValidPixel(tx, ty))
	{
		dk = (frame->width*ty)+tx;
		if(frame->IsValidPixel(lx, ly)&&frame->IsValidPixel(hx, hy))
		{
			tx = lx;ty = ly;percent = lxp*lyp;dk = (frame->width*ty)+tx;{percentages[dk] += percent;colors[(dk*3)] += ((float)frame->original_rgb[(sk*3)]/255)*percent;colors[(dk*3)+1] += ((float)frame->original_rgb[(sk*3)+1]/255)*percent;colors[(dk*3)+2] += ((float)frame->original_rgb[(sk*3)+2]/255)*percent;}
			tx = hx;ty = ly;percent = hxp*lyp;dk = (frame->width*ty)+tx;{percentages[dk] += percent;colors[(dk*3)] += ((float)frame->original_rgb[(sk*3)]/255)*percent;colors[(dk*3)+1] += ((float)frame->original_rgb[(sk*3)+1]/255)*percent;colors[(dk*3)+2] += ((float)frame->original_rgb[(sk*3)+2]/255)*percent;}
			tx = hx;ty = hy;percent = hxp*hyp;dk = (frame->width*ty)+tx;{percentages[dk] += percent;colors[(dk*3)] += ((float)frame->original_rgb[(sk*3)]/255)*percent;colors[(dk*3)+1] += ((float)frame->original_rgb[(sk*3)+1]/255)*percent;colors[(dk*3)+2] += ((float)frame->original_rgb[(sk*3)+2]/255)*percent;}
			tx = lx;ty = hy;percent = lxp*hyp;dk = (frame->width*ty)+tx;{percentages[dk] += percent;colors[(dk*3)] += ((float)frame->original_rgb[(sk*3)]/255)*percent;colors[(dk*3)+1] += ((float)frame->original_rgb[(sk*3)+1]/255)*percent;colors[(dk*3)+2] += ((float)frame->original_rgb[(sk*3)+2]/255)*percent;}
		}
	}
}

bool Invalidate_Obscured_Pixels(float *positions, float *depths)
{
	memset(depths, 0, sizeof(float)*frame->width*frame->height);
	int i, j, k;
	float px, py, pz;
	int ix, iy;
	//first register all depths and overwrite values out of range
	for(i = 0;i<frame->width;i++)
	{
		for(j = 0;j<frame->height;j++)
		{
			k = (frame->width*j)+i;
			px = positions[(k*3)];
			py = positions[(k*3)+1];
			pz = positions[(k*3)+2];
			ix = (int)px;
			iy = (int)py;
			if(frame->IsValidPixel(ix, iy))
			{
				k = (frame->width*iy)+ix;
				if(depths[k]==0||depths[k]>pz)
				{
					depths[k] = pz;
				}
			}
		}
	}
	int n = frame->width*frame->height;
	for(i = 0;i<n;i++)
	{
		px = positions[(i*3)];
		py = positions[(i*3)+1];
		pz = positions[(i*3)+2];
		ix = (int)px;
		iy = (int)py;
		if(frame->IsValidPixel(ix, iy))
		{
			k = (frame->width*iy)+ix;
			if(depths[k]!=pz)
			{
				//this pixel was overwritten
				positions[(i*3)] = -10;//set it out of range
				positions[(i*3)+1] = -10;
			}
		}
	}
	return true;
}

//accumulate blended pixels into the buffer
bool Accumulate_Pixels(float *positions, float *percentages, float *colors)
{
	memset(percentages, 0, sizeof(float)*frame->width*frame->height);
	memset(colors, 0, sizeof(float)*frame->width*frame->height*3);
	int i, j;
	for(i = 0;i<frame->width;i++)
	{
		for(j = 0;j<frame->height;j++)
		{
			Accumulate_Pixel(i, j, positions, percentages, colors);
		}
	}
	return true;
}

//get a pixel source map from the projected positions
void Get_Source_Map(int *pmap, float *positions)
{
	//pmap is a 1d pixel index map of all source pixels
	int i, j;
	int n = frame->width*frame->height;
	for(i = 0;i<n;i++)
	{
		pmap[i] = -1;//assume no source pixel
	}
	int px, py;//projected pixel locations
	for(i = 0;i<frame->width;i++)
	{
		for(j = 0;j<frame->height;j++)
		{
			//get the projected pixel location for src pixel i,j
			px = (int)positions[(((frame->width*j)+i)*3)];
			py = (int)positions[(((frame->width*j)+i)*3)+1];
			if(frame->IsValidPixel(px, py))
			{
				//see if this one's already been hit
				if(pmap[(((frame->width*py)+px))]==-1)
				{
					//nothing there already, use i,j as the source pixel
					pmap[(((frame->width*py)+px))] = (frame->width*j)+i;
				}
				else
				{
					//THIS IS A BUG< IT DOESN"T COMPARE REAL DEPTH FOR WHAT MIGHT ALREADY
					//BE IN THE TARGET PIXEL!!!!!

					//a pixel already exists, compare the older depth to the current
					float *old_pos = &positions[pmap[(((frame->width*py)+px))]*3];
					float *new_pos = &positions[(((frame->width*j)+i)*3)];
					if(new_pos[2]<old_pos[2])
					{
						//it's closer, use this i,j as the src instead
						pmap[(((frame->width*py)+px))] = (frame->width*j)+i;
					}
				}
			}
		}
	}
}

bool Rasterize_Left_Frame(unsigned char *rgb);
bool Rasterize_Right_Frame(unsigned char *rgb);

bool Rasterize_Frame(unsigned char *rgb, bool left, bool right)
{

	bool shared_vectors = false;
	if(conserve_memory_during_output)
	{
		//dammit dump the projection vectors and restore below
		if(frame->identity_vectors==frame->frame_vectors)
		{
			delete[] frame->identity_vectors;
			shared_vectors = true;
		}
		else
		{
			delete[] frame->identity_vectors;
			delete[] frame->frame_vectors;
		}
	}

	int total = frame->width*frame->height;
	//re-use the frame rgb buffer for the dmap
	
	float *dmap = frame->rgb_colors;
	if(left)
	{
		Get_Left_Raycast_Map(dmap);
	}
	else if(right)
	{
		Get_Right_Raycast_Map(dmap);
	}
	else
	{
		return false;
	}

	float *depths = new float[frame->width*frame->height];
	Invalidate_Obscured_Pixels(dmap, depths);
	delete[] depths;
	
	float *colors = new float[frame->width*frame->height*3];
	float *percentages = new float[frame->width*frame->height];

	Accumulate_Pixels(dmap, percentages, colors);
	int n = frame->width*frame->height;
	//normalize the colors
	int i;
	for(i = 0;i<n;i++)
	{
		//normalize the color
		float p = 1.0f/percentages[i];
		colors[(i*3)] *= p;
		colors[(i*3)+1] *= p;
		colors[(i*3)+2] *= p;
		if(colors[(i*3)]>1)colors[(i*3)] = 1;
		if(colors[(i*3)+1]>1)colors[(i*3)+1] = 1;
		if(colors[(i*3)+2]>1)colors[(i*3)+2] = 1;
	}
	for(i = 0;i<n;i++)
	{
		rgb[(i*3)] = (unsigned char)((float)colors[(i*3)]*255);
		rgb[(i*3)+1] = (unsigned char)((float)colors[(i*3)+1]*255);
		rgb[(i*3)+2] = (unsigned char)((float)colors[(i*3)+2]*255);
	}

	delete[] colors;
	delete[] percentages;

	Interpolate_Holes(rgb);//fill any holes
	AntiAlias_Edges(rgb, dmap);//fix hard edges
	if(left)
	{
		Auto_Fill_Gaps_In_Left_Image(rgb, dmap);
	}
	else
	{
		Auto_Fill_Gaps_In_Right_Image(rgb, dmap);
	}
	//restore the original colors
	Replace_RGB_Values(frame->original_rgb);
	
	if(conserve_memory_during_output)
	{   
		if(shared_vectors)
		{
			frame->identity_vectors = new float[frame->width*frame->height*3];
			frame->frame_vectors = frame->identity_vectors;
		}
		else
		{
			frame->identity_vectors = new float[frame->width*frame->height*3];
			frame->frame_vectors = new float[frame->width*frame->height*3];
		}
		Get_Flat_Projection_Vectors();
		ReApply_Frame_Transform();
	}

	return true;
}

bool Rasterize_Left_Frame(unsigned char *rgb)
{
	bool shared_vectors = false;
	if(conserve_memory_during_output)
	{
		//dammit dump the projection vectors and restore below
		if(frame->identity_vectors==frame->frame_vectors)
		{
			delete[] frame->identity_vectors;
			shared_vectors = true;
		}
		else
		{
			delete[] frame->identity_vectors;
			delete[] frame->frame_vectors;
		}
	}

	int total = frame->width*frame->height;
	//re-use the frame rgb buffer for the dmap
	
	float *dmap = frame->rgb_colors;
	Get_Left_Raycast_Map(dmap);

	int *pmap = new int[frame->width*frame->height];
	Get_Source_Map(pmap, dmap);

	int n = frame->width*frame->height;
	unsigned char *clr;
	for(int i = 0;i<n;i++)
	{
		if(pmap[i]!=-1)
		{
			clr = &frame->original_rgb[pmap[i]*3];
			rgb[(i*3)] = clr[0];
			rgb[(i*3)+1] = clr[1];
			rgb[(i*3)+2] = clr[2];
		}
	}
	delete[] pmap;


	Interpolate_Holes(rgb);//fill any holes
	AntiAlias_Edges(rgb, dmap);//fix hard edges
	Auto_Fill_Gaps_In_Left_Image(rgb, dmap);
	//restore the original colors
	Replace_RGB_Values(frame->original_rgb);
	
	if(conserve_memory_during_output)
	{
		if(shared_vectors)
		{
			frame->identity_vectors = new float[frame->width*frame->height*3];
			frame->frame_vectors = frame->identity_vectors;
		}
		else
		{
			frame->identity_vectors = new float[frame->width*frame->height*3];
			frame->frame_vectors = new float[frame->width*frame->height*3];
		}
		Get_Flat_Projection_Vectors();
		ReApply_Frame_Transform();
	}

	return true;
}

bool Rasterize_Right_Frame(unsigned char *rgb)
{
	bool shared_vectors = false;
	if(conserve_memory_during_output)
	{
		//dammit dump the projection vectors and restore below
		if(frame->identity_vectors==frame->frame_vectors)
		{
			delete[] frame->identity_vectors;
			shared_vectors = true;
		}
		else
		{
			delete[] frame->identity_vectors;
			delete[] frame->frame_vectors;
		}
	}

	int total = frame->width*frame->height;
	//re-use the frame rgb buffer for the dmap
	float *dmap = frame->rgb_colors;
	Get_Right_Raycast_Map(dmap);

	int *pmap = new int[frame->width*frame->height];
	Get_Source_Map(pmap, dmap);
	int n = frame->width*frame->height;
	unsigned char *clr;
	for(int i = 0;i<n;i++)
	{
		if(pmap[i]!=-1)
		{
			clr = &frame->original_rgb[pmap[i]*3];
			rgb[(i*3)] = clr[0];
			rgb[(i*3)+1] = clr[1];
			rgb[(i*3)+2] = clr[2];
		}
	}
	delete[] pmap;
	Interpolate_Holes(rgb);//fill any holes
	AntiAlias_Edges(rgb, dmap);//fix hard edges
	Auto_Fill_Gaps_In_Right_Image(rgb, dmap);
	//restore the original colors
	Replace_RGB_Values(frame->original_rgb);

	if(conserve_memory_during_output)
	{
		if(shared_vectors)
		{
			frame->identity_vectors = new float[frame->width*frame->height*3];
			frame->frame_vectors = frame->identity_vectors;
		}
		else
		{
			frame->identity_vectors = new float[frame->width*frame->height*3];
			frame->frame_vectors = new float[frame->width*frame->height*3];
		}
		Get_Flat_Projection_Vectors();
		ReApply_Frame_Transform();
	}

	return true;
}


bool Get_Left_Frame(unsigned char *rgb)
{
	if(manually_rasterize_frames)
	{
		return Rasterize_Frame(rgb, true, false);
	}
	//stop halos from rendering to intermediate image
	bool rht = render_halo_tool;
	render_halo_tool = false;

	memset(rgb, 0, frame->width*frame->height*3);
	Setup_PBuffer(frame->width, frame->height);
	Render_Left_Frame(frame->width, frame->height);
	float *depth_buffer = 0;
	if(Halos_Exist())
	{
		depth_buffer = new float[frame->width*frame->height];
		glReadPixels(0, 0, frame->width, frame->height, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer);
	}
	Get_Export_Frame(rgb, true, false);
	Undo_PBuffer();
	//auto gap fill
	if(auto_gapfill)Auto_Fill_Gaps_In_Left_Image(rgb, 0);
	if(Halos_Exist())
	{
		PostRender_Halos(rgb, depth_buffer, frame->width, frame->height, true, false);
		delete[] depth_buffer;
	}
	Add_Lines_To_Image(rgb, frame->width, frame->height);

	render_halo_tool = rht;
	return true;
}

bool Get_Right_Frame(unsigned char *rgb)
{
	if(manually_rasterize_frames)
	{
		return Rasterize_Frame(rgb, false, true);
	}
	//stop halos from rendering to intermediate image
	bool rht = render_halo_tool;
	render_halo_tool = false;

	memset(rgb, 0, frame->width*frame->height*3);
	Setup_PBuffer(frame->width, frame->height);
	Render_Right_Frame(frame->width, frame->height);
	float *depth_buffer = 0;
	if(Halos_Exist())
	{
		depth_buffer = new float[frame->width*frame->height];
		glReadPixels(0, 0, frame->width, frame->height, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer);
	}
	Get_Export_Frame(rgb, false, true);
	Undo_PBuffer();
	//auto gap fill
	if(auto_gapfill)Auto_Fill_Gaps_In_Right_Image(rgb, 0);
	if(Halos_Exist())
	{
		PostRender_Halos(rgb, depth_buffer, frame->width, frame->height, false, true);
		delete[] depth_buffer;
	}
	Add_Lines_To_Image(rgb, frame->width, frame->height);
	render_halo_tool = rht;
	return true;
}

//used for gapfill tool only
bool Get_Clean_Left_Frame(unsigned char *rgb)
{
	memset(rgb, 0, frame->width*frame->height*3);
	Setup_PBuffer(frame->width, frame->height);
	Render_Left_Frame(frame->width, frame->height);
	Get_Export_Frame(rgb, true, false);
	Undo_PBuffer();
	return true;
}

//used for gapfill tool only
bool Get_Clean_Right_Frame(unsigned char *rgb)
{
	memset(rgb, 0, frame->width*frame->height*3);
	Setup_PBuffer(frame->width, frame->height);
	Render_Right_Frame(frame->width, frame->height);
	Get_Export_Frame(rgb, false, true);
	Undo_PBuffer();
	return true;
}

bool Get_Stereo_Frame(unsigned char *rgb)
{
	memset(rgb, 0, frame->width*frame->height*6);
	int w = frame->width;
	int h = frame->height;
	unsigned char *left = new unsigned char[w*h*3];
	unsigned char *right = new unsigned char[w*h*3];
	Get_Left_Frame(left);
	Get_Right_Frame(right);
	for(int i = 0;i<h;i++)
	{
		memcpy(&rgb[(i*w*2*3)], &left[i*w*3], sizeof(unsigned char)*w*3);
		memcpy(&rgb[(i*w*2*3)+(w*3)], &right[i*w*3], sizeof(unsigned char)*w*3);
	}
	return true;
}

bool Save_Stereo_Image(char *file)
{
	Print_Status("Rendering stereo image %s", file);
	unsigned char *image = new unsigned char[frame->width*frame->height*6];
	Get_Stereo_Frame(image);
	Save_Stereo_Watermarked_Image(image, frame->width*2, frame->height, file);
	delete[] image;
	Print_Status("Done");
	return true;
}


bool Get_Anaglyph_Frame(unsigned char *rgb)
{
	memset(rgb, 0, frame->width*frame->height*3);
	unsigned char *left = new unsigned char[frame->width*frame->height*3];
	unsigned char *right = new unsigned char[frame->width*frame->height*3];
	Get_Left_Frame(right);
	Get_Right_Frame(left);
	int n = frame->width*frame->height;
	for(int i = 0;i<n;i++)
	{
		Get_Anaglyph_Pixel(&left[i*3], &right[i*3], &rgb[i*3]);
	}
	delete[] left;
	delete[] right;
	return true;
}


bool Save_Anaglyph_Image(char *file)
{
	Print_Status("Rendering anaglyph image %s", file);
	unsigned char *image = new unsigned char[frame->width*frame->height*3];
	Get_Anaglyph_Frame(image);
	Save_Watermarked_Image(image, frame->width, frame->height, file);
	delete[] image;
	Print_Status("Done");
	return true;
}


bool Save_Left_Image(char *file)
{
	Print_Status("Rendering left image %s", file);
	unsigned char *image = new unsigned char[frame->width*frame->height*3];
	Get_Left_Frame(image);

	if(conserve_memory_during_output)
	{
		delete[] frame->rgb_colors;
	}
	Save_Watermarked_Image(image, frame->width, frame->height, file);
	delete[] image;

	if(conserve_memory_during_output)
	{
		frame->rgb_colors = new float[frame->width*frame->height*3];
		Replace_RGB_Values(frame->original_rgb);
	}

	Print_Status("Done");
	return true;
}

bool Save_Right_Image(char *file)
{
	Print_Status("Rendering right image %s", file);
	unsigned char *image = new unsigned char[frame->width*frame->height*3];
	Get_Right_Frame(image);

	if(conserve_memory_during_output)
	{
		delete[] frame->rgb_colors;
	}

	Save_Watermarked_Image(image, frame->width, frame->height, file);
	delete[] image;

	if(conserve_memory_during_output)
	{
		frame->rgb_colors = new float[frame->width*frame->height*3];
		Replace_RGB_Values(frame->original_rgb);
	}

	Print_Status("Done");
	return true;
}


__forceinline void Render_Frame_3D()
{
	if(render_preview_data)
	{
		Render_Preview_3D();
	}
	else
	{
		Render_Layers_3D();
	}

}

__forceinline bool Render_Frame_Only(FRAME *f)
{
	glPushMatrix();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, f->vertices);
	glColorPointer(3, GL_FLOAT, 0, f->rgb_colors);

	Render_Frame_3D();
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	if(render_halo_tool)Render_Halo_Tool_3D();

	if(render_background_meshes)
	{
		Render_Background_Meshes();
	}

	Report_Render3D_Event();

	Get_Matrices();
	glPopMatrix();
	
	return true;
}


bool Render_Frame(FRAME *f, bool frame_only)
{
	if(frame_only)
	{
		return Render_Frame_Only(frame);
	}
	glPushMatrix();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, f->vertices);
	glColorPointer(3, GL_FLOAT, 0, f->rgb_colors);


	Render_Frame_3D();
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	if(render_grid)
	{
		Render_Grid();
	}

	if(show_geometry)
	{
		Render_Geometry();
	}
	Render_Outlined_Layer_Outlines();
	Render_Speckled_Layer_Speckles();

	if(render_background_meshes)
	{
		Render_Background_Meshes();
	}
		
	if(render_halo_tool)Render_Halo_Tool_3D();
	
	Report_Render3D_Event();

	Get_Matrices();
	glPopMatrix();
	
	if(virtual_camera_tool_open)
	{
		Render_Virtual_Camera_Tool();
	}


	Render_Selection_Outline();
	if(render_puff_preview&&!anaglyph_mode)Render_Puff_Preview();//false, false);
	if(run_layer_popup_menu)
	{
		Render_3D_Layer_Outline(last_clicked_layer);
	}
	if(render_feature_points_3d)
	{
		glPushMatrix();
		Render_Feature_Points_3D();
		glPopMatrix();
	}

	//do this last it works independent of scale
	if(render_camera)
	{
		Render_Camera();
	}
	return true;
}

//FIXTHIS all should use the glapp inline stuff
float VecLength(float* v)
{
	return (float)sqrt((v[0]*v[0])+(v[1]*v[1])+(v[2]*v[2]));
}

void Normalize(float *v)
{
	float len = 1.0f / VecLength(v);
	v[0] *= len;
	v[1] *= len;
	v[2] *= len;
}

void SetLength(float *v, float s)
{
	float len = s / VecLength(v);
	v[0] *= len;
	v[1] *= len;
	v[2] *= len;
}

bool Render_Selection_Outline()
{
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
	if(frame)
	{
		glPushMatrix();

		glPointSize(4);
		glColor3f(1,0,0);
		if(!anaglyph_mode)Render_Selection_Outline_3D();
		glPopMatrix();
		glColor3f(1,1,1);
		glPointSize(1);
	}
	return true;
}


bool Render_3D_View()
{
	if(!frame)return false;
	//FIXTHIS it should not be necessary
	//only happens when selection occurs or perspective alignment
	Set_GL_Camera_Transform();

	if(render_link_points_3d_tool)
	{
		if(view_link_points_model)
		{
			glPushMatrix();
			Get_Matrices();
			Render_Link_Points_Tool_3D_Model();
			glPopMatrix();
			return true;
		}
		else if(render_link_points_vectors)
		{
			glPushMatrix();
			Get_Matrices();
			Render_Link_Points_Vectors();
			glPopMatrix();
		}
	}

	if(cull_backfaces)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	if(anaglyph_mode)
	{
		Render_Anaglyph_Frame(_screenwidth, _screenheight);
		return true;
	}
	if(keys_pressed[view_anaglyph_key]||edit_keys_pressed[view_anaglyph_key])
	{
		Render_Temp_Anaglyph_Frame(_screenwidth, _screenheight);
		return true;
	}
	if(keys_pressed[view_left_key]||edit_keys_pressed[view_left_key])
	{
		Render_Temp_Left_Frame(_screenwidth, _screenheight);
		return true;
	}
	if(keys_pressed[view_right_key]||edit_keys_pressed[view_right_key])
	{
		Render_Temp_Right_Frame(_screenwidth, _screenheight);
		return true;
	}
	if(frame)
	{
		Render_Frame(frame, false);
	}
	return true;
}

float Frame_World_Width()
{
	if(!frame)return 0;
	return frame->world_width;
}

float Frame_World_Depth()
{
	if(!frame)return 0;
	return -frame->max_world_depth;
}

float Frame_World_Height()
{
	if(!frame)return 0;
	return frame->world_height;
}

//FIXTHIS put into dedicated src file for projection
void Get_Flat_Projection_Vectors()
{
	if(!frame)return;
	int i, j, k;
	float ow = Screenwidth();
	float oh = Screenheight();
	
	ReSizeGLScene(frame->width, frame->height);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	Get_Matrices();
	float rx, ry;
	float lo[3];
	float hi[3];
	float sdepth = 1;
	for(i = 0;i<frame->width;i++)
	{
		for(j = 0;j<frame->height;j++)
		{
			rx = i;
			ry = j;
			k = (j*frame->width)+i;
			float *pos = frame->Get_Identity_Vector(k);
			Get_World_Pos((int)rx, (int)ry, 1, pos);
			if(i==0&&j==0)
			{
				frame->min_world_depth = frame->max_world_depth = pos[2];
				lo[0] = hi[0] = pos[0];
				lo[1] = hi[1] = pos[1];
				lo[2] = hi[2] = pos[2];
			}
			else
			{
				if(frame->min_world_depth>pos[2])frame->min_world_depth = pos[2];
				if(frame->max_world_depth<pos[2])frame->max_world_depth = pos[2];
				if(lo[0]>pos[0])lo[0] = pos[0];
				if(lo[1]>pos[1])lo[1] = pos[1];
				if(lo[2]>pos[2])lo[2] = pos[2];
				if(hi[0]<pos[0])hi[0] = pos[0];
				if(hi[1]<pos[1])hi[1] = pos[1];
				if(hi[2]<pos[2])hi[2] = pos[2];
			}
			SetLength(pos, 0.001f);
		}
	}
	frame->world_width = hi[0]-lo[0];
	frame->world_height = hi[1]-lo[1];
	frame->world_depth = hi[2]-lo[2];
	glPopMatrix();
	ReSizeGLScene(ow, oh);
}

bool Get_Spherical_Projection(int x, int y, float *res)
{
	float center[3];
	center[0] = ((float)frame->width)*0.5f;
	center[1] = 0;
	center[2] = ((float)frame->height)*0.5f;
	float pixel_pos[3] = {x, 0, y};
	float diff[3] = {center[0]-pixel_pos[0],center[1]-pixel_pos[1],center[2]-pixel_pos[2]};
	float d = VecLength(diff);
	float p = d/center[0];
	float angle = p*90;
	angle *= RAD;
	float arc = cos(angle);
	res[0] = diff[0];
	res[1] = -frame->width*arc;
	res[2] = diff[2];
	return true;
}

void Get_Dome_Projection_Vectors()
{
	Get_Flat_Projection_Vectors();
	if(!frame)return;
	int i, j, k;
	for(i = 0;i<frame->width;i++)
	{
		for(j = 0;j<frame->height;j++)
		{
			k = (j*frame->width)+i;
			float *pos = frame->Get_Identity_Vector(k);
			Get_Spherical_Projection(i, j, pos);
			SetLength(pos, 0.1f);
			float *npos = frame->Get_Pos(i, j);
			npos[0] = pos[0];
			npos[1] = pos[1];
			npos[2] = pos[2];
		}
	}
}

bool Get_Cylinder_Projection(int x, int y, float *res)
{
	float p = (((float)x)/(frame->width-1))*360;
	if(x==frame->width-1)p = 0;
	p*=RAD;
	res[0] = cos(p)/2;
	res[1] = 0.5f-(((float)y/(frame->height-1)));
	res[2] = sin(p)/2;
	return true;
}

void Get_Cylinder_Projection_Vectors()
{
	if(!frame)return;
	int i, j, k;
	for(i = 0;i<frame->width;i++)
	{
		for(j = 0;j<frame->height;j++)
		{
			k = (j*frame->width)+i;
			float *pos = frame->Get_Identity_Vector(k);
			Get_Cylinder_Projection(i, j, pos);
			float *npos = frame->Get_Pos(i, j);
			npos[0] = pos[0];
			npos[1] = pos[1];
			npos[2] = pos[2];
		}
	}
}

bool Free_Frame()
{
	if(frame)
	{
		Allocate_Frame_Layers(0);
		delete frame;
	}
	frame = 0;
	frame_loaded = false;
	Set_Edit_Context();
	Free_GL_Texture(frame_texture);
	Free_GL_Texture(edge_texture);
	frame_texture = -1;
	edge_texture = -1;
	Set_GLContext();
	Set_Edit_Frame_Size(0, 0);
	Free_Relief_Images();
	Clear_Feature_Points();
	Clear_Virtual_Camera_Info();
	Free_Background_Meshes();
	return true;
}

bool TestFind_Frame_Image(char *file)
{
	char tfile[512];
	if(!Exists(file))
	{
		if(!Try_Alternate_Image_Paths(file, tfile))
		{
			return false;
		}
	}
	return true;
}



//single session OR for first frame of sequence
bool Load_Frame_Image(char *file, bool default_layer)
{
	if(!Exists(file))
	{
		if(!Try_Alternate_Image_Paths(file, file))
		{
			SkinMsgBox(0, "Can't load frame image!", file, MB_OK);
		}
	}
	Free_Frame();
	Allocate_Frame_Layers(1);
	Set_Active_Frame_Layers(0, false);
	frame = new FRAME;
	strcpy(frame->image_file, file);
	int w, h;
	Print_Status("Loading image data %s", file);
	unsigned char *rgb = Get_RGB_Image_Data(file, &w, &h);
	if(!rgb)
	{
		char msg[512];
		sprintf(msg, "Error loading image %s", file);
		Print_Status(msg);
		Print_To_Console(msg);
		SkinMsgBox(0, "Error loading image!", file, MB_OK);
		return false;
	}
	frame->original_rgb = rgb;
	Set_Current_Tracking_Frame(rgb, w, h);
	frame->contrast_map = new float[w*h];
	frame->edge_image = new unsigned char[w*h*3];
	Print_Status("Finding edges");
	Set_Edit_Context();
	Free_GL_Texture(frame_texture);
	Free_GL_Texture(edge_texture);
	Print_Status("Creating textures");
	frame_texture = Create_Frame_Texture(w, h, rgb);
	edge_texture = -1;
	update_2d_view_texture = false;
	if(view_edges)
	{
		Update_Edge_Texture();
		edit_texture = edge_texture;
	}
	Set_Edit_Frame_Size(w, h);
	Set_GLContext();
	frame->width = w;
	frame->height = h;
	Print_Status("Allocating data");
	int total = frame->width*frame->height;

	frame->pixels = new FRAME_PIXEL[total];
	frame->vertices = new float[total*3];
	frame->selection_buffer = new bool[total];
	
	frame->identity_vectors = new float[total*3];

	if(keep_original_identity_vectors)
	{
		frame->frame_vectors = new float[total*3];
	}
	else
	{
		frame->frame_vectors = frame->identity_vectors;
	}
	frame->rgb_colors = new float[total*3];

	int i, j, k;

	frame->min_pixel_depth = 0.960930f;
	frame->max_pixel_depth = 0.965854f;
	float r = frame->max_pixel_depth-frame->min_pixel_depth;
	float scale = 7.74f;
	r *= scale;
	float default_depth = frame->min_pixel_depth+(r/2);
	
	int default_layer_id = 1;
	if(default_layer)
	{
		Init_Default_Layer();
		Find_Layer("Default Layer", &default_layer_id);
	}

	Print_Status("Copying image data..");
	for(j = 0;j<frame->height;j++)
	{
		for(i = 0;i<frame->width;i++)
		{
			k = (j*frame->width)+i;
			frame->pixels[k].contour_ratio = 0;
			frame->pixels[k].relief = 0;
			frame->pixels[k].x = i;
			frame->pixels[k].y = j;
			frame->Set_Pixel_Layer(i, j, default_layer_id);
			frame->rgb_colors[(k*3)] = ((float)rgb[(k*3)])/255;
			frame->rgb_colors[(k*3)+1] = ((float)rgb[(k*3)+1])/255;
			frame->rgb_colors[(k*3)+2] = ((float)rgb[(k*3)+2])/255;
		}
	}

	Update_Layer_List();

	Print_Status("Getting initial projection..");
	if(projection_type==FLAT_PROJECTION)
	{
		Get_Flat_Projection_Vectors();
	}
	else if(projection_type==DOME_PROJECTION)
	{
		Get_Dome_Projection_Vectors();
	}
	else if(projection_type==CYLINDER_PROJECTION)
	{
		Get_Cylinder_Projection_Vectors();
	}

	if(default_layer)
	{
		Set_Default_Frame_Transform();
	}

	redraw_frame = true;
	redraw_edit_window = true;

	float ow = Screenwidth();
	float oh = Screenheight();
	ReSizeGLScene(Frame_Width(), Frame_Height());
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	Get_Matrices();
	float depth = 0.5f;
	r = frame->max_pixel_depth-frame->min_pixel_depth;
	scale = 7.74f;
	r *= scale;
	depth = frame->min_pixel_depth+(depth*r);
	scene_center[0] = 0;
	scene_center[1] = 0;
	scene_center[2] = 0;
	glPopMatrix();
	ReSizeGLScene(ow, oh);

	Print_Status("Updating layer data.");
	Update_All_Layer_Data();
	Project_All_Layers(NEW_FRAME_LAYER_DEPTH);
	Reset_Default_Layer_Depth();
	Center_Edit_View();
	Print_Status("Done.");
	frame_loaded = true;
	update_edge_texture = true;
	if(view_edges||edge_detection_enabled)Update_Edge_Texture();
	ReCalc_Focal_Plane_Preview();
	Center_View();
	return true;
}

void ReCalc_Center()
{
	if(frame)
	{
		frame->Get_Average_Center(scene_center);
	}
}


bool Replace_RGB_Values(unsigned char *rgb)
{
	int total = frame->width*frame->height;
	for(int i = 0;i<total;i++)
	{
		float *frgb = frame->Get_RGB(i);
		frgb[0] = ((float)rgb[(i*3)])/255;
		frgb[1] = ((float)rgb[(i*3)+1])/255;
		frgb[2] = ((float)rgb[(i*3)+2])/255;
	}
	return true;
}

bool Restore_Original_Frame_Texture()
{
	if(!frame)
	{
		update_2d_view_texture = false;
		return false;
	}
	Set_Edit_Context();
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, frame_texture);
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, frame->width, frame->height, GL_RGB, GL_UNSIGNED_BYTE, frame->original_rgb);
	Set_GLContext();
	redraw_edit_window = true;
	update_2d_view_texture = false;
	return true;
}

bool Update_Current_Frame_Texture()
{
	if(!frame)
	{
		update_2d_view_texture = false;
		return false;
	}
	Set_Edit_Context();
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, frame_texture);
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, frame->width, frame->height, GL_RGB, GL_FLOAT, frame->rgb_colors);
	Set_GLContext();
	redraw_edit_window = true;
	update_2d_view_texture = false;
	return true;
}

bool Replace_Frame_Image(char *file)
{
	int w, h;
	char text[256];
	if(!Replace_RGB_Image_Data(frame->original_rgb, frame->width, frame->height, file, &w, &h))
	{
		char msg[512];
		sprintf(msg, "Error loading image %s", file);
		Print_Status(msg);
		Print_To_Console(msg);
		SkinMsgBox(0, "Error loading image!", file, MB_OK);
		return false;
	}
	if(w!=frame->width||h!=frame->height)
	{
		sprintf(text, "Image dimensions don't match!\nCurrent frame size: %ix%i\nNew image size: %i %i", frame->width, frame->height, w, h);
		SkinMsgBox(0, text, 0, MB_OK);
		return false;
	}
	Set_Current_Tracking_Frame(frame->original_rgb, w, h);
	update_2d_view_texture = true;
	if(show_edit_view)
	{
		Restore_Original_Frame_Texture();
	}
	Replace_RGB_Values(frame->original_rgb);
	redraw_frame = true;
	strcpy(frame->image_file, file);
	update_edge_texture = true;
	if(view_edges||edge_detection_enabled)Update_Edge_Texture();
	return true;
}

bool Free_Frames()
{
	Free_Frame();
	Free_Layers();
	return true;
}

bool Get_Frame_Pixel_Layer(int x, int y, int *layer)
{
	if(!frame)
	{
		*layer = -1;
		return false;
	}
	*layer = frame->Get_Pixel_Layer(x, y);
	return (*layer!=-1);
}

int Frame_Width()
{
	if(!frame)return 0;
	return frame->width;
}

int Frame_Height()
{
	if(!frame)return 0;
	return frame->height;
}

void Fill_In_Header(FRAME_INFO_HEADER *p)
{
	p->version =  SFI_VERSION;
	p->width = frame->width;
	p->height = frame->height;
	strcpy(p->image_file, frame->image_file);
	p->camera_pos[0] = frame->view_origin[0];
	p->camera_pos[1] = frame->view_origin[1];
	p->camera_pos[2] = frame->view_origin[2];
	p->camera_rot[0] = frame->view_rotation[0];
	p->camera_rot[1] = frame->view_rotation[1];
	p->camera_rot[2] = frame->view_rotation[2];
	p->eye_separation = eye_separation;
	p->focal_length = focal_length;
	p->aperture = aperture;
	p->depth_scale =  depth_scale;
	p->num_layers = Num_Layers();
	p->preview_skip = puff_preview_spacing;
	p->speckle_skip = speckle_skip;
	p->anaglyph_mode = anaglyph_mode;
	p->render_borders = render_borders;
	p->render_normals = render_normals;
	p->view_edges = view_edges;
	p->edge_detection = edge_detection_enabled;
	p->edge_detection_range = edge_detection_range;
	p->edge_detection_threshold = edge_detection_threshold;
	p->cull_backfaces = cull_backfaces;
}

//used by sequence projects and single frame sessions
//single frame sessions keep camera views, sequences save it with the project
bool Save_Frame_Info(char *file, bool save_camera_views)
{
	FILE *f = fopen(file, "wb");
	FRAME_INFO_HEADER header;
	Fill_In_Header(&header);
	fwrite(&header, sizeof(FRAME_INFO_HEADER), 1, f);
	Save_Relief_Images(f);
	Save_Layers(f);
	int code = 42;
	bool b = false;
	fwrite(&b, sizeof(bool), 1, f);
	fwrite(&code, sizeof(code), 1, f);
	if(save_camera_views)Save_Camera_Views(f);
	fclose(f);
	session_altered = false;
	strcpy(frame->filename, file);
	return true;
}


//need to import supporting data
bool Import_Scene_Primitives(char *file);

//used to import resolution-independent touchups
bool Clear_All_TouchUp_Data();
bool Load_TouchUps(char *file);
bool Refresh_Layers_And_Apply_Touchups();


bool Import_Touchups(char *file)
{
	char tufile[512];
	strcpy(tufile, file);
	char *c = strrchr(tufile, '.');
	if(!c)
	{
		return false;
	}
	strcpy(c, ".tch");
	Clear_All_TouchUp_Data();
	Load_TouchUps(tufile);
	return true;
}



//called when importing resolution-independent edits to a higher res version of the same session
bool Import_Layer_Edits(char *file)
{
	Print_Status("Loading %s", file);
	FRAME_INFO_HEADER header;
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		SkinMsgBox(0, "Can't open file for reading!", file, MB_OK);
		return false;
	}

	fread(&header, sizeof(FRAME_INFO_HEADER), 1, f);

	if(header.version!=SFI_VERSION)
	{
		SkinMsgBox("Error importing layer edits from file, version mismatch!", "This never happens.");
		fclose(f);
		return false;
	}

	header.anaglyph_mode = false;

	aperture = _fov = DEFAULT_FOV = header.aperture;
	Get_Flat_Projection_Vectors();
	Set_Projection_Dlg_FOV(_fov);

	Set_Frame_Transform(header.camera_pos, header.camera_rot);

	char *c = strrchr(file, '\\');
	Print_Status("Importing layer edits from %s", &c[1]);

	Skip_Relief_Images(f);
	Import_Layer_Edits(f, header.num_layers);

	fclose(f);
	Update_Pixels_Layer_IDs();
	Import_Scene_Primitives(file);
	Import_Touchups(file);
	Print_Status("Projecting layers..");
	Refresh_Layers_And_Apply_Touchups();
	
	eye_separation = header.eye_separation;
	depth_scale = header.depth_scale;
	focal_length = header.focal_length;
	Update_Stereo_Sliders();

	Print_Status("Done.");
	return true;
}


//called when changing frames in sequence project
bool Load_Layers_Only(char *file)
{
	Print_Status("Loading %s", file);
	char text[256];
	char msg[512];
	FRAME_INFO_HEADER header;
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		SkinMsgBox(0, "Can't open file for reading!", file, MB_OK);
		return false;
	}

	fread(&header, sizeof(FRAME_INFO_HEADER), 1, f);

	if(header.version!=SFI_VERSION)
	{
		SkinMsgBox("Error loading layers from file, version mismatch!", "This never happens.");
		fclose(f);
		return false;
	}

	header.anaglyph_mode = false;

	if(frame->width!=header.width||frame->height!=header.height)
	{
		sprintf(msg, "The image file:\n\"%s\"\n does not the resolution of the session file.\nExpected Resolution %i %i\nImage Resolution %i %i", text, header.width, header.height, frame->width, frame->height);
		SkinMsgBox(0, msg, "Error loading session!", MB_OK);
		fclose(f);
		Free_Frame();
		Free_Layers();
		Clear_Console();
		Print_To_Console("Error loading session \"%s\"", file);
		return false;
	}

	Update_Contour_Dialog();

	char *c = strrchr(file, '\\');
	Print_Status("Pre-Loading layer data from %s", &c[1]);

	Skip_Relief_Images(f);
	Load_Layers(f, header.num_layers);

	fclose(f);
	Update_All_Layer_Data();
	redraw_frame = true;
	selection_changed = true;
	Update_Selected_Layers();
	Print_Status("Done.");
	if(header.version==2)//temp fix for existing content with overlapping RLE strips
	{
		//fix start/end separation between rle strips
		Resample_Strips();
		Update_All_Layer_Data();
	}
	return true;
}

bool Load_Temp_Layers_Only(char *file)
{
	Print_Status("Loading %s", file);
	char text[256];
	char msg[512];
	FRAME_INFO_HEADER header;
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		SkinMsgBox(0, "Can't open file for reading!", file, MB_OK);
		return false;
	}

	fread(&header, sizeof(FRAME_INFO_HEADER), 1, f);

	if(header.version!=SFI_VERSION)
	{
		SkinMsgBox("Error loading layers from file, version mismatch!", "This never happens.");
		fclose(f);
		return false;
	}

	header.anaglyph_mode = false;

	if(frame->width!=header.width||frame->height!=header.height)
	{
		sprintf(msg, "The image file:\n\"%s\"\n does not the resolution of the session file.\nExpected Resolution %i %i\nImage Resolution %i %i", text, header.width, header.height, frame->width, frame->height);
		SkinMsgBox(0, msg, "Error loading session!", MB_OK);
		fclose(f);
		Free_Frame();
		Free_Layers();
		Clear_Console();
		Print_To_Console("Error loading session \"%s\"", file);
		return false;
	}

	char *c = strrchr(file, '\\');
	Print_Status("Pre-Loading layer data from %s", &c[1]);

	Skip_Relief_Images(f);
	Load_Layers(f, header.num_layers);

	fclose(f);
	redraw_frame = true;
	Print_Status("Done.");
	if(header.version==2)//temp fix for existing content with overlapping RLE strips
	{
		//fix start/end separation between rle strips
		Resample_Strips();
		Update_All_Layer_Data();
	}
	return true;
}

bool Find_Alternate_Image_Path(char *test_file)
{
	char apath[512];
	char msg[512];
	if(!BrowseForFolder(apath, "Select an alternate folder for the frame images."))
	{
		return false;
	}
	Register_Alternate_Image_Path(apath);
	if(!TestFind_Frame_Image(test_file))
	{
		char file[512];
		Get_File_From_Full_Path(test_file, file);
		sprintf(msg, "ERROR! The specified folder:\n\n\"%s\"\n\ndoes not contain the image file:\n\n\"%s\"\n\nreferenced in the project.\n", apath, file);
		SkinMsgBox(0, msg, "Image not found!", MB_OK);
		return false;
	}
	return true;
}

bool Load_First_Project_Frame_Image(char *file)
{
	char msg[512];
	if(!TestFind_Frame_Image(file))
	{
		sprintf(msg, "The image file:\n\n\"%s\"\n\ncannot be located. Browse for alternate image path?", file);
		if(SkinMsgBox(0, msg, "Image Not Found", MB_YESNO)==IDNO)
		{
			return false;
		}
		if(!Find_Alternate_Image_Path(file))
		{
			return false;
		}
	}
	if(!Load_Frame_Image(file, false))
	{
		sprintf(msg, "Error loading image %s", file);
		Print_Status(msg);
		Print_To_Console(msg);
		SkinMsgBox(0, file, "Error loading image!", MB_OK);
		return false;
	}
	return true;
}


//header image file has full path, check local folder
bool Check_Local_Folder_For_Image(char *file)
{
	char nfile[512];
	char *c = strrchr(file, '\\');
	if(!c)
	{
		return false;
	}
	strcpy(nfile, &c[1]);
	if(Exists(nfile))
	{
		strcpy(file, nfile);
		return true;
	}
	return false;
}

//called for single image sessions only
bool Load_Frame_Info(char *file)
{
	Print_Status("Loading %s", file);
	char text[256];
	char msg[512];
	FRAME_INFO_HEADER header;

	FILE *f = fopen(file, "rb");
	if(!f)
	{
		SkinMsgBox(0, "Can't open file for reading!", file, MB_OK);
		return false;
	}

	fread(&header, sizeof(FRAME_INFO_HEADER), 1, f);

	if(header.version!=SFI_VERSION)
	{
		SkinMsgBox("Error loading layers from file, version mismatch!", "This never happens.");
		fclose(f);
		return false;
	}

	header.anaglyph_mode = false;

	if(!Exists(header.image_file))
	{
		if(!Check_Local_Folder_For_Image(header.image_file))
		{
			sprintf(msg, "The image file:\n\"%s\"\n cannot be located.\nBrowse for the file?", header.image_file);
			if(SkinMsgBox(0, msg, "Image Not Found", MB_YESNO)==IDNO)
			{
				return false;
			}
			if(!Browse(header.image_file, "*", false))
			{
				fclose(f);
				return false;
			}
		}
	}
	
	aperture = _fov = DEFAULT_FOV = header.aperture;

	if(!Load_Frame_Image(header.image_file, false))
	{
		char msg[512];
		sprintf(msg, "Error loading image %s", header.image_file);
		Print_Status(msg);
		Print_To_Console(msg);
		SkinMsgBox(0, header.image_file, "Error loading image!", MB_OK);
		fclose(f);
		return false;
	}
	if(frame->width!=header.width||frame->height!=header.height)
	{
		sprintf(msg, "The image file:\n\"%s\"\n does not the resolution of the session file.\nExpected Resolution %i %i\nImage Resolution %i %i", text, header.width, header.height, frame->width, frame->height);
		SkinMsgBox(0, msg, "Error loading session!", MB_OK);
		fclose(f);
		Free_Frame();
		Free_Layers();
		Clear_Console();
		Print_To_Console("Error loading session \"%s\"", file);
		return false;
	}

	Set_Frame_Transform(header.camera_pos, header.camera_rot);

	//WHY
	Update_Contour_Dialog();

	Print_Status("Loading layer data");
	Load_Relief_Images(f);
	
	Load_Layers(f, header.num_layers);

	int code = 42;
	bool b = true;
	fread(&b, sizeof(bool), 1, f);
	fread(&code, sizeof(code), 1, f);
	if(code!=42)
	{
		SkinMsgBox(0, "The file appears to be corrupted.", "ERROR READING FILE!", MB_OK);
		fclose(f);
		Free_Frame();
	}
	Load_Camera_Views(f);
	
	fclose(f);

	eye_separation = header.eye_separation;
	depth_scale = header.depth_scale;
	focal_length = header.focal_length;
	Update_Stereo_Sliders();

	Update_All_Layer_Data();
	redraw_frame = true;
	Check_For_Contoured_Layers();
	Update_Layers_Relief(false);
	selection_changed = true;
	Update_Selected_Layers();
	session_altered = false;
	Display_FOV(_fov);
	strcpy(frame->filename, file);
	Print_Status("Done.");
	if(header.version==2)
	{
		//fix start/end separation between rle strips
		Resample_Strips();
		Update_All_Layer_Data();
	}
	Load_Scene_Primitives(false);
	Report_Loading_Single_Frame_Session();
	Project_Layers();
	Apply_All_TouchUps();
	Center_View();
	return true;
}

bool Load_Stereo_Info(char *file)
{
	FRAME_INFO_HEADER header;
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		SkinMsgBox(0, "Can't open file for reading!", file, MB_OK);
		return false;
	}
	fread(&header, sizeof(FRAME_INFO_HEADER), 1, f);
	fclose(f);
	if(header.version!=SFI_VERSION)
	{
		SkinMsgBox("Error loading stereo settings from file, version mismatch!", "This never happens.");
		return false;
	}
	eye_separation = header.eye_separation;
	focal_length = header.focal_length;
	aperture = header.aperture;
	depth_scale = header.depth_scale;
	Update_Stereo_Sliders();
	fclose(f);
	return true;
}

bool Get_Frame_Pixel_Depth(int x, int y, float *res)
{
	if(frame)
	{
		if(frame->IsValidPixel(x, y))
		{
			float *pos = frame->Get_Pos(x, y);
			*res = MATH_UTILS::Distance3D(pos, frame->view_origin);
			return true;
		}
	}
	return false;
}

bool Set_Frame_Pixel_Depth(int x, int y, float d)
{
	if(frame)
	{
		return frame->Set_Pixel_Depth(x, y, d);
	}
	return false;
}

bool Get_Frame_Pixel_RGB(int x, int y, float *rgb)
{
	if(frame)
	{
		if(frame->IsValidPixel(x, y))
		{
			float *p = frame->Get_RGB(x,y);
			rgb[0] = p[0];
			rgb[1] = p[1];
			rgb[2] = p[2];
			return true;
		}
	}
	return false;
}

bool Set_Frame_Pixel_RGB(int x, int y, float *rgb)
{
	if(frame)
	{
		if(frame->IsValidPixel(x, y))
		{
			float *p = frame->Get_RGB(x,y);
			p[0] = rgb[0];
			p[1] = rgb[1];
			p[2] = rgb[2];
			return true;
		}
	}
	return false;
}


bool Get_Frame_Pixel_XYZ(int x, int y, float *xyz)
{
	if(frame)
	{
		if(frame->IsValidPixel(x, y))
		{
			float *p = frame->Get_Pos(x,y);
			xyz[0] = p[0];
			xyz[1] = p[1];
			xyz[2] = p[2];
			return true;
		}
	}
	return false;
}


bool Set_Frame_Pixel_XYZ(int x, int y, float *xyz)
{
	if(frame)
	{
		if(frame->IsValidPixel(x, y))
		{
			float *p = frame->Get_Pos(x,y);
			p[0] = xyz[0];
			p[1] = xyz[1];
			p[2] = xyz[2];
			return true;
		}
	}
	return false;
}


bool Get_Frame_RGB_Buffer(float *rgb)
{
	if(frame)
	{
		memcpy(rgb, frame->rgb_colors, sizeof(float)*frame->width*frame->height*3);
		return true;
	}
	return false;
}

bool Get_Frame_XYZ_Buffer(float *xyz)
{
	if(frame)
	{
		memcpy(xyz, frame->vertices, sizeof(float)*frame->width*frame->height*3);
		return true;
	}
	return false;
}

bool Get_Vector_From_Camera_Origin(int x, int y, float *xyz)
{
	if(frame)
	{
		if(frame->IsValidPixel(x,y))
		{
			memcpy(xyz, frame->Get_Frame_Vector(x, y), sizeof(float)*3);
			return true;
		}
	}
	return false;
}

bool Set_Vector_From_Camera_Origin(int x, int y, float *xyz)
{
	if(frame)
	{
		if(frame->IsValidPixel(x,y))
		{
			memcpy(frame->Get_Frame_Vector(x, y), xyz, sizeof(float)*3);
			return true;
		}
	}
	return false;
}


//get a grid of depth values for the specified pixel range in the supplied buffer
bool Get_Depth_Grid(int startx, int starty, int width, int height, float *grid)
{
	if(!frame)return false;
	int i, j;
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			grid[(j*width)+i] = frame->Get_Pixel_Depth(startx+i, starty+j);
		}
	}
	return true;
}

//set a grid of depth values for the specified pixel range
bool Set_Depth_Grid(int startx, int starty, int width, int height, float *grid)
{
	if(!frame)return false;
	int i, j;
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			frame->Set_Pixel_Depth(startx+i, starty+j, grid[(j*width)+i]);
		}
	}
	return true;
}

//set a grid of depth values for the specified pixel range ONLY if the pixel is selected
bool Set_Selected_Depth_Grid(int startx, int starty, int width, int height, float *grid)
{
	if(!frame)return false;
	frame->Validate_Selection_Buffer();
	int i, j;
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			if(frame->Pixel_Is_Selected(startx+i, starty+j))
			{
				frame->Set_Pixel_Depth(startx+i, starty+j, grid[(j*width)+i]);
			}
		}
	}
	return true;
}

//get a grid of rgb values for the specified pixel range in the supplied buffer
bool Get_RGB_Grid24(int startx, int starty, int width, int height, float *grid)
{
	if(!frame)return false;
	int i, j;
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			frame->Get_Pixel_RGB(startx+i, starty+j, &grid[((j*width)+i)*3]);
		}
	}
	return true;
}

//get a grid of rgb values for the specified pixel range in the supplied buffer
bool Get_RGB_Grid32(int startx, int starty, int width, int height, float *grid)
{
	if(!frame)return false;
	int i, j;
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			frame->Get_Pixel_RGB(startx+i, starty+j, &grid[((j*width)+i)*4]);
		}
	}
	return true;
}

//set a grid of rgb values for the specified pixel range
bool Set_RGB_Grid24(int startx, int starty, int width, int height, float *grid)
{
	if(!frame)return false;
	int i, j;
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			frame->Set_Pixel_RGB(startx+i, starty+j, &grid[((j*width)+i)*3]);
		}
	}
	return true;
}

//set a grid of rgb values for the specified pixel range
bool Set_RGB_Grid32(int startx, int starty, int width, int height, float *grid)
{
	if(!frame)return false;
	int i, j;
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			frame->Set_Pixel_RGB(startx+i, starty+j, &grid[((j*width)+i)*4]);
		}
	}
	return true;
}

//get a grid of xyz values for the specified pixel range in the supplied buffer
bool Get_XYZ_Grid(int startx, int starty, int width, int height, float *grid)
{
	if(!frame)return false;
	int i, j;
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			frame->Get_Pixel_XYZ(startx+i, starty+j, &grid[((j*width)+i)*3]);
		}
	}
	return true;
}

//set a grid of xyz values for the specified pixel range
bool Set_XYZ_Grid(int startx, int starty, int width, int height, float *grid)
{
	if(!frame)return false;
	int i, j;
	for(i = 0;i<width;i++)
	{
		for(j = 0;j<height;j++)
		{
			frame->Set_Pixel_XYZ(startx+i, starty+j, &grid[((j*width)+i)*3]);
		}
	}
	return true;
}



