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
// ToolBarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "ToolBarDlg.h"
#include "G3DMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ToolBarDlg dialog

ToolBarDlg *toolBarDlg = 0;

CBitmap *TB_anaglyph_bmp = 0;
CBitmap *TB_fullscreen_bmp = 0;
CBitmap *TB_grid_bmp = 0;
CBitmap *TB_camera_bmp = 0;

extern int toolbar_height;

extern bool anaglyph_mode;

extern bool fullscreen_app;
extern bool render_camera;


bool Update_Toolbar_View_Buttons()
{
	toolBarDlg->m_GridButton.render_pushed = render_grid;
	toolBarDlg->m_Fullscreen3D.render_pushed = fullscreen_3d;
	toolBarDlg->m_SplitScreen.render_pushed = split_screen;
	toolBarDlg->m_Fullscreen2D.render_pushed = fullscreen_2d;
	toolBarDlg->m_FullscreenApp.render_pushed = fullscreen_app;
	toolBarDlg->m_WidescreenButton.render_pushed = wide_screen;

	if(fullscreen_app)
	{
		toolBarDlg->m_FullscreenApp.SetBitmapInfo(32, 0, 2, 2);
	}
	else
	{
		toolBarDlg->m_FullscreenApp.SetBitmapInfo(0, 0, 2, 2);
	}

	toolBarDlg->m_GridButton.Invalidate();
	toolBarDlg->m_Fullscreen3D.Invalidate();
	toolBarDlg->m_SplitScreen.Invalidate();
	toolBarDlg->m_Fullscreen2D.Invalidate();
	toolBarDlg->m_FullscreenApp.Invalidate();
	toolBarDlg->m_WidescreenButton.Invalidate();
	return true;
}

bool Update_Toolbar_Anaglyph_Button()
{
	toolBarDlg->m_Anaglyph.render_pushed = anaglyph_mode;
	toolBarDlg->m_Anaglyph.Invalidate();
	return true;
}

bool Update_Toolbar_Grid_Button()
{
	toolBarDlg->m_GridButton.render_pushed = render_grid;
	toolBarDlg->m_GridButton.Invalidate();
	return true;
}

bool Update_Toolbar_Camera_Button()
{
	toolBarDlg->m_CameraButton.render_pushed = render_camera;
	toolBarDlg->m_CameraButton.Invalidate();
	return true;
}


ToolBarDlg::ToolBarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ToolBarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ToolBarDlg)
	//}}AFX_DATA_INIT
}

void ToolBarDlg::SetButtonLayout(int width)
{
	int button_width = 36;
	int button_height = toolbar_height-2;
	int xpos = 0;
	m_New.SetWindowPos(0, xpos, 0, button_width, button_height, 0);xpos+=button_width;
	m_Load.SetWindowPos(0, xpos, 0, button_width, button_height, 0);xpos+=button_width;
	m_Save.SetWindowPos(0, xpos, 0, button_width, button_height, 0);xpos+=button_width;
	m_Import.SetWindowPos(0, xpos, 0, button_width, button_height, 0);xpos+=button_width;
	m_Export.SetWindowPos(0, xpos, 0, button_width, button_height, 0);xpos+=button_width;
	
	xpos+=button_width;
	m_Fullscreen3D.SetWindowPos(0, xpos, 0, button_width, button_height, 0);xpos+=button_width;
	m_SplitScreen.SetWindowPos(0, xpos, 0, button_width, button_height, 0);xpos+=button_width;
	m_Fullscreen2D.SetWindowPos(0, xpos, 0, button_width, button_height, 0);xpos+=button_width;

	xpos+=button_width;
	m_WidescreenButton.SetWindowPos(0, xpos, button_height/4, button_width, button_height/2, 0);
	xpos+=button_width;
	m_Anaglyph.SetWindowPos(0, xpos, 0, button_width, button_height, 0);xpos+=button_width;
	m_GridButton.SetWindowPos(0, xpos, 0, button_width, button_height, 0);xpos+=button_width;
	m_CameraButton.SetWindowPos(0, xpos, 0, button_width, button_height, 0);xpos+=button_width;


	m_FullscreenApp.SetWindowPos(0, width-button_width, 0, button_width, button_height, 0);
}


void ToolBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ToolBarDlg)
	DDX_Control(pDX, IDC_WIDE_SCREEN_3D, m_WidescreenButton);
	DDX_Control(pDX, IDC_CAMERA_BUTTON, m_CameraButton);
	DDX_Control(pDX, IDC_GRID_BUTTON, m_GridButton);
	DDX_Control(pDX, IDC_FULL_SCREEN_APP, m_FullscreenApp);
	DDX_Control(pDX, IDC_ANAGLYPH, m_Anaglyph);
	DDX_Control(pDX, IDC_SPLITSCREEN, m_SplitScreen);
	DDX_Control(pDX, IDC_FULLSCREEN_2D, m_Fullscreen2D);
	DDX_Control(pDX, IDC_FULLSCREEN_3D, m_Fullscreen3D);
	DDX_Control(pDX, IDC_SAVE, m_Save);
	DDX_Control(pDX, IDC_NEW, m_New);
	DDX_Control(pDX, IDC_LOAD, m_Load);
	DDX_Control(pDX, IDC_IMPORT_DATA, m_Import);
	DDX_Control(pDX, IDC_EXPORT_DATA, m_Export);
	//}}AFX_DATA_MAP

	Update_Toolbar_View_Buttons();
	Update_Toolbar_Anaglyph_Button();
	
	m_FullscreenApp.SetFont(skin_font);
	m_Fullscreen3D.SetFont(skin_font);
	m_SplitScreen.SetFont(skin_font);
	m_Fullscreen2D.SetFont(skin_font);
	m_New.SetFont(skin_font);
	m_Load.SetFont(skin_font);
	m_Save.SetFont(skin_font);
	m_Import.SetFont(skin_font);
	m_Export.SetFont(skin_font);
	m_WidescreenButton.SetFont(skin_font);

	TB_anaglyph_bmp = new CBitmap;
	TB_anaglyph_bmp->LoadBitmap(IDB_ANAGLYPH_BUTTON);
	m_Anaglyph.SetBitmap(TB_anaglyph_bmp, 32, 24, 0, 0, 2, 2);

	TB_fullscreen_bmp = new CBitmap;
	TB_fullscreen_bmp->LoadBitmap(IDB_FULLSCREEN_BUTTON);
	m_FullscreenApp.SetBitmap(TB_fullscreen_bmp, 32, 24, 0, 0, 2, 2);

	TB_grid_bmp = new CBitmap;
	TB_grid_bmp->LoadBitmap(IDB_GRID_BUTTON);
	m_GridButton.SetBitmap(TB_grid_bmp, 32, 24, 0, 0, 2, 2);

	TB_camera_bmp = new CBitmap;
	TB_camera_bmp->LoadBitmap(IDB_CAMERA_BUTTON);
	m_CameraButton.SetBitmap(TB_camera_bmp, 32, 24, 0, 0, 2, 2);

	m_New.Autoload_Tooltip();
	m_Load.Autoload_Tooltip();
	m_Save.Autoload_Tooltip();
	m_Import.Autoload_Tooltip();
	m_Export.Autoload_Tooltip();
	m_Fullscreen3D.Autoload_Tooltip();
	m_SplitScreen.Autoload_Tooltip();
	m_Fullscreen2D.Autoload_Tooltip();
	m_Anaglyph.Autoload_Tooltip();
	m_GridButton.Autoload_Tooltip();
	m_CameraButton.Autoload_Tooltip();
	m_FullscreenApp.Autoload_Tooltip();
	m_WidescreenButton.Autoload_Tooltip();
}


BEGIN_MESSAGE_MAP(ToolBarDlg, CDialog)
	//{{AFX_MSG_MAP(ToolBarDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_IMPORT_DATA, OnImportData)
	ON_BN_CLICKED(IDC_EXPORT_DATA, OnExportData)
	ON_BN_CLICKED(IDC_FULLSCREEN_3D, OnFullscreen3d)
	ON_BN_CLICKED(IDC_SPLITSCREEN, OnSplitscreen)
	ON_BN_CLICKED(IDC_FULLSCREEN_2D, OnFullscreen2d)
	ON_BN_CLICKED(IDC_ANAGLYPH, OnAnaglyph)
	ON_BN_CLICKED(IDC_FULL_SCREEN_APP, OnFullScreenApp)
	ON_BN_CLICKED(IDC_GRID_BUTTON, OnGridButton)
	ON_BN_CLICKED(IDC_CAMERA_BUTTON, OnCameraButton)
	ON_BN_CLICKED(IDC_WIDE_SCREEN_3D, OnWideScreen3d)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ToolBarDlg message handlers

bool Resize_Toolbar_Dlg()
{
	CRect rect;
	mainwindow->GetClientRect(&rect);
	int width = rect.Width();
	int height = toolbar_height;
	toolBarDlg->SetWindowPos(0, 0, 0, width, height, 0);
	toolBarDlg->SetButtonLayout(width);
	return true;
}

bool Init_Toolbar_Dlg()
{
	toolBarDlg = new ToolBarDlg;
	toolBarDlg->Create(IDD_TOOLBAR_DLG);
	toolBarDlg->SetParent(mainwindow);
	toolBarDlg->ShowWindow(SW_SHOW);
	Resize_Toolbar_Dlg();
	return true;
}

bool Free_Toolbar_Dlg()
{
	if(toolBarDlg)delete toolBarDlg;
	if(TB_anaglyph_bmp)delete TB_anaglyph_bmp;
	if(TB_fullscreen_bmp)delete TB_fullscreen_bmp;
	if(TB_grid_bmp)delete TB_grid_bmp;
	if(TB_camera_bmp)delete TB_camera_bmp;
	toolBarDlg = 0;
	TB_anaglyph_bmp = 0;
	TB_fullscreen_bmp = 0;
	TB_grid_bmp = 0;
	TB_camera_bmp = 0;
	return true;
}


HBRUSH ToolBarDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return dark_skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void ToolBarDlg::OnNew() 
{
	mainframe->OnNewProject();
}

void ToolBarDlg::OnLoad() 
{
	mainframe->OnLoadProject();
}

void ToolBarDlg::OnSave() 
{
	mainframe->OnSaveProject();
}

void ToolBarDlg::OnImportData() 
{
	mainframe->SetFocus();
	mainframe->OnImport();
}

void ToolBarDlg::OnExportData() 
{
	mainframe->SetFocus();
	mainframe->OnExport();
}

void ToolBarDlg::OnFullscreen3d() 
{
	mainframe->SetFocus();
	mainframe->OnFullScreen3d();
}

void ToolBarDlg::OnSplitscreen() 
{
	mainframe->SetFocus();
	mainframe->OnSplitScreen();
}

void ToolBarDlg::OnFullscreen2d() 
{
	mainframe->SetFocus();
	mainframe->OnFullScreen2d();
}

void ToolBarDlg::OnAnaglyph() 
{
	mainframe->SetFocus();
	mainframe->OnAnaglyph();
}

void ToolBarDlg::OnFullScreenApp() 
{
	mainframe->SetFocus();
	mainframe->OnFullScreenApp();
}

void ToolBarDlg::OnGridButton() 
{
	mainframe->SetFocus();
	mainframe->OnGridButton();
}

void ToolBarDlg::OnCameraButton() 
{
	mainframe->SetFocus();
	mainframe->OnCameraButton();
}

void ToolBarDlg::OnWideScreen3d() 
{
	mainframe->SetFocus();
	mainframe->OnWideScreen3d();
}
