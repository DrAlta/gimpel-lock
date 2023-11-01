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
// HelpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "HelpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char help_path[512];



/////////////////////////////////////////////////////////////////////////////
// HelpDlg dialog

HelpDlg *helpDlg = 0;

HelpDlg::HelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(HelpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(HelpDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void HelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(HelpDlg)
	DDX_Control(pDX, IDC_FORWARD, m_Forward);
	DDX_Control(pDX, IDC_BACK, m_Back);
	DDX_Control(pDX, IDC_HELP_TOC, m_HelpToc);
	DDX_Control(pDX, IDC_HTML_WINDOW, m_HtmlWindow);
	//}}AFX_DATA_MAP
	ResizeLayout();

}


BEGIN_MESSAGE_MAP(HelpDlg, CDialog)
	//{{AFX_MSG_MAP(HelpDlg)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_HELP_TOC, OnSelchangedHelpToc)
	ON_NOTIFY(NM_CLICK, IDC_HELP_TOC, OnClickHelpToc)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_BN_CLICKED(IDC_FORWARD, OnForward)
	ON_WM_VSCROLL()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


enum
{
	hlp_OVERVIEW,
	hlp_OVERVIEW_READ_THIS_FIRST,
	hlp_OVERVIEW_STEREO_CONVERSION,
	hlp_OVERVIEW_CONVERTING_A_SINGLE_IMAGE,
	hlp_OVERVIEW_CONVERTING_A_SEQUENCE,
	hlp_CONVERSION_STEPS,
	hlp_CONVERSION_STEPS_IMPORTING_DATA,
	hlp_CONVERSION_STEPS_2D_ROTOSCOPING,
	hlp_CONVERSION_STEPS_3D_EDITING,
	hlp_CONVERSION_STEPS_STEREO_SETTINGS,
	hlp_CONVERSION_STEPS_FILLING_GAPS,
	hlp_CONVERSION_STEPS_EXPORTING_DATA,
	hlp_REFERENCE,
	hlp_REFERENCE_MAIN_WORKSPACE,
	hlp_REFERENCE_APPLICATION_LAYOUT,
	hlp_REFERENCE_MAIN_MENU,
	hlp_REFERENCE_MAIN_TOOLBAR,
	hlp_REFERENCE_SIDE_BAR,
	hlp_REFERENCE_TIMELINE_SLIDER,
	hlp_REFERENCE_EDIT_RENDER_VIEW,
	hlp_REFERENCE_POPUP_MENU,
	hlp_REFERENCE_KB_MOUSE_CONTROLS,
	hlp_REFERENCE_2D_EDITING,
	hlp_REFERENCE_2D_VIEW_OPTIONS,
	hlp_REFERENCE_ROTOSCOPING,
	hlp_REFERENCE_SPLIT_LAYER,
	hlp_REFERENCE_SHIFT_EDGES,
	hlp_REFERENCE_TRACK_OUTLINES,
	hlp_REFERENCE_AUTO_SEGMENT,
	hlp_REFERENCE_3D_EDITING,
	hlp_REFERENCE_MISC_TOOLS,
	hlp_REFERENCE_FEATURE_POINTS,
	hlp_REFERENCE_LINK_POINTS,
	hlp_REFERENCE_VIRTUAL_CAMERA,
	hlp_REFERENCE_PLUGIN_MANAGER,
	hlp_REFERENCE_TOUCHUPS,
	hlp_REFERENCE_ANCHOR_POINTS,
	hlp_REFERENCE_HALO_TOOL,
	hlp_REFERENCE_PROJECTION_SETTINGS,
	hlp_REFERENCE_COLOR_SCHEME,
	hlp_REFERENCE_3D_VIEW_OPTIONS,
	hlp_REFERENCE_PERSPECTIVE_PROJECTION,
	hlp_REFERENCE_DEPTH,
	hlp_REFERENCE_SCALE,
	hlp_REFERENCE_ORIENTATION,
	hlp_REFERENCE_AUTO_ALIGNMENT,
	hlp_REFERENCE_RELIEF_MAP,
	hlp_REFERENCE_CONTOUR_EXTRUSION,
	hlp_REFERENCE_PROJECTION_GEOMETRY,
	hlp_REFERENCE_FINISHING,
	hlp_REFERENCE_STEREO_SETTINGS,
	hlp_REFERENCE_FILLING_GAPS,
	hlp_REFERENCE_IMPORTING_EXPORTING,
	hlp_REFERENCE_IMPORTING,
	hlp_REFERENCE_EXPORTING,
	hlp_REFERENCE_EXAMPLES,
	hlp_REFERENCE_EXAMPLE_BASIC_SCENE_CONSTRUCTION,
	hlp_REFERENCE_EXAMPLE_VIRTUAL_CAMERA_PATH,
	hlp_REFERENCE_EXAMPLE_ROTO_AUTOMATION,
	hlp_REFERENCE_EXAMPLE_RELIEF_TEXTURING,
	hlp_REFERENCE_EXAMPLE_HALO_EFFECTS,
	hlp_REFERENCE_EXAMPLE_GEOMETRY_PROJECTION,
	hlp_REFERENCE_EXAMPLE_CONTOUR_EXTRUSION,
	hlp_REFERENCE_EXAMPLE_SHIFTING_EDGES,
	hlp_REFERENCE_EXAMPLE_ANCHOR_POINTS,
	hlp_REFERENCE_EXAMPLE_TOUCH_UPS,
	hlp_REFERENCE_EXAMPLE_FILLING_GAPS,
	hlp_REFERENCE_EXAMPLE_DEPTH_AND_LAYER_MASKS,
	hlp_FUTURE_DEVELOPMENT,
	hlp_CHARACTER_ANIMATION_SYSTEM,
	hlp_PLUGIN_SYSTEM,
	hlp_CONVERSION_MANAGEMENT,
	hlp_AUTOMATED_CONVERSION,
	hlp_NUM_HELP_PAGES
};

class HELP_PAGE
{
public:
	HELP_PAGE()
	{
		id = -1;
		strcpy(title, "NOTITLE");
		strcpy(file, "NOFILE");
		tree_item = 0;
	}
	~HELP_PAGE()
	{
	}
	int id;
	char title[512];
	char file[512];
	HTREEITEM tree_item;
};

HELP_PAGE help_pages[hlp_NUM_HELP_PAGES];

int current_help_id = -1;


HELP_PAGE* Find_Help_Page(char *file)
{
	for(int i = 0;i<hlp_NUM_HELP_PAGES;i++)
	{
		if(!strcmp(help_pages[i].file, file))
		{
			return &help_pages[i];
		}
	}
	return 0;
}

bool Get_Help_File(int help_id, char *res)
{
	switch(help_id)
	{
#ifdef SHAREWARE_VERSION
		case hlp_OVERVIEW:strcpy(res, "Gimpel3D_OverviewSW.html");return true;
#else
		case hlp_OVERVIEW:strcpy(res, "Gimpel3D_Overview.html");return true;
#endif
		case hlp_OVERVIEW_READ_THIS_FIRST:strcpy(res, "Gimpel3D_ReadThisFirst.html");return true;
		case hlp_OVERVIEW_STEREO_CONVERSION:strcpy(res, "Gimpel3D_StereoConversion.html");return true;
		case hlp_OVERVIEW_CONVERTING_A_SINGLE_IMAGE:strcpy(res, "Gimpel3D_ConvertingASingleImage.html");return true;
		case hlp_OVERVIEW_CONVERTING_A_SEQUENCE:strcpy(res, "Gimpel3D_ConvertingASequence.html");return true;
		case hlp_CONVERSION_STEPS:strcpy(res, "Gimpel3D_ConversionSteps.html");return true;
		case hlp_CONVERSION_STEPS_IMPORTING_DATA:strcpy(res, "Gimpel3D_StepsImportingData.html");return true;
		case hlp_CONVERSION_STEPS_2D_ROTOSCOPING:strcpy(res, "Gimpel3D_Steps2DRotoscoping.html");return true;
		case hlp_CONVERSION_STEPS_3D_EDITING:strcpy(res, "Gimpel3D_Steps3DEditing.html");return true;
		case hlp_CONVERSION_STEPS_STEREO_SETTINGS:strcpy(res, "Gimpel3D_StepsStereoSettings.html");return true;
		case hlp_CONVERSION_STEPS_FILLING_GAPS:strcpy(res, "Gimpel3D_StepsFillingGaps.html");return true;
		case hlp_CONVERSION_STEPS_EXPORTING_DATA:strcpy(res, "Gimpel3D_StepsExportingData.html");return true;
		case hlp_REFERENCE:strcpy(res, "Gimpel3D_Reference.html");return true;
		case hlp_REFERENCE_MAIN_WORKSPACE:strcpy(res, "Gimpel3D_MainWorkspace.html");return true;
		case hlp_REFERENCE_APPLICATION_LAYOUT:strcpy(res, "Gimpel3D_ApplicationLayout.html");return true;
		case hlp_REFERENCE_MAIN_MENU:strcpy(res, "Gimpel3D_MainMenu.html");return true;
		case hlp_REFERENCE_MAIN_TOOLBAR:strcpy(res, "Gimpel3D_MainToolbar.html");return true;
		case hlp_REFERENCE_SIDE_BAR:strcpy(res, "Gimpel3D_SideBar.html");return true;
		case hlp_REFERENCE_TIMELINE_SLIDER:strcpy(res, "Gimpel3D_TimelineSlider.html");return true;
		case hlp_REFERENCE_EDIT_RENDER_VIEW:strcpy(res, "Gimpel3D_EditRenderView.html");return true;
		case hlp_REFERENCE_POPUP_MENU:strcpy(res, "Gimpel3D_PopupMenu.html");return true;
		case hlp_REFERENCE_KB_MOUSE_CONTROLS:strcpy(res, "Gimpel3D_KBMouseControls.html");return true;
		case hlp_REFERENCE_2D_EDITING:strcpy(res, "Gimpel3D_Ref2DEditing.html");return true;
		case hlp_REFERENCE_2D_VIEW_OPTIONS:strcpy(res, "Gimpel3D_Ref2DViewOptions.html");return true;
		case hlp_REFERENCE_ROTOSCOPING:strcpy(res, "Gimpel3D_Rotoscoping.html");return true;
		case hlp_REFERENCE_SPLIT_LAYER:strcpy(res, "Gimpel3D_SplitLayer.html");return true;
		case hlp_REFERENCE_SHIFT_EDGES:strcpy(res, "Gimpel3D_ShiftEdges.html");return true;
		case hlp_REFERENCE_TRACK_OUTLINES:strcpy(res, "Gimpel3D_TrackOutlines.html");return true;
		case hlp_REFERENCE_AUTO_SEGMENT:strcpy(res, "Gimpel3D_AutoSegment.html");return true;
		case hlp_REFERENCE_3D_EDITING:strcpy(res, "Gimpel3D_Ref3DEditing.html");return true;
		case hlp_REFERENCE_MISC_TOOLS:strcpy(res, "Gimpel3D_MiscTools.html");return true;
		case hlp_REFERENCE_FEATURE_POINTS:strcpy(res, "Gimpel3D_FeaturePoints.html");return true;
		case hlp_REFERENCE_LINK_POINTS:strcpy(res, "Gimpel3D_LinkPoints.html");return true;
		case hlp_REFERENCE_VIRTUAL_CAMERA:strcpy(res, "Gimpel3D_VirtualCamera.html");return true;
		case hlp_REFERENCE_PLUGIN_MANAGER:strcpy(res, "Gimpel3D_PluginManager.html");return true;
		case hlp_REFERENCE_TOUCHUPS:strcpy(res, "Gimpel3D_TouchUps.html");return true;
		case hlp_REFERENCE_ANCHOR_POINTS:strcpy(res, "Gimpel3D_AnchorPoints.html");return true;
		case hlp_REFERENCE_HALO_TOOL:strcpy(res, "Gimpel3D_HaloTool.html");return true;
		case hlp_REFERENCE_PROJECTION_SETTINGS:strcpy(res, "Gimpel3D_ProjectionSettings.html");return true;
		case hlp_REFERENCE_COLOR_SCHEME:strcpy(res, "Gimpel3D_ColorScheme.html");return true;
		case hlp_REFERENCE_3D_VIEW_OPTIONS:strcpy(res, "Gimpel3D_Ref3DViewOptions.html");return true;
		case hlp_REFERENCE_PERSPECTIVE_PROJECTION:strcpy(res, "Gimpel3D_PerspectiveProjection.html");return true;
		case hlp_REFERENCE_DEPTH:strcpy(res, "Gimpel3D_Depth.html");return true;
		case hlp_REFERENCE_SCALE:strcpy(res, "Gimpel3D_Scale.html");return true;
		case hlp_REFERENCE_ORIENTATION:strcpy(res, "Gimpel3D_Orientation.html");return true;
		case hlp_REFERENCE_AUTO_ALIGNMENT:strcpy(res, "Gimpel3D_AutoAlignment.html");return true;
		case hlp_REFERENCE_RELIEF_MAP:strcpy(res, "Gimpel3D_ReliefMap.html");return true;
		case hlp_REFERENCE_CONTOUR_EXTRUSION:strcpy(res, "Gimpel3D_ContourExtrusion.html");return true;
		case hlp_REFERENCE_PROJECTION_GEOMETRY:strcpy(res, "Gimpel3D_ProjectionGeometry.html");return true;
		case hlp_REFERENCE_FINISHING:strcpy(res, "Gimpel3D_Finishing.html");return true;
		case hlp_REFERENCE_STEREO_SETTINGS:strcpy(res, "Gimpel3D_StereoSettings.html");return true;
		case hlp_REFERENCE_FILLING_GAPS:strcpy(res, "Gimpel3D_FillingGaps.html");return true;
		case hlp_REFERENCE_IMPORTING_EXPORTING:strcpy(res, "Gimpel3D_ImportExport.html");return true;
		case hlp_REFERENCE_IMPORTING:strcpy(res, "Gimpel3D_Importing.html");return true;
		case hlp_REFERENCE_EXPORTING:strcpy(res, "Gimpel3D_Exporting.html");return true;
		case hlp_REFERENCE_EXAMPLES:strcpy(res, "Gimpel3D_Examples.html");return true;
		case hlp_REFERENCE_EXAMPLE_BASIC_SCENE_CONSTRUCTION:strcpy(res, "Gimpel3D_Example_BasicSceneConstruction.html");return true;
		case hlp_REFERENCE_EXAMPLE_VIRTUAL_CAMERA_PATH:strcpy(res, "Gimpel3D_Example_VirtualCamera.html");return true;
		case hlp_REFERENCE_EXAMPLE_ROTO_AUTOMATION:strcpy(res, "Gimpel3D_Example_RotoscopingAutomation.html");return true;
		case hlp_REFERENCE_EXAMPLE_RELIEF_TEXTURING:strcpy(res, "Gimpel3D_Example_ReliefTexturing.html");return true;
		case hlp_REFERENCE_EXAMPLE_HALO_EFFECTS:strcpy(res, "Gimpel3D_Example_HaloEffects.html");return true;
		case hlp_REFERENCE_EXAMPLE_GEOMETRY_PROJECTION:strcpy(res, "Gimpel3D_Example_GeometryProjection.html");return true;
		case hlp_REFERENCE_EXAMPLE_CONTOUR_EXTRUSION:strcpy(res, "Gimpel3D_Example_ContourExtrusion.html");return true;
		case hlp_REFERENCE_EXAMPLE_SHIFTING_EDGES:strcpy(res, "Gimpel3D_Example_ShiftingEdges.html");return true;
		case hlp_REFERENCE_EXAMPLE_ANCHOR_POINTS:strcpy(res, "Gimpel3D_Example_AnchorPoints.html");return true;
		case hlp_REFERENCE_EXAMPLE_TOUCH_UPS:strcpy(res, "Gimpel3D_Example_TouchUps.html");return true;
		case hlp_REFERENCE_EXAMPLE_FILLING_GAPS:strcpy(res, "Gimpel3D_Example_FillingGaps.html");return true;
		case hlp_REFERENCE_EXAMPLE_DEPTH_AND_LAYER_MASKS:strcpy(res, "Gimpel3D_Example_DepthAndLayerMasks.html");return true;
		case hlp_FUTURE_DEVELOPMENT:strcpy(res, "Gimpel3D_FutureDevelopment.html");return true;
		case hlp_CHARACTER_ANIMATION_SYSTEM:strcpy(res, "Gimpel3D_CharacterAnimationSystem.html");return true;
		case hlp_PLUGIN_SYSTEM:strcpy(res, "Gimpel3D_PluginSystem.html");return true;
		case hlp_CONVERSION_MANAGEMENT:strcpy(res, "Gimpel3D_ConversionManagement.html");return true;
		case hlp_AUTOMATED_CONVERSION:strcpy(res, "Gimpel3D_AutomatedConversion.html");return true;
	};
	return false;
}

HTREEITEM Add_Tree_Item(CTreeCtrl *tc, int help_id, char *text, HTREEITEM root)
{
	HTREEITEM ti = tc->InsertItem(text, root);
	tc->SetItemData(ti, help_id);
	HELP_PAGE *hp = &help_pages[help_id];
	Get_Help_File(help_id, hp->file);
	hp->id = help_id;
	strcpy(hp->title, text);
	hp->tree_item = ti;
	return ti;
}

bool Create_Help_TOC()
{
	CTreeCtrl *tc = &helpDlg->m_HelpToc;

	HTREEITEM overview = Add_Tree_Item(tc, hlp_OVERVIEW, "Overview", TVI_ROOT);
		Add_Tree_Item(tc, hlp_OVERVIEW_READ_THIS_FIRST, "Read This First", overview);
		Add_Tree_Item(tc, hlp_OVERVIEW_STEREO_CONVERSION, "Stereo Conversion", overview);


		HTREEITEM examples = Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLES, "Examples", TVI_ROOT);//ref);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_BASIC_SCENE_CONSTRUCTION, "Basic Scene Construction", examples);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_VIRTUAL_CAMERA_PATH, "Virtual Camera Path", examples);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_GEOMETRY_PROJECTION, "Geometry Projection", examples);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_RELIEF_TEXTURING, "Relief Texturing", examples);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_CONTOUR_EXTRUSION, "Contour Extrusion", examples);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_SHIFTING_EDGES, "Shifting Edges", examples);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_FILLING_GAPS, "Filling Gaps", examples);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_HALO_EFFECTS, "Halo Effects", examples);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_ANCHOR_POINTS, "Anchor Points", examples);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_TOUCH_UPS, "Manual Touch Ups", examples);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_DEPTH_AND_LAYER_MASKS, "Depth & Layer Masks", examples);
		Add_Tree_Item(tc, hlp_REFERENCE_EXAMPLE_ROTO_AUTOMATION, "Rotoscoping Automation", examples);

	HTREEITEM csteps = Add_Tree_Item(tc, hlp_CONVERSION_STEPS, "Conversion Steps", TVI_ROOT);
		Add_Tree_Item(tc, hlp_CONVERSION_STEPS_IMPORTING_DATA, "Importing Data", csteps);
		Add_Tree_Item(tc, hlp_CONVERSION_STEPS_2D_ROTOSCOPING, "2D Rotoscoping", csteps);
		Add_Tree_Item(tc, hlp_CONVERSION_STEPS_3D_EDITING, "3D Editing", csteps);
		Add_Tree_Item(tc, hlp_CONVERSION_STEPS_STEREO_SETTINGS, "Stereo Settings", csteps);
		Add_Tree_Item(tc, hlp_CONVERSION_STEPS_FILLING_GAPS, "Filling Gaps", csteps);
		Add_Tree_Item(tc, hlp_CONVERSION_STEPS_EXPORTING_DATA, "Exporting Data", csteps);

	HTREEITEM ref = Add_Tree_Item(tc, hlp_REFERENCE, "Reference", TVI_ROOT);

		
		HTREEITEM workspace = Add_Tree_Item(tc, hlp_REFERENCE_MAIN_WORKSPACE, "Main Workspace", ref);
		Add_Tree_Item(tc, hlp_REFERENCE_APPLICATION_LAYOUT, "Application Layout", workspace);
		Add_Tree_Item(tc, hlp_REFERENCE_MAIN_MENU, "Main Menu", workspace);
		Add_Tree_Item(tc, hlp_REFERENCE_MAIN_TOOLBAR, "Main Toolbar", workspace);
		Add_Tree_Item(tc, hlp_REFERENCE_SIDE_BAR, "Side Bar", workspace);
		Add_Tree_Item(tc, hlp_REFERENCE_TIMELINE_SLIDER, "Timeline Slider", workspace);
		Add_Tree_Item(tc, hlp_REFERENCE_EDIT_RENDER_VIEW, "Editing/Rendering View", workspace);
		Add_Tree_Item(tc, hlp_REFERENCE_POPUP_MENU, "Popup Menu", workspace);

		Add_Tree_Item(tc, hlp_REFERENCE_KB_MOUSE_CONTROLS, "KB/Mouse Controls", ref);

		HTREEITEM edit = Add_Tree_Item(tc, hlp_REFERENCE_2D_EDITING, "2D Editing", ref);
		Add_Tree_Item(tc, hlp_REFERENCE_2D_VIEW_OPTIONS, "2D View Options", edit);
		Add_Tree_Item(tc, hlp_REFERENCE_ROTOSCOPING, "Rotoscoping", edit);
		Add_Tree_Item(tc, hlp_REFERENCE_SPLIT_LAYER, "Split Layer", edit);
		Add_Tree_Item(tc, hlp_REFERENCE_SHIFT_EDGES, "Shift Edges", edit);
		Add_Tree_Item(tc, hlp_REFERENCE_TRACK_OUTLINES, "Track Outline", edit);
		Add_Tree_Item(tc, hlp_REFERENCE_AUTO_SEGMENT, "Auto-Segment", edit);

		HTREEITEM pms = Add_Tree_Item(tc, hlp_REFERENCE_3D_EDITING, "3D Editing", ref);
		Add_Tree_Item(tc, hlp_REFERENCE_3D_VIEW_OPTIONS, "3D View Options", pms);
		Add_Tree_Item(tc, hlp_REFERENCE_DEPTH, "Depth", pms);
		Add_Tree_Item(tc, hlp_REFERENCE_ORIENTATION, "Orientation", pms);
		Add_Tree_Item(tc, hlp_REFERENCE_AUTO_ALIGNMENT, "Auto-Alignment", pms);
		Add_Tree_Item(tc, hlp_REFERENCE_PERSPECTIVE_PROJECTION, "Perspective Projection", pms);
		Add_Tree_Item(tc, hlp_REFERENCE_CONTOUR_EXTRUSION, "Contour Extrusion", pms);
		Add_Tree_Item(tc, hlp_REFERENCE_RELIEF_MAP, "Relief Map", pms);
		Add_Tree_Item(tc, hlp_REFERENCE_PROJECTION_GEOMETRY, "Projection Geometry", pms);
		Add_Tree_Item(tc, hlp_REFERENCE_SCALE, "Scale", pms);

		HTREEITEM pmt = Add_Tree_Item(tc, hlp_REFERENCE_MISC_TOOLS, "Misc. Tools", ref);
		Add_Tree_Item(tc, hlp_REFERENCE_FEATURE_POINTS, "2D Feature Points", pmt);
		Add_Tree_Item(tc, hlp_REFERENCE_LINK_POINTS, "Link Points", pmt);
		Add_Tree_Item(tc, hlp_REFERENCE_VIRTUAL_CAMERA, "Virtual Camera", pmt);
		Add_Tree_Item(tc, hlp_REFERENCE_PLUGIN_MANAGER, "Plugin Manager", pmt);
		Add_Tree_Item(tc, hlp_REFERENCE_TOUCHUPS, "Manual Touch Ups", pmt);
		Add_Tree_Item(tc, hlp_REFERENCE_ANCHOR_POINTS, "Anchor Points", pmt);
		Add_Tree_Item(tc, hlp_REFERENCE_PROJECTION_SETTINGS, "Projection Settings", pmt);
		Add_Tree_Item(tc, hlp_REFERENCE_COLOR_SCHEME, "Color Scheme", pmt);

		HTREEITEM fin = Add_Tree_Item(tc, hlp_REFERENCE_FINISHING, "Post-Processing/Finishing", ref);
		Add_Tree_Item(tc, hlp_REFERENCE_STEREO_SETTINGS, "Stereo Settings", fin);
		Add_Tree_Item(tc, hlp_REFERENCE_HALO_TOOL, "Halo Tool", fin);
		Add_Tree_Item(tc, hlp_REFERENCE_FILLING_GAPS, "Filling Gaps", fin);

		HTREEITEM impexp = Add_Tree_Item(tc, hlp_REFERENCE_IMPORTING_EXPORTING, "Importing/Exporting", ref);
		Add_Tree_Item(tc, hlp_REFERENCE_IMPORTING, "Importing", impexp);
		Add_Tree_Item(tc, hlp_REFERENCE_EXPORTING, "Exporting", impexp);


		HTREEITEM fudev = Add_Tree_Item(tc, hlp_FUTURE_DEVELOPMENT, "Future Development", TVI_ROOT);

		Add_Tree_Item(tc, hlp_CHARACTER_ANIMATION_SYSTEM, "Character Animation System", fudev);
		Add_Tree_Item(tc, hlp_PLUGIN_SYSTEM, "Gimpel3D Plugin System", fudev);
		Add_Tree_Item(tc, hlp_AUTOMATED_CONVERSION, "Automated Conversion", fudev);
		Add_Tree_Item(tc, hlp_CONVERSION_MANAGEMENT, "Conversion Management", fudev);

	return true;
}

bool Get_Help_Path()
{
	GetModuleFileName(GetModuleHandle(NULL), help_path, 512);
	char *c = strrchr(help_path, '\\');
	if(!c)c = help_path;
	strcpy(c, "\\Docs\\");
	return true;
}

bool Open_Help_Dlg()
{
	if(!helpDlg)
	{
		helpDlg = new HelpDlg;
		helpDlg->Create(IDD_HELP_DLG);
		Create_Help_TOC();
		Get_Help_Path();
		helpDlg->SetHelpView(help_pages[0].tree_item);
	}
	helpDlg->ShowWindow(SW_SHOW);
	return true;
}

void HelpDlg::OnClose() 
{
	helpDlg->ShowWindow(SW_HIDE);
}

void HelpDlg::ResizeLayout() 
{
	CRect rect;
	GetClientRect(rect);
	int side_border = 10;
	int top_area_height = 50;
	int toc_width = 180;
	int inner_border = 8;
	int html_width = ((rect.Width()-(side_border*2))-toc_width)-inner_border;
	if(!m_HtmlWindow.m_hWnd||!m_HelpToc.m_hWnd)
	{
		MessageBox("There was an error creating the help dialog.", "No help for you.", MB_OK);
	}
	else
	{
		m_HtmlWindow.SetWindowPos(0, side_border+toc_width+inner_border, top_area_height, html_width, (rect.Height()-top_area_height)-side_border, 0);
		m_HelpToc.SetWindowPos(0, side_border, top_area_height, toc_width, (rect.Height()-top_area_height)-side_border, 0);
	}
}

void HelpDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	ResizeLayout() ;
}

bool use_clicked_url = false;
char clicked_url[512];

void HelpDlg::SetHelpView(HTREEITEM ti)
{
	int help_id = m_HelpToc.GetItemData(ti);
	char file[512];
	if(help_id!=current_help_id)
	{
		if(Get_Help_File(help_id, file))
		{
			char rfile[512];
			sprintf(rfile, "%s%s", help_path, file);
			if(use_clicked_url)
			{
				use_clicked_url = false;
				m_HtmlWindow.Navigate(clicked_url, 0,0,0,0);
			}
			else
			{
				m_HtmlWindow.Navigate(rfile, 0,0,0,0);
			}
			current_help_id = help_id;
			//FUCK IT MANUALLY CONTROL HIGHLIGHTED STATE FOR TREE ITEMS
			//SEE BELOW
			m_HelpToc.SelectDropTarget(ti);
		}
	}
}

void HelpDlg::OnSelchangedHelpToc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	static HTREEITEM last_hit = 0;
	HTREEITEM ti = m_HelpToc.GetSelectedItem();
	if(ti)
	{
		//if(ti!=last_hit)
		{
			SetHelpView(ti);
			last_hit = ti;
		}
	}
	*pResult = 0;
}

BEGIN_EVENTSINK_MAP(HelpDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(HelpDlg)
	ON_EVENT(HelpDlg, IDC_HTML_WINDOW, 113 /* TitleChange */, OnTitleChangeHtmlWindow, VTS_BSTR)
	ON_EVENT(HelpDlg, IDC_HTML_WINDOW, 105 /* CommandStateChange */, OnCommandStateChangeHtmlWindow, VTS_I4 VTS_BOOL)
	ON_EVENT(HelpDlg, IDC_HTML_WINDOW, 250 /* BeforeNavigate2 */, OnBeforeNavigate2HtmlWindow, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void HelpDlg::OnTitleChangeHtmlWindow(LPCTSTR Text) 
{
	CString url = m_HtmlWindow.GetLocationURL();

	char text[512];
	char ntext[512];

	strcpy(text, url.operator LPCTSTR());
	char *c = strrchr(text, '/');
	if(!c)
	{
		//invalid url
		return;
	}
	strcpy(ntext, &c[1]);

	//erase any tags past the "html" extension
	c = strrchr(ntext, '.');
	if(!c)
	{
		//invalid file
		return;
	}
	c[5] = 0;

	
	HELP_PAGE *hp = Find_Help_Page(ntext);
	if(!hp)
	{
	}
	else
	{
		if(hp->id!=current_help_id)//if not the current expected page
		{//it was clicked as a link, need to update tree selection
			use_clicked_url = true;
			strcpy(clicked_url, (char*)url.operator LPCTSTR());
			if(!m_HelpToc.SelectItem(hp->tree_item))
			{
			}
			else
			{
				//item is technically "selected" but not highlighted, spend time browsing msdn docs
				//to figure out the magic combination of "stuff" required to highlight the selected item
				//sort of like you would expect with just selecting it

				//gee maybe this similarly named function will do any different
		//m_HelpToc.Select(hp->tree_item, TVGN_CARET|TVGN_DROPHILITE|TVGN_FIRSTVISIBLE);
				//with all possible flags even though we just
				//want it to visually indicate that it is a selected item

				//NOPE it is still not highlighted how much time can we lose today
				//trying to programmatically select a simple item in the tree and have
				//it highlighted like if you clicked on it

				//started simple "how to select the item" search about 1:30, no reference to special
				//behavior on msdn, time to randomly "browse"

				//DUH this might work, "redraws" the items as a target of a drag and drop operation
				//I hope at least THAT's highlighted, even though I just want the "selected" look
				
		//m_HelpToc.SelectDropTarget(hp->tree_item);

				//WOW it actually highlights the goddamn item like it was actually selected..

				//AND KEEPS IT THAT WAY no matter what else is clicked or selected..

				//OK, now I need to find (a) the "correct" way, or (b) hack around this and (get this)
				//MANUALLY control which items are highlighted at every turn because I can't rely on
				//consistency between "selecting" an item by clicking on it (which highlights
				//the item), or "selecting" the item in the code, which does not highlight or visually
				//indicate the selected item at all..

				//gee I just wanted to "select" it like it was clicked on..

				//no new info, went back over the old, it looks like these flags CANNOT be combined
				//TVGN_CARET|TVGN_DROPHILITE|TVGN_FIRSTVISIBLE
				//docs say it can be ONE of the following

		//m_HelpToc.Select(hp->tree_item, TVGN_DROPHILITE);

				//well that was not helpful at all, it just does the same thing as SelectDroptarget

				//HJMFC, is there no way to programmatically "select" an item and make it look selected?

				//this should do it, but then again, so should SelectItem
		//m_HelpToc.Select(hp->tree_item, TVGN_CARET);
				//NOPE

			
				//OK FUCK THIS, I will manually set the highlight state for each and every single tree item
				//in this function
				//void HelpDlg::SetHelpView(HTREEITEM ti)

				//Apparently How It Works:
				//since it "magically happens" whenever the user clicks a selection, and does not happen
				//when the item is selected in the code, it has to be manually set..

				//after it gets manually set, the "auto-magic" highlight STOPS working when the user clicks
				//an item, so it then has to be manually set for THAT too..

				//and it only took me about 30 minutes to select a simple tree item, and there are TWO apparently
				//straightforward functions (Select and SelectItem) that should do what I needed
				//and (the worst part) this would all be perfectly fine if this was just referenced somewhere
				//on msdn vs. leaving me to figure out the quirky order of operations on my own

				//in the docs for the select functions just simply fucking state that the highlight STOPS
				//WORKING when you do this in the code AND WILL NOT WORK CORRECTLY with subsequent mouse
				//clicks

				//that could have saved me about 29 minutes and 45 seconds.

				//where I verified earlier
				//SetWindowText("Selected item ok, you must be blind, it is clearly selected and highlighted");
				//if(hp->tree_item==m_HelpToc.GetSelectedItem())
				//{
				//	//MessageBox("It is selected what else do you expect");
				//}

				//now I can continue doing actual productive work but need a break to regain my enthusiasm
			}
		}
	}
}

void HelpDlg::OnClickHelpToc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	static HTREEITEM last_hit = 0;
	HTREEITEM ti = m_HelpToc.GetSelectedItem();
	if(ti)
	{
		//if(ti!=last_hit)
		{
			SetHelpView(ti);
			last_hit = ti;
		}
	}
	*pResult = 0;
}

void HelpDlg::OnBack() 
{
	m_HtmlWindow.GoBack();
}

void HelpDlg::OnForward() 
{
	m_HtmlWindow.GoForward();
}

void HelpDlg::OnCommandStateChangeHtmlWindow(long Command, BOOL Enable) 
{
	if(Command==CSC_NAVIGATEBACK)
	{
		if(Enable==VARIANT_FALSE)
		{
			GetDlgItem(IDC_BACK)->EnableWindow(false);
		}
		else
		{
			GetDlgItem(IDC_BACK)->EnableWindow(true);
		}
	}
	if(Command==CSC_NAVIGATEFORWARD)
	{
		if(Enable==VARIANT_FALSE)
		{
			GetDlgItem(IDC_FORWARD)->EnableWindow(false);
		}
		else
		{
			GetDlgItem(IDC_FORWARD)->EnableWindow(true);
		}
	}
}

void HelpDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void HelpDlg::OnBeforeNavigate2HtmlWindow(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) 
{
}

HBRUSH HelpDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

//add this so the tree control expands when you click the "+"
BOOL HelpDlg::PreTranslateMessage(MSG* pMsg)
{
	return false;
}
//otherwise it won't work that way if the main app calls pretranslate message