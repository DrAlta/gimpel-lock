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
#ifndef G3D_TOOL_PLUGIN_H
#define G3D_TOOL_PLUGIN_H


#define G3D_SDK_VERSION 42

////////////////////////////////////////////////////

//main class for accessing all public interfaces
//plugins can register custom interfaces as needed
//to work together

class G3D_INTERFACES
{
public:
	G3D_INTERFACES();
	~G3D_INTERFACES();
	virtual int Num_Interfaces();
	virtual bool Interface_Name(int id, char *res);
	virtual int Find_Interface(char *name);
	virtual bool Register_Interface(char *name, void *p);
	virtual void* Get_Interface(int id);
};

//prototype base class for the plugin, each plugin
//will create a derived class

class G3D_PLUGIN
{
public:
	G3D_PLUGIN(){}
	~G3D_PLUGIN(){}
	virtual bool Init_Plugin(){return false;}
	virtual bool Free_Plugin(){return false;}
	virtual bool Open_Plugin(){return false;}
	virtual bool Close_Plugin(){return false;}
	virtual bool Get_Plugin_Name(char *res){return false;}
};

//the only exported function required in the dll
//a pointer to the interfaces class is provided, the plugin
//will use this for access to all functionality
//the plugin returns a pointer to its derived plugin class
//the version number ensures only compatible plugins are loaded
typedef G3D_PLUGIN* (*_Init_G3D_Plugin_Interface)(G3D_INTERFACES *p, int version);


//prototype class for a public interface exposed by the core or other plugins
class G3D_PUBLIC_INTERFACE
{
public:
	G3D_PUBLIC_INTERFACE()
	{
	}
	~G3D_PUBLIC_INTERFACE()
	{
	}
	char name[64];
	void *p;
};
//NOTE: this class is used as a means of accessing the shared data interface and ALL
//other main interfaces..


//include definitions for interfaces provided by the core
#include "G3DPluginInterfaces.h"

//a variable that should be declared IN THE PLUGIN to access the core (and other plugin) interfaces
extern G3D_INTERFACES *G3D_interfaces;
//a pointer is passed to the plugin at init time, the plugin should maintain
//a local copy "G3D_INTERFACES *G3D_interfaces" for access throughout the plugin code

//a plugin variable used to access the skinning system, should be standard for most plugins
extern G3DSkinInterface *skin_interface;



/////////////////////////////////////////////////////////
//Shared data updates use callback functions to pass a void pointer to data

//The following callback events are generated within the core, the associated data
//structures (if any) for each event are show after the text name.


//<text name> - <data passed to callback function>

//"Render2D" - NULL
//"Render3D" - NULL


struct MouseMove2D
{
	int oldx, oldy;
	int newx, newy;
	float pixel_x, pixel_y;
};

//"MouseMove2D" - MouseMove2D

struct MouseMove3D
{
	int oldx, oldy;
	int newx, newy;
};

//"MouseMove3D" - MouseMove3D

struct FrameChange
{
	int old_frame_index;
	int new_frame_index;
	int width, height;
};

//"KeyStateFor2DWindow" - KeyStateFor2DWindow

struct KeyStateFor2DWindow
{
	int key;
	bool pressed;
};

//"KeyStateFor3DWindow" - KeyStateFor3DWindow

struct KeyStateFor3DWindow
{
	int key;
	bool pressed;
};

//"KeyStateFor3DWindow" - KeyStateFor3DWindow

//"NewProjectOpened" - NULL

//"SavingProject" - NULL

//"SelectionChanged" - NULL

//"AllLayersReProjected" - NULL

//"FrameChanged" - NULL

//"LoadingSingleFrameSession" - NULL

//"SavingSingleFrameSession" - NULL

//"FrameAboutToChange" - NULL

struct LayerReProjected
{
	int layer_id;
};

//"LayerReProjected" - LayerReProjected

struct FeaturePointInfo
{
	int unique_id;
};

//"FeaturePointAdded" - FeaturePointInfo

//"FeaturePointDeleted" - FeaturePointInfo

//"FeaturePointMoved" - FeaturePointInfo

//"FeaturePointSelectionChanged" - FeaturePointInfo

//"FeaturePointsCleared" - NULL

//"FeaturePointsLoaded" - NULL

//"FeaturePointsSaved" - NULL









#endif
