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
#include "Clock.h"

bool clock_enabled = true;

float delta_time = 1;
float last_delta_time = 1;

LARGE_INTEGER tickspersecond;
LARGE_INTEGER currentticks;
LARGE_INTEGER framedelay;

double elapsed_time = 0;


bool Init_Clock()
{
	QueryPerformanceCounter(&framedelay);
	QueryPerformanceFrequency(&tickspersecond);
	Update_Clock();
	delta_time = 0;
	return true;
}

void Update_Clock()
{
	if(clock_enabled)
	{
		QueryPerformanceCounter(&currentticks);
		delta_time = (float)(currentticks.QuadPart-framedelay.QuadPart)/((float)tickspersecond.QuadPart);
		if(delta_time <= 0){delta_time = last_delta_time;}
		last_delta_time = delta_time;
		framedelay = currentticks;
		elapsed_time += delta_time;
	}
}

float Delta_Time()
{
	return delta_time;
}

double Elapsed_Time()
{
	return elapsed_time;
}

void Enable_Clock(bool b)
{
	clock_enabled = b;
}

void Set_Delta_Time(float dt)
{
	delta_time = dt;
}


void Record_Keystroke(int key)
{
}

