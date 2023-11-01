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
#ifndef G3D_CORE_INTERFACE
#define G3D_CORE_INTERFACE

#include "resource.h"
#include <vector>

using namespace std;

enum
{
	RELIEF_TYPE_RANDOM_NOISE,
	RELIEF_TYPE_IMAGE,
	RELIEF_TYPE_COLOR_DIFFERENCE,
	RELIEF_TYPE_BRIGHTNESS
};

struct RELIEF_INFO
{
	int type;
	float contrast;
	float contrast_slider_range;
	float tiling;
	float tiling_slider_range;
	float bias;
	unsigned char concave_color[3];
	unsigned char convex_color[3];
	float scale;
	float scale_slider_range;
	char image[256];
	bool smooth;
};

extern int edge_detection_range;
extern float edge_detection_threshold;
extern bool edge_detection_enabled;
extern int speckle_skip;
extern bool show_edit_view;


///////////////////////////////////////////////////////////
//core stuff

bool Init_G3D_Core();
bool Update_G3D_Core();
bool Free_G3D_Core();

extern bool app_running;

HWND Get_3D_Window();
HWND Get_2D_Window();
HWND Get_GapFill_Window();

int Get_Num_Frames();

bool Stereo_Settings_Keyframed(int frame_index);

bool Set_Project_Frame(int f);

bool View_Edges(bool b);

int Num_Layers();
bool Layer_Is_Selected(int index);

void Get_Layer_Relief_Info(int index, RELIEF_INFO *ri);

extern RELIEF_INFO relief_info;

bool Layer_Name(int index, char *res);

extern bool render_2d_relief_layer_split;
extern bool rotoscope_color_difference_mode;

void Clear_Layer_Split_Pixels();

extern bool redraw_frame;
extern bool render_relief_preview;

void Init_Relief_Layer_Split_Pixels();

extern bool redraw_edit_window;

extern bool pick_concave_color;
extern bool pick_convex_color;

void Update_Selection_Relief(bool update_values);
void Update_Relief_Layer_Split_Pixels();


void Set_Plane_Dialog_To_Selection();
bool Set_Selection_Plane_Origin(int type);


extern float horizontal_rotation;
extern float vertical_rotation;
extern float z_rotation;
extern float horizontal_position;
extern float vertical_position;
extern float depth_position;

extern int plane_origin_type;

extern bool selection_changed;



enum
{
	PLANE_ORIGIN_SELECTION_TOP,
	PLANE_ORIGIN_SELECTION_LEFT,
	PLANE_ORIGIN_SELECTION_CENTER,
	PLANE_ORIGIN_SELECTION_RIGHT,
	PLANE_ORIGIN_SELECTION_BOTTOM
};
int Num_Selected_Layers();

extern bool show_3d_view;
extern bool cull_backfaces;
extern bool render_borders;
extern bool render_normals;

extern int puff_preview_spacing;
extern int puff_search_range;
extern float puff_range;
extern float puff_scale;
extern float directional_puff_angle;
extern int current_puff_type;
enum
{
	PUFF_SPHERICAL,
	PUFF_LINEAR
};

enum
{
	ANAGLYPH_OUTPUT,
	STEREO_OUTPUT,
	LEFTRIGHT_OUTPUT
};
enum
{
	FLAT_PROJECTION,
	DOME_PROJECTION,
	CYLINDER_PROJECTION
};

extern bool directional_extrusion;
extern bool render_puff_preview;

void Get_Preview_Puff_Pixels();
bool Get_Selection_Puff_Info(int *search_range, float *scale, float *range, int *type, bool *directional, float *angle);

extern float last_selection_puff_angle;
bool Set_Puff_Search_Range(int range);

extern float scale_slider_range;

bool Get_Layer_Scale(int index, float *res);

extern int num_selected_scales;
extern float last_scale_slider_pos;

extern bool scale_mismatch;

extern float total_scale_adjustment;

bool Get_Frame_Name(int index, char *res);
void Get_Display_Name(char *image);
bool Get_Project_Name(char *res);
extern float eye_separation;

extern float min_es;
extern float max_es;

extern float min_fl;
extern float max_fl;

extern float min_ds;
extern float max_ds;

extern float min_dso;
extern float max_dso;
extern float focal_length;
extern float depth_scale;

extern float edge_fade;
extern bool session_altered;

extern float min_vr;
extern float max_vr;
extern float min_hr;
extern float max_hr;
extern float min_zr;
extern float max_zr;

extern float depth_increment;

extern int last_relief_scale_pos;

extern int depth_slider_range;
bool Get_Layer_Depth_ID_Float(int index, float *res);

extern int num_selected_depths;
extern float last_depth_slider_pos;

extern bool depth_mismatch;
extern float total_depth_adjustment;


extern float alignment_angle;
extern bool align_depth;
bool Start_Alignment();
bool Get_Alignment_Data();
bool Verify_Alignment_Data();
void Update_Alignment_Angle();
extern float alignment_neighbor_rotation[3];
void Set_Initial_Relative_Alignment_Rotation(float xrot, float yrot);
bool Form_Fit_To_Surroundings();


extern bool align_xrot;
extern bool align_yrot;

extern bool align_primitive;
extern bool align_primitive_scale_x;
extern bool align_primitive_scale_y;
extern bool align_primitive_scale_z;
extern bool align_primitive_rot_x;
extern bool align_primitive_rot_y;
extern bool align_primitive_rot_z;
extern bool align_primitive_move_x;
extern bool align_primitive_move_y;
extern bool align_primitive_move_z;



int Get_Most_Recently_Selected_Layer(int *index);

int Get_Oldest_Selected_Layer(int *index);
int Get_Layer_Index(int layer_id);
bool Get_Indexed_Layer_Plane(int index, float *pos, float *dir, float *rotation, float *offset, int *origin_type);

bool Get_Project_Images_Path(char *res);
bool Get_Project_Layermasks_Path(char *res);
bool Get_Project_Depthmasks_Path(char *res);

extern float alignment_neighbor_pos[3];
extern float alignment_neighbor_dir[3];
extern float alignment_neighbor_rotation[3];
extern float alignment_neighbor_offset[3];
extern int alignment_neighbor_origin_type;

extern float alignment_plane_pos[3];
extern float alignment_plane_dir[3];
extern float alignment_plane_rotation[3];
extern float alignment_plane_offset[3];
extern int alignment_plane_origin_type;

bool ReSet_Alignment_Plane();
bool Stop_Alignment();
bool Start_Vertical_Convex_Alignment();
bool Start_Vertical_Concave_Alignment();
bool Start_Horizontal_Convex_Alignment();
bool Start_Horizontal_Concave_Alignment();


bool Puff_Selection();
void Set_Spherical_Extrusion();
void Set_Linear_Extrusion();
void Set_Directional_Extrusion(bool b);
void Set_Directional_Extrusion_Angle(float angle);


extern float background_color[4];
extern float edit_background_color[4];

bool Set_Edit_Context();
bool Set_GLContext();

bool Set_Anaglyph_Mode(bool b);
void Enable_All_Borders(bool b);

extern float horizontal_view;
extern float vertical_view;
extern float view_change;
extern float zoom_change;
extern float zoom;

void Reset_Stereo();
bool Browse(char *res, char *ext, bool save);
void Set_Camera_To_Target();
bool Save_Left_Image(char *file);
bool Save_Right_Image(char *file);
bool Save_Stereo_Image(char *file);
bool Save_Anaglyph_Image(char *file);
bool Load_Stereo_Info(char *file);
bool Save_Stereogram_Image(char *file);

bool Print_To_Console(const char *fmt, ...);

void Shift_Left_Out();
void Shift_Left_In();
void Shift_Right_In();
void Shift_Right_Out();

void Shift_Top_Down();
void Shift_Top_Up();
void Shift_Bottom_Down();
void Shift_Bottom_Up();

extern float fly_speed_scale;
void Set_Selected_Layers_Depth_ID_Float(float f);
void Adjust_Selected_Layers_Depth_ID_Float(float f);

extern bool prefilter_layermasks;

bool Find_Image_Files_In_Path(char *path, vector<char*> *files);
bool Import_Project_Images(vector<char*> *files);
bool BrowseForFolder(char *path, char *title);
bool PreFilter_Path(char *path);
bool Import_Project_Layermasks(vector<char*> *files);
bool Import_Project_Depthmasks(vector<char*> *files);
void Finalize_Selection_Planar_Projection();

extern int projection_type;

extern float _fov;

extern float aperture;
extern float DEFAULT_FOV;

void Get_Flat_Projection_Vectors();
bool Project_Layers();
void Get_Dome_Projection_Vectors();
void Get_Cylinder_Projection_Vectors();
bool Set_Default_Relief_Info(RELIEF_INFO *ri);
HWND Edit_HWND();
bool Split_Selection_By_Relief();

extern bool render_layer_outlines;
extern bool render_layer_mask;
bool Load_Frame_Image(char *file, bool default_layer);
bool Replace_Frame_Image(char *file);
bool Load_Frame_Info(char *file);
bool Save_Frame_Info(char *file, bool save_camera_views);
bool PreFilter_Image(char *file);
bool Load_Layer_Mask_Image(char *file);
void Set_Global_Relief_Image(char *file);
bool Save_Layer_Map(char *file);
bool Save_Frame_Depth_Map(char *file);
void Select_All_Layers(bool b);
void Rename_Layer(int index);
bool Evaporate_Layer(int layer_id);

bool Shift_Selected_Layers_Vertically(int ydiff);

extern int alignment_layer_index;
void Set_View();
void Pan_View(float dx, float dy);

extern bool render_grid;

extern bool frame_loaded;

extern bool auto_center_stereoview;

extern bool exporting;
int Get_Current_Project_Frame();

bool Export_Frame(int index, int type, char *output_path, char *extension, char *prefix);
bool Get_Project_Path(char *path);
bool Get_Default_Output_Path(char *path);
bool Get_Project_Filename(char *file);

int Frame_Width();
int Frame_Height();
bool Start_AVI_File(const char *fn, int width, int height);
void Render_Frame_Update();
bool Get_Anaglyph_Frame(unsigned char *rgb);
bool Add_AVI_Frame(unsigned char *data);
bool Get_Stereo_Frame(unsigned char *rgb);
bool Get_Left_Frame(unsigned char *rgb);
bool Get_Right_Frame(unsigned char *rgb);
bool Close_AVI();
bool Get_Frame_Filename(char *file);

void Set_Layer_Relief_Info(int index, RELIEF_INFO *ri, bool update_values);
bool Absorb_Layer(int layer_id, int new_layer_id);
int Get_Layer_ID(int index);
bool Update_All_Layer_Data();
bool Delete_Layer(int frame, int id);
bool Delete_Layer(int id);

bool Update_Layer_Data(int id);
void Set_All_Layers_Visible();
void Set_All_Layers_Invisible();
bool Keyframe_Layer(int index, bool b);
bool Hide_Layer(int index, bool b);
bool Freeze_Layer(int index, bool b);
bool Outline_Layer(int index, bool b);
bool Speckle_Layer(int index, bool b);
bool Exists(char *file);
int Find_Layer_Index(char *name);
bool Select_Layer(int index, bool b);
void Update_Selected_States();
void Set_Selected_Layers_Scale(float scale);
void Adjust_Selected_Layers_Scale(float scale);
extern bool flip_loaded_images_vertical;
extern bool flip_loaded_images_horizontal;

extern bool flip_redblue;
extern bool flip_loaded_layers_vertical;
extern bool flip_loaded_layers_horizontal;
extern bool update_3d_data;
extern bool flip_relief_depth;
extern bool flip_relief_vertical;
extern bool flip_relief_horizontal;

extern bool click_select;
extern bool apply_post_blend;
int Get_Current_Project_Frame();
bool Create_New_Project(char *file);
bool Load_Project(char *file, bool notify_empty);
bool Free_Project();
bool Save_Project(char *file);
bool Update_Project_Frame_3D_Data();
bool Check_For_Contoured_Layers();
void Reset_Contour();
bool Backup_Current_Contours();
bool Backup_All_Contours();
bool Copy_Layers_To_All_Frames(int frame);
void Set_Project_Frame_Layers_Loaded();
bool Set_Project_Frame_Layers_Loaded(int index);
bool Copy_Layers_To_Frame(int src, int dst);
bool ReLoad_Current_Project_Frame();
bool Tag_All_Layers_For_Reload();
bool ReLoad_Current_Project_Layers();
bool Update_Layers_Relief(bool reproject);
bool Generate_Tracking_Info_For_Selection();
bool Open_Track_Outlines_Tool();
bool Clear_Tracking_Info();
bool Create_Layers_From_Tracking_Info();
bool Replace_Layers_From_Tracking_Info();
bool Advance_Project_Frame();
bool Advance_Project_Frames(int n);
bool Extra_Smooth_Tracking_Outlines();
bool ExpandContract_Outlines(float amount, bool use_brush);
bool Apply_Post_Blend();
void Free_Preview_Puff_Pixels();
bool Clear_Modified_Layers();
bool Update_Modified_Layers();

void ReCalc_Center();
void Set_View();

extern float scene_center[3];
void Enable_Window_Controls(bool b);

void Set_Target_Camera(float *pos, float *rot);

extern bool update_contours;

extern bool use_backup_contours;
void Reset_Depth_Scale();
void Set_Eye_Separation(float v);
void Set_Focal_Length(float v);
void Set_Depth_Scale(float v);

///////////////////////////////////////////////////////////
//front end stuff

//timeline slider
bool Init_Timeline_Slider();
bool Free_Timeline_Slider();
bool Set_Timeline_Ticks(int n);
bool Set_Timeline_Frame(int n);
bool Resize_Timeline_Slider();

//stereo gui
bool Open_Stereo_Settings_Dlg();
void Update_Stereo_Sliders();
void Update_Stereo_Keyframe_Buttons(int frame_index);

//shot sequence
void Reset_Play_Sequence_Button();

//scale slider
void Set_Scale_Slider_To_Selection();


void Enable_Freeze_Checkbox(bool b);
void Enable_Visible_Checkbox(bool b);
void Enable_Keyframe_Checkbox(bool b);
void Enable_Outline_Checkbox(bool b);
void Enable_Speckle_Checkbox(bool b);

void Check_Freeze_Checkbox(bool b);
void Check_Visible_Checkbox(bool b);
void Check_Keyframe_Checkbox(bool b);
void Check_Outline_Checkbox(bool b);
void Check_Speckle_Checkbox(bool b);

bool Toggle_Edge_Detection();

extern bool update_edge_texture;
bool Update_Edge_Texture();

void Check_View_Edges_Checkbox(bool b);
;

bool Merge_Selection();

void Center_View();


void Print_Status(const char *fmt, ...);

bool Check_Save_Session();
bool Update_Selection_Info();

//relief dialog
void Set_Relief_To_Selection();
bool Close_Rotoscope_Color_Difference_Dlg();
bool Open_Rotoscope_Color_Difference_Dlg();

bool Open_Relief_Map_Dlg();

void Update_Concave_Color(unsigned char *rgb);
void Update_Convex_Color(unsigned char *rgb);
void Select_Concave_Color(unsigned char *rgb);
void Select_Convex_Color(unsigned char *rgb);

//projection settings

void Display_FOV(float fov);
bool Open_Projection_Settings_Dlg();

//planar projection
bool Open_Planar_Projection_Dlg();
void Reset_Plane_Sliders();
void Update_Planar_GUI();
void Update_Plane_Title();

//import options
bool Open_Import_Options_Dialog();

//export options
bool Open_Export_Options_Dialog();

//depth
bool Open_Depth_Slider_Dlg();
void Set_Depth_Slider_To_Selection();

void Update_Depth_Slider_Position();

void Check_Render_Borders_Checkbox(bool b);
void Set_Fly_Speed_Slider(float pos);

//contour

void Update_Contour_Dialog();
bool Open_Contour_Extrusion_Dlg();
void Set_Contour_To_Selection();
void Print_Contour_Progress(char *layer_name, int percent);

//alignment
void Enable_Start_Auto_Alignment_Button(bool b);
void Get_Alignment_Error_Text(char *res);
void Set_Alignment_Error_Text(char *text);
bool Open_Alignment_Options_Dialog();

void Get_Alignment_Angle_Text(char *text);

//scale
bool Open_Scale_Slider_Dlg();

//misc
bool Get_User_Text(char *text, char *title);

bool Open_Tracking_Outline_Dlg();
bool Open_Shift_Edges_Dlg();

///////////////////////////////////////////////////

bool Quit();

extern CWnd *mainwindow;

bool Show_3D_View(bool b);
bool Show_2D_View(bool b);
bool Show_GapFill_View(bool b);
bool Fit_2D_Window();
bool Fit_3D_Window();
bool Fit_GapFill_Window();
bool Fit_Splitscreen_Window();


extern bool split_screen;
extern bool wide_screen;

extern bool fullscreen_2d;
extern bool fullscreen_3d;

extern unsigned char background_clr[3];

bool Is_First_Frame();
bool Is_Last_Frame();

extern int window_border;
extern int window_left_border;
extern int window_right_border;
extern int window_top_border;
extern int window_bottom_border;

bool Update_Layer_List();
void List_Selected_Layers();
bool Update_Selection_Info();

void Select_Frame_In_List(int frame);
bool List_Frames();

bool Init_Clock();
void Update_Clock();
float Delta_Time();

bool Get_Desktop_Dimensions(int *width, int *height);

extern bool dual_monitor;
HWND Get_3D_Frame_Window();

extern bool swap_monitors;

bool Resize_Layout();

bool Resize_Layers_Dlg();
bool Resize_Frames_Dlg();
bool Resize_Timeline();
bool Resize_Toolbar_Dlg();
bool Resize_Info_Bar_Dlg();

void Switch_View_Mode();

bool Open_3D_View_Options_Dlg();
bool Open_2D_View_Options_Dlg();

extern bool show_edit_crosshairs;

bool Update_Selection_Status_Info();
bool Update_Selection_Status_Depth(float v);
bool Update_Selection_Status_Orientation(float x, float y);
bool Update_Selection_Status_Geometry_Type(char *name);

#endif