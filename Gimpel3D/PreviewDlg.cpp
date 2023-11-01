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
// PreviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "PreviewDlg.h"
#include "G3DCoreFiles\Frame.h"

bool Step_Preview(int n);

float Recalc_Preview_Space(int start, int end);

bool Init_PreCached_Sequence_Data();
bool Free_PreCached_Sequence_Data();

float PreCache_DiskSpace_GB();
bool PreCache_Current_Frame_Data();
bool Toggle_PreCache_All_Frame_Data(int start, int end);

bool Toggle_Preview_Playback(int start, int end);

bool Display_Current_Preview_Frame(int index);

extern bool delete_preview_data_on_exit;

extern bool loop_preview;

extern bool render_preview_data;

extern bool play_preview;

extern bool restrict_preview_range;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PreviewDlg dialog


PreviewDlg::PreviewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PreviewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PreviewDlg)
	//}}AFX_DATA_INIT
}


void PreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PreviewDlg)
	DDX_Control(pDX, IDC_RECALC_SPACE, m_RecalcSpace);
	DDX_Control(pDX, IDC_STEP_FORWARD3, m_StepForward3);
	DDX_Control(pDX, IDC_STEP_FORWARD2, m_StepForward2);
	DDX_Control(pDX, IDC_STEP_BACK3, m_StepBack3);
	DDX_Control(pDX, IDC_STEP_BACK2, m_StepBack2);
	DDX_Control(pDX, IDC_DELETE_FRAME_DATA, m_DeleteFrameData);
	DDX_Control(pDX, IDC_CURRENT_FRAME, m_CurrentFrame);
	DDX_Control(pDX, IDC_STATIC4, m_Static4);
	DDX_Control(pDX, IDC_STATIC3, m_Static3);
	DDX_Control(pDX, IDC_STATIC2, m_Static2);
	DDX_Control(pDX, IDC_STATIC1, m_Static1);
	DDX_Control(pDX, IDC_STEP_FORWARD, m_StepForward);
	DDX_Control(pDX, IDC_STEP_BACK, m_StepBack);
	DDX_Control(pDX, IDC_START_FRAME, m_StartFrame);
	DDX_Control(pDX, IDC_RESTRICT_RANGE, m_RestrictRange);
	DDX_Control(pDX, IDC_PLAY_STOP, m_PlayStop);
	DDX_Control(pDX, IDC_LOOP, m_Loop);
	DDX_Control(pDX, IDC_END_FRAME, m_EndFrame);
	DDX_Control(pDX, IDC_DISK_SPACE, m_DiskSpace);
	DDX_Control(pDX, IDC_PRECACHE_CURRENT_FRAMES, m_PreCacheCurrentFrame);
	DDX_Control(pDX, IDC_PRECACHE_ALL_FRAMES, m_PreCacheAllFrames);
	//}}AFX_DATA_MAP
	m_RecalcSpace.SetFont(skin_font);
	m_CurrentFrame.SetFont(skin_font);
	m_Static4.SetFont(skin_font);
	m_Static3.SetFont(skin_font);
	m_Static2.SetFont(skin_font);
	m_Static1.SetFont(skin_font);
	m_StepForward.SetFont(skin_font);
	m_StepBack.SetFont(skin_font);
	m_StepForward2.SetFont(skin_font);
	m_StepBack2.SetFont(skin_font);
	m_StepForward3.SetFont(skin_font);
	m_StepBack3.SetFont(skin_font);
	m_StartFrame.SetFont(skin_font);
	m_RestrictRange.SetFont(skin_font);
	m_PlayStop.SetFont(skin_font);
	m_Loop.SetFont(skin_font);
	m_EndFrame.SetFont(skin_font);
	m_DiskSpace.SetFont(skin_font);
	m_DeleteFrameData.SetFont(skin_font);
	m_PreCacheCurrentFrame.SetFont(skin_font);
	m_PreCacheAllFrames.SetFont(skin_font);
	int n = Get_Num_Frames();
	char text[512];
	sprintf(text, "%i", Get_Num_Frames());
	m_EndFrame.SetWindowText(text);
	m_StartFrame.SetWindowText("0");
	Display_Current_Preview_Frame(Get_Current_Project_Frame());
	CheckDlgButton(IDC_DELETE_FRAME_DATA, delete_preview_data_on_exit);
	CheckDlgButton(IDC_LOOP, loop_preview);
	CheckDlgButton(IDC_RESTRICT_RANGE, restrict_preview_range);
}


BEGIN_MESSAGE_MAP(PreviewDlg, CDialog)
	//{{AFX_MSG_MAP(PreviewDlg)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_PRECACHE_ALL_FRAMES, OnPrecacheAllFrames)
	ON_BN_CLICKED(IDC_PRECACHE_CURRENT_FRAMES, OnPrecacheCurrentFrames)
	ON_BN_CLICKED(IDC_PLAY_STOP, OnPlayStop)
	ON_BN_CLICKED(IDC_PAUSE, OnPause)
	ON_BN_CLICKED(IDC_STEP_BACK, OnStepBack)
	ON_BN_CLICKED(IDC_STEP_FORWARD, OnStepForward)
	ON_BN_CLICKED(IDC_LOOP, OnLoop)
	ON_BN_CLICKED(IDC_DELETE_FRAME_DATA, OnDeleteFrameData)
	ON_BN_CLICKED(IDC_STEP_BACK2, OnStepBack2)
	ON_BN_CLICKED(IDC_STEP_FORWARD2, OnStepForward2)
	ON_BN_CLICKED(IDC_STEP_BACK3, OnStepBack3)
	ON_BN_CLICKED(IDC_STEP_FORWARD3, OnStepForward3)
	ON_BN_CLICKED(IDC_RESTRICT_RANGE, OnRestrictRange)
	ON_BN_CLICKED(IDC_RECALC_SPACE, OnRecalcSpace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PreviewDlg message handlers

PreviewDlg *previewDlg = 0;


bool Open_Preview_Dlg()
{
	if(!previewDlg)
	{
		previewDlg = new PreviewDlg;
		previewDlg->Create(IDD_PREVIEW_DLG);
		previewDlg->SetWindowText("Preview");
		Create_ToolSkin_Frame(previewDlg, "Preview", IDC_PREVIEW_TOOL);
	}
	previewDlg->GetParent()->ShowWindow(SW_SHOW);
	return true;
}

bool Close_Preview_Dlg()
{
	if(!previewDlg)
	{
		return false;
	}
	previewDlg->GetParent()->ShowWindow(SW_HIDE);
	return true;
}

bool Set_Preview_Dlg_Disk_Space(float gb)
{
	if(!previewDlg)
	{
		return false;
	}
	char text[512];
	sprintf(text, "%.2f GB", gb);
	previewDlg->m_DiskSpace.SetWindowText(text);
	return true;
}

bool Set_Preview_Play_Button(char *text)
{
	if(!previewDlg)
	{
		return false;
	}
	previewDlg->m_PlayStop.SetWindowText(text);
	return true;
}

bool Set_PreCache_All_Button(char *text)
{
	if(!previewDlg)
	{
		return false;
	}
	previewDlg->m_PreCacheAllFrames.SetWindowText(text);
	return true;
}

bool Display_Current_Preview_Frame(int index)
{
	if(!previewDlg)
	{
		return false;
	}
	char text[32];
	sprintf(text, "%i", index);
	previewDlg->m_CurrentFrame.SetWindowText(text);
	return true;
}

void Push_3D_View();
void Pop_3D_View();
bool Get_VC_Frame_Transform(int frame, float *pos, float *rot);
bool Convert_Image_To_Float(float *dst, unsigned char *src, int total);
bool Replace_RGB_Values(unsigned char *rgb);

bool preview_tool_open = false;

//temp hold for original frame data
float *pre_preview_vertices = 0;
unsigned char *pre_preview_colors = 0;
int pre_preview_frame = -1;

bool Open_Preview_Tool()
{
	if(preview_tool_open)return false;
	if(Get_Num_Frames()==0)
	{
		SkinMsgBox(0, "No loaded sequence, nothing to preview.", "Preview Tool Not Loading", MB_OK);
		return false;
	}
	preview_tool_open = true;
	Open_Preview_Dlg();
	Init_PreCached_Sequence_Data();
	previewDlg->RecalcSpace();
	render_preview_data = true;
	redraw_frame = true;
	if(frame)
	{
		pre_preview_vertices = new float[frame->width*frame->height*3];
		memcpy(pre_preview_vertices, frame->vertices, sizeof(float)*frame->width*frame->height*3);
		pre_preview_colors = new unsigned char[frame->width*frame->height*3];
		memcpy(pre_preview_colors, frame->original_rgb, sizeof(unsigned char)*frame->width*frame->height*3);
	}
	pre_preview_frame = Get_Current_Project_Frame();
	Push_3D_View();
	return true;
}

bool Close_Preview_Tool()
{
	if(!preview_tool_open)return false;
	preview_tool_open = false;
	Close_Preview_Dlg();
	Free_PreCached_Sequence_Data();
	render_preview_data = false;
	redraw_frame = true;
	//restore the previous frame info
	if(pre_preview_vertices)
	{
		if(frame)
		{
			memcpy(frame->vertices, pre_preview_vertices, sizeof(float)*frame->width*frame->height*3);
		}
		delete[] pre_preview_vertices;
		pre_preview_vertices = 0;
	}
	if(pre_preview_colors)
	{
		if(frame)
		{
			memcpy(frame->original_rgb, pre_preview_colors, sizeof(unsigned char)*frame->width*frame->height*3);
			Replace_RGB_Values(frame->original_rgb);
		}
		delete[] pre_preview_colors;
		pre_preview_colors = 0;
	}
	Pop_3D_View();
	if(frame)
	{
		Get_VC_Frame_Transform(Get_Current_Project_Frame(), frame->view_origin, frame->view_rotation);
	}
	Set_Project_Frame(pre_preview_frame);
	redraw_frame = true;
	return true;
}


void PreviewDlg::OnClose() 
{
	Close_Preview_Tool();
}

HBRUSH PreviewDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_EDIT||nCtlColor==CTLCOLOR_LISTBOX)
	{
		pDC->SetTextColor(skin_edit_text_color);
		pDC->SetBkColor(skin_edit_text_bg_color);
		pDC->SetBkMode(OPAQUE);
		return skin_edit_background_brush_color;
	}
	unsigned int id = pWnd->GetDlgCtrlID();
	if(id==IDC_STATIC1||id==IDC_STATIC2||id==IDC_STATIC3||id==IDC_STATIC4||id==IDC_DISK_SPACE||id==IDC_CURRENT_FRAME)
	{
		pDC->SetTextColor(skin_static_text_color);
		pDC->SetBkColor(skin_static_text_bg_color);
		return skin_static_background_brush_color;
	}
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
}

void PreviewDlg::OnPrecacheAllFrames() 
{
	int start = 0;
	int end = Get_Num_Frames();
	char text[512];
	if(restrict_preview_range)
	{
		int s = 0;
		int e = 0;
		m_StartFrame.GetWindowText(text, 512);sscanf(text, "%i", &s);
		m_EndFrame.GetWindowText(text, 512);sscanf(text, "%i", &e);
		if(e<=s)
		{
			SkinMsgBox("End frame must be greater than start frame!", 0, MB_OK);
			return;
		}
		if(s>end||e>end)
		{
			SkinMsgBox("Frames are out of range!", 0, MB_OK);
			return;
		}
		start = s;
		end = e;
	}
	Toggle_PreCache_All_Frame_Data(start, end);
}

void PreviewDlg::OnPrecacheCurrentFrames() 
{
	PreCache_Current_Frame_Data();
}

void PreviewDlg::OnPlayStop() 
{
	int start = 0;
	int end = Get_Num_Frames();
	char text[512];
	if(restrict_preview_range&&!play_preview)//about to start playing
	{
		int s = 0;
		int e = 0;
		m_StartFrame.GetWindowText(text, 512);sscanf(text, "%i", &s);
		m_EndFrame.GetWindowText(text, 512);sscanf(text, "%i", &e);
		if(e<=s)
		{
			SkinMsgBox("End frame must be greater than start frame!", 0, MB_OK);
			return;
		}
		if(s>end||e>end)
		{
			SkinMsgBox("Frames are out of range!", 0, MB_OK);
			return;
		}
		start = s;
		end = e;
	}
	Toggle_Preview_Playback(start, end);
}

void PreviewDlg::OnPause() 
{
}

void PreviewDlg::OnLoop() 
{
	loop_preview = IsDlgButtonChecked(IDC_LOOP)!=0;
}

void PreviewDlg::OnDeleteFrameData() 
{
	delete_preview_data_on_exit = IsDlgButtonChecked(IDC_DELETE_FRAME_DATA)!=0;
}

void PreviewDlg::OnStepBack() 
{
	Step_Preview(-1);
}

void PreviewDlg::OnStepForward() 
{
	Step_Preview(1);
}

void PreviewDlg::OnStepBack2() 
{
	Step_Preview(-5);
}

void PreviewDlg::OnStepForward2() 
{
	Step_Preview(5);
}

void PreviewDlg::OnStepBack3() 
{
	Step_Preview(-10);
}

void PreviewDlg::OnStepForward3() 
{
	Step_Preview(10);
}

void PreviewDlg::OnRestrictRange() 
{
	restrict_preview_range = IsDlgButtonChecked(IDC_RESTRICT_RANGE)!=0;
}

void PreviewDlg::RecalcSpace() 
{
	OnRecalcSpace();
}

void PreviewDlg::OnRecalcSpace() 
{
	int start = 0;
	int end = Get_Num_Frames();
	char text[512];
	if(restrict_preview_range)
	{
		int s = 0;
		int e = 0;
		m_StartFrame.GetWindowText(text, 512);sscanf(text, "%i", &s);
		m_EndFrame.GetWindowText(text, 512);sscanf(text, "%i", &e);
		if(e<s)
		{
			SkinMsgBox("End frame must be greater than start frame!", 0, MB_OK);
			return;
		}
		if(s>end||e>end)
		{
			SkinMsgBox("Frames are out of range!", 0, MB_OK);
			return;
		}
		if(e<end)
		{
			e++;
		}
		start = s;
		end = e;
		Set_Preview_Dlg_Disk_Space(Recalc_Preview_Space(start, end));
	}
	else
	{
		Set_Preview_Dlg_Disk_Space(PreCache_DiskSpace_GB());
	}
}
