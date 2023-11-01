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
#include "LayerMask.h"
#include "Frame.h"
#include "ProjectFile.h"
#include "Image.h"
#include "Layers.h"
#include "Relief.h"
#include "../GeometryTool.h"
#include "../FeaturePoints.h"
#include <vector>
#include "../Skin.h"

bool Report_New_Project_Opened();
bool Report_Saving_Project();
bool Report_Frame_Changed();
bool Report_Frame_About_To_Change();


bool Apply_All_TouchUps();

bool Extract_Frames(char *file, char *output);
bool Browse(char *res, char *ext, bool save);
void Update_Stereo_Settings(int frame_index);
bool Load_Stereo_Settings(FILE *f, int num);
bool Save_Stereo_Settings(FILE *f, int num);
bool Append_Single_Stereo_Setting();
bool Keyframe_FirstLast_Stereo_Setting();
bool Save_Camera_Views(FILE *f);
bool Load_Camera_Views(FILE *f);

bool Load_Alternate_Image_Paths();
bool Save_Alternate_Image_Paths();

bool Save_Virtual_Camera_Info();
bool Load_Virtual_Camera_Info();

bool Set_Virtual_Camera_Frame_Transform(int frame);


bool Is_First_Or_Last_Frame();

void ReSet_View();

bool Update_Motion_Tracking();

extern bool redraw_frame;
extern bool redraw_edit_window;

extern bool update_contours;
extern bool use_backup_contours;

const int PROJECT_FILE_VERSION = 43;

using namespace std;

bool Exists(char *file);

void Set_Global_Relief_Image(char *file);
void Set_Temp_Global_Relief_Image(char *file);

int current_project_frame = 0;

int num_project_layers = 0;

char current_project_file[512] = {"NOFILE"};

bool update_3d_data = true;

bool project_loaded = false;

bool Project_Is_Loaded()
{
	return project_loaded;
}

class PROJECT_FRAME
{
public:
	PROJECT_FRAME()
	{
		image[0] = 0;
		layermask[0] = 0;
		depthmask[0] = 0;
		session_file[0] = 0;
		layers_loaded = false;
	}
	~PROJECT_FRAME()
	{
	}
	char image[512];
	char layermask[512];
	char depthmask[512];
	char session_file[512];
	bool layers_loaded;
};

vector<PROJECT_FRAME> project_frames;

bool Get_Project_Name(char *file, char *name)
{
	char *c = strrchr(file, '\\');
	if(!c)
	{
		return false;
	}
	strcpy(name, &c[1]);
	int len = strlen(name);
	name[len-4] = 0;
	return true;
}

bool Valid_Path(char *path)
{
	SetCurrentDirectory(path);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char fpath[512];
	strcpy(fpath, path);
	strcat(fpath, "\\*.*");
	hFind = FindFirstFile(fpath, &FindFileData);
	int dc = 1;
	if(hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	FindClose(hFind);
	return true;
}

bool Is_Supported_Image_Format(char *file)
{
	char *c = strrchr(file, '.');
	if(!c)
	{
		return false;
	}
	if(!strcmp(c, ".dpx"))return true;
	if(!strcmp(c, ".jpg"))return true;
	if(!strcmp(c, ".bmp"))return true;
	if(!strcmp(c, ".gif"))return true;
	if(!strcmp(c, ".tga"))return true;
	if(!strcmp(c, ".tiff"))return true;
	if(!strcmp(c, ".tif"))return true;
	if(!strcmp(c, ".png"))return true;
	if(!strcmp(c, ".pcx"))return true;
	return false;
}

bool Find_Files_In_Path(char *path, vector<char*> *files)
{
	SetCurrentDirectory(path);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError;

	char msg[512];
	char fpath[512];
	strcpy(fpath, path);

	strcat(fpath, "\\*.*");
	
	hFind = FindFirstFile(fpath, &FindFileData);
	
	int dc = 1;
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		sprintf(msg, "Invalid file handle searching for files in path \"%s\". Error is %u.\n", path, GetLastError());
		SkinMsgBox(0, msg, 0, MB_OK);
		return false;
	} 
	else 
	{
		char *c;
		if(Exists(FindFileData.cFileName))
		{
			c = new char[512];
			strcpy(c, FindFileData.cFileName);
			(*files).push_back(c);
		}
		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			if(dc==1)Print_Status("Searching folders.");
			if(dc==2)Print_Status("Searching folders..");
			if(dc==3)Print_Status("Searching folders...");
			if(dc==4)Print_Status("Searching folders....");
			if(dc==5)Print_Status("Searching folders....");
			dc++;
			if(dc==6)dc = 1;
			if(Exists(FindFileData.cFileName))
			{
				c = new char[512];
				strcpy(c, FindFileData.cFileName);
				(*files).push_back(c);
			}
		}
		
		dwError = GetLastError();
		FindClose(hFind);
		if (dwError != ERROR_NO_MORE_FILES) 
		{
			sprintf(msg, "FindNextFile error searching for files in path \"%s\". Error is %u.\n", path, dwError);
			Print_Status("Done.");
			SkinMsgBox(0, msg, 0, MB_OK);
			return false;
		}
	}
	Print_Status("Done.");
	return true;
}


//FIXTHIS abstract to generic file and path routines file
bool Find_Image_Files_In_Path(char *path, vector<char*> *files)
{
	SetCurrentDirectory(path);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError;

	char msg[512];
	char fpath[512];
	strcpy(fpath, path);

	strcat(fpath, "\\*.*");
	
	hFind = FindFirstFile(fpath, &FindFileData);
	
	int dc = 1;
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		sprintf(msg, "Invalid file handle searching for files in path \"%s\". Error is %u.\n", path, GetLastError());
		SkinMsgBox(0, msg, 0, MB_OK);
		return false;
	} 
	else 
	{
		char *c;
		if(Exists(FindFileData.cFileName)&&Is_Supported_Image_Format(FindFileData.cFileName))
		{
			c = new char[512];
			strcpy(c, FindFileData.cFileName);
			(*files).push_back(c);
		}
		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			if(dc==1)Print_Status("Searching folders.");
			if(dc==2)Print_Status("Searching folders..");
			if(dc==3)Print_Status("Searching folders...");
			if(dc==4)Print_Status("Searching folders....");
			if(dc==5)Print_Status("Searching folders....");
			dc++;
			if(dc==6)dc = 1;
			if(Exists(FindFileData.cFileName)&&Is_Supported_Image_Format(FindFileData.cFileName))
			{
				c = new char[512];
				strcpy(c, FindFileData.cFileName);
				(*files).push_back(c);
			}
		}
		
		dwError = GetLastError();
		FindClose(hFind);
		if (dwError != ERROR_NO_MORE_FILES) 
		{
			sprintf(msg, "FindNextFile error searching for files in path \"%s\". Error is %u.\n", path, dwError);
			Print_Status("Done.");
			SkinMsgBox(0, msg, 0, MB_OK);
			return false;
		}
	}
	Print_Status("Done.");
	return true;
}

bool Get_Session_Filename(char *image, char *session)
{
	strcpy(session, image);
	char *c = strrchr(session, '\\');
	if(!c)
	{
		SkinMsgBox(0, "Error getting session file name from image path!", image, MB_OK);
		return false;
	}
	char tfile[512];
	strcpy(tfile, &c[1]);
	*c = 0;
	c = strrchr(session, '\\');
	sprintf(c, "\\SessionFiles\\%s", tfile);
	c = strrchr(session, '.');
	c[1] = 's';
	c[2] = 'f';
	c[3] = 'i';
	c[4] = 0;
	return true;
}

//FIXTHIS is this even used?
bool Get_Temp_Image_Filename(char *image, char *temp_image)
{
	strcpy(temp_image, image);
	char *c = strrchr(temp_image, '\\');
	if(!c)
	{
		SkinMsgBox(0, "Error getting temp image file name from image path!", image, MB_OK);
		return false;
	}
	char tfile[512];
	strcpy(tfile, &c[1]);
	*c = 0;
	c = strrchr(temp_image, '\\');
	sprintf(c, "\\TempImages\\%s", tfile);
	c = strrchr(temp_image, '.');
	c[1] = 'j';
	c[2] = 'p';
	c[3] = 'g';
	c[4] = 0;
	return true;
}

//FIXTHIS dead code?
bool Get_Temp_Depthmask_Filename(char *image, char *temp_image)
{
	strcpy(temp_image, image);
	char *c = strrchr(temp_image, '\\');
	if(!c)
	{
		SkinMsgBox(0, "Error getting temp depthmask file name from image path!", image, MB_OK);
		return false;
	}
	char tfile[512];
	strcpy(tfile, &c[1]);
	*c = 0;
	c = strrchr(temp_image, '\\');
	sprintf(c, "\\TempDepthMasks\\%s", tfile);
	c = strrchr(temp_image, '.');
	c[1] = 'j';
	c[2] = 'p';
	c[3] = 'g';
	c[4] = 0;
	return true;
}


bool Identify_Layermask_Colors()
{
	if(project_frames[0].layermask[0]==0)
	{
		//no layermasks
		return true;
	}
	Clear_Layer_Colors();
	int n = project_frames.size();
	for(int i = 0;i<n;i++)
	{
		char *c = strrchr(project_frames[i].layermask, '\\');
		Print_Status("Loading layermask colors from %s", &c[1]);
		if(!Add_Layer_Colors_From_Mask_Image(project_frames[i].layermask))
		{
			SkinMsgBox(0, "An error occured loading the layer mask colors from the image file", project_frames[i].layermask, MB_OK);
			Clear_Layer_Colors();
			return false;
		}
	}
	num_project_layers = layer_colors.size()/3;
	return true;
}


bool Create_Duplicate_Session_Files()
{
	int n = project_frames.size();
	for(int i = 1;i<n;i++)
	{
		Print_Status("Creating session %i of %i", i, n);
		strcpy(frame->filename, project_frames[i].image);
		Update_Layer_List();
		if(project_frames[i].depthmask[0]!=0)
		{
			Set_Default_Layer_Relief_Image(project_frames[i].depthmask);
		}
		if(i==n-1)Keyframe_All_Layers();
		Save_Frame_Info(project_frames[i].session_file, false);//don't save camera views
	}
	Print_Status("Done.");
	return true;
}

bool Create_Initial_Session_Files()
{
	int n = project_frames.size();
	for(int i = 0;i<n;i++)
	{
		Reset_Next_Layer_ID();
		Load_Frame_Image(project_frames[i].image, true);
		if(project_frames[i].layermask[0]!=0)Load_Preset_Layer_Mask_Image(project_frames[i].layermask);
		Update_Layer_List();
		if(project_frames[i].depthmask[0]!=0)Set_Global_Relief_Image(project_frames[i].depthmask);
		if(i==0||i==n-1)Keyframe_All_Layers();
		Save_Frame_Info(project_frames[i].session_file, false);//don't save camera views
		if(project_frames[i].layermask[0]==0)
		{
			return Create_Duplicate_Session_Files();
		}
	}
	return true;
}

//ugh last minute fix to save fov with project header info

struct PROJECT_HEADER
{
	int version;
	int num_frames;
	float fov;
};

extern float _fov;
void Set_Fov(float fov);

bool Save_Project_Info(char *file)
{
	FILE *f = fopen(file, "wb");
	if(!f)
	{
		SkinMsgBox(0, "Can't open project file for writing!", file, MB_OK);
		return false;
	}
	int n = project_frames.size();
	PROJECT_HEADER ph;
	ph.version = PROJECT_FILE_VERSION;
	ph.num_frames = n;
	ph.fov = _fov;
	fwrite(&ph, sizeof(PROJECT_HEADER), 1, f);
	for(int i = 0;i<n;i++)
	{
		fwrite(project_frames[i].image, sizeof(char), 512, f);
		fwrite(project_frames[i].layermask, sizeof(char), 512, f);
		fwrite(project_frames[i].depthmask, sizeof(char), 512, f);
		fwrite(project_frames[i].session_file, sizeof(char), 512, f);
	}
	fwrite(&num_project_layers, sizeof(int), 1, f);
	Save_Stereo_Settings(f, n);
	Save_Camera_Views(f);
	fclose(f);
	return true;
}

bool Load_Project_Info(char *file)
{
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		SkinMsgBox(0, "Can't open project file for reading!", file, MB_OK);
		return false;
	}
	PROJECT_HEADER ph;
	fread(&ph, sizeof(PROJECT_HEADER), 1, f);
	if(ph.version!=PROJECT_FILE_VERSION)
	{
		SkinMsgBox(0, "Can't load project file, version mis-match!", "This never happens.", MB_OK);
		fclose(f);
		return false;
	}
	int n = ph.num_frames;
	for(int i = 0;i<n;i++)
	{
		PROJECT_FRAME pf;
		fread(pf.image, sizeof(char), 512, f);
		fread(pf.layermask, sizeof(char), 512, f);
		fread(pf.depthmask, sizeof(char), 512, f);
		fread(pf.session_file, sizeof(char), 512, f);
		project_frames.push_back(pf);
	}
	fread(&num_project_layers, sizeof(int), 1, f);
	Load_Stereo_Settings(f, n);
	Load_Camera_Views(f);
	fclose(f);
	Set_Fov(ph.fov);
	return true;
}

bool Free_Project()
{
	Free_Frame();
	Free_Frame_Layers();
	project_frames.clear();
	Clear_Tracking_Info();
	Free_Scene_Primitives();
	project_loaded = false;
	return true;
}


bool Create_New_Project(char *file)
{
	Free_Project();
	char path[512];
	char data_path[512];
	char name[512];
	strcpy(path, file);
	char *c = strrchr(path, '\\');
	if(!c)
	{
		SkinMsgBox(0, "Invalid project file path!", file, MB_OK);
		return false;
	}
	if(!Get_Project_Name(file, name))
	{
		SkinMsgBox(0, "ERROR! Can't get project name from file path!", file, MB_OK);
	}
	*c = 0;
	sprintf(data_path, "%s\\%s_data", path, name);
	CreateDirectory(data_path, 0);
	SetCurrentDirectory(data_path);
	CreateDirectory("SessionFiles", 0);
	SkinMsgBox(0, "A blank project will be saved with no frames.\nUse the Import Options dialog to specify the image files.", "Creating empty project.", MB_OK);
	Save_Project_Info(file);
	Load_Project(file, false);
	return true;
}

bool Allocate_Layers()
{
	int n = project_frames.size();
	Allocate_Frame_Layers(n);
	return true;
}

bool Replace_Extension(char *file, char *extension)
{
	char *c = strrchr(file, '.');
	if(!c)return false;
	sprintf(c, ".%s", extension);
	return true;
}

bool Get_Extension(char *file, char *res)
{
	char *c = strrchr(file, '.');
	if(!c)return false;
	strcpy(res, &c[1]);
	return true;
}

//get frame-specific filename for aux data, from sequence or single frame session
bool Get_Session_Aux_Data_Filename(char *res, char *extension)
{
	char base[512];
	if(project_loaded)
	{
		//get filename from session in sequence
		int n = project_frames.size();
		if(current_project_frame<0||current_project_frame>=n)
		{
			return false;
		}
		strcpy(base, project_frames[current_project_frame].session_file);
	}
	else if(frame)
	{
		//single-frame-session, just get the session name
		strcpy(base, frame->filename);
	}
	else
	{
		return false;
	}
	if(!Replace_Extension(base, extension))
	{
		return false;
	}
	strcpy(res, base);
	return true;
}

//gets the session filename for the curent frame or single-frame session file
bool Get_Frame_Session_Filename(char *res)
{
	if(!Get_Project_Filename(res))
	{
		if(!frame)
		{
			return false;
		}
		strcpy(res, frame->filename);
		return true;
	}
	return true;
}

bool Get_Project_Aux_Data_Filename(char *res, char *extension)
{
	char base[512];
	if(!Get_Project_Filename(base))
	{
		if(!frame)
		{
			return false;
		}
		strcpy(base, frame->filename);
	}
	if(!Replace_Extension(base, extension))
	{
		return false;
	}
	strcpy(res, base);
	return true;
}


bool Get_Contour_Filename(char *session_file, char *res)
{
	strcpy(res, session_file);
	char *c = strrchr(res, '.');
	if(!c)
	{
		return false;
	}
	strcpy(c, ".ctr");
	return true;
}

bool Get_File_From_Full_Path(char *path, char *file)
{
	char *c = strrchr(path, '\\');
	if(!c)
	{
		return false;
	}
	strcpy(file, &c[1]);
	return true;
}

bool Remove_File_From_Full_Path(char *path)
{
	char *c = strrchr(path, '\\');
	if(!c)
	{
		return false;
	}
	*c = 0;
	return true;
}

char* strrstr(char *s1, char *s2)
{
	char *c = strstr(s1, s2);
	if(!c){return 0;}
	char *nc = strstr(&c[1], s2);
	while(nc)
	{
		c = nc;
		nc = strstr(&c[1], s2);
		if(!nc)
		{
			return c;
		}
	}
	return c;
}

bool Get_Preceding_Path_Root(char *path, char *data_folder, char *res)
{
	strcpy(res, path);
	char *c = strrstr(res, data_folder);
	if(!c)
	{
		SkinMsgBox(0, "Data folder not found in path!", 0, MB_OK);
		return false;
	}
	int n = strlen(res);
	for(int i = 0;i<n;i++)
	{
		if(&res[i]==c)
		{
			res[i] = 0;
			return true;
		}
	}
	return false;
}

bool Replace_Session_Path(char *file, char *res, char *old_root, char *new_root)
{
	int olen = strlen(file);
	int orlen = strlen(old_root);
	if(orlen>olen)
	{
		return false;
	}
	if(!strstr(file, old_root))
	{
		return false;
	}
	char *c = &file[orlen];
	sprintf(res, "%s\\%s", new_root, c);
	return true;
}

bool Replace_Project_Path(char *old_root, char *new_root)
{
	int n = project_frames.size();
	char temp[512];
	for(int i = 0;i<n;i++)
	{
		if(Replace_Session_Path(project_frames[i].session_file, temp, old_root, new_root))
		{
			strcpy(project_frames[i].session_file, temp);
		}
	}
	return true;
}

bool Verify_Project_Path()
{
	if(Exists(project_frames[0].session_file))
	{
		return true;
	}
	char project_path[512];
	strcpy(project_path, project_frames[0].session_file);
	Remove_File_From_Full_Path(project_path);
	char local_path[512];
	strcpy(local_path, current_project_file);
	Remove_File_From_Full_Path(local_path);

	char name[512];

	if(!Get_Project_Name(current_project_file, name))
	{
		SkinMsgBox(0, "ERROR! Can't get project name from file path!", current_project_file, MB_OK);
	}
	
	char data_folder[512];
	sprintf(data_folder, "%s_data", name);

	char project_path_root[512];
	char local_path_root[512];

	Get_Preceding_Path_Root(project_path, data_folder, project_path_root);
	strcpy(local_path_root, local_path);
	
	Replace_Project_Path(project_path_root, local_path_root);
	return true;
}

bool Notify_Frame_Change(int last_frame, int new_frame)
{
	Set_Timeline_Frame(new_frame);
	Select_Frame_In_List(new_frame);
	Update_Stereo_Settings(new_frame);
	Update_Motion_Tracking();
	Update_Feature_Points_Tracking();
	Report_Frame_Changed();
	return true;
}

void Verify_First_Last_Keyframes()
{
	if(Is_First_Or_Last_Frame())
	{
		Keyframe_All_Layers();
	}
}

bool PreLoad_All_Other_Frames();


bool Load_Project(char *file, bool notify_empty)
{
	if(!Exists(file))
	{
		SkinMsgBox(0, "Invalid project file path!", 0, MB_OK);
		return false;
	}
	Enable_Window_Controls(false);
	Print_Status("Project loading.");
	Free_Project();
	if(!Load_Project_Info(file))
	{
		project_loaded = false;
		return false;
	}
	project_loaded = true;
	strcpy(current_project_file, file);
	if(project_frames.size()==0)
	{
		if(notify_empty)
		{
			SkinMsgBox(0, "Loaded empty project file.\nUse the Import Options dialog to add frames to this project.", file, MB_OK);
		}
		Print_Status("Project loaded.");
		Set_Timeline_Ticks(0);
		Enable_Window_Controls(true);
		return true;
	}
	Verify_Project_Path();
	Load_Alternate_Image_Paths();

	//load first frame fresh and set all up
	if(!Load_First_Project_Frame_Image(project_frames[0].image))
	{
		SkinMsgBox(project_frames[0].image, "Error loading first frame!", MB_OK);
		Free_Project();
		SkinMsgBox("Project not loaded.");
		return false;
	}
	Load_Virtual_Camera_Info();
	Set_Timeline_Ticks(project_frames.size());
	Allocate_Layers();

	Load_Scene_Primitives(false);
	if(Get_Num_Frames()>0)
	{
		if(SkinMsgBox("Pre-load data for all frames?", 0, MB_YESNO)==IDYES)
		{
			PreLoad_All_Other_Frames();
		}
	}

	Set_Timeline_Frame(0);
	Update_Stereo_Settings(0);
	Select_Frame_In_List(0);
	Set_Next_Highest_Layer_ID();
	Free_Relief_Images();
	Set_Active_Frame_Layers(0, false);
	Set_Virtual_Camera_Frame_Transform(0);
	current_project_frame = 0;
	Load_Layers_Only(project_frames[0].session_file);
	Verify_First_Last_Keyframes();
	project_frames[0].layers_loaded = true;
	Set_Next_Highest_Layer_ID();
	if(project_frames[0].depthmask[0]!=0)Load_Single_Relief_Image(project_frames[0].depthmask);
	if(update_contours)
	{
		if(use_backup_contours)
		{
			char ctrfile[512];
			Get_Contour_Filename(project_frames[0].session_file, ctrfile);
			if(!Exists(ctrfile))
			{
				Check_For_Contoured_Layers();
				Save_Contour_Pixels(ctrfile);
			}
			else
			{
				Load_Contour_Pixels(ctrfile);
			}
		}
		else
		{
			Check_For_Contoured_Layers();
		}
	}
	Update_Layers_Relief(false);
	Report_Frame_Changed();
	Project_Layers();
	Apply_All_TouchUps();
	Update_Layer_List();
	Print_Status("Project loaded.");
	Center_View();
	Enable_Window_Controls(true);

	Report_New_Project_Opened();
	return true;
}

bool ReLoad_Current_Project_Frame()
{
	if(!project_loaded&&frame)
	{
		//single frame session only
		Replace_Frame_Image(frame->image_file);
		Update_Layers_Relief(false);
		Project_Layers();
		Apply_All_TouchUps();
	}
	if(current_project_frame>=(int)project_frames.size()||current_project_frame<0)
	{
		return false;
	}
	Replace_Frame_Image(project_frames[current_project_frame].image);
	if(project_frames[current_project_frame].depthmask[0]!=0)
	{
		Free_Relief_Images();
		Load_Single_Relief_Image(project_frames[current_project_frame].depthmask);
		Update_Layers_Relief(false);
		Project_Layers();
		Apply_All_TouchUps();
	}
	return true;
}

//called after flipping layers
bool ReLoad_Current_Project_Layers()
{
	if(current_project_frame>=(int)project_frames.size()||current_project_frame<0)
	{
		return false;
	}
	Free_Layers();
	Load_Layers_Only(project_frames[current_project_frame].session_file);
	Verify_First_Last_Keyframes();
	project_frames[current_project_frame].layers_loaded = true;
	if(update_contours)
	{
		if(use_backup_contours)
		{
			char ctrfile[512];
			Get_Contour_Filename(project_frames[current_project_frame].session_file, ctrfile);
			if(!Exists(ctrfile))
			{
				Check_For_Contoured_Layers();
				Save_Contour_Pixels(ctrfile);
			}
			else
			{
				Load_Contour_Pixels(ctrfile);
			}
		}
		else
		{
			Check_For_Contoured_Layers();
		}
	}
	Update_Layers_Relief(false);
	Project_Layers();
	Apply_All_TouchUps();
	Update_Layer_List();
	redraw_edit_window = true;
	redraw_frame = true;
	return true;
}

//called after layers are flipped
bool Tag_All_Layers_For_Reload()
{
	int n = project_frames.size();
	for(int i = 0;i<n;i++)
	{
		project_frames[i].layers_loaded = false;
	}
	return true;
}

bool Check_For_Pending_Alignments();


//called when chaging current frame in sequence if update 3d info is checked
bool Update_Project_Frame_3D_Data()
{
	if(!project_loaded)
	{
		return false;
	}
	static int last_frame = 0;
	int f = current_project_frame;
	Set_Virtual_Camera_Frame_Transform(f);
	Dump_Disposable_Layer_Data();
	if(project_frames[f].depthmask[0]!=0)
	{
		Free_Relief_Images();
		Load_Single_Relief_Image(project_frames[f].depthmask);
	}
	if(!project_frames[f].layers_loaded)
	{
		Set_Active_Frame_Layers(f, false);
		Load_Layers_Only(project_frames[f].session_file);
		project_frames[f].layers_loaded = true;
		Set_Next_Highest_Layer_ID();
		Transfer_Layer_Names(last_frame, current_project_frame);
		Verify_First_Last_Keyframes();
	}

	Transfer_Layer_Selection_States(last_frame, current_project_frame);
	
	Set_Active_Frame_Layers(f, true);
	if(f!=0&&f!=project_frames.size()-1)
	{
		Interpolate_All_Layers(f);
	}
	if(update_contours)
	{
		if(use_backup_contours)
		{
			char ctrfile[512];
			Get_Contour_Filename(project_frames[current_project_frame].session_file, ctrfile);
			if(!Exists(ctrfile))
			{
				Check_For_Contoured_Layers();
				Save_Contour_Pixels(ctrfile);
			}
			else
			{
				Load_Contour_Pixels(ctrfile);
			}
		}
		else
		{
			Check_For_Contoured_Layers();
		}
	}
	Update_Layers_Relief(false);
	Update_Layer_List();
	Project_Layers();
	Apply_All_TouchUps();
	last_frame = f;

	Check_For_Pending_Alignments();
	return true;
}

bool Verify_Frame_Loaded(int frame)
{
	if(frame<0||frame>=(int)project_frames.size())
	{
		return false;
	}
	if(!project_frames[frame].layers_loaded)
	{
		int f = current_project_frame;
		Set_Active_Frame_Layers(frame, false);
		Load_Temp_Layers_Only(project_frames[frame].session_file);
		project_frames[frame].layers_loaded = true;
		Set_Active_Frame_Layers(f, false);
	}
	return true;
}

bool PreLoad_All_Other_Frames()
{
	int n = Get_Num_Frames();
	for(int i = 1;i<n;i++)
	{
		if(!project_frames[i].layers_loaded)
		{
			Set_Active_Frame_Layers(i, false);
			Load_Temp_Layers_Only(project_frames[i].session_file);
			project_frames[i].layers_loaded = true;
		}
	}
	return true;
}



bool Update_Temp_3D_Data()
{
	static int last_frame = 0;
	int f = current_project_frame;
	if(!project_frames[f].layers_loaded)
	{
		Set_Active_Frame_Layers(f, false);
		Load_Temp_Layers_Only(project_frames[f].session_file);
		project_frames[f].layers_loaded = true;
	}
	else
	{
		Set_Active_Frame_Layers(f, false);
	}
	Transfer_Layer_Names(last_frame, current_project_frame);

	last_frame = f;
	return true;
}



bool Set_Project_Frame(int f)
{
	if(f==current_project_frame)
	{
		return true;
	}
	if(f>=(int)project_frames.size()||f<0)
	{
		return false;
	}
	Report_Frame_About_To_Change();
	int last_frame = current_project_frame;
	current_project_frame = f;
	Replace_Frame_Image(project_frames[f].image);
	if(update_3d_data)
	{
		Update_Project_Frame_3D_Data();
	}
	Notify_Frame_Change(last_frame, current_project_frame);
	return true;
}

bool Set_Temp_Active_Project_Frame(int f)
{
	if(f==current_project_frame)
	{
		return true;
	}
	if(f>=(int)project_frames.size()||f<0)
	{
		return false;
	}
	int last_frame = current_project_frame;
	current_project_frame = f;
	Update_Temp_3D_Data();
	Set_Timeline_Frame(f);
	Select_Frame_In_List(f);
	return true;
}


bool Advance_Project_Frame()
{
	if(current_project_frame+1>=(int)project_frames.size())
	{
		return false;
	}
	Set_Project_Frame(current_project_frame+1);
	return true;
}

bool Advance_Project_Frames(int n)
{
	int nf = current_project_frame+n;
	if(nf<0)
	{
		nf = 0;
	}
	if(nf>=(int)project_frames.size())
	{
		nf = project_frames.size()-1;
	}
	Set_Project_Frame(nf);
	return true;
}


bool Get_Alternate_Session_Filename(char *file, char *res, char *newpath)
{
	char sfile[512];
	char temp[512];
	if(!Get_File_From_Full_Path(file, sfile))
	{
		return false;
	}
	sprintf(temp, "%s%s", newpath, sfile);
	strcpy(res, temp);
	return true;
}

bool Create_Alternate_Sessions_Folder(char *newfile)
{
	char newpath[512];
	strcpy(newpath, newfile);
	char *c = strrchr(newpath, '.');
	if(c)
	{
		strcpy(c, "_data");
	}
	strcat(newpath, "\\");
	CreateDirectory(newpath, 0);
	strcat(newpath, "\\SessionFiles\\");
	CreateDirectory(newpath, 0);
	int n = project_frames.size();
	char new_sfile[512];
	for(int i = 0;i<n;i++)
	{
		Get_Alternate_Session_Filename(project_frames[i].session_file, new_sfile, newpath);
		CopyFile(project_frames[i].session_file, new_sfile, false);
		strcpy(project_frames[i].session_file, new_sfile);
	}
	return true;
}

bool Save_Project(char *file)
{
	bool save_all = false;
	//if it doesn't match the current filename then create new folder for session files
	if(strcmp(current_project_file, file))
	{
		save_all = true;
		Create_Alternate_Sessions_Folder(file);
	}

	int oldframe = current_project_frame;
	int n = project_frames.size();
	for(int i = 0;i<n;i++)
	{
		//only save frames that have been opened for editing
		if(project_frames[i].layers_loaded||save_all)
		{
			//add flag in the future to indicate modified frames
			Set_Temp_Active_Project_Frame(i);
			Save_Frame_Info(project_frames[i].session_file, false);//don't save camera views
		}
	}
	Save_Project_Info(file);
	strcpy(current_project_file, file);
	Set_Project_Frame(oldframe);
	Save_Alternate_Image_Paths();
	Save_Scene_Primitives();
	Save_Virtual_Camera_Info();
	Report_Saving_Project();
	return true;
}

bool AutoSave_Project()
{
	char file[512];
	strcpy(file, current_project_file);
	return Save_Project(file);
}

int Get_Num_Frames()
{
	return project_frames.size();
}

bool Get_Frame_Name(int index, char *res)
{
	if(index>=(int)project_frames.size()||index<0)
	{
		return false;
	}
	strcpy(res, project_frames[index].image);
	return true;
}

bool Get_Frame_Image(int index, char *res)
{
	if(index>=(int)project_frames.size()||index<0)
	{
		return false;
	}
	strcpy(res, project_frames[index].image);
	return true;
}

int Get_Current_Project_Frame()
{
	return current_project_frame;
}

void Set_Project_Frame_Layers_Loaded()
{
	int n = project_frames.size();
	for(int i = 0;i<n;i++)
	{
		project_frames[i].layers_loaded = true;
	}
}


bool Verify_Project_Frame_Layers_Loaded(int index)
{
	if(!project_frames[index].layers_loaded)
	{
		Set_Active_Frame_Layers(index, false);
		Load_Temp_Layers_Only(project_frames[index].session_file);
		project_frames[index].layers_loaded = true;
		Set_Active_Frame_Layers(current_project_frame, false);
	}
	return true;
}


bool Set_Project_Frame_Layers_Loaded(int index)
{
	if(index>=(int)project_frames.size()||index<0)
	{
		return false;
	}
	project_frames[index].layers_loaded = true;
	return true;
}

bool Get_Project_Filename(char *file)
{
	if(project_loaded)
	{
		strcpy(file, current_project_file);
		return true;
	}
	return false;
}

bool Get_Project_Path(char *path)
{
	if(Get_Num_Frames()==0)
	{
		strcpy(path, "c:\\");
		return false;
	}
	strcpy(path, current_project_file);
	char *c = strrchr(path, '\\');
	if(c)
	{
		*c = 0;
		return true;
	}
	return false;
}

bool Get_Default_Output_Path(char *path)
{
	if(Get_Num_Frames()==0)
	{
		strcpy(path, "c:\\");
		return false;
	}
	strcpy(path, current_project_file);
	char *c = strrchr(path, '.');
	if(c)
	{
		sprintf(c, "_data\\OutputImages");
	}
	return true;
}

bool Get_Project_Name(char *res)
{
	if(!project_loaded)
	{
		strcpy(res, "Frame");
		return false;
	}
	Get_File_From_Full_Path(current_project_file, res);
	char *c = strrchr(res, '.');
	if(c)
	{
		*c = 0;
		return true;
	}
	return false;
}

bool Get_Project_Images_Path(char *res)
{
	if(!project_loaded)
	{
		strcpy(res, "UNSPECIFIED");
		return false;
	}
	if(project_frames.size()==0)
	{
		strcpy(res, "UNSPECIFIED");
		return true;
	}
	strcpy(res, project_frames[0].image);
	char *c = strrchr(res, '\\');
	if(c)
	{
		*c = 0;
	}
	return true;
}

bool Get_Project_Layermasks_Path(char *res)
{
	if(!project_loaded)
	{
		strcpy(res, "UNSPECIFIED");
		return false;
	}
	if(project_frames.size()==0)
	{
		strcpy(res, "UNSPECIFIED");
		return true;
	}
	if(project_frames[0].layermask[0]==0)
	{
		strcpy(res, "UNSPECIFIED");
		return true;
	}
	strcpy(res, project_frames[0].layermask);
	char *c = strrchr(res, '\\');
	if(c)
	{
		*c = 0;
	}
	return true;
}

bool Get_Project_Depthmasks_Path(char *res)
{
	if(!project_loaded)
	{
		strcpy(res, "UNSPECIFIED");
		return false;
	}
	if(project_frames.size()==0)
	{
		strcpy(res, "UNSPECIFIED");
		return true;
	}
	if(project_frames[0].depthmask[0]==0)
	{
		strcpy(res, "UNSPECIFIED");
		return true;
	}
	strcpy(res, project_frames[0].depthmask);
	char *c = strrchr(res, '\\');
	if(c)
	{
		*c = 0;
	}
	return true;
}

bool Prune_Frames_After(int n)
{
	Set_Project_Frame(0);
	int total = project_frames.size()-n;
	for(int i = 0;i<total;i++)
	{
		project_frames.pop_back();
	}
	return true;
}

bool Get_Default_Session_Filename(char *image, char *res)
{
	char newpath[512];
	strcpy(newpath, current_project_file);
	char *c = strrchr(newpath, '.');
	if(c)
	{
		strcpy(c, "_data\\SessionFiles");
	}
	char ifile[512];
	Get_File_From_Full_Path(image, ifile);
	c = strrchr(ifile, '.');
	if(c)
	{
		strcpy(c, ".sif");
	}
	sprintf(res, "%s\\%s", newpath, ifile);
	return true;
}

bool Add_Fresh_Frame_Image_To_Project(char *image)
{
	if(frame)
	{
		strcpy(frame->filename, image);
	}
	else
	{
		Load_Frame_Image(image, true);
	}
	PROJECT_FRAME pf;
	strcpy(pf.image, image);
	Get_Default_Session_Filename(image, pf.session_file);
	Append_Single_Stereo_Setting();
	Append_Single_Frame_Layers();
	project_frames.push_back(pf);
	Set_Timeline_Ticks(project_frames.size());
	if(project_frames.size()>1)
	{
		//if prior frame exists just copy layers
		Dump_Disposable_Layer_Data();
		Copy_Layers_To_Frame(0, project_frames.size()-1);
	}
	int id = project_frames.size()-1;
	Set_Active_Frame_Layers(id, false);
	Save_Frame_Info(pf.session_file, false);//don't save camera views
	if(id>0)
	{
		project_frames[id].layers_loaded = false;
		Free_Layers();
	}
	return true;
}

bool Import_Fresh_Project_Images(vector<char*> *files)
{
	int n = files->size();
	for(int i = 0;i<n;i++)
	{
		Add_Fresh_Frame_Image_To_Project((*files)[i]);
	}
	Keyframe_FirstLast_Stereo_Setting();
	Set_Project_Frame(0);
	Update_Project_Frame_3D_Data();
	return true;
}

bool Import_Project_Images(vector<char*> *files)
{
	int osize = project_frames.size();
	int nsize = files->size();
	if(osize!=0)
	{
		if(SkinMsgBox(0, "Warning! This will replace the current image in the project!\n(Any existing layers are depth masks will not be affected).\nAre you sure?", " ", MB_YESNO)==IDNO)
		{
			return false;
		}
	}
	if(nsize<osize)
	{
		if(SkinMsgBox(0, "Warning! The new directory contains fewer frames than in the existing project!\nAny existing higher frames past the new total will be discarded.\nAre you sure?", " ", MB_YESNO)==IDNO)
		{
			return false;
		}
		Prune_Frames_After(nsize);
	}
	if(osize==0)
	{
		return Import_Fresh_Project_Images(files);
	}
	int i;
	if(nsize==project_frames.size())
	{
		for(i = 0;i<nsize;i++)
		{
			strcpy(project_frames[i].image, (*files)[i]);
		}
		Set_Project_Frame(0);
		return true;
	}
	if(nsize>(int)project_frames.size())
	{
		osize = project_frames.size();
		for(i = 0;i<osize;i++)
		{
			strcpy(project_frames[i].image, (*files)[i]);
		}
		for(i = osize;i<nsize;i++)
		{
			Add_Fresh_Frame_Image_To_Project((*files)[i]);
		}
		Keyframe_FirstLast_Stereo_Setting();
		Set_Project_Frame(0);
		return true;
	}
	return true;
}

bool Import_Project_Layermasks(vector<char*> *files)
{
	int osize = project_frames.size();
	int nsize = files->size();
	if(osize!=nsize)
	{
		SkinMsgBox(0, "ERROR! The number of layermasks does not match the number of frames currently in the project!", "ERROR", MB_OK);
		return false;
	}
	Clear_Layer_Colors();
	int i;
	for(i = 0;i<nsize;i++)
	{
		Print_Status("Loading layermask colors from %s", (*files)[i]);
		Select_Frame_In_List(i);
		if(!Add_Layer_Colors_From_Mask_Image((*files)[i]))
		{
			SkinMsgBox(0, "An error occured loading the layer mask colors from the image file", (*files)[i], MB_OK);
			Clear_Layer_Colors();
			return false;
		}
	}
	num_project_layers = layer_colors.size()/3;

	for(i = 0;i<nsize;i++)
	{
		Set_Temp_Active_Project_Frame(i);
		Set_Active_Frame_Layers(i, false);
		Reset_Next_Layer_ID();
		Load_Preset_Layer_Mask_Image((*files)[i]);
		if(i==0||i==nsize-1)Keyframe_All_Layers();
		Dump_Disposable_Layer_Data();
		strcpy(project_frames[i].layermask, (*files)[i]);
		if(project_frames[i].depthmask[0]!=0)Set_Temp_Global_Relief_Image(project_frames[i].depthmask);
		Save_Frame_Info(project_frames[i].session_file, false);//don't save camera views
		Free_Layers();
		project_frames[i].layers_loaded = false;
	}
	Clear_Layer_Colors();

	Set_Project_Frame(0);
	Set_Next_Highest_Layer_ID();

	return true;
}

bool Import_Project_Depthmasks(vector<char*> *files)
{
	int osize = project_frames.size();
	int nsize = files->size();
	int oldframe = current_project_frame;
	if(osize!=nsize)
	{
		SkinMsgBox(0, "ERROR! The number of depthmasks does not match the number of frames currently in the project!", "ERROR", MB_OK);
		return false;
	}
	for(int i = 0;i<nsize;i++)
	{
		Set_Temp_Active_Project_Frame(i);
		Set_Active_Frame_Layers(i, false);
		strcpy(project_frames[i].depthmask, (*files)[i]);
		Set_Temp_Global_Relief_Image(project_frames[i].depthmask);
		Dump_Disposable_Layer_Data();
		Save_Frame_Info(project_frames[i].session_file, false);//don't save camera views
		Free_Layers();
		project_frames[i].layers_loaded = false;
	}
	Set_Project_Frame(oldframe);
	return true;
}


bool Backup_Current_Contours()
{
	char ctrfile[512];
	Get_Contour_Filename(project_frames[current_project_frame].session_file, ctrfile);
	Save_Contour_Pixels(ctrfile);
	return true;
}

//ensures that basic metadata is loaded for all frames
//FIXTHIS does this occur?
bool PreLoad_All_Layers()
{
	int n = project_frames.size();
	for(int i = 0;i<n;i++)
	{
		Set_Temp_Active_Project_Frame(i);
	}
	return true;
}


bool Is_First_Or_Last_Frame()
{
	int n = project_frames.size();
	if(n==0)return true;
	return !(current_project_frame>0&&current_project_frame<n-1);
}

bool Is_First_Frame()
{
	return current_project_frame==0;
}

bool Is_Last_Frame()
{
	int n = project_frames.size();
	return (current_project_frame==n-1);
}

bool Get_Session_Filename(int frame, char *res)
{
	int n = project_frames.size();
	if(frame<0||frame>=n)
	{
		return false;
	}
	strcpy(res, project_frames[frame].session_file);
	return true;
}


