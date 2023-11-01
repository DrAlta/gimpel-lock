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
#include "TouchUpTool.h"
#include "Skin.h"
#include "Plugins.h"
#include "G3DCoreFiles/Frame.h"
#include "G3DCoreFiles/Layers.h"

#define TOUCHUP_FILE_VERSION 2

bool Get_Session_Aux_Data_Filename(char *res, char *extension);

bool Get_Brush_Position(float *px, float *py);
float Get_Brush_Size();
int Get_Brush_Grid_Size();
bool Get_Brush_Grid_Start(int *px, int *py);

float* Get_Brush_Depth_Grid(int startx, int starty, int grid_size);
float* Get_AntiAliased_Brush_Depth_Grid(float *depth_grid, int grid_size);
float* Get_Linear_Brush_Strength_Grid(float bx, float by, float bsize, int startx, int starty, int grid_size);

float* Get_Brush_Selected_Depth_Grid(int startx, int starty, int grid_size);

float Get_Averaged_Brush_Depth_Grid(float *depth_grid, int grid_size);

bool Set_Depth_Grid(int startx, int starty, int width, int height, float *grid);
bool Set_Selected_Depth_Grid(int startx, int starty, int width, int height, float *grid);

bool touchup_tool_open = false;

bool touchups_altered = false;

//touchups altered is false when:
//fresh data is loaded from file----
//no data is loaded from file----
//touchups are saved to disk voluntarily
//touchups are restored from disk

//touchups altered is true when:
//touchup edits are added to list----
//touchup edits are deleted from list----
//touchup edits are added from stored list If Stored List Exists----
//touchup edits are cleared When They Existed---

void Set_TouchUps_Altered(bool b)
{
	touchups_altered = b;
}

bool Open_TouchUp_Tool()
{
	if(touchup_tool_open)
	{
		return false;
	}
	touchup_tool_open = true;
	Open_TouchUp_Dlg();
	List_All_TouchUp_Edits();
	return true;
}

bool Close_TouchUp_Tool()
{
	if(!touchup_tool_open)
	{
		return false;
	}
	touchup_tool_open = false;
	Close_TouchUp_Dlg();
	return true;
}

/////////////////////////////////////////////////////////////

//system to store and manage touch up edits

enum
{
	tu_PUSH,
	tu_PULL,
	tu_BLEND,
	tu_FLATTEN
};

int touchup_edit_group_id = 0;

class TOUCHUP
{
public:
	TOUCHUP()
	{
	}
	~TOUCHUP()
	{
	}
	__forceinline void Get_Real_Values()
	{
		//translate logical units into frame-based units in actual pixels
		real_brushx = logical_brushx*frame->width;
		real_brushy = logical_brushy*frame->height;
		real_brush_size = logical_brush_size*frame->width;
		real_grid_size = (real_brush_size*2)+4;
		real_start_x = (real_brushx-(real_grid_size/2));
		real_start_y = (real_brushy-(real_grid_size/2));
	}
	__forceinline void Get_Logical_Values(float brushx, float brushy, float brush_size)
	{
		logical_brushx = brushx/frame->width;
		logical_brushy = brushy/frame->height;
		logical_brush_size = brush_size/frame->width;
	}
	//save relative percentages so edits are resolution-independent
	float logical_brushx, logical_brushy;//percentage into frame
	float logical_brush_size;//the brush size as a percentage of the frame width

	float real_brushx, real_brushy;//the brush pixel location
	float real_brush_size;//the brush size
	int real_grid_size;//the width/height of the pixel grid covered by the brush
	int real_start_x, real_start_y;//the starting pixel position of the brush grid
};

class TOUCHUP_EDIT_GROUP
{
public:
	TOUCHUP_EDIT_GROUP()
	{
		num_selected_layers = 0;
		selected_layer_ids = 0;
	}
	~TOUCHUP_EDIT_GROUP()
	{
		int n = touchups.size();
		for(int i = 0;i<n;i++)
		{
			delete touchups[i];
		}
		touchups.clear();
		if(selected_layer_ids)delete[] selected_layer_ids;
	}
	__forceinline void Get_Real_Values()
	{
		real_start_brushx = logical_start_brushx*frame->width;
		real_start_brushy = logical_start_brushy*frame->height;
	}
	float logical_start_brushx, logical_start_brushy;//the starting brush pixel location
	float real_start_brushx, real_start_brushy;//the starting brush pixel location
	int type;//push/pull, blend, or flatten
	int num_selected_layers;
	int *selected_layer_ids;
	vector<TOUCHUP*> touchups;
};

vector<TOUCHUP_EDIT_GROUP*> touchup_edit_groups;
vector<TOUCHUP_EDIT_GROUP*> stored_touchup_edit_groups;

TOUCHUP_EDIT_GROUP *current_touchup_edit_group = 0;

TOUCHUP* Create_TouchUp_Copy(TOUCHUP *src)
{
	TOUCHUP *dst = new TOUCHUP;
	dst->logical_brush_size = src->logical_brush_size;
	dst->logical_brushx = src->logical_brushx;
	dst->logical_brushy = src->logical_brushy;
	dst->real_brush_size = src->real_brush_size;
	dst->real_brushx = src->real_brushx;
	dst->real_brushy = src->real_brushy;
	dst->real_grid_size = src->real_grid_size;
	dst->real_start_x = src->real_start_x;
	dst->real_start_y = src->real_start_y;
	return dst;
}

TOUCHUP_EDIT_GROUP* Create_TouchUp_Edit_Copy(TOUCHUP_EDIT_GROUP *src)
{
	TOUCHUP_EDIT_GROUP *dst = new TOUCHUP_EDIT_GROUP;
	dst->logical_start_brushx = src->logical_start_brushx;
	dst->logical_start_brushy = src->logical_start_brushy;
	dst->real_start_brushx = src->real_start_brushx;
	dst->real_start_brushy = src->real_start_brushy;
	dst->type = src->type;
	int n = src->touchups.size();
	for(int i = 0;i<n;i++)
	{
		dst->touchups.push_back(Create_TouchUp_Copy(src->touchups[i]));
	}
	return dst;
}

bool Clear_Stored_TouchUp_Edits()
{
	int n = stored_touchup_edit_groups.size();
	for(int i = 0;i<n;i++)
	{
		delete stored_touchup_edit_groups[i];
	}
	stored_touchup_edit_groups.clear();
	return true;
}

bool Remember_All_TouchUp_Edits()
{
	Clear_Stored_TouchUp_Edits();
	int n = touchup_edit_groups.size();
	for(int i = 0;i<n;i++)
	{
		stored_touchup_edit_groups.push_back(Create_TouchUp_Edit_Copy(touchup_edit_groups[i]));
	}
	return true;
}

bool Add_All_Stored_TouchUp_Edits()
{
	int n = stored_touchup_edit_groups.size();
	for(int i = 0;i<n;i++)
	{
		touchup_edit_groups.push_back(Create_TouchUp_Edit_Copy(stored_touchup_edit_groups[i]));
	}
	if(n>0)Set_TouchUps_Altered(true);
	return true;
}


bool Start_New_TouchUp_Edit_Group(int type, float logical_x, float logical_y)
{
	TOUCHUP_EDIT_GROUP *tg = new TOUCHUP_EDIT_GROUP;
	tg->type = type;
	tg->logical_start_brushx = logical_x;
	tg->logical_start_brushy = logical_y;
	tg->Get_Real_Values();
	tg->num_selected_layers = Num_Selected_Layers();
	tg->selected_layer_ids = new int[tg->num_selected_layers];
	for(int i = 0;i<tg->num_selected_layers;i++)
	{
		tg->selected_layer_ids[i] = Get_Selected_Layer_ID(i);
	}
	touchup_edit_groups.push_back(tg);
	current_touchup_edit_group = tg;
	if(touchup_tool_open)Append_Last_TouchUp_To_List();
	Set_TouchUps_Altered(true);
	return true;
}

//called when keys are released for touchup edits
bool Reset_TouchUp_Edit_Group()
{
	current_touchup_edit_group = 0;
	return true;
}

bool Register_TouchUp(TOUCHUP *tu, int type, float px, float py)
{
	if(!current_touchup_edit_group)
	{
		Start_New_TouchUp_Edit_Group(type, px, py);
	}
	else if(current_touchup_edit_group->type!=type)
	{
		Start_New_TouchUp_Edit_Group(type, px, py);
	}
	current_touchup_edit_group->touchups.push_back(tu);
	return true;
}

int Num_TouchUp_Edit_Groups()
{
	return touchup_edit_groups.size();
}

bool Get_TouchUp_Edit_Group_Text(int index, char *res)
{
	int n = touchup_edit_groups.size();
	if(index<0||index>=n)return false;
	TOUCHUP_EDIT_GROUP *tg = touchup_edit_groups[index];
	if(tg->type==tu_PUSH)
	{
		sprintf(res, "Push Starting At: %i - %i", (int)tg->real_start_brushx, (int)tg->real_start_brushy);
		return true;
	}
	else if(tg->type==tu_PULL)
	{
		sprintf(res, "Pull Starting At: %i - %i", (int)tg->real_start_brushx, (int)tg->real_start_brushy);
		return true;
	}
	else if(tg->type==tu_BLEND)
	{
		sprintf(res, "Blend Starting At: %i - %i", (int)tg->real_start_brushx, (int)tg->real_start_brushy);
		return true;
	}
	else if(tg->type==tu_FLATTEN)
	{
		sprintf(res, "Flatten Starting At: %i - %i", (int)tg->real_start_brushx, (int)tg->real_start_brushy);
		return true;
	}
	return true;
}

bool Apply_TouchUp(TOUCHUP *tu, int type);

bool Apply_TouchUp_Edit_Group(TOUCHUP_EDIT_GROUP *tg)
{
	//save the current selection
	vector<int> old_selection;
	int nsl = Num_Selected_Layers();
	int i;
	for(i = 0;i<nsl;i++)
	{
		old_selection.push_back(Get_Selected_Layer_ID(i));
	}

	//clear the selection
	Select_All_Layers(false);
	frame->Invalidate_Selection_Buffer();

	//select the laters associated with this touchup group
	for(i = 0;i<tg->num_selected_layers;i++)
	{
		Select_Layer(Get_Layer_Index(tg->selected_layer_ids[i]), true);
	}
	frame->Validate_Selection_Buffer();

	//apply the touchup edits
	int n = tg->touchups.size();
	for(i = 0;i<n;i++)
	{
		Apply_TouchUp(tg->touchups[i], tg->type);
	}

	//clear the selection
	Select_All_Layers(false);
	frame->Invalidate_Selection_Buffer();

	//restore the previous selection
	nsl = old_selection.size();
	for(i = 0;i<nsl;i++)
	{
		Select_Layer(Get_Layer_Index(old_selection[i]), true);
	}
	frame->Validate_Selection_Buffer();
	return true;
}

bool Apply_Single_TouchUp_Edit(int index)
{
	int n = touchup_edit_groups.size();
	if(index<0||index>=n)
	{
		return false;
	}
	return Apply_TouchUp_Edit_Group(touchup_edit_groups[index]);
}

bool Remove_Single_TouchUp_Edit(int index)
{
	int n = touchup_edit_groups.size();
	if(index<0||index>=n)
	{
		return false;
	}
	delete touchup_edit_groups[index];
	touchup_edit_groups.erase(touchup_edit_groups.begin()+index);
	Set_TouchUps_Altered(true);
	return true;
}


bool Apply_All_TouchUps()
{
	int n = touchup_edit_groups.size();
	for(int i = 0;i<n;i++)
	{
		Apply_TouchUp_Edit_Group(touchup_edit_groups[i]);
	}
	return true;
}

bool Clear_All_TouchUps()
{
	int n = touchup_edit_groups.size();
	for(int i = 0;i<n;i++)
	{
		delete touchup_edit_groups[i];
	}
	touchup_edit_groups.clear();
	if(n>0)Set_TouchUps_Altered(true);
	return true;
}

bool Apply_TouchUp(TOUCHUP *tu, int type)
{
	float *depth_grid;
	float *brush_strength_grid = Get_Linear_Brush_Strength_Grid(tu->real_brushx, tu->real_brushy, tu->real_brush_size, tu->real_start_x, tu->real_start_y, tu->real_grid_size);
	int total = tu->real_grid_size*tu->real_grid_size;
	float inc_percent = 0.0005f;
	float inc;
	int i;
	if(type==tu_PUSH)
	{
		depth_grid = Get_Brush_Depth_Grid(tu->real_start_x, tu->real_start_y, tu->real_grid_size);
		for(i = 0;i<total;i++)
		{
			inc = depth_grid[i]*inc_percent;
			depth_grid[i] += inc*brush_strength_grid[i];
		}
	}
	else if(type==tu_PULL)
	{
		depth_grid = Get_Brush_Depth_Grid(tu->real_start_x, tu->real_start_y, tu->real_grid_size);
		for(i = 0;i<total;i++)
		{
			inc = depth_grid[i]*inc_percent;
			depth_grid[i] -= inc*brush_strength_grid[i];
		}
	}
	else if(type==tu_BLEND)
	{
		depth_grid = Get_Brush_Selected_Depth_Grid(tu->real_start_x, tu->real_start_y, tu->real_grid_size);
		float *aa_grid = Get_AntiAliased_Brush_Depth_Grid(depth_grid, tu->real_grid_size);
		for(i = 0;i<total;i++)
		{
			depth_grid[i] = (depth_grid[i]*(1.0f-brush_strength_grid[i]))+(aa_grid[i]*brush_strength_grid[i]);
		}
	}
	else if(type==tu_FLATTEN)
	{
		depth_grid = Get_Brush_Selected_Depth_Grid(tu->real_start_x, tu->real_start_y, tu->real_grid_size);
		float grid_average = Get_Averaged_Brush_Depth_Grid(depth_grid, tu->real_grid_size);
		//cut the brush strength down to 10%, flatten happens FAST
		float bs = 0;
		for(i = 0;i<total;i++)
		{
			bs = brush_strength_grid[i]*0.1f;
			depth_grid[i] = (depth_grid[i]*(1.0f-bs))+(grid_average*bs);
		}
	}
	Set_Selected_Depth_Grid(tu->real_start_x, tu->real_start_y, tu->real_grid_size, tu->real_grid_size, depth_grid);
	redraw_frame = true;
	return true;
}

TOUCHUP* New_TouchUp_Data(int type)
{
	TOUCHUP *tu = new TOUCHUP;
	float rx, ry;
	Get_Brush_Position(&rx, &ry);
	tu->Get_Logical_Values(rx, ry, Get_Brush_Size());
	tu->Get_Real_Values();

	Get_Brush_Position(&tu->real_brushx, &tu->real_brushy);

	tu->real_brush_size = Get_Brush_Size();
	tu->real_grid_size = Get_Brush_Grid_Size();
	Get_Brush_Grid_Start(&tu->real_start_x, &tu->real_start_y);
	
	Register_TouchUp(tu, type, tu->logical_brushx, tu->logical_brushy);
	return tu;
}

bool Push_Depth_Within_Brush()
{
	if(Num_Selected_Layers()==0)return false;
	frame->Validate_Selection_Buffer();
	TOUCHUP *tu = New_TouchUp_Data(tu_PUSH);
	Apply_TouchUp(tu, tu_PUSH);
	return true;
}

bool Blend_Depth_Within_Brush()
{
	if(Num_Selected_Layers()==0)return false;
	frame->Validate_Selection_Buffer();
	TOUCHUP *tu = New_TouchUp_Data(tu_BLEND);
	Apply_TouchUp(tu, tu_BLEND);
	return true;
}

bool Flatten_Depth_Within_Brush()
{
	if(Num_Selected_Layers()==0)return false;
	frame->Validate_Selection_Buffer();
	TOUCHUP *tu = New_TouchUp_Data(tu_FLATTEN);
	Apply_TouchUp(tu, tu_FLATTEN);
	return true;
}

bool Pull_Depth_Within_Brush()
{
	if(Num_Selected_Layers()==0)return false;
	frame->Validate_Selection_Buffer();
	TOUCHUP *tu = New_TouchUp_Data(tu_PULL);
	Apply_TouchUp(tu, tu_PULL);
	return true;
}

bool Refresh_Layers_And_Dont_Apply_Touchups()
{
	Project_Layers();
	redraw_frame = true;
	return true;
}

bool Refresh_Layers_And_Apply_Touchups()
{
	Project_Layers();
	Apply_All_TouchUps();
	redraw_frame = true;
	return true;
}

bool Clear_All_TouchUp_Data()
{
	Clear_Stored_TouchUp_Edits();
	Clear_All_TouchUps();
	return true;
}

//////////////////////////////////////////////////////////////////


struct TOUCHUP_SAVE_DATA
{
	float logical_brushx, logical_brushy;
	float logical_brush_size;
};

struct TOUCHUP_EDIT_SAVE_DATA
{
	float logical_start_brushx, logical_start_brushy;
	int type;
	int num_touchups;
	int num_selected_layers;
	int selected_layer_ids[1];//trick out buffer by padding end for extra ints as needed
};

//variable structure size based on number of extra ints
int Touchup_Edit_Save_Structure_Size(int num_selected_layers)
{
	//size of the basic structure with 1 layer id
	int base = sizeof(TOUCHUP_EDIT_SAVE_DATA);
	//bytes needed for extra ints
	int extra = sizeof(int)*(num_selected_layers-1);
	return base + extra;
}

//the total amount of buffer space needed for this touchup group
int Touchup_Save_Buffer_Size(TOUCHUP_EDIT_GROUP *tg)
{
	//size for the structure
	int dsize = Touchup_Edit_Save_Structure_Size(tg->num_selected_layers);
	//size for each mouse op
	int xsize = sizeof(TOUCHUP_SAVE_DATA)*tg->touchups.size();
	return dsize+xsize;
}

//serialize edit group into buffer, uses variable size based on number of extra layer ids needed
//return pointer to the next spot in the buffer
//NOTE: buffer is presized to fit all edit groups
unsigned char* Save_TouchUp_Edit_To_Buffer(TOUCHUP_EDIT_GROUP *tg, unsigned char *buffer)
{
	//size of the structure with space for extra layer ids
	int dsize = Touchup_Edit_Save_Structure_Size(tg->num_selected_layers);
	//cast a pointer to the structure to fill the data
	TOUCHUP_EDIT_SAVE_DATA *tesd = (TOUCHUP_EDIT_SAVE_DATA*)buffer;

	//fill in normal data
	tesd->num_touchups = tg->touchups.size();
	tesd->logical_start_brushx = tg->logical_start_brushx;
	tesd->logical_start_brushy = tg->logical_start_brushy;	
	tesd->type = tg->type;
	tesd->num_selected_layers = tg->num_selected_layers;

	//fill in selected layer ids
	int i;
	for(i = 0;i<tesd->num_selected_layers;i++)
	{
		tesd->selected_layer_ids[i] = tg->selected_layer_ids[i];
	}

	//now save all touchup mouse ops into a series
	TOUCHUP_SAVE_DATA tsd;
	buffer = &buffer[dsize];
	for(i = 0;i<tesd->num_touchups;i++)
	{
		TOUCHUP *tu = tg->touchups[i];
		tsd.logical_brush_size = tu->logical_brush_size;
		tsd.logical_brushx = tu->logical_brushx;
		tsd.logical_brushy = tu->logical_brushy;
		memcpy(buffer, &tsd, sizeof(TOUCHUP_SAVE_DATA));
		buffer = &buffer[sizeof(TOUCHUP_SAVE_DATA)];
	}
	return buffer;
}

int Get_TouchUp_Edits_Buffer_Size()
{
	int res = 0;
	int n = touchup_edit_groups.size();
	for(int i = 0;i<n;i++)
	{
		res += Touchup_Save_Buffer_Size(touchup_edit_groups[i]);
	}
	return res;
}

int Save_TouchUp_Edits_To_Buffer(unsigned char *buffer)
{
	int n = touchup_edit_groups.size();
	int cnt = 0;
	for(int i = 0;i<n;i++)
	{
		Save_TouchUp_Edit_To_Buffer(touchup_edit_groups[i], &buffer[cnt]);
		cnt += Touchup_Save_Buffer_Size(touchup_edit_groups[i]);
	}
	return cnt;
}

struct TOUCHUP_FILE_HEADER
{
	int version;
	int num_edits;
	int buffer_size;
};

bool Save_TouchUps(char *file)
{
	FILE *f = fopen(file, "wb");
	if(!f)
	{
		SkinMsgBox("Can't open file for writing!", file, MB_OK);
		return false;
	}
	TOUCHUP_FILE_HEADER tfh;
	tfh.version = TOUCHUP_FILE_VERSION;
	tfh.buffer_size = Get_TouchUp_Edits_Buffer_Size();
	tfh.num_edits = touchup_edit_groups.size();
	unsigned char *buffer = new unsigned char[tfh.buffer_size];
	Save_TouchUp_Edits_To_Buffer(buffer);
	fwrite(&tfh, 1, sizeof(TOUCHUP_FILE_HEADER), f);
	fwrite(buffer, 1, tfh.buffer_size, f);
	delete[] buffer;
	fclose(f);
	return true;
}

//load edit group data from buffer and return the number of bytes consumed
int Load_TouchUp_Edit_From_Buffer(unsigned char *buffer, int max)
{
	TOUCHUP_EDIT_SAVE_DATA *tesd = (TOUCHUP_EDIT_SAVE_DATA*)buffer;

	//get the size of the structure with all extra layer ids padded at the end
	int dsize = sizeof(TOUCHUP_EDIT_SAVE_DATA) + (sizeof(int)*(tesd->num_selected_layers-1));

	int total = dsize+(tesd->num_touchups*sizeof(TOUCHUP_SAVE_DATA));
	if(total<0||total>max)
	{
		//something went wrong, no bytes were used
		return -1;
	}
	TOUCHUP_EDIT_GROUP *tg = new TOUCHUP_EDIT_GROUP;
	//copy general data
	tg->logical_start_brushx = tesd->logical_start_brushx;
	tg->logical_start_brushy = tesd->logical_start_brushy;
	tg->num_selected_layers = tesd->num_selected_layers;
	tg->type = tesd->type;

	//get the applicable layer ids for this touchup group
	tg->selected_layer_ids = new int[tg->num_selected_layers];
	int i;
	for(i = 0;i<tg->num_selected_layers;i++)
	{
		tg->selected_layer_ids[i] = tesd->selected_layer_ids[i];
	}

	TOUCHUP_SAVE_DATA tsd;
	buffer = &buffer[dsize];
	for(i = 0;i<tesd->num_touchups;i++)
	{
		memcpy(&tsd, buffer, sizeof(TOUCHUP_SAVE_DATA));
		buffer = &buffer[sizeof(TOUCHUP_SAVE_DATA)];
		TOUCHUP *tu = new TOUCHUP;
		tu->logical_brush_size = tsd.logical_brush_size;
		tu->logical_brushx = tsd.logical_brushx;
		tu->logical_brushy = tsd.logical_brushy;
		tu->Get_Real_Values();
		tg->touchups.push_back(tu);
	}
	touchup_edit_groups.push_back(tg);
	return total;
}

bool Load_TouchUp_Edits_From_Buffer(unsigned char *buffer, int num_edits, int max)
{
	int cnt = 0;
	for(int i = 0;i<num_edits;i++)
	{
		int n = Load_TouchUp_Edit_From_Buffer(buffer, max);
		if(n==-1)
		{
			//there was an error loading the edit group
			return false;
		}
		cnt += n;
		if(cnt>max)
		{
			return false;
		}
		buffer = &buffer[n];
	}
	return true;
}

bool Load_TouchUps(char *file)
{
	FILE *f = fopen(file, "rb");
	if(!f)
	{
		return false;
	}
	TOUCHUP_FILE_HEADER tfh;
	fread(&tfh, 1, sizeof(TOUCHUP_FILE_HEADER), f);
	if(tfh.version!=TOUCHUP_FILE_VERSION)
	{
		SkinMsgBox("Error loading touchup data, file version mismatch!", file, MB_OK);
		fclose(f);
		return false;
	}
	unsigned char *buffer = new unsigned char[tfh.buffer_size];
	fread(buffer, 1, tfh.buffer_size, f);
	fclose(f);
	bool res = Load_TouchUp_Edits_From_Buffer(buffer, tfh.num_edits, tfh.buffer_size);
	delete[] buffer;
	List_All_TouchUp_Edits();
	return res;
}

bool Get_TouchUps_Save_File(char *res)
{
	if(!Get_Session_Aux_Data_Filename(res, "tch"))
	{
		return false;
	}
	return true;
}

bool Load_TouchUp_Edits_For_Current_Frame(void *p)
{
	Clear_All_TouchUps();//clear old ones
	List_All_TouchUp_Edits();
	char file[512];
	if(Get_TouchUps_Save_File(file))
	{
		return Load_TouchUps(file);
	}
	Set_TouchUps_Altered(false);
	return false;
}


bool Save_TouchUp_Edits_For_Current_Frame(void *p)
{
	char file[512];
	if(Get_TouchUps_Save_File(file))
	{
		return Save_TouchUps(file);
	}
	return false;
}



//catch all possible events to auto load and save touchup data for any and all frames
//maybe change this in the future to ask?
bool Register_TouchUp_LoadSave_Events()
{
	shared_data_interface.Register_Shared_Data_Update_Callback("FrameChanged", Load_TouchUp_Edits_For_Current_Frame);
	shared_data_interface.Register_Shared_Data_Update_Callback("LoadingSingleFrameSession", Load_TouchUp_Edits_For_Current_Frame);
	shared_data_interface.Register_Shared_Data_Update_Callback("SavingProject", Save_TouchUp_Edits_For_Current_Frame);
	shared_data_interface.Register_Shared_Data_Update_Callback("SavingSingleFrameSession", Save_TouchUp_Edits_For_Current_Frame);
	shared_data_interface.Register_Shared_Data_Update_Callback("FrameAboutToChange", Save_TouchUp_Edits_For_Current_Frame);
	return false;
}


