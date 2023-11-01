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
// ExportOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "ExportOptionsDlg.h"

extern bool manually_rasterize_frames;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ExportOptionsDlg dialog


ExportOptionsDlg::ExportOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ExportOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ExportOptionsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	image_type = ANAGLYPH_OUTPUT;
	strcpy(extension, "bmp");
	Get_Project_Name(file_prefix);
}


void ExportOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ExportOptionsDlg)
	DDX_Control(pDX, IDC_START_EXPORT, m_StartExport);
	DDX_Control(pDX, IDC_SAVE_STEREO_IMAGE, m_SaveStereo);
	DDX_Control(pDX, IDC_SAVE_RIGHT_IMAGE, m_SaveRight);
	DDX_Control(pDX, IDC_SAVE_LEFT_IMAGE, m_SaveLeft);
	DDX_Control(pDX, IDC_SAVE_ANAGLYPH, m_SaveAnaglyph);
	DDX_Control(pDX, IDC_BROWSE_OUTPUT_PATH, m_BrowseOutputPath);
	DDX_Control(pDX, IDC_FILE_FORMAT, m_FileFormat);
	//}}AFX_DATA_MAP
	UpdateData();

	CheckDlgButton(IDC_USE_MANUAL_RASTERIZATION, manually_rasterize_frames);

	GetDlgItem(IDC_USE_MANUAL_RASTERIZATION)->SetFont(skin_font);

	GetDlgItem(IDC_STATIC1)->SetFont(skin_font);
	GetDlgItem(IDC_ANAGLYPH_OUTPUT)->SetFont(skin_font);
	GetDlgItem(IDC_STEREO_OUTPUT)->SetFont(skin_font);
	GetDlgItem(IDC_LEFT_RIGHT_OUTPUT)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC2)->SetFont(skin_font);
	GetDlgItem(IDC_FILE_FORMAT)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC3)->SetFont(skin_font);
	GetDlgItem(IDC_START_FRAME)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC4)->SetFont(skin_font);
	GetDlgItem(IDC_END_FRAME)->SetFont(skin_font);
	GetDlgItem(IDC_START_EXPORT)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC5)->SetFont(skin_font);
	GetDlgItem(IDC_OUTPUT_PATH)->SetFont(skin_font);
	GetDlgItem(IDC_BROWSE_OUTPUT_PATH)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC6)->SetFont(skin_font);
	GetDlgItem(IDC_FILE_PREFIX)->SetFont(skin_font);
	GetDlgItem(IDC_CENTER_VIEW)->SetFont(skin_font);
	GetDlgItem(IDC_SAVE_STEREO_IMAGE)->SetFont(skin_font);
	GetDlgItem(IDC_SAVE_LEFT_IMAGE)->SetFont(skin_font);
	GetDlgItem(IDC_SAVE_RIGHT_IMAGE)->SetFont(skin_font);
	GetDlgItem(IDC_SAVE_ANAGLYPH)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC7)->SetFont(skin_font);
	GetDlgItem(IDC_STATIC8)->SetFont(skin_font);
	GetDlgItem(IDC_ANTIALIAS_HARD_EDGES)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(ExportOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(ExportOptionsDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_ANAGLYPH_OUTPUT, OnAnaglyphOutput)
	ON_BN_CLICKED(IDC_STEREO_OUTPUT, OnStereoOutput)
	ON_BN_CLICKED(IDC_LEFT_RIGHT_OUTPUT, OnLeftRightOutput)
	ON_BN_CLICKED(IDC_CENTER_VIEW, OnCenterView)
	ON_BN_CLICKED(IDC_START_EXPORT, OnStartExport)
	ON_BN_CLICKED(IDC_BROWSE_OUTPUT_PATH, OnBrowseOutputPath)
	ON_BN_CLICKED(IDC_SAVE_LEFT_IMAGE, OnSaveLeftImage)
	ON_BN_CLICKED(IDC_SAVE_RIGHT_IMAGE, OnSaveRightImage)
	ON_BN_CLICKED(IDC_SAVE_STEREO_IMAGE, OnSaveStereoImage)
	ON_BN_CLICKED(IDC_SAVE_ANAGLYPH, OnSaveAnaglyph)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_USE_MANUAL_RASTERIZATION, OnUseManualRasterization)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ExportOptionsDlg message handlers

void ExportOptionsDlg::OnAnaglyphOutput() 
{
	image_type = ANAGLYPH_OUTPUT;
}

void ExportOptionsDlg::OnStereoOutput() 
{
	image_type = STEREO_OUTPUT;
}

void ExportOptionsDlg::OnLeftRightOutput() 
{
	image_type = LEFTRIGHT_OUTPUT;
}

void ExportOptionsDlg::OnCenterView() 
{
	auto_center_stereoview = IsDlgButtonChecked(IDC_CENTER_VIEW)!=0;
}

void ExportOptionsDlg::OnStartExport() 
{
	if(exporting)
	{
		if(SkinMsgBox("Stop export?", " ", MB_YESNO)==IDYES)
		{
			exporting = false;
			SetDlgItemText(IDC_START_EXPORT, "Start Export");
		}
		return;
	}
	exporting = true;
	SetDlgItemText(IDC_START_EXPORT, "Stop Export");
	char text[32];
	GetDlgItemText(IDC_START_FRAME, text, 32);
	sscanf(text, "%i", &start_frame);
	GetDlgItemText(IDC_END_FRAME, text, 32);
	sscanf(text, "%i", &end_frame);
	if(start_frame>end_frame)
	{
		SkinMsgBox("Start frame must be less than end frame!");
		exporting = false;
		return;
	}
	if(start_frame<0||start_frame>Get_Num_Frames())
	{
		SkinMsgBox("Invalid start frame!");
		exporting = false;
		return;
	}
	if(end_frame<0||end_frame>Get_Num_Frames())
	{
		SkinMsgBox("Invalid end frame!");
		exporting = false;
		return;
	}
	GetDlgItemText(IDC_FILE_PREFIX, file_prefix, 64);
	m_FileFormat.GetWindowText(extension, 8);
	int old_frame = Get_Current_Project_Frame();
	int i;
	if(!strcmp(extension, "avi"))
	{
		Export_AVI();
	}
	else
	{
		GetDlgItemText(IDC_OUTPUT_PATH, output_path, 512);
		CreateDirectory(output_path, 0);
		if(image_type==LEFTRIGHT_OUTPUT)
		{
			char temp[512];
			sprintf(temp, "%s\\LEFT", output_path);
			CreateDirectory(temp, 0);
			sprintf(temp, "%s\\RIGHT", output_path);
			CreateDirectory(temp, 0);
		}
		for(i = start_frame;i<end_frame;i++)
		{
			if(exporting)
			{
				if(!Export_Frame(i, image_type, output_path, extension, file_prefix))
				{
					SkinMsgBox("An error occured while exporting frames!", 0, MB_OK);
					exporting = false;
				}
			}
			else
			{
				i = end_frame;
			}
		}
	}
	Set_Project_Frame(old_frame);
	SetDlgItemText(IDC_START_EXPORT, "Start Export");
	if(!exporting)
	{
		SkinMsgBox("Export cancelled.");
	}
	else
	{
		SkinMsgBox("Export completed.");
	}
	exporting = false;
	redraw_frame = true;
	redraw_edit_window = true;
}

void ExportOptionsDlg::OnBrowseOutputPath() 
{
	char path[512];
	if(BrowseForFolder(path, "Select the folder for the exported image sequence"))
	{
		SetDlgItemText(IDC_OUTPUT_PATH, path);
		strcpy(output_path, path);
	}
}

void ExportOptionsDlg::UpdateData()
{
	start_frame = 0;
	end_frame = Get_Num_Frames();
	Get_Default_Output_Path(output_path);
	m_FileFormat.SelectString(0, extension);
	CheckDlgButton(IDC_ANAGLYPH_OUTPUT, image_type==ANAGLYPH_OUTPUT);
	CheckDlgButton(IDC_STEREO_OUTPUT, image_type==STEREO_OUTPUT);
	CheckDlgButton(IDC_LEFT_RIGHT_OUTPUT, image_type==LEFTRIGHT_OUTPUT);
	SetDlgItemText(IDC_START_FRAME, "0");
	char text[32];
	sprintf(text, "%i", start_frame);
	SetDlgItemText(IDC_START_FRAME, text);
	sprintf(text, "%i", end_frame);
	SetDlgItemText(IDC_END_FRAME, text);
	SetDlgItemText(IDC_OUTPUT_PATH, output_path);
	SetDlgItemText(IDC_FILE_PREFIX, file_prefix);
	CheckDlgButton(IDC_CENTER_VIEW, auto_center_stereoview);
}


void ExportOptionsDlg::Export_AVI()
{
	char avifile[512];
	char leftfile[512];
	char rightfile[512];
	int avi_width = Frame_Width();
	int avi_height = Frame_Height();
	int i;
	if(image_type==STEREO_OUTPUT)
	{
		avi_width+=Frame_Width();
	}
	unsigned char *avi_frame = new unsigned char[avi_width*avi_height*3];
	if(Browse(avifile, "avi", true))
	{
		if(image_type==LEFTRIGHT_OUTPUT)
		{
			char *c = strrchr(avifile, '.');
			if(c)*c = 0;
			strcpy(leftfile, avifile);
			strcpy(rightfile, avifile);
			strcat(leftfile, "_LEFT.avi");
			strcat(rightfile, "_RIGHT.avi");
			if(!Start_AVI_File(leftfile, avi_width, avi_height))
			{
				SkinMsgBox("Error opening avi file!", leftfile, MB_OK);
				delete[] avi_frame;
				return;
			}
		}
		else
		{
			if(!Start_AVI_File(avifile, avi_width, avi_height))
			{
				SkinMsgBox("Error opening avi file!", avifile, MB_OK);
				delete[] avi_frame;
				return;
			}
		}
		for(i = start_frame;i<end_frame;i++)
		{
			Set_Project_Frame(i);
			Render_Frame_Update();
			if(image_type==ANAGLYPH_OUTPUT)
			{
				Get_Anaglyph_Frame(avi_frame);
				Add_AVI_Frame(avi_frame);
			}
			if(image_type==STEREO_OUTPUT)
			{
				Get_Stereo_Frame(avi_frame);
				Add_AVI_Frame(avi_frame);
			}
			if(image_type==LEFTRIGHT_OUTPUT)
			{
				Get_Left_Frame(avi_frame);
				Add_AVI_Frame(avi_frame);
			}
			if(!exporting)//cancelled
			{
				i = end_frame;
			}
		}
	}
	Close_AVI();
	if(image_type!=LEFTRIGHT_OUTPUT)
	{
		delete[] avi_frame;
		if(!exporting)//cancelled
		{
			//remove file
			remove(avifile);
		}
		return;
	}
	if(!exporting)//cancelled
	{
		//remove file and return;
		remove(leftfile);
		return;
	}
	if(!Start_AVI_File(rightfile, Frame_Width(), Frame_Height()))
	{
		SkinMsgBox("Error opening avi file!", rightfile, MB_OK);
		return;
	}
	for(i = start_frame;i<end_frame;i++)
	{
		Set_Project_Frame(i);
		Render_Frame_Update();
		if(image_type==ANAGLYPH_OUTPUT)
		{
			Get_Anaglyph_Frame(avi_frame);
			Add_AVI_Frame(avi_frame);
		}
		if(image_type==STEREO_OUTPUT)
		{
			Get_Stereo_Frame(avi_frame);
			Add_AVI_Frame(avi_frame);
		}
		if(image_type==LEFTRIGHT_OUTPUT)
		{
			Get_Right_Frame(avi_frame);
			Add_AVI_Frame(avi_frame);
		}
		if(!exporting)//cancelled
		{
			i = end_frame;
		}
	}
	Close_AVI();
	delete[] avi_frame;
	if(!exporting)//cancelled
	{
		//remove file and return;
		remove(leftfile);
		remove(rightfile);
	}
}

void ExportOptionsDlg::OnClose() 
{
	ShowWindow(SW_HIDE);
}

void ExportOptionsDlg::OnSaveLeftImage() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "*", true))
	{
		Set_Camera_To_Target();
		Save_Left_Image(data);
		Print_To_Console("Saved left image to \"%s\"", data);
	}
}

void ExportOptionsDlg::OnSaveRightImage() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "*", true))
	{
		Set_Camera_To_Target();
		Save_Right_Image(data);
		Print_To_Console("Saved right image to \"%s\"", data);
	}
}

void ExportOptionsDlg::OnSaveStereoImage() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "*", true))
	{
		Set_Camera_To_Target();
		Save_Stereo_Image(data);
		Print_To_Console("Saved stereo image to \"%s\"", data);
	}
}

void ExportOptionsDlg::OnSaveAnaglyph() 
{
	if(!frame_loaded)
	{
		SkinMsgBox("No session loaded!", 0, MB_OK);
		return;
	}
	char data[512];
	if(Browse(data, "bmp", true))
	{
		Set_Camera_To_Target();
		Save_Anaglyph_Image(data);
		Print_To_Console("Saved anaglyph image to \"%s\"", data);
	}
}

HBRUSH ExportOptionsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2||id==IDC_STATIC3||id==IDC_STATIC4||id==IDC_STATIC5||id==IDC_STATIC6||id==IDC_STATIC7)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void ExportOptionsDlg::OnUseManualRasterization() 
{
	manually_rasterize_frames = IsDlgButtonChecked(IDC_USE_MANUAL_RASTERIZATION)!=0;
}
