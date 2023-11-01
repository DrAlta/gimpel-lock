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
#include "G3DCoreFiles/Frame.h"
#include <GL/gl.h>
#include "Skin.h"

void Update_Win32_Messages();

void Render_Frame_Update();
float VecLength(float* v);
bool Replace_RGB_Values(unsigned char *rgb);
bool Load_PreCached_Frame_Data(int index);

bool Set_PreCache_All_Button(char *text);

bool Set_Preview_Play_Button(char *text);
bool Display_Current_Preview_Frame(int index);

bool Toggle_Preview_Playback(int start, int end);

bool play_preview = false;
int preview_frame = -1;

bool delete_preview_data_on_exit = true;
bool precaching_all_frame_data = false;

bool render_preview_data = false;

bool loop_preview = true;

bool restrict_preview_range = false;

int preview_start_frame = 0;
int preview_end_frame = 0;


bool Init_Preview_Renderer();

const int PRECACHE_FILE_VERSION = 2;

class PRECACHE_LUMP
{
public:
	PRECACHE_LUMP()
	{
		frame = -1;
		filepos = 0;
	}
	~PRECACHE_LUMP()
	{
	}
	int frame;
	unsigned int filepos;
};

class PRECACHE_FILE
{
public:
	PRECACHE_FILE()
	{
		start_frame = -1;
		num_frames = 0;
		file = 0;
	}
	~PRECACHE_FILE()
	{
		int n = lumps.size();
		for(int i = 0;i<n;i++)
		{
			delete lumps[i];
		}
		lumps.clear();
		if(file)
		{
			fclose(file);
			if(delete_preview_data_on_exit)
			{
				remove(filename);
			}
		}
	}
	FILE *Get_File_Pointer(int frame)
	{
		int index = frame-start_frame;
		if(index<0||index>=(int)num_frames)
		{
			return 0;
		}
		if(lumps[index]->frame!=frame)
		{
			return 0;
		}
		fseek(file, lumps[index]->filepos, SEEK_SET);
		return file;
	}	
	bool Init_Lumps(unsigned int start_filepos, unsigned int lump_size)
	{
		for(unsigned int i = 0;i<num_frames;i++)
		{
			PRECACHE_LUMP *pcl = new PRECACHE_LUMP;
			pcl->frame = start_frame+i;
			pcl->filepos = start_filepos+(lump_size*i);
			lumps.push_back(pcl);
		}
		return true;
	}
	vector<PRECACHE_LUMP*> lumps;
	FILE *file;
	char filename[512];
	unsigned int start_frame;
	unsigned int num_frames;
};

struct PRECACHE_HEADER
{
	int version;
	unsigned int num_files;
	unsigned int max_file_size;
	int max_frames_per_file;
	unsigned int frame_data_size;
	int num_frames;
	int frame_width;
	int frame_height;
	unsigned int rgb_size;
	unsigned int vertex_size;
};

class PRECACHE_SET
{
public:
	PRECACHE_SET()
	{
		num_tri_indices = 0;
		tri_indices = 0;
		frames_generated = 0;
	}
	~PRECACHE_SET()
	{
		int n = files.size();
		for(int i = 0;i<n;i++)
		{
			delete files[i];
		}
		files.clear();
		if(tri_indices)
		{
			delete[] tri_indices;
		}
		if(frames_generated)
		{
			delete[] frames_generated;
			frames_generated = 0;
		}
	}
	unsigned int num_tri_indices;
	unsigned int *tri_indices;
	vector<PRECACHE_FILE*> files;
	PRECACHE_HEADER pc_header;
	bool *frames_generated;//flags for which frames are available
};

PRECACHE_SET *precache_set = 0;

bool Free_PreCached_Sequence_Data()
{
	if(precache_set)
	{
		if(!delete_preview_data_on_exit)
		{
			//save header and frames generated list
			FILE *f = precache_set->files[0]->file;
			rewind(f);
			fwrite(&precache_set->pc_header, 1, sizeof(PRECACHE_HEADER), f);
			fwrite(precache_set->frames_generated, 1, sizeof(bool)*precache_set->pc_header.num_frames, f);
		}
		delete precache_set;
	}
	precache_set = 0;
	if(play_preview)Toggle_Preview_Playback(0, 0);
	return true;
}

bool Init_PreCache_Set_Info(PRECACHE_HEADER *pch)
{
	pch->version = PRECACHE_FILE_VERSION;
	pch->num_frames = Get_Num_Frames();
	pch->frame_width =  Frame_Width();
	pch->frame_height = Frame_Height();
	//size rgb data
	pch->rgb_size = sizeof(unsigned char)*(pch->frame_width*pch->frame_height*3);
	//size vertex data
	pch->vertex_size = sizeof(float)*(pch->frame_width*pch->frame_height*3);
	pch->frame_data_size = pch->rgb_size+pch->vertex_size;
	pch->max_file_size = 1024*1024*1024;//1GB
	pch->num_files = 1;//at least 1 for the header
	pch->max_frames_per_file = 0;
	unsigned int dc = 0;
	for(int i = 0;i<pch->num_frames;i++)//just loop through the frames
	{
		dc+=pch->frame_data_size;//in case the total size is greater than a UINT can hold
		if(dc>=pch->max_file_size)
		{
			dc = 0;
			pch->num_files++;
			if(pch->max_frames_per_file==0)
			{
				pch->max_frames_per_file = i+1;
			}
		}
	}
	if(pch->num_files==1)
	{
		pch->max_frames_per_file = pch->num_frames;
	}
	return true;
}

bool Get_PreCache_Filename(int index, char *res)
{
	Get_Project_Filename(res);
	char *c = strrchr(res, '.');
	if(!c){return false;}
	sprintf(c, ".pv%i", index);
	return true;
}


float Recalc_Preview_Space(int start, int end)
{
	if(!precache_set)return 0;
	PRECACHE_HEADER *pch = &precache_set->pc_header;

	unsigned int bytes_per_frame = pch->frame_data_size;
	int num_frames = (end-start);
	unsigned int gb = 1024*1024*1024;
	float gbs_per_frame = ((float)bytes_per_frame)/gb;
	float res = gbs_per_frame*num_frames;
	return res;
}

float PreCache_DiskSpace_GB()
{
	return Recalc_Preview_Space(0, Get_Num_Frames());
	if(!precache_set)return 0;
	PRECACHE_HEADER *pch = &precache_set->pc_header;
	unsigned int gb = 1024*1024*1024;
	float res = 0;
	for(int i = 0;i<(int)pch->num_files-1;i++)
	{
		res += 1;
	}
	unsigned int last_bytes = precache_set->files[pch->num_files-1]->num_frames*pch->frame_data_size;
	res += ((float)last_bytes)/gb;
	return res;
}

bool Open_PreCache_Fileset()
{
	if(!precache_set){return false;}
	int n = precache_set->files.size();
	for(int i = 0;i<n;i++)
	{
		Get_PreCache_Filename(i, precache_set->files[i]->filename);
		if(Exists(precache_set->files[i]->filename))
		{
			precache_set->files[i]->file = fopen(precache_set->files[i]->filename, "rb+");
			if(i==0)
			{
				FILE *f = precache_set->files[0]->file;
				rewind(f);
				PRECACHE_HEADER pch;
				fread(&pch, 1, sizeof(PRECACHE_HEADER), f);
				fread(precache_set->frames_generated, 1, sizeof(bool)*precache_set->pc_header.num_frames, f);
			}
		}
		else
		{
			precache_set->files[i]->file = fopen(precache_set->files[i]->filename, "wb+");
		}
	}
	return true;
}

FILE* Get_PreCache_File_Pointer(int frame)
{
	if(!precache_set){return 0;}
	int n = precache_set->files.size();
	for(int i = 0;i<n;i++)
	{
		if(frame-precache_set->files[i]->start_frame<precache_set->files[i]->num_frames)
		{
			return precache_set->files[i]->Get_File_Pointer(frame);
		}
	}
	return 0;
}

bool Preview_Frame_Exists(int frame)
{
	if(precache_set)
	{
		if(frame>=0&&frame<precache_set->pc_header.num_frames)
		{
			return precache_set->frames_generated[frame];
		}
	}
	return false;
}

bool Flag_Preview_Frame_Created(int frame)
{
	if(precache_set)
	{
		if(frame>=0&&frame<precache_set->pc_header.num_frames)
		{
			precache_set->frames_generated[frame] = true;
			return true;
		}
	}
	return false;
}

bool Init_PreCached_Sequence_Data()
{
	if(Get_Num_Frames()==0)
	{
		SkinMsgBox(0, "Internal error! Attempt to init precache data for sequence, no sequence loaded!", "This never happens", MB_OK);
		return false;
	}
	Free_PreCached_Sequence_Data();
	precache_set = new PRECACHE_SET;
	PRECACHE_HEADER *pch = &precache_set->pc_header;
	Init_PreCache_Set_Info(pch);
	precache_set->frames_generated = new bool[pch->num_frames];
	int i;
	for(i = 0;i<pch->num_frames;i++)
	{
		precache_set->frames_generated[i] = false;
	}
	for(i = 0;i<(int)pch->num_files;i++)
	{
		PRECACHE_FILE *pcf = new PRECACHE_FILE;
		precache_set->files.push_back(pcf);
		if(pch->num_files==1)
		{
			pcf->start_frame = 0;
			pcf->num_frames = pch->num_frames;
		}
		else
		{
			pcf->start_frame = pch->max_frames_per_file*i;
			if(i==pch->num_files-1)
			{
				pcf->num_frames = pch->num_frames-pcf->start_frame;
			}
			else
			{
				pcf->num_frames = pch->max_frames_per_file;
			}
		}
		if(i==0)
		{
			int offset = sizeof(PRECACHE_HEADER);
			offset += sizeof(bool)*pch->num_frames;
			pcf->Init_Lumps(offset, pch->frame_data_size);
		}
		else
		{
			pcf->Init_Lumps(0, pch->frame_data_size);
		}
	}
	Init_Preview_Renderer();
	Open_PreCache_Fileset();
	return true;
}


bool Set_Virtual_Camera_Frame_Transform(int frame);

bool PreCache_Current_Frame_Data()
{
	int index = Get_Current_Project_Frame();
	FILE *f = Get_PreCache_File_Pointer(index);
	if(!f)
	{
		char msg[512];
		sprintf(msg, "Can't get file pointer for frame %i! Error precaching frame data.", index);
		SkinMsgBox(0, msg, 0, MB_OK);
		return false;
	}
	fwrite(frame->original_rgb, sizeof(unsigned char)*(frame->width*frame->height*3), 1, f);
	fwrite(frame->vertices, sizeof(float)*(frame->width*frame->height*3), 1, f);
	redraw_frame = true;
	Flag_Preview_Frame_Created(index);
	return true;
}

bool Load_PreCached_Frame_Data(int index)
{
	FILE *f = Get_PreCache_File_Pointer(index);
	if(!f)
	{
		char msg[512];
		sprintf(msg, "Can't get file pointer for frame %i! Error loading precached frame data.", index);
		SkinMsgBox(0, msg, 0, MB_OK);
		return false;
	}
	fread(frame->original_rgb, sizeof(unsigned char)*(frame->width*frame->height*3), 1, f);
	fread(frame->vertices, sizeof(float)*(frame->width*frame->height*3), 1, f);
	Replace_RGB_Values(frame->original_rgb);
	return true;
}

bool PreCache_All_Frame_Data(int start, int end)
{
	int old_frame = Get_Current_Project_Frame();
	Set_Project_Frame(start);
	bool cancelled = false;
	for(int i = start;i<end;i++)
	{
		if(!precaching_all_frame_data)
		{
			cancelled = true;
			i =end;
		}
		else
		{
			Display_Current_Preview_Frame(i);
			preview_frame = i;
			Print_Status("Generating data for frame %i", i);
			Set_Project_Frame(i);
			Update_Win32_Messages();
			PreCache_Current_Frame_Data();
		}
	}
	Print_Status("Done.");
	Set_PreCache_All_Button("Pre-Generate Data For All Frames");
	if(cancelled)
	{
		SkinMsgBox("Cancelled");
	}
	else
	{
		SkinMsgBox("Done");
	}
	return true;
}

bool Toggle_PreCache_All_Frame_Data(int start, int end)
{
	precaching_all_frame_data = !precaching_all_frame_data;
	if(precaching_all_frame_data)
	{
		Set_PreCache_All_Button("Stop");
		PreCache_All_Frame_Data(start, end);
	}
	else
	{
		Set_PreCache_All_Button("Pre-Generate Data For All Frames");
	}
	return true;
}


bool Toggle_Preview_Playback(int start, int end)
{
	play_preview = !play_preview;
	if(play_preview)
	{
		Set_Preview_Play_Button("Stop");
		preview_start_frame = start;
		preview_end_frame = end;
		if(preview_frame==-1)
		{
			preview_frame = start;
		}
	}
	else
	{
		Set_Preview_Play_Button("Play");
	}
	return true;
}


bool Get_VC_Frame_Transform(int frame, float *pos, float *rot);
void Set_Camera(float *pos, float *rot);

//similar code to "Center_View()"

extern bool camera_moving;

void Center_PreView()
{
	float pos[3] = {0,0,0};
	float rot[3] = {0,180,0};
	if(frame)
	{
		pos[0] = frame->view_origin[0];
		pos[1] = frame->view_origin[1];
		pos[2] = frame->view_origin[2];
		rot[0] = (frame->view_rotation[0]);//flip pitch because we're spinning around vertical axis
		rot[1] = (-frame->view_rotation[1])+180;
		rot[2] = 0;
	}
	Set_Camera(pos, rot);
	horizontal_view = (frame->view_rotation[1])-180;
	vertical_view = rot[0];
	ReCalc_Center();
	zoom = VecLength(scene_center);
	camera_moving = true;
}


void Set_Preview_Virtual_Camera_View()
{
	Get_VC_Frame_Transform(preview_frame, frame->view_origin, frame->view_rotation);
	Center_PreView();
}


bool Update_Preview_Playback()
{
	if(Preview_Frame_Exists(preview_frame))
	{
		Load_PreCached_Frame_Data(preview_frame);
		Set_Preview_Virtual_Camera_View();
	}
	preview_frame++;
	if(preview_frame>=preview_end_frame)
	{
		if(loop_preview)
		{
			preview_frame = preview_start_frame;
		}
		else
		{
			preview_frame--;
			if(play_preview)
			{
				Toggle_Preview_Playback(0, 0);
			}
		}
	}
	redraw_frame = true;
	Display_Current_Preview_Frame(preview_frame);
	return true;
}

bool Step_Preview(int n)
{
	preview_frame += n;
	if(preview_frame<0)
	{
		preview_frame = Get_Num_Frames()-1;
	}
	if(preview_frame>=Get_Num_Frames())
	{
		preview_frame = 0;
	}
	if(Preview_Frame_Exists(preview_frame))
	{
		Load_PreCached_Frame_Data(preview_frame);
		Set_Preview_Virtual_Camera_View();
	}
	Display_Current_Preview_Frame(preview_frame);
	redraw_frame = true;

	return true;
}


///////////////////////////////////////////////////

bool Init_Preview_Renderer()
{
	precache_set->num_tri_indices = 0;
	precache_set->tri_indices = 0;
	int i, j;
	for(i = 0;i<frame->height-1;i++)
	{
		precache_set->num_tri_indices += ((frame->width)*2)+4;
	}
	precache_set->tri_indices = new unsigned int[precache_set->num_tri_indices];
	unsigned int cnt = 0;
	for(i = 0;i<frame->height-1;i++)
	{
		for(j = 0;j<frame->width;j++)
		{
			precache_set->tri_indices[cnt] = (i*frame->width)+j;cnt++;
			precache_set->tri_indices[cnt] = ((i+1)*frame->width)+j;cnt++;
		}
		if(i!=frame->height-2)
		{
			precache_set->tri_indices[cnt] = precache_set->tri_indices[cnt-1];cnt++;
			precache_set->tri_indices[cnt] = precache_set->tri_indices[cnt-1];cnt++;
			precache_set->tri_indices[cnt] = ((i+1)*frame->width);cnt++;
			precache_set->tri_indices[cnt] = ((i+1)*frame->width);cnt++;
		}
	}
	precache_set->num_tri_indices = cnt;
	return true;
}

bool Render_Preview_3D()
{
	if(!precache_set)
	{
		return false;
	}
	glDrawElements(GL_TRIANGLE_STRIP, precache_set->num_tri_indices, GL_UNSIGNED_INT, precache_set->tri_indices);
	return true;
}








