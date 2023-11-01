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
#ifndef PROJECT_FILE_H
#define PROJECT_FILE_H

#include <vector>

using namespace std;

bool Load_Project(char *file);
bool Create_New_Project(char *file);
bool Set_Project_Frame(int f);
bool Advance_Project_Frame();
bool Advance_Project_Frames(int n);

bool Save_Project(char *file);

int Get_Num_Frames();

bool Get_Frame_Name(int index, char *res);
bool Get_Frame_Image(int index, char *res);
bool Get_Session_Filename(int frame, char *res);

int Get_Current_Project_Frame();
bool ReLoad_Current_Project_Frame();
bool ReLoad_Current_Project_Layers();
bool Tag_All_Layers_For_Reload();

void Set_Project_Frame_Layers_Loaded();
bool Set_Project_Frame_Layers_Loaded(int index);
bool Update_Project_Frame_3D_Data();

bool Get_Project_Name(char *res);

bool Get_Project_Images_Path(char *res);
bool Get_Project_Layermasks_Path(char *res);
bool Get_Project_Depthmasks_Path(char *res);

bool Import_Project_Images(vector<char*> *files);
bool Import_Project_Layermasks(vector<char*> *files);
bool Import_Project_Depthmasks(vector<char*> *files);

bool Backup_Current_Contours();
bool Backup_All_Contours();
bool Verify_Project_Frame_Layers_Loaded(int index);


#endif