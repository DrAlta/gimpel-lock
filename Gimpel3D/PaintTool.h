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
#ifndef PAINT_TOOL_H
#define PAINT_TOOL_H






bool Open_Paint_Tool();
bool Close_Paint_Tool();


bool Open_Paint_Tool_Dlg();
bool Close_Paint_Tool_Dlg();


extern bool render_paint_tool;
extern bool render_paint_frame_image;
extern bool render_paint_grayscale_image;
extern bool render_paint_layermask_image;





#endif