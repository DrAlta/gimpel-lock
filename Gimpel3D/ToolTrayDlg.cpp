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
// ToolTrayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "ToolTrayDlg.h"
#include "SkinFrame.h"
#include "Skin.h"
#include "SkinButton.h"
#include "Plugins.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int tt_command_id = 25000;

bool Resize_ToolTray_Dlg();
bool Resize_Frames_Dlg();
bool Resize_Layers_Dlg();

extern bool hide_lists;
extern bool switch_lists_side;

extern int sidebar_top_border;
extern int sidebar_bottom_border;
extern int sidebar_width;
extern int sidebar_restore_width;

int tooltray_height = 50;
int tooltray_item_height = 18;
int tooltray_control_button_width = 18;

bool Close_SkinFrame(int restore_control_id);

ToolTrayDlg *toolTrayDlg = 0;

ToolTrayDlg::ToolTrayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ToolTrayDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ToolTrayDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	toolTrayDlg = this;
}


void ToolTrayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ToolTrayDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ToolTrayDlg, CDialog)
	//{{AFX_MSG_MAP(ToolTrayDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ToolTrayDlg message handlers

class TOOLTRAY_ITEM
{
public:
	TOOLTRAY_ITEM()
	{
	}
	~TOOLTRAY_ITEM()
	{
	}
	CSkinButton restore_button;
	CSkinButton up_button;
	CSkinButton down_button;
	CSkinButton remove_button;
	char text[32];
	int restore_button_id;
	int up_button_id;
	int down_button_id;
	int remove_button_id;
	bool visible;
	int restore_control_id;
};

vector<TOOLTRAY_ITEM*> tooltray_items;

int Find_ToolTray_Item(int restore_control_id)
{
	int n = tooltray_items.size();
	for(int i = 0;i<n;i++)
	{
		if(tooltray_items[i]->restore_control_id==restore_control_id)
		{
			return i;
		}
	}
	return -1;
}

int Find_ToolTray_Item(char *text)
{
	int n = tooltray_items.size();
	for(int i = 0;i<n;i++)
	{
		if(!strcmp(tooltray_items[i]->text, text))
		{
			return i;
		}
	}
	return -1;
}

int Find_ToolTray_Item(char *text, int restore_control_id)
{
	int n = tooltray_items.size();
	for(int i = 0;i<n;i++)
	{
		if(tooltray_items[i]->restore_control_id==restore_control_id)
		{
			if(!strcmp(tooltray_items[i]->text, text))
			{
				return i;
			}
		}
	}
	return -1;
}

bool Notify_ToolTray_Item_Visible(int restore_control_id, char *text, bool visible)
{
	int index = Find_ToolTray_Item(text);
	if(index==-1)
	{
		return false;
	}
	TOOLTRAY_ITEM *tt = tooltray_items[index];
	tt->restore_button.render_pushed = visible;
	tt->restore_button.Invalidate();
	return true;
}

bool ToolTray_Item_Exists(int restore_control_id)
{
	int index = Find_ToolTray_Item(restore_control_id);
	if(index==-1)
	{
		return false;
	}
	return true;
}

bool Try_Add_ToolTray_Item(char *text, int restore_control_id)
{
	int index = Find_ToolTray_Item(text, restore_control_id);
	if(index!=-1)
	{
		return false;
	}
	TOOLTRAY_ITEM *tt = new TOOLTRAY_ITEM;
	strcpy(tt->text, text);
	CRect rect;
	rect.top = 0;
	rect.bottom = 10;
	rect.left = 0;
	rect.right = 10;
	tt->visible = false;
	tt->restore_button_id = tt_command_id;tt_command_id++;
	tt->up_button_id = tt_command_id;tt_command_id++;
	tt->down_button_id = tt_command_id;tt_command_id++;
	tt->remove_button_id = tt_command_id;tt_command_id++;
	tt->restore_control_id = restore_control_id;
	tt->restore_button.Create(text, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW | BS_VCENTER, CRect(0,0,10,10), toolTrayDlg, tt->restore_button_id);
	tt->up_button.Create("^", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW | BS_VCENTER, CRect(0,0,10,10), toolTrayDlg, tt->up_button_id);
	tt->down_button.Create("v", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW | BS_VCENTER, CRect(0,0,10,10), toolTrayDlg, tt->down_button_id);
	tt->remove_button.Create("X", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW | BS_VCENTER, CRect(0,0,10,10), toolTrayDlg, tt->remove_button_id);
	tt->restore_button.SetFont(skin_font);
	tt->up_button.SetFont(skin_font);
	tt->down_button.SetFont(skin_font);
	tt->remove_button.SetFont(skin_font);
	tooltray_items.push_back(tt);
	//check for plugin that's not loaded
	if(restore_control_id==-1)
	{
		if(!Plugin_Loaded(text))
		{
			tt->restore_button.EnableWindow(false);
			tt->restore_button.SetFont(skin_cross_out_font);
		}
	}
	return true;
}

bool Enable_Plugin_Tooltray_Item(char *text, bool enable)
{
	int i = Find_ToolTray_Item(text, -1);
	if(i==-1)return false;
	tooltray_items[i]->restore_button.EnableWindow(enable);
	if(enable)
	{
		tooltray_items[i]->restore_button.SetFont(skin_font);
	}
	else
	{
		tooltray_items[i]->restore_button.SetFont(skin_cross_out_font);
	}
	return true;
}

bool Try_Remove_ToolTray_Item(int restore_control_id)//SkinFrame *sf)
{
	int index = Find_ToolTray_Item(restore_control_id);//sf);
	if(index==-1)
	{
		return false;
	}
	TOOLTRAY_ITEM *tt = tooltray_items[index];
	delete tt;
	tooltray_items.erase(tooltray_items.begin()+index);
	return true;
}

bool Add_ToolTray_Item(char *text, int restore_control_id)
{
	if(Try_Add_ToolTray_Item(text, restore_control_id))
	{
		Resize_ToolTray_Dlg();
		Resize_Frames_Dlg();
		Resize_Layers_Dlg();
		return true;
	}
	return false;
}

bool Remove_ToolTray_Item(int restore_control_id)
{
	if(Try_Remove_ToolTray_Item(restore_control_id))
	{
		Resize_ToolTray_Dlg();
		Resize_Frames_Dlg();
		Resize_Layers_Dlg();
		return true;
	}
	return false;
}

bool Resize_ToolTray_Items()
{
	int n = tooltray_items.size();
	int rb_width = sidebar_width-(tooltray_control_button_width*3);
	float x0 = 0;
	float x1 = (float)rb_width;
	float x2 = x1+tooltray_control_button_width;
	float x3 = x2+tooltray_control_button_width;
	if(switch_lists_side)
	{
		x3 = 0;
		x2 = x3+tooltray_control_button_width;
		x1 = x2+tooltray_control_button_width;
		x0 = x1+tooltray_control_button_width;
		rb_width -= 3;
	}
	for(int i = 0;i<n;i++)
	{
		tooltray_items[i]->restore_button.SetWindowPos(0, (int)x0, i*tooltray_item_height, rb_width, tooltray_item_height, 0);
		tooltray_items[i]->down_button.SetWindowPos(0, (int)x1, i*tooltray_item_height, tooltray_control_button_width, tooltray_item_height, 0);
		tooltray_items[i]->up_button.SetWindowPos(0, (int)x2, i*tooltray_item_height, tooltray_control_button_width, tooltray_item_height, 0);
		tooltray_items[i]->remove_button.SetWindowPos(0, (int)x3, i*tooltray_item_height, tooltray_control_button_width, tooltray_item_height, 0);
	}
	tooltray_height = (tooltray_item_height*n)+4;
	return true;
}


bool Resize_ToolTray_Dlg()
{
	Resize_ToolTray_Items();
	if(toolTrayDlg->GetSafeHwnd()==0)return false;
	if(hide_lists)
	{
		toolTrayDlg->ShowWindow(SW_HIDE);
	}
	else
	{
		toolTrayDlg->ShowWindow(SW_SHOW);
	}
	CRect rect;
	mainwindow->GetClientRect(&rect);
	int xpos = rect.Width()-sidebar_width;
	int height = tooltray_height;
	int ypos = sidebar_top_border;
	int interior_width = sidebar_width-1;
	toolTrayDlg->SetParent(mainwindow);
	if(switch_lists_side)
	{
		toolTrayDlg->SetWindowPos(0, 0, ypos, sidebar_width, height, 0);
	}
	else
	{
		toolTrayDlg->SetWindowPos(0, xpos, ypos, sidebar_width, height, 0);
	}
	return true;
}

BOOL ToolTrayDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	int id = LOWORD(wParam);
	int n = tooltray_items.size();
	for(int i = 0;i<n;i++)
	{
		if(id==tooltray_items[i]->restore_button_id)
		{
			if(tooltray_items[i]->restore_button.render_pushed)
			{
				Close_SkinFrame(tooltray_items[i]->restore_control_id);
			}
			else
			{
				if(tooltray_items[i]->restore_control_id==-1)
				{
					//no menu command, it must be a plugin, try to open the plugin by name
					Open_Plugin(tooltray_items[i]->text);
				}
				else
				{
					//it's a menu command, just call it from the mainframe
					mainwindow->SendMessage(WM_COMMAND, tooltray_items[i]->restore_control_id, tooltray_items[i]->restore_control_id);
				}
			}
			return true;
		}
		else if(id==tooltray_items[i]->up_button_id)
		{
			if(i!=0)
			{
				TOOLTRAY_ITEM *tt = tooltray_items[i-1];
				tooltray_items[i-1] = tooltray_items[i];
				tooltray_items[i] = tt;
				Resize_ToolTray_Items();
			}
			return true;
		}
		else if(id==tooltray_items[i]->down_button_id)
		{
			if(i!=n-1)
			{
				TOOLTRAY_ITEM *tt = tooltray_items[i+1];
				tooltray_items[i+1] = tooltray_items[i];
				tooltray_items[i] = tt;
				Resize_ToolTray_Items();
			}
			return true;
		}
		else if(id==tooltray_items[i]->remove_button_id)
		{
			Remove_ToolTray_Item(tooltray_items[i]->restore_control_id);
			return true;
		}
	}
	return true;
}

HBRUSH ToolTrayDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return skin_background_brush_color;
}

int Get_ToolTray_Item_ID(char *text)
{
	if(!strcmp(text, "2D View Options"))return IDC_2D_VIEW_OPTIONS;
	if(!strcmp(text, "3D View Options"))return IDC_3D_VIEW_OPTIONS;
	if(!strcmp(text, "Anchor Points"))return IDC_OPEN_ANCHOR_POINTS_TOOL;
	if(!strcmp(text, "Auto Segment Image"))return IDC_AUTO_SEGMENT_IMAGE;
	if(!strcmp(text, "Feature Points"))return IDC_FEATURE_POINTS_TOOL;
	if(!strcmp(text, "Gap Fill"))return IDC_OPEN_GAPFILL_EDITOR;
	if(!strcmp(text, "Geometry Tool"))return IDC_APPLY_GEOMETRY;
	if(!strcmp(text, "Perspective Projection"))return IDC_GET_PERSPECTIVE_PROJECTION;
	if(!strcmp(text, "Stereo Settings"))return IDC_STEREO_SETTINGS;
	if(!strcmp(text, "Scale"))return IDC_OPEN_SCALE_SLIDER;
	if(!strcmp(text, "Relief Map"))return IDC_RELIEF_MAP;
	if(!strcmp(text, "Orientation"))return IDC_PLANAR_PROJECTION;
	if(!strcmp(text, "Import Options"))return IDC_IMPORT;
	if(!strcmp(text, "Export Options"))return IDC_EXPORT;
	if(!strcmp(text, "Depth"))return IDC_OPEN_DEPTH_SLIDER;
	if(!strcmp(text, "Contour Extrusion"))return IDC_CONTOUR_EXTRUSION;
	if(!strcmp(text, "Alignment Options"))return IDC_AUTO_ALIGNMENT;
	if(!strcmp(text, "Shift Edges"))return IDC_SHIFT_EDGES;
	if(!strcmp(text, "Track Outline"))return IDC_TRACK_OUTLINE;
	if(!strcmp(text, "Virtual Set"))return IDC_OPEN_VIRTUAL_SET_TOOL;
	if(!strcmp(text, "Virtual Camera"))return IDC_OPEN_VIRTUAL_CAMERA_TOOL;
	if(!strcmp(text, "Paint Tool"))return IDC_PAINT_TOOL;
	if(!strcmp(text, "Plugins"))return IDC_PLUGINS;
	if(!strcmp(text, "Touch Ups"))return IDC_TOUCHUPS_TOOL;
	if(!strcmp(text, "Halo Tool"))return IDC_HALO_TOOL;
	return -1;
}

bool Add_ToolTray_Item(char *text)
{
	int restore_control_id = Get_ToolTray_Item_ID(text);
	//if it's -1 assume it's a plugin
	Add_ToolTray_Item(text, restore_control_id);
	return true;
}

bool Is_Whitespace(char *text)
{
	int n = strlen(text);
	for(int i = 0;i<n;i++)
	{
		if(text[i]!=' '&&text[i]!='\n')
		{
			return false;
		}
	}
	return true;
}

bool Load_ToolTray(char *file)
{
	char line[512];
	char *c;
	FILE *f = fopen(file, "rt");
	if(!f)
	{
		return false;
	}
	while(!feof(f))
	{
		line[0];
		if(fgets(line, 512, f))
		{
			c = strrchr(line, '\n');
			if(c)
			{
				*c = 0;
				if(!Is_Whitespace(line))
				{
					Add_ToolTray_Item(line);
				}
			}
		}
	}
	fclose(f);
	return true;
}

bool Save_ToolTray(char *file)
{
	FILE *f = fopen(file, "wt");
	if(!f)
	{
		return false;
	}
	int n = tooltray_items.size();
	for(int i = 0;i<n;i++)
	{
		fprintf(f, "%s\n", tooltray_items[i]->text);
	}
	fclose(f);
	return false;
}

bool Align_To_Tooltray(SkinFrame *skinframe)
{
	int index = Find_ToolTray_Item(skinframe->tooltray_control_id);
	if(index==-1)
	{
		return false;
	}
	CRect rect;
	tooltray_items[index]->restore_button.GetWindowRect(&rect);
	int x = rect.left;
	int y = rect.top;
	if(switch_lists_side)
	{
		x = rect.right;
	}
	else
	{
		skinframe->GetWindowRect(&rect);
		x -= rect.Width();
	}
	skinframe->SetWindowPos(0, x, y, 0, 0, SWP_NOSIZE);
	return true;
}

bool Align_Plugin_Dialog_To_Tooltray(CWnd *p)
{
	char text[512];
	p->GetWindowText(text, 512);
	int index = Find_ToolTray_Item(text);
	if(index==-1)
	{
		return false;
	}
	CRect rect;
	tooltray_items[index]->restore_button.GetWindowRect(&rect);
	int x = rect.left;
	int y = rect.top;
	if(switch_lists_side)
	{
		x = rect.right;
	}
	else
	{
		p->GetWindowRect(&rect);
		x -= rect.Width();
	}
	p->SetWindowPos(0, x, y, 0, 0, SWP_NOSIZE);
	return true;
}