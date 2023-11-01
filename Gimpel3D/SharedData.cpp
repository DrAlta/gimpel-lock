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
#include "G3DCoreInterface.h"
#include "Plugins.h"
#include "Skin.h"
#include "G3DCoreFiles/Frame.h"
#include <vector>
#include <list>


//internal core code that accesses shared data and events
bool Init_Alignment_Selection_Changed_Callback();
bool Register_TouchUp_LoadSave_Events();
bool Register_Halo_LoadSave_Events();
bool Register_AnchorPoints_SharedData_Events();


/////////////////////////////

//init all callbacks for various core modules that access shared data
bool Init_Core_Shared_Data_Access()
{
	Init_Alignment_Selection_Changed_Callback();
	Register_TouchUp_LoadSave_Events();
	Register_Halo_LoadSave_Events();
	Register_AnchorPoints_SharedData_Events();
	return true;
}

using namespace std;

G3DSharedDataInterface g3d_shared_data_interface;

class G3D_SHARED_DATA
{
public:
	G3D_SHARED_DATA()
	{
		dsize = 0;
	}
	~G3D_SHARED_DATA()
	{
		update_callbacks.clear();
	}
	char name[64];
	int dsize;
	void *data;
	vector<_Shared_Data_Update_Callback> update_callbacks;
};

class DELAYED_SHARED_DATA_UPDATE
{
public:
	DELAYED_SHARED_DATA_UPDATE()
	{
	}
	~DELAYED_SHARED_DATA_UPDATE()
	{
	}
	float delay;
	float time;
	int id;
};

//shared data types
vector <G3D_SHARED_DATA*> g3d_shared_data;

//delayed shared data updates
list <DELAYED_SHARED_DATA_UPDATE> delayed_shared_data_updates;

//stack for "current event" being executed
vector<int> current_events;

bool Shared_Data_Exists(char *name)
{
	int n = g3d_shared_data.size();
	for(int i = 0;i<n;i++)
	{
		if(!strcmp(g3d_shared_data[i]->name, name))
		{
			return true;
		}
	}
	return false;
}


G3DSharedDataInterface shared_data_interface;

int G3DSharedDataInterface::Num_Shared_Data()
{
	return g3d_shared_data.size();
}

bool G3DSharedDataInterface::Shared_Data_Info(int id, char *name, int *dsize)
{
	if(id>=Num_Shared_Data())return false;
	strcpy(name, g3d_shared_data[id]->name);
	if(dsize) *dsize = g3d_shared_data[id]->dsize;
	return true;
}

bool G3DSharedDataInterface::Register_Shared_Data(char *name, int dsize, void *data, int *res)
{
	if(Shared_Data_Exists(name))return false;
	G3D_SHARED_DATA *sd = new G3D_SHARED_DATA;
	strcpy(sd->name, name);
	sd->dsize = dsize;
	sd->data = data;
	g3d_shared_data.push_back(sd);
	*res = g3d_shared_data.size()-1;
	return true;
}

bool G3DSharedDataInterface::Find_Shared_Data(char *name, int *dsize, int *res)
{
	int n = g3d_shared_data.size();
	for(int i = 0;i<n;i++)
	{
		if(!strcmp(g3d_shared_data[i]->name, name))
		{
			if(dsize)
			{
				*dsize = g3d_shared_data[i]->dsize;
			}
			*res = i;
			return true;
		}
	}
	Print_To_Console("Warning! Failed attempt to access shared data \"%s\"\n", name);
	return false;
}

bool G3DSharedDataInterface::Register_Shared_Data_Update_Callback(int id, void *callback)
{
	if(id>=(int)g3d_shared_data.size())return false;
	g3d_shared_data[id]->update_callbacks.push_back((_Shared_Data_Update_Callback)callback);
	return true;
}

bool G3DSharedDataInterface::Register_Shared_Data_Update_Callback(char *name, void *callback)
{
	int id, dsize;
	if(!Find_Shared_Data(name, &dsize, &id))
	{
		return false;
	}
	return Register_Shared_Data_Update_Callback(id, callback);
}

bool G3DSharedDataInterface::Set_Shared_Data(int id, int dsize, void *data)
{
	if(id>=(int)g3d_shared_data.size())return false;
	memcpy(g3d_shared_data[id]->data, data, dsize);
	return true;
}

bool G3DSharedDataInterface::Notify_Shared_Data_Update(int id)
{
	int n = g3d_shared_data.size();
	if(id<0||id>=n)return false;
	G3D_SHARED_DATA *gsd = g3d_shared_data[id];
	n = gsd->update_callbacks.size();
	current_events.push_back(id);
	for(int i = 0;i<n;i++)
	{
		gsd->update_callbacks[i](gsd->data);
	}
	current_events.pop_back();
	return true;
}

bool G3DSharedDataInterface::Register_Delayed_Shared_Data_Update(int id, float delay)
{
	if(id>=Num_Shared_Data())return false;
	DELAYED_SHARED_DATA_UPDATE d;
	d.id = id;
	d.delay = delay;
	d.time = 0;
	delayed_shared_data_updates.push_back(d);
	return true;
}

bool G3DSharedDataInterface::Get_Shared_Data(int id, void **res)
{
	if(id>=Num_Shared_Data())return false;
	*res = g3d_shared_data[id]->data;
	return true;
}

bool G3DSharedDataInterface::Get_Current_Callback_Data_Type(int *res)
{
	int n = current_events.size();
	if(n==0)
	{
		return false;
	}
	*res = current_events[n-1];
	return true;
}


bool Check_Delayed_Shared_Data_Updates(float elapsed_time)
{
	int n = delayed_shared_data_updates.size();
	list<DELAYED_SHARED_DATA_UPDATE>::iterator i = delayed_shared_data_updates.begin();
	list<DELAYED_SHARED_DATA_UPDATE>::iterator last;
	while(i!=delayed_shared_data_updates.end())
	{
		last = i;
		i++;
		last->time += elapsed_time;
		if(last->time>=last->delay)
		{
			shared_data_interface.Notify_Shared_Data_Update(last->id);
			delayed_shared_data_updates.erase(last);
		}
	}
	return true;
}

//the main class used to track all interfaces (core or plugin-provided)
extern G3D_INTERFACES _g3d_interfaces;

bool Init_Shared_Data_Interface()
{
	_g3d_interfaces.Register_Interface("G3DSharedDataInterface", &shared_data_interface);
	return true;
}

bool Free_Shared_Data_Interface()
{
	int n = g3d_shared_data.size();
	for(int i = 0;i<n;i++)
	{
		delete g3d_shared_data[i];
	}
	g3d_shared_data.clear();
	return true;
}

//ids for all core shared events and data
int e_Render2D = -1;
int e_Render3D = -1;
int e_MouseMove2D = -1;
int e_MouseMove3D = -1;
int e_FrameChange = -1;
int e_KeysResetFor2DWindow = -1;
int e_KeysResetFor3DWindow = -1;
int e_KeyStateFor2DWindow = -1;
int e_KeyStateFor3DWindow = -1;
int e_NewProjectOpened = -1;
int e_SavingProject = -1;
int e_SelectionChanged = -1;
int e_AllLayersReProjected = -1;
int e_FrameChanged = -1;

int e_LoadingSingleFrameSession = -1;
int e_SavingSingleFrameSession = -1;

int e_FrameAboutToChange = -1;

int e_LayerReProjected = -1;

int e_FeaturePointAdded = -1;
int e_FeaturePointDeleted = -1;
int e_FeaturePointMoved = -1;
int e_FeaturePointSelectionChanged = -1;
int e_FeaturePointsCleared = -1;
int e_FeaturePointsLoaded = -1;
int e_FeaturePointsSaved = -1;



MouseMove2D _MouseMove2D;
MouseMove3D _MouseMove3D;
FrameChange _FrameChange;
KeyStateFor2DWindow _KeyStateFor2DWindow;
KeyStateFor3DWindow _KeyStateFor3DWindow;

LayerReProjected _LayerReProjected;

FeaturePointInfo _FeaturePointInfo;


/*
#include <gl/gl.h>

bool Respond_To_Render_2D_Event(void *p)
{
	static int id = 0;
	id++;
	Print_To_Console("got render 2d event %i", id);
	if(frame)
	{
		glBegin(GL_LINES);
		glVertex2f(-10,-10);
		glVertex2f(frame->width+10, frame->height+10);
		glEnd();
	}
	return true;
}

bool Respond_To_Render_3D_Event(void *p)
{
	static int id = 0;
	id++;
	Print_Status("got render 3d event %i", id);
	if(frame)
	{
		glBegin(GL_LINES);
		glVertex3fv(frame->Get_Pos(0,0));
		glVertex3fv(frame->Get_Pos(frame->width-1,frame->height-1));
		glEnd();
	}
	return true;
}
*/

bool Register_Core_Shared_Data()
{
	shared_data_interface.Register_Shared_Data("Render2D", 0, 0, &e_Render2D);
	shared_data_interface.Register_Shared_Data("Render3D", 0, 0, &e_Render3D);
	shared_data_interface.Register_Shared_Data("MouseMove2D", sizeof(MouseMove2D), &_MouseMove2D, &e_MouseMove2D);
	shared_data_interface.Register_Shared_Data("MouseMove3D", sizeof(MouseMove3D), &_MouseMove3D, &e_MouseMove3D);
	shared_data_interface.Register_Shared_Data("FrameChange", sizeof(FrameChange), &_FrameChange, &e_FrameChange);
	shared_data_interface.Register_Shared_Data("KeysResetFor2DWindow", 0, 0, &e_KeysResetFor2DWindow);
	shared_data_interface.Register_Shared_Data("KeysResetFor3DWindow", 0, 0, &e_KeysResetFor3DWindow);

	shared_data_interface.Register_Shared_Data("KeyStateFor2DWindow", sizeof(KeyStateFor2DWindow), &_KeyStateFor2DWindow, &e_KeyStateFor2DWindow);
	shared_data_interface.Register_Shared_Data("KeyStateFor3DWindow", sizeof(KeyStateFor3DWindow), &_KeyStateFor3DWindow, &e_KeyStateFor3DWindow);

	shared_data_interface.Register_Shared_Data("NewProjectOpened", 0, 0, &e_NewProjectOpened);
	shared_data_interface.Register_Shared_Data("SavingProject", 0, 0, &e_SavingProject);
	
	shared_data_interface.Register_Shared_Data("SelectionChanged", 0, 0, &e_SelectionChanged);
	shared_data_interface.Register_Shared_Data("AllLayersReProjected", 0, 0, &e_AllLayersReProjected);

	shared_data_interface.Register_Shared_Data("FrameChanged", 0, 0, &e_FrameChanged);

	shared_data_interface.Register_Shared_Data("LoadingSingleFrameSession", 0, 0, &e_LoadingSingleFrameSession);
	shared_data_interface.Register_Shared_Data("SavingSingleFrameSession", 0, 0, &e_SavingSingleFrameSession);

	shared_data_interface.Register_Shared_Data("FrameAboutToChange", 0, 0, &e_FrameAboutToChange);

	shared_data_interface.Register_Shared_Data("LayerReProjected", sizeof(LayerReProjected), &_LayerReProjected, &e_LayerReProjected);

	shared_data_interface.Register_Shared_Data("FeaturePointAdded", sizeof(FeaturePointInfo), &_FeaturePointInfo, &e_FeaturePointAdded);
	shared_data_interface.Register_Shared_Data("FeaturePointDeleted", sizeof(FeaturePointInfo), &_FeaturePointInfo, &e_FeaturePointDeleted);
	shared_data_interface.Register_Shared_Data("FeaturePointMoved", sizeof(FeaturePointInfo), &_FeaturePointInfo, &e_FeaturePointMoved);
	shared_data_interface.Register_Shared_Data("FeaturePointSelectionChanged", sizeof(FeaturePointInfo), &_FeaturePointInfo, &e_FeaturePointSelectionChanged);
	shared_data_interface.Register_Shared_Data("FeaturePointsCleared", 0, 0, &e_FeaturePointsCleared);
	shared_data_interface.Register_Shared_Data("FeaturePointsLoaded", 0, 0, &e_FeaturePointsLoaded);
	shared_data_interface.Register_Shared_Data("FeaturePointsSaved", 0, 0, &e_FeaturePointsSaved);

	Init_Core_Shared_Data_Access();
	return true;
}

bool Report_Render2D_Event()
{
	shared_data_interface.Notify_Shared_Data_Update(e_Render2D);
	return true;
}

bool Report_Render3D_Event()
{
	shared_data_interface.Notify_Shared_Data_Update(e_Render3D);
	return true;
}

bool Report_MouseMove2D(int oldx, int oldy, int newx, int newy, float pixel_x, float pixel_y)
{
	_MouseMove2D.oldx = oldx;
	_MouseMove2D.oldy = oldy;
	_MouseMove2D.newx = newx;
	_MouseMove2D.newy = newy;
	_MouseMove2D.pixel_x = pixel_x;
	_MouseMove2D.pixel_y = pixel_y;
	shared_data_interface.Notify_Shared_Data_Update(e_MouseMove2D);
	return true;
};

bool Report_MouseMove3D(int oldx, int oldy, int newx, int newy)
{
	_MouseMove3D.oldx = oldx;
	_MouseMove3D.oldy = oldy;
	_MouseMove3D.newx = newx;
	_MouseMove3D.newy = newy;
	shared_data_interface.Notify_Shared_Data_Update(e_MouseMove3D);
	return true;
};


bool Report_Frame_Change(int old_frame_index, int new_frame_index, int width, int height)
{
	_FrameChange.old_frame_index = old_frame_index;
	_FrameChange.new_frame_index = new_frame_index;
	_FrameChange.width = width;
	_FrameChange.height = height;
	shared_data_interface.Notify_Shared_Data_Update(e_FrameChange);
	return true;
}

bool Report_2D_Window_KeyState(int key, bool pressed)
{
	_KeyStateFor2DWindow.key = key;
	_KeyStateFor2DWindow.pressed = pressed;
	shared_data_interface.Notify_Shared_Data_Update(e_KeyStateFor2DWindow);
	return true;
}

bool Report_3D_Window_KeyState(int key, bool pressed)
{
	_KeyStateFor3DWindow.key = key;
	_KeyStateFor3DWindow.pressed = pressed;
	shared_data_interface.Notify_Shared_Data_Update(e_KeyStateFor3DWindow);
	return true;
}

bool Report_2D_Window_Keys_Cleared()
{
	shared_data_interface.Notify_Shared_Data_Update(e_KeysResetFor2DWindow);
	return true;
}

bool Report_3D_Window_Keys_Cleared()
{
	shared_data_interface.Notify_Shared_Data_Update(e_KeysResetFor3DWindow);
	return true;
}

bool Report_New_Project_Opened()
{
	shared_data_interface.Notify_Shared_Data_Update(e_NewProjectOpened);
	return true;
}

bool Report_Saving_Project()
{
	shared_data_interface.Notify_Shared_Data_Update(e_SavingProject);
	return true;
}

bool Report_Selection_Changed()
{
	shared_data_interface.Notify_Shared_Data_Update(e_SelectionChanged);
	return true;
}

bool Report_Layers_ReProjected()
{
	shared_data_interface.Notify_Shared_Data_Update(e_AllLayersReProjected);
	return true;
}

bool Report_Layer_ReProjected(int layer_id)
{
	_LayerReProjected.layer_id = layer_id;
	shared_data_interface.Notify_Shared_Data_Update(e_LayerReProjected);
	return true;
}

bool Report_Frame_Changed()
{
	shared_data_interface.Notify_Shared_Data_Update(e_FrameChanged);
	return true;
}

bool Report_Loading_Single_Frame_Session()
{
	shared_data_interface.Notify_Shared_Data_Update(e_LoadingSingleFrameSession);
	return true;
}

bool Report_Saving_Single_Frame_Session()
{
	shared_data_interface.Notify_Shared_Data_Update(e_SavingSingleFrameSession);
	return true;
}

bool Clear_Alignment_Points();
bool Stop_Alignment();


bool Report_Frame_About_To_Change()
{
	shared_data_interface.Notify_Shared_Data_Update(e_FrameAboutToChange);
	Stop_Alignment();
	Clear_Alignment_Points();
	return true;
}

bool Report_Feature_Point_Added(int id)
{
	_FeaturePointInfo.unique_id = id;
	shared_data_interface.Notify_Shared_Data_Update(e_FeaturePointAdded);
	return true;
}

bool Report_Feature_Point_Deleted(int id)
{
	_FeaturePointInfo.unique_id = id;
	shared_data_interface.Notify_Shared_Data_Update(e_FeaturePointDeleted);
	return true;
}

bool Report_Feature_Point_Moved(int id)
{
	_FeaturePointInfo.unique_id = id;
	shared_data_interface.Notify_Shared_Data_Update(e_FeaturePointMoved);
	return true;
}

bool Report_Feature_Point_Selection_Changed(int id)
{
	_FeaturePointInfo.unique_id = id;
	shared_data_interface.Notify_Shared_Data_Update(e_FeaturePointSelectionChanged);
	return true;
}

bool Report_Feature_Points_Cleared()
{
	shared_data_interface.Notify_Shared_Data_Update(e_FeaturePointsCleared);
	return true;
}

bool Report_Feature_Points_Loaded()
{
	shared_data_interface.Notify_Shared_Data_Update(e_FeaturePointsLoaded);
	return true;
}

bool Report_Feature_Points_Saved()
{
	shared_data_interface.Notify_Shared_Data_Update(e_FeaturePointsSaved);
	return true;
}
