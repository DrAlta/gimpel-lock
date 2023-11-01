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
// SkinMessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "Gimpel3D.h"
#include "SkinMessageBox.h"
#include "G3DMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SkinMessageBox dialog

SkinFrame *mb_skinframe = 0;

SkinMessageBox::SkinMessageBox(CWnd* pParent /*=NULL*/)
	: CDialog(SkinMessageBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(SkinMessageBox)
	//}}AFX_DATA_INIT
	strcpy(title, "TITLE");
	strcpy(text, "TEXT");
	return_type = -1;
}


void SkinMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SkinMessageBox)
	DDX_Control(pDX, IDC_YES, m_Yes);
	DDX_Control(pDX, IDC_OK, m_OK);
	DDX_Control(pDX, IDC_NOPE, m_No);
	DDX_Control(pDX, IDC_CANCEL, m_Cancel);
	//}}AFX_DATA_MAP

	SetWindowText(title);

	m_Yes.SetFont(skin_font);
	m_OK.SetFont(skin_font);
	m_No.SetFont(skin_font);
	m_Cancel.SetFont(skin_font);

	dlg_width = 200;
	dlg_height = 80;

	int max_line_size = 32;

	char ntext[512];
	int len = strlen(text);
	int cnt = 0;
	text_items.push_back(&text[0]);//get the first string
	int i;
	for(i = 0;i<len;i++)
	{
		if(cnt>=max_line_size&&text[i]==' ')
		{
			ntext[i] = 0;
			cnt = 0;
			dlg_height += 20;
			text_items.push_back(&text[i+1]);//get the next string
		}
		else
		{
			ntext[i] = text[i];
		}
		cnt++;
	}
	ntext[i] = 0;
	memcpy(text, ntext, sizeof(unsigned char)*(len+1));


	int button_width = 60;
	int button_height = 26;
	int bottom_border = 8;
	int button_y = dlg_height - (button_height+bottom_border);

	int button1_x = (((dlg_width/2)-(button_width)))/2;
	int button2_x = button1_x+(dlg_width/2);

	int mid_button_x = (dlg_width/2)-(button_width/2);


	text_area_width = dlg_width;
	text_area_height = button_y;
	int text_x = 0;
	int text_y = 0;


	if(type==MB_OK)
	{
		m_OK.SetWindowPos(0, mid_button_x, button_y, button_width, button_height, 0);
		m_OK.ShowWindow(SW_SHOW);
	}
	if(type==MB_YESNO)
	{
		m_Yes.SetWindowPos(0, button1_x, button_y, button_width, button_height, 0);
		m_No.SetWindowPos(0, button2_x, button_y, button_width, button_height, 0);
		m_Yes.ShowWindow(SW_SHOW);
		m_No.ShowWindow(SW_SHOW);
	}

	int dtw = 0;
	int dth = 0;
	Get_Desktop_Dimensions(&dtw, &dth);

	int xpos = (dtw/2)-(dlg_width/2);
	int ypos = (dth/2)-(dlg_height/2);

	SetWindowPos(0, xpos, ypos, dlg_width, dlg_height, 0);
	if(!mb_skinframe)
	{
		mb_skinframe = (SkinFrame*)Create_Skin_Frame(this);
		mb_skinframe->m_CloseButton.ShowWindow(SW_HIDE);
		mb_skinframe->SetWindowPos(0, xpos, ypos, dlg_width, dlg_height, SWP_NOSIZE);
	}
	else
	{
		mb_skinframe->child_window = this;
		SetParent(mb_skinframe);
		ReSize_Skinframe(mb_skinframe);
		mb_skinframe->ShowWindow(SW_SHOW);
	}
}


BEGIN_MESSAGE_MAP(SkinMessageBox, CDialog)
	//{{AFX_MSG_MAP(SkinMessageBox)
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_BN_CLICKED(IDC_YES, OnYes)
	ON_BN_CLICKED(IDC_NOPE, OnNo)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SkinMessageBox message handlers

HBRUSH SkinMessageBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetTextColor(skin_text_color);
	pDC->SetBkMode(TRANSPARENT);
	return skin_texture_brush_color;
}

void SkinMessageBox::OnClose() 
{
}

void SkinMessageBox::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
}

void SkinMessageBox::Close()
{
	CDialog::EndDialog(return_type);
}

void SkinMessageBox::OnOk() 
{
	return_type = smb_OK;
	Close();
}

void SkinMessageBox::OnCancel() 
{
	return_type = smb_CANCEL;
	Close();
}

void SkinMessageBox::OnYes() 
{
	return_type = smb_YES;
	Close();
}

void SkinMessageBox::OnNo() 
{
	return_type = smb_NO;
	Close();
}



int SkinMsgBox(char *text, char *title, int type)
{
	if(!text)return -1;
	if(type==0)
	{
		type = MB_OK;
	}
	SkinMessageBox smb;
	strcpy(smb.text, text);
	if(title)
	{
		strcpy(smb.title, title);
	}
	else
	{
		strcpy(smb.title, "Gimpel3D");
	}
	smb.type = type;
	int dmres = smb.DoModal();
	if(dmres==IDABORT||dmres==-1)
	{
		//user switched focus away from app
		mainframe->SetForegroundWindow();
		smb.DoModal();
	}
	if(mb_skinframe)
	{
		mb_skinframe->child_window = 0;
		Remove_Skinframe(mb_skinframe);
		mb_skinframe = 0;
	}
	if(smb.return_type==smb_YES)
	{
		return IDYES;
	}
	if(smb.return_type==smb_NO)
	{
		return IDNO;
	}
	if(smb.return_type==smb_OK)
	{
		return IDOK;
	}
	if(smb.return_type==smb_CANCEL)
	{
		return IDCANCEL;
	}
	int res = MessageBox(0, text, title, type);
	return res;
}

int SkinMsgBox(char *text)
{
	return SkinMsgBox(text, "Gimpel3D", 0);
}

int SkinMsgBox(int window, char *text, char *title, int type)
{
	return SkinMsgBox(text, title, type);
}

int SkinMsgBox(char *text, char *title)
{
	if(!text&&title)//had a NULL first then actual text
	{
		SkinMsgBox(title);
	}
	return SkinMsgBox(text, title, 0);
}

void SkinMessageBox::OnPaint() 
{
	CPaintDC dc(this);
	dc.SelectObject(skin_font);
	CRect rect;
	rect.left = 0;
	rect.right = text_area_width;
	int n = text_items.size();
	int xpos = 0;
	int ypos = 0;
	int th = 20;
	int tth = n*th;
	ypos = (text_area_height-tth)/2;
	for(int i = 0;i<n;i++)
	{
		rect.top = ypos;
		rect.bottom = ypos+th;
		dc.DrawText(text_items[i], strlen(text_items[i]), &rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		ypos += th;
	}
}
