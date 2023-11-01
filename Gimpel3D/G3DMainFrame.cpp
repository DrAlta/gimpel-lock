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
// G3DMainFrame.cpp : implementation file
//


#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "G3DMainFrame.h"
#include "ListSelectDlg.h"
#include "GapFillTool.h"
#include "GeometryTool.h"
#include "G3DCoreFiles/AutoSegment.h"
#include "PerspectiveTool.h"
#include "Shapes.h"
#include "AnchorPoints.h"
#include "VirtualSet.h"
#include "FeaturePoints.h"
#include "G3DCoreFiles/Layers.h"
#include "PaintTool.h"
#include "Plugins.h"
#include "TouchUpTool.h"
#include "HaloTool.h"


bool Export_Background_Mesh(char *file);
bool Import_Background_Mesh(char *file);

bool Get_Left_View_Gapmask(unsigned char *rgb);
bool Get_Right_View_Gapmask(unsigned char *rgb);
bool Save_Watermarked_Image(unsigned char *rgb, int w, int h, char *file);


bool Report_Loading_Single_Frame_Session();
bool Report_Saving_Single_Frame_Session();

bool Run_Color_Dlg();
bool Open_Preview_Tool();

bool Open_Virtual_Camera_Tool();

bool Update_Toolbar_Grid_Button();

bool Update_Toolbar_Camera_Button();
bool ReCalc_Focal_Plane_Preview();

extern bool render_camera;

bool Open_Track_Outlines_Tool();

bool fullscreen_app = false;

bool Update_Toolbar_View_Buttons();
bool Update_Toolbar_Anaglyph_Button();

int min_app_width = 500;
int min_app_height = 500;

bool Init_Toolbar_Dlg();
bool Free_Toolbar_Dlg();

bool Open_Help_Dlg();

extern bool project_loaded;


unsigned char background_clr[3] = {128,128,128};

int toolbar_height = 30;

int sidebar_top_border = toolbar_height;
int sidebar_bottom_border = 70;
int sidebar_width = 200;
int sidebar_restore_width = 18;

int window_border = 6;

int window_left_border = 6;
int window_right_border = sidebar_width;
int window_top_border = toolbar_height;
int window_bottom_border = 70;

bool Init_3D_Frame_Dialog();
bool Close_3D_Frame_Dialog();

extern bool running_popup_menu;
void DrawPopupMenuItem(LPDRAWITEMSTRUCT lpDIS);
void MeasurePopupMenuItem(LPMEASUREITEMSTRUCT lpMIS);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// G3DMainFrame

IMPLEMENT_DYNCREATE(G3DMainFrame, CFrameWnd)


G3DMainFrame::G3DMainFrame()
{
}

G3DMainFrame::~G3DMainFrame()
{
}


BEGIN_MESSAGE_MAP(G3DMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(G3DMainFrame)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_COMMAND(IDC_LOAD_PROJECT, OnLoadProject)
	ON_COMMAND(IDC_NEW_PROJECT, OnNewProject)
	ON_COMMAND(IDC_SAVE_PROJECT, OnSaveProject)
	ON_COMMAND(IDC_EXIT, OnExit)
	ON_COMMAND(IDC_IMPORT, OnImport)
	ON_COMMAND(IDC_EXPORT, OnExport)
	ON_COMMAND(IDC_SAVE_SESSION, OnSaveSession)
	ON_COMMAND(IDC_REPLACE_IMAGE, OnReplaceImage)
	ON_COMMAND(IDC_LOAD_SESSION, OnLoadSession)
	ON_COMMAND(IDC_LOAD_FRAME, OnLoadFrame)
	ON_COMMAND(IDC_IMPORT_LAYERS, OnImportLayers)
	ON_COMMAND(IDC_IMPORT_DEPTH_MASK, OnImportDepthMask)
	ON_COMMAND(IDC_EXPORT_LAYER_MASK, OnExportLayerMask)
	ON_COMMAND(IDC_EXPORT_DEPTH_MASK, OnExportDepthMask)
	ON_COMMAND(IDC_IMPORT_STEREO_SETTINGS, OnImportStereoSettings)
	ON_COMMAND(IDC_FLIP_DEPTH_MASK_DEPTH, OnFlipDepthMaskDepth)
	ON_COMMAND(IDC_FLIP_DEPTH_MASK_HORIZONTAL, OnFlipDepthMaskHorizontal)
	ON_COMMAND(IDC_FLIP_DEPTH_MASK_VERTICAL, OnFlipDepthMaskVertical)
	ON_COMMAND(IDC_FLIP_IMAGE_HORIZONTAL, OnFlipImageHorizontal)
	ON_COMMAND(IDC_FLIP_LAYERS_HORIZONTAL, OnFlipLayersHorizontal)
	ON_COMMAND(IDC_FLIP_LAYERS_VERTICAL, OnFlipLayersVertical)
	ON_COMMAND(IDC_FLIP_RED_BLUE, OnFlipRedBlue)
	ON_COMMAND(IDC_FLIP_IMAGE_VERTICAL, OnFlipImageVertical)
	ON_COMMAND(IDC_DUAL_MONITOR, OnDualMonitor)
	ON_COMMAND(IDC_SINGLE_MONITOR, OnSingleMonitor)
	ON_WM_SIZE()
	ON_COMMAND(IDC_SWAP_MONITORS, OnSwapMonitors)
	ON_BN_CLICKED(IDC_FULL_SCREEN_2D, OnFullScreen2d)
	ON_BN_CLICKED(IDC_FULL_SCREEN_3D, OnFullScreen3d)
	ON_BN_CLICKED(IDC_WIDE_SCREEN_3D, OnWideScreen3d)
	ON_BN_CLICKED(IDC_SPLIT_SCREEN, OnSplitScreen)
	ON_COMMAND(IDC_ANAGLYPH, OnAnaglyph)
	ON_COMMAND(IDC_STEREO_SETTINGS, OnStereoSettings)
	ON_COMMAND(IDC_3D_VIEW_OPTIONS, On3dViewOptions)
	ON_COMMAND(IDC_SELECT_ALL, OnSelectAll)
	ON_COMMAND(IDC_UNSELECT_LAYERS, OnUnselectLayers)
	ON_COMMAND(IDC_RENAME_SELECTION, OnRenameSelection)
	ON_COMMAND(IDC_MERGE_LAYERS, OnMergeLayers)
	ON_COMMAND(IDC_DELETE_LAYERS, OnDeleteLayers)
	ON_COMMAND(IDC_EVAPORATE_LAYERS, OnEvaporateLayers)
	ON_COMMAND(IDC_SPLIT_LAYER, OnSplitLayer)
	ON_COMMAND(IDC_OPEN_DEPTH_SLIDER, OnOpenDepthSlider)
	ON_COMMAND(IDC_OPEN_SCALE_SLIDER, OnOpenScaleSlider)
	ON_COMMAND(IDC_PLANAR_PROJECTION, OnPlanarProjection)
	ON_COMMAND(IDC_CONTOUR_EXTRUSION, OnContourExtrusion)
	ON_COMMAND(IDC_RELIEF_MAP, OnReliefMap)
	ON_COMMAND(IDC_AUTO_ALIGNMENT, OnAutoAlignment)
	ON_COMMAND(IDC_2D_VIEW_OPTIONS, On2dViewOptions)
	ON_COMMAND(IDC_LAYER_ALL_FRAMES, OnLayerAllFrames)
	ON_COMMAND(IDC_LAYER_SINGLE_FRAME, OnLayerSingleFrame)
	ON_COMMAND(IDC_KEY_COMMANDS, OnKeyCommands)
	ON_COMMAND(IDC_SHIFT_EDGES, OnShiftEdges)
	ON_COMMAND(IDC_OPEN_POST_EDITOR, OnOpenPostEditor)
	ON_COMMAND(IDC_OPEN_GAPFILL_EDITOR, OnOpenGapfillEditor)
	ON_COMMAND(IDC_APPLY_GEOMETRY, OnApplyGeometry)
	ON_COMMAND(IDC_AUTO_SEGMENT_IMAGE, OnAutoSegmentImage)
	ON_COMMAND(IDC_SHOW_HELP, OnShowHelp)
	ON_COMMAND(IDC_GET_PERSPECTIVE_PROJECTION, OnGetPerspectiveProjection)
	ON_COMMAND(IDC_OPEN_SHAPES_TOOL, OnOpenShapesTool)
	ON_COMMAND(IDC_OPEN_ANCHOR_POINTS_TOOL, OnOpenAnchorPointsTool)
	ON_COMMAND(IDC_OPEN_VIRTUAL_SET_TOOL, OnOpenVirtualSetTool)
	ON_COMMAND(IDC_FEATURE_POINTS_TOOL, OnFeaturePointsTool)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_COMMAND(IDC_FULL_SCREEN_APP, OnFullScreenApp)
	ON_COMMAND(IDC_TRACK_OUTLINE, OnTrackOutline)
	ON_COMMAND(IDC_COLOR_SCHEME, OnColorScheme)
	ON_COMMAND(IDC_PREVIEW_TOOL, OnPreviewTool)
	ON_COMMAND(IDC_COPY_EDITS_ALL_FRAMES, OnCopyEditsAllFrames)
	ON_COMMAND(IDC_COPY_EDITS_SINGLE_FRAME, OnCopyEditsSingleFrame)
	ON_COMMAND(IDC_OPEN_VIRTUAL_CAMERA_TOOL, OnOpenVirtualCameraTool)
	ON_COMMAND(IDC_GRID_BUTTON, OnGridButton)
	ON_COMMAND(IDC_CAMERA_BUTTON, OnCameraButton)
	ON_COMMAND(IDC_PAINT_TOOL, OnPaintTool)
	ON_COMMAND(IDC_PLUGINS, OnPlugins)
	ON_COMMAND(IDC_TOUCHUPS_TOOL, OnTouchupsTool)
	ON_COMMAND(IDC_HALO_TOOL, OnHaloTool)
	ON_COMMAND(IDC_IMPORT_LAYER_EDITS, OnImportLayerEdits)
	ON_COMMAND(IDC_HI_RES_PERFORMANCE, OnHiResPerformance)
	ON_COMMAND(IDC_EXPORT_BACKGROUND_MESH, OnExportBackgroundMesh)
	ON_COMMAND(IDC_IMPORT_BACKGROUND_MESH, OnImportBackgroundMesh)
	ON_COMMAND(IDC_EXPORT_LEFT_GAP_MASK, OnExportLeftGapMask)
	ON_COMMAND(IDC_EXPORT_RIGHT_GAP_MASK, OnExportRightGapMask)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// G3DMainFrame message handlers


//keep track of text strings to manually draw menu items
vector<char*> od_menu_items;
int num_od_menu_items = 0;


class CSkinMenu : public CMenu
{
public:
	CSkinMenu()
	{
	}
	~CSkinMenu()
	{
	}
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS)
	{
		if(running_popup_menu)//THIS SUCKS!
		{
			DrawPopupMenuItem(lpDIS);
			return;
		}
		CDC* pDC = CDC::FromHandle(lpDIS->hDC);
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
		char *text = od_menu_items[lpDIS->itemData];
		int len = strlen(text);
		pDC->SelectObject(skin_font);
		RECT rect = lpDIS->rcItem;
		if(lpDIS->itemData<14)//popup item
		{
			pDC->DrawText(text, len, &rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		}
		else
		{
			if(lpDIS->itemState & ODS_CHECKED)
			{
				pDC->DrawText("  *", 3, &rect, DT_SINGLELINE|DT_VCENTER|DT_LEFT);//CENTER);
			}
			int checkbox_border_size = 16;
			rect.left += checkbox_border_size;
			pDC->DrawText(text, len, &rect, DT_SINGLELINE|DT_VCENTER|DT_LEFT);//CENTER);
		}
	}
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
	{
		if(running_popup_menu)//THIS SUCKS TOO!!!!
		{
			MeasurePopupMenuItem(lpMIS);
			return;
		}
		if(lpMIS->itemData<14)//popup item
		{
			char *text = od_menu_items[lpMIS->itemData];
			int len = strlen(text);
			float l = 5*(len);
			lpMIS->itemWidth = (int)l;
			lpMIS->itemHeight = 16;
		}
		else
		{
			char *text = od_menu_items[lpMIS->itemData];
			int len = strlen(text);
			float l = 5.5f*(len+2);
			lpMIS->itemWidth = (int)l;
			lpMIS->itemHeight = 16;
		}
	}
};



G3DMainFrame *mainframe = 0;

CWnd *mainwindow = 0;
CSkinMenu *mainmenu = 0;


void Create_Ownerdraw_Menu_Item(char *text, unsigned int id)
{
	unsigned int nid = 0;
	mainmenu->ModifyMenu(id, MF_OWNERDRAW, id, (char*)num_od_menu_items);
	char *c = new char[strlen(text)+2];
	strcpy(c, text);
	od_menu_items.push_back(c);
	num_od_menu_items++;
}

void Create_Ownerdraw_Popup_Menu_Item(char *text, unsigned int id)
{
	unsigned int nid = 0;
	mainmenu->ModifyMenu(id, MF_BYPOSITION | MF_OWNERDRAW, -1, (char*)num_od_menu_items);
	char *c = new char[strlen(text)+2];
	strcpy(c, text);
	od_menu_items.push_back(c);
	num_od_menu_items++;
}

void Free_Ownerdraw_Menu_Items()
{
	int n = od_menu_items.size();
	for(int i = 0;i<n;i++)
	{
		delete[] od_menu_items[i];
	}
	od_menu_items.clear();
}

CWnd* Create_App_Skin_Frame(CWnd *w);

bool Update_Menu_Color()
{
	MENUINFO MenuInfo = {0};
	MenuInfo.cbSize = sizeof(MenuInfo);
	MenuInfo.hbrBack = skin_background_brush_color;
	MenuInfo.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
	MenuInfo.dwStyle = MNS_AUTODISMISS;
	SetMenuInfo(mainmenu->GetSafeHmenu(), &MenuInfo);
	return true;
}

bool Init_MainFrame()
{
	mainframe = new G3DMainFrame;
	mainwindow = mainframe;
	mainframe->LoadFrame(IDR_MAIN_MENU, WS_OVERLAPPEDWINDOW, 0, 0);
	mainframe->ShowWindow(SW_HIDE);
	
	mainmenu = new CSkinMenu;

	mainmenu->LoadMenu(IDR_MAIN_MENU);


	bool owner_draw_menu = true;

	Create_Ownerdraw_Popup_Menu_Item("File", 0);
	Create_Ownerdraw_Popup_Menu_Item("Single Frame", 2);
	Create_Ownerdraw_Popup_Menu_Item("Tools", 4);
	Create_Ownerdraw_Popup_Menu_Item("Monitors", 6);
	Create_Ownerdraw_Popup_Menu_Item("Views", 8);
	Create_Ownerdraw_Popup_Menu_Item("Image Settings", 10);
	Create_Ownerdraw_Popup_Menu_Item("Gap Fill", 12);
	Create_Ownerdraw_Popup_Menu_Item("Help", 14);

	Create_Ownerdraw_Popup_Menu_Item("-", 1);
	Create_Ownerdraw_Popup_Menu_Item("-", 3);
	Create_Ownerdraw_Popup_Menu_Item("-", 5);
	Create_Ownerdraw_Popup_Menu_Item("-", 7);
	Create_Ownerdraw_Popup_Menu_Item("-", 9);
	Create_Ownerdraw_Popup_Menu_Item("-", 11);
	Create_Ownerdraw_Popup_Menu_Item("-", 13);

	if(owner_draw_menu)
	{
		Create_Ownerdraw_Menu_Item("New Project",                 IDC_NEW_PROJECT);
		Create_Ownerdraw_Menu_Item("Load Project",                IDC_LOAD_PROJECT);
		Create_Ownerdraw_Menu_Item("Save Project",                IDC_SAVE_PROJECT);
		Create_Ownerdraw_Menu_Item("Import Files",                IDC_IMPORT);
		Create_Ownerdraw_Menu_Item("Export Files",                IDC_EXPORT);
		Create_Ownerdraw_Menu_Item("Exit",                        IDC_EXIT);
		Create_Ownerdraw_Menu_Item("Load Fresh Frame",            IDC_LOAD_FRAME);
		Create_Ownerdraw_Menu_Item("Replace Image",               IDC_REPLACE_IMAGE);
		Create_Ownerdraw_Menu_Item("Load Session",                IDC_LOAD_SESSION);
		Create_Ownerdraw_Menu_Item("Save Session",                IDC_SAVE_SESSION);
		Create_Ownerdraw_Menu_Item("Import Layer Mask",           IDC_IMPORT_LAYERS);
		Create_Ownerdraw_Menu_Item("Import Depth Mask",           IDC_IMPORT_DEPTH_MASK);
		Create_Ownerdraw_Menu_Item("Export Layer Mask",           IDC_EXPORT_LAYER_MASK);
		Create_Ownerdraw_Menu_Item("Export Depth Mask",           IDC_EXPORT_DEPTH_MASK);
		Create_Ownerdraw_Menu_Item("Export Left Gap Mask",        IDC_EXPORT_LEFT_GAP_MASK);
		Create_Ownerdraw_Menu_Item("Export Right Gap Mask",       IDC_EXPORT_RIGHT_GAP_MASK);
		Create_Ownerdraw_Menu_Item("Import Stereo Settings",      IDC_IMPORT_STEREO_SETTINGS);
		Create_Ownerdraw_Menu_Item("Import Layer Edits",          IDC_IMPORT_LAYER_EDITS);
		Create_Ownerdraw_Menu_Item("Export Background Mesh",      IDC_EXPORT_BACKGROUND_MESH);
		Create_Ownerdraw_Menu_Item("Import Background Mesh",      IDC_IMPORT_BACKGROUND_MESH);
		Create_Ownerdraw_Menu_Item("Hi-Res Performance",          IDC_HI_RES_PERFORMANCE);
		Create_Ownerdraw_Menu_Item("Select All",                  IDC_SELECT_ALL);
		Create_Ownerdraw_Menu_Item("Clear Selection",             IDC_UNSELECT_LAYERS);
		Create_Ownerdraw_Menu_Item("Rename Selection",            IDC_RENAME_SELECTION);
		Create_Ownerdraw_Menu_Item("Merge Selection",             IDC_MERGE_LAYERS);
		Create_Ownerdraw_Menu_Item("Delete Selection",            IDC_DELETE_LAYERS);
		Create_Ownerdraw_Menu_Item("Evaporate Selection",         IDC_EVAPORATE_LAYERS);
		Create_Ownerdraw_Menu_Item("Split Selected Layer",        IDC_SPLIT_LAYER);
		Create_Ownerdraw_Menu_Item("Auto-Segment Image",          IDC_AUTO_SEGMENT_IMAGE);
		Create_Ownerdraw_Menu_Item("Get Perspective Projection",  IDC_GET_PERSPECTIVE_PROJECTION);
		Create_Ownerdraw_Menu_Item("Anchor Points Tool",          IDC_OPEN_ANCHOR_POINTS_TOOL);
		Create_Ownerdraw_Menu_Item("Virtual Camera",              IDC_OPEN_VIRTUAL_CAMERA_TOOL);
		Create_Ownerdraw_Menu_Item("Track Feature Points",        IDC_FEATURE_POINTS_TOOL);
		Create_Ownerdraw_Menu_Item("Plugins",                     IDC_PLUGINS);
		Create_Ownerdraw_Menu_Item("Manual Touch Ups",            IDC_TOUCHUPS_TOOL);
		Create_Ownerdraw_Menu_Item("Halo Tool",                   IDC_HALO_TOOL);
		Create_Ownerdraw_Menu_Item("Basic Depth",                 IDC_OPEN_DEPTH_SLIDER);
		Create_Ownerdraw_Menu_Item("Basic Scale",                 IDC_OPEN_SCALE_SLIDER);
		Create_Ownerdraw_Menu_Item("Orientation",                 IDC_PLANAR_PROJECTION);
		Create_Ownerdraw_Menu_Item("Contour Extrusion",           IDC_CONTOUR_EXTRUSION);
		Create_Ownerdraw_Menu_Item("Relief Map",                  IDC_RELIEF_MAP);
		Create_Ownerdraw_Menu_Item("Auto Alignment",              IDC_AUTO_ALIGNMENT);
		Create_Ownerdraw_Menu_Item("Shift Edges",                 IDC_SHIFT_EDGES);
		Create_Ownerdraw_Menu_Item("Apply Geometry",              IDC_APPLY_GEOMETRY);
		Create_Ownerdraw_Menu_Item("Track Outline",				  IDC_TRACK_OUTLINE);
		Create_Ownerdraw_Menu_Item("Copy Layers To All Frames",   IDC_LAYER_ALL_FRAMES);
		Create_Ownerdraw_Menu_Item("Copy Layers To Single Frame", IDC_LAYER_SINGLE_FRAME);
		Create_Ownerdraw_Menu_Item("Copy Selected Edits To All Frames", IDC_COPY_EDITS_ALL_FRAMES);
		Create_Ownerdraw_Menu_Item("Copy Selected Edits To Single Frame", IDC_COPY_EDITS_SINGLE_FRAME);
		Create_Ownerdraw_Menu_Item("Single Monitor",              IDC_SINGLE_MONITOR);
		Create_Ownerdraw_Menu_Item("Dual Monitor",                IDC_DUAL_MONITOR);
		Create_Ownerdraw_Menu_Item("Swap Monitors",               IDC_SWAP_MONITORS);
		Create_Ownerdraw_Menu_Item("Fullscreen App",              IDC_FULL_SCREEN_APP);
		Create_Ownerdraw_Menu_Item("Fullscreen 2D",               IDC_FULL_SCREEN_2D);
		Create_Ownerdraw_Menu_Item("Fullscreen 3D",               IDC_FULL_SCREEN_3D);
		Create_Ownerdraw_Menu_Item("Split Screen",                IDC_SPLIT_SCREEN);
		Create_Ownerdraw_Menu_Item("Widescreen",                  IDC_WIDE_SCREEN_3D);
		Create_Ownerdraw_Menu_Item("Anaglyph Mode",               IDC_ANAGLYPH);
		Create_Ownerdraw_Menu_Item("Render Grid",                 IDC_GRID_BUTTON);
		Create_Ownerdraw_Menu_Item("Show Camera",                 IDC_CAMERA_BUTTON);
		Create_Ownerdraw_Menu_Item("Stereo Settings",             IDC_STEREO_SETTINGS);
		Create_Ownerdraw_Menu_Item("3D View Options",             IDC_3D_VIEW_OPTIONS);
		Create_Ownerdraw_Menu_Item("2D View Options",             IDC_2D_VIEW_OPTIONS);
		Create_Ownerdraw_Menu_Item("Flip Image Vertical",         IDC_FLIP_IMAGE_VERTICAL);
		Create_Ownerdraw_Menu_Item("Flip Image Horizontal",       IDC_FLIP_IMAGE_HORIZONTAL);
		Create_Ownerdraw_Menu_Item("Flip Layers Vertical",        IDC_FLIP_LAYERS_VERTICAL);
		Create_Ownerdraw_Menu_Item("Flip Layers Horizontal",      IDC_FLIP_LAYERS_HORIZONTAL);
		Create_Ownerdraw_Menu_Item("Flip Depth Mask Vertical",    IDC_FLIP_DEPTH_MASK_VERTICAL);
		Create_Ownerdraw_Menu_Item("Flip Depth Mask Horizontal",  IDC_FLIP_DEPTH_MASK_HORIZONTAL);
		Create_Ownerdraw_Menu_Item("Flip Depth Mask Depth",       IDC_FLIP_DEPTH_MASK_DEPTH);
		Create_Ownerdraw_Menu_Item("Flip Red/Blue",               IDC_FLIP_RED_BLUE);
		Create_Ownerdraw_Menu_Item("Open Gap-Fill Editor",        IDC_OPEN_GAPFILL_EDITOR);
		Create_Ownerdraw_Menu_Item("Gimpel3D Reference",          IDC_SHOW_HELP);
		Create_Ownerdraw_Menu_Item("Color Scheme",                IDC_COLOR_SCHEME);
	}


	mainframe->SetMenu(mainmenu);

	int width, height;
	Get_Desktop_Dimensions(&width, &height);
	mainframe->SetWindowPos(0, 0, 0, width, height, 0);
	mainframe->m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mainframe->SetIcon(mainframe->m_hIcon, TRUE);			// Set big icon
	mainframe->SetIcon(mainframe->m_hIcon, FALSE);		// Set small icon

	mainmenu->CheckMenuItem(IDC_SINGLE_MONITOR, MF_CHECKED);
	mainframe->OnFullScreen2d();
	mainframe->m_menuTipManager.Install(mainframe);

	Update_Menu_Color();

	mainframe->SetFont(skin_font);//like it makes a difference

	return true;
}

bool Check_Save_Session()
{
	return mainframe->CheckSaveSession();
}

void Check_Menu_Item(int id, bool b)
{
	if(b)mainmenu->CheckMenuItem(id, MF_CHECKED);
	else mainmenu->CheckMenuItem(id, MF_UNCHECKED);
}

bool Update_View_Settings_Checkboxes()
{
	Check_Menu_Item(IDC_GRID_BUTTON, render_grid);
	Check_Menu_Item(IDC_FULL_SCREEN_2D, fullscreen_2d);
	Check_Menu_Item(IDC_FULL_SCREEN_3D, fullscreen_3d);
	Check_Menu_Item(IDC_WIDE_SCREEN_3D, wide_screen);
	Check_Menu_Item(IDC_SPLIT_SCREEN, split_screen);
	Update_Toolbar_View_Buttons();
	return true;
}

bool Swap_Monitors(bool b)
{
	swap_monitors = b;
	int width, height;
	Get_Desktop_Dimensions(&width, &height);
	if(dual_monitor)
	{
		if(swap_monitors)
		{
			mainframe->ShowWindow(SW_RESTORE);
			mainwindow->SetWindowPos(0, 0, 0, width, height, 0);
			SetWindowPos(Get_3D_Frame_Window(), 0, width, 0, width, height, 0);
		}
		else
		{
			mainframe->ShowWindow(SW_RESTORE);
			mainwindow->SetWindowPos(0, width, 0, width, height, 0);
			SetWindowPos(Get_3D_Frame_Window(), 0, 0, 0, width, height, 0);
		}
	}
	return false;
}

bool Set_Single_Monitor_Layout()
{
	if(dual_monitor)
	{
		split_screen = false;
		mainframe->ShowWindow(SW_MAXIMIZE);
		dual_monitor = false;
		Close_3D_Frame_Dialog();
		Show_3D_View(false);
		Show_2D_View(true);
		Resize_Layout();
		Update_Toolbar_View_Buttons();
		return true;
	}
	return false;
}

bool Set_Dual_Monitor_Layout()
{
	if(!dual_monitor)
	{
		split_screen = false;
		dual_monitor = true;
		Init_3D_Frame_Dialog();
		Show_3D_View(true);
		Show_2D_View(true);
		Swap_Monitors(swap_monitors);
		Resize_Layout();
		Update_Toolbar_View_Buttons();
		return true;
	}
	return false;
}

bool Free_MainFrame()
{
	Free_Toolbar_Dlg();
	delete mainmenu;
	delete mainframe;
	Free_Ownerdraw_Menu_Items();
	mainmenu = 0;
	mainframe = 0;
	return true;
}

void G3DMainFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	mainmenu->DrawItem(lpDrawItemStruct);
}

void G3DMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (lpMeasureItemStruct->CtlType == ODT_MENU)
	{
		CMenu* pMenu;
		_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
		if (pThreadState->m_hTrackingWindow == m_hWnd)
		{
			// start from popup
			pMenu = CMenu::FromHandle(pThreadState->m_hTrackingMenu);
		}
		else
		{
			// start from menubar
			pMenu = GetMenu();
		}
		if (pMenu != NULL)
		{
			pMenu->MeasureItem(lpMeasureItemStruct);
		}
		else
		{
			TRACE1("Warning: unknown WM_MEASUREITEM for menu item 0x%04X.\n", lpMeasureItemStruct->itemID);
		}
	}
	else
	{
		CWnd* pChild = GetDescendantWindow(lpMeasureItemStruct->CtlID, TRUE);
		if (pChild != NULL && pChild->SendChildNotifyLastMsg())
			return;     // eaten by child
	}
	// not handled - do default
	Default();
}

void G3DMainFrame::OnClose() 
{
	Quit();
}

BOOL G3DMainFrame::OnEraseBkgnd(CDC* pDC) 
{
	CBrush* pOldBrush = pDC->SelectObject(&dark_skin_background_brush_color);

	CRect rect;
	pDC->GetClipBox(&rect);     // Erase the area needed
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),
	PATCOPY);
	pDC->SelectObject(pOldBrush);

	return TRUE;
}

int G3DMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	EnableDocking(CBRS_ALIGN_ANY);

	Init_Toolbar_Dlg();

	SetProjectTitle();
	return 0;
}

void G3DMainFrame::SetProjectTitle() 
{
	char title[512];
	char name[512];
	if(Get_Project_Name(name))
	{
		sprintf(title, "\"%s\" - Gimpel3D - Stereo Conversion Editor - Version 2.0", name);
	}
	else
	{
		strcpy(title, "Gimpel3D - Stereo Conversion Editor - Version 2.0");
	}
	SetWindowText(title);
}

void G3DMainFrame::OnLoadProject() 
{
	if(frame_loaded)
	{
		if(SkinMsgBox("This will start a new session, continue?", "Gimpel3D", MB_YESNO)==IDNO)
		{
			return;
		}
	}
	char data[512];
	if(Browse(data, "sss", false))
	{
		if(Load_Project(data, true))
		{
			List_Frames();
			SetProjectTitle();
			Fit_3D_Window();
			Load_Feature_Points_Sequence(data);
			Set_Camera_To_Target();//set camera directly to stored view
		}
		else
		{
			List_Frames();
		}
		redraw_edit_window = true;
		redraw_frame = true;
	}
}

void G3DMainFrame::OnNewProject() 
{
	if(frame_loaded)
	{
		if(SkinMsgBox("This will start a new session, continue?", "Gimpel3D", MB_YESNO)==IDNO)
		{
			return;
		}
	}
	char data[512];
	if(Browse(data, "sss", false))
	{
		if(Exists(data))
		{
			if(SkinMsgBox("WARNING! This project file already exists! Overwrite this project file? (If you choose \"Yes\" the old project cannot be restored.", data, MB_YESNO)==IDNO)
			{
				return;
			}
		}
		if(Create_New_Project(data))
		{
			List_Frames();
			SetProjectTitle();
		}
		else
		{
			List_Frames();
		}
		redraw_edit_window = true;
		redraw_frame = true;
	}
}

void G3DMainFrame::OnSaveProject() 
{
	if(!project_loaded)
	{
		OnSaveSession();
		return;
	}
	char data[512];
	if(Browse(data, "sss", true))
	{
		Save_Project(data);
		Save_Feature_Points_Sequence(data);
		SkinMsgBox("Project save completed");
	}
}

void G3DMainFrame::OnExit() 
{
	Quit();
}

void G3DMainFrame::OnImport() 
{
	if(!project_loaded)
	{
		if(SkinMsgBox("There is no sequence project currently loaded, would you like to create a fresh project?", 0, MB_YESNO)==IDYES)
		{
			OnNewProject();
		}
	}
	else
	{
		Open_Import_Options_Dialog();
	}
}

void G3DMainFrame::OnExport() 
{
	Open_Export_Options_Dialog();
}

void G3DMainFrame::OnSaveSession() 
{
	char data[512];
	if(!frame_loaded)
	{
		SkinMsgBox("No frame loaded!", 0, MB_OK);
		return;
	}
	Get_Frame_Filename(data);
	if(Browse(data, "sfi", true))
	{
		Save_Frame_Info(data, true);//save camera views
		Save_Scene_Primitives();//special case
		Save_Feature_Points_Single_Frame(data);
		Report_Saving_Single_Frame_Session();
		//because this is not part of a project save
	}
}

void G3DMainFrame::OnReplaceImage() 
{
	if(!frame_loaded)
	{
		if(SkinMsgBox("There is no frame currently loaded, do you want to start a new single-frame session?", 0, MB_YESNO)==IDNO)
		{
			return;
		}
		OnLoadFrame();
		return;
	}
	char data[512];
	if(Browse(data, "*", false))
	{
		Replace_Frame_Image(data);
	}
}

bool G3DMainFrame::CheckSaveSession()
{
	return true;
}

void G3DMainFrame::OnLoadSession() 
{
	if(!CheckSaveSession())
	{
		return;
	}
	if(frame_loaded)
	{
		if(SkinMsgBox("This will start a new session, continue?", "Gimpel3D", MB_YESNO)==IDNO)
		{
			return;
		}
	}
	char data[512];
	if(Browse(data, "sfi", false))
	{
		Free_Project();
		List_Frames();
		Load_Frame_Info(data);
		Set_Camera_To_Target();//set camera directly to stored view
		Update_Layer_List();
		Fit_3D_Window();
		Load_Feature_Points_Single_Frame(data);
	}
}

void G3DMainFrame::OnLoadFrame() 
{
	if(frame_loaded)
	{
		if(SkinMsgBox("This will start a new session, continue?", "Gimpel3D", MB_YESNO)==IDNO)
		{
			return;
		}
	}
	char data[512];
	if(Browse(data, "*", false))
	{
		_fov = aperture = DEFAULT_FOV = 45;
		Display_FOV(_fov);
		Free_Project();
		List_Frames();
		Update_Layer_List();
		Load_Frame_Image(data, true);
		Fit_3D_Window();
	}
}

void G3DMainFrame::OnImportLayers() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "*", false))
	{
		if(SkinMsgBox("Apply image filter to remove stray pixels?", " ", MB_YESNO)==IDYES)
		{
			PreFilter_Image(data);
		}
		else
		{
		}
		Load_Layer_Mask_Image(data);
		Update_Layer_List();
	}
}

void G3DMainFrame::OnImportDepthMask() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "*", false))
	{
		Set_Global_Relief_Image(data);
	}
}

void G3DMainFrame::OnExportLayerMask() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "bmp", true))
	{
		Save_Layer_Map(data);
	}
}

void G3DMainFrame::OnExportDepthMask() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "bmp", true))
	{
		Save_Frame_Depth_Map(data);
	}
}

void G3DMainFrame::OnImportStereoSettings() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "sfi", false))
	{
		Load_Stereo_Info(data);
		Update_Stereo_Sliders();
		redraw_frame = true;
	}
}

bool Toggle_Menu_Item(int id)
{
	int s = mainmenu->GetMenuState(id, MF_BYCOMMAND);
	if(s & MF_CHECKED)
	{
		mainmenu->CheckMenuItem(id, MF_UNCHECKED);
		return false;
	}
	else
	{
		mainmenu->CheckMenuItem(id, MF_CHECKED);
		return true;
	}
	return false;
}

void G3DMainFrame::OnFlipImageVertical() 
{
	flip_loaded_images_vertical = Toggle_Menu_Item(IDC_FLIP_IMAGE_VERTICAL);
	ReLoad_Current_Project_Frame();
}

void G3DMainFrame::OnFlipImageHorizontal() 
{
	flip_loaded_images_horizontal = Toggle_Menu_Item(IDC_FLIP_IMAGE_HORIZONTAL);
	ReLoad_Current_Project_Frame();
}

void G3DMainFrame::OnFlipLayersVertical() 
{
	Select_All_Layers(false);
	flip_loaded_layers_vertical = Toggle_Menu_Item(IDC_FLIP_LAYERS_VERTICAL);
	Tag_All_Layers_For_Reload();
	ReLoad_Current_Project_Layers();
	redraw_edit_window = true;
	redraw_frame = true;
}

void G3DMainFrame::OnFlipLayersHorizontal() 
{
	Select_All_Layers(false);
	flip_loaded_layers_horizontal = Toggle_Menu_Item(IDC_FLIP_LAYERS_HORIZONTAL);
	Tag_All_Layers_For_Reload();
	ReLoad_Current_Project_Layers();
	redraw_edit_window = true;
	redraw_frame = true;
}

void G3DMainFrame::OnFlipDepthMaskVertical() 
{
	flip_relief_vertical = Toggle_Menu_Item(IDC_FLIP_DEPTH_MASK_VERTICAL);
	Update_Layers_Relief(false);
	Project_Layers();
	redraw_frame = true;
}

void G3DMainFrame::OnFlipDepthMaskHorizontal() 
{
	flip_relief_horizontal = Toggle_Menu_Item(IDC_FLIP_DEPTH_MASK_HORIZONTAL);
	Update_Layers_Relief(false);
	Project_Layers();
	redraw_frame = true;
}

void G3DMainFrame::OnFlipDepthMaskDepth() 
{
	flip_relief_depth = Toggle_Menu_Item(IDC_FLIP_DEPTH_MASK_DEPTH);
	Update_Layers_Relief(false);
	Project_Layers();
	redraw_frame = true;
}

void G3DMainFrame::OnFlipRedBlue() 
{
	flip_redblue = Toggle_Menu_Item(IDC_FLIP_RED_BLUE);
	ReLoad_Current_Project_Frame();
}


void G3DMainFrame::OnDualMonitor() 
{
	mainmenu->CheckMenuItem(IDC_SINGLE_MONITOR, MF_UNCHECKED);
	mainmenu->CheckMenuItem(IDC_DUAL_MONITOR, MF_CHECKED);
	Set_Dual_Monitor_Layout();
}

void G3DMainFrame::OnSingleMonitor() 
{
	mainmenu->CheckMenuItem(IDC_SINGLE_MONITOR, MF_CHECKED);
	mainmenu->CheckMenuItem(IDC_DUAL_MONITOR, MF_UNCHECKED);
	Set_Single_Monitor_Layout();
}

void G3DMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	if(cx>=min_app_width||cy>=min_app_height)
	{
		CFrameWnd::OnSize(nType, cx, cy);
		Resize_Layout();
	}
	else
	{
		SetWindowPos(0, 0, 0, min_app_width, min_app_height, SWP_NOMOVE);
		CFrameWnd::OnSize(nType, min_app_width, min_app_height);
		Resize_Layout();
	}
}

void G3DMainFrame::OnSwapMonitors() 
{
	Swap_Monitors(Toggle_Menu_Item(IDC_SWAP_MONITORS));
}
/*
#define WS_OVERLAPPED       0x00000000L
#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L
#define WS_MINIMIZE         0x20000000L
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L
#define WS_MAXIMIZE         0x01000000L
#define WS_CAPTION          0x00C00000L
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L*/

void G3DMainFrame::OnFullScreenApp() 
{
	fullscreen_app = Toggle_Menu_Item(IDC_FULL_SCREEN_APP);
	if(fullscreen_app)
	{
		ModifyStyle(WS_CAPTION|WS_THICKFRAME, 0, SWP_FRAMECHANGED);
		int width, height;
		Get_Desktop_Dimensions(&width, &height);
		SetWindowPos(0, 0, 0, width, height, 0);
	}
	else
	{
		ModifyStyle(0, WS_CAPTION|WS_THICKFRAME, SWP_FRAMECHANGED);
		ShowWindow(SW_MAXIMIZE);
	}
	Update_Toolbar_View_Buttons();
	Resize_Layout();
}

void G3DMainFrame::OnFullScreen2d() 
{
	Fit_2D_Window();
	split_screen = false;
	fullscreen_2d = true;
	Show_2D_View(true);
	if(!dual_monitor)
	{
		Show_3D_View(false);
		fullscreen_3d = false;
	}
	Update_View_Settings_Checkboxes();
}

void G3DMainFrame::OnFullScreen3d() 
{
	fullscreen_3d = true;
	split_screen = false;
	Fit_3D_Window();
	if(!dual_monitor)
	{
		Show_3D_View(true);
		Show_2D_View(false);
		fullscreen_2d = false;
		fullscreen_3d = true;
	}
	Update_View_Settings_Checkboxes();
}

void G3DMainFrame::OnWideScreen3d() 
{
	wide_screen = Toggle_Menu_Item(IDC_WIDE_SCREEN_3D);
	Fit_3D_Window();
	Update_View_Settings_Checkboxes();
}

void G3DMainFrame::OnSplitScreen() 
{
	if(dual_monitor)
	{
	}
	else
	{
		split_screen = true;
		fullscreen_2d = false;
		fullscreen_3d = false;
		Fit_Splitscreen_Window();
		Fit_3D_Window();
		Show_3D_View(true);
		Show_2D_View(true);
	}
	Update_View_Settings_Checkboxes();
}

void G3DMainFrame::OnAnaglyph() 
{
	Set_Anaglyph_Mode(Toggle_Menu_Item(IDC_ANAGLYPH));
	Update_Toolbar_Anaglyph_Button();
}

void G3DMainFrame::OnStereoSettings() 
{
	Open_Stereo_Settings_Dlg();
}

void G3DMainFrame::On3dViewOptions() 
{
	Open_3D_View_Options_Dlg();
}

void G3DMainFrame::On2dViewOptions() 
{
	Open_2D_View_Options_Dlg();
}

void G3DMainFrame::OnSelectAll() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	Select_All_Layers(true);
}

void G3DMainFrame::OnUnselectLayers() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("No selected layers!", 0, MB_OK);
		return;
	}
	Select_All_Layers(false);
}

void G3DMainFrame::OnRenameSelection() 
{
	int n = Num_Layers();
	int index = -1;
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			if(index!=-1)
			{
				SkinMsgBox("Multiple selection, select single to rename", 0, MB_OK);
				return;
			}
			index = i;
		}
	}
	if(index==-1)
	{
		SkinMsgBox("Nothing selected.", 0, MB_OK);
		return;
	}
	Rename_Layer(index);
}

void G3DMainFrame::OnMergeLayers() 
{
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("Nothing selected.", 0, MB_OK);
		return;
	}
	Merge_Selection();
}

void G3DMainFrame::OnDeleteLayers() 
{
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("Nothing selected.", 0, MB_OK);
		return;
	}
	vector<int> sel_ids;
	int n = Num_Layers();
	int i;
	for(int i = 0;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			sel_ids.push_back(Get_Layer_ID(i));
		}
	}
	n = sel_ids.size();
	for(i = 0;i<n;i++)
	{
		Delete_Layer(sel_ids[i]);
	}
	Select_All_Layers(false);
	Update_Layer_Data(Get_Layer_ID(0));
	sel_ids.clear();
	Update_Layer_List();
	redraw_frame = true;
}

void G3DMainFrame::OnEvaporateLayers() 
{
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("Nothing selected.", 0, MB_OK);
		return;
	}
	int n = Num_Layers();
	for(int i = 1;i<n;i++)
	{
		if(Layer_Is_Selected(i))
		{
			Evaporate_Layer(Get_Layer_ID(i));
			redraw_frame = true;
			redraw_edit_window = true;
		}
	}
	Update_Modified_Layers();
	Clear_Modified_Layers();
}

void G3DMainFrame::OnSplitLayer() 
{
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("No selection!", 0, MB_OK);
	}
	else
	{
		Open_Rotoscope_Color_Difference_Dlg();
	}
}

void G3DMainFrame::OnOpenDepthSlider() 
{
	Open_Depth_Slider_Dlg();
}

void G3DMainFrame::OnOpenScaleSlider() 
{
	Open_Scale_Slider_Dlg();
}

void G3DMainFrame::OnPlanarProjection() 
{
	Open_Planar_Projection_Dlg();
}

void G3DMainFrame::OnContourExtrusion() 
{
	Open_Contour_Extrusion_Dlg();
}

void G3DMainFrame::OnReliefMap() 
{
	Open_Relief_Map_Dlg();
}

void G3DMainFrame::OnAutoAlignment() 
{
	Open_Alignment_Options_Dialog();
}


void G3DMainFrame::OnLayerAllFrames() 
{
	if(Get_Num_Frames()==0)
	{
		SkinMsgBox("No sequence loaded.", 0, MB_OK);
		return;
	}
	int n = Get_Current_Project_Frame();
	Copy_Layers_To_All_Frames(n);
	Set_Project_Frame_Layers_Loaded();
	SkinMsgBox("Layers copied to all frames.");
}

void G3DMainFrame::OnLayerSingleFrame() 
{
	if(Get_Num_Frames()==0)
	{
		SkinMsgBox("No sequence loaded.", 0, MB_OK);
		return;
	}
	if(Get_Num_Frames()<2)
	{
		SkinMsgBox("Single frame, no other possible frames to copy to!", 0, MB_OK);
		return;
	}
	ListSelectDlg dlg;
	int current = Get_Current_Project_Frame();
	int n = Get_Num_Frames();
	char name[512];
	for(int i = 0;i<n;i++)
	{
		Get_Frame_Name(i, name);
		Get_Display_Name(name);
		dlg.Add_List_Item(name);
	}
	strcpy(dlg.title, "Select Frame To Copy Layers To");
	dlg.DoModal();
	if(dlg.got_selection)
	{
		if(dlg.selection==current)
		{
			SkinMsgBox("The current frame was selected, no reason to copy.");
		}
		else
		{
			Copy_Layers_To_Frame(current, dlg.selection);
			Set_Project_Frame_Layers_Loaded(dlg.selection);
			Set_Project_Frame(dlg.selection);
		}
	}
}

void Switch_View_Mode()
{
	if(split_screen)
	{
		mainframe->OnFullScreen3d();
	}
	else if(fullscreen_2d)
	{
		mainframe->OnSplitScreen();
		SetFocus(Get_2D_Window());
	}
	else if(fullscreen_3d)
	{
		mainframe->OnFullScreen2d();
	}
}

void G3DMainFrame::OnShiftEdges() 
{
	Open_Shift_Edges_Dlg();
}

void G3DMainFrame::OnOpenPostEditor() 
{
	//render out images with motion map
}

void G3DMainFrame::OnOpenGapfillEditor() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No frame loaded!");
		return;
	}
	Open_GapFill_Tools();
	Show_2D_View(false);
	if(!dual_monitor)
	{
		Show_3D_View(false);
	}
	Show_GapFill_View(true);
}

bool Restore_Windows_From_GapFill_Tool()
{
	Show_GapFill_View(false);
	if(fullscreen_2d)
	{
		mainframe->OnFullScreen2d();
	}
	else if(fullscreen_3d)
	{
		mainframe->OnFullScreen3d();
	}
	else if(split_screen)
	{
		mainframe->OnSplitScreen();
	}
	return true;
}

void G3DMainFrame::OnApplyGeometry() 
{
	Open_Geometry_Tool();
}

void G3DMainFrame::OnAutoSegmentImage() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No frame loaded!");
		return;
	}
	Open_AutoSegment_Tool();
}

void G3DMainFrame::OnHelpReadmeFirst() 
{
}

void G3DMainFrame::OnHelpSingleImage() 
{
}

void G3DMainFrame::OnHelpSequence() 
{
}

void G3DMainFrame::OnKeyCommands() 
{
}

void G3DMainFrame::OnHelpRotoscoping() 
{
}

void G3DMainFrame::OnHelpGapfill() 
{
}

void G3DMainFrame::OnHelpStereoPreview() 
{
}

void G3DMainFrame::OnHelp3dEditing() 
{
}

void G3DMainFrame::OnHelpImportingData() 
{
}

void G3DMainFrame::OnHelpExportingData() 
{
}


void G3DMainFrame::OnHelpToolsReference() 
{
}

void G3DMainFrame::OnShowHelp() 
{
	Open_Help_Dlg();
}

void G3DMainFrame::OnGetPerspectiveProjection() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No frame loaded!", 0, MB_OK);
		return;
	}
	Open_Perspective_Tool();
}

void G3DMainFrame::OnOpenShapesTool() 
{
	Open_Shapes_Tool();
}

void G3DMainFrame::OnOpenAnchorPointsTool() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No frame loaded!");
		return;
	}
	Open_Anchor_Points_Tool();
}

void G3DMainFrame::OnOpenVirtualSetTool() 
{
	Open_Virtual_Set_Tool();
}

void G3DMainFrame::OnFeaturePointsTool() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	Open_Feature_Points_Tool();
}


void G3DMainFrame::OnTrackOutline() 
{
	Open_Track_Outlines_Tool();
}

void G3DMainFrame::OnColorScheme() 
{
	Run_Color_Dlg();
}

void G3DMainFrame::OnPreviewTool() 
{
	Open_Preview_Tool();
}

void G3DMainFrame::OnCopyEditsAllFrames() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("Nothing selected.", 0, MB_OK);
		return;
	}
	if(Get_Num_Frames()<2)
	{
		SkinMsgBox("No frames to copy to!", 0, MB_OK);
		return;
	}
	//this has to switch frames
	int n = Get_Current_Project_Frame();
	Copy_Selected_Layer_Edits_To_All_Frames(n);
	SkinMsgBox("Layer edits copied to all frames.");
}

void G3DMainFrame::OnCopyEditsSingleFrame() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	if(Num_Selected_Layers()==0)
	{
		SkinMsgBox("Nothing selected.", 0, MB_OK);
		return;
	}
	if(Get_Num_Frames()<2)
	{
		SkinMsgBox("No frames to copy to!", 0, MB_OK);
		return;
	}
	ListSelectDlg dlg;
	int current = Get_Current_Project_Frame();
	int n = Get_Num_Frames();
	char name[512];
	for(int i = 0;i<n;i++)
	{
		Get_Frame_Name(i, name);
		Get_Display_Name(name);
		dlg.Add_List_Item(name);
	}
	strcpy(dlg.title, "Select Frame To Copy Layers To");
	dlg.DoModal();
	if(dlg.got_selection)
	{
		if(dlg.selection==current)
		{
			SkinMsgBox("The current frame was selected, no reason to copy.");
		}
		else
		{
			Copy_Selected_Layer_Edits_To_Frame(current, dlg.selection);
			Set_Project_Frame(dlg.selection);
		}
	}
}

void G3DMainFrame::OnOpenVirtualCameraTool() 
{
	if(Get_Num_Frames()==0)
	{
		SkinMsgBox("No sequence loaded.", 0, MB_OK);
		return;
	}
	Open_Virtual_Camera_Tool();
}

void G3DMainFrame::OnGridButton() 
{
	render_grid = (Toggle_Menu_Item(IDC_GRID_BUTTON));
	Update_Toolbar_Grid_Button();
	redraw_frame = true;
}

void G3DMainFrame::OnCameraButton() 
{
	render_camera = (Toggle_Menu_Item(IDC_CAMERA_BUTTON));
	Update_Toolbar_Camera_Button();
	if(render_camera)
	{
		ReCalc_Focal_Plane_Preview();
	}
	redraw_frame = true;
}

void G3DMainFrame::OnPaintTool() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No frame loaded!");
		return;
	}
	Open_Paint_Tool();
}

void G3DMainFrame::OnPlugins() 
{
	Open_Plugins_Dialog();
}

void G3DMainFrame::OnTouchupsTool() 
{
	Open_TouchUp_Tool();
}

void G3DMainFrame::OnHaloTool() 
{
	Open_Halo_Tool();
}

void G3DMainFrame::OnImportLayerEdits() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "sfi", false))
	{
		Import_Layer_Edits(data);
		redraw_frame = true;
	}
}

void G3DMainFrame::OnHiResPerformance() 
{
}

void G3DMainFrame::OnExportBackgroundMesh() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "bgm", true))
	{
		Export_Background_Mesh(data);
	}
}

void G3DMainFrame::OnImportBackgroundMesh() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "bgm", false))
	{
		Import_Background_Mesh(data);
	}
}

void G3DMainFrame::OnExportLeftGapMask() 
{
	char file[512];
	if(Browse(file, "*", true))
	{
		unsigned char *rgb = new unsigned char[Frame_Width()*Frame_Height()*3];
		Get_Left_View_Gapmask(rgb);
		Save_Watermarked_Image(rgb, Frame_Width(), Frame_Height(), file);
		delete[] rgb;
		Print_To_Console("Saved image %s..", file);
		redraw_gapfill_window = true;
	}
}

void G3DMainFrame::OnExportRightGapMask() 
{
	char file[512];
	if(Browse(file, "*", true))
	{
		unsigned char *rgb = new unsigned char[Frame_Width()*Frame_Height()*3];
		Get_Right_View_Gapmask(rgb);
		Save_Watermarked_Image(rgb, Frame_Width(), Frame_Height(), file);
		delete[] rgb;
		Print_To_Console("Saved image %s..", file);
		redraw_gapfill_window = true;
	}
}
