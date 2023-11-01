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
#include "../Skin.h"
#include "Console.h"
#include "Layers.h"
#include <GL\gl.h>
#include <GL\glu.h>
#include <vector>
#include "../GeometryTool.h"
#include "../HaloTool.h"


//THIS SUCKS TOO!
bool kill_menutip_manager = false;

//THIS SUCKS!!!
bool running_popup_menu = false;

int layer_popup_menu_start = 25000;

class PU_MENU_ITEM
{
public:
	PU_MENU_ITEM()
	{
	}
	~PU_MENU_ITEM()
	{
	}
	char text[64];
	bool enabled;
	bool checked;
	bool grayed;
};

vector<PU_MENU_ITEM> layer_menu_items;

class CSkinPopupMenu : public CMenu
{
public:
	CSkinPopupMenu()
	{
	}
	~CSkinPopupMenu()
	{
	}
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS)
	{
		if(!lpDIS)
		{
			//might be NULL if user clicks mainframe close button while popup menu is up
			//that was a tough one to nail down
			return;
		}
		CDC* pDC = CDC::FromHandle(lpDIS->hDC);
		if(!pDC)
		{
			//might be NULL if user clicks mainframe close button while popup menu is up
			//that was a tough one to nail down
			return;
		}
		if ((lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
		{
			pDC->FillRect(&lpDIS->rcItem, &selected_menu_brush);
			pDC->SetTextColor(RGB(255,255,255));
			pDC->SetBkColor(selected_menu_bg_color);
		}
		else
		{
			pDC->FillRect(&lpDIS->rcItem, &unselected_menu_brush);
			pDC->SetTextColor(skin_text_color);
			pDC->SetBkColor(skin_text_bg_color);
		}
		char *text = layer_menu_items[lpDIS->itemID-layer_popup_menu_start].text;
		int len = strlen(text);
		pDC->SelectObject(skin_font);
		RECT rect = lpDIS->rcItem;
		if(lpDIS->itemState & ODS_CHECKED)
		{
			pDC->DrawText("  *", 3, &rect, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
		}
		int checkbox_border_size = 16;
		rect.left += checkbox_border_size;
		pDC->DrawText(text, len, &rect, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	}
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
	{
		char *text = layer_menu_items[lpMIS->itemID-layer_popup_menu_start].text;
		int len = strlen(text);
		float l = 5*(len+2);
		lpMIS->itemWidth = 150;
		lpMIS->itemHeight = 16;
	}
};

CSkinPopupMenu *popup_menu = 0;

void DrawPopupMenuItem(LPDRAWITEMSTRUCT lpDIS)
{
	popup_menu->DrawItem(lpDIS);
}

void MeasurePopupMenuItem(LPMEASUREITEMSTRUCT lpMIS)
{
	popup_menu->MeasureItem(lpMIS);
}


using namespace std;

bool Is_First_Or_Last_Frame();

bool Update_Selection_Info();

void Update_GUI_Selection_Info();

extern bool redraw_edit_window;

extern float _fov;

int popup_xpos = 0;
int popup_ypos = 0;

extern float camera_pos[3];
extern float camera_rot[3];

int Get_3D_Clicked_Layer(int x, int y);

bool Open_Planar_Projection_Dlg();
bool Open_Contour_Extrusion_Dlg();
bool Open_Relief_Map_Dlg();

bool Open_Depth_Slider_Dlg();
bool Open_Scale_Slider_Dlg();

void Render_Layers_For_Selection();

bool run_layer_popup_menu = false;

int last_clicked_layer = 0;

HWND puHWND = 0;

bool waiting_for_popup_menu_result = false;

GLuint gl_select_buffer[512];

bool Reset_Layer_Popup_Menu()
{
	layer_menu_items.clear();
	return true;
}

bool Add_Layer_Menu_Item(char *text, bool enabled, bool checked, bool grayed)
{
	PU_MENU_ITEM i;
	strcpy(i.text, text);
	i.enabled = enabled;
	i.checked = checked;
	i.grayed = grayed;
	layer_menu_items.push_back(i);
	return true;
}

bool Run_Popup_Menu(int px, int py, unsigned int *res)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CSkinPopupMenu menu;
	menu.CreatePopupMenu();
	unsigned int n = layer_menu_items.size();
	int flags = MF_STRING;

	RECT rect;
	GetWindowRect(puHWND, &rect);
	px+=rect.left;
	py+=rect.top;

	int menu_offset = 30;
	px-=menu_offset;
	py-=menu_offset;

	for(unsigned int i = 0;i<n;i++)
	{
		if(!layer_menu_items[i].enabled)
		{
			flags = MF_STRING | MF_DISABLED | MF_OWNERDRAW;
		}
		else
		{
			flags = MF_STRING | MF_ENABLED | MF_OWNERDRAW;
		}
		if(layer_menu_items[i].grayed)
		{
			flags = flags | MF_GRAYED | MF_OWNERDRAW;
		}
		menu.AppendMenu(flags, layer_popup_menu_start+i, layer_menu_items[i].text);
		if(layer_menu_items[i].checked)
		{
			menu.CheckMenuItem(i, MF_BYPOSITION | MF_CHECKED);
		}
	}

	MENUINFO MenuInfo = {0};
	MenuInfo.cbSize = sizeof(MenuInfo);
	MenuInfo.hbrBack = skin_background_brush_color;
	MenuInfo.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
	MenuInfo.dwStyle = MNS_AUTODISMISS;

	SetMenuInfo(menu.GetSafeHmenu(), &MenuInfo);
	
	popup_menu = &menu;
	running_popup_menu = true;

	TrackPopupMenu(menu.GetSafeHmenu(), TPM_NONOTIFY | TPM_LEFTALIGN |TPM_TOPALIGN |TPM_RIGHTBUTTON, px, py, 0, mainwindow->GetSafeHwnd(), 0);
	
	MSG msg;
	
	bool waiting_for_popup_menu_result = true;

	//spin and update windows messages until popup menu has been
	//processed
	int result = -1;
	bool dispatch = true;
	while(waiting_for_popup_menu_result)
	{
		while(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			dispatch = true;
			if(msg.message==WM_COMMAND)
			{
				result = msg.wParam-layer_popup_menu_start;
				if(result!=0)
				{
					waiting_for_popup_menu_result = false;
				}
			}
			else if(msg.message==WM_MEASUREITEM)
			{
				SkinMsgBox(0, "This never happens.", 0, MB_OK);
				menu.MeasureItem((LPMEASUREITEMSTRUCT)msg.lParam);
				dispatch = false;
			}
			else if(msg.message==WM_DRAWITEM)
			{
				dispatch = false;
			}
			else
			{
				waiting_for_popup_menu_result = false;
			}
			if(dispatch)
			{
				TranslateMessage(&msg);
				if(msg.message==WM_MEASUREITEM||msg.message==WM_DRAWITEM)
				{
					SkinMsgBox(0, "WTF!!!", 0, MB_OK);
				}
				DispatchMessage(&msg);
			}
		}
	}
	SendMessage(puHWND, WM_RBUTTONUP, 0, 0);
	popup_menu = 0;
	running_popup_menu = false;
	if(result==-1)
	{
		return false;
	}
	*res = result;
	return true;
}

void Layer_Popup_Menu(HWND hWnd, int xpos, int ypos, int clicked_layer)
{
	last_clicked_layer = clicked_layer;
	if(last_clicked_layer!=-1)
	{
		run_layer_popup_menu = true;
		redraw_frame = true;
		redraw_edit_window = true;
	}
	popup_xpos = xpos;
	popup_ypos = ypos;
	puHWND = hWnd;
}

void Rename_Layer(int index)
{
	char text[256];
	Layer_Name(index, text);
	if(Get_User_Text(text, "New Layer Name"))
	{
		int id;
		if(Find_Layer(text, &id))
		{
			if(Find_Layer_Index(id)!=index)
			{
				SkinMsgBox(0, "This name is already in use", text, MB_OK);
				return;
			}
		}
		Set_Layer_Name(index, text);
		Update_Layer_List();
		List_Selected_Layers();
	}
	selection_changed = true;
}


bool Toggle_Keyframe_All_Selected()
{
	int nl = Num_Layers();
	for(int i = 0;i<nl;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Keyframe_Layer(i, !Layer_Is_Keyframed(i));
		}
	}
	return true;
}

bool Toggle_Freeze_All_Selected()
{
	int nl = Num_Layers();
	for(int i = 0;i<nl;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Freeze_Layer(i, !Layer_Is_Frozen(i));
		}
	}
	return true;
}

bool Toggle_Visible_All_Selected()
{
	int nl = Num_Layers();
	for(int i = 0;i<nl;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Hide_Layer(i, !Layer_Is_Hidden(i));
		}
	}
	return true;
}

bool Toggle_Outline_All_Selected()
{
	int nl = Num_Layers();
	for(int i = 0;i<nl;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Outline_Layer(i, !Layer_Is_Outlined(i));
		}
	}
	return true;
}

bool Toggle_Speckle_All_Selected()
{
	int nl = Num_Layers();
	for(int i = 0;i<nl;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Speckle_Layer(i, !Layer_Is_Speckled(i));
		}
	}
	return true;
}

void Run_Layer_Popup_Menu()
{
	run_layer_popup_menu = false;
	int l = last_clicked_layer;
	if(l==-1)
	{
		return;
	}
	char name[64];
	char text[128];
	int index = Get_Layer_Index(l);
	Layer_Name(index, name);
	sprintf(text, " --- \"%s\" --- ", name);
	Add_Layer_Menu_Item(text, false, false, false);
	if(index==0)
	{
		Add_Layer_Menu_Item("Rename", false, false, true);
	}
	else
	{
		Add_Layer_Menu_Item("Rename", Num_Selected_Layers()<2, false, Num_Selected_Layers()>1);
	}
	
	int ns = Num_Selected_Layers();

	bool freeze_mismatch = false;
	bool visible_mismatch = false;
	bool outline_mismatch = false;
	bool speckle_mismatch = false;
	bool keyframed_mismatch = false;

	bool frozen = false;
	bool visible = false;
	bool outline = false;
	bool speckle = false;
	bool keyframed = false;

	int cnt = 0;

	if(ns>1)
	{
		int nl = Num_Layers();
		for(int i = 0;i<nl;i++)
		{
			if(Layer_Is_Selected(i))
			{
				cnt++;
				if(cnt==1)
				{
					frozen = Layer_Is_Frozen(i);
					visible = !Layer_Is_Hidden(i);
					outline = Layer_Is_Outlined(i);
					speckle = Layer_Is_Speckled(i);
					keyframed = Layer_Is_Keyframed(i);
				}
				else
				{
					if(!freeze_mismatch)
					{
						if(frozen!=Layer_Is_Frozen(i))freeze_mismatch = true;
					}
					if(!visible_mismatch)
					{
						if(visible==Layer_Is_Hidden(i))visible_mismatch = true;
					}
					if(!outline_mismatch)
					{
						if(outline!=Layer_Is_Outlined(i))outline_mismatch = true;
					}
					if(!speckle_mismatch)
					{
						if(speckle!=Layer_Is_Speckled(i))speckle_mismatch = true;
					}
					if(!keyframed_mismatch)
					{
						if(keyframed!=Layer_Is_Keyframed(i))keyframed_mismatch = true;
					}
				}
			}
		}
	}

	bool check_keyframe = false;
	bool check_frozen = false;
	bool check_hidden = false;
	bool check_outlined = false;
	bool check_speckled = false;
	if(ns<=1)
	{
		check_keyframe = Layer_Is_Keyframed(index);
		check_frozen = Layer_Is_Frozen(index);
		check_hidden = !Layer_Is_Hidden(index);
		check_outlined = Layer_Is_Outlined(index);
		check_speckled = Layer_Is_Speckled(index);
	}
	else
	{
		check_keyframe = Layer_Is_Keyframed(index)||keyframed_mismatch;
		check_frozen = Layer_Is_Frozen(index)||freeze_mismatch;
		check_hidden = !Layer_Is_Hidden(index)||visible_mismatch;
		check_outlined = Layer_Is_Outlined(index)||outline_mismatch;
		check_speckled = Layer_Is_Speckled(index)||speckle_mismatch;
	}

	if(Is_First_Or_Last_Frame())
	{
		keyframed_mismatch = true;
	}

	Add_Layer_Menu_Item("Keyframe", true, check_keyframe, keyframed_mismatch);
	Add_Layer_Menu_Item("Freeze", true, check_frozen, freeze_mismatch);
	Add_Layer_Menu_Item("Visible", true, check_hidden, visible_mismatch);
	Add_Layer_Menu_Item("Outline", true, check_outlined, outline_mismatch);
	Add_Layer_Menu_Item("Speckle", true, check_speckled, speckle_mismatch);
	Add_Layer_Menu_Item("Depth", true, false, false);
	Add_Layer_Menu_Item("Scale", true, false, false);
	Add_Layer_Menu_Item("Orientation", true, false, false);
	Add_Layer_Menu_Item("Contour Extrusion", true, false, false);
	Add_Layer_Menu_Item("Relief Map", true, false, false);
	Add_Layer_Menu_Item("Track Outline", true, false, false);
	Add_Layer_Menu_Item("Shift Edges", true, false, false);
	Add_Layer_Menu_Item("Apply Geometry", true, false, false);
	Add_Layer_Menu_Item("Extract Halo", true, false, false);
	unsigned int res = 0;

	kill_menutip_manager = true;//HACK!

	Run_Popup_Menu(popup_xpos, popup_ypos, &res);

	kill_menutip_manager = false;//HACK!!

	if(res==1)//rename
	{
		Rename_Layer(index);
	}
	if(res==2)//keyframe
	{
		if(!Layer_Is_Selected(index))
		{
			Keyframe_Layer(index, !Layer_Is_Keyframed(index));
		}
		else
		{
			Toggle_Keyframe_All_Selected();
		}
		Update_Selection_Info();
	}
	if(res==3)//freeze
	{
		if(!Layer_Is_Selected(index))
		{
			Freeze_Layer(index, !Layer_Is_Frozen(index));
		}
		else
		{
			Toggle_Freeze_All_Selected();
		}
		Update_Selection_Info();
	}
	if(res==4)//visible
	{
		if(!Layer_Is_Selected(index))
		{
			Hide_Layer(index, !Layer_Is_Hidden(index));
		}
		else
		{
			Toggle_Visible_All_Selected();
		}
		Update_Selection_Info();
	}
	if(res==5)//outline
	{
		if(!Layer_Is_Selected(index))
		{
			Outline_Layer(index, !Layer_Is_Outlined(index));
		}
		else
		{
			Toggle_Outline_All_Selected();
		}
		Update_Selection_Info();
	}
	if(res==6)//speckle
	{
		if(!Layer_Is_Selected(index))
		{
			Speckle_Layer(index, !Layer_Is_Speckled(index));
		}
		else
		{
			Toggle_Speckle_All_Selected();
		}
		Update_Selection_Info();
	}
	if(res==7)//adjust depth
	{
		Select_Layer(index, true);
		List_Selected_Layers();
		Open_Depth_Slider_Dlg();
	}
	if(res==8)//adjust scale
	{
		Select_Layer(index, true);
		List_Selected_Layers();
		Open_Scale_Slider_Dlg();
	}
	if(res==9)//planar projection
	{
		Select_Layer(index, true);
		List_Selected_Layers();
		Open_Planar_Projection_Dlg();
	}
	if(res==10)//contour extrusion
	{
		Select_Layer(index, true);
		List_Selected_Layers();
		Open_Contour_Extrusion_Dlg();
	}
	if(res==11)//relief map
	{
		Select_Layer(index, true);
		List_Selected_Layers();
		Open_Relief_Map_Dlg();
	}
	if(res==12)//track outline
	{
		Select_Layer(index, true);
		List_Selected_Layers();
		Open_Track_Outlines_Tool();
	}
	if(res==13)//shift edges
	{
		Select_Layer(index, true);
		Open_Shift_Edges_Dlg();
	}
	if(res==14)//apply geometry
	{
		Select_Layer(index, true);
		Open_Geometry_Tool();
	}
	if(res==15)//extract halo
	{
		//single selection only for halo tool
		Select_All_Layers(false);
		Select_Layer(index, true);
		Open_Halo_Tool();
	}

	Reset_Layer_Popup_Menu();
	redraw_edit_window = true;
	redraw_frame = true;
}


void Setup_GL_Selection_Render(int px, int py)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, gl_select_buffer);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(-5);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix((GLdouble) px, (GLdouble) (viewport[3]-py), 1.0f, 1.0f, viewport);
	gluPerspective(_fov, (GLfloat) (viewport[2]-viewport[0])/(GLfloat) (viewport[3]-viewport[1]), 0.001f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(camera_rot[0], 1, 0, 0);
	glRotatef(camera_rot[1], 0, 1, 0);
	glTranslatef(-camera_pos[0], -camera_pos[1], -camera_pos[2]);
}

bool Get_Selection_Hit(unsigned int *res)
{
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	GLint hits;
	hits = glRenderMode(GL_RENDER);
	if (hits > 0)
	{
		int	choose = gl_select_buffer[3];
		int	depth = gl_select_buffer[1];
		for (int loop = 1; loop < hits; loop++)
		{
			if (gl_select_buffer[loop*4+1] < GLuint(depth))
			{
				choose = gl_select_buffer[loop*4+3];
				depth = gl_select_buffer[loop*4+1];
			}       
		}
		*res = choose;
		return true;
	}
	return false;
}

int Get_3D_Clicked_Layer(int x, int y)
{
	Setup_GL_Selection_Render(x, y);
	Render_Layers_For_Selection();
	unsigned int res = 0;
	if(!Get_Selection_Hit(&res))
	{
		return -1;
	}
	return res;
}


