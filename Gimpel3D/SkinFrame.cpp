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
// SkinFrame.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "Gimpel3D.h"
#include "SkinFrame.h"
#include <vector>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SkinFrame dialog
bool Add_ToolTray_Item(char *text, int restore_control_id);
bool Remove_ToolTray_Item(int restore_control_id);
bool Notify_ToolTray_Item_Visible(int restore_control_id, char *text, bool visible);
bool ToolTray_Item_Exists(SkinFrame *sf);
bool Align_To_Tooltray(SkinFrame *skinframe);

bool Align_Plugin_Dialog_To_Tooltray(CWnd *p);


SkinFrame::SkinFrame(CWnd* pParent /*=NULL*/)
	: CDialog(SkinFrame::IDD, pParent)
{
	//{{AFX_DATA_INIT(SkinFrame)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	mouse_pos.x = 0;
	mouse_pos.y = 0;
	left_mouse_pressed = false;
	dragging_window = false;
	plugin_control = false;
}

void SkinFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SkinFrame)
	DDX_Control(pDX, IDC_MINIMIZE_BUTTON, m_MinimizeButton);
	DDX_Control(pDX, IDC_CLOSE_BUTTON, m_CloseButton);
	//}}AFX_DATA_MAP
	m_MinimizeButton.SetFont(skin_font);
	m_CloseButton.SetFont(skin_font);
}


BEGIN_MESSAGE_MAP(SkinFrame, CDialog)
	//{{AFX_MSG_MAP(SkinFrame)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_CLOSE_BUTTON, OnCloseButton)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_MINIMIZE_BUTTON, OnMinimizeButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SkinFrame message handlers

vector<SkinFrame*> skin_frames;

const int skin_titlebar_height = 19;
const int skin_border_width = 2;

int close_button_xoffset = -2;
int close_button_yoffset = 1;
int close_button_width = 14;
int close_button_height = 18;

bool Free_Skin_Frames()
{
	//NOTE: this crashes on new build, skip for now..
	return true;
	int n = skin_frames.size();
	for(int i = 0;i<n;i++)
	{
		if(skin_frames[i]->child_window)
		{
			delete skin_frames[i]->child_window;
			skin_frames[i]->child_window = 0;
		}
		delete skin_frames[i];
	}
	skin_frames.clear();
	return true;
}

bool ReSize_Skinframe(SkinFrame *sf)
{
	CRect rect;
	sf->child_window->GetClientRect(&rect);
	sf->SetWindowPos(0, 0, 0, rect.Width()+(skin_border_width*2), rect.Height()+skin_titlebar_height+(skin_border_width*2), SWP_NOMOVE);
	sf->child_window->SetWindowPos(0, skin_border_width, skin_titlebar_height+skin_border_width, 0, 0, SWP_NOSIZE);
	sf->m_CloseButton.SetWindowPos(0, (rect.Width()-close_button_xoffset)-close_button_width, close_button_yoffset, close_button_width, close_button_height, 0);
	return true;
}

CWnd* Create_Skin_Frame(CWnd *child)
{
	SkinFrame *sf = new SkinFrame;
	sf->child_window = child;
	sf->Create(IDD_SKIN_FRAME);
	sf->SetFont(skin_font);
	CRect rect;
	child->GetClientRect(&rect);
	sf->SetWindowPos(0, 0, 0, rect.Width()+(skin_border_width*2), rect.Height()+skin_titlebar_height+(skin_border_width*2), SWP_NOMOVE);
	child->SetParent(sf);
	child->SetWindowPos(0, skin_border_width, skin_titlebar_height+skin_border_width, 0, 0, SWP_NOSIZE);
	sf->m_CloseButton.SetWindowPos(0, (rect.Width()-close_button_xoffset)-close_button_width, close_button_yoffset, close_button_width, close_button_height, 0);
	
	sf->tooltray_control_id = -1;
	sf->m_MinimizeButton.ShowWindow(SW_HIDE);
	child->ShowWindow(SW_SHOW);
	sf->ShowWindow(SW_SHOW);
	skin_frames.push_back(sf);
	sf->tooltray_control_id = -1;
	return sf;
}

CWnd* Create_Plugin_Skin_Frame(CWnd *_sf, CWnd *child)
{
	SkinFrame *sf = (SkinFrame*)_sf;
	sf->child_window = child;
	sf->SetFont(skin_font);
	CRect rect;
	child->GetClientRect(&rect);
	sf->SetWindowPos(0, 0, 0, rect.Width()+(skin_border_width*2), rect.Height()+skin_titlebar_height+(skin_border_width*2), SWP_NOMOVE);
	child->SetParent(sf);
	child->SetWindowPos(0, skin_border_width, skin_titlebar_height+skin_border_width, 0, 0, SWP_NOSIZE);
	sf->m_CloseButton.SetWindowPos(0, (rect.Width()-close_button_xoffset)-close_button_width, close_button_yoffset, close_button_width, close_button_height, 0);
	
	sf->tooltray_control_id = -1;
	sf->m_MinimizeButton.SetWindowPos(0, ((rect.Width()-close_button_xoffset)-(close_button_width*2))-1, close_button_yoffset, close_button_width, close_button_height, 0);
	sf->m_MinimizeButton.ShowWindow(SW_SHOW);
	child->ShowWindow(SW_SHOW);
	sf->ShowWindow(SW_HIDE);
	skin_frames.push_back(sf);
	sf->tooltray_control_id = -1;
	sf->plugin_control = true;
	char tool_text[512];
	child->GetWindowText(tool_text, 512);
	sf->SetWindowText(tool_text);
	//if it exists on the tooltray
	Align_Plugin_Dialog_To_Tooltray(sf);
	return sf;
}

CWnd* Get_Plugin_Skin_Frame()
{
	SkinFrame *sf = new SkinFrame;
	sf->child_window = 0;
	sf->Create(IDD_SKIN_FRAME);
	sf->SetFont(skin_font);
	sf->tooltray_control_id = -1;
	sf->m_MinimizeButton.ShowWindow(SW_HIDE);
	sf->tooltray_control_id = -1;
	return sf;
}


CWnd* Create_ToolSkin_Frame(CWnd *child, char *tool_text, int tooltray_control_id)
{
	SkinFrame *sf = (SkinFrame*)Create_Skin_Frame(child);
	CRect rect;
	child->GetClientRect(&rect);
	sf->m_MinimizeButton.SetWindowPos(0, ((rect.Width()-close_button_xoffset)-(close_button_width*2))-1, close_button_yoffset, close_button_width, close_button_height, 0);
	sf->m_MinimizeButton.ShowWindow(SW_SHOW);
	sf->tooltray_control_id = tooltray_control_id;
	sf->SetWindowText(tool_text);
	Notify_ToolTray_Item_Visible(tooltray_control_id, tool_text, true);
	Align_To_Tooltray(sf);
	return sf;
}

bool Remove_Skinframe(CWnd *sf)
{
	int n = skin_frames.size();
	for(int i = 0;i<n;i++)
	{
		if(skin_frames[i]==sf)
		{
			delete sf;
			skin_frames.erase(skin_frames.begin()+i);
			return true;
		}
	}
	return false;
}

bool Remove_Plugin_Skin_Frame(CWnd *sf)
{
	int n = skin_frames.size();
	for(int i = 0;i<n;i++)
	{
		if(skin_frames[i]==sf)
		{
			delete sf;
			skin_frames.erase(skin_frames.begin()+i);
			return true;
		}
	}
	return false;
}

bool Close_SkinFrame(int restore_control_id)
{
	int n = skin_frames.size();
	for(int i = 0;i<n;i++)
	{
		if(skin_frames[i]->tooltray_control_id==restore_control_id)
		{
			skin_frames[i]->SendMessage(WM_COMMAND, IDC_CLOSE_BUTTON, IDC_CLOSE_BUTTON);
			return true;
		}
	}
	return false;
}

CWnd* Create_App_Skin_Frame(CWnd *w)
{
	SkinFrame *sf = new SkinFrame;
	sf->child_window = w;
	skin_frames.push_back(sf);
	sf->ShowWindow(SW_MAXIMIZE);
	return sf;
}

void SkinFrame::OnClose() 
{
}

void SkinFrame::DrawSkinFrame(CDC *pDC)
{
	if(child_window)
	{
		CRect rect;
		GetClientRect(rect);
		CRect trect = rect;
		trect.bottom = trect.top + skin_titlebar_height;
		rect.top += skin_titlebar_height;
		pDC->FillSolidRect(trect, skin_titlebar_color);
		pDC->DrawEdge(&rect, EDGE_BUMP, BF_RECT);
		pDC->DrawEdge(&trect, EDGE_BUMP, BF_TOPLEFT);
		pDC->DrawEdge(&trect, EDGE_BUMP, BF_RIGHT);

		pDC->SelectObject(skin_font);
		pDC->SetTextColor(skin_titlebar_text_color);
		pDC->SetBkColor(skin_titlebar_color);

		char text[512];
		child_window->GetWindowText(text, 512);
		trect.top += 1;
		trect.left += 8;
		pDC->DrawText(text, strlen(text), trect, DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	}
}

bool SkinFrame::PointInsideTitleBar(CPoint *p) 
{
	CPoint tp = *p;
	ScreenToClient(&tp);
	if(tp.y<=skin_titlebar_height)
	{
		return true;
	}
	return false;
}

void SkinFrame::DragWindow(int dx, int dy) 
{
	CRect rect;
	GetWindowRect(rect);
	SetWindowPos(0, rect.left+dx, rect.top+dy, 0, 0, SWP_NOSIZE);
}

void Minimize_SkinFrame(SkinFrame *sf)
{
}

LRESULT SkinFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	CPoint new_mouse_pos;
	switch(message)
	{
	case WM_ACTIVATE:
		return 0;
	case WM_LBUTTONDOWN:
		left_mouse_pressed = true;
		if(PointInsideTitleBar(&mouse_pos)) 
		{
			dragging_window = true;
			SetCapture();
		}
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		left_mouse_pressed = false;
		dragging_window = false;
		break;
	case WM_KILLFOCUS:
		ReleaseCapture();
		left_mouse_pressed = false;
		break;
	case WM_MOUSEMOVE:
			GetCursorPos(&new_mouse_pos);
			if(dragging_window)
			{
				DragWindow(new_mouse_pos.x-mouse_pos.x, new_mouse_pos.y-mouse_pos.y);
			}
			mouse_pos = new_mouse_pos;
		break;
	};
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL SkinFrame::OnEraseBkgnd(CDC* pDC) 
{
	DrawSkinFrame(pDC);
	return TRUE;
}

void SkinFrame::OnCloseButton() 
{
	child_window->SendMessage(WM_CLOSE, 0, 0);
	ShowWindow(SW_HIDE);
}

void SkinFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if(child_window)
	{
		if(bShow)
		{
			if(child_window->GetSafeHwnd()!=0)
			{
				child_window->ShowWindow(SW_SHOW);
			}
			char text[512];
			GetWindowText(text, 512);
			Notify_ToolTray_Item_Visible(tooltray_control_id, text, true);
		}
		else
		{
			if(child_window->GetSafeHwnd()!=0)
			{
				child_window->ShowWindow(SW_HIDE);
			}
			char text[512];
			GetWindowText(text, 512);
			Notify_ToolTray_Item_Visible(tooltray_control_id, text, false);
		}
	}
}

void SkinFrame::OnMinimizeButton() 
{
	child_window->SendMessage(WM_CLOSE, 0, 0);
	char text[512];
	GetWindowText(text, 512);
	Add_ToolTray_Item(text, tooltray_control_id);
	ShowWindow(SW_HIDE);
}
