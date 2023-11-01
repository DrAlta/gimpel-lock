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
#include "G3DCoreInterface.h"
#include "Plugins.h"
#include "Skin.h"
#include "G3DCoreFiles/Frame.h"
#include "G3DCoreFiles/Layers.h"
#include <vector>


//this file contains all core interfaces exposed to plugins
//all functionality is defined in "G3DPluginInterfaces.h"
//the classes are instanced and implemented here
//a single function is called at init to register all interfaces
//before any plugins are loaded



//all things needed tolink up functions
#include "GeometryTool/GLBasic.h"

#include "G3DCoreFiles/Image.h"

void Set_Camera(float *pos, float *rot);
void Get_Camera(float *pos, float *rot);
void Get_Camera_Vectors(float *side, float *up, float *front);
bool Save_Camera_View(int n);
bool Set_Saved_Camera_View(int n);
void Push_3D_View();
void Pop_3D_View();
void Get_3D_View(float *rot, float *center, float *zoom_out);
void Set_3D_View(float *rot, float *center, float zoom_out);
void Pan_View(float dx, float dy);
bool Valid_Path(char *path);
bool Find_Files_In_Path(char *path, vector<char*> *files);
bool Get_File_From_Full_Path(char *path, char *file);
bool Remove_File_From_Full_Path(char *path);
bool Is_Supported_Image_Format(char *file);

bool Get_Frame_Pixel_Depth(int x, int y, float *res);
bool Set_Frame_Pixel_Depth(int x, int y, float d);
bool Get_Frame_Pixel_RGB(int x, int y, float *rgb);
bool Set_Frame_Pixel_RGB(int x, int y, float *rgb);
bool Get_Frame_Pixel_XYZ(int x, int y, float *xyz);
bool Set_Frame_Pixel_XYZ(int x, int y, float *xyz);
bool Get_Frame_RGB_Buffer(float *rgb);
bool Get_Frame_XYZ_Buffer(float *xyz);

bool Get_Vector_From_Camera_Origin(int x, int y, float *xyz);
bool Set_Vector_From_Camera_Origin(int x, int y, float *xyz);

bool Project_Is_Loaded();

bool Clear_Console();

int List_Select_Dialog(vector<char*> *items, char *title);

bool Set_VC_Frame_Transform(int frame, float *pos, float *rot);
bool Get_VC_Frame_Transform(int frame, float *pos, float *rot);

bool Keyframe_Stereo_Settings(int frame_index, bool keyframe);

void Set_Focal_Length(int frame, float v);
void Set_Depth_Scale(int frame, float v);
void Set_Eye_Separation(int frame, float v);

float Get_Focal_Length(int frame);
float Get_Depth_Scale(int frame);
float Get_Eye_Separation(int frame);

bool Reset_Default_Stereo_Settings(int frame);

bool Get_Left_Raycast_Map(float *res);
bool Get_Right_Raycast_Map(float *res);
bool Get_Pixel_3D_Raycast_Hits(int width, int height, bool *hits, float *ray_hits);

bool Expand_Hit_Gaps_Leftward(bool *hits, int width, int height);
bool Expand_Hit_Gaps_Rightward(bool *hits, int width, int height);
bool Get_Pixel_Gap_Size_Map(int width, int height, bool *hits, float *buffer);



bool Get_Frame_Layer_Map(unsigned char *res);
bool Get_Frame_Depth_Map(unsigned char *res);

int Edit_Window_Width();
int Edit_Window_Height();

bool Get_Current_Edit_View(float *xpos, float *ypos, float *zoom);
bool Set_Current_Edit_View(float xpos, float ypos, float zoom);
bool Get_Pixel_At_Screen_Position(int sx, int sy, float *px, float *py);

bool Center_Edit_View();
float Pixel_Size();

int Screenwidth();
int Screenheight();

void Rotate_3D_View(int dx, int dy);
void Zoom_3D_View(int d);

bool Get_Left_View_Gapmask(unsigned char *rgb);
bool Get_Right_View_Gapmask(unsigned char *rgb);

bool Anti_Alias_Image(unsigned char *rgb, bool *mask, int width, int height);

bool Get_Depth_Grid(int startx, int starty, int width, int height, float *grid);
bool Set_Depth_Grid(int startx, int starty, int width, int height, float *grid);

bool Set_Selected_Depth_Grid(int startx, int starty, int width, int height, float *grid);


bool Get_RGB_Grid24(int startx, int starty, int width, int height, float *grid);
bool Get_RGB_Grid32(int startx, int starty, int width, int height, float *grid);
bool Set_RGB_Grid24(int startx, int starty, int width, int height, float *grid);
bool Set_RGB_Grid32(int startx, int starty, int width, int height, float *grid);

bool Get_XYZ_Grid(int startx, int starty, int width, int height, float *grid);
bool Set_XYZ_Grid(int startx, int starty, int width, int height, float *grid);

bool Get_Session_Aux_Data_Filename(char *res, char *extension);
bool Get_Project_Aux_Data_Filename(char *res, char *extension);

bool Get_Frame_Session_Filename(char *res);

bool Replace_Extension(char *file, char *extension);
bool Get_Extension(char *file, char *res);

void Set_All_Layers_Visible();
void Set_All_Layers_Invisible();

bool Get_Layer_Geometry_Type(int index, int *type);
bool Get_Selection_Extents(int *lx, int *ly, int *hx, int *hy);
void Update_GUI_Selection_Info();

bool Get_Num_Layer_Outline_Pixels(int index, int *res);
bool Get_Layer_Outline_Pixel(int index, int pixel, int *x, int *y);
bool Get_Num_Layer_Pixels(int index, int *res);
bool Get_Layer_Pixel(int index, int pixel, int *x, int *y);

bool Get_Layer_AABB(int index, int *lx, int *ly, int *hx, int *hy);

////////////////////////////////////////////////////////////////////////
//"G3DSharedDataInterface"

//This is covered in the "SharedData.cpp" file

////////////////////////////////////////////////////////////////////////
//"G3DSkinInterface"

COLORREF color;

HBRUSH G3DSkinInterface::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)//||nCtlColor==CTLCOLOR_STATIC)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}

//	unsigned int id = pWnd->GetDlgCtrlID();
//	if(nCtlColor==CTLCOLOR_STATIC)
//	{
//		pDC->SetTextColor(RGB(255,0,0));
//		pDC->SetBkColor(RGB(0,255,0));
//		return skin_static_background_brush_color;
//	}
	//else background
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
}

CFont* G3DSkinInterface::Get_Font()
{
	return skin_font;
}

CWnd* G3DSkinInterface::Create_Plugin_Skin_Frame(CWnd *sf, CWnd *child)
{
	return ::Create_Plugin_Skin_Frame(sf, child);
}

CWnd* G3DSkinInterface::Get_Plugin_Skin_Frame()
{
	return ::Get_Plugin_Skin_Frame();
}

bool G3DSkinInterface::Remove_Plugin_Skin_Frame(CWnd *sf)
{
	return ::Remove_Plugin_Skin_Frame(sf);
}

void G3DSkinInterface::DrawSkinnedButton(CButton *button, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CSkinButton::DrawSkinnedButton(button, lpDrawItemStruct);
}

void G3DSkinInterface::DrawSkinnedSlider(NMHDR *pNotifyStruct, LRESULT* result)
{
	::DrawSkinnedSlider(pNotifyStruct, result);
}

int G3DSkinInterface::SkinMsgBox(char *text, char *title, int type)
{
	return ::SkinMsgBox(text, title, type);
}

int G3DSkinInterface::SkinMsgBox(char *text)
{
	return ::SkinMsgBox(text);
}

int G3DSkinInterface::SkinMsgBox(int window, char *text, char *title, int type)
{
	return ::SkinMsgBox(window, text, title, type);
}

int G3DSkinInterface::SkinMsgBox(char *text, char *title)
{
	return ::SkinMsgBox(text, title);
}

////////////////////////////////////////////////////////////////////////
//"G3DStereoSettings"

void G3DStereoSettings::Set_Eye_Separation(int frame, float v)
{
	Set_Eye_Separation(frame, v);
}

void G3DStereoSettings::Set_Focal_Length(int frame, float v)
{
	::Set_Focal_Length(frame, v);
}

void G3DStereoSettings::Set_Depth_Scale(int frame, float v)
{
	::Set_Depth_Scale(frame, v);
}

float G3DStereoSettings::Get_Eye_Separation(int frame)
{
	return ::Get_Eye_Separation(frame);
}

float G3DStereoSettings::Get_Focal_Length(int frame)
{
	return ::Get_Focal_Length(frame);
}

float G3DStereoSettings::Get_Depth_Scale(int frame)
{
	return ::Get_Depth_Scale(frame);
}

bool G3DStereoSettings::Keyframe_Stereo_Settings(int frame, bool keyframe)
{
	return ::Keyframe_Stereo_Settings(frame, keyframe);
}

bool G3DStereoSettings::Stereo_Settings_Keyframed(int frame)
{
	return ::Stereo_Settings_Keyframed(frame);
}

bool G3DStereoSettings::Reset_Default_Stereo_Settings(int frame)
{
	return ::Reset_Default_Stereo_Settings(frame);
}


////////////////////////////////////////////////////////////////////////
//"G3DProjectInformation"

//interface to control frames in the project


bool G3DProjectInformation::Project_Is_Loaded()
{
	return ::Project_Is_Loaded();
}

int G3DProjectInformation::Get_Num_Frames()
{
	return ::Get_Num_Frames();
}

int G3DProjectInformation::Get_Current_Project_Frame()
{
	return ::Get_Current_Project_Frame();
}

bool G3DProjectInformation::Advance_Project_Frame()
{
	return ::Advance_Project_Frame();
}

bool G3DProjectInformation::Advance_Project_Frames(int n)
{
	return ::Advance_Project_Frames(n);
}

bool G3DProjectInformation::Set_Project_Frame(int f)
{
	return ::Set_Project_Frame(f);
}

bool G3DProjectInformation::Get_Frame_Name(int index, char *res)
{
	return ::Get_Frame_Name(index, res);
}

bool G3DProjectInformation::Get_Project_Name(char *res)
{
	return ::Get_Project_Name(res);
}

bool G3DProjectInformation::Create_New_Project(char *file)
{
	return ::Create_New_Project(file);
}

bool G3DProjectInformation::Load_Project(char *file)
{
	return ::Load_Project(file, false);
}

bool G3DProjectInformation::Free_Project()
{
	return ::Free_Project();
}

bool G3DProjectInformation::Save_Project(char *file)
{
	return ::Save_Project(file);
}

bool G3DProjectInformation::Get_Project_Path(char *path)
{
	return ::Get_Project_Path(path);
}

bool G3DProjectInformation::Get_Frame_Session_Aux_Data_Filename(char *res, char *extension)
{
	return ::Get_Session_Aux_Data_Filename(res, extension);
}

bool G3DProjectInformation::Get_Project_Aux_Data_Filename(char *res, char *extension)
{
	return ::Get_Project_Aux_Data_Filename(res, extension);
}

bool G3DProjectInformation::Get_Frame_Session_Filename(char *res)
{
	return ::Get_Frame_Session_Filename(res);
}

////////////////////////////////////////////////////////////////////////
//"G3DFrameInterface"

//interface to set and get frame data for the current frame

int G3DFrameInterface::Frame_Width()
{
	return ::Frame_Width();
}

int G3DFrameInterface::Frame_Height()
{
	return ::Frame_Height();
}

bool G3DFrameInterface::Get_Pixel_RGB(int x, int y, float *rgb)
{
	return ::Get_Frame_Pixel_RGB(x, y, rgb);
}

bool G3DFrameInterface::Get_Pixel_XYZ(int x, int y, float *xyz)
{
	return ::Get_Frame_Pixel_XYZ(x, y, xyz);
}

bool G3DFrameInterface::Get_Frame_RGB_Buffer(float *rgb)
{
	return ::Get_Frame_RGB_Buffer(rgb);
}

bool G3DFrameInterface::Get_Frame_XYZ_Buffer(float *xyz)
{
	return ::Get_Frame_XYZ_Buffer(xyz);
}

float G3DFrameInterface::Get_Pixel_Depth(int x, int y)
{
	if(!frame)
	{
		return -1;
	}
	return frame->Get_Pixel_Depth(x, y);
}

bool G3DFrameInterface::Get_Pixel_Layer_ID(int x, int y, int *res)
{
	if(frame)
	{
		if(frame->IsValidPixel(x, y))
		{
			*res = frame->Get_Pixel_Layer(x, y);
			return true;
		}
	}
	return false;
}

bool G3DFrameInterface::Get_Pixel_Depth_Grid(int startx, int starty, int width, int height, float *grid)
{
	return ::Get_Depth_Grid(startx, starty, width, height, grid);
}

bool G3DFrameInterface::Get_Pixel_RGB_Grid(int startx, int starty, int width, int height, float *grid)
{
	return ::Get_RGB_Grid24(startx, starty, width, height, grid);
}

bool G3DFrameInterface::Get_Pixel_XYZ_Grid(int startx, int starty, int width, int height, float *grid)
{
	return ::Get_XYZ_Grid(startx, starty, width, height, grid);
}

bool G3DFrameInterface::Get_Vector_From_Camera_Origin(int x, int y, float *xyz)
{
	return ::Get_Vector_From_Camera_Origin(x, y, xyz);
}

bool G3DFrameInterface::Get_Anaglyph_Frame(unsigned char *rgb)
{
	return ::Get_Anaglyph_Frame(rgb);
}

bool G3DFrameInterface::Get_Left_Frame(unsigned char *rgb)
{
	return ::Get_Left_Frame(rgb);
}

bool G3DFrameInterface::Get_Right_Frame(unsigned char *rgb)
{
	return ::Get_Right_Frame(rgb);
}

bool G3DFrameInterface::Get_Stereo_Frame(unsigned char *rgb)
{
	return ::Get_Stereo_Frame(rgb);
}

bool G3DFrameInterface::Get_Left_Raycast_Map(float *res)
{
	return ::Get_Left_Raycast_Map(res);
}

bool G3DFrameInterface::Get_Right_Raycast_Map(float *res)
{
	return ::Get_Right_Raycast_Map(res);
}

bool G3DFrameInterface::Get_Pixel_3D_Raycast_Hits(int width, int height, bool *hits, float *ray_hits)
{
	return Get_Pixel_3D_Raycast_Hits(width, height, hits, ray_hits);
}

bool G3DFrameInterface::Expand_Hit_Gaps_Leftward(bool *hits, int width, int height)
{
	return Expand_Hit_Gaps_Leftward(hits, width, height);
}

bool G3DFrameInterface::Expand_Hit_Gaps_Rightward(bool *hits, int width, int height)
{
	return Expand_Hit_Gaps_Rightward(hits, width, height);
}

bool G3DFrameInterface::Get_Pixel_Gap_Size_Map(int width, int height, bool *hits, float *buffer)
{
	return Get_Pixel_Gap_Size_Map(width, height, hits, buffer);
}

bool G3DFrameInterface::Set_Pixel_RGB(int x, int y, float *rgb)
{
	return ::Set_Frame_Pixel_RGB(x, y, rgb);
}

bool G3DFrameInterface::Set_Pixel_XYZ(int x, int y, float *xyz)
{
	return ::Set_Frame_Pixel_XYZ(x, y, xyz);
}

bool G3DFrameInterface::Set_Pixel_Depth(int x, int y, float d)
{
	return ::Set_Frame_Pixel_Depth(x, y, d);
}

bool G3DFrameInterface::Set_Pixel_Depth_Grid(int startx, int starty, int width, int height, float *grid)
{
	return ::Set_Depth_Grid(startx, starty, width, height, grid);
}

bool G3DFrameInterface::Set_Pixel_RGB_Grid(int startx, int starty, int width, int height, float *grid)
{
	return ::Set_RGB_Grid24(startx, starty, width, height, grid);
}

bool G3DFrameInterface::Set_Pixel_XYZ_Grid(int startx, int starty, int width, int height, float *grid)
{
	return ::Set_XYZ_Grid(startx, starty, width, height, grid);
}

bool G3DFrameInterface::Set_Vector_From_Camera_Origin(int x, int y, float *xyz)
{
	return ::Set_Vector_From_Camera_Origin(x, y, xyz);
}

bool G3DFrameInterface::Get_Frame_Colored_Layermask(float *rgb)
{
	if(!frame)return false;
	unsigned char *data = new unsigned char[frame->width*frame->height*3];
	Get_Frame_Layer_Map(data);
	Convert_Image_To_Float(rgb, data, frame->width*frame->height);
	delete[] data;
	return true;
}

bool G3DFrameInterface::Get_Frame_Grayscale_Depthmask(float *rgb)
{
	if(!frame)return false;
	unsigned char *data = new unsigned char[frame->width*frame->height*3];
	Get_Frame_Depth_Map(data);
	Convert_Image_To_Float(rgb, data, frame->width*frame->height);
	delete[] data;
	return true;
}

bool G3DFrameInterface::Get_Left_View_Gapmask(unsigned char *rgb)
{
	return ::Get_Left_View_Gapmask(rgb);
}

bool G3DFrameInterface::Get_Right_View_Gapmask(unsigned char *rgb)
{
	return ::Get_Right_View_Gapmask(rgb);
}

bool G3DFrameInterface::IsValidPixel(int x, int y)
{
	if(frame)
	{
		return frame->IsValidPixel(x, y);
	}
	return false;
}

bool G3DFrameInterface::IndexToPixel(int k, int *x, int *y)
{
	if(frame)
	{
		return frame->IndexToPixel(k, x, y);
	}
	return false;
}

bool G3DFrameInterface::PixelToIndex(int x, int y, int *k)
{
	if(frame)
	{
		if(frame->IsValidPixel(x, y))
		{
			*k = (frame->width*y)+x;
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////
//"G3DVirtualCameraInterface"


bool G3DVirtualCameraInterface::Set_VC_Frame_Transform(int frame, float *pos, float *rot)
{
	return ::Set_VC_Frame_Transform(frame, pos, rot);
}

bool G3DVirtualCameraInterface::Get_VC_Frame_Transform(int frame, float *pos, float *rot)
{
	return ::Get_VC_Frame_Transform(frame, pos, rot);
}


////////////////////////////////////////////////////////////////////////
//"G3DFileUtilities"

bool G3DFileUtilities::BrowseForFolder(char *path, char *title)
{
	return ::BrowseForFolder(path, title);
}

bool G3DFileUtilities::Exists(char *file)
{
	return ::Exists(file);
}

bool G3DFileUtilities::Browse(char *res, char *ext, bool save)
{
	return ::Browse(res, ext, save);
}

bool G3DFileUtilities::Valid_Path(char *path)
{
	return ::Valid_Path(path);
}

bool G3DFileUtilities::Find_Files_In_Path(char *path, vector<char*> *files)
{
	return ::Find_Files_In_Path(path, files);
}

bool G3DFileUtilities::Find_Image_Files_In_Path(char *path, vector<char*> *files)
{
	return ::Find_Image_Files_In_Path(path, files);
}

bool G3DFileUtilities::Get_File_From_Full_Path(char *path, char *file)
{
	return ::Get_File_From_Full_Path(path, file);
}

bool G3DFileUtilities::Remove_File_From_Full_Path(char *path)
{
	return ::Remove_File_From_Full_Path(path);
}

bool G3DFileUtilities::Replace_Extension(char *file, char *extension)
{
	return ::Replace_Extension(file, extension);
}

bool G3DFileUtilities::Get_Extension(char *file, char *res)
{
	return ::Get_Extension(file, res);
}

////////////////////////////////////////////////////////////////////////
//"G3DAVIOutput"

//a simple class for saving generic AVI data

bool G3DAVIOutput::Start_AVI_File(const char *fn, int width, int height)
{
	return ::Start_AVI_File(fn, width, height);
}

bool G3DAVIOutput::Add_AVI_Frame(unsigned char *data)
{
	return ::Add_AVI_Frame(data);
}

bool G3DAVIOutput::Close_AVI()
{
	return ::Close_AVI();
}


////////////////////////////////////////////////////////////////////////
//"G3DMiscUtilities"

void G3DMiscUtilities::Print_Status(const char *fmt, ...)
{
	char text[512];
	va_list		ap;
	if (fmt == NULL)return;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);
	::Print_Status(text);
}

bool G3DMiscUtilities::Clear_Console()
{
	return ::Clear_Console();
}

bool G3DMiscUtilities::Print_To_Console(const char *fmt, ...)
{
	char text[512];
	va_list		ap;
	if (fmt == NULL)return false;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);
	return ::Print_To_Console(text);
}

int G3DMiscUtilities::List_Select_Dialog(vector<char*> *items, char *title)
{
	return ::List_Select_Dialog(items, title);
}

HWND G3DMiscUtilities::Get_3D_Window()
{
	return ::Get_3D_Window();
}

////////////////////////////////////////////////////////////////////////
//"G3DImageUtilities"

bool G3DImageUtilities::Is_Supported_Image_Format(char *file)
{
	return ::Is_Supported_Image_Format(file);
}

unsigned char* G3DImageUtilities::Get_Image_Data(char *file, int *width, int *height, int *bits)
{
	return ::Get_Image_Data(file, width, height, bits);
}

unsigned char* G3DImageUtilities::Get_RGB_Image_Data(char *file, int *width, int *height)
{
	return ::Get_RGB_Image_Data(file, width, height);
}

unsigned char* G3DImageUtilities::Get_Grayscale_Image_Data(char *file, int *width, int *height, int *bits)
{
	return ::Get_Grayscale_Image_Data(file, width, height, bits);
}

bool G3DImageUtilities::Save_RGB_Image(unsigned char *rgb, int w, int h, char *file)
{
	return ::Save_Image(rgb, w, h, file);
}

bool G3DImageUtilities::Save_Grayscale_Image(unsigned char *img, int w, int h, char *file)
{
	return ::Save_Grayscale_Image(img, w, h, file);
}

bool G3DImageUtilities::Grayscale_To_RGB(unsigned char *dst, unsigned char *src, int num)
{
	return ::Grayscale_To_RGB(dst, src, num);
}

bool G3DImageUtilities::RGB_To_Grayscale(unsigned char *dst, unsigned char *src, int num)
{
	return ::RGB_To_Grayscale(dst, src, num);
}

bool G3DImageUtilities::RGB_To_Single_Channel(unsigned char *dst, unsigned char *src, int num, int channel)
{
	return ::RGB_To_Single_Channel(dst, src, num, channel);
}

bool G3DImageUtilities::RGB_To_RGBA(unsigned char *dst, unsigned char *src, int num)
{
	return ::RGB_To_RGBA(dst, src, num);
}

bool G3DImageUtilities::Convert_RGB_Image_To_Float(float *dst, unsigned char *src, int total)
{
	return ::Convert_Image_To_Float(dst, src, total);
}

bool G3DImageUtilities::Anti_Alias_Image(unsigned char *rgb, bool *mask, int width, int height)
{
	return ::Anti_Alias_Image(rgb, mask, width, height);
}


////////////////////////////////////////////////////////////////////////
//"G3DMathUtilities"

float G3DMathUtilities::Dist_To_Line(float *p, float *tp, float *dir)
{
	return MATH_UTILS::Dist_To_Line(p, tp, dir);
}

bool G3DMathUtilities::IsEven(int n)
{
	return MATH_UTILS::IsEven(n);
}

float G3DMathUtilities::Dot(float *a, float *b)
{
	return MATH_UTILS::Dot(a, b);
}

void G3DMathUtilities::Cross(float *v1, float *v2, float *v3)
{
	MATH_UTILS::Cross(v1, v2, v3);
}

float G3DMathUtilities::Distance3D(float *v1, float *v2)
{
	return MATH_UTILS::Distance3D(v1, v2);
}

float G3DMathUtilities::Distance2D(float x1, float y1, float x2, float y2)
{
	return MATH_UTILS::Distance2D(x1, y1, x2, y2);
}

float G3DMathUtilities::VecLength(float *v)
{
	return MATH_UTILS::VecLength(v);
}

void G3DMathUtilities::SetLength(float *v, float s)
{
	MATH_UTILS::SetLength(v, s);
}

void G3DMathUtilities::Normalize(float *v)
{
	MATH_UTILS::Normalize(v);
}

float G3DMathUtilities::RF(float max)
{
	return MATH_UTILS::RF(max);
}

float G3DMathUtilities::NRF(float max)
{
	return MATH_UTILS::NRF(max);
}

float G3DMathUtilities::Get_Random_Value(int v)
{
	return MATH_UTILS::Get_Random_Value(v);
}

float G3DMathUtilities::Dist_To_Plane(float *p, float *tp, float *n)
{
	return MATH_UTILS::Dist_To_Plane(p, tp, n);
}

void G3DMathUtilities::Plane_Intersection(float *start, float *finish, float *result, float *point, float *normal)
{
	MATH_UTILS::Plane_Intersection(start, finish, result, point, normal);
}

int G3DMathUtilities::Side_Of_Plane(float *p, float *n, float *t)
{
	return MATH_UTILS::Side_Of_Plane(p, n, t);
}

////////////////////////////////////////////////////////////////////////
//"G3D2DWindow"


bool G3D2DWindow::Trigger_Redraw()
{
	redraw_edit_window = true;
	return true;
}

HWND G3D2DWindow::Get_2D_Window()
{
	return Get_2D_Window();
}

bool G3D2DWindow::Set_GL_Context()
{
	Set_Edit_Context();
	return true;
}

int G3D2DWindow::Width()
{
	return Edit_Window_Width();
}

int G3D2DWindow::Height()
{
	return Edit_Window_Height();
}

bool G3D2DWindow::Center_View()
{
	return Center_Edit_View();
}

float G3D2DWindow::Pixel_Screen_Size()
{
	return Pixel_Size();
}

bool G3D2DWindow::Get_Current_View(float *xpos, float *ypos, float *zoom)
{
	return Get_Current_Edit_View(xpos, ypos, zoom);
}

bool G3D2DWindow::Set_Current_View(float xpos, float ypos, float zoom)
{
	return Set_Current_Edit_View(xpos, ypos, zoom);
}

//bool G3D2DWindow::Get_Pixel_Screen_Position(float px, float py, float *sx, float *sy)
//{
//	return Get_Pixel_Screen_Position(px, py, sx, sy);
//}

bool G3D2DWindow::Get_Pixel_At_Screen_Position(int sx, int sy, float *px, float *py)
{
	return Get_Pixel_At_Screen_Position(sx, sy, px, py);
}

bool G3D2DWindow::Drag_Edit_Image(int x, int y)
{
	return Drag_Edit_Image(x, y);
}

bool G3D2DWindow::Scale_Edit_Image(int n)
{
	return Scale_Edit_Image(n);
}

////////////////////////////////////////////////////////////////////////
//"G3D3DWindow"

bool G3D3DWindow::Trigger_Redraw()
{
	redraw_frame = true;
	return true;
}

HWND G3D3DWindow::Get_3D_Window()
{
	return Get_3D_Window();
}

bool G3D3DWindow::Set_GL_Context()
{
	Set_GLContext();
	return true;
}

int G3D3DWindow::Width()
{
	return Screenwidth();
}

int G3D3DWindow::Height()
{
	return Screenheight();
}

bool G3D3DWindow::Center_View()
{
	::Center_View();
	return true;
}

bool G3D3DWindow::Set_Camera(float *pos, float *rot)
{
	::Set_Camera(pos, rot);
	return true;
}

bool G3D3DWindow::Get_Camera(float *pos, float *rot)
{
	::Get_Camera(pos, rot);
	return true;
}

bool G3D3DWindow::Set_Target_Camera(float *pos, float *rot)
{
	::Set_Target_Camera(pos, rot);
	return true;
}

bool G3D3DWindow::Get_Camera_Vectors(float *side, float *up, float *front)
{
	::Get_Camera_Vectors(side, up, front);
	return true;
}

bool G3D3DWindow::Save_Camera_View(int n)
{
	return ::Save_Camera_View(n);
}

bool G3D3DWindow::Set_Saved_Camera_View(int n)
{
	return ::Set_Saved_Camera_View(n);
}

bool G3D3DWindow::Push_3D_View()
{
	::Push_3D_View();
	return true;
}

bool G3D3DWindow::Pop_3D_View()
{
	::Pop_3D_View();
	return true;
}

bool G3D3DWindow::Get_3D_View(float *rot, float *center, float *zoom_out)
{
	::Get_3D_View(rot, center, zoom_out);
	return true;
}

bool G3D3DWindow::Set_3D_View(float *rot, float *center, float zoom_out)
{
	::Set_3D_View(rot, center, zoom_out);
	return true;
}

bool G3D3DWindow::Pan_View(float dx, float dy)
{
	::Pan_View(dx, dy);
	return true;
}

bool G3D3DWindow::Rotate_View(int dx, int dy)
{
	::Rotate_3D_View(dx, dy);
	return true;
}

bool G3D3DWindow::Zoom_View(int d)
{
	::Zoom_3D_View(d);
	return true;
}

////////////////////////////////////////////////////////////////////////
//"G3DLayers"

//interface to access layer data

int G3DLayers::Num_Layers()
{
	return ::Num_Layers();
}

bool G3DLayers::Layer_Name(int index, char *res)
{
	return ::Layer_Name(index, res);
}

int G3DLayers::Find_Layer_Index(char *name)
{
	return ::Find_Layer_Index(name);
}

int G3DLayers::Find_Layer_Index(int id)
{
	return ::Find_Layer_Index(id);
}

bool G3DLayers::Find_Layer(char *name, int *id)
{
	return ::Find_Layer(name, id);
}

int G3DLayers::Get_Layer_ID(int index)
{
	return ::Get_Layer_ID(index);
}

int G3DLayers::Get_Layer_Index(int layer_id)
{
	return ::Get_Layer_Index(layer_id);
}

int G3DLayers::Get_Layer_Index(int frame, int layer_id)
{
	return ::Get_Layer_Index(frame, layer_id);
}

bool G3DLayers::Project_Layers()
{
	return ::Project_Layers();
}

bool G3DLayers::ReProject_Layer(int index)
{
	return ::ReProject_Layer(index);
}

bool G3DLayers::Flag_Layer_For_ReProject(int index)
{
	return ::Flag_Layer_For_ReProject(index);
}

int G3DLayers::Num_RLE_Strips_In_Layer(int index)
{
	return ::Num_RLE_Strips_In_Layer(index);
}

bool G3DLayers::Get_RLE_Strip(int layer_index, int strip_index, int *start_x, int *end_x, int *y)
{
	RLE_STRIP *rs = ::Get_RLE_Strip(layer_index, strip_index);
	if(!rs)return false;
	*start_x = rs->start_x;
	*end_x = rs->end_x = rs->end_x;
	*y = rs->y;
	return true;
}

bool G3DLayers::Get_Num_Layer_Outline_Pixels(int index, int *res)
{
	return ::Get_Num_Layer_Outline_Pixels(index, res);
}

bool G3DLayers::Get_Layer_Outline_Pixel(int index, int pixel, int *x, int *y)
{
	return ::Get_Layer_Outline_Pixel(index, pixel, x, y);
}

bool G3DLayers::Get_Num_Layer_Pixels(int index, int *res)
{
	return ::Get_Num_Layer_Pixels(index, res);
}

bool G3DLayers::Get_Layer_Pixel(int index, int pixel, int *x, int *y)
{
	return ::Get_Layer_Pixel(index, pixel, x, y);
}

bool G3DLayers::Get_Layer_AABB(int index, int *lx, int *ly, int *hx, int *hy)
{
	return ::Get_Layer_AABB(index, lx, ly, hx, hy);
}

int G3DLayers::Get_Oldest_Selected_Layer(int *index)
{
	return ::Get_Oldest_Selected_Layer(index);
}

int G3DLayers::Get_Most_Recently_Selected_Layer(int *index)
{
	return ::Get_Most_Recently_Selected_Layer(index);
}

bool G3DLayers::Toggle_Select_Layer(int id)
{
	return ::Toggle_Select_Layer(id);
}

bool G3DLayers::Layer_Is_Selected(int index)
{
	return ::Layer_Is_Selected(index);
}

bool G3DLayers::Select_Layer(int index, bool b)
{
	return ::Select_Layer(index, b);
}

bool G3DLayers::Select_All_Layers(bool b)
{
	::Select_All_Layers(b);
	return true;
}

int G3DLayers::Num_Selected_Layers()
{
	return ::Num_Selected_Layers();
}

bool G3DLayers::Set_Selection_Transform(float *pos, float *normal, float *rotation)
{
	return ::Set_Selection_Transform(pos, normal, rotation);
}

bool G3DLayers::Get_Selection_Transform(float *pos, float *normal, float *rotation)
{
	return ::Get_Selection_Transform(pos, normal, rotation);
}

bool G3DLayers::Move_Layer_Geometry_Horizontally(int layer_index, float v, bool reproject)
{
	return ::Move_Layer_Geometry_Horizontally(layer_index, v, reproject);
}

bool G3DLayers::Move_Layer_Geometry_Vertically(int layer_index, float v, bool reproject)
{
	return ::Move_Layer_Geometry_Vertically(layer_index, v, reproject);
}

bool G3DLayers::Move_Layer_Geometry_DepthWise(int layer_index, float v, bool reproject)
{
	return ::Move_Layer_Geometry_DepthWise(layer_index, v, reproject);
}

bool G3DLayers::Scale_Layer_Geometry(int layer_index, float x, float y, float z, bool reproject)
{
	return ::Scale_Layer_Geometry(layer_index, x, y, z, reproject);
}

bool G3DLayers::Rotate_Layer_Geometry(int layer_index, float x, float y, float z, bool reproject)
{
	return ::Rotate_Layer_Geometry(layer_index, x, y, z, reproject);
}

bool G3DLayers::Project_Pixel_To_Layer(int layer_index, int px, int py, float *res)
{
	return ::Project_Pixel_To_Layer(layer_index, px, py, res);
}

bool G3DLayers::Pixel_Hits_Layer_Geometry(int layer_index, int px, int py, float *res)
{
	return ::Pixel_Hits_Layer_Geometry(layer_index, px, py, res);
}

bool G3DLayers::Freeze_Layer(int index, bool b)
{
	return ::Freeze_Layer(index, b);
}

bool G3DLayers::Hide_Layer(int index, bool b)
{
	return ::Hide_Layer(index, b);
}
	
bool G3DLayers::Outline_Layer(int index, bool b)
{
	return ::Outline_Layer(index, b);
}
	
bool G3DLayers::Speckle_Layer(int index, bool b)
{
	return ::Speckle_Layer(index, b);
}
	
bool G3DLayers::Keyframe_Layer(int index, bool b)
{
	return ::Keyframe_Layer(index, b);
}
	
bool G3DLayers::Layer_Is_Frozen(int index)
{
	return ::Layer_Is_Frozen(index);
}
	
bool G3DLayers::Layer_Is_Hidden(int index)
{
	return ::Layer_Is_Hidden(index);
}
	
bool G3DLayers::Layer_Is_Outlined(int index)
{
	return ::Layer_Is_Outlined(index);
}
	
bool G3DLayers::Layer_Is_Speckled(int index)
{
	return ::Layer_Is_Speckled(index);
}
	
bool G3DLayers::Layer_Is_Keyframed(int index)
{
	return ::Layer_Is_Keyframed(index);
}
	
bool G3DLayers::Keyframe_All_Layers()
{
	return ::Keyframe_All_Layers();
}

bool G3DLayers::Set_All_Layers_Visible(bool b)
{
	if(b)
	{
		::Set_All_Layers_Visible();
	}
	else
	{
		::Set_All_Layers_Invisible();
	}
	return true;
}

bool G3DLayers::Get_Layer_Geometry_Type(int index, int *type)
{
	return ::Get_Layer_Geometry_Type(index, type);
}

bool G3DLayers::Get_Layer_Center(int layer_id, float *pos)
{
	return ::Get_Layer_Center(layer_id, pos);
}

bool G3DLayers::Get_Selection_Extents(int *lx, int *ly, int *hx, int *hy)
{
	return ::Get_Selection_Extents(lx, ly, hx, hy);
}

bool G3DLayers::Get_Selection_Center(float *pos)
{
	return ::Get_Selection_Center(pos);
}

bool G3DLayers::Update_GUI_Selection_Info()
{
	::Update_GUI_Selection_Info();
	return true;
}



////////////////////////////////////////////////////////////////////////
//here is the full list if internal interfaces provided to plugins:

G3DSkinInterface _G3DSkinInterface;
G3DStereoSettings _G3DStereoSettings;
G3DProjectInformation _G3DProjectInformation;
G3DFrameInterface _G3DFrameInterface;
G3DVirtualCameraInterface _G3DVirtualCameraInterface;
G3DFileUtilities _G3DFileUtilities;
G3DAVIOutput _G3DAVIOutput;
G3DMiscUtilities _G3DMiscUtilities;
G3DImageUtilities _G3DImageUtilities;
G3DMathUtilities _G3DMathUtilities;
G3DMenuOptions _G3DMenuOptions;
G3D2DWindow _G3D2DWindow;
G3D3DWindow _G3D3DWindow;
G3DLayers _G3DLayers;

//the main class used to track all interfaces (core or plugin-provided)
extern G3D_INTERFACES _g3d_interfaces;


//called at init to register all core interfaces exposed to plugins
bool Register_Core_Interfaces()
{
	_g3d_interfaces.Register_Interface("G3DSkinInterface", &_G3DSkinInterface);
	_g3d_interfaces.Register_Interface("G3DStereoSettings", &_G3DStereoSettings);
	_g3d_interfaces.Register_Interface("G3DProjectInformation", &_G3DProjectInformation);
	_g3d_interfaces.Register_Interface("G3DFrameInterface", &_G3DFrameInterface);
	_g3d_interfaces.Register_Interface("G3DVirtualCameraInterface", &_G3DVirtualCameraInterface);
	_g3d_interfaces.Register_Interface("G3DFileUtilities", &_G3DFileUtilities);
	_g3d_interfaces.Register_Interface("G3DAVIOutput", &_G3DAVIOutput);
	_g3d_interfaces.Register_Interface("G3DMiscUtilities", &_G3DMiscUtilities);
	_g3d_interfaces.Register_Interface("G3DImageUtilities", &_G3DImageUtilities);
	_g3d_interfaces.Register_Interface("G3DMathUtilities", &_G3DMathUtilities);
	_g3d_interfaces.Register_Interface("G3DMenuOptions", &_G3DMenuOptions);
	_g3d_interfaces.Register_Interface("G3D2DWindow", &_G3D2DWindow);
	_g3d_interfaces.Register_Interface("G3D3DWindow", &_G3D3DWindow);
	_g3d_interfaces.Register_Interface("G3DLayers", &_G3DLayers);
	Init_Shared_Data_Interface();
	Register_Core_Shared_Data();
	return true;
}

bool Free_Interfaces()
{
	Free_All_Public_Interfaces();
	Free_Shared_Data_Interface();
	return true;
}



