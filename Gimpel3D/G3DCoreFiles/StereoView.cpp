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
#include "StereoView.h"
#include "Camera.h"
#include "Frame.h"
#include "GLWnd.h"
#include "../GeometryTool/GLBasic.h"
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../Skin.h"


void SetLength(float *v, float s);

void Get_Frame_View_Vector(int axis, float *res);

extern bool render_camera;

vector<STEREO_SETTINGS> stereo_frame_settings;

bool ReCalc_Focal_Plane_Preview();

float default_eye_separation = 0.0015f;
float default_focal_length = 0.022150f;

float eye_separation = 0.0015f;
float focal_length = 0.022150f;
float aperture = 0;
float depth_scale = 1;

float min_es = 0;
float max_es = 0.01f;
float min_fl = 0.01f;
float max_fl = 0.1f;
float min_ds = 0;
float max_ds = 2;
float min_dso = 0;//used for exp scale
float max_dso = 2;

void Reset_Stereo()
{
	eye_separation = default_eye_separation;
	focal_length = default_focal_length;
	aperture = DEFAULT_FOV;
	depth_scale = 1;
	redraw_frame = true;
	ReCalc_Focal_Plane_Preview();
}

void Reset_Depth_Scale()
{
	if(depth_scale!=1&&render_camera)
	{
		ReCalc_Focal_Plane_Preview();
	}
	depth_scale = 1;
}

void Set_Eye_Separation(float v)
{
	eye_separation = v;
	redraw_frame = true;
}

void Set_Focal_Length(float v)
{
	if(v!=focal_length&&render_camera)
	{
		ReCalc_Focal_Plane_Preview();
	}
	focal_length = v;
	redraw_frame = true;
}

void Set_Focal_Length(int frame, float v)
{
	int n = stereo_frame_settings.size();
	if(frame<0||frame>=n)
	{
		focal_length = v;
		if(v!=focal_length&&render_camera)
		{
			ReCalc_Focal_Plane_Preview();
		}
		Update_Stereo_Sliders();
		redraw_frame = true;
		return;
	}
	stereo_frame_settings[frame].focal_length = v;
	if(frame==Get_Current_Project_Frame())
	{
		if(v!=focal_length&&render_camera)
		{
			ReCalc_Focal_Plane_Preview();
		}
		focal_length = v;
		Update_Stereo_Sliders();
		redraw_frame = true;
	}
}

void Set_Depth_Scale(int frame, float v)
{
	int n = stereo_frame_settings.size();
	if(frame<0||frame>=n)
	{
		if(depth_scale!=v&&render_camera)
		{
			ReCalc_Focal_Plane_Preview();
		}
		depth_scale = v;
		Update_Stereo_Sliders();
		redraw_frame = true;
		return;
	}
	stereo_frame_settings[frame].depth_scale = v;
	if(frame==Get_Current_Project_Frame())
	{
		if(depth_scale!=v&&render_camera)
		{
			ReCalc_Focal_Plane_Preview();
		}
		depth_scale = v;
		Update_Stereo_Sliders();
		redraw_frame = true;
	}
}

void Set_Eye_Separation(int frame, float v)
{
	int n = stereo_frame_settings.size();
	if(frame<0||frame>=n)
	{
		eye_separation = v;
		Update_Stereo_Sliders();
		redraw_frame = true;
		return;
	}
	stereo_frame_settings[frame].eye_separation = v;
	if(frame==Get_Current_Project_Frame())
	{
		eye_separation = v;
		Update_Stereo_Sliders();
		redraw_frame = true;
	}
}



float Get_Focal_Length(int frame)
{
	int n = stereo_frame_settings.size();
	if(frame<0||frame>=n||frame==Get_Current_Project_Frame())
	{
		return focal_length;
	}
	return stereo_frame_settings[frame].focal_length;
}

float Get_Depth_Scale(int frame)
{
	int n = stereo_frame_settings.size();
	if(frame<0||frame>=n||frame==Get_Current_Project_Frame())
	{
		return depth_scale;
	}
	return stereo_frame_settings[frame].depth_scale;
}

float Get_Eye_Separation(int frame)
{
	int n = stereo_frame_settings.size();
	if(frame<0||frame>=n||frame==Get_Current_Project_Frame())
	{
		return eye_separation;
	}
	return stereo_frame_settings[frame].eye_separation;
}

bool Reset_Default_Stereo_Settings(int frame)
{
	int n = stereo_frame_settings.size();
	if(frame<0||frame>=n)
	{
		Reset_Stereo();
		return false;
	}
	stereo_frame_settings[frame].eye_separation = default_eye_separation;
	stereo_frame_settings[frame].focal_length = default_focal_length;
	stereo_frame_settings[frame].depth_scale = 1;
	if(frame==Get_Current_Project_Frame())
	{
		Reset_Stereo();
	}
	if(render_camera)
	{
		ReCalc_Focal_Plane_Preview();
	}
	return true;
}



void Set_Aperture(float v)
{
	aperture = v;
	redraw_frame = true;
}

void Set_Depth_Scale(float v)
{
	if(v!=depth_scale&&render_camera)
	{
		ReCalc_Focal_Plane_Preview();
	}
	depth_scale = v;
	redraw_frame = true;
}

bool Stereo_Settings_Keyframed(int frame_index)
{
	int n = stereo_frame_settings.size();
	if(frame_index<0||frame_index>=n)
	{
		return false;
	}
	return stereo_frame_settings[frame_index].keyframe;
}

bool Keyframe_Stereo_Settings(int frame_index, bool keyframe)
{
	int n = stereo_frame_settings.size();
	if(frame_index<0||frame_index>=n)
	{
		return false;
	}
	stereo_frame_settings[frame_index].keyframe = keyframe;
	if(frame_index==Get_Current_Project_Frame())
	{
		Update_Stereo_Keyframe_Buttons(frame_index);
	}
	return true;
}

bool Save_Stereo_Settings(FILE *f, int num)
{
	int n = stereo_frame_settings.size();
	fwrite(&n, sizeof(int), 1, f);
	for(int i = 0;i<n;i++)
	{
		fwrite(&stereo_frame_settings[i], sizeof(STEREO_SETTINGS), 1, f);
	}
	if(n!=num)
	{
		return false;
	}
	return true;
}

bool Allocate_Stereo_Settings(int n)
{
	stereo_frame_settings.clear();
	STEREO_SETTINGS ss;
	ss.eye_separation = eye_separation;
	ss.focal_length = focal_length;
	ss.depth_scale = depth_scale;
	for(int  i = 0;i<n;i++)
	{
		if(i==0||i==n-1)
		{
			ss.keyframe = true;
		}
		else
		{
			ss.keyframe = false;
		}
		stereo_frame_settings.push_back(ss);
	}
	return true;
}

bool Load_Stereo_Settings(FILE *f, int num)
{
	int n = 0;
	fread(&n, sizeof(int), 1, f);
	if(n==0)
	{
		Allocate_Stereo_Settings(num);
		return false;
	}
	stereo_frame_settings.clear();
	STEREO_SETTINGS ss;
	for(int  i = 0;i<n;i++)
	{
		fread(&ss, sizeof(STEREO_SETTINGS), 1, f);
		stereo_frame_settings.push_back(ss);
	}
	return true;
}
bool Get_Interpolated_Stereo_Settings()
{
	int index = Get_Current_Project_Frame();
	STEREO_SETTINGS *ss = &stereo_frame_settings[index];
	if(ss->keyframe)
	{
		eye_separation = ss->eye_separation;
		focal_length = ss->focal_length;
		depth_scale = ss->depth_scale;
		Update_Stereo_Sliders();
		return true;
	}
	STEREO_SETTINGS *next = 0;
	STEREO_SETTINGS *prev = 0;
	int start = -1;
	int end = -1;
	int n = stereo_frame_settings.size();
	int i;
	for(i = index;i<n;i++)
	{
		if(stereo_frame_settings[i].keyframe)
		{
			next = &stereo_frame_settings[i];
			end = i;
			i = n;
		}
	}
	for(i = index;i>-1;i--)
	{
		if(stereo_frame_settings[i].keyframe)
		{
			prev = &stereo_frame_settings[i];
			start = i;
			i = 0;
		}
	}
	if(!next||!prev)
	{
		SkinMsgBox(0, "ERROR! Can't get surrounding keyframes for stereo settings!", "INTERNAL ERROR", MB_OK);
		return false;
	}
	float p = ((float)(index-start))/((float)(end-start));
	float ip = 1.0f-p;
	ss->eye_separation = (ip*prev->eye_separation)+(p*next->eye_separation);
	ss->focal_length = (ip*prev->focal_length)+(p*next->focal_length);
	ss->depth_scale = (ip*prev->depth_scale)+(p*next->depth_scale);
	eye_separation = ss->eye_separation;
	focal_length = ss->focal_length;
	depth_scale = ss->depth_scale;
	Update_Stereo_Sliders();
	return true;
}

void Update_Stereo_Settings(int frame_index)
{
	Update_Stereo_Keyframe_Buttons(frame_index);
	Get_Interpolated_Stereo_Settings();
}

bool Append_Single_Stereo_Setting()
{
	STEREO_SETTINGS ss;
	STEREO_SETTINGS *oss;
	int n = stereo_frame_settings.size();
	if(n>0)
	{
		oss = &stereo_frame_settings[n-1];
		ss.depth_scale = oss->depth_scale;
		ss.eye_separation = oss->eye_separation;
		ss.focal_length = oss->focal_length;
	}
	else
	{
		ss.depth_scale = depth_scale;
		ss.eye_separation = eye_separation;
		ss.focal_length = focal_length;
	}
	ss.keyframe = false;
	stereo_frame_settings.push_back(ss);
	return true;
}

bool Keyframe_FirstLast_Stereo_Setting()
{
	int n = stereo_frame_settings.size();
	if(n==0)
	{
		return false;
	}
	stereo_frame_settings[0].keyframe = true;
	stereo_frame_settings[n-1].keyframe = true;
	return true;
}


//stereo view code based on example from
//http://paulbourke.net/stereographics/stereorender/


typedef struct {
   double x,y,z;
} XYZ;

typedef struct {
   XYZ vp;              /* View position           */
   XYZ vd;              /* View direction vector   */
   XYZ vu;              /* View up direction       */
   double focallength;  /* Focal Length along vd   */
   double aperture;     /* Camera aperture         */
   double eyesep;       /* Eye separation          */
   int screenwidth,screenheight;
} __CAMERA;
#define DTOR            0.0174532925
#define CROSSPROD(p1,p2,p3) \
   p3.x = p1.y*p2.z - p1.z*p2.y; \
   p3.y = p1.z*p2.x - p1.x*p2.z; \
   p3.z = p1.x*p2.y - p1.y*p2.x
void Normalise(XYZ *p)
{
   double length;

   length = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
   if (length != 0) {
      p->x /= length;
      p->y /= length;
      p->z /= length;
   } else {
      p->x = 0;
      p->y = 0;
      p->z = 0;
   }
}
__CAMERA camera;


void Set_Left_View(int w, int h)
{
   XYZ r;
   double ratio,radians,wd2,ndfl;
   double left,right,top,bottom;

   float ___near = _near_plane;
   float ___far = _far_plane;

	camera.screenwidth = w;
	camera.screenheight = h;
	camera.vp.x = 0;
	camera.vp.y = 0;
	camera.vp.z = 0;
	camera.vd.x = 0;
	camera.vd.y = 0;
	camera.vd.z = -1;
	camera.vu.x = 0;
	camera.vu.y = 1;
	camera.vu.z = 0;

	
	camera.vp.x = camera_pos[0];
	camera.vp.y = camera_pos[1];
	camera.vp.z = camera_pos[2];
	camera.vd.x = -camera_front[0];
	camera.vd.y = -camera_front[1];
	camera.vd.z = -camera_front[2];
	camera.vu.x = camera_up[0];
	camera.vu.y = camera_up[1];
	camera.vu.z = camera_up[2];
	
	camera.focallength = focal_length*depth_scale;
	camera.aperture = aperture;
	camera.eyesep = eye_separation*depth_scale;

   /* Clip to avoid extreme stereo */
      ___near = camera.focallength / 5;

   /* Misc stuff */
   ratio  = camera.screenwidth / (double)camera.screenheight;
   radians = DTOR * camera.aperture / 2;
   wd2     = ___near * tan(radians);
   ndfl    = ___near / camera.focallength;

      /* Derive the two eye positions */
      CROSSPROD(camera.vd,camera.vu,r);
      Normalise(&r);
      r.x *= camera.eyesep / 2.0;
      r.y *= camera.eyesep / 2.0;
      r.z *= camera.eyesep / 2.0;

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      left  = - ratio * wd2 - 0.5 * camera.eyesep * ndfl;
      right =   ratio * wd2 - 0.5 * camera.eyesep * ndfl;
      top    =   wd2;
      bottom = - wd2;
      glFrustum(left,right,bottom,top,___near,___far);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera.vp.x + r.x,camera.vp.y + r.y,camera.vp.z + r.z,
	             camera.vp.x + r.x + camera.vd.x,
	             camera.vp.y + r.y + camera.vd.y,
	             camera.vp.z + r.z + camera.vd.z,
	             camera.vu.x,camera.vu.y,camera.vu.z);
}

void Set_Right_View(int w, int h)
{
   XYZ r;
   float ___near = _near_plane;
   float ___far = _far_plane;
	camera.screenwidth = w;
	camera.screenheight = h;
	camera.vp.x = 0;
	camera.vp.y = 0;
	camera.vp.z = 0;
	camera.vd.x = 0;
	camera.vd.y = 0;
	camera.vd.z = -1;
	camera.vu.x = 0;
	camera.vu.y = 1;
	camera.vu.z = 0;

	
	camera.vp.x = camera_pos[0];
	camera.vp.y = camera_pos[1];
	camera.vp.z = camera_pos[2];
	camera.vd.x = -camera_front[0];
	camera.vd.y = -camera_front[1];
	camera.vd.z = -camera_front[2];
	camera.vu.x = camera_up[0];
	camera.vu.y = camera_up[1];
	camera.vu.z = camera_up[2];
	
	camera.focallength = focal_length*depth_scale;
	camera.aperture = aperture;
	camera.eyesep = eye_separation*depth_scale;

   /* Clip to avoid extreme stereo */
      ___near = camera.focallength / 5;
   double ratio  = camera.screenwidth / (double)camera.screenheight;
   double radians = DTOR * camera.aperture / 2;
   double wd2     = ___near * tan(radians);
   double ndfl    = ___near / camera.focallength;

      /* Derive the two eye positions */
      CROSSPROD(camera.vd,camera.vu,r);
      Normalise(&r);
      r.x *= camera.eyesep / 2.0;
      r.y *= camera.eyesep / 2.0;
      r.z *= camera.eyesep / 2.0;

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      double left  = - ratio * wd2 + 0.5 * camera.eyesep * ndfl;
      double right =   ratio * wd2 + 0.5 * camera.eyesep * ndfl;
      double top    =   wd2;
      double bottom = - wd2;
      glFrustum(left,right,bottom,top,___near,___far);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(camera.vp.x - r.x,camera.vp.y - r.y,camera.vp.z - r.z,
                camera.vp.x - r.x + camera.vd.x,
                camera.vp.y - r.y + camera.vd.y,
                camera.vp.z - r.z + camera.vd.z,
                camera.vu.x,camera.vu.y,camera.vu.z);
}




bool Get_Displacement_Map(float *res)
{
	int i, j;
	for(i = 0;i<frame->width;i++)
	{
		for(j = 0;j<frame->height;j++)
		{
			float *dst = &res[(((j*frame->width)+i)*3)];
			float *pos = frame->Get_Pos(i, j);
			double wx, wy, wz;
			gluProject(pos[0], pos[1], pos[2], modelview_matrix, projection_matrix, viewport, &wx, &wy, &wz);
			dst[0] = wx;
			dst[1] = wy;
			dst[2] = wz;
		}
	}
	return true;
}


void Push_3D_View();
void Pop_3D_View();

bool Get_Left_Raycast_Map(float *res)
{

	//need centered view for this to work!
	Push_3D_View();
	Center_View();
	Set_Camera_To_Target();//snap to target camera
	Set_GL_Camera_Transform();
	Get_Matrices();
	Update_Camera_Dir();//get camera vectors
		
	glViewport(0,0,frame->width,frame->height);
	
	Set_Left_View(frame->width, frame->height);
	Get_Matrices();
	Get_Displacement_Map(res);

	//restore the current editing view
	Pop_3D_View();
	Set_3D_View();
	Set_GL_Camera_Transform();
	Get_Matrices();
	return true;
}

bool Get_Right_Raycast_Map(float *res)
{
	//need centered view for this to work!
	Push_3D_View();
	Center_View();
	Set_Camera_To_Target();//snap to target camera
	Set_GL_Camera_Transform();
	Get_Matrices();
	Update_Camera_Dir();//get camera vectors
		
	glViewport(0,0,frame->width,frame->height);
	Set_Right_View(frame->width, frame->height);
	Get_Matrices();
	Get_Displacement_Map(res);

	//restore the current editing view
	Pop_3D_View();
	Set_3D_View();
	Set_GL_Camera_Transform();
	Get_Matrices();
	return true;
}

