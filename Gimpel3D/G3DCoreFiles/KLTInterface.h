/*  Gimpel3D 2D/3D Stereo Converter
    Copyright (C) 2008-2011  Daniel Ren� Dever (Gimpel)

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
#ifndef KLT_INTERFACE_H
#define KLT_INTERFACE_H


bool Reset_KLT_Tracking(char *first_image_file);
bool Set_Current_Tracking_Frame(unsigned char *data, int w, int h);
bool Stop_KLT_Tracking();

bool Track_KLT_Points(vector<float> *old_pixels, vector<float> *new_pixels);


#endif