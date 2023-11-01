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
// ImportOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "ImportOptionsDlg.h"
#include "ImportListDlg.h"

bool Create_Default_Virtual_Camera_Path();

bool AutoSave_Project();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ImportOptionsDlg dialog
bool Valid_Path(char *path);


ImportOptionsDlg::ImportOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ImportOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ImportOptionsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ImportOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ImportOptionsDlg)
	DDX_Control(pDX, IDC_IMPORT_LAYERMASKS, m_ImportLayermasks);
	DDX_Control(pDX, IDC_IMPORT_IMAGES, m_ImportImages);
	DDX_Control(pDX, IDC_IMPORT_DEPTHMASKS, m_ImportDepthMasks);
	DDX_Control(pDX, IDC_BROWSE_LAYERMASKS_PATH, m_BrowseLayermasks);
	DDX_Control(pDX, IDC_BROWSE_IMAGES_PATH, m_BrowseImages);
	DDX_Control(pDX, IDC_BROWSE_DEPTHMASKS_PATH, m_BrowseDepthmasks);
	//}}AFX_DATA_MAP
	UpdatePaths();
	CheckDlgButton(IDC_APPLY_IMAGE_FILTER, prefilter_layermasks);

	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_IMAGES_PATH)->SetFont(skin_font);
	GetDlgItem(IDC_BROWSE_IMAGES_PATH)->SetFont(skin_font);
	GetDlgItem(IDC_IMPORT_IMAGES)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_LAYERMASKS_PATH)->SetFont(skin_font);
	GetDlgItem(IDC_BROWSE_LAYERMASKS_PATH)->SetFont(skin_font);
	GetDlgItem(IDC_IMPORT_LAYERMASKS)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC3)->SetFont(skin_font);
	GetDlgItem(IDC_DEPTHMASKS_PATH)->SetFont(skin_font);
	GetDlgItem(IDC_BROWSE_DEPTHMASKS_PATH)->SetFont(skin_font);
	GetDlgItem(IDC_IMPORT_DEPTHMASKS)->SetFont(skin_font);
	GetDlgItem(IDC_APPLY_IMAGE_FILTER)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(ImportOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(ImportOptionsDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_IMPORT_IMAGES, OnImportImages)
	ON_BN_CLICKED(IDC_BROWSE_IMAGES_PATH, OnBrowseImagesPath)
	ON_BN_CLICKED(IDC_IMPORT_LAYERMASKS, OnImportLayermasks)
	ON_BN_CLICKED(IDC_BROWSE_LAYERMASKS_PATH, OnBrowseLayermasksPath)
	ON_BN_CLICKED(IDC_APPLY_IMAGE_FILTER, OnApplyImageFilter)
	ON_BN_CLICKED(IDC_IMPORT_DEPTHMASKS, OnImportDepthmasks)
	ON_BN_CLICKED(IDC_BROWSE_DEPTHMASKS_PATH, OnBrowseDepthmasksPath)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ImportOptionsDlg message handlers

void ImportOptionsDlg::OnImportImages() 
{
	vector<char*> files;
	SetWindowText("Searching for files..");
	GetParent()->Invalidate();
	char path[512];
	GetDlgItemText(IDC_IMAGES_PATH, path, 512);

	if(!Valid_Path(path))
	{
		char msg[512];
		sprintf(msg, "The directory \"%s\" does not appear to be a valid path.", path);
		SkinMsgBox(msg);
		return;
	}

	Find_Image_Files_In_Path(path, &files);
	
	ImportListDlg dlg;
	dlg.files = &files;
	dlg.InitListInfo();
	dlg.DoModal();
	if(dlg.num_included_files==0)
	{
		SkinMsgBox("No files selected for import.");
		return;
	}

	SetWindowText("Import Options");
	GetParent()->Invalidate();
	int n = files.size();
	if(n==0)
	{
		SkinMsgBox("No files found in the specified path!", path);
		return;
	}
	char temp[512];
	int i;
	for(i = 0;i<n;i++)
	{
		sprintf(temp, "%s\\%s", path, files[i]);
		strcpy(files[i], temp);
	}
	Print_Status("Importing files..");
	Import_Project_Images(&files);
	for(i = 0;i<n;i++)
	{
		delete[] files[i];
	}
	files.clear();
	List_Frames();//Update_Shot_Sequence_List();
	//wide_screen = true;
	//Fit_3D_Window();
	Fit_3D_Window();
	//we have a new sequence, create a default virual camera path
	Create_Default_Virtual_Camera_Path();
	AutoSave_Project();
	SkinMsgBox("Image import completed");
}

void ImportOptionsDlg::OnBrowseImagesPath() 
{
	char path[512];
	if(BrowseForFolder(path, "Select the folder containing image files to import"))
	{
		SetDlgItemText(IDC_IMAGES_PATH, path);
	}
}

void ImportOptionsDlg::OnImportLayermasks() 
{
	char path[512];
	GetDlgItemText(IDC_LAYERMASKS_PATH, path, 512);
	if(!Valid_Path(path))
	{
		char msg[512];
		sprintf(msg, "The directory \"%s\" does not appear to be a valid path.", path);
		SkinMsgBox(msg);
		return;
	}
	if(prefilter_layermasks)
	{
		PreFilter_Path(path);
		SetDlgItemText(IDC_LAYERMASKS_PATH, path);
	}
	vector<char*> files;
	SetWindowText("Searching for files..");
	GetParent()->Invalidate();
	Find_Image_Files_In_Path(path, &files);

	ImportListDlg dlg;
	dlg.files = &files;
	dlg.InitListInfo();
	dlg.DoModal();
	if(dlg.num_included_files==0)
	{
		SkinMsgBox("No files selected for import.");
		return;
	}
	
	SetWindowText("Import Options");
	GetParent()->Invalidate();
	int n = files.size();
	if(n==0)
	{
		SkinMsgBox("No files found in the specified path!", path);
		return;
	}
	char temp[512];
	int i;
	for(i = 0;i<n;i++)
	{
		sprintf(temp, "%s\\%s", path, files[i]);
		strcpy(files[i], temp);
	}
	Print_Status("Importing files..");
	Import_Project_Layermasks(&files);
	for(i = 0;i<n;i++)
	{
		delete[] files[i];
	}
	files.clear();
	AutoSave_Project();
	SkinMsgBox("Layermask import completed");
}

void ImportOptionsDlg::OnBrowseLayermasksPath() 
{
	char path[512];
	if(BrowseForFolder(path, "Select the folder containing layermask files to import"))
	{
		SetDlgItemText(IDC_LAYERMASKS_PATH, path);
	}
}

void ImportOptionsDlg::OnApplyImageFilter() 
{
	prefilter_layermasks = IsDlgButtonChecked(IDC_APPLY_IMAGE_FILTER)!=0;
}

void ImportOptionsDlg::OnImportDepthmasks() 
{
	vector<char*> files;
	SetWindowText("Searching for files..");
	GetParent()->Invalidate();
	char path[512];
	GetDlgItemText(IDC_DEPTHMASKS_PATH, path, 512);
	if(!Valid_Path(path))
	{
		char msg[512];
		sprintf(msg, "The directory \"%s\" does not appear to be a valid path.", path);
		SkinMsgBox(msg);
		return;
	}
	Find_Image_Files_In_Path(path, &files);

	ImportListDlg dlg;
	dlg.files = &files;
	dlg.InitListInfo();
	dlg.DoModal();
	if(dlg.num_included_files==0)
	{
		SkinMsgBox("No files selected for import.");
		return;
	}

	SetWindowText("Import Options");
	GetParent()->Invalidate();
	int n = files.size();
	if(n==0)
	{
		SkinMsgBox("No files found in the specified path!", path);
		return;
	}
	char temp[512];
	int i;
	for(i = 0;i<n;i++)
	{
		sprintf(temp, "%s\\%s", path, files[i]);
		strcpy(files[i], temp);
	}
	Print_Status("Importing files..");
	Import_Project_Depthmasks(&files);
	for(i = 0;i<n;i++)
	{
		delete[] files[i];
	}
	files.clear();
	AutoSave_Project();
	SkinMsgBox("Depthmask import completed");
}

void ImportOptionsDlg::OnBrowseDepthmasksPath() 
{
	char path[512];
	if(BrowseForFolder(path, "Select the folder containing depthmask files to import"))
	{
		SetDlgItemText(IDC_DEPTHMASKS_PATH, path);
	}
}

void ImportOptionsDlg::UpdatePaths()
{
	char path[512];
	strcpy(path, "UNSPECIFIED");
	Get_Project_Images_Path(path);SetDlgItemText(IDC_IMAGES_PATH, path);
	Get_Project_Layermasks_Path(path);SetDlgItemText(IDC_LAYERMASKS_PATH, path);
	Get_Project_Depthmasks_Path(path);SetDlgItemText(IDC_DEPTHMASKS_PATH, path);
}

void ImportOptionsDlg::OnClose() 
{
	ShowWindow(SW_HIDE);
}

HBRUSH ImportOptionsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2||id==IDC_STATIC3)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	//pDC->SetBkColor(skin_text_bg_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
