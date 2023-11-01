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
#ifndef SKIN_H
#define SKIN_H


bool Init_Skin();
bool Free_Skin();

extern CBrush skin_texture_brush_color;

extern CBrush skin_background_brush_color;
extern CBrush dark_skin_background_brush_color;

extern CFont *skin_font;
extern CFont *skin_cross_out_font;

extern COLORREF skin_text_color;
extern COLORREF skin_text_bg_color;

extern COLORREF skin_button_color;
extern COLORREF skin_button_text_color;

extern COLORREF selected_menu_bg_color;
extern COLORREF unselected_menu_bg_color;
extern CBrush selected_menu_brush;
extern CBrush unselected_menu_brush;

extern COLORREF skin_button_disabled_color;
extern COLORREF skin_button_pushed_color;

extern COLORREF skin_static_background_color;
extern COLORREF skin_static_text_color;

extern COLORREF skin_checkbox_color;

extern COLORREF skin_static_text_bg_color;
extern COLORREF skin_static_text_color;
extern CBrush skin_static_background_brush_color;

extern COLORREF skin_edit_text_color;
extern COLORREF skin_edit_text_bg_color;
extern CBrush skin_edit_background_brush_color;

extern COLORREF skin_titlebar_text_color;
extern COLORREF skin_titlebar_color;

extern CPen skin_slider_pen;
extern CBrush skin_slider_brush;

#include "SkinButton.h"
#include "SkinStaticLabel.h"
#include "SkinSlider.h"

CWnd* Create_Skin_Frame(CWnd *child);
CWnd* Create_ToolSkin_Frame(CWnd *child, char *tool_text, int tooltray_control_id);

CWnd* Create_Plugin_Skin_Frame(CWnd *sf, CWnd *child);
bool Remove_Plugin_Skin_Frame(CWnd *sf);

CWnd* Get_Plugin_Skin_Frame();

bool Remove_Skinframe(CWnd *sf);

int SkinMsgBox(char *text, char *title, int type);
int SkinMsgBox(char *text);
int SkinMsgBox(int window, char *text, char *title, int type);
int SkinMsgBox(char *text, char *title);

void DrawSkinnedSlider(NMHDR *pNotifyStruct, LRESULT* result);

#endif