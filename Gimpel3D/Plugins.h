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
#ifndef PLUGINS_H
#define PLUGINS_H

#include "G3DPlugin.h"

bool Open_Plugins_Tool();
bool Close_Plugins_Tool();

bool Open_Plugins_Dialog();
bool Close_Plugins_Dialog();

bool Load_Plugins(char *path);
bool Unload_Plugins();


int Num_Plugins();
bool Get_Plugin_Name(int index, char *res);

bool Open_Plugin(char *name);
bool Close_All_Plugins();
bool Reload_Plugin(char *name);
bool Reload_All_Plugins();
bool Unload_Plugin(char *namet);
bool Unload_All_Plugins();
bool Load_Custom_Plugin(char *file);

bool Plugin_Loaded(char *name);

bool Free_All_Public_Interfaces();

bool Register_Core_Interfaces();
bool Free_Interfaces();

bool Init_Shared_Data_Interface();
bool Free_Shared_Data_Interface();

bool Register_Core_Shared_Data();

//for internal core code to access shared data and events
extern G3DSharedDataInterface shared_data_interface;


#endif