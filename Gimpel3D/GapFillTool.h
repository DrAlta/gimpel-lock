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
#ifndef GAP_FILL_TOOL_H
#define GAP_FILL_TOOL_H



enum
{
	GAPFILL_NO_FILL,
	GAPFILL_INTERPOLATE_FILL,
	GAPFILL_DUPLICATE_FILL,
	GAPFILL_BLEND_FILL
};

bool Init_GapFill_Window(int x, int y, int w, int h);
bool Center_GapFill_View();

bool Center_BlendImage_View();

bool Free_GapFill_Window();
bool Set_GapFill_Context();

bool Set_GapFill_Frame_Size(int w, int h);
bool Set_GapFill_Blend_Image_Size(int w, int h);


bool Reset_GapFill_View();
void Clear_GapFill_Screen();
bool Render_GapFill_Window();

HDC GapFill_HDC();
HWND Get_GapFill_Window();
HGLRC GapFill_HRC();

extern bool redraw_gapfill_window;

extern float gapfill_background_color[4];

extern bool render_ghost_blend_image;
extern bool render_background_blend_image;

void Set_Left_Gap_Threshold(float v, int slider_pos);
void Set_Right_Gap_Threshold(float v, int slider_pos);

extern int left_gap_slider_pos;
extern int right_gap_slider_pos;

bool Open_GapFill_Tools();
bool Open_GapFill_Dialog();
void Update_GapFill_Slider();
void Update_GapFill_List();

bool Close_GapFill_Dialog();
bool Close_GapFill_Tools();

bool Restore_Windows_From_GapFill_Tool();

extern bool view_gapfill_left;
extern bool view_gapfill_right;
extern bool view_gapfill_anaglyph;

extern bool highlight_gaps;
extern bool show_gaps;

void View_GapFill_Left();
void View_GapFill_Right();
void View_GapFill_Anaglyph();

bool Identify_Gaps();

bool Free_Left_Gaps();
bool Free_Right_Gaps();

int Num_Left_Gaps();
int Num_Right_Gaps();

bool Select_Left_Gap(int index);
bool Select_Right_Gap(int index);

int Left_Gap_Selection();
int Right_Gap_Selection();

void Print_GapFill_Status(const char *fmt, ...);

int Get_Left_Gap_Fill_Type(int index);
int Get_Right_Gap_Fill_Type(int index);

bool Set_Left_Gap_Fill_Type(int index, int fill_type);
bool Set_Right_Gap_Fill_Type(int index, int fill_type);

bool Finalize_GapFill_Images();

bool Set_Left_Gap_Blend_Frame(int index, int frame);
bool Set_Right_Gap_Blend_Frame(int index, int frame);

bool Set_Left_Gap_Blend_Image(int index, char *file);
bool Set_Right_Gap_Blend_Image(int index, char *file);

int Get_Left_Gap_Blend_Frame(int index);
int Get_Right_Gap_Blend_Frame(int index);

bool Align_GapFill_Blend_Image();
bool Apply_GapFill_Blend_Pixels();
bool Clear_GapFill_Blend_Pixels();

bool Preview_Blended_Gap_Fill();

bool Restore_Original_GapFill_Texture();

bool Save_GapFill_Data();
bool Load_GapFill_Data();

bool Preview_GapFilled_Anaglyph();

bool Get_Left_Blended_Gap_Fill(unsigned char *new_image, bool anti_alias);
bool Get_Right_Blended_Gap_Fill(unsigned char *new_image, bool anti_alias);

#endif