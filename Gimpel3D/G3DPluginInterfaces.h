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
#ifndef G3D_PLUGIN_INTERFACES_H
#define G3D_PLUGIN_INTERFACES_H

#include <vector>

using namespace std;

////////////////////////////////////////////////////////////////////////
//"G3DSharedDataInterface"

//this class manages "shared data" between the core and plugins

//shared data is registered with a text name and a pointer to the memory

//other plugins that need to access the data can request the pointer by 
//the shared name, this allows privtae communication between plugins as needed

//callback functions can be registered for shared data when it changes state,
//this mechanism can be used to trigger callback functions for particular events

//notifications of shared data changes can be immediately dispatched or stored
//until the next cycle

typedef bool (*_Shared_Data_Update_Callback)(void *p);

class G3DSharedDataInterface
{
public:
	G3DSharedDataInterface(){};
	~G3DSharedDataInterface(){};
	virtual int Num_Shared_Data();
	virtual bool Shared_Data_Info(int id, char *name, int *dsize);
	virtual bool Register_Shared_Data(char *name, int dsize, void *data, int *res);
	virtual bool Set_Shared_Data(int id, int dsize, void *data);
	virtual bool Find_Shared_Data(char *name, int *dsize, int *res);
	virtual bool Register_Shared_Data_Update_Callback(char *name, void *callback);
	virtual bool Register_Shared_Data_Update_Callback(int id, void *callback);
	virtual bool Notify_Shared_Data_Update(int id);//immediate
	virtual bool Register_Delayed_Shared_Data_Update(int id, float delay);//delayed
	virtual bool Get_Shared_Data(int id, void **res);
	virtual bool Get_Current_Callback_Data_Type(int *res);
};


////////////////////////////////////////////////////////////////////////
//"G3DSkinInterface"

//this class provides generic control colors for dialogs and direct access
//to the current colors and styles used for skinning..
//plugins can use the skinning style or not, plugings can operate with
//standard OR custom graphics as needed

class G3DSkinInterface
{
public:
	G3DSkinInterface(){};
	~G3DSkinInterface(){};
	//this is called for each plugin dialog to provide a generic skin for
	//the background and most common controls..
	virtual HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual CFont* Get_Font();
	virtual CWnd* Get_Plugin_Skin_Frame();
	virtual CWnd* Create_Plugin_Skin_Frame(CWnd *sf, CWnd *child);
	virtual bool Remove_Plugin_Skin_Frame(CWnd *sf);
	virtual void DrawSkinnedButton(CButton *button, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawSkinnedSlider(NMHDR *pNotifyStruct, LRESULT* result);
	virtual int SkinMsgBox(char *text, char *title, int type);
	virtual int SkinMsgBox(char *text);
	virtual int SkinMsgBox(int window, char *text, char *title, int type);
	virtual int SkinMsgBox(char *text, char *title);
};


////////////////////////////////////////////////////////////////////////
//"G3DStereoSettings"

//interface to control stereo separation and view settings for all frames

class G3DStereoSettings
{
public:
	G3DStereoSettings(){};
	~G3DStereoSettings(){};
	virtual void Set_Eye_Separation(int frame, float v);
	virtual void Set_Focal_Length(int frame, float v);
	virtual void Set_Depth_Scale(int frame, float v);
	virtual float Get_Eye_Separation(int frame);
	virtual float Get_Focal_Length(int frame);
	virtual float Get_Depth_Scale(int frame);
	virtual bool Keyframe_Stereo_Settings(int frame, bool keyframe);
	virtual bool Stereo_Settings_Keyframed(int frame);
	virtual bool Reset_Default_Stereo_Settings(int frame);
};

////////////////////////////////////////////////////////////////////////
//"G3DProjectInformation"

//interface to control frames in the project

class G3DProjectInformation
{
public:
	G3DProjectInformation(){};
	~G3DProjectInformation(){};
	virtual bool Project_Is_Loaded();
	virtual int Get_Num_Frames();
	virtual int Get_Current_Project_Frame();
	virtual bool Advance_Project_Frame();
	virtual bool Advance_Project_Frames(int n);
	virtual bool Set_Project_Frame(int f);
	virtual bool Get_Frame_Name(int index, char *res);
	virtual bool Get_Project_Name(char *res);
	virtual bool Create_New_Project(char *file);
	virtual bool Load_Project(char *file);
	virtual bool Free_Project();
	virtual bool Save_Project(char *file);
	virtual bool Get_Project_Path(char *path);
	virtual bool Get_Frame_Session_Aux_Data_Filename(char *res, char *extension);
	virtual bool Get_Project_Aux_Data_Filename(char *res, char *extension);
	virtual bool Get_Frame_Session_Filename(char *res);
};

////////////////////////////////////////////////////////////////////////
//"G3DFrameInterface"

//interface to set and get frame data for the current frame

class G3DFrameInterface
{
public:
	G3DFrameInterface(){};
	~G3DFrameInterface(){};
	virtual int Frame_Width();
	virtual int Frame_Height();
	virtual bool Get_Pixel_RGB(int x, int y, float *rgb);
	virtual bool Get_Pixel_XYZ(int x, int y, float *xyz);
	virtual bool Get_Frame_RGB_Buffer(float *rgb);
	virtual bool Get_Frame_XYZ_Buffer(float *xyz);
	virtual float Get_Pixel_Depth(int x, int y);
	virtual bool Get_Pixel_Layer_ID(int x, int y, int *res);
	virtual bool Get_Pixel_Depth_Grid(int startx, int starty, int width, int height, float *grid);
	virtual bool Get_Pixel_RGB_Grid(int startx, int starty, int width, int height, float *grid);
	virtual bool Get_Pixel_XYZ_Grid(int startx, int starty, int width, int height, float *grid);
	virtual bool Get_Vector_From_Camera_Origin(int x, int y, float *xyz);
	virtual bool Get_Anaglyph_Frame(unsigned char *rgb);
	virtual bool Get_Left_Frame(unsigned char *rgb);
	virtual bool Get_Right_Frame(unsigned char *rgb);
	virtual bool Get_Stereo_Frame(unsigned char *rgb);
	virtual bool Get_Left_Raycast_Map(float *res);
	virtual bool Get_Right_Raycast_Map(float *res);
	virtual bool Get_Pixel_3D_Raycast_Hits(int width, int height, bool *hits, float *ray_hits);
	virtual bool Expand_Hit_Gaps_Leftward(bool *hits, int width, int height);
	virtual bool Expand_Hit_Gaps_Rightward(bool *hits, int width, int height);
	virtual bool Get_Pixel_Gap_Size_Map(int width, int height, bool *hits, float *buffer);
	virtual bool Set_Pixel_RGB(int x, int y, float *rgb);
	virtual bool Set_Pixel_XYZ(int x, int y, float *xyz);
	virtual bool Set_Pixel_Depth(int x, int y, float d);
	virtual bool Set_Pixel_Depth_Grid(int startx, int starty, int width, int height, float *grid);
	virtual bool Set_Pixel_RGB_Grid(int startx, int starty, int width, int height, float *grid);
	virtual bool Set_Pixel_XYZ_Grid(int startx, int starty, int width, int height, float *grid);
	virtual bool Set_Vector_From_Camera_Origin(int x, int y, float *xyz);
	virtual bool Get_Frame_Colored_Layermask(float *rgb);
	virtual bool Get_Frame_Grayscale_Depthmask(float *rgb);
	virtual bool Get_Left_View_Gapmask(unsigned char *rgb);
	virtual bool Get_Right_View_Gapmask(unsigned char *rgb);
	virtual bool IsValidPixel(int x, int y);
	virtual bool IndexToPixel(int k, int *x, int *y);
	virtual bool PixelToIndex(int x, int y, int *k);
};

////////////////////////////////////////////////////////////////////////
//"G3DVirtualCameraInterface"

class G3DVirtualCameraInterface
{
public:
	G3DVirtualCameraInterface(){};
	~G3DVirtualCameraInterface(){};
	virtual bool Set_VC_Frame_Transform(int frame, float *pos, float *rot);
	virtual bool Get_VC_Frame_Transform(int frame, float *pos, float *rot);
};

////////////////////////////////////////////////////////////////////////
//"G3DFileUtilities"

class G3DFileUtilities
{
public:
	G3DFileUtilities(){};
	~G3DFileUtilities(){};
	virtual bool BrowseForFolder(char *path, char *title);
	virtual bool Exists(char *file);
	virtual bool Browse(char *res, char *ext, bool save);
	virtual bool Valid_Path(char *path);
	virtual bool Find_Files_In_Path(char *path, vector<char*> *files);
	virtual bool Find_Image_Files_In_Path(char *path, vector<char*> *files);
	virtual bool Get_File_From_Full_Path(char *path, char *file);
	virtual bool Remove_File_From_Full_Path(char *path);
	virtual bool Replace_Extension(char *file, char *extension);
	virtual bool Get_Extension(char *file, char *res);
};


////////////////////////////////////////////////////////////////////////
//"G3DAVIOutput"

//a simple class for saving generic AVI data

class G3DAVIOutput
{
public:
	G3DAVIOutput(){};
	~G3DAVIOutput(){};
	virtual bool Start_AVI_File(const char *fn, int width, int height);
	virtual bool Add_AVI_Frame(unsigned char *data);
	virtual bool Close_AVI();
};


////////////////////////////////////////////////////////////////////////
//"G3DMiscUtilities"

class G3DMiscUtilities
{
public:
	G3DMiscUtilities(){};
	~G3DMiscUtilities(){};
	virtual void Print_Status(const char *fmt, ...);
	virtual bool Clear_Console();
	virtual bool Print_To_Console(const char *fmt, ...);
	virtual int List_Select_Dialog(vector<char*> *items, char *title);
	virtual HWND Get_3D_Window();
};


////////////////////////////////////////////////////////////////////////
//"G3DImageUtilities"

class G3DImageUtilities
{
public:
	G3DImageUtilities(){};
	~G3DImageUtilities(){};
	virtual bool Is_Supported_Image_Format(char *file);
	virtual unsigned char* Get_Image_Data(char *file, int *width, int *height, int *bits);
	virtual unsigned char* Get_RGB_Image_Data(char *file, int *width, int *height);
	virtual unsigned char* Get_Grayscale_Image_Data(char *file, int *width, int *height, int *bits);
	virtual bool Save_RGB_Image(unsigned char *rgb, int w, int h, char *file);
	virtual bool Save_Grayscale_Image(unsigned char *img, int w, int h, char *file);
	virtual bool Grayscale_To_RGB(unsigned char *dst, unsigned char *src, int num);
	virtual bool RGB_To_Grayscale(unsigned char *dst, unsigned char *src, int num);
	virtual bool RGB_To_Single_Channel(unsigned char *dst, unsigned char *src, int num, int channel);
	virtual bool RGB_To_RGBA(unsigned char *dst, unsigned char *src, int num);
	virtual bool Convert_RGB_Image_To_Float(float *dst, unsigned char *src, int total);
	virtual bool Anti_Alias_Image(unsigned char *rgb, bool *mask, int width, int height);
};


////////////////////////////////////////////////////////////////////////
//"G3DMathUtilities"

//generic useful 3d math routines

class G3DMathUtilities
{
public:
	G3DMathUtilities(){};
	~G3DMathUtilities(){};
	virtual float Dist_To_Line(float *p, float *tp, float *dir);
	virtual bool IsEven(int n);
	virtual float Dot(float *a, float *b);
	virtual void Cross(float *v1, float *v2, float *v3);
	virtual float Distance3D(float *v1, float *v2);
	virtual float Distance2D(float x1, float y1, float x2, float y2);
	virtual float VecLength(float *v);
	virtual void SetLength(float *v, float s);
	virtual void Normalize(float *v);
	virtual float RF(float max);
	virtual float NRF(float max);
	virtual float Get_Random_Value(int v);
	virtual float Dist_To_Plane(float *p, float *tp, float *n);
	virtual void Plane_Intersection(float *start, float *finish, float *result, float *point, float *normal);
	virtual int Side_Of_Plane(float *p, float *n, float *t);
};

////////////////////////////////////////////////////////////////////////
//"G3D2DWindow

//class to access 2d window

class G3D2DWindow
{
public:
	G3D2DWindow(){}
	~G3D2DWindow(){}
	virtual bool Trigger_Redraw();
	virtual HWND Get_2D_Window();
	virtual bool Set_GL_Context();
	virtual int Width();
	virtual int Height();
	virtual bool Center_View();
	virtual float Pixel_Screen_Size();
	virtual bool Get_Current_View(float *xpos, float *ypos, float *zoom);
	virtual bool Set_Current_View(float xpos, float ypos, float zoom);
	virtual bool Get_Pixel_At_Screen_Position(int sx, int sy, float *px, float *py);
	virtual bool Drag_Edit_Image(int x, int y);
	virtual bool Scale_Edit_Image(int n);
};

////////////////////////////////////////////////////////////////////////
//"G3D3DWindow

//class to access 3d window

class G3D3DWindow
{
public:
	G3D3DWindow(){}
	~G3D3DWindow(){}
	virtual bool Trigger_Redraw();
	virtual HWND Get_3D_Window();
	virtual bool Set_GL_Context();
	virtual int Width();
	virtual int Height();
	virtual bool Center_View();
	virtual bool Set_Camera(float *pos, float *rot);
	virtual bool Get_Camera(float *pos, float *rot);
	virtual bool Set_Target_Camera(float *pos, float *rot);
	virtual bool Get_Camera_Vectors(float *side, float *up, float *front);
	virtual bool Save_Camera_View(int n);
	virtual bool Set_Saved_Camera_View(int n);
	virtual bool Push_3D_View();
	virtual bool Pop_3D_View();
	virtual bool Get_3D_View(float *rot, float *center, float *zoom_out);
	virtual bool Set_3D_View(float *rot, float *center, float zoom_out);
	virtual bool Pan_View(float dx, float dy);
	virtual bool Rotate_View(int dx, int dy);
	virtual bool Zoom_View(int d);
};

////////////////////////////////////////////////////////////////////////
//"G3DGenericCircleBrush

//class to access generic circle brush

class G3DGenericCircleBrush
{
public:
	G3DGenericCircleBrush(){};
	~G3DGenericCircleBrush(){};
	virtual bool Set_Brush_Circle_Color(float r,float g, float b, float a);
	virtual bool Set_CrossHair_Color(float r,float g, float b, float a);
	virtual bool Get_Brush_Circle_Color(float r,float g, float b, float a);
	virtual bool Get_CrossHair_Color(float r,float g, float b, float a);
	virtual bool Render_Brush_Circle();
	virtual bool Render_Brush_CrossHair();
	virtual float Brush_Size();
	virtual bool Image_Pixel_Within_Brush(int px, int py);
	virtual bool Brush_Strength_At_Image_Pixel(int px, int py);
};

////////////////////////////////////////////////////////////////////////
//"G3DLayers"

//interface to access layer data

class G3DLayers
{
public:
	G3DLayers(){};
	~G3DLayers(){};
	//query layers in list for names,ids, current index, etc.
	virtual int Num_Layers();
	virtual bool Layer_Name(int index, char *res);
	virtual int Find_Layer_Index(char *name);
	virtual int Find_Layer_Index(int id);
	virtual bool Find_Layer(char *name, int *id);
	virtual int Get_Layer_ID(int index);
	virtual int Get_Layer_Index(int layer_id);
	virtual int Get_Layer_Index(int frame, int layer_id);

	//reset/reproject layers
	virtual bool Project_Layers();
	virtual bool ReProject_Layer(int index);
	virtual bool Flag_Layer_For_ReProject(int index);

	//query associated pixel data for layer
	virtual int Num_RLE_Strips_In_Layer(int index);
	virtual bool Get_RLE_Strip(int layer_index, int strip_index, int *start_x, int *end_x, int *y);
	virtual bool Get_Num_Layer_Outline_Pixels(int index, int *res);
	virtual bool Get_Layer_Outline_Pixel(int index, int pixel, int *x, int *y);
	virtual bool Get_Num_Layer_Pixels(int index, int *res);
	virtual bool Get_Layer_Pixel(int index, int pixel, int *x, int *y);
	virtual bool Get_Layer_AABB(int index, int *lx, int *ly, int *hx, int *hy);

	//set/get query selection states
	virtual int Get_Oldest_Selected_Layer(int *index);
	virtual int Get_Most_Recently_Selected_Layer(int *index);
	virtual bool Toggle_Select_Layer(int id);
	virtual bool Layer_Is_Selected(int index);
	virtual bool Select_Layer(int index, bool b);
	virtual bool Select_All_Layers(bool b);
	virtual int Num_Selected_Layers();

	//get/set modify layer transforms
	virtual bool Set_Selection_Transform(float *pos, float *normal, float *rotation);
	virtual bool Get_Selection_Transform(float *pos, float *normal, float *rotation);

	//move attached layer geometry
	virtual bool Move_Layer_Geometry_Horizontally(int layer_index, float v, bool reproject);
	virtual bool Move_Layer_Geometry_Vertically(int layer_index, float v, bool reproject);
	virtual bool Move_Layer_Geometry_DepthWise(int layer_index, float v, bool reproject);
	virtual bool Scale_Layer_Geometry(int layer_index, float x, float y, float z, bool reproject);
	virtual bool Rotate_Layer_Geometry(int layer_index, float x, float y, float z, bool reproject);

	//ray tracing functions for generic pixels against specific layer
	virtual bool Project_Pixel_To_Layer(int layer_index, int px, int py, float *res);
	virtual bool Pixel_Hits_Layer_Geometry(int layer_index, int px, int py, float *res);

	//manage layer state flags
	virtual bool Freeze_Layer(int index, bool b);
	virtual bool Hide_Layer(int index, bool b);
	virtual bool Outline_Layer(int index, bool b);
	virtual bool Speckle_Layer(int index, bool b);
	virtual bool Keyframe_Layer(int index, bool b);
	virtual bool Layer_Is_Frozen(int index);
	virtual bool Layer_Is_Hidden(int index);
	virtual bool Layer_Is_Outlined(int index);
	virtual bool Layer_Is_Speckled(int index);
	virtual bool Layer_Is_Keyframed(int index);
	virtual bool Keyframe_All_Layers();
	virtual bool Set_All_Layers_Visible(bool b);

	//query layer geometry
	virtual bool Get_Layer_Geometry_Type(int index, int *type);
	virtual bool Get_Layer_Center(int layer_id, float *pos);
	virtual bool Get_Selection_Extents(int *lx, int *ly, int *hx, int *hy);
	virtual bool Get_Selection_Center(float *pos);

	//update the core if a plugin has modified a layer
	virtual bool Update_GUI_Selection_Info();

};


////////////////////////////////////////////////////////////////////////
//"G3DMenuOptions"

//a wrapper class for all options in the main menu

class G3DMenuOptions
{
public:
	G3DMenuOptions(){};
	~G3DMenuOptions(){};
};

#endif
