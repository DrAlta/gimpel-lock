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
#include "Layers.h"


//SOLE purpose of this file is to resample rle strips from frame data to match up start/end
//to fix issues in SFI_VERSION 2

void Delete_All_RLE_Strips();

void Resample_Strips()
{
	Delete_All_RLE_Strips();
	int i, j;
	int layer = -1;
	int id = -1;
	int startx = 0;
	int layer_index = -1;
	for(j = 0;j<frame->height;j++)
	{
		layer = -1;
		for(i = 0;i<frame->width;i++)
		{
			id = frame->Get_Pixel_Layer(i, j);
			if(id!=layer)
			{
				layer_index = Get_Layer_Index(layer);
				if(layer_index!=-1)
				{
					Force_Layer_RLE_Strip(layer_index, j, startx, i);
				}
				layer = id;
				startx = i;
			}
		}
		layer_index = Get_Layer_Index(layer);
		if(layer_index!=-1)
		{
			Force_Layer_RLE_Strip(layer_index, j, startx, i);
		}
	}
}

