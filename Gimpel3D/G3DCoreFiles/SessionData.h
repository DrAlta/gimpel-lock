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
#ifndef SESSION_DATA_H
#define SESSION_DATA_H

#include <vector>

using namespace std;

class FRAME_INFO
{
public:
	FRAME_INFO()
	{
		filename[0] = 0;
		layerfile[0] = 0;
		depthfile[0] = 0;
	}
	~FRAME_INFO()
	{
	}
	char filename[512];
	char layerfile[512];
	char depthfile[512];
	bool keyframe;
	int next_keyframe, previous_keyframe;
	float interpolation_percent;
};

class SHOT_INFO
{
public:
	SHOT_INFO()
	{
		Clear();
	}
	~SHOT_INFO()
	{
	}
	bool Clear()
	{
		frames.clear();
		return true;
	}
	void Update_Interpolation_Rates()
	{
		int n = frames.size();
		for(int i = 0;i<n;i++)
		{
			frames[i].next_keyframe = Get_Next_Keyframe(i);
			frames[i].previous_keyframe = Get_Previous_Keyframe(i);
			frames[i].interpolation_percent = ((float)(i-frames[i].previous_keyframe))/(frames[i].next_keyframe-frames[i].previous_keyframe);
		}
	}
	bool Add_Frame(char *filename, bool keyframe, char *layer, char *depth)
	{
		FRAME_INFO fi;
		strcpy(fi.filename, filename);
		if(layer)
		{
			strcpy(fi.layerfile, layer);
		}
		if(depth)
		{
			strcpy(fi.depthfile, depth);
		}
		fi.keyframe = keyframe;
		frames.push_back(fi);
		return true;
	}
	int Get_Next_Keyframe(int start)
	{
		int n = frames.size();
		for(int i = start+1;i<n;i++)
		{
			if(frames[i].keyframe)
			{
				return i;
			}
		}
		return -1;
	}
	int Get_Previous_Keyframe(int start)
	{
		for(int i = start-1;i>-1;i--)
		{
			if(frames[i].keyframe)
			{
				return i;
			}
		}
		return -1;
	}
	vector<FRAME_INFO> frames;
};


class PROJECT_INFO
{
public:
	PROJECT_INFO()
	{
	}
	~PROJECT_INFO()
	{
	}
	bool Save(FILE *f)
	{
		return false;
	}
	bool Load(FILE *f)
	{
		return true;
	}
	bool Check_Completed()
	{
		return false;
	}
	vector<SHOT_INFO*> shots;
	char name[128];
	char *description;
	bool completed;
};





#endif