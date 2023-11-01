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
#include <vector>

using namespace std;

bool Find_Files_In_Path(char *path, vector<char*> *files);

bool Enable_Plugin_Tooltray_Item(char *text, bool enable);


//generic crap to open and close dialog
bool plugins_tool_open = false;

bool Open_Plugins_Tool()
{
	if(plugins_tool_open)return false;
	Open_Plugins_Dialog();
	plugins_tool_open = true;
	return true;
}

bool Close_Plugins_Tool()
{
	if(!plugins_tool_open)return false;
	Close_Plugins_Dialog();
	plugins_tool_open = false;
	return true;
}

////////////////////////////////////////////////////////////////////////////

//core interfaces manager for core and inter-plugin communication
G3D_INTERFACES _g3d_interfaces;

//interfaces registered by plugins and/or core
vector<G3D_PUBLIC_INTERFACE*> public_interfaces;

G3D_INTERFACES::G3D_INTERFACES(){}
G3D_INTERFACES::~G3D_INTERFACES(){}

int G3D_INTERFACES::Num_Interfaces()
{
	return public_interfaces.size();
}

bool G3D_INTERFACES::Interface_Name(int id, char *res)
{
	if(id>=Num_Interfaces()||id<0)return false;
	strcpy(res, public_interfaces[id]->name);
	return true;
}

int G3D_INTERFACES::Find_Interface(char *name)
{
	unsigned int n = Num_Interfaces();
	unsigned int i;
	for(i = 0;i<n;i++)
	{
		if(!strcmp(public_interfaces[i]->name, name))
		{
			return i;
		}
	}
	printf("Warning! Attempt to access unknown interface \"%s\"\nAvailable interfaces:", name);
	for(i = 0;i<n;i++)
	{
		printf("\"%s\"\n", public_interfaces[i]->name);
	}
	return -1;
}

bool Interface_Exists(char *name)
{
	unsigned int n = public_interfaces.size();
	for(unsigned int i = 0;i<n;i++)
	{
		if(!strcmp(public_interfaces[i]->name, name))
		{
			return true;
		}
	}
	return false;
}

bool G3D_INTERFACES::Register_Interface(char *name, void *p)
{
	if(Interface_Exists(name))return false;
	G3D_PUBLIC_INTERFACE *gpi = new G3D_PUBLIC_INTERFACE;
	strcpy(gpi->name, name);
	gpi->p = p;
	public_interfaces.push_back(gpi);
	return false;
}

void* G3D_INTERFACES::Get_Interface(int id)
{
	if(id>=Num_Interfaces()||id<0)return false;
	return public_interfaces[id]->p;
}

bool Free_All_Public_Interfaces()
{
	int n = public_interfaces.size();
	for(int i = 0;i<n;i++)
	{
		delete public_interfaces[i];
	}
	public_interfaces.clear();
	return true;
}


class G3D_PLUGIN_HOLDER
{
public:
	G3D_PLUGIN_HOLDER()
	{
		handle = 0;
		pinterface = 0;
	}
	~G3D_PLUGIN_HOLDER()
	{
		if(pinterface)pinterface->Free_Plugin();
		if(handle)FreeLibrary(handle);
	}
	char file[512];
	char name[64];
	HMODULE handle;
	G3D_PLUGIN *pinterface;
};

vector<G3D_PLUGIN_HOLDER*> g3d_plugins;

bool Is_DLL(char *file)
{
	char *c = strrchr(file, '.');
	if(!c)
	{
		return false;
	}
	if(!strcmp(c, ".dll"))return true;
	return false;
}

bool Load_Plugin(char *file)
{
	G3D_PLUGIN_HOLDER *pp = new G3D_PLUGIN_HOLDER;
	pp->handle = LoadLibrary(file);
	if(!pp->handle)
	{
		SkinMsgBox(0, "Error loading plugin:\nCan't load library", file, MB_OK);
		delete pp;
		return false;
	}
	_Init_G3D_Plugin_Interface spi = (_Init_G3D_Plugin_Interface)GetProcAddress(pp->handle, "Init_G3D_Plugin_Interface");
	if(!spi)
	{
		SkinMsgBox(0, "Error loading plugin:\nCan't get plugin init interface", file, MB_OK);
		delete pp;
		return false;
	}
	pp->pinterface = (G3D_PLUGIN*)spi(&_g3d_interfaces, G3D_SDK_VERSION);
	if(!pp->pinterface)
	{
		SkinMsgBox(0, "Error loading plugin:\nInvalid plugin interface", file, MB_OK);
		delete pp;
		return false;
	}
	Print_Status("Loaded plugin %s", file);
	strcpy(pp->file, file);
	pp->pinterface->Get_Plugin_Name(pp->name);
	pp->pinterface->Init_Plugin();
	g3d_plugins.push_back(pp);
	Enable_Plugin_Tooltray_Item(pp->name, true);
	return true;
}

bool Load_Plugins(char *path)
{
	vector<char*> files;
	Find_Files_In_Path(path, &files);
	int n = files.size();
	for(int i = 0;i<n;i++)
	{
		if(Is_DLL(files[i]))
		{
			Load_Plugin(files[i]);
		}
		delete[] files[i];
	}
	files.clear();
	Print_Status("");
	return true;
}

bool Unload_Plugins()
{
	int n = g3d_plugins.size();
	for(int i = 0;i<n;i++)
	{
		G3D_PLUGIN_HOLDER *ph = g3d_plugins[i];
	//NOTE: plugins can crash themselves when calling the virtual Close_Plugin and Free_Plugin functions, going to skip this for now
//		ph->pinterface->Close_Plugin();
//		ph->pinterface->Free_Plugin();
		FreeLibrary(ph->handle);
		ph->pinterface = 0;
		ph->handle = 0;
		Enable_Plugin_Tooltray_Item(ph->name, false);
		delete ph;
	}
	g3d_plugins.clear();
	return true;
}

int Num_Plugins()
{
	return g3d_plugins.size();
}

bool Get_Plugin_Name(int index, char *res)
{
	int n = g3d_plugins.size();
	if(index>=n)return false;
	strcpy(res, g3d_plugins[index]->name);
	return true;
}

G3D_PLUGIN_HOLDER* Find_Plugin(char *name)
{
	int n = g3d_plugins.size();
	for(int i = 0;i<n;i++)
	{
		if(!strcmp(g3d_plugins[i]->name, name))
		{
			return g3d_plugins[i];
		}
	}
	return 0;
}

bool Open_Plugin(char *name)
{
	G3D_PLUGIN_HOLDER *pp = Find_Plugin(name);
	if(!pp)return false;
	pp->pinterface->Open_Plugin();
	return true;
}

bool Plugin_Loaded(char *name)
{
	G3D_PLUGIN_HOLDER *pp = Find_Plugin(name);
	if(!pp)return false;
	pp->pinterface->Open_Plugin();
	return true;
}

bool Close_All_Plugins()
{
	int n = g3d_plugins.size();
	for(int i = 0;i<n;i++)
	{
		g3d_plugins[i]->pinterface->Close_Plugin();
	}
	return true;
}

bool Reload_Plugin(char *name)
{
	char file[512];
	int n = g3d_plugins.size();
	for(int i = 0;i<n;i++)
	{
		if(!strcmp(g3d_plugins[i]->name, name))
		{
			g3d_plugins[i]->pinterface->Close_Plugin();
			g3d_plugins[i]->pinterface->Free_Plugin();
			FreeLibrary(g3d_plugins[i]->handle);
			strcpy(file, g3d_plugins[i]->file);
			g3d_plugins.erase(g3d_plugins.begin()+i);
			delete g3d_plugins[i];
			return Load_Plugin(file);
		}
	}
	return false;
}

bool Reload_All_Plugins()
{
	vector<char*> ofiles;
	int n = g3d_plugins.size();
	int i;
	for(i = 0;i<n;i++)
	{
		char *c = new char[512];
		strcpy(c, g3d_plugins[i]->file);
		ofiles.push_back(c);
	}
	Unload_Plugins();
	for(i = 0;i<n;i++)
	{
		Load_Plugin(ofiles[i]);
		delete[] ofiles[i];
	}
	ofiles.clear();
	return true;
}

bool Unload_Plugin(char *name)
{
	char file[512];
	int n = g3d_plugins.size();
	for(int i = 0;i<n;i++)
	{
		if(!strcmp(g3d_plugins[i]->name, name))
		{
			g3d_plugins[i]->pinterface->Close_Plugin();
			g3d_plugins[i]->pinterface->Free_Plugin();
			FreeLibrary(g3d_plugins[i]->handle);
			g3d_plugins[i]->pinterface = 0;
			g3d_plugins[i]->handle = 0;
			strcpy(file, g3d_plugins[i]->file);
			Enable_Plugin_Tooltray_Item(g3d_plugins[i]->name, false);
			delete g3d_plugins[i];
			g3d_plugins.erase(g3d_plugins.begin()+i);
			return true;
		}
	}
	return false;
}

bool Unload_All_Plugins()
{
	return Unload_Plugins();
}

bool Load_Custom_Plugin(char *file)
{
	return Load_Plugin(file);
}
