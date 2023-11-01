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
// Gimpel3D.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "Gimpel3DDlg.h"
#include "InfoBarDlg.h"
#include "LayersDlg.h"
#include "FramesDlg.h"
#include "ToolTrayDlg.h"
#include "GapFillTool.h"
#include "Skin.h"
#include "Plugins.h"
#include <gl\gl.h>

bool Demo_Intro();


int random_seed = -1;
bool Free_Brush_Grid_Buffers();
bool Clear_All_TouchUp_Data();

bool Valid_Path(char *path);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGimpel3DApp

BEGIN_MESSAGE_MAP(CGimpel3DApp, CWinApp)
	//{{AFX_MSG_MAP(CGimpel3DApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGimpel3DApp construction

CGimpel3DApp::CGimpel3DApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGimpel3DApp object

CGimpel3DApp theApp;
CWinApp *mainApp = &theApp;

/////////////////////////////////////////////////////////////////////////////
// CGimpel3DApp initialization

bool Init_MainFrame();
bool Free_MainFrame();

bool Free_Skin_Frames();

bool Init_Splash_Dialog();
bool Free_Splash_Dialog();
bool Wait_For_Splash_Dialog();

bool Init_Closing_Splash_Dialog();
bool Free_Closing_Splash_Dialog();
bool Wait_For_Closing_Splash_Dialog();

bool Test_Disparity_Map(char *file1, char *file2, char *output);

bool Load_ToolTray(char *file);
bool Save_ToolTray(char *file);
bool Save_Color_Scheme(char *file);
bool Load_Color_Scheme(char *file);

bool Load_Color_Scheme()
{
	char file[512];
	GetModuleFileName(GetModuleHandle(NULL), file, 512);
	char *c = strrchr(file, '\\');
	if(!c)c = file;
	strcpy(c, "\\Colors.dat");Load_Color_Scheme(file);
	return true;
}

bool Load_Global_Settings()
{
	char file[512];
	GetModuleFileName(GetModuleHandle(NULL), file, 512);
	char *c = strrchr(file, '\\');
	if(!c)c = file;
	strcpy(c, "\\ToolTray.txt");Load_ToolTray(file);
	return true;
}

bool Save_Global_Settings()
{
	char file[512];
	GetModuleFileName(GetModuleHandle(NULL), file, 512);
	char *c = strrchr(file, '\\');
	if(!c)c = file;
	strcpy(c, "\\ToolTray.txt");Save_ToolTray(file);
	strcpy(c, "\\Colors.dat");Save_Color_Scheme(file);
	return true;
}

bool Load_Plugins()
{
	char path[512];
	GetModuleFileName(GetModuleHandle(NULL), path, 512);
	char *c = strrchr(path, '\\');
	if(!c)c = path;
	strcpy(c, "\\plugins\\");
	if(Valid_Path(path))
	{
		Load_Plugins(path);
	}
	return true;
}

bool Open_Help_Dlg();

bool RunApp()
{
	InfoBarDlg ibdlg;
	LayersDlg ldlg;
	FramesDlg fdlg;
	ToolTrayDlg ttdlg;
	
	Init_MainFrame();
	mainwindow->ShowWindow(SW_HIDE);
	int width, height;
	Get_Desktop_Dimensions(&width, &height);
	mainwindow->SetWindowPos(0, 0, 0, width, height, 0);

	Init_G3D_Core();

	SetParent(Get_3D_Window(), mainwindow->GetSafeHwnd());
	SetParent(Get_2D_Window(), mainwindow->GetSafeHwnd());

	//safe to show window now
	Show_2D_View(true);

	SetParent(Get_GapFill_Window(), mainwindow->GetSafeHwnd());

	Init_Timeline_Slider();

	ibdlg.Create(IDD_INFO_BAR_DLG, 0);
	ldlg.Create(IDD_LAYERS_DLG, 0);
	fdlg.Create(IDD_FRAMES_DLG, 0);
	ttdlg.Create(IDD_TOOLTRAY_DLG, 0);
	
	ibdlg.ShowWindow(SW_SHOW);
	ldlg.ShowWindow(SW_SHOW);
	fdlg.ShowWindow(SW_SHOW);
	ttdlg.ShowWindow(SW_SHOW);

	Resize_Layout();
	mainwindow->ShowWindow(SW_MAXIMIZE);
	mainwindow->ShowWindow(SW_SHOW);

	Load_Global_Settings();

	Register_Core_Interfaces();//for plugins

	Load_Plugins();

	mainwindow->SetFocus();

	glGetError();//clear any error flags from init (sigh, there is one, fix for long term)

	while(app_running)
	{
		Update_G3D_Core();
	}

	Save_Global_Settings();

	Unload_Plugins();
	Free_Interfaces();

	Free_G3D_Core();
	Free_Timeline_Slider();
	Free_MainFrame();
	Free_Skin_Frames();
	Free_Skin();
	Free_Brush_Grid_Buffers();
	Clear_All_TouchUp_Data();
	return true;
}

BOOL CGimpel3DApp::InitInstance()
{
	srand((unsigned int)time(0));
	random_seed =  rand()%999999;
	AfxEnableControlContainer();

	SetRegistryKey(_T("DWNS"));
	app_running = true;


#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	Load_Color_Scheme();
	Init_Skin();

	RunApp();

	return FALSE;
}

