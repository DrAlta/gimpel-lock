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
// TimeLineSliderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "TimeLineSliderDlg.h"
#include <Winuser.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TimeLineSliderDlg dialog

TimeLineSliderDlg *timelineSliderDlg = 0;

int timeline_slider_xstart = 50;
int timeline_slider_xend = 50;

int max_timeline_frames = 60;
int num_timeline_frames = 0;
int timeline_frame_start = 0;

TimeLineSliderDlg::TimeLineSliderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TimeLineSliderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TimeLineSliderDlg)
	//}}AFX_DATA_INIT
}


void TimeLineSliderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TimeLineSliderDlg)
	DDX_Control(pDX, IDC_SHIFT_FORWARD, m_ShiftForward);
	DDX_Control(pDX, IDC_SHIFT_BACK, m_ShiftBack);
	DDX_Control(pDX, IDC_ZOOM_OUT, m_ZoomOut);
	DDX_Control(pDX, IDC_ZOOM_IN, m_ZoomIn);
	DDX_Control(pDX, IDC_TIMELINE_SLIDER, m_Slider);
	//}}AFX_DATA_MAP
	
	m_ShiftForward.SetFont(skin_font);
	m_ShiftBack.SetFont(skin_font);
	m_ZoomOut.SetFont(skin_font);
	m_ZoomIn.SetFont(skin_font);

	GetDlgItem(IDC_TIMELINE_SLIDER)->SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(TimeLineSliderDlg, CDialog)
	//{{AFX_MSG_MAP(TimeLineSliderDlg)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_PAUSE, OnPause)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_ZOOM_IN, OnZoomIn)
	ON_BN_CLICKED(IDC_ZOOM_OUT, OnZoomOut)
	ON_BN_CLICKED(IDC_SHIFT_BACK, OnShiftBack)
	ON_BN_CLICKED(IDC_SHIFT_FORWARD, OnShiftForward)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TimeLineSliderDlg message handlers

void TimeLineSliderDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	Set_Project_Frame(timeline_frame_start+m_Slider.GetPos());
}

void TimeLineSliderDlg::ClearTextItems()
{
	text_items.clear();
}

void TimeLineSliderDlg::RefreshWindow()
{
	CRect rect;
	GetClientRect(&rect);
	InvalidateRect(&rect);
}

void TimeLineSliderDlg::Add_Text_Item(char *text, int xpos, int ypos)
{
	FC_TEXT_ITEM ti;
	text_items.push_back(ti);
	FC_TEXT_ITEM *p = &text_items[text_items.size()-1];
	strcpy(p->text, text);
	p->len = strlen(text);
	p->xpos = xpos+timeline_slider_xstart;
	p->ypos = ypos;
}

bool Init_Timeline_Slider()
{
	timelineSliderDlg = new TimeLineSliderDlg;
	timelineSliderDlg->Create(IDD_TIMELINE_SLIDER_DLG);
	timelineSliderDlg->ShowWindow(SW_SHOW);
	SetParent(timelineSliderDlg->GetSafeHwnd(), mainwindow->GetSafeHwnd());
	Resize_Timeline_Slider();
	return true;
}

bool Free_Timeline_Slider()
{
	delete timelineSliderDlg;
	return true;
}

bool Create_Frame_Counter_Text()
{
	timelineSliderDlg->ClearTextItems();
	int n = timelineSliderDlg->m_Slider.GetNumTics();
	if(timelineSliderDlg->m_Slider.GetTicPos(n-1)==-1)
	{
		//just returned -1 index for invalid 0-based tic index when it just reported the number
		//of valid tics it should have
		n--;//since the control is reporting a bogus number
		//and I have to manually check for the correct value and assume it's over
		//I can fix their incorrect number here, though it IS the number of tics I specified
		//when initializing so I can only guess that the control is somehow unable to return the final
		//position
		//or the first 1
		n--;//maybe it doesn't consider the first and last ones as "real" tics but this isn't documented
		//anywhere I've seen, The Shit just Doesn't Work As Advertised And I Am Working Around How It DOES Work
	}
	if(n>0)
	{
		char text[32];
		int frame_number = 0;
		for(int i = 0;i<n;i++)
		{
			int xpos = timelineSliderDlg->m_Slider.GetTicPos(i);
			frame_number = timeline_frame_start+i+2;
			sprintf(text, "%i", frame_number);//would be nice to just get the real indices from the control
			//apparently this is too much for the control to handle
			timelineSliderDlg->Add_Text_Item(text, xpos, 0);
		}
		//go ahead and get the first and last ones manually since the control isn't capable of dealing with
		//the tic mark positions
		//funny I didn't see anything about this in msdn
		int firstx = timelineSliderDlg->m_Slider.GetTicPos(0);
		int secondx = timelineSliderDlg->m_Slider.GetTicPos(1);
		int lastx = timelineSliderDlg->m_Slider.GetTicPos(n-1);
		int spacing = secondx-firstx;

		int xpos = firstx-spacing;
		frame_number = timeline_frame_start+1;
		sprintf(text, "%i", frame_number);
		timelineSliderDlg->Add_Text_Item(text, xpos, 0);//6);

		xpos = lastx + (spacing);
		frame_number = timeline_frame_start+n+2;
		sprintf(text, "%i", frame_number);
		timelineSliderDlg->Add_Text_Item(text, xpos, 0);//6);
	}
	timelineSliderDlg->RefreshWindow();
	return true;
}

bool Set_Timeline_Ticks(int n)
{
	if(n>max_timeline_frames)
	{
		n = max_timeline_frames;
	}
	timelineSliderDlg->m_Slider.SetRange(0, n-1, true);
	timelineSliderDlg->m_Slider.SetPageSize(1);
	num_timeline_frames = n;

	Create_Frame_Counter_Text();
	return true;
}

bool Set_Timeline_Frame(int n)
{

	bool reset_frames = false;
	int index = n-timeline_frame_start;
	if(index>=num_timeline_frames)
	{
		index = 0;
		timeline_frame_start = n;
		reset_frames = true;
	}
	else if(index<0)
	{
		int ti = n-(timeline_frame_start-num_timeline_frames);
		if(ti>=0&&ti<num_timeline_frames)
		{
			timeline_frame_start -= num_timeline_frames;
			if(timeline_frame_start<0)
			{
				timeline_frame_start = 0;
				ti = n;
			}
			index = ti;
			reset_frames = true;
		}
		else
		{
			index = 0;
			timeline_frame_start = n;
			reset_frames = true;
		}
	}

	if(reset_frames)
	{
		if(timeline_frame_start+num_timeline_frames>Get_Num_Frames())
		{
			timeline_frame_start = Get_Num_Frames()-num_timeline_frames;
			index = n-timeline_frame_start;
		}
		Create_Frame_Counter_Text();
	}

	timelineSliderDlg->m_Slider.SetPos(index);
	return true;
}

bool Resize_Timeline_Slider()
{
	if(timelineSliderDlg)
	{
		int play_buttons_width = 20;
		int play_buttons_height = 20;
		int play_buttons_startx = 0;
		int play_buttons_starty = 20;
		CRect rect;
		mainwindow->GetClientRect(&rect);
		int width = rect.Width();
		int height = 45;
		int xpos = 0;
		int ypos = (rect.Height()-height);
		timelineSliderDlg->SetWindowPos(0, xpos, ypos, width, height, 0);
		int right_border = 20;
		int timeline_slider_width = ((width-(play_buttons_width*3))-right_border)+10;
		timeline_slider_xstart = play_buttons_width*3;
		timeline_slider_xend = timeline_slider_xstart+timeline_slider_width;
		timelineSliderDlg->m_Slider.SetWindowPos(0, timeline_slider_xstart+5, 11, timeline_slider_width, height-5, 0);
		height -= 5;
		ypos -= height;
		Create_Frame_Counter_Text();
		int zoom_buttons_startx = 0;
		int zoom_buttons_starty = 0;
		int zoom_buttons_width = 31;
		int zoom_buttons_height = 20;
		timelineSliderDlg->m_ZoomIn.SetWindowPos(0, zoom_buttons_startx, zoom_buttons_starty, zoom_buttons_width, zoom_buttons_height, 0);
		timelineSliderDlg->m_ZoomOut.SetWindowPos(0, zoom_buttons_startx+zoom_buttons_width, zoom_buttons_starty, zoom_buttons_width, zoom_buttons_height, 0);
		zoom_buttons_starty += zoom_buttons_height;
		timelineSliderDlg->m_ShiftBack.SetWindowPos(0, zoom_buttons_startx, zoom_buttons_starty, zoom_buttons_width, zoom_buttons_height, 0);
		timelineSliderDlg->m_ShiftForward.SetWindowPos(0, zoom_buttons_startx+zoom_buttons_width, zoom_buttons_starty, zoom_buttons_width, zoom_buttons_height, 0);
	}
	return true;
}

bool Resize_Timeline()
{
	return Resize_Timeline_Slider();
}

/*
bool Update_Play_Buttons()
{
	if(Get_Num_Frames()<2)
	{
		timelineSliderDlg->m_Start.EnableWindow(false);
		timelineSliderDlg->m_Stop.EnableWindow(false);
		timelineSliderDlg->m_Pause.EnableWindow(false);
		return false;
	}
	if(run_sequence)
	{
		timelineSliderDlg->m_Start.EnableWindow(false);
		timelineSliderDlg->m_Stop.EnableWindow(true);
		timelineSliderDlg->m_Pause.EnableWindow(true);
	}
	else
	{
		timelineSliderDlg->m_Start.EnableWindow(true);
		timelineSliderDlg->m_Stop.EnableWindow(false);
		timelineSliderDlg->m_Pause.EnableWindow(false);
	}
	return true;
}
*/

void TimeLineSliderDlg::OnStart() 
{
}

void TimeLineSliderDlg::OnStop() 
{
}

void TimeLineSliderDlg::OnPause() 
{
}

BOOL TimeLineSliderDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;
}

HBRUSH TimeLineSliderDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkColor(skin_text_bg_color);	
	return skin_background_brush_color;
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}


void TimeLineSliderDlg::OnZoomIn() 
{
	int nn = num_timeline_frames/2;
	if(nn>=10)
	{
		Set_Timeline_Ticks(nn);
	}
}

void TimeLineSliderDlg::OnZoomOut() 
{
	int nn = num_timeline_frames*2;
	if(nn>=10)
	{
		Set_Timeline_Ticks(nn);
	}
}

void TimeLineSliderDlg::OnShiftBack() 
{
	int f = Get_Current_Project_Frame()-num_timeline_frames;
	if(f>=0&&f<Get_Num_Frames())
	{
		Set_Project_Frame(f);
	}
}

void TimeLineSliderDlg::OnShiftForward() 
{
	int f = Get_Current_Project_Frame()+num_timeline_frames;
	if(f>=0&&f<Get_Num_Frames())
	{
		Set_Project_Frame(f);
	}
}

void TimeLineSliderDlg::OnPaint() 
{
	CPaintDC dc(this);
	dc.SelectObject(skin_font);
	int n = text_items.size();
	for(int i = 0;i<n;i++)
	{
		FC_TEXT_ITEM *ti = &text_items[i];
		dc.TextOut(ti->xpos,ti->ypos,ti->text,ti->len);
	}
}
