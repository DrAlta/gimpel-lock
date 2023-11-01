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
#ifndef AUTO_SEGMENT_H
#define AUTO_SEGMENT_H




bool Open_AutoSegment_Tool();
bool Close_AutoSegment_Tool();

bool Open_AutoSegment_Dialog();
bool Close_AutoSegment_Dialog();


bool Auto_Segment_Cartoon_Image();


extern bool render_autosegment_state;
extern bool auto_segment_render_outlines_only;

extern float outline_darkness_threshold;
extern float outline_color_threshold;

bool Update_AutoSegment_Outline_Pixels();

bool Render_AutoSegment_Outlines();

bool Preview_AutoSegments();
bool Reset_Preview_AutoSegments();
bool Finalize_AutoSegments();
bool Generate_AutoSegment_Contrast_Map();



#endif