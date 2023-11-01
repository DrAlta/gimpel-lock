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
#ifndef RLE_CIRCLE_GRID_H
#define RLE_CIRCLE_GRID_H

#include "Frame.h"

bool Set_RLE_Circle_Grid(int range);

int Num_Circle_Grid_RLE_Strips();

RLE_STRIP* Get_Circle_Grid_RLE_Strip(int index);

bool Free_Circle_Grid_RLE_Strips();


#endif